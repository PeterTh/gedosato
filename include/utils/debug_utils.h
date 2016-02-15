#pragma once

#include <vector>

std::vector<uint32_t> enumThreads();

void freezeThreads(const std::vector<uint32_t>& threads);

void unfreezeThreads(const std::vector<uint32_t>& threads);