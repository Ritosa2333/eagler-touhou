#pragma once
#include "../../cpp/game/SpellAnmHost.hpp"
struct SpellFixture final:SpellCardServices {
    AnmManager& animations;PlayerSpellStatus status;SpellRecords records;i32 score=0;u32 roaring_flags=0;
    EnemyWorldState world;EnemyState boss;SpellAnmHost host;SpellCard spell;SpellStartContext start_context;
    std::vector<u32> audio,results,trophies;std::string title;u32 background_flags=1;i32 hud_label=0;std::array<u8,156> record_view{};
    SpellFixture(AnmManagerFixture& a):animations(a.manager),host(animations,world,*this),spell(status,score,records,roaring_flags,host){boss.id=1;world.enemies.push_back(&boss);world.boss_ids[0]=1;start_context.visuals={2,6,8,{{20,0,false,-1,-1},{20,0,true,-1,-1},{20,0,false,8,20}}};}
    bool spell_background_visible(bool value)override{background_flags=(background_flags&~1u)|(value?1:0);return true;}
    bool spell_prepare_hud(bool start)override{hud_label=start?2:3;return true;}
    bool spell_title(AnmVm&,const std::string& value)override{title=value;return true;}
    bool spell_result(i32 value,bool failed)override{results.insert(results.end(),{u32(value),u32(failed)});return true;}
    bool spell_sound(i32 id,bool queued)override{audio.insert(audio.end(),{u32(id),u32(queued)});return true;}
    bool spell_trophy(i32 id)override{trophies.push_back(id);return true;}
};
API(spell_create) SpellFixture* spell_create(AnmManagerFixture* a){return new SpellFixture(*a);}
API(spell_delete) void spell_delete(SpellFixture* p){delete p;}
API(spell_begin) u32 spell_begin(SpellFixture* p,i32 id,i32 duration,i32 mode,const char* name,i32 stage,i32 difficulty,i32 character,i32 animal,u32 replay,u32 practice){p->start_context.stage=stage;p->start_context.difficulty=difficulty;p->start_context.character=character;p->start_context.animal=animal;p->start_context.replay=replay!=0;p->start_context.practice=practice!=0;return p->spell.begin({id,duration,mode,name},p->start_context);}
API(spell_update) u32 spell_update(SpellFixture* p,float rate,float player_y,i32 bomb){return p->spell.update({rate,player_y,bomb});}
API(spell_finish) u32 spell_finish(SpellFixture* p){return p->spell.finish();}
API(spell_error) const char* spell_error(SpellFixture* p){return p->spell.error.c_str();}
API(spell_title) const char* spell_title(SpellFixture* p){return p->title.c_str();}
API(spell_field) void* spell_field(SpellFixture* p,u32 kind){switch(kind){case 0:return &p->status;case 1:return &p->spell.age;case 2:return &p->spell.maximum_bonus;case 3:return &p->spell.duration;case 4:return &p->spell.active_frames;case 5:return p->spell.handles.data();case 6:return &p->spell.anchor;case 7:return &p->score;case 8:return &p->roaring_flags;case 9:return &p->boss.motion.position;case 10:return &p->background_flags;case 11:return &p->hud_label;default:return nullptr;}}
API(spell_record) const void* spell_record(SpellFixture* p,u32 loadout,u32 id){if(loadout>9||id>=101)return nullptr;const auto& value=loadout==9?p->records.aggregate[id]:p->records.loadouts[loadout][id];p->record_view.fill(0);std::memcpy(p->record_view.data(),value.name.data(),std::min(value.name.size(),std::size_t(127)));std::memcpy(p->record_view.data()+128,value.captures.data(),8);std::memcpy(p->record_view.data()+136,value.attempts.data(),8);return p->record_view.data();}
API(spell_record_set) void spell_record_set(SpellFixture* p,u32 loadout,u32 id,u32 field,i32 value){auto& record=loadout==9?p->records.aggregate[id]:p->records.loadouts[loadout][id];if(field<2)record.captures[field]=value;else record.attempts[field-2]=value;}
API(spell_events) const u32* spell_events(SpellFixture* p,u32 kind){return kind==0?p->audio.data():kind==1?p->results.data():p->trophies.data();}
API(spell_event_count) u32 spell_event_count(SpellFixture* p,u32 kind){return kind==0?p->audio.size():kind==1?p->results.size():p->trophies.size();}
