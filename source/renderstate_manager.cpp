#include "renderstate_manager_dx9.h"
#include "renderstate_manager_dx11.h"

////////// Static

RSManager* RSManager::latest = NULL;
bool RSManager::forceDSoff = false;

RSManager& RSManager::get() {
	if(latest == NULL) SDLOG(-1, "ERROR: Getting NULL RSManager!!\n");
	return *latest;
}

RSManagerDX9& RSManager::getDX9() {
	auto& ret = get();
	if(typeid(ret) != typeid(RSManagerDX9)) SDLOG(-1, "ERROR: Getting DX9 RSManager from non-DX9 source!!\n");
	return *dynamic_cast<RSManagerDX9*>(&ret);
}
RSManagerDX11& RSManager::getDX11() {
	auto& ret = get();
	if(typeid(ret) != typeid(RSManagerDX11)) SDLOG(-1, "ERROR: Getting DX11 RSManager from non-DX11 source!!\n");
	return *dynamic_cast<RSManagerDX11*>(&ret);
}

void RSManager::setLatest(RSManager *man) {
	latest = man;
	Console::setLatest(&man->console);
}

RenderTargetManager& RSManager::getRTMan() {
	return *latest->rtMan;
}

bool RSManager::currentlyDownsampling() {
	if(forceDSoff) return false;
	return Settings::get().getForceAlwaysDownsamplingRes() || (latest && latest->downsampling);
}

////////// Instance

void RSManager::enableTakeScreenshot(ScreenshotType type) {
	takeScreenshot = type;
	SDLOG(0, "takeScreenshot: %d\n", type);
}
