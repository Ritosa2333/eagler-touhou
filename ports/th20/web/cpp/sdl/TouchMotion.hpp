#pragma once
// Port-owned direct-touch link between the SDL input host and the recovered
// player movement. The shared gesture controller owns the reachable target;
// this header exposes the continuous movement vector for the current logical
// frame, already converted to movement units (1/128 game unit) and clamped to
// the player's own speed. Nothing here changes the game's digital input model:
// the analog path only exists while a touch gesture is actually active.
namespace th20::source::input {
bool analog_motion(float& x, float& y);
}
