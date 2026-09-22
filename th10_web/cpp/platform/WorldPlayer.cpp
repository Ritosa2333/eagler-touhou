#include "../game/CallbackNames.hpp"
#include "World.hpp"
#include "../game/PlayerResources.hpp"
#include "../game/PlayerFrame.hpp"
#include "../game/PlayerOptions.hpp"
#include "../game/PlayerDraw.hpp"
#include "../game/BombEnvironment.hpp"
#include "../game/HighRefresh.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../game/PracticeRuntime.hpp"
#endif
#include <cstdlib>
namespace th10::browser {
namespace {
struct Lifecycle final:PlayerLifecycleEnvironment {
    World& w;explicit Lifecycle(World& world):w(world){
        economy=&w.state.game;manager=&w.engine.manager;effect_file=w.actors.bullets->animation_file;animations=&w.engine;allocation=&w.engine;default_rate=&w.engine.speed;
        auto& spell=*w.actors.spell;spell_elapsed=&spell.elapsed.current;spell_flags=&spell.spell_flags;spell_bonus=&spell.bonus;const u32 indices[]={0,1,3,4,5,6,7};for(u32 i=0;i<7;++i)spell_animation_flags[i]=&spell.bonus_digits[indices[i]].flags;
        death_sound_enabled=w.actors.session&&!(w.actors.session->session_flags&0x200);replay_mode=w.state.replay->mode;
#ifdef TH_ENABLE_THPRAC
        practice=&w.state.practice;
#endif
    }
    void play_death_sound() override{w.sound(4);}
    void update_lives(i32 lives) override{w.actors.gui->update_lives(lives);}
    void show_caution(const Vec3& p) override{w.record_hint("Caution!",p,true);}
};
struct Movement final:PlayerMovementEnvironment {
    World& w;explicit Movement(World& world):w(world){economy=&w.state.game;manager=&w.engine.manager;effect_file=w.actors.bullets->animation_file;animations=&w.engine;allocation=&w.engine;default_rate=&w.engine.speed;always_hitbox=&w.always_hitbox;input_keys=reinterpret_cast<const u32*>(&w.input.player_profiles[0].input.current);enemy_count=w.actors.enemies?&w.actors.enemies->count:nullptr;}
    void update_option(PlayerOption& option) override{if(option.on_update==callback_id::PlayerOptionInitialize)w.actors.player->update_trailing_option(option);else if(option.on_update==callback_id::PlayerOptionUpdate)w.actors.player->update_anchored_option(option,manager->registry);else __builtin_trap();}
    bool movement(const Player& p,i32 speed,i32& x,i32& y)override{
        if(!w.state.replay||w.state.replay->active_stage<0)return false;
        float dx=x,dy=y;bool enabled;
        if(w.motion.playing){enabled=w.motion.playback(w.state.game.stage,dx,dy);}
        else{
            enabled=w.motion.active&&p.state==1&&(!w.actors.gui||!w.actors.gui->dialogue);
            if(enabled){const float rate=w.engine.speed;dx=rate?(w.motion.target_x*100-p.fixed_position.x)/rate:0;dy=rate?(w.motion.target_y*100-p.fixed_position.y)/rate:0;if(!w.motion.unlimited)touhou::input::limit_vector(dx,dy,float(speed));dx=std::round(dx);dy=std::round(dy);w.motion.mark_cheat_movement(dx,dy);}
            w.motion.record(w.state.game.stage,enabled,dx,dy);
        }
        if(enabled&&(!std::isfinite(dx)||!std::isfinite(dy)||std::abs(dx)>touhou::input::MotionTrack::velocity_limit||std::abs(dy)>touhou::input::MotionTrack::velocity_limit)){w.fail();return false;}
        if(enabled){x=i32(dx);y=i32(dy);}return enabled;
    }
};
struct Shooting final:PlayerShootingEnvironment {
    World& w;explicit Shooting(World& world):w(world){economy=&w.state.game;manager=&w.engine.manager;animations=&w.engine;allocation=&w.engine;default_rate=&w.engine.speed;input_keys=reinterpret_cast<const u32*>(&w.input.player_profiles[0].input.current);dialogue_active=w.actors.gui&&w.actors.gui->dialogue;enemy_manager_present=w.actors.enemies!=nullptr;}
    void initialize_shot(Player& player,PlayerShot& shot,i32) override{if(shot.definition->on_initialize==callback_id::HomingShotInitialize)player.initialize_homing_shot(shot);else __builtin_trap();}
    void update_shot(Player& player,PlayerShot& shot) override{if(shot.definition->on_update==callback_id::HomingShotUpdate)player.update_homing_shot(shot);else if(shot.definition->on_update==callback_id::LaserShotUpdate)player.update_option_laser(shot);else __builtin_trap();}
    void play_shot_sound(i32 sound,float x) override{w.sound(sound,x);}
};
struct Damage final:PlayerDamageEnvironment,PlayerCollisionEnvironment {
    World& w;explicit Damage(World& world):w(world){registry=&w.engine.manager.registry;economy=&w.state.game;default_rate=&w.engine.speed;dialogue_active=w.actors.gui&&w.actors.gui->dialogue;
#ifdef TH_ENABLE_THPRAC
        practice=&w.state.practice;
#endif
    }
    void hit(Player& player) override{Lifecycle env(w);player.hit(env);}
    // The shipped profiles have no hit callbacks; their sole table entry is 0.
    i32 shot_hit(Player&,PlayerShot&,const Vec3&) override{__builtin_trap();}
    u32 create_animation(AnmFile& file,i32 script,u32 tag) override{return w.animation(file,script,tag);}
    i32 bomb_damage(const Vec3& target) override{return w.bomb_damage(target);}
};
struct Frame final:PlayerFrameEnvironment {
    World& w;Lifecycle life;Movement move;Shooting shoot;
    explicit Frame(World& world):w(world),life(w),move(w),shoot(w){
        economy=&w.state.game;default_rate=&w.engine.speed;input_keys=reinterpret_cast<const u32*>(&w.input.player_profiles[0].input.current);gui_present=w.actors.gui!=nullptr;dialogue=w.actors.gui?reinterpret_cast<const u32*>(&w.actors.gui->dialogue):nullptr;enemy_count=w.actors.enemies?&w.actors.enemies->count:nullptr;replay_mode=&w.state.replay->mode;bomb=w.actors.bomb;animations=&w.engine;lifecycle=&life;movement=&move;shooting=&shoot;
#ifdef TH_ENABLE_THPRAC
        practice=&w.state.practice;
#endif
    }
    void clear_bullets(bool force) override{
        // This API's flag means include protection, the opposite of the
        // BulletManager cancellation API used by dialogue and spell clears.
        w.cancel_bullets(!force);
    }
    void clear_lasers(bool convert) override{w.cancel_lasers(convert);}
    void cancel_bullet_circle(const Vec3& p,float radius,bool protection) override{w.cancel_bullet_circle(p,radius,false,protection);}
    void cancel_laser_circle(const Vec3& p,float radius) override{w.cancel_laser_circle(p,radius,0);}
    void start_bomb() override{w.start_bomb();}
    void update_options(Player&) override{w.configure_player();}
    void update_power(i32 whole,i32 fraction) override{w.hud->update_power(whole,fraction);}
    void drop_power(const Vec3& p,i32 kind,float angle) override{w.spawn_item(p,kind,0xffffff,angle,3);}
    void game_over(bool replay) override{if(replay)w.select_screen(4);else w.show_results(false);}
};
struct Profiles final:PlayerProfileEnvironment {
    World& w;explicit Profiles(World& world):w(world){static constexpr u32 initialize[]={0,callback_id::HomingShotInitialize,0},update[]={0,callback_id::HomingShotUpdate,callback_id::LaserShotUpdate,0},empty[]={0};callbacks={initialize,update,empty,empty};}
    PlayerProfile* load(const char* name) override{return static_cast<PlayerProfile*>(w.read_file(name));}
};
struct Resources final:PlayerResourceEnvironment {
    World& w;explicit Resources(World& world):w(world){
        static constexpr const char* names[]={"pl00a.sht","pl00b.sht","pl00c.sht","pl01a.sht","pl01b.sht","pl01c.sht"};static constexpr float hitbox[]={2,3.5f},attraction[]={5,7},pickup[]={40,48},focus[]={100,118};
        game=&w.state.game;current=&w.actors.player;cached_profile=&w.cached_profile;profile_names=names;hitbox_sizes=hitbox;attraction_speeds=attraction;pickup_sizes=pickup;focused_pickup_sizes=focus;rate=&w.engine.speed;registry=&w.engine.manager.registry;animation_slot=&w.engine.manager.files[8];chain=&w.chain;callbacks=&w.engine.callback_environment;update_callback=callback_id::PlayerUpdate;draw_callback=callback_id::PlayerDraw;
    }
    Player* allocate() override{return static_cast<Player*>(std::malloc(sizeof(Player)));}
    AnmFile* load_animations(const char* name) override{return w.engine.manager.load(8,name,w.engine.resources);}
    i32 load_profile(Player& player,const char* name) override{Profiles env(w);return player.load_profile(name,env);}
    void initialize_animation(Player& player) override{initialize_embedded_animation(*player.animation_file,player.animation,0,w.engine,w.engine.manager.started_scripts);}
    void configure_options(Player&) override{w.configure_player();}
    void display_lives(i32 lives) override{w.actors.gui->update_lives(lives);}
    void report_error() override{w.fail();}
    void release_animations(AnmFile& file) override{file.release(w.engine.resources);}
    void delete_object(void* p) override{std::free(p);}void free_bytes(void* p) override{std::free(p);}
};
struct Draw final:PlayerDrawEnvironment {
    World& w;explicit Draw(World& world):w(world){game=&w.state.game;enemies=&w.actors.enemies;gui=&w.actors.gui;controller_flags=w.actors.session?reinterpret_cast<const std::int8_t*>(&w.actors.session->configuration[48]):nullptr;results_state=&w.actors.results->state;}
    void draw_animation(AnmVm& vm) override{w.engine.draw(vm);}
    void draw_option(PlayerOption&) override{__builtin_trap();} // Original option draw slots remain null.
    void rectangle(const ScreenRect& rect,u32 color) override{w.rectangle(rect,color);}
};
struct BombServices final:BombEnvironment {
    World& w;Lifecycle life;explicit BombServices(World& world):w(world),life(world){shared=&life;player_position=&w.actors.player->position;spell_number=&w.actors.spell->number;}
    void play_sound(i32 id,float x) override{w.sound(id,x);}
    void update_power(i32 whole,i32 fraction) override{w.hud->update_power(whole,fraction);}
    void cancel_bullets(const Vec3& p,float radius,bool convert,bool protection) override{w.cancel_bullet_circle(p,radius,convert,protection);}
    void cancel_lasers(const Vec3& p,float radius,bool convert) override{w.cancel_laser_circle(p,radius,convert);}
};
}
bool World::create_player(){Resources env(*this);return PlayerResources::create(env)!=nullptr;}
void World::destroy_player(Player* player){Resources env(*this);PlayerResources{*player,env}.shutdown();std::free(player);}
void World::activate_player(){Resources env(*this);PlayerResources{*actors.player,env}.activate();}
void World::configure_player(){PlayerOptionsEnvironment env{&state.game,&engine.manager,&engine,&engine,actors.player,callback_id::PlayerOptionInitialize,callback_id::PlayerOptionUpdate};actors.player->reconfigure_options(env);}
i32 World::update_player(){
#ifdef TH_ENABLE_THPRAC
    // Per-frame trainer updates run after input publication and before the
    // player consumes its buttons (F5 auto-bomb injects the bomb bit here).
    update_practice(*this,state);
#endif
    auto& p=*actors.player;player_presentation={p.position,p.state,true};Frame env(*this);return p.update(env);}
i32 World::draw_player(){
    Draw env(*this);auto& player=*actors.player;Player copy;Player* draw=&player;
    if(high_refresh::render_only){copy=player;draw=&copy;engine.present(copy.animation,player.animation);if(high_refresh::active&&player_presentation.valid&&player_presentation.state==player.state){const float dx=player.position.x-player_presentation.position.x,dy=player.position.y-player_presentation.position.y;if(dx*dx+dy*dy<16384.0f)copy.position={high_refresh::lerp_world(player_presentation.position.x,player.position.x),high_refresh::lerp_world(player_presentation.position.y,player.position.y),high_refresh::lerp_world(player_presentation.position.z,player.position.z)};}}
    return draw->draw(env);
}
void World::hit_player(){Lifecycle env(*this);actors.player->hit(env);}
i32 World::player_damage(const Vec3& p,const Vec2& size){Damage env(*this);return actors.player->damage(p,size,env);}
i32 World::collide_player(const Vec3& p,const Vec2& size){Damage env(*this);return actors.player->collide_rectangle(p,size,env);}
i32 World::collide_player_laser(const Vec3& p,float angle,float width,float length){Damage env(*this);return actors.player->collide_laser(p,angle,width,length,env);}
i32 World::start_bomb(){BombServices env(*this);return actors.bomb->start(env);}
i32 World::update_bomb(){BombServices env(*this);return actors.bomb->update(env);}
i32 World::bomb_damage(const Vec3& target){return actors.bomb->damage(target,{actors.spell->spell_flags,state.game.character,actors.enemies->bosses[0]!=nullptr});}
}
