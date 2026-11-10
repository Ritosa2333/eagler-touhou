#pragma once
#include "../../cpp/game/SpellCaptureClock.hpp"
struct SpellCaptureFixture {th17::SpellCaptureClock clock;th17::PlayerSpellStatus status;std::array<i32,20> times{};};
API(spell_capture_create) SpellCaptureFixture* spell_capture_create(){return new SpellCaptureFixture;}
API(spell_capture_delete) void spell_capture_delete(SpellCaptureFixture* p){delete p;}
API(spell_capture_present) u32 spell_capture_present(SpellCaptureFixture* p,i32 frames,double seconds,u32 replay){return p->clock.present(p->status,frames,seconds,replay!=0,p->times);}
API(spell_capture_field) void* spell_capture_field(SpellCaptureFixture* p,u32 kind){switch(kind){case 0:return &p->status;case 1:return &p->clock.displayed_frames;case 2:return &p->clock.encoded_time;case 3:return &p->clock.completed;case 4:return &p->clock.began;case 5:return p->times.data();default:return nullptr;}}
