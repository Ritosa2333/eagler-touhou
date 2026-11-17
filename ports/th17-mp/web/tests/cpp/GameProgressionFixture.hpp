#pragma once
#include "../../cpp/game/GameProgression.hpp"
struct GameProgressionFixture:GameProgressionHost {
    HudFixture& hud;GameRun run;GameRecords records;GameProgression flow;std::vector<i32> events;
    explicit GameProgressionFixture(HudFixture& h):hud(h),flow(run,records,h.score,h.session,h.hud,*this){}
    bool event(i32 id,i32 value=0){events.push_back(id);events.push_back(value);return true;}
    bool finish_player_options()override{return event(1);}bool stop_bomb()override{return event(2);}
    bool record_play_time()override{return event(3);}bool trophy(i32 id)override{return event(4,id);}
    bool finish_practice()override{return event(5);}bool return_to_title()override{return event(6);}
};
API(progression_create) GameProgressionFixture* progression_create(HudFixture* h){return new GameProgressionFixture(*h);}
API(progression_delete) void progression_delete(GameProgressionFixture* p){delete p;}
API(progression_next) u32 progression_next(GameProgressionFixture* p){return p->flow.next_stage();}
API(progression_error) const char* progression_error(GameProgressionFixture* p){return p->flow.error.c_str();}
API(progression_field) void* progression_field(GameProgressionFixture* p,u32 kind){switch(kind){case 0:return &p->run.scoring;case 1:return &p->run.stage;case 2:return &p->run.character;case 3:return &p->run.animal;case 4:return &p->run.difficulty;case 5:return &p->run.spell_id;case 6:return &p->run.scene_flags;case 7:return &p->run.application_flags;case 8:return &p->run.screen;case 9:return &p->run.ending_age;case 10:return &p->run.demo_slot;case 11:return &p->run.practice_record;case 12:return p->records.clears.data();case 13:return p->records.title.cleared.data();case 14:return p->records.stage_cleared.data();case 15:return p->records.spell_best.data();default:return nullptr;}}
API(progression_events) const i32* progression_events(GameProgressionFixture* p){return p->events.data();}
API(progression_event_size) u32 progression_event_size(GameProgressionFixture* p){return p->events.size();}
API(progression_notice_age) Timer* progression_notice_age(GameProgressionFixture* p){return &p->hud.hud.notice_age;}
