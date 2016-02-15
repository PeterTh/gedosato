#include "utils/debug_utils.h"

#include "utils/win_utils.h"

#include <TlHelp32.h>

#define THREAD_ACCESS \
    (THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION | THREAD_SET_CONTEXT)

std::vector<uint32_t> enumThreads() {
	std::vector<uint32_t> ret;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if(hSnapshot != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te;
		te.dwSize = sizeof(THREADENTRY32);
		if(Thread32First(hSnapshot, &te)) {
			do {
				if(te.dwSize >= (FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(DWORD))
						&& te.th32OwnerProcessID == GetCurrentProcessId() && te.th32ThreadID != GetCurrentThreadId()) {
					ret.push_back(te.th32ThreadID);
				}

				te.dwSize = sizeof(THREADENTRY32);
			} while(Thread32Next(hSnapshot, &te));
		}
		CloseHandle(hSnapshot);
	}
	return ret;
}

void freezeThreads(const std::vector<uint32_t>& threads) {
	for(const auto& tid : threads) {
		HANDLE hThread = OpenThread(THREAD_ACCESS, FALSE, tid);
		if(hThread != NULL) {
			SuspendThread(hThread);
			CloseHandle(hThread);
		}
	}
}

void unfreezeThreads(const std::vector<uint32_t>& threads) {
	for(const auto& tid : threads) {
		HANDLE hThread = OpenThread(THREAD_ACCESS, FALSE, tid);
		if(hThread != NULL) {
			ResumeThread(hThread);
			CloseHandle(hThread);
		}
	}
}