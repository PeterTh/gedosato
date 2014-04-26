#pragma once

#include <map>
#include <string>
using std::string;

#include <d3dx9.h>

#include "main.h"
#include "settings.h"
#include "hash.h"


class ShaderManager {
	std::map<IDirect3DPixelShader9*, UINT32> shaderPtrIdMap;
	std::map<IDirect3DPixelShader9*, string> shaderPtrNameMap;

public:
	void registerShader(CONST DWORD* pFunction, IDirect3DPixelShader9* pShader) {
		SDLOG(3, "ShaderManager: registering shader %p from %p.\n", pShader, pFunction);
		LPD3DXBUFFER disassembly;
		HRESULT hr = D3DXDisassembleShader(pFunction, true, "", &disassembly);
		if(!SUCCEEDED(hr)) {
			SDLOG(0, " -> ERROR: could not disassemble shader\n");
			return;
		}
		UINT32 id = SuperFastHash(static_cast<char*>(disassembly->GetBufferPointer()), disassembly->GetBufferSize());
		SDLOG(8, " -> Shader code (hash %08x):\n%s\n===============\n", id, disassembly->GetBufferPointer());
		shaderPtrIdMap.emplace(pShader, id);

		// given names
		#define SHADER(_name, _id) \
		if(_id == id) shaderPtrNameMap.emplace(pShader, #_name);
		#include "shaders.def"
		#undef SHADER

		// name fallback
		if(shaderPtrNameMap.find(pShader) == shaderPtrNameMap.end()) {
			shaderPtrNameMap.emplace(pShader, format("%08x", id));
		}
	}
	
	const char* getName(IDirect3DPixelShader9* pShader) {
		auto i = shaderPtrNameMap.find(pShader);
		if(i != shaderPtrNameMap.end()) {
			return i->second.c_str();
		}
		return "UNREGISTERED_SHADER";
	}

	#define SHADER(_name, _id) \
	bool is##_name##Shader(IDirect3DPixelShader9* pShader) { \
		auto i = shaderPtrIdMap.find(pShader); \
		return (i != shaderPtrIdMap.end()) && (i->second == _id); \
	}
	#include "shaders.def"
	#undef SHADER
};
