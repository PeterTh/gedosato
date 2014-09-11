#pragma once

#include <string>

#include "dxgi.h"

const char* DxgiFormatToString(const DXGI_FORMAT& fmt);
const char* DxgiModeScalingToString(const DXGI_MODE_SCALING& scaling);
const char* DxgiModeScanlineOrderToString(const DXGI_MODE_SCANLINE_ORDER& order);
const char* DxgiSwapEffectToString(const DXGI_SWAP_EFFECT& eff);

std::string DxgiUsageToString(const DXGI_USAGE& usage);
std::string DxgiRationalToString(const DXGI_RATIONAL& rat);
std::string DxgiModeDescToString(const DXGI_MODE_DESC& desc);
std::string DxgiSwapChainDescToString(const DXGI_SWAP_CHAIN_DESC& desc);
