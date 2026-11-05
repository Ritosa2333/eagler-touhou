#pragma once
#include <string>
#include <array>
#include <cstdint>
namespace th12 {
extern std::string presentationMotionBuffer;
void collectPresentationRevisions();
const std::string& capturePresentationMotion();
const std::array<uint32_t,17>& capturePresentationMotionBinary();
}
