
#pragma once

#include <boost/optional/optional.hpp>
using boost::optional;

#include "main.h"
#include "settings.h"
#include "d3d9.h"
#include "timer.h"

class D3DPerfQuery {
	IDirect3DQuery9* query;

public:
	D3DPerfQuery(D3DQUERYTYPE type, IDirect3DDevice9* device);
	~D3DPerfQuery();

	void begin();
	void end();

	optional<bool> getBoolData();
	optional<UINT64> getUIntData();
};

class D3DPerfMonitor {
	IDirect3DDevice9* device;
	D3DPerfQuery disjointQuery, frequencyQuery, startQuery, endQuery;
	SlidingAverage avg;

	bool inProgress;

public:
	D3DPerfMonitor(IDirect3DDevice9* d3ddev, unsigned interval);

	void start();
	void end();

	double getCurrent();
	double getMax();
};
