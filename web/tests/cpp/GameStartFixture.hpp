#pragma once
#include "../../cpp/game/GameStart.hpp"
struct GameStartFixture:GameStartHost {
    GameRun run;GameRecords records;ItemScoreState score;PlayerLifeSession player;RoaringState roaring;i32 grazes=0;
    i32 step=100,maximum=400;bool has_hud=true;std::vector<i32> events;
    GameStart start{run,records,score,player,roaring,grazes,*this};
    bool previous_bomb_display(i32 count,i32 pieces)override{if(has_hud)events.insert(events.end(),{1,count,pieces});return true;}
    bool create_player()override{events.push_back(2);player.power_step=step;score.max_power=maximum;return true;}
    bool configure_player()override{events.insert(events.end(),{3,player.power});return true;}
};
API(game_start_create) GameStartFixture* game_start_create(){return new GameStartFixture;}
API(game_start_delete) void game_start_delete(GameStartFixture* p){delete p;}
API(game_start_begin) u32 game_start_begin(GameStartFixture* p,u32 fresh){return p->start.begin(fresh!=0);}
API(game_start_error) const char* game_start_error(GameStartFixture* p){return p->start.error.c_str();}
API(game_start_events) const i32* game_start_events(GameStartFixture* p){return p->events.data();}
API(game_start_event_count) u32 game_start_event_count(GameStartFixture* p){return p->events.size();}
API(game_start_field) void* game_start_field(GameStartFixture* p,u32 field){switch(field){case 0:return &p->run.scoring;case 1:return &p->run.stage;case 2:return &p->run.character;case 3:return &p->run.animal;case 4:return &p->run.difficulty;case 5:return &p->run.spell_id;case 6:return &p->run.practice_record;case 7:return &p->run.practice_lives;case 8:return &p->run.demo_slot;case 9:return &p->run.initial_point_value;case 10:return &p->run.ranking_default;case 11:return &p->player;case 12:return &p->score;case 13:return &p->roaring;case 14:return &p->grazes;case 15:return p->records.high_scores.data();case 16:return p->records.practice_scores.data();case 17:return p->records.spell_best.data();case 18:return p->records.starts.data();case 19:return p->records.title.stage_available.data();case 20:return &p->step;case 21:return &p->maximum;case 22:return &p->has_hud;default:return nullptr;}}
