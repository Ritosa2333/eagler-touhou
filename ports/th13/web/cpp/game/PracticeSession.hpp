// TH13 practice request. Section ids and stage mapping come from the generated
// RUEEE/thprac catalog. Exact ECL routine jumps stay deferred; a selected
// section changes the stage the existing run start already owns.
#pragma once
#include "PracticeSections.gen.hpp"
namespace th13 {
struct PracticeRequest {
    bool enabled = false;
    int section = 0;
    int life = -1;
    int bomb = -1;
    int power = -1;
    int trance = -1;
};
struct PracticeEffect {
    bool applied = false;
    int stage = 1;
    int section = 0;
    int life = -1;
    int bomb = -1;
    int power = -1;
    int trance = -1;
};
inline PracticeEffect resolve_practice(const PracticeRequest& request, int current_stage) noexcept {
    PracticeEffect effect;
    effect.stage = current_stage;
    if (!request.enabled) return effect;
    effect.applied = true;
    effect.life = request.life;
    effect.bomb = request.bomb;
    effect.power = request.power;
    effect.trance = request.trance;
    if (request.section > 0 && request.section <= practice_section_count) {
        effect.section = request.section;
        effect.stage = practice_sections[request.section].stage;
    }
    return effect;
}
}
