#include "shader_manager.h"

#include "console.h"

bool ShaderManager::isAOInjectionShaderInternal(const string& name) const {
	for(const string& hashStr : Settings::get().getInjectAOHash()) {
		if(name == hashStr) return true;
	}
	return false;
}

bool ShaderManager::isInjectionPSInternal(const string& name) const {
	for(const string& hashStr : Settings::get().getInjectPSHash()) {
		if(name == hashStr) return true;
	}
	return false;
}

bool ShaderManager::isInjectionVSInternal(const string& name) const {
	for(const string& hashStr : Settings::get().getInjectVSHash()) {
		if(name == hashStr) return true;
	}
	return false;
}

void ShaderManager::registerShaderInternal(CONST DWORD* pFunction, void* pShader) {
	SDLOG(3, "ShaderManager: registering shader %p from %p.\n", pShader, pFunction);
	LPD3DXBUFFER disassembly;
	HRESULT hr = D3DXDisassembleShader(pFunction, true, "", &disassembly);
	if(!SUCCEEDED(hr)) {
		SDLOG(0, " -> ERROR: could not disassemble shader\n");
		return;
	}
	UINT32 id = SuperFastHash(static_cast<char*>(disassembly->GetBufferPointer()), disassembly->GetBufferSize());
	SDLOG(8, " -> Shader code (hash %08x):\n%s\n===============\n", id, static_cast<char*>(disassembly->GetBufferPointer()));
	if(Settings::get().getTrackShaders()) shaderPtrIdMap.emplace(pShader, id);

	// given names
	#define SHADER(_name, _id) \
	if(_id == id) { \
		shaderPtrNameMap.emplace(pShader, #_name); \
		shaderPtrIdMap.emplace(pShader, id); \
	}
	#include "shaders.def"
	#undef SHADER

	// name fallback
	string name = format("%08x", id);
	if(shaderPtrNameMap.find(pShader) == shaderPtrNameMap.end()) {
		if(Settings::get().getTrackShaders() || isInjectionPSInternal(name) || isInjectionVSInternal(name) || isAOInjectionShaderInternal(name)) {
			SDLOG(2, "Found relevant shader hash: %s", name);
			shaderPtrNameMap.emplace(pShader, name);
		}
	}

	disassembly->Release();
}

void ShaderManager::registerShader(CONST DWORD* pFunction, IDirect3DVertexShader9* pShader) {
	registerShaderInternal(pFunction, pShader);
}

void ShaderManager::registerShader(CONST DWORD* pFunction, IDirect3DPixelShader9* pShader) {
	registerShaderInternal(pFunction, pShader);
}

bool ShaderManager::isAOInjectionShader(void* pShader) const {
	auto i = shaderPtrNameMap.find(pShader);
	if(i == shaderPtrNameMap.end()) return false;
	return isAOInjectionShaderInternal(i->second);
}

bool ShaderManager::isInjectionShader(void* pShader) const {
	auto i = shaderPtrNameMap.find(pShader);
	if(i == shaderPtrNameMap.end()) return false;
	return isInjectionPSInternal(i->second) || isInjectionVSInternal(i->second);
}

const char* ShaderManager::getName(void* pShader) {
	auto i = shaderPtrNameMap.find(pShader);
	if(i != shaderPtrNameMap.end()) {
		return i->second.c_str();
	}
	return "UNREGISTERED_SHADER";
}
