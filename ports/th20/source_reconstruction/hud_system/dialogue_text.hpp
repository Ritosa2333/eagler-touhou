#pragma once
#include "dialogue.hpp"
namespace th20::source::hud {
void clear_dialogue_text(Dialogue&,bool final_clear); //opcode6/18, eight actual worker functors
void queue_dialogue_text(Dialogue&,bool second); //opcode15/16; decode when worker executes
void queue_dialogue_line(Dialogue&); //opcode17; owns a decoded string snapshot
}
