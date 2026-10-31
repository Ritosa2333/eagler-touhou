#pragma once
#include "../../cpp/game/Dialogue.hpp"
struct DialogueFixture final:DialogueServices {
    AnmManager& animations;MessageResource& file;std::unique_ptr<Dialogue> dialogue;std::vector<u32> events;
    DialogueFixture(AnmManagerFixture& a,MessageResource& m,i32 stage,i32 character):animations(a.manager),file(m){DialogueResources r;r.enemy={{7,8,12,13,14,15}};r.character=character;r.player=character?15+character:10;dialogue=std::make_unique<Dialogue>(animations,*this,*StageConfiguration::get(stage),r);}
    bool clear_dialogue_field()override{events.push_back(1);return true;}
    bool dialogue_text(AnmVm& vm,u32 color,u32 shadow,i32 style,i32 x,i32 y,const std::string& value)override{events.insert(events.end(),{2,animations.registry.handle(vm),color,shadow,u32(style),u32(x),u32(y),u32(value.size())});for(auto ch:value)events.push_back(u8(ch));return true;}
    bool dialogue_sound(i32 id)override{events.insert(events.end(),{3,u32(id)});return true;}
    bool dialogue_boss_music()override{events.push_back(4);return true;}
    bool dialogue_music_fade(i32 seconds)override{events.insert(events.end(),{5,u32(seconds)});return true;}
    bool dialogue_next_stage()override{events.push_back(6);return true;}
    bool dialogue_boss_name()override{events.push_back(7);return true;}
    bool dialogue_notice(u32 handle)override{events.insert(events.end(),{8,handle});return true;}
};
API(dialogue_create) DialogueFixture* dialogue_create(AnmManagerFixture* a,MessageResource* m,i32 stage,i32 character){return new DialogueFixture(*a,*m,stage,character);}
API(dialogue_delete) void dialogue_delete(DialogueFixture* p){delete p;}
API(dialogue_initialize) u32 dialogue_initialize(DialogueFixture* p,i32 entry){return p->dialogue->initialize(p->file,entry);}
API(dialogue_update) i32 dialogue_update(DialogueFixture* p,float rate,u32 held,u32 pressed,i32 shot,i32 skip){return p->dialogue->update({rate,held,pressed,shot,skip});}
API(dialogue_error) const char* dialogue_error(DialogueFixture* p){return p->dialogue->error.c_str();}
API(dialogue_events) const u32* dialogue_events(DialogueFixture* p){return p->events.data();}
API(dialogue_event_count) u32 dialogue_event_count(DialogueFixture* p){return p->events.size();}
API(dialogue_clear_events) void dialogue_clear_events(DialogueFixture* p){p->events.clear();}
API(dialogue_offset) u32 dialogue_offset(DialogueFixture* p){return p->dialogue->instruction_offset();}
API(dialogue_field) void* dialogue_field(DialogueFixture* p,u32 field){auto& d=*p->dialogue;switch(field){case 0:return &d.id;case 1:return &d.age;case 2:return &d.script_age;case 3:return &d.wait;case 4:return &d.player_portrait;case 5:return d.boss_portraits.data();case 6:return &d.name;case 7:return d.text_handles.data();case 8:return &d.boss_label;case 9:return &d.bubble;case 10:return &d.complete_pulse;case 11:return &d.flags;case 12:return &d.line;case 13:return &d.cleared;case 14:return &d.speaker;case 15:return d.colors.data();case 16:return &d.position;case 17:return &d.width;case 18:return &d.bubble_style;case 19:return &d.portrait_mode;default:return nullptr;}}
