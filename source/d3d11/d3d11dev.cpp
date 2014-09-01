// wrapper for ID3D11Device in d3d11.h
// generated using wrapper_gen11.rb

#include "d3d11/d3d11dev.h"

#include "settings.h"

hkID3D11Device::hkID3D11Device(ID3D11Device **ppID3D11Device) {
	rsMan = new RSManagerDX11(*ppID3D11Device);
	RSManager::setLatest(rsMan);
	pWrapped = *ppID3D11Device;
	*ppID3D11Device = this;
}

HRESULT APIENTRY hkID3D11Device::QueryInterface(REFIID riid, void **ppvObject) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkID3D11Device::AddRef() {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkID3D11Device::Release() {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::Release\n");
	return pWrapped->Release();
}

HRESULT APIENTRY hkID3D11Device::CreateBuffer(const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateBuffer\n");
	return pWrapped->CreateBuffer(pDesc, pInitialData, ppBuffer);
}

HRESULT APIENTRY hkID3D11Device::CreateTexture1D(const D3D11_TEXTURE1D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture1D **ppTexture1D) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateTexture1D\n");
	return pWrapped->CreateTexture1D(pDesc, pInitialData, ppTexture1D);
}

HRESULT APIENTRY hkID3D11Device::CreateTexture2D(const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateTexture2D\n");
	return pWrapped->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
}

HRESULT APIENTRY hkID3D11Device::CreateTexture3D(const D3D11_TEXTURE3D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture3D **ppTexture3D) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateTexture3D\n");
	return pWrapped->CreateTexture3D(pDesc, pInitialData, ppTexture3D);
}

HRESULT APIENTRY hkID3D11Device::CreateShaderResourceView(ID3D11Resource *pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D11ShaderResourceView **ppSRView) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateShaderResourceView\n");
	return pWrapped->CreateShaderResourceView(pResource, pDesc, ppSRView);
}

HRESULT APIENTRY hkID3D11Device::CreateUnorderedAccessView(ID3D11Resource *pResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc, ID3D11UnorderedAccessView **ppUAView) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateUnorderedAccessView\n");
	return pWrapped->CreateUnorderedAccessView(pResource, pDesc, ppUAView);
}

HRESULT APIENTRY hkID3D11Device::CreateRenderTargetView(ID3D11Resource *pResource, const D3D11_RENDER_TARGET_VIEW_DESC *pDesc, ID3D11RenderTargetView **ppRTView) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateRenderTargetView\n");
	return pWrapped->CreateRenderTargetView(pResource, pDesc, ppRTView);
}

HRESULT APIENTRY hkID3D11Device::CreateDepthStencilView(ID3D11Resource *pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D11DepthStencilView **ppDepthStencilView) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateDepthStencilView\n");
	return pWrapped->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);
}

HRESULT APIENTRY hkID3D11Device::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout **ppInputLayout) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateInputLayout\n");
	return pWrapped->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecodeWithInputSignature, BytecodeLength, ppInputLayout);
}

HRESULT APIENTRY hkID3D11Device::CreateVertexShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateVertexShader\n");
	return pWrapped->CreateVertexShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppVertexShader);
}

HRESULT APIENTRY hkID3D11Device::CreateGeometryShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateGeometryShader\n");
	return pWrapped->CreateGeometryShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppGeometryShader);
}

HRESULT APIENTRY hkID3D11Device::CreateGeometryShaderWithStreamOutput(const void *pShaderBytecode, SIZE_T BytecodeLength, const D3D11_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, const UINT *pBufferStrides, UINT NumStrides, UINT RasterizedStream, ID3D11ClassLinkage *pClassLinkage, ID3D11GeometryShader **ppGeometryShader) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateGeometryShaderWithStreamOutput\n");
	return pWrapped->CreateGeometryShaderWithStreamOutput(pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, pBufferStrides, NumStrides, RasterizedStream, pClassLinkage, ppGeometryShader);
}

HRESULT APIENTRY hkID3D11Device::CreatePixelShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreatePixelShader\n");
	return pWrapped->CreatePixelShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppPixelShader);
}

HRESULT APIENTRY hkID3D11Device::CreateHullShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11HullShader **ppHullShader) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateHullShader\n");
	return pWrapped->CreateHullShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppHullShader);
}

HRESULT APIENTRY hkID3D11Device::CreateDomainShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11DomainShader **ppDomainShader) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateDomainShader\n");
	return pWrapped->CreateDomainShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppDomainShader);
}

HRESULT APIENTRY hkID3D11Device::CreateComputeShader(const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11ComputeShader **ppComputeShader) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateComputeShader\n");
	return pWrapped->CreateComputeShader(pShaderBytecode, BytecodeLength, pClassLinkage, ppComputeShader);
}

HRESULT APIENTRY hkID3D11Device::CreateClassLinkage(ID3D11ClassLinkage **ppLinkage) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateClassLinkage\n");
	return pWrapped->CreateClassLinkage(ppLinkage);
}

HRESULT APIENTRY hkID3D11Device::CreateBlendState(const D3D11_BLEND_DESC *pBlendStateDesc, ID3D11BlendState **ppBlendState) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateBlendState\n");
	return pWrapped->CreateBlendState(pBlendStateDesc, ppBlendState);
}

HRESULT APIENTRY hkID3D11Device::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D11DepthStencilState **ppDepthStencilState) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateDepthStencilState\n");
	return pWrapped->CreateDepthStencilState(pDepthStencilDesc, ppDepthStencilState);
}

HRESULT APIENTRY hkID3D11Device::CreateRasterizerState(const D3D11_RASTERIZER_DESC *pRasterizerDesc, ID3D11RasterizerState **ppRasterizerState) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateRasterizerState\n");
	return pWrapped->CreateRasterizerState(pRasterizerDesc, ppRasterizerState);
}

HRESULT APIENTRY hkID3D11Device::CreateSamplerState(const D3D11_SAMPLER_DESC *pSamplerDesc, ID3D11SamplerState **ppSamplerState) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateSamplerState\n");
	return pWrapped->CreateSamplerState(pSamplerDesc, ppSamplerState);
}

HRESULT APIENTRY hkID3D11Device::CreateQuery(const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateQuery\n");
	return pWrapped->CreateQuery(pQueryDesc, ppQuery);
}

HRESULT APIENTRY hkID3D11Device::CreatePredicate(const D3D11_QUERY_DESC *pPredicateDesc, ID3D11Predicate **ppPredicate) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreatePredicate\n");
	return pWrapped->CreatePredicate(pPredicateDesc, ppPredicate);
}

HRESULT APIENTRY hkID3D11Device::CreateCounter(const D3D11_COUNTER_DESC *pCounterDesc, ID3D11Counter **ppCounter) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateCounter\n");
	return pWrapped->CreateCounter(pCounterDesc, ppCounter);
}

HRESULT APIENTRY hkID3D11Device::CreateDeferredContext(UINT ContextFlags, ID3D11DeviceContext **ppDeferredContext) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CreateDeferredContext\n");
	return pWrapped->CreateDeferredContext(ContextFlags, ppDeferredContext);
}

HRESULT APIENTRY hkID3D11Device::OpenSharedResource(HANDLE hResource, REFIID ReturnedInterface, void **ppResource) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::OpenSharedResource\n");
	return pWrapped->OpenSharedResource(hResource, ReturnedInterface, ppResource);
}

HRESULT APIENTRY hkID3D11Device::CheckFormatSupport(DXGI_FORMAT Format, UINT *pFormatSupport) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CheckFormatSupport\n");
	return pWrapped->CheckFormatSupport(Format, pFormatSupport);
}

HRESULT APIENTRY hkID3D11Device::CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CheckMultisampleQualityLevels\n");
	return pWrapped->CheckMultisampleQualityLevels(Format, SampleCount, pNumQualityLevels);
}

void APIENTRY hkID3D11Device::CheckCounterInfo(D3D11_COUNTER_INFO *pCounterInfo) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CheckCounterInfo\n");
	return pWrapped->CheckCounterInfo(pCounterInfo);
}

HRESULT APIENTRY hkID3D11Device::CheckCounter(const D3D11_COUNTER_DESC *pDesc, D3D11_COUNTER_TYPE *pType, UINT *pActiveCounters, LPSTR szName, UINT *pNameLength, LPSTR szUnits, UINT *pUnitsLength, LPSTR szDescription, UINT *pDescriptionLength) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CheckCounter\n");
	return pWrapped->CheckCounter(pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
}

HRESULT APIENTRY hkID3D11Device::CheckFeatureSupport(D3D11_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::CheckFeatureSupport\n");
	return pWrapped->CheckFeatureSupport(Feature, pFeatureSupportData, FeatureSupportDataSize);
}

HRESULT APIENTRY hkID3D11Device::GetPrivateData(REFGUID guid, UINT *pDataSize, void *pData) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::GetPrivateData\n");
	return pWrapped->GetPrivateData(guid, pDataSize, pData);
}

HRESULT APIENTRY hkID3D11Device::SetPrivateData(REFGUID guid, UINT DataSize, const void *pData) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::SetPrivateData\n");
	return pWrapped->SetPrivateData(guid, DataSize, pData);
}

HRESULT APIENTRY hkID3D11Device::SetPrivateDataInterface(REFGUID guid, const IUnknown *pData) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(guid, pData);
}

D3D_FEATURE_LEVEL APIENTRY hkID3D11Device::GetFeatureLevel() {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::GetFeatureLevel\n");
	return pWrapped->GetFeatureLevel();
}

UINT APIENTRY hkID3D11Device::GetCreationFlags() {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::GetCreationFlags\n");
	return pWrapped->GetCreationFlags();
}

HRESULT APIENTRY hkID3D11Device::GetDeviceRemovedReason() {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::GetDeviceRemovedReason\n");
	return pWrapped->GetDeviceRemovedReason();
}

void APIENTRY hkID3D11Device::GetImmediateContext(ID3D11DeviceContext **ppImmediateContext) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::GetImmediateContext\n");
	return pWrapped->GetImmediateContext(ppImmediateContext);
}

HRESULT APIENTRY hkID3D11Device::SetExceptionMode(UINT RaiseFlags) {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::SetExceptionMode\n");
	return pWrapped->SetExceptionMode(RaiseFlags);
}

UINT APIENTRY hkID3D11Device::GetExceptionMode() {
	RSManager::setLatest(rsMan);
	SDLOG(20, "hkID3D11Device::GetExceptionMode\n");
	return pWrapped->GetExceptionMode();
}
