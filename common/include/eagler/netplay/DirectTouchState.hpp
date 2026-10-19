#pragma once

#include <eagler/netplay/NetplayProtocol.hpp>

namespace Netplay
{
// Authoritative simulation state, not device state. Capture before Apply() in
// the rollback journal. This preserves limited-speed remainder across input
// delay, prediction and correction without feeding old frames into SDL touch.
struct DirectTouchState
{
    float x = 0.0f;
    float y = 0.0f;
    bool active = false;

    void Apply(const FrameInput &input)
    {
        if (input.analogMode != AnalogMode::DirectTouchDelta &&
            input.analogMode != AnalogMode::DirectTouchBegin)
        {
            *this = {};
            return;
        }
        if (!active || input.analogMode == AnalogMode::DirectTouchBegin)
            x = y = 0.0f;
        active = true;
        x += input.x;
        y += input.y;
    }

    void Set(float remainingX, float remainingY) { x = remainingX; y = remainingY; }
    void Consume(float dx, float dy) { x -= dx; y -= dy; }
};
} // namespace Netplay
