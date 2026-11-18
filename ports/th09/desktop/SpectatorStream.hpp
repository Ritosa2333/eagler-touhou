#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace th09::desktop {
bool openSpectatorStream(const std::string& url, const std::string& build, bool viewer);
void closeSpectatorStream();
void pumpSpectatorStream();
bool spectatorStreamFailed();
bool takeSpectatorPacket(std::vector<std::uint8_t>& packet);
void publishTiming(const std::uint32_t* startup, bool automatic);
void publishBegin(std::uint32_t frame, const void* bytes, std::uint32_t size);
void publishEnd(std::uint32_t frame);
void publishRound(std::uint32_t frame, std::uint32_t round);
void publishInput(std::uint32_t frame, unsigned left, unsigned right, int leftMode, float leftX, float leftY,
    int rightMode, float rightX, float rightY, const void* charge, unsigned chargeSize);
}
