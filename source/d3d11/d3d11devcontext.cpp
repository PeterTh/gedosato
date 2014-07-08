// wrapper for ID3D11DeviceContext in d3d11.h
// generated using wrapper_gen11.rb

#include "d3d11/d3d11devcontext.h"

#include "settings.h"

hkID3D11DeviceContext::hkID3D11DeviceContext(ID3D11DeviceContext **ppID3D11DeviceContext) {
	pWrapped = *ppID3D11DeviceContext;
	*ppID3D11DeviceContext = this;
}

HRESULT APIENTRY hkID3D11DeviceContext::QueryInterface(REFIID riid, void **ppvObject) {
	SDLOG(20, "hkID3D11DeviceContext::QueryInterface\n");
	return pWrapped->QueryInterface(riid, ppvObject);
}

ULONG APIENTRY hkID3D11DeviceContext::AddRef() {
	SDLOG(20, "hkID3D11DeviceContext::AddRef\n");
	return pWrapped->AddRef();
}

ULONG APIENTRY hkID3D11DeviceContext::Release() {
	SDLOG(20, "hkID3D11DeviceContext::Release\n");
	return pWrapped->Release();
}

void APIENTRY hkID3D11DeviceContext::GetDevice(ID3D11Device **ppDevice) {
	SDLOG(20, "hkID3D11DeviceContext::GetDevice\n");
	return pWrapped->GetDevice(ppDevice);
}

HRESULT APIENTRY hkID3D11DeviceContext::GetPrivateData(REFGUID guid, UINT *pDataSize, void *pData) {
	SDLOG(20, "hkID3D11DeviceContext::GetPrivateData\n");
	return pWrapped->GetPrivateData(guid, pDataSize, pData);
}

HRESULT APIENTRY hkID3D11DeviceContext::SetPrivateData(REFGUID guid, UINT DataSize, const void *pData) {
	SDLOG(20, "hkID3D11DeviceContext::SetPrivateData\n");
	return pWrapped->SetPrivateData(guid, DataSize, pData);
}

HRESULT APIENTRY hkID3D11DeviceContext::SetPrivateDataInterface(REFGUID guid, const IUnknown *pData) {
	SDLOG(20, "hkID3D11DeviceContext::SetPrivateDataInterface\n");
	return pWrapped->SetPrivateDataInterface(guid, pData);
}

void APIENTRY hkID3D11DeviceContext::VSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::VSSetConstantBuffers\n");
	return pWrapped->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::PSSetShaderResources\n");
	return pWrapped->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::PSSetShader(ID3D11PixelShader *pPixelShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::PSSetShader\n");
	return pWrapped->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::PSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::PSSetSamplers\n");
	return pWrapped->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::VSSetShader(ID3D11VertexShader *pVertexShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::VSSetShader\n");
	return pWrapped->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) {
	SDLOG(20, "hkID3D11DeviceContext::DrawIndexed\n");
	return pWrapped->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void APIENTRY hkID3D11DeviceContext::Draw(UINT VertexCount, UINT StartVertexLocation) {
	SDLOG(20, "hkID3D11DeviceContext::Draw\n");
	return pWrapped->Draw(VertexCount, StartVertexLocation);
}

HRESULT APIENTRY hkID3D11DeviceContext::Map(ID3D11Resource *pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE *pMappedResource) {
	SDLOG(20, "hkID3D11DeviceContext::Map\n");
	return pWrapped->Map(pResource, Subresource, MapType, MapFlags, pMappedResource);
}

void APIENTRY hkID3D11DeviceContext::Unmap(ID3D11Resource *pResource, UINT Subresource) {
	SDLOG(20, "hkID3D11DeviceContext::Unmap\n");
	return pWrapped->Unmap(pResource, Subresource);
}

void APIENTRY hkID3D11DeviceContext::PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::PSSetConstantBuffers\n");
	return pWrapped->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::IASetInputLayout(ID3D11InputLayout *pInputLayout) {
	SDLOG(20, "hkID3D11DeviceContext::IASetInputLayout\n");
	return pWrapped->IASetInputLayout(pInputLayout);
}

void APIENTRY hkID3D11DeviceContext::IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets) {
	SDLOG(20, "hkID3D11DeviceContext::IASetVertexBuffers\n");
	return pWrapped->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void APIENTRY hkID3D11DeviceContext::IASetIndexBuffer(ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset) {
	SDLOG(20, "hkID3D11DeviceContext::IASetIndexBuffer\n");
	return pWrapped->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}

void APIENTRY hkID3D11DeviceContext::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) {
	SDLOG(20, "hkID3D11DeviceContext::DrawIndexedInstanced\n");
	return pWrapped->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void APIENTRY hkID3D11DeviceContext::DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) {
	SDLOG(20, "hkID3D11DeviceContext::DrawInstanced\n");
	return pWrapped->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void APIENTRY hkID3D11DeviceContext::GSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::GSSetConstantBuffers\n");
	return pWrapped->GSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::GSSetShader(ID3D11GeometryShader *pShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::GSSetShader\n");
	return pWrapped->GSSetShader(pShader, ppClassInstances, NumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) {
	SDLOG(20, "hkID3D11DeviceContext::IASetPrimitiveTopology\n");
	return pWrapped->IASetPrimitiveTopology(Topology);
}

void APIENTRY hkID3D11DeviceContext::VSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::VSSetShaderResources\n");
	return pWrapped->VSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::VSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::VSSetSamplers\n");
	return pWrapped->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::Begin(ID3D11Asynchronous *pAsync) {
	SDLOG(20, "hkID3D11DeviceContext::Begin\n");
	return pWrapped->Begin(pAsync);
}

void APIENTRY hkID3D11DeviceContext::End(ID3D11Asynchronous *pAsync) {
	SDLOG(20, "hkID3D11DeviceContext::End\n");
	return pWrapped->End(pAsync);
}

HRESULT APIENTRY hkID3D11DeviceContext::GetData(ID3D11Asynchronous *pAsync, void *pData, UINT DataSize, UINT GetDataFlags) {
	SDLOG(20, "hkID3D11DeviceContext::GetData\n");
	return pWrapped->GetData(pAsync, pData, DataSize, GetDataFlags);
}

void APIENTRY hkID3D11DeviceContext::SetPredication(ID3D11Predicate *pPredicate, BOOL PredicateValue) {
	SDLOG(20, "hkID3D11DeviceContext::SetPredication\n");
	return pWrapped->SetPredication(pPredicate, PredicateValue);
}

void APIENTRY hkID3D11DeviceContext::GSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::GSSetShaderResources\n");
	return pWrapped->GSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::GSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::GSSetSamplers\n");
	return pWrapped->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::OMSetRenderTargets(UINT NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView) {
	SDLOG(20, "hkID3D11DeviceContext::OMSetRenderTargets\n");
	return pWrapped->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}

void APIENTRY hkID3D11DeviceContext::OMSetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts) {
	SDLOG(20, "hkID3D11DeviceContext::OMSetRenderTargetsAndUnorderedAccessViews\n");
	return pWrapped->OMSetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, pDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}

void APIENTRY hkID3D11DeviceContext::OMSetBlendState(ID3D11BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask) {
	SDLOG(20, "hkID3D11DeviceContext::OMSetBlendState\n");
	return pWrapped->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
}

void APIENTRY hkID3D11DeviceContext::OMSetDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef) {
	SDLOG(20, "hkID3D11DeviceContext::OMSetDepthStencilState\n");
	return pWrapped->OMSetDepthStencilState(pDepthStencilState, StencilRef);
}

void APIENTRY hkID3D11DeviceContext::SOSetTargets(UINT NumBuffers, ID3D11Buffer *const *ppSOTargets, const UINT *pOffsets) {
	SDLOG(20, "hkID3D11DeviceContext::SOSetTargets\n");
	return pWrapped->SOSetTargets(NumBuffers, ppSOTargets, pOffsets);
}

void APIENTRY hkID3D11DeviceContext::DrawAuto() {
	SDLOG(20, "hkID3D11DeviceContext::DrawAuto\n");
	return pWrapped->DrawAuto();
}

void APIENTRY hkID3D11DeviceContext::DrawIndexedInstancedIndirect(ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs) {
	SDLOG(20, "hkID3D11DeviceContext::DrawIndexedInstancedIndirect\n");
	return pWrapped->DrawIndexedInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void APIENTRY hkID3D11DeviceContext::DrawInstancedIndirect(ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs) {
	SDLOG(20, "hkID3D11DeviceContext::DrawInstancedIndirect\n");
	return pWrapped->DrawInstancedIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void APIENTRY hkID3D11DeviceContext::Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ) {
	SDLOG(20, "hkID3D11DeviceContext::Dispatch\n");
	return pWrapped->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void APIENTRY hkID3D11DeviceContext::DispatchIndirect(ID3D11Buffer *pBufferForArgs, UINT AlignedByteOffsetForArgs) {
	SDLOG(20, "hkID3D11DeviceContext::DispatchIndirect\n");
	return pWrapped->DispatchIndirect(pBufferForArgs, AlignedByteOffsetForArgs);
}

void APIENTRY hkID3D11DeviceContext::RSSetState(ID3D11RasterizerState *pRasterizerState) {
	SDLOG(20, "hkID3D11DeviceContext::RSSetState\n");
	return pWrapped->RSSetState(pRasterizerState);
}

void APIENTRY hkID3D11DeviceContext::RSSetViewports(UINT NumViewports, const D3D11_VIEWPORT *pViewports) {
	SDLOG(20, "hkID3D11DeviceContext::RSSetViewports\n");
	return pWrapped->RSSetViewports(NumViewports, pViewports);
}

void APIENTRY hkID3D11DeviceContext::RSSetScissorRects(UINT NumRects, const D3D11_RECT *pRects) {
	SDLOG(20, "hkID3D11DeviceContext::RSSetScissorRects\n");
	return pWrapped->RSSetScissorRects(NumRects, pRects);
}

void APIENTRY hkID3D11DeviceContext::CopySubresourceRegion(ID3D11Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D11Resource *pSrcResource, UINT SrcSubresource, const D3D11_BOX *pSrcBox) {
	SDLOG(20, "hkID3D11DeviceContext::CopySubresourceRegion\n");
	return pWrapped->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}

void APIENTRY hkID3D11DeviceContext::CopyResource(ID3D11Resource *pDstResource, ID3D11Resource *pSrcResource) {
	SDLOG(20, "hkID3D11DeviceContext::CopyResource\n");
	return pWrapped->CopyResource(pDstResource, pSrcResource);
}

void APIENTRY hkID3D11DeviceContext::UpdateSubresource(ID3D11Resource *pDstResource, UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch) {
	SDLOG(20, "hkID3D11DeviceContext::UpdateSubresource\n");
	return pWrapped->UpdateSubresource(pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}

void APIENTRY hkID3D11DeviceContext::CopyStructureCount(ID3D11Buffer *pDstBuffer, UINT DstAlignedByteOffset, ID3D11UnorderedAccessView *pSrcView) {
	SDLOG(20, "hkID3D11DeviceContext::CopyStructureCount\n");
	return pWrapped->CopyStructureCount(pDstBuffer, DstAlignedByteOffset, pSrcView);
}

void APIENTRY hkID3D11DeviceContext::ClearRenderTargetView(ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4]) {
	SDLOG(20, "hkID3D11DeviceContext::ClearRenderTargetView\n");
	return pWrapped->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
}

void APIENTRY hkID3D11DeviceContext::ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView *pUnorderedAccessView, const UINT Values[4]) {
	SDLOG(20, "hkID3D11DeviceContext::ClearUnorderedAccessViewUint\n");
	return pWrapped->ClearUnorderedAccessViewUint(pUnorderedAccessView, Values);
}

void APIENTRY hkID3D11DeviceContext::ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView *pUnorderedAccessView, const FLOAT Values[4]) {
	SDLOG(20, "hkID3D11DeviceContext::ClearUnorderedAccessViewFloat\n");
	return pWrapped->ClearUnorderedAccessViewFloat(pUnorderedAccessView, Values);
}

void APIENTRY hkID3D11DeviceContext::ClearDepthStencilView(ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil) {
	SDLOG(20, "hkID3D11DeviceContext::ClearDepthStencilView\n");
	return pWrapped->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
}

void APIENTRY hkID3D11DeviceContext::GenerateMips(ID3D11ShaderResourceView *pShaderResourceView) {
	SDLOG(20, "hkID3D11DeviceContext::GenerateMips\n");
	return pWrapped->GenerateMips(pShaderResourceView);
}

void APIENTRY hkID3D11DeviceContext::SetResourceMinLOD(ID3D11Resource *pResource, FLOAT MinLOD) {
	SDLOG(20, "hkID3D11DeviceContext::SetResourceMinLOD\n");
	return pWrapped->SetResourceMinLOD(pResource, MinLOD);
}

FLOAT APIENTRY hkID3D11DeviceContext::GetResourceMinLOD(ID3D11Resource *pResource) {
	SDLOG(20, "hkID3D11DeviceContext::GetResourceMinLOD\n");
	return pWrapped->GetResourceMinLOD(pResource);
}

void APIENTRY hkID3D11DeviceContext::ResolveSubresource(ID3D11Resource *pDstResource, UINT DstSubresource, ID3D11Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format) {
	SDLOG(20, "hkID3D11DeviceContext::ResolveSubresource\n");
	return pWrapped->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}

void APIENTRY hkID3D11DeviceContext::ExecuteCommandList(ID3D11CommandList *pCommandList, BOOL RestoreContextState) {
	SDLOG(20, "hkID3D11DeviceContext::ExecuteCommandList\n");
	return pWrapped->ExecuteCommandList(pCommandList, RestoreContextState);
}

void APIENTRY hkID3D11DeviceContext::HSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::HSSetShaderResources\n");
	return pWrapped->HSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::HSSetShader(ID3D11HullShader *pHullShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::HSSetShader\n");
	return pWrapped->HSSetShader(pHullShader, ppClassInstances, NumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::HSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::HSSetSamplers\n");
	return pWrapped->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::HSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::HSSetConstantBuffers\n");
	return pWrapped->HSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::DSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::DSSetShaderResources\n");
	return pWrapped->DSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::DSSetShader(ID3D11DomainShader *pDomainShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::DSSetShader\n");
	return pWrapped->DSSetShader(pDomainShader, ppClassInstances, NumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::DSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::DSSetSamplers\n");
	return pWrapped->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::DSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::DSSetConstantBuffers\n");
	return pWrapped->DSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::CSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::CSSetShaderResources\n");
	return pWrapped->CSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::CSSetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView *const *ppUnorderedAccessViews, const UINT *pUAVInitialCounts) {
	SDLOG(20, "hkID3D11DeviceContext::CSSetUnorderedAccessViews\n");
	return pWrapped->CSSetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts);
}

void APIENTRY hkID3D11DeviceContext::CSSetShader(ID3D11ComputeShader *pComputeShader, ID3D11ClassInstance *const *ppClassInstances, UINT NumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::CSSetShader\n");
	return pWrapped->CSSetShader(pComputeShader, ppClassInstances, NumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::CSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::CSSetSamplers\n");
	return pWrapped->CSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::CSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::CSSetConstantBuffers\n");
	return pWrapped->CSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::VSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::VSGetConstantBuffers\n");
	return pWrapped->VSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::PSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::PSGetShaderResources\n");
	return pWrapped->PSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::PSGetShader(ID3D11PixelShader **ppPixelShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::PSGetShader\n");
	return pWrapped->PSGetShader(ppPixelShader, ppClassInstances, pNumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::PSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::PSGetSamplers\n");
	return pWrapped->PSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::VSGetShader(ID3D11VertexShader **ppVertexShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::VSGetShader\n");
	return pWrapped->VSGetShader(ppVertexShader, ppClassInstances, pNumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::PSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::PSGetConstantBuffers\n");
	return pWrapped->PSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::IAGetInputLayout(ID3D11InputLayout **ppInputLayout) {
	SDLOG(20, "hkID3D11DeviceContext::IAGetInputLayout\n");
	return pWrapped->IAGetInputLayout(ppInputLayout);
}

void APIENTRY hkID3D11DeviceContext::IAGetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppVertexBuffers, UINT *pStrides, UINT *pOffsets) {
	SDLOG(20, "hkID3D11DeviceContext::IAGetVertexBuffers\n");
	return pWrapped->IAGetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void APIENTRY hkID3D11DeviceContext::IAGetIndexBuffer(ID3D11Buffer **pIndexBuffer, DXGI_FORMAT *Format, UINT *Offset) {
	SDLOG(20, "hkID3D11DeviceContext::IAGetIndexBuffer\n");
	return pWrapped->IAGetIndexBuffer(pIndexBuffer, Format, Offset);
}

void APIENTRY hkID3D11DeviceContext::GSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::GSGetConstantBuffers\n");
	return pWrapped->GSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::GSGetShader(ID3D11GeometryShader **ppGeometryShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::GSGetShader\n");
	return pWrapped->GSGetShader(ppGeometryShader, ppClassInstances, pNumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::IAGetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY *pTopology) {
	SDLOG(20, "hkID3D11DeviceContext::IAGetPrimitiveTopology\n");
	return pWrapped->IAGetPrimitiveTopology(pTopology);
}

void APIENTRY hkID3D11DeviceContext::VSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::VSGetShaderResources\n");
	return pWrapped->VSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::VSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::VSGetSamplers\n");
	return pWrapped->VSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::GetPredication(ID3D11Predicate **ppPredicate, BOOL *pPredicateValue) {
	SDLOG(20, "hkID3D11DeviceContext::GetPredication\n");
	return pWrapped->GetPredication(ppPredicate, pPredicateValue);
}

void APIENTRY hkID3D11DeviceContext::GSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::GSGetShaderResources\n");
	return pWrapped->GSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::GSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::GSGetSamplers\n");
	return pWrapped->GSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::OMGetRenderTargets(UINT NumViews, ID3D11RenderTargetView **ppRenderTargetViews, ID3D11DepthStencilView **ppDepthStencilView) {
	SDLOG(20, "hkID3D11DeviceContext::OMGetRenderTargets\n");
	return pWrapped->OMGetRenderTargets(NumViews, ppRenderTargetViews, ppDepthStencilView);
}

void APIENTRY hkID3D11DeviceContext::OMGetRenderTargetsAndUnorderedAccessViews(UINT NumRTVs, ID3D11RenderTargetView **ppRenderTargetViews, ID3D11DepthStencilView **ppDepthStencilView, UINT UAVStartSlot, UINT NumUAVs, ID3D11UnorderedAccessView **ppUnorderedAccessViews) {
	SDLOG(20, "hkID3D11DeviceContext::OMGetRenderTargetsAndUnorderedAccessViews\n");
	return pWrapped->OMGetRenderTargetsAndUnorderedAccessViews(NumRTVs, ppRenderTargetViews, ppDepthStencilView, UAVStartSlot, NumUAVs, ppUnorderedAccessViews);
}

void APIENTRY hkID3D11DeviceContext::OMGetBlendState(ID3D11BlendState **ppBlendState, FLOAT BlendFactor[4], UINT *pSampleMask) {
	SDLOG(20, "hkID3D11DeviceContext::OMGetBlendState\n");
	return pWrapped->OMGetBlendState(ppBlendState, BlendFactor, pSampleMask);
}

void APIENTRY hkID3D11DeviceContext::OMGetDepthStencilState(ID3D11DepthStencilState **ppDepthStencilState, UINT *pStencilRef) {
	SDLOG(20, "hkID3D11DeviceContext::OMGetDepthStencilState\n");
	return pWrapped->OMGetDepthStencilState(ppDepthStencilState, pStencilRef);
}

void APIENTRY hkID3D11DeviceContext::SOGetTargets(UINT NumBuffers, ID3D11Buffer **ppSOTargets) {
	SDLOG(20, "hkID3D11DeviceContext::SOGetTargets\n");
	return pWrapped->SOGetTargets(NumBuffers, ppSOTargets);
}

void APIENTRY hkID3D11DeviceContext::RSGetState(ID3D11RasterizerState **ppRasterizerState) {
	SDLOG(20, "hkID3D11DeviceContext::RSGetState\n");
	return pWrapped->RSGetState(ppRasterizerState);
}

void APIENTRY hkID3D11DeviceContext::RSGetViewports(UINT *pNumViewports, D3D11_VIEWPORT *pViewports) {
	SDLOG(20, "hkID3D11DeviceContext::RSGetViewports\n");
	return pWrapped->RSGetViewports(pNumViewports, pViewports);
}

void APIENTRY hkID3D11DeviceContext::RSGetScissorRects(UINT *pNumRects, D3D11_RECT *pRects) {
	SDLOG(20, "hkID3D11DeviceContext::RSGetScissorRects\n");
	return pWrapped->RSGetScissorRects(pNumRects, pRects);
}

void APIENTRY hkID3D11DeviceContext::HSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::HSGetShaderResources\n");
	return pWrapped->HSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::HSGetShader(ID3D11HullShader **ppHullShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::HSGetShader\n");
	return pWrapped->HSGetShader(ppHullShader, ppClassInstances, pNumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::HSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::HSGetSamplers\n");
	return pWrapped->HSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::HSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::HSGetConstantBuffers\n");
	return pWrapped->HSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::DSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::DSGetShaderResources\n");
	return pWrapped->DSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::DSGetShader(ID3D11DomainShader **ppDomainShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::DSGetShader\n");
	return pWrapped->DSGetShader(ppDomainShader, ppClassInstances, pNumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::DSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::DSGetSamplers\n");
	return pWrapped->DSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::DSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::DSGetConstantBuffers\n");
	return pWrapped->DSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::CSGetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView **ppShaderResourceViews) {
	SDLOG(20, "hkID3D11DeviceContext::CSGetShaderResources\n");
	return pWrapped->CSGetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void APIENTRY hkID3D11DeviceContext::CSGetUnorderedAccessViews(UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView **ppUnorderedAccessViews) {
	SDLOG(20, "hkID3D11DeviceContext::CSGetUnorderedAccessViews\n");
	return pWrapped->CSGetUnorderedAccessViews(StartSlot, NumUAVs, ppUnorderedAccessViews);
}

void APIENTRY hkID3D11DeviceContext::CSGetShader(ID3D11ComputeShader **ppComputeShader, ID3D11ClassInstance **ppClassInstances, UINT *pNumClassInstances) {
	SDLOG(20, "hkID3D11DeviceContext::CSGetShader\n");
	return pWrapped->CSGetShader(ppComputeShader, ppClassInstances, pNumClassInstances);
}

void APIENTRY hkID3D11DeviceContext::CSGetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState **ppSamplers) {
	SDLOG(20, "hkID3D11DeviceContext::CSGetSamplers\n");
	return pWrapped->CSGetSamplers(StartSlot, NumSamplers, ppSamplers);
}

void APIENTRY hkID3D11DeviceContext::CSGetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer **ppConstantBuffers) {
	SDLOG(20, "hkID3D11DeviceContext::CSGetConstantBuffers\n");
	return pWrapped->CSGetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

void APIENTRY hkID3D11DeviceContext::ClearState() {
	SDLOG(20, "hkID3D11DeviceContext::ClearState\n");
	return pWrapped->ClearState();
}

void APIENTRY hkID3D11DeviceContext::Flush() {
	SDLOG(20, "hkID3D11DeviceContext::Flush\n");
	return pWrapped->Flush();
}

D3D11_DEVICE_CONTEXT_TYPE APIENTRY hkID3D11DeviceContext::GetType() {
	SDLOG(20, "hkID3D11DeviceContext::GetType\n");
	return pWrapped->GetType();
}

UINT APIENTRY hkID3D11DeviceContext::GetContextFlags() {
	SDLOG(20, "hkID3D11DeviceContext::GetContextFlags\n");
	return pWrapped->GetContextFlags();
}

HRESULT APIENTRY hkID3D11DeviceContext::FinishCommandList(BOOL RestoreDeferredContextState, ID3D11CommandList **ppCommandList) {
	SDLOG(20, "hkID3D11DeviceContext::FinishCommandList\n");
	return pWrapped->FinishCommandList(RestoreDeferredContextState, ppCommandList);
}
