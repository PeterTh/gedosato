#pragma once

#include <map>
#include <string>
using std::string;

#include <d3dx9.h>

#include "main.h"
#include "settings.h"
#include "hash.h"
#include "utils/string_utils.h"

class ShaderManager {
	std::map<void*, UINT32> shaderPtrIdMap;
	std::map<void*, string> shaderPtrNameMap;

	void registerShaderInternal(CONST DWORD* pFunction, void* pShader);

	bool isAOInjectionShaderInternal(const string& name) const;
	bool isInjectionPSInternal(const string& name) const;
	bool isInjectionVSInternal(const string& name) const;

public:
	void registerShader(CONST DWORD* pFunction, IDirect3DPixelShader9* pShader);
	void registerShader(CONST DWORD* pFunction, IDirect3DVertexShader9* pShader);

	bool isAOInjectionShader(void* pShader) const;
	bool isInjectionShader(void* pShader) const;
	
	const char* getName(void* pShader);

	#define SHADER(_name, _id) \
	bool is##_name##Shader(void* pShader) const { \
		auto i = shaderPtrIdMap.find(pShader); \
		return (i != shaderPtrIdMap.end()) && (i->second == _id); \
	}
	#include "shaders.def"
	#undef SHADER
};
