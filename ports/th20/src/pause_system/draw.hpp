#pragma once
#include "pause.hpp"
#include "../replay_system/replay.hpp"
#include "../text_renderer/text.hpp"
namespace th20::source::pause {
void draw_name_editor(PauseInf&,sprite::Vec3);                         //4e5020, original Vec3 by value
void draw_replay_slots(PauseInf&);                                    //4e5240
void draw_replay_entry(int,const sprite::Vec3&,const replay::UserHeader&); //4e5390, original unused this
void draw_replay_name(PauseInf&);                                     //4e54c0
void draw_score_ranking(PauseInf&);                                   //4e5620
void reset_pause_text(text::Renderer&);                               //4e67e0
}
