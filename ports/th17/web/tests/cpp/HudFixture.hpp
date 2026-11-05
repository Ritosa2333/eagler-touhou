#pragma once
#include "../../cpp/game/Hud.hpp"
struct HudFixture:HudDrawServices {
    AnmManager& animations;EnemyWorldState world;PlayerLifeSession session;PlayerMotion player;PlayerSpellStatus spell;
    std::array<EnemyState,3> bosses;Hud hud;HudStartContext start;std::vector<i32> sounds;
    ItemScoreState score;HudScoreState progress;HudDrawContext draw_context{score,progress};std::vector<HudTextDraw> text;
    bool hud_text(const HudTextDraw& value)override{text.push_back(value);return true;}
    HudFixture(AnmManagerFixture& a):animations(a.manager),hud(animations,world,session,player,spell){for(u32 i=0;i<bosses.size();i++){bosses[i].id=i+1;world.enemies.push_back(&bosses[i]);}hud.sound=[this](i32 id){sounds.push_back(id);return true;};}
};
API(hud_create) HudFixture* hud_create(AnmManagerFixture* a){return new HudFixture(*a);}
API(hud_delete) void hud_delete(HudFixture* p){delete p;}
API(hud_initialize) u32 hud_initialize(HudFixture* p,u32 mode,i32 screen,i32 stage,i32 character,i32 animal,i32 difficulty,i32 continues,u32 game,u32 practice){p->start={mode,screen,stage,character,animal,difficulty,continues,game!=0,practice!=0};return p->hud.initialize(p->start);}
API(hud_stock) u32 hud_stock(HudFixture* p,u32 bomb,i32 count,i32 pieces){return bomb?p->hud.bombs(count,pieces):p->hud.lives(count,pieces);}
API(hud_prepare_spell) u32 hud_prepare_spell(HudFixture* p,u32 start,float rate){p->animations.rate=rate;return p->hud.prepare_spell(start!=0);}
API(hud_update) u32 hud_update(HudFixture* p,float rate,u32 flags,u32 dialogue){p->hud.game_flags=flags;p->hud.dialogue=dialogue!=0;return p->hud.update(rate);}
API(hud_rate) void hud_rate(HudFixture* p,float rate){p->animations.rate=rate;}
API(hud_segments) u32 hud_segments(HudFixture* p,i32 count){return p->hud.segments(count);}
API(hud_result) u32 hud_result(HudFixture* p,i32 amount,i32 kind){return p->hud.result(amount,kind);}
API(hud_segment) u32 hud_segment(HudFixture* p,i32 boss,i32 index,float value,u32 color){return p->hud.segment(boss,index,value,color);}
API(hud_boss) void hud_boss(HudFixture* p,u32 index,u32 active,i32 life,i32 initial,i32 phase,u32 flags,i32 collision_delay,float x,float y,float z){if(index>=3)return;auto& e=p->bosses[index];e.life=life;e.initial_life=initial;e.phase_life=phase;e.flags=flags;e.collision_timer.set(collision_delay);e.motion.position={x,y,z};p->world.boss_ids[index]=active?index+1:0;}
API(hud_error) const char* hud_error(HudFixture* p){return p->hud.error.c_str();}
API(hud_sounds) const i32* hud_sounds(HudFixture* p){return p->sounds.data();}
API(hud_sound_count) u32 hud_sound_count(HudFixture* p){return p->sounds.size();}
API(hud_field) void* hud_field(HudFixture* p,u32 kind){switch(kind){case 0:return p->hud.life_handles.data();case 1:return p->hud.bomb_handles.data();case 2:return p->hud.clock_handles.data();case 3:return &p->hud.base;case 4:return &p->hud.indicator;case 5:return &p->hud.difficulty_banner;case 6:return &p->hud.difficulty_label;case 7:return &p->hud.loadout_label;case 8:return &p->hud.flags;case 9:return &p->hud.age;case 10:return &p->hud.current_seconds;case 11:return &p->hud.previous_seconds;case 12:return p->hud.gauges.data();case 13:return p->hud.phase_handles.data();case 14:return &p->session;case 15:return &p->player.position;case 16:return &p->spell;case 17:return &p->world.manager_flags;case 18:return p->hud.bonus_digits.data();case 19:return p->hud.bonus_separators.data();case 20:return &p->hud.result_banner;case 21:return &p->hud.pickup_banner;case 22:return &p->hud.result_panel;case 23:return &p->hud.result_active;default:return nullptr;}}
API(hud_number) const char* hud_number(u32 value,u32 score,i32 last){static std::string text;text=score?score_number(value,last):grouped_number(value);return text.c_str();}
API(hud_score_update) void hud_score_update(HudFixture* p,i32 value){p->hud.update_score(value,p->progress);}
API(hud_draw) u32 hud_draw(HudFixture* p,u32 dialogue,u32 game_flags){p->hud.dialogue=dialogue!=0;p->hud.game_flags=game_flags;p->text.clear();return p->hud.draw(p->draw_context,*p);}
API(hud_draw_field) void* hud_draw_field(HudFixture* p,u32 kind){switch(kind){case 0:return &p->hud.displayed_score;case 1:return &p->hud.score_increment;case 2:return &p->hud.stage_clear_bonus;case 3:return &p->hud.stage_clear_notice;case 4:return &p->hud.chapter_result_notice;case 5:return &p->hud.chapter_result;case 6:return &p->score;case 7:return &p->progress;case 8:return &p->draw_context.graze_total;case 9:return &p->draw_context.spell_available;case 10:return &p->draw_context.spell_frames;case 11:return &p->draw_context.spell_best_time;case 12:return &p->world.boss_hundredths;default:return nullptr;}}
API(hud_text_count) u32 hud_text_count(HudFixture* p){return p->text.size();}
API(hud_text_position) const Vec3* hud_text_position(HudFixture* p,u32 i){return &p->text[i].position;}
API(hud_text_style) const HudTextStyle* hud_text_style(HudFixture* p,u32 i){return &p->text[i].style;}
API(hud_text_value) const char* hud_text_value(HudFixture* p,u32 i){return p->text[i].text.c_str();}
API(hud_notice_setup) void hud_notice_setup(HudFixture* p,u32 flags,float remaining,float initial,i32 target,i32 increment,i32 duration){p->hud.flags=(p->hud.flags&~0x1800u)|flags;p->hud.notice_age.set(0);auto& r=p->hud.chapter_result;r.percentage=remaining;r.initial_percentage=initial;r.target=target;r.increment=increment;r.score=0;r.duration=duration;}
API(hud_notice_age) const Timer* hud_notice_age(HudFixture* p){return &p->hud.notice_age;}
