#include <windows.h>

#include "d3d9.h"
#include "main.h"
#include "Settings.h"

IDirect3D9 *APIENTRY Direct3DCreate9(UINT SDKVersion) {
	OutputDebugString("BLA\n");
	SDLOG(0, "=== Direct3DCreate9 V:%d\n", SDKVersion);
	IDirect3D9 *d3dint = NULL;
	d3dint = TrueDirect3DCreate9(SDKVersion);

	if(d3dint != NULL) {
		hkIDirect3D9 *ret = new hkIDirect3D9(&d3dint);
		SDLOG(0, "=== Direct3DCreate9 hooking %p\n", ret);
	}
	SDLOG(0, "=== Direct3DCreate9 returning\n");
	return d3dint;
}

int WINAPI D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR wszName) {
	return TrueD3DPERF_BeginEvent(col, wszName);
}

int WINAPI D3DPERF_EndEvent(void) {
	return TrueD3DPERF_EndEvent();
}

void WINAPI D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR wszName) {
	TrueD3DPERF_SetMarker(col, wszName);
}

void WINAPI D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR wszName) {
	TrueD3DPERF_SetRegion(col, wszName);
}

BOOL WINAPI D3DPERF_QueryRepeatFrame(void) {
	return TrueD3DPERF_QueryRepeatFrame();
}

//void WINAPI D3DPERF_SetOptions(DWORD dwOptions) {
//	TrueD3DPERF_SetOptions(dwOptions);
//}

DWORD WINAPI D3DPERF_GetStatus(void) {
	return TrueD3DPERF_GetStatus();
}
