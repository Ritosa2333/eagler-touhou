#pragma once
#include "EnemySystem.hpp"
#include "BulletSystem.hpp"
#include "BackgroundScript.hpp"
#include "PracticeConfig.hpp"
namespace th08 {
struct PlayerScenePlatform {
    virtual ~PlayerScenePlatform()=default;
    virtual std::vector<u8> read(const char* path)=0;
    virtual void sound(i32 index,i32 mode,float position,bool panned)=0;
    virtual bool player_motion(const PlayerMovementState&,float,const FrameTiming&,float&,float&){return false;}
};
struct PlayerSceneWorld {
    EffectSystem& effects;ItemSystem& items;EnemySystem& enemies;BulletSystem& bullets;
    SpellSystem& spells;SpellPresentation& announcement;GuiState& hud;GuiImplState& display;GuiController& gui;
    BackgroundScript& background;ScreenEffects& screen;AsciiManager& ascii;AsciiContext& ascii_context;EclGlobals& ecl;
};
// Production services for PlayerSimulation and ItemSystem. Bind the scene
// after constructing its owners, before calling player initialization.
class PlayerScene:public PlayerSetupActions,public PlayerLifeActions,public PlayerBombActions,public PlayerShotActions,public PlayerSimulationWorld,public ItemSystemActions {
    PlayerSimulationState& state;ShotResource (&shots)[2];GameGlobals& numbers;GameValues& values;GameGauge& gauge;GameRank& rank;PracticeState& practice;
    AnmLibrary& library;AnmExecutor& animations;AnmRenderer& renderer;PlayerScenePlatform& platform;PlayerSceneWorld* world=nullptr;bool failed=false;
    PlayerBombBoss boss_views[8];EclVm* boss_owners[8]{};
    struct Motion:PlayerMotionActions {
        PlayerScene& s;explicit Motion(PlayerScene& s):s(s){}
        void animation(AnmVm& vm,i32 id)override{s.animation(vm,id);}
        void animation(i32 id)override{s.animation(id);}
        void pose(i32 id)override{s.animation(id);}
        bool movement(const PlayerMovementState& state,float speed,const FrameTiming& timing,float& x,float& y)override{return s.platform.player_motion(state,speed,timing,x,y);}
        void effect(i32 id,const Vec3& p,u32 color)override{s.effect(id,p,1,color);}
        AnmVm* focus_effect(const Vec3& p)override{return s.fixed_effect(22,p,2,0xffffffff);}
        EffectState* gauge_effect(const Vec3& p)override{return s.fixed_effect(25,p,8,0xffffffff);}
        void step_animation(AnmVm& vm)override{s.step_animation(vm);}
        void draw_player(AnmVm& vm)override{s.renderer.draw_no_rotation(vm);}
        void draw(AnmVm& vm)override{s.renderer.draw_2d(vm);}
    } motion{*this};
    struct Patterns:PlayerBombPatternActions {
        PlayerScene& s;explicit Patterns(PlayerScene& s):s(s){}
        void spell_overlay(i32 form,const char* name,i32 style)override;
        EffectState* fixed_effect(i32 id,const Vec3& p,i32 slot,u32 color)override{return s.fixed_effect(id,p,slot,color);}
        void home_items()override{s.world->items.collect_all();}
        void animation(AnmVm& vm,i32 script,bool effect)override{s.failed|=!s.library.start(effect?6:5,script,vm,s.animations);}
        bool step_animation(AnmVm& vm)override{return s.step_animation(vm);}
        EffectState* spawn_effect(i32 id,const Vec3& p,i32 n,u32 color)override{return s.world->effects.spawn(id,p,n,color);}
        EffectState* parameter_effect(i32 id,const Vec3& p,const Vec3& params,i32 slot,u32 color)override{return s.world->effects.fixed(id,p,slot,color,&params);}
        void sound(i32 id,i32 mode)override{s.sound(id,mode);}
        void panned_sound(i32 id,float x)override{s.sound(id,x);}
        void screen(ScreenEffectType type,i32 duration,i32 a,i32 b,i32 c,i32 priority)override{s.world->screen.create(type,duration,a,b,c,priority);}
        void background_color(u32 color)override{s.reset_screen_color();s.world->background.tint(color);s.world->background.state.use_tint=1;}
        void reset_screen_color()override{s.reset_screen_color();}
        void draw(AnmVm& vm,bool rotated)override{if(rotated)s.renderer.draw_2d(vm);else s.renderer.draw_no_rotation(vm);}
        void rectangle(float a,float b,float c,float d,u32 color)override{const u32 colors[]{color,color,color,color};s.renderer.draw_rectangle(a,b,c,d,colors);}
    } patterns{*this};
public:
    PlayerScene(PlayerSimulationState& s,ShotResource (&r)[2],GameGlobals& n,GameValues& v,GameGauge& g,GameRank& rank,PracticeState& practice,AnmLibrary& l,AnmExecutor& a,AnmRenderer& renderer,PlayerScenePlatform& p)
     :state(s),shots(r),numbers(n),values(v),gauge(g),rank(rank),practice(practice),library(l),animations(a),renderer(renderer),platform(p){}
    void bind(PlayerSceneWorld& scene){world=&scene;scene.items.bind_hud(scene.hud);}
    void reset(){failed=false;for(u32 i=0;i<8;i++){boss_owners[i]=nullptr;boss_views[i]={};}}
    PlayerSimulationServices services(){return {*this,motion,*this,*this,patterns,*this,*this};}
    bool prepare();void finish();void sync_values();bool invalid()const{return failed;}
    bool load_shots(bool focused,const char* path)override{const auto bytes=platform.read(path);return shots[focused].load(bytes.data(),bytes.size());}
    bool load_animation(const char* path)override{const auto bytes=platform.read(path);return library.load(5,bytes.data(),bytes.size())!=nullptr;}
    void retained_animation()override{failed|=library.get(5)==nullptr;}
    void animation(AnmVm& vm,i32 script)override{failed|=!library.start(5,script,vm,animations);}
    void animation(i32 script)override{animation(state.motion.animation,script);}
    void hud_interrupt(i32 value)override{world->ascii.set_gauge_interrupt(value);}
    void hud_group(i32 index,i32 interrupt)override{if(u32(index)<4)world->ascii.state.boss_markers[index].pendingInterrupt=i16(interrupt);else failed=true;}
    void time_item_threshold(i32 value)override{world->enemies.time_item_threshold=value;}
    void update_integrity()override{values.update_integrity();}
    void dissolve()override{++practice.tracker_dissolve_count;}
    void randomize_integrity()override{values.randomize_integrity();}
    void effect(i32 kind,const Vec3& p,i32 count,u32 color)override{world->effects.spawn(kind,p,count,color);failed|=world->effects.invalid;}
    void effect(i32 kind,const Vec3& p)override{effect(kind,p,1,0xffffffff);}
    EffectState* fixed_effect(i32 kind,const Vec3& p,i32 slot,u32 color)override{auto* result=world->effects.fixed(kind,p,slot,color);failed|=world->effects.invalid;return result;}
    void sound(i32 index,float x)override{platform.sound(index,0,x,true);}
    void sound(i32 index,i32 mode)override{platform.sound(index,mode,0,false);}
    void cancel_item_homing()override{world->items.cancel_homing();}
    void cancel_rectangle(const Vec3& p,float width,float height,i32 value,i32 lifetime)override{state.shots.regions.rectangle(false,{p.x,p.y},width,height,value,lifetime);}
    void damage_region(const Vec3& p,const Vec2& size,i32 damage,i32 lifetime,bool laser)override{state.shots.regions.rectangle(true,{p.x,p.y},size.x,size.y,damage,lifetime).suppress_effect=laser;}
    void reset_screen_color()override{renderer.mix_color=0x80808080;renderer.mix_enabled=false;}
    void fail_spell()override{world->spells.fail();}
    void fail_spell_with_bomb()override{world->spells.fail(true);}
    void add_deaths(i32 n)override{failed|=!values.add_deaths(n);sync_values();}
    void add_time_orbs(i32 n)override{values.add_time_orbs(n);sync_values();}
    void set_power(i32 n)override{values.set_power(n);sync_values();}
    void add_power(i32 n)override{failed|=!values.add_power(n);sync_values();}
    void set_bombs(i32 n)override{values.set_bombs(n);sync_values();}
    void add_lives(i32 n)override{failed|=!values.add_lives(n);sync_values();}
    void subtract_rank(i32 n)override{rank.subtract(n);}
    void item(i32 kind,const Vec3& p,i32 mode)override{world->items.spawn(p,kind,mode);failed|=world->items.invalid();}
    void update(PlayerBombKind)override{failed=true;}
    void finish_spell_overlay()override{world->announcement.end_player();}
    void defeat_boss(u32 slot)override;
    void last_word_flash()override{world->screen.create(ScreenEffectType::Flash,30,1,-1,0,21);}
    void add_gauge(i16 n,bool force)override{gauge.add(n,state.bomb.active,force);sync_values();}
    void add_bombs(i32 n)override{failed|=!values.add_bombs(n);sync_values();}
    void count_bomb(i32 n)override{failed|=!values.count_bombs(n);sync_values();}
    bool step_animation(AnmVm& vm)override{const bool result=animations.execute(vm);failed|=animations.invalid;return result;}
    void draw(AnmVm& vm,bool impact)override{if(impact)renderer.draw_player_bullet(vm);else renderer.draw_2d(vm);}
    bool gui_blocked()override{return world->display.dialogue.message>=0||world->display.dialogue.message==-2;}
    u32 practice_cheats()override{return practice.enabled&&!practice.replay?practice.cheats:0;}
    i32 hud_state()override{return world->ascii.state.gauge_interrupt;}
    void add_score(i32 value)override{values.add_score(value);}
    bool boss_present()override{for(auto* boss:world->ecl.boss_slots)if(boss)return true;return false;}
    void popup(const Vec3& p,i32 value,u32 color,bool small)override{world->ascii.create_score(p,value,color,world->ascii_context,small);}
    void gui_popup(i32 value,i32 kind)override{world->gui.show_popup(value,kind);}
    void cancel_bullets()override{world->bullets.clear(1);failed|=world->bullets.invalid();}
    void spell_time(i32 amount)override{world->spells.add_bonus(amount);}
};
}
