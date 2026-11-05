#pragma once
#include "EnemySimulation.hpp"
#include "EnemyDrawing.hpp"
#include "PlayerSimulation.hpp"
#include "EffectSystem.hpp"
#include "ItemSystem.hpp"
#include "AsciiManager.hpp"
#include "BulletScoreCancel.hpp"
#include "EclScene.hpp"
#include "EclNative.hpp"
#include "SpellSystem.hpp"
#include "SpellBackground.hpp"
#include "GuiController.hpp"
namespace th08 {
// These are the live scene owners. The binding must outlive EnemySystem and
// its timing must be the same timing object supplied to the ECL executor.
struct EnemyNativeScene {
    FrameTiming& timing;AnmExecutor& animations;ScreenEffects& screen;BackgroundState& background;
    SpellBackground& spell_background;SpellPresentation& spell_presentation;
    i32& hud_redraw;i32& screen_effect_counter;GuiController* gui=nullptr;
};
struct EnemySystemActions {
    virtual ~EnemySystemActions()=default;
    virtual void sound(i32 index,i32 mode)=0;
    virtual void panned_sound(i32 index,float x)=0;
    virtual void bonus(i32 value)=0;
    virtual void message(i32 entry)=0;
    virtual bool clock(i32 action)=0;
};
// Connects the recovered manager to the actual player, items, effects,
// projectiles and popup objects used by the same running scene.
class EnemySystem:public SpellSystemActions,private EnemySimulationActions,private BulletScoreCancelActions,private EclSceneActions,private EclNativeActions {
public:
    EnemySimulationState state;
    EnemyPopulation population;
private:
    EclGlobals& globals;GameGlobals& numbers;GameValues& values;PlayerSimulation& player;
    EffectSystem& effects;ItemSystem& items;BulletManagerState& projectiles;
    AsciiManager& ascii;AsciiContext& ascii_context;EnemySystemActions& actions;
    EnemySimulationInput input;EnemySimulation simulation;EnemyDrawing drawing;bool failed=false;
    EclNativeServices native_services;EnemyNativeScene* native_scene=nullptr;
    void read_player();
    void read_collision(){read_player();if(globals.gui)std::memcpy(&globals.gui->flags,&player.status().context.hud_flags,4);}
    void publish_player();
    void effect(i32,const Vec3&,i32,u32)override;
    void parameter_effect(i32,const Vec3&,const Vec3&,i32,u32)override;
    EffectState* attached_effect(i32,const Vec3&,i32,u32,bool)override;
    bool clock(i32 action)override{publish_player();return native_scene&&native_scene->gui?native_scene->gui->clock(action):actions.clock(action);}
    bool cancel_enemies(i32 maximum,i32& score)override{return population.cancel_for_score(maximum,score,*this);}
    bool clear_projectiles(i32 mode)override;
    void clear_projectiles_near(const Vec3&,float radius)override;
    void item(const Vec3&,i32,i32)override;
    void screen(i32,i32,i32,i32,i32,i32)override;
    bool spell_background(i32,const Vec3&)override;
    void background_interrupt(i16)override;
    void tint(u32)override;
    void laser(const Vec2&,const Vec2&,const Vec3&,float,bool)override;
    bool bomb_active()override{return player.status().bomb.active!=0;}
    bool spell_announcement(i32,const char*,i32)override;
    void bonus(i32 n)override{if(native_scene&&native_scene->gui)native_scene->gui->show_bonus(n);else actions.bonus(n);}
    void spell_bonus(i32 n)override{if(native_scene&&native_scene->gui)native_scene->gui->show_spell_bonus(n);else failed=true;}
    void popup(i32 n,i32 kind)override{if(native_scene&&native_scene->gui)native_scene->gui->show_popup(n,kind);else failed=true;}
    void drop(const Vec3& p,i32 kind,i32 mode)override{item(p,kind,mode);}
    i32 power()override{return number(numbers.power).truncate_int();}
    void sound(i32 index,i32 mode)override{actions.sound(index,mode);}
    void panned_sound(i32 index,float x)override{actions.panned_sound(index,x);}
    void popup(const Vec3& p,i32 value,i32 multiplier,u32 color)override{ascii.create_time(p,value,multiplier,color,ascii_context);}
    void popup_scale(float x,float y)override{ascii.state.scale_x=x;ascii.state.scale_y=y;}
    void score_popup(const Vec3& p,i32 value,u32 color)override{ascii.create_score(p,value,color,ascii_context);}
    AnmVm* overlay(i32,const Vec3&,i32,u32)override;
    void move_overlay(AnmVm& vm,const Vec3& p)override{static_cast<EffectState&>(vm).position=p;}
    bool cancel_projectiles(i32 maximum,bool reward,i32& score)override;
    i32 barrier(BulletState&)override;
    void popup_bonus(i32 value)override{bonus(value);}
    void message(i32 entry)override{actions.message(entry);}
    void set_power(i32 value)override{values.set_power(value);player.status().context.power=number(numbers.power).truncate_int();}
    i32 graze(const Vec3&,const Vec3&)override;
    i32 hit(const Vec3&,const Vec3&)override;
    i32 damage(const Vec3&,const Vec3&,i32& time_items,i32& bomb_hit)override;
public:
    i32 time_item_threshold=0;
    EnemySystem(EclProgram&,EclExecutor&,const FrameTiming&,Rng&,GameGlobals&,GameValues&,GameRank&,GameGauge&,PlayerSimulation&,EffectSystem&,ItemSystem&,BulletManagerState&,AsciiManager&,AsciiContext&,AnmRenderer&,EnemySystemActions&);
    ~EnemySystem(){if(globals.scene_actions==this)globals.scene_actions=nullptr;if(globals.native_services==&native_services)globals.native_services=nullptr;}
    void bind_native(EnemyNativeScene& scene){native_scene=&scene;native_services.timing=&scene.timing;native_services.actions=this;native_services.projectiles=&projectiles;native_services.screen_effect_counter_value=&scene.screen_effect_counter;globals.native_services=&native_services;scene.spell_presentation.bind_redraw(scene.hud_redraw);}
    EclNativeServices& native_state()noexcept{return native_services;}
    void reset();
    EnemySpawnResult spawn(const TimelineSpawn&);
    JobResult update();
    bool draw(i32 first=0,i32 last=4);
    bool draw_high(){return draw(0,2);}
    bool draw_low(){failed|=!drawing.low(state.layers,globals.game_flags,ascii_context.arcade_origin);return !failed;}
#if defined(TH_PRESENTATION_AUDIT)
    const float* audit_presentation_sample(uintptr_t object,u32 index)const{return drawing.audit_presentation_sample(object,index);}
#endif
    bool invalid()const noexcept{return failed||simulation.invalid();}
};
}
