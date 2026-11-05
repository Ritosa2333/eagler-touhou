#pragma once
#include "../../cpp/game/Dialogue.hpp"
namespace th11::test {
struct DialogueFixture:DialogueEffects {
    AnmManager manager;AnmResource resources[5];Dialogue dialogue;
    std::vector<u8> message;std::vector<u32> events;
    DialogueFixture():dialogue(manager,resources[0],resources[1],resources[2],resources[3],resources[4],*this){}
    bool dialogue_text(const DialogueText& r)override{events.insert(events.end(),{0,r.animation,r.color,u32(r.offset),u32(r.font),u32(r.style),u32(r.bytes.size())});for(u8 c:r.bytes)events.push_back(c);return true;}
    bool dialogue_sound(i32 id)override{events.insert(events.end(),{1,u32(id)});return true;}
    bool dialogue_music()override{events.push_back(2);return true;}
    bool dialogue_fade(float seconds)override{u32 bits;std::memcpy(&bits,&seconds,4);events.insert(events.end(),{3,bits});return true;}
};
}
