#include "d3dperf.h"

D3DPerfQuery::D3DPerfQuery(D3DQUERYTYPE type, IDirect3DDevice9* device) : query(NULL) {
	device->CreateQuery(type, &query);
}
D3DPerfQuery::~D3DPerfQuery() {
	SAFERELEASE(query);
}

void D3DPerfQuery::begin() {
	query->Issue(D3DISSUE_BEGIN);
}
void D3DPerfQuery::end() {
	query->Issue(D3DISSUE_END);
}

optional<bool> D3DPerfQuery::getBoolData() {
	BOOL val;
	if(query->GetData(&val, sizeof(BOOL), 0) == S_OK) {
		return optional<bool>(val ? true : false);
	}
	return optional<bool>();
}

optional<UINT64> D3DPerfQuery::getUIntData() {
	UINT64 val;
	if(query->GetData(&val, sizeof(UINT64), 0) == S_OK) {
		return optional<UINT64>(val);
	}
	return optional<UINT64>();
}


D3DPerfMonitor::D3DPerfMonitor(IDirect3DDevice9* d3ddev, unsigned interval) 
	: device(d3ddev),
	  disjointQuery(D3DQUERYTYPE_TIMESTAMPDISJOINT, d3ddev), frequencyQuery(D3DQUERYTYPE_TIMESTAMPFREQ, d3ddev),
	  startQuery(D3DQUERYTYPE_TIMESTAMP, d3ddev), endQuery(D3DQUERYTYPE_TIMESTAMP, d3ddev), 
	  avg(interval), inProgress(false) {
	HRESULT tsHr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMP, NULL);
	HRESULT tsdHr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMPDISJOINT, NULL);
	HRESULT tsfHr = device->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, NULL);
	if(tsHr == D3DERR_NOTAVAILABLE || tsdHr == D3DERR_NOTAVAILABLE || tsfHr == D3DERR_NOTAVAILABLE) {
		SDLOG(-1, "ERROR: D3D timestamp queries not available\n");
	}
}

void D3DPerfMonitor::start() {
	if(!inProgress) {
		disjointQuery.begin();
		frequencyQuery.end();
		startQuery.end();
	}
}

void D3DPerfMonitor::end() {
	if(!inProgress) {
		endQuery.end();
		disjointQuery.end();
		inProgress = true;
	}
}

double D3DPerfMonitor::getCurrent() {
	if(inProgress) {
		auto disjoint = disjointQuery.getBoolData();
		auto frequency = frequencyQuery.getUIntData();
		auto startTime = startQuery.getUIntData();
		auto endTime = endQuery.getUIntData();
		if(disjoint.is_initialized() && !disjoint.get()) {
			if(frequency.is_initialized() && startTime.is_initialized() && endTime.is_initialized()) {
				avg.add(((endTime.get() - startTime.get())*1000.0) / frequency.get());
				inProgress = false;
			}
		}
	}
	return avg.get();
}

double D3DPerfMonitor::getMax() {
	return avg.maximum();
}
