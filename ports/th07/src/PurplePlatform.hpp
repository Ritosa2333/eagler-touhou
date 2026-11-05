#pragma once
#include "PurpleSharedLabels.hpp"
#include "ThpracImGui.hpp"
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeSpeed.hpp>
#include <SDL3/SDL.h>
#include <functional>
#include <random>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cfloat>

namespace PracticeRuntime {
inline const char* label(const char* const* values) {
    return values[static_cast<int>(ThpracImGui::GetLocale())];
}
inline eagler::thprac::PracticeKeyMonitor key_monitor;
#include <eagler/thprac/PracticeKeyHud.inc>
struct PracticeCounter { int64_t QuadPart = 0; };
inline void practice_counter_frequency(PracticeCounter* counter) { counter->QuadPart = 1000000000; }
inline void practice_counter_now(PracticeCounter* counter) { counter->QuadPart = SDL_GetTicksNS(); }
inline std::function<unsigned()> practice_random_generator(unsigned lo, unsigned hi) {
    return std::bind(std::uniform_int_distribution<unsigned>(lo, hi), std::mt19937(unsigned(SDL_GetTicksNS())));
}
#include <eagler/thprac/PracticeReaction.inc>
#include <eagler/thprac/PracticeSpeed.inc>
}
