#include "../platform/World.hpp"
#include "PlayerCollisionBroadphase.hpp"
#include "Balance.hpp"
#include "PlayerPresentation.hpp"
#include "../game/CallbackNames.hpp"
#include "../game/BombEnvironment.hpp"
#include "../game/GameObjectResources.hpp"
#include "../game/HighRefresh.hpp"
#include "../game/PlayerDraw.hpp"
#include "../game/PlayerFrame.hpp"
#include "../game/PlayerOptions.hpp"
#include "../game/PlayerResources.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../game/PracticeRuntime.hpp"
#endif
#include <cstdlib>
#include <cstring>
#include <cmath>

namespace th10::browser {
namespace {
constexpr const char* kProfileNames[]={"pl00a.sht","pl00b.sht","pl00c.sht",
                                       "pl01a.sht","pl01b.sht","pl01c.sht"};
constexpr float kHitboxSizes[]={2.f,3.5f};
constexpr float kAttractionSpeeds[]={5.f,7.f};
constexpr float kPickupSizes[]={40.f,48.f};
constexpr float kFocusedPickupSizes[]={100.f,118.f};
constexpr u32 kOptionInitializers[]={0,callback_id::HomingShotInitialize,0};
constexpr u32 kOptionUpdates[]={0,callback_id::HomingShotUpdate,
                                callback_id::LaserShotUpdate,0};
constexpr u32 kEmptyCallbacks[]={0};
constexpr u32 kOptionIndices[]={0,1,3,4,5,6,7};
constexpr i32 kSpiritDrift=20;
constexpr i32 kSpiritMinY=31600,kSpiritMaxY=41600;

i32 multiplayer_spawn_x(u32 seat,u32 count){
    if(count==2)return seat==0?-3200:3200;
    if(count==3)return (static_cast<i32>(seat)-1)*4800;
    return 0;
}

void initialize_spirit_drift(World& world,Player& player){
    auto& random=world.engine.script_random;
    const i32 x=(random.next_word()&1)?kSpiritDrift:-kSpiritDrift;
    const i32 y=(random.next_word()&1)?kSpiritDrift:-kSpiritDrift;
    player.input_velocity={x,y};player.velocity={x,y};
    player.focused=0;player.direction=0;
    if(player.focus_animation){
        world.engine.manager.registry.request_delete(player.focus_animation);
        player.focus_animation=0;
    }
}

void update_spirit_drift(Player& player){
    i32 x=wrapping_add(player.fixed_position.x,player.velocity.x);
    i32 y=wrapping_add(player.fixed_position.y,player.velocity.y);
    if(x< -18400){x=-18400;player.velocity.x=std::abs(player.velocity.x);}
    else if(x>18400){x=18400;player.velocity.x=-std::abs(player.velocity.x);}
    if(y<kSpiritMinY){y=kSpiritMinY;player.velocity.y=std::abs(player.velocity.y);}
    else if(y>kSpiritMaxY){y=kSpiritMaxY;player.velocity.y=-std::abs(player.velocity.y);}
    player.fixed_position={x,y};
    player.position.x=Scalar::mul_int(x,.01f);player.position.y=Scalar::mul_int(y,.01f);
    player.position_history[0]=player.fixed_position;
}

void set_timer(Timer& timer,u32& flags,i32 frames,const float* rate){
    if(!(flags&1)){timer.rate=rate;flags|=1;}
    timer.previous=wrapping_add(frames,-1);
    timer.current=frames;
    timer.fractional=Extended::from_int(frames).to_float();
}

void clear_player_offense(Player& player,AnmRegistry& registry){
    for(auto& shot:player.shots){
        if(shot.animation)registry.request_delete(shot.animation);
        if(shot.secondary_animation)registry.request_delete(shot.secondary_animation);
        shot.animation=shot.secondary_animation=0;
        shot.state=0;
        shot.collided=shot.first_collision=0;
    }
    for(auto& area:player.damage_areas)area.flags&=~1u;
    for(auto& laser:player.active_lasers)laser=0;
    player.fire_timer.current=-1;
    player.fire_timer.previous=-2;
}

void canonicalize_next_stage_player(World& world,multiplayer::Pilot& pilot){
    if(!pilot.player)return;
    auto& player=*pilot.player;
    auto& registry=world.engine.manager.registry;

    // Unlike an in-stage rescue, a new stage is a portable Replay boundary.
    // Rebuild the transient combat portion of Player to the same semantics as
    // PlayerResources::start(), while ReplayStage remains responsible for the
    // authored position/history/focus/option geometry captured at activation.
    for(auto& shot:player.shots){
        if(shot.animation)registry.request_delete(shot.animation);
        if(shot.secondary_animation)registry.request_delete(shot.secondary_animation);
        std::memset(&shot,0,sizeof(shot));
    }
    for(auto& area:player.damage_areas)std::memset(&area,0,sizeof(area));
    for(auto& laser:player.active_lasers)laser=0;
    for(auto& option:player.options){
        for(auto animation:option.animations)if(animation)registry.request_delete(animation);
        std::memset(&option,0,sizeof(option));
    }
    player.option_count=0;
    player.state=0;
    player.death_position={};
    player.input_velocity={};
    player.velocity={};
    player.direction=0;
    player.target=nullptr;
    player.target_seen=0;
    player.focused=0;
    player.option_follow_speed=30;
    set_timer(player.invulnerability,player.invulnerability_flags,120,&world.engine.speed);
    world.configure_player(pilot);
    // ReplayStage restores its captured option geometry with snap_next clear.
    // A real next-stage boundary must expose the same first-tick semantics;
    // reconfigure_options sets this latch only for ordinary power changes.
    for(auto& option:player.options)option.snap_next=0;
}

struct Profiles final:PlayerProfileEnvironment {
    World& world;
    explicit Profiles(World& value):world(value){
        callbacks={kOptionInitializers,kOptionUpdates,kEmptyCallbacks,kEmptyCallbacks};
    }
    PlayerProfile* load(const char* name) override{
        return static_cast<PlayerProfile*>(world.read_file(name));
    }
};

struct Resources final:PlayerResourceEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    explicit Resources(World& value,multiplayer::Pilot& seat):world(value),pilot(seat){
        game=&pilot.game;
        current=&pilot.player;
        cached_profile=&pilot.cached_profile;
        profile_names=kProfileNames;
        hitbox_sizes=kHitboxSizes;
        attraction_speeds=kAttractionSpeeds;
        pickup_sizes=kPickupSizes;
        focused_pickup_sizes=kFocusedPickupSizes;
        rate=&world.engine.speed;
        registry=&world.engine.manager.registry;
        animation_slot=&world.engine.manager.files[World::pilot_animation_slot(pilot.game.character)];
        chain=&world.chain;
        callbacks=&world.engine.callback_environment;
        update_callback=callback_id::PlayerUpdate;
        draw_callback=callback_id::PlayerDraw;
    }
    Player* allocate() override{return static_cast<Player*>(std::malloc(sizeof(Player)));}
    AnmFile* load_animations(const char* name) override{
        if(pilot.game.character<0||pilot.game.character>1)return nullptr;
        const char* expected=pilot.game.character==0?"pl00.anm":"pl01.anm";
        if(std::strcmp(name,expected))return nullptr;
        return world.engine.manager.load(World::pilot_animation_slot(pilot.game.character),
                                         name,world.engine.resources);
    }
    i32 load_profile(Player& player,const char* name) override{
        Profiles env(world);return player.load_profile(name,env);
    }
    void initialize_animation(Player& player) override{
        initialize_embedded_animation(*player.animation_file,player.animation,0,
                                      world.engine,world.engine.manager.started_scripts);
    }
    void configure_options(Player&) override{world.configure_player(pilot);}
    void display_lives(i32 lives) override{
        if(pilot.seat==0&&world.actors.gui)
            world.actors.gui->update_lives(lives);
    }
    void report_error() override{world.fail();}
    void release_animations(AnmFile& file) override{file.release(world.engine.resources);}
    void delete_object(void* object) override{std::free(object);}
    void free_bytes(void* memory) override{world.engine.release_memory(memory);}
};

struct Lifecycle final:PlayerLifecycleEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    Lifecycle(World& value,multiplayer::Pilot& seat):world(value),pilot(seat){
        economy=&pilot.game;
        player_count=world.player_count;
        manager=&world.engine.manager;
        effect_file=world.actors.bullets->animation_file;
        animations=&world.engine;
        allocation=&world.engine;
        default_rate=&world.engine.speed;
        auto& spell=*world.actors.spell;
        spell_elapsed=&spell.elapsed.current;
        spell_flags=&spell.spell_flags;
        spell_bonus=&spell.bonus;
        for(u32 i=0;i<7;++i)
            spell_animation_flags[i]=&spell.bonus_digits[kOptionIndices[i]].flags;
        death_sound_enabled=world.actors.session&&!(world.actors.session->session_flags&0x200);
        replay_mode=world.state.replay?world.state.replay->mode:0;
#ifdef TH_ENABLE_THPRAC
        practice=&world.state.practice;
#endif
    }
    void play_death_sound() override{world.sound(4);}
    void update_lives(i32 lives) override{
        if(pilot.seat==0&&world.actors.gui)
            world.actors.gui->update_lives(lives);
    }
    void show_caution(const Vec3& position) override{
        world.record_hint("Caution!",position,true);
    }
};

struct Movement final:PlayerMovementEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    i32 spirit_enemy_count=1;
    Movement(World& value,multiplayer::Pilot& seat,bool is_spirit=false)
        :world(value),pilot(seat){
        economy=&pilot.game;
        manager=&world.engine.manager;
        effect_file=world.actors.bullets->animation_file;
        animations=&world.engine;
        allocation=&world.engine;
        default_rate=&world.engine.speed;
        always_hitbox=&world.always_hitbox;
        input_keys=&pilot.input_keys;
        enemy_count=is_spirit?&spirit_enemy_count:
            (world.actors.enemies?&world.actors.enemies->count:nullptr);
    }
    void update_option(PlayerOption& option) override{
        auto* player=pilot.player;
        if(!player)__builtin_trap();
        if(option.on_update==callback_id::PlayerOptionInitialize)
            player->update_trailing_option(option);
        else if(option.on_update==callback_id::PlayerOptionUpdate)
            player->update_anchored_option(option,manager->registry);
        else __builtin_trap();
    }
    bool movement(const Player& player,i32 speed,i32& x,i32& y) override{
        return multiplayer::InputLanes::ResolveMovement(
            world.state.input_lanes.inputs[pilot.seat],speed,
            player.fixed_position.x,player.fixed_position.y,world.engine.speed,x,y);
    }
};

struct Shooting final:PlayerShootingEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    explicit Shooting(World& value,multiplayer::Pilot& seat):world(value),pilot(seat){
        economy=&pilot.game;
        manager=&world.engine.manager;
        animations=&world.engine;
        allocation=&world.engine;
        default_rate=&world.engine.speed;
        input_keys=&pilot.input_keys;
        dialogue_active=world.actors.gui&&world.actors.gui->dialogue;
        enemy_manager_present=world.actors.enemies!=nullptr;
    }
    void initialize_shot(Player& player,PlayerShot& shot,i32) override{
        if(shot.definition->on_initialize==callback_id::HomingShotInitialize)
            player.initialize_homing_shot(shot);
        else __builtin_trap();
    }
    void update_shot(Player& player,PlayerShot& shot) override{
        if(shot.definition->on_update==callback_id::HomingShotUpdate)
            player.update_homing_shot(shot);
        else if(shot.definition->on_update==callback_id::LaserShotUpdate)
            player.update_option_laser(shot);
        else __builtin_trap();
    }
    void play_shot_sound(i32 sound,float x) override{world.sound(sound,x);}
};

struct Damage final:PlayerDamageEnvironment,PlayerCollisionEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    Damage(World& value,multiplayer::Pilot& seat):world(value),pilot(seat){
        registry=&world.engine.manager.registry;
        economy=&pilot.game;
        default_rate=&world.engine.speed;
        dialogue_active=world.actors.gui&&world.actors.gui->dialogue;
#ifdef TH_ENABLE_THPRAC
        practice=&world.state.practice;
#endif
    }
    void hit(Player& player) override{
        Lifecycle lifecycle(world,pilot);player.hit(lifecycle);
    }
    i32 shot_hit(Player&,PlayerShot&,const Vec3&) override{__builtin_trap();}
    u32 create_animation(AnmFile& file,i32 script,u32 tag) override{
        return world.animation(file,script,tag);
    }
    i32 bomb_damage(const Vec3& target) override{
        return world.bomb_damage(pilot,target);
    }
};

struct Frame final:PlayerFrameEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    Player& player;
    Lifecycle lifecycle_env;
    Movement movement_env;
    Shooting shooting_env;
    bool spirit;
    explicit Frame(World& value,multiplayer::Pilot& seat,bool is_spirit)
        :world(value),pilot(seat),player(*seat.player),lifecycle_env(value,seat),
         movement_env(value,seat,is_spirit),shooting_env(value,seat),spirit(is_spirit){
        economy=&pilot.game;
        default_rate=&world.engine.speed;
        input_keys=&pilot.input_keys;
        gui_present=world.actors.gui!=nullptr;
        dialogue=world.actors.gui?
            reinterpret_cast<const u32*>(&world.actors.gui->dialogue):nullptr;
        enemy_count=spirit?nullptr:
            (world.actors.enemies?&world.actors.enemies->count:nullptr);
        replay_mode=world.state.replay?&world.state.replay->mode:nullptr;
        bomb=spirit?nullptr:pilot.bomb;
        animations=&world.engine;
        PlayerFrameEnvironment::lifecycle=&lifecycle_env;
        PlayerFrameEnvironment::movement=&movement_env;
        PlayerFrameEnvironment::shooting=&shooting_env;
#ifdef TH_ENABLE_THPRAC
        practice=&world.state.practice;
#endif
    }
    void clear_bullets(bool force) override{
        if(!spirit)world.cancel_bullets(!force);
    }
    void clear_lasers(bool convert) override{
        if(!spirit)world.cancel_lasers(convert);
    }
    void cancel_bullet_circle(const Vec3& point,float radius,bool protection) override{
        world.cancel_bullet_circle(point,radius,false,protection);
    }
    void cancel_laser_circle(const Vec3& point,float radius) override{
        world.cancel_laser_circle(point,radius,0);
    }
    void start_bomb() override{world.start_bomb(pilot);}
    void update_options(Player&) override{world.configure_player(pilot);}
    void update_power(i32 level,i32 fraction) override{
        if(pilot.seat==0&&world.hud)
            world.hud->update_power(level,fraction);
    }
    void drop_power(const Vec3& point,i32 kind,float angle) override{
        // Final-death contents stay native; ordinary P drops follow the roster multiplier.
        const u32 copies=pilot.game.lives<0?1:world.player_count;
        for(u32 copy=0;copy<copies;++copy)world.spawn_item(point,kind,0xffffff,angle,3);
    }
    void game_over(bool) override{
        // Native life/power penalties have already run. Keep the native pilot
        // in TH10's non-colliding Spirit state; cooperation policy owns retry.
        player.state=3;
        if(pilot.bomb)pilot.bomb->active=0;
        clear_player_offense(player,world.engine.manager.registry);
        initialize_spirit_drift(world,player);

        i32 recipient=-1;std::int64_t best=0;
        for(u32 seat=0;seat<world.player_count;++seat){
            if(seat==pilot.seat)continue;
            auto& candidate=world.pilots[seat];
            if(!candidate.player||candidate.player->state==3||candidate.game.lives<0)continue;
            const std::int64_t dx=std::int64_t(candidate.player->fixed_position.x)-player.fixed_position.x;
            const std::int64_t dy=std::int64_t(candidate.player->fixed_position.y)-player.fixed_position.y;
            const std::int64_t distance=dx*dx+dy*dy;
            if(recipient<0||distance<best){recipient=i32(seat);best=distance;}
        }
        if(recipient>=0){
            auto& target=world.pilots[u32(recipient)];
            if(target.game.lives<multiplayer::kMaxLives)++target.game.lives;
            if(target.seat==0&&world.actors.gui)world.actors.gui->update_lives(target.game.lives);
            world.sound(0x2c);
        }
    }
};

struct Draw final:PlayerDrawEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    Draw(World& value,multiplayer::Pilot& seat):world(value),pilot(seat){
        game=&pilot.game;
        enemies=&world.actors.enemies;
        gui=&world.actors.gui;
        controller_flags=world.actors.session?
            reinterpret_cast<const std::int8_t*>(&world.actors.session->configuration[48]):nullptr;
        results_state=world.actors.results?&world.actors.results->state:nullptr;
    }
    void draw_animation(AnmVm& vm) override{world.engine.draw(vm);}
    void draw_option(PlayerOption&) override{__builtin_trap();}
    void rectangle(const ScreenRect& rect,u32 color) override{world.rectangle(rect,color);}
};

struct BombServices final:BombEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    Lifecycle lifecycle;
    BombServices(World& value,multiplayer::Pilot& seat)
        :world(value),pilot(seat),lifecycle(value,seat){
        shared=&lifecycle;
        player_position=&pilot.player->position;
        spell_number=&world.actors.spell->number;
    }
    void play_sound(i32 sound,float x) override{world.sound(sound,x);}
    void update_power(i32 level,i32 fraction) override{
        if(pilot.seat==0&&world.hud)
            world.hud->update_power(level,fraction);
    }
    void cancel_bullets(const Vec3& point,float radius,bool convert,bool protection) override{
        world.cancel_bullet_circle(point,radius,convert,protection);
    }
    void cancel_lasers(const Vec3& point,float radius,bool convert) override{
        world.cancel_laser_circle(point,radius,convert);
    }
};

struct BombResources final:GameObjectResourceEnvironment {
    World& world;
    multiplayer::Pilot& pilot;
    BombResources(World& value,multiplayer::Pilot& seat):world(value),pilot(seat){
        items=&world.actors.items;
        bomb=&pilot.bomb;
        effects=&world.actors.effects;
        chain=&world.chain;
        callbacks=&world.engine.callback_environment;
        item_update=callback_id::ItemsUpdate;item_draw=callback_id::ItemsDraw;
        bomb_update=callback_id::BombUpdate;bomb_draw=callback_id::BombDraw;
        effects_update=callback_id::EffectsUpdate;effects_draw=callback_id::EffectsDraw;
    }
    void* allocate(u32 bytes) override{return std::malloc(bytes);}
    void delete_object(void* object) override{std::free(object);}
    void release_geometry(void* geometry) override{world.engine.release_memory(geometry);}
    AnmFile* load_effect_animations() override{
        return world.engine.manager.load(7,"bullet.anm",world.engine.resources);
    }
    void report_effect_error() override{world.fail();}
};

void cleanup_player(World& world,multiplayer::Pilot& pilot,bool retain_profile){
    Player* player=pilot.player;
    if(!player)return;
    world.chain->remove_locked(player->update_entry,world.engine.callback_environment);
    world.chain->remove_locked(player->draw_entry,world.engine.callback_environment);
    if(player->animation.geometry){world.engine.release_memory(player->animation.geometry);player->animation.geometry=nullptr;}
    if(retain_profile)pilot.cached_profile=player->profile;
    else{
        std::free(player->profile);
        player->profile=nullptr;
        std::free(pilot.cached_profile);
        pilot.cached_profile=nullptr;
    }
    std::free(player);
    pilot.player=nullptr;
}

bool allocate_life_item(void* context,u8 donor,u8 recipient) noexcept{
    auto& world=*static_cast<World*>(context);
    return world.spawn_life_transfer(donor,recipient);
}
bool allocate_power_item(void* context,u8 donor,u8 recipient) noexcept{
    auto& world=*static_cast<World*>(context);
    return world.spawn_power_transfer(donor,recipient);
}

bool nearer(const Player& a,const Player& b,const Vec3& point){
    const double adx=double(a.position.x)-point.x,ady=double(a.position.y)-point.y;
    const double bdx=double(b.position.x)-point.x,bdy=double(b.position.y)-point.y;
    return adx*adx+ady*ady<bdx*bdx+bdy*bdy;
}

template<class Collision,class Candidate>
i32 collide_one_pilot(World& world,const Vec3& point,Collision&& collide,Candidate&& candidate){
    u8 order[multiplayer::kMaxSeats]{};
    u8 count=0;
    for(u32 seat=0;seat<world.player_count;++seat)
        if(world.pilots[seat].player&&world.pilots[seat].player->state!=3&&candidate(*world.pilots[seat].player))
            order[count++]=static_cast<u8>(seat);
    for(u8 i=1;i<count;++i){
        const u8 candidate=order[i];u8 j=i;
        while(j&&nearer(*world.pilots[candidate].player,
                        *world.pilots[order[j-1]].player,point)){
            order[j]=order[j-1];--j;
        }
        order[j]=candidate;
    }
    i32 graze=0;
    for(u8 i=0;i<count;++i){
        auto& pilot=world.pilots[order[i]];
        Damage damage(world,pilot);
        const i32 result=collide(*pilot.player,damage);
        if(result==1)return 1;
        if(result==2)graze=2;
    }
    return graze;
}

void revive_player(World& world,multiplayer::Pilot& pilot,i32 lives){
    if(!pilot.player)return;
    Player& player=*pilot.player;
    pilot.game.lives=lives;
    player.death_position=player.position;
    // A cooperation rescue is not TH10's native respawn.  Keep the Spirit's
    // current world position and resume play there; state 0 would run the
    // authored 60-frame bottom-of-screen respawn path and move the player.
    player.state=1;
    player.input_velocity={0,0};
    player.velocity={0,0};
    for(auto& position:player.position_history)position=player.fixed_position;
    // Active play's first 30 ticks clear bullets in the native respawn path.
    // Rescue starts after that window and supplies only invulnerability.
    set_timer(player.state_timer,player.state_timer_flags,30,&world.engine.speed);
    set_timer(player.focus_timer,player.focus_timer_flags,0,&world.engine.speed);
    set_timer(player.invulnerability,player.invulnerability_flags,280,&world.engine.speed);
    player.target=nullptr;player.target_seen=0;
    clear_player_offense(player,world.engine.manager.registry);
    world.configure_player(pilot);
}

bool refresh_cooperation_from_native(World& world,
                                     multiplayer::State& state) noexcept{
    if(state.seatCount!=world.player_count)return false;
    for(u32 seat=0;seat<world.player_count;++seat){
        const auto& pilot=world.pilots[seat];
        if(!pilot.player)return false;
        const auto life_state=pilot.player->state==3?
            multiplayer::LifeState::Spirit:
            pilot.player->state==2&&pilot.game.lives<0?
                multiplayer::LifeState::Dying:multiplayer::LifeState::Alive;
        if(!multiplayer::ReportNativeSeatOutcome(
               state,static_cast<u8>(seat),life_state,
               static_cast<std::int16_t>(pilot.game.lives),pilot.game.power))
            return false;
    }
    return true;
}
} // namespace

multiplayer::Pilot* World::pilot_for(Player* player){
    for(u32 seat=0;seat<player_count;++seat)
        if(pilots[seat].player==player)return &pilots[seat];
    return nullptr;
}

multiplayer::Pilot* World::pilot_for(Bomb* bomb){
    for(u32 seat=0;seat<player_count;++seat)
        if(pilots[seat].bomb==bomb)return &pilots[seat];
    return nullptr;
}

void World::configure_player(multiplayer::Pilot& pilot){
    if(!pilot.player)return;
    PlayerOptionsEnvironment environment{&pilot.game,&engine.manager,&engine,&engine,
                                         pilot.player,callback_id::PlayerOptionInitialize,
                                         callback_id::PlayerOptionUpdate};
    pilot.player->reconfigure_options(environment);
}

void World::configure_player(){
    for(u32 seat=0;seat<player_count;++seat)configure_player(pilots[seat]);
}

bool World::create_player(){
    if(player_count<multiplayer::kMinSeats||player_count>multiplayer::kMaxSeats){
        fail();return false;
    }
    multiplayer::SeatSetup setup[multiplayer::kMaxSeats]{};
    for(u32 seat=0;seat<player_count;++seat){
        const auto& game=pilots[seat].game;
        setup[seat]={static_cast<u8>(game.character),static_cast<u8>(game.shot_type),
                     static_cast<std::int16_t>(game.lives),game.power};
    }
    if(!multiplayer::Initialize(cooperation,setup,static_cast<u8>(player_count))){
        fail();return false;
    }
    if(state.netplay_runtime.Playback()){
        if(const auto* checkpoint=state.multiplayer_replay.SelectedCheckpoint()){
            const auto& saved=checkpoint->cooperation;
            if(saved.seatCount!=player_count){fail();return false;}
            cooperation.seatCount=saved.seatCount;
            cooperation.wipeTicks=saved.wipeTicks;
            cooperation.retryPending=saved.retryPending!=0;
            for(u32 seat=0;seat<player_count;++seat){
                const auto& from=saved.seats[seat];auto& to=cooperation.seats[seat];
                if(from.lifeState>u8(multiplayer::LifeState::Eliminated)||
                   from.waitingForFocusRelease>1||from.character!=setup[seat].character||
                   from.shot!=setup[seat].shot||from.lives!=setup[seat].lives||
                   from.power!=setup[seat].power){fail();return false;}
                to.lives=from.lives;to.power=from.power;to.character=from.character;to.shot=from.shot;
                to.lifeState=multiplayer::LifeState(from.lifeState);to.rescueTicks=from.rescueTicks;
                to.rescueTarget=from.rescueTarget;to.waitingForFocusRelease=from.waitingForFocusRelease!=0;
            }
        }
    }

    bool success=true;
    // UpdateChain inserts before equal priorities; creating in reverse gives
    // stable seat-ascending PlayerUpdate/PlayerDraw order.
    for(u32 offset=0;offset<player_count;++offset){
        const u32 seat=player_count-1-offset;
        auto& pilot=pilots[seat];
        if(pilot.game.character<0||pilot.game.character>1||
           pilot.game.shot_type<0||pilot.game.shot_type>2){
            success=false;break;
        }
        Resources resources(*this,pilot);
        Player* player=resources.allocate();
        if(!player){success=false;break;}
        PlayerResources native{*player,resources};
        native.initialize();
        if(native.start()){
            cleanup_player(*this,pilot,false);
            success=false;break;
        }
        // Native TH10 starts every Player at x=0.  Spread multiplayer seats
        // around that authored centre point, matching TH07's presentation
        // invariant instead of stacking all ships/options on the first frame.
        const i32 spawn_x=multiplayer_spawn_x(seat,player_count);
        player->set_position({spawn_x,player->fixed_position.y});
        for(auto& point:player->position_history)point=player->fixed_position;
    }
    if(!success){
        const bool retain=(state.game.flags&1u)!=0;
        for(u32 seat=0;seat<player_count;++seat)cleanup_player(*this,pilots[seat],retain);
        if(!retain){
            for(i32 character=0;character<2;++character)
                engine.manager.unload(pilot_animation_slot(character),engine.resources);
        }
        fail();return false;
    }
    actors.player=pilots[0].player;
    return true;
}

void World::destroy_player(Player*){
    const bool retain=(state.game.flags&1u)!=0;
    for(u32 seat=0;seat<player_count;++seat)cleanup_player(*this,pilots[seat],retain);
    actors.player=nullptr;
    if(retain){
        for(i32 character=0;character<2;++character)
            if(auto* file=engine.manager.files[pilot_animation_slot(character)])
                engine.manager.registry.discard_file(file);
    }else{
        for(i32 character=0;character<2;++character)
            engine.manager.unload(pilot_animation_slot(character),engine.resources);
    }
}

void World::activate_player(){
    const bool next_stage=backgrounds.previous!=nullptr;
    if(next_stage){
        multiplayer::State normalized=cooperation;
        if(!refresh_cooperation_from_native(*this,normalized)||
           !multiplayer::BeginNextStage(normalized)){
            fail();return;
        }
        cooperation=normalized;
        for(u32 seat=0;seat<player_count;++seat){
            auto& pilot=pilots[seat];
            pilot.game.lives=cooperation.seats[seat].lives;
            pilot.game.power=cooperation.seats[seat].power;
            canonicalize_next_stage_player(*this,pilot);
        }
        if(pilots[0].player&&actors.gui)actors.gui->update_lives(pilots[0].game.lives);
    }
    for(u32 seat=0;seat<player_count;++seat){
        auto& pilot=pilots[seat];
        if(!pilot.player)continue;
        if(pilot.player->update_entry)pilot.player->update_entry->flags|=2;
        if(pilot.player->draw_entry)pilot.player->draw_entry->flags|=2;
        const bool spirit=!next_stage&&cooperation.seatCount>seat&&
            cooperation.seats[seat].lifeState==multiplayer::LifeState::Spirit;
        if(spirit){pilot.player->state=3;continue;}
        if(!next_stage&&cooperation.seatCount>seat&&
           cooperation.seats[seat].lifeState==multiplayer::LifeState::Dying){
            pilot.player->state=2;continue;
        }
        Resources resources(*this,pilot);
        PlayerResources native{*pilot.player,resources};
        native.activate();
    }
}

i32 World::update_player(Player* player){
    auto* pilot=pilot_for(player);
    if(!pilot||!pilot->player)return 1;
#ifdef TH_ENABLE_THPRAC
    if(pilot->seat==local_player)update_practice(*this,state);
#endif
    const bool spirit=player->state==3;
    pilot->presentation={player->position,player->state,true};
    if(pilot->seat==0)player_presentation={player->position,player->state,true};
    Frame environment(*this,*pilot,spirit);
    if(spirit)update_spirit_drift(*player);
    const i32 result=player->update(environment);
    if(pilot->seat+1==player_count){
        // Rank is shared; adding its timed increase once per pilot would
        // change the difficulty ramp with the number of ships.
        if(actors.enemies&&actors.enemies->count&&actors.gui&&!actors.gui->dialogue&&state.game.stage_frames%60==0){
            bool playing=false;for(u32 seat=0;seat<player_count;++seat)playing|=pilots[seat].player->state!=3;
            if(playing)state.game.add_rank(1);
        }
        update_cooperation();
    }
    return result;
}

u8 World::player_visual_alpha(const AnmVm& vm)const{
    if(!vm.id||local_player>=player_count||
       state.netplay_runtime.Spectator()||state.netplay_runtime.Playback()||!pilots[local_player].player)return 255;
    const auto* root=&vm.child_node;
    for(u32 i=0;root->previous&&i<4096;++i)root=root->previous;
    const u32 id=root->value?root->value->id:vm.id;
    const auto& local=*pilots[local_player].player;
    for(u32 seat=0;seat<player_count;++seat){
        const auto* p=pilots[seat].player;if(!p||seat==local_player)continue;
        if(vm.animation_file!=p->animation_file)continue;
        const float dx=p->position.x-local.position.x,dy=p->position.y-local.position.y;
        const u8 alpha=multiplayer::player_proximity_alpha(dx,dy);
        if(alpha==255)continue;
        if(id==p->focus_animation)return alpha;
        for(const auto& option:p->options)for(auto animation:option.animations)if(animation&&animation==id)return alpha;
    }
    return 255;
}

i32 World::draw_player(Player* player){
    auto* pilot=pilot_for(player);
    if(!pilot||!pilot->player)return 1;
    Draw environment(*this,*pilot);
    Player copy;Player* draw=player;
    if(high_refresh::render_only){
        copy=*player;draw=&copy;engine.present(copy.animation,player->animation);
        const auto& previous=pilot->presentation;
        if(high_refresh::active&&previous.valid&&previous.state==player->state){
            const float dx=player->position.x-previous.position.x;
            const float dy=player->position.y-previous.position.y;
            if(dx*dx+dy*dy<16384.0f)
                copy.position={high_refresh::lerp_world(previous.position.x,player->position.x),
                               high_refresh::lerp_world(previous.position.y,player->position.y),
                               high_refresh::lerp_world(previous.position.z,player->position.z)};
        }
    }
    // These alpha changes belong to presentation only. Restore the native VM
    // after drawing so rollback snapshots never retain a display decision.
    u32 alpha=player->state==3?0x50u:0xffu;
    if(player->state==3){
        u32 rescue_ticks=0;
        for(u32 seat=0;seat<player_count;++seat){
            const auto& rescue=cooperation.seats[seat];
            if(rescue.rescueTarget==static_cast<std::int8_t>(pilot->seat)&&rescue.rescueTicks>rescue_ticks)
                rescue_ticks=rescue.rescueTicks;
        }
        if(rescue_ticks){
            const u32 capped=std::min<u32>(rescue_ticks,multiplayer::kRescueTicks);
            alpha=0x50u+(0xafu*capped)/multiplayer::kRescueTicks;
        }
    }
    if(!state.netplay_runtime.Spectator()&&!state.netplay_runtime.Playback()&&
       pilot->seat!=local_player&&local_player<player_count&&pilots[local_player].player){
        const auto& local=pilots[local_player].player->position;
        const float dx=player->position.x-local.x,dy=player->position.y-local.y;
        const u32 overlap=multiplayer::player_proximity_alpha(dx,dy);
        if(overlap<alpha)alpha=overlap;
    }
    const u32 color=draw->animation.color,secondary=draw->animation.secondary_color;
    if(alpha<0xffu){
        auto clamp_alpha=[alpha](u32 value){
            const u32 current=value>>24;
            return (value&0x00ffffffu)|((current<alpha?current:alpha)<<24);
        };
        draw->animation.color=clamp_alpha(color);
        draw->animation.secondary_color=clamp_alpha(secondary);
    }
    const i32 result=draw->draw(environment);
    draw->animation.color=color;
    draw->animation.secondary_color=secondary;
    if(!high_refresh::render_only&&engine.enhance_local_player_visibility&&!state.netplay_runtime.Spectator()&&
       !state.netplay_runtime.Playback()&&pilot->seat==local_player&&common.value){
        auto& text=*common.value;const auto saved_color=text.color;const auto scale=text.scale;
        const auto camera=text.camera,shadow=text.shadow;
        char label[8];std::snprintf(label,sizeof(label),"P%u",pilot->seat+1);
        text.color=0xfff3eee4;text.scale={1,1};text.camera=0;text.shadow=1;
        text.queue(label,{draw->position.x+239.f,draw->position.y+10.f,.47f},false);
        text.color=saved_color;text.scale=scale;text.camera=camera;text.shadow=shadow;
    }
    const auto& rescue=cooperation.seats[pilot->seat];
    if(!high_refresh::render_only&&common.value&&rescue.rescueTarget>=0&&rescue.rescueTicks){
        auto& text=*common.value;
        const auto scale=text.scale;const auto color=text.color;
        const auto camera=text.camera,shadow=text.shadow;
        const u32 ticks=rescue.rescueTicks<multiplayer::kRescueTicks?rescue.rescueTicks:multiplayer::kRescueTicks;
        char label[32];std::snprintf(label,sizeof(label),"%u%%",ticks*100u/multiplayer::kRescueTicks);
        text.scale={1,1};text.color=0xffd5efc8;text.camera=0;text.shadow=1;
        text.queue(label,{draw->position.x+194.0f,draw->position.y-10.0f,.47f},false);
        text.scale=scale;text.color=color;text.camera=camera;text.shadow=shadow;
    }else if(!high_refresh::render_only&&common.value&&multiplayer::PowerTapProgress(rescue)>=3){
        auto& text=*common.value;
        const auto scale=text.scale;const auto color=text.color;
        const auto camera=text.camera,shadow=text.shadow;
        char label[16];std::snprintf(label,sizeof(label),"P %u/5",u32(multiplayer::PowerTapProgress(rescue)));
        text.scale={1,1};text.color=0xffe2edbd;text.camera=0;text.shadow=1;
        text.queue(label,{draw->position.x+222.0f,draw->position.y-10.0f,.47f},false);
        text.scale=scale;text.color=color;text.camera=camera;text.shadow=shadow;
    }
    return result;
}

void World::hit_player(){
    auto* pilot=pilot_for(actors.player);
    if(!pilot||!pilot->player||pilot->player->state==3)return;
    Lifecycle environment(*this,*pilot);pilot->player->hit(environment);
}

i32 World::player_damage(const Vec3& point,const Vec2& size){
    i32 total=0;
    for(u32 seat=0;seat<player_count;++seat){
        auto& pilot=pilots[seat];
        if(!pilot.player||pilot.player->state==3)continue;
        Damage environment(*this,pilot);
        i32 damage=pilot.player->damage(point,size,environment);
        if(pilot.player->state==2||pilot.player->state==0)damage/=5;
        total=wrapping_add(total,damage);
    }
    return total;
}

i32 World::collide_player(const Vec3& point,const Vec2& size){
    const multiplayer::PlayerCollisionBroadphase broadphase(point.x,point.y,size.x,size.y);
    return collide_one_pilot(*this,point,[&](Player& player,Damage& environment){
        return player.collide_rectangle(point,size,environment);
    },[&](const Player& player){
        return broadphase.MayOverlap(player.collision_bounds.minimum.x,player.collision_bounds.minimum.y,
                                     player.collision_bounds.maximum.x,player.collision_bounds.maximum.y);
    });
}

i32 World::collide_player_laser(const Vec3& point,float angle,float width,float length){
    return collide_one_pilot(*this,point,[&](Player& player,Damage& environment){
        return player.collide_laser(point,angle,width,length,environment);
    },[](const Player&){return true;});
}

bool World::create_bomb(){
    for(u32 offset=0;offset<player_count;++offset){
        const u32 seat=player_count-1-offset;
        auto& pilot=pilots[seat];
        if(!pilot.player)goto fail_bombs;
        BombResources resources(*this,pilot);
        if(!GameObjectResources{resources}.create(GameObjectKind::Bomb))goto fail_bombs;
    }
    actors.bomb=pilots[0].bomb;
    return true;
fail_bombs:
    for(u32 seat=0;seat<player_count;++seat){
        auto& pilot=pilots[seat];
        if(!pilot.bomb)continue;
        BombResources resources(*this,pilot);
        GameObjectResources{resources}.shutdown(*pilot.bomb);
        std::free(pilot.bomb);pilot.bomb=nullptr;
    }
    actors.bomb=nullptr;
    fail();return false;
}

void World::destroy_bomb(Bomb*){
    for(u32 seat=0;seat<player_count;++seat){
        auto& pilot=pilots[seat];
        if(!pilot.bomb)continue;
        BombResources resources(*this,pilot);
        GameObjectResources{resources}.shutdown(*pilot.bomb);
        std::free(pilot.bomb);pilot.bomb=nullptr;
    }
    actors.bomb=nullptr;
}

i32 World::start_bomb(multiplayer::Pilot& pilot){
    if(!pilot.player||pilot.player->state==3||!pilot.bomb)return -1;
    BombServices environment(*this,pilot);return pilot.bomb->start(environment);
}

i32 World::start_bomb(){
    auto* pilot=pilot_for(actors.player);return pilot?start_bomb(*pilot):-1;
}

i32 World::update_bomb(Bomb* bomb){
    auto* pilot=pilot_for(bomb);
    if(!pilot||!pilot->bomb)return 1;
    BombServices environment(*this,*pilot);return pilot->bomb->update(environment);
}

i32 World::update_bomb(){return update_bomb(actors.bomb);}

u32 World::boss_participant_count()const{
    u32 count=0;for(u32 seat=0;seat<player_count;++seat)
        if(pilots[seat].player&&pilots[seat].player->state!=3&&
           cooperation.seats[seat].lifeState!=multiplayer::LifeState::Eliminated)++count;
    return count;
}
i32 World::bomb_damage(multiplayer::Pilot& pilot,const Vec3& target){
    if(!pilot.bomb)return 0;
    const bool boss_active=actors.enemies&&actors.enemies->bosses[0];
    return multiplayer::bomb_damage(pilot.bomb->damage(target,{actors.spell->spell_flags,pilot.game.character,boss_active}),player_count);
}

i32 World::bomb_damage(const Vec3& target){
    auto* pilot=pilot_for(actors.player);return pilot?bomb_damage(*pilot,target):0;
}

void World::update_cooperation(){
    if(cooperation.seatCount!=player_count)return;
    multiplayer::FrameInput input_frame{};
    for(u32 seat=0;seat<player_count;++seat){
        auto& pilot=pilots[seat];
        if(!pilot.player){fail();return;}
        const auto& player=*pilot.player;
        const auto life_state=player.state==3?multiplayer::LifeState::Spirit:
            player.state==2&&pilot.game.lives<0?multiplayer::LifeState::Dying:
                                                   multiplayer::LifeState::Alive;
        if(!multiplayer::ReportNativeSeatOutcome(
               cooperation,static_cast<u8>(seat),life_state,
               static_cast<std::int16_t>(pilot.game.lives),pilot.game.power)){
            fail();return;
        }
        auto& controls=input_frame.seats[seat];
        controls.x=player.fixed_position.x;
        controls.y=player.fixed_position.y;
        controls.canInitiateLifeTransfer=player.state==1;
        controls.canReceiveLifeTransfer=player.state==1&&pilot.game.lives<9;
        controls.canReceivePowerTransfer=player.state==1&&
            pilot.game.power<=multiplayer::kMaxPowerTransferRecipient;
        controls.focus=(pilot.input_keys&4u)!=0;
        controls.shoot=(pilot.input_keys&1u)!=0;
        controls.shootPressed=(state.input_lanes.seats[seat].pressed&multiplayer::InputLanes::kShoot)!=0;
    }
    const auto result=multiplayer::AdvanceOneTick(
        cooperation,input_frame,{this,allocate_life_item},{this,allocate_power_item},
        {this,[](void* raw,std::uint8_t giver,std::uint8_t target) noexcept {
            return static_cast<World*>(raw)->spawn_rescue_power(giver,target);
        }});
    for(u32 i=0;i<result.eventCount;++i){
        const auto& event=result.events[i];
        if(event.kind==multiplayer::EventKind::SpiritRevived){
            auto& donor=pilots[event.seat];
            donor.game.lives=event.giverLivesAfter;
            donor.game.power=cooperation.seats[event.seat].power;
            configure_player(donor);
            if(donor.seat==0&&hud)hud->update_power(donor.game.power/20,(donor.game.power%20)*100/20);
            if(event.targetSeat<0||static_cast<u32>(event.targetSeat)>=player_count){
                fail();return;
            }
            revive_player(*this,pilots[static_cast<u32>(event.targetSeat)],
                          event.targetLivesAfter);
            if(donor.seat==0&&actors.gui)
                actors.gui->update_lives(donor.game.lives);
        }else if(event.kind==multiplayer::EventKind::LifeItemTransferCommitted){
            auto& donor=pilots[event.seat];
            donor.game.lives=event.giverLivesAfter;
            if(donor.seat==0&&actors.gui)
                actors.gui->update_lives(donor.game.lives);
        }else if(event.kind==multiplayer::EventKind::PowerItemTransferCommitted){
            auto& donor=pilots[event.seat];
            donor.game.power=cooperation.seats[event.seat].power;
            configure_player(donor);
            if(donor.seat==0&&hud)
                hud->update_power(donor.game.power/20,(donor.game.power%20)*100/20);
        }else if(event.kind==multiplayer::EventKind::WipeRetryRequested){
            // Multiplayer never enters TH10's Continue/Retry path.  Once the
            // 180-tick wipe grace period expires, finish the run through the
            // same native Game Over results flow used by ordinary play.
            show_results(false);
        }
    }
}

void World::award_team_life(){
    multiplayer::State next=cooperation;
    if(!refresh_cooperation_from_native(*this,next)){
        fail();return;
    }
    bool increased=false;
    for(u32 seat=0;seat<player_count;++seat){
        const auto before=next.seats[seat].lives;
        if(!multiplayer::ApplyLifeAward(next,static_cast<u8>(seat),1,
                                        next.seats[seat].lifeState)){
            fail();return;
        }
        increased|=next.seats[seat].lives>before;
    }

    cooperation=next;
    for(u32 seat=0;seat<player_count;++seat){
        auto& pilot=pilots[seat];
        pilot.game.lives=cooperation.seats[seat].lives;
    }
    if(pilots[0].player&&actors.gui)
        actors.gui->update_lives(pilots[0].game.lives);
    if(increased){
        sound(0x2c);
        if(hud&&actors.gui)hud->notify(0x4b,0);
    }
}
} // namespace th10::browser
