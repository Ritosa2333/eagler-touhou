#include "../game/CallbackNames.hpp"
#include "World.hpp"
#include "../game/EnemyCommands.hpp"
#include "../game/EnemySystems.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../game/PracticeRuntime.hpp"
#endif
#include <cstdlib>
namespace th10::browser {
namespace {
struct Frame final:EnemyFrameEnvironment,EnemyDropEnvironment {
    World& w;explicit Frame(World& world):w(world){
        static const Vec2 tangent2{};script_rng=drop_rng=&w.engine.script_random;items=this;economy=&w.state.game;timer_rate=default_rate=&w.engine.speed;player_position=&w.actors.player->position;boss=w.actors.enemies?w.actors.enemies->bosses[0]:nullptr;rank=&w.state.game.rank;difficulty=&w.state.game.difficulty;default_tangent=&w.engine.tangent;default_tangent2=&tangent2;camera_delta=&w.engine.world.animation_delta;
        registry=&w.engine.manager.registry;started_animations=&w.engine.manager.started_scripts;animations=&w.engine;scripts=nullptr;
        auto& spell=*w.actors.spell;auto& gui=*w.actors.gui;auto& player=*w.actors.player;
        phase.default_rate=default_rate;phase.countdown=&gui.countdown;phase.item_value=&w.state.game.item_value;phase.spell_flags=&spell.spell_flags;phase.spell_elapsed=&spell.elapsed.current;phase.spell_bonus=&spell.bonus;
        const u32 indices[]={0,1,3,4,5,6,7};for(u32 i=0;i<7;++i)phase.spell_animation_flags[i]=&spell.bonus_digits[indices[i]].flags;
        alternate_active=&w.actors.bomb->active;player_state=&player.state;player_target=&player.target;player_target_seen=&player.target_seen;score=&w.state.game.score;enemy_activity=&w.state.game.enemy_activity;boss_hp_flags=&gui.enemy_marker.flags;boss_slots=w.actors.enemies->bosses;health_bars=reinterpret_cast<EnemyHealthBar*>(gui.boss_health);boss_lives=&gui.boss_lives;message_status=gui.dialogue?reinterpret_cast<const u32*>(&gui.dialogue->blocking_frames):nullptr;
    }
    AnmVm* animation(u32& id) override{return registry->find_and_clear(id);}
    i32 player_damage(const Vec3& p,const Vec2& size) override{return w.player_damage(p,size);}
    void player_collision(const Vec3& p,const Vec2& size) override{w.collide_player(p,size);}
    i32 destroy(EnemyState& enemy) override{return enemy.destroy(*this);}
    void play_sound(i32 id,float x) override{w.sound(id,x);}
    void spawn_item(const Vec3& p,i32 kind,i32 color,float angle,float speed) override{w.spawn_item(p,kind,color,angle,speed);}
    void spawn_death_animation(i32 file,i32 script,const Vec3& p) override{w.effect(*w.actors.enemies->animation_files[file],script,p);}
#ifdef TH_ENABLE_THPRAC
    // F4 time lock (0x40e5b0): hold the enemy lifetime counter.
    bool time_locked() const override{return practice_time_lock(w.state.practice);}
#endif
};
struct Script final:EclServices {
    World& w;Enemy* owner;Script(World& world,Enemy* enemy):w(world),owner(enemy){}
    i32 integer(i32 id) override{if(!owner)__builtin_trap();Frame env(w);return EnemyVariables(owner->state,env).integer(id);}
    Extended floating(i32 id) override{if(!owner)__builtin_trap();Frame env(w);return EnemyVariables(owner->state,env).floating(id);}
    i32* integer_reference(i32 id) override{if(!owner)__builtin_trap();Frame env(w);return EnemyVariables(owner->state,env).integer_reference(id);}
    float* float_reference(i32 id) override{if(!owner)__builtin_trap();Frame env(w);return EnemyVariables(owner->state,env).float_reference(id);}
    i32 command(EclContext& context) override{if(!owner)__builtin_trap();return w.enemy_command(owner->state,context,*this);}
    void* allocate(u32 size) override{return std::malloc(size);}void release(void* p) override{std::free(p);}
#ifdef TH_ENABLE_THPRAC
    // thprac_th10.cpp:527-547. The upstream EHOOK at 0x44fb9f freezes the ECL
    // sub-time for the stage 1/2/4 main enemy while a boss exists, and jumps
    // the midboss wait to its post-midboss value. STAGE_NUM is one-based here.
    bool hold_time(EclContext& context,float&) override{
        if(!owner)return false;
        auto& p=w.state.practice;
        if(!practice_time_lock(p))return false;
        const i32 stage=w.state.game.stage-1;
        if(stage<0||(stage>=2&&stage!=3))return false;
        // GetMemContent(context+0x1014, 0x103c+0x1444): the owner EnemyState.flags.
        if(owner->state.flags!=0x510u)return false;
        const bool boss_exists=w.actors.enemies&&w.actors.enemies->bosses[0]!=nullptr;
        const float cur_time=context.time;
        if(boss_exists&&cur_time){
            constexpr float main_time_mid[4]={2200.0f,2200.0f,0.0f,3600.0f};
            constexpr float main_time_post_mid[4]={2600.0f,2900.0f,0.0f,4100.0f};
            if(cur_time>=main_time_mid[stage]&&cur_time<main_time_post_mid[stage])context.time=main_time_post_mid[stage];
            return true;
        }
        return false;
    }
#endif
};
struct Enemies final:EnemyManagerEnvironment {
    World& w;Script cleanup;explicit Enemies(World& world):w(world),cleanup(world,nullptr){rate=&w.engine.speed;difficulty=&w.state.game.difficulty;registry=&w.engine.manager.registry;player=w.actors.player;scripts=&cleanup;enemy_type_table=reinterpret_cast<void*>(1);script_type_table=reinterpret_cast<void*>(2);}
    Enemy* allocate_enemy() override{return static_cast<Enemy*>(std::malloc(sizeof(Enemy)));}
    void release_enemy(Enemy* enemy) override{std::free(enemy);}
    i32 update_enemy(Enemy& enemy) override{return w.update_enemy(enemy);}
    void destroy_enemy(Enemy& enemy) override{enemy.shutdown(*w.actors.enemies,*this);std::free(&enemy);}
    void spawn_death_effect(const EnemyState& enemy) override{w.effect(*w.actors.enemies->animation_files[enemy.death_animation_file],enemy.death_animation,enemy.current.position);}
};
struct Scripts final:EnemyScriptResourceEnvironment {
    World& w;explicit Scripts(World& world):w(world){enemy_animations=w.actors.enemies?w.actors.enemies->animation_files:nullptr;}
    void* allocate(u32 size) override{return std::malloc(size);}void release(void* p) override{std::free(p);}
    u8* read_file(const char* name) override{return static_cast<u8*>(w.read_file(name));}
    AnmFile* load_animation(u32 slot,const char* name) override{return w.engine.manager.load(slot,name,w.engine.resources);}
    void missing_animation() override{w.fail();}
};
struct Resources final:EnemySystemsEnvironment {
    World& w;Enemies actors;Scripts files;explicit Resources(World& world):w(world),actors(world),files(world){chain=&w.engine.chain_value;callbacks=&w.engine.callback_environment;enemies=&actors;resources=&files;active_enemies=&w.actors.enemies;bullet_animation_file=w.actors.bullets?w.actors.bullets->animation_file:nullptr;animation_slots=w.engine.manager.files;game_flags=&w.state.game.flags;rate=&w.engine.speed;program_type_table=reinterpret_cast<void*>(3);generic_program_type_table=reinterpret_cast<void*>(4);update_callback=callback_id::EnemiesUpdate;draw_callback=callback_id::EnemiesDraw;}
    void* allocate(u32 size) override{return std::malloc(size);}void release(void* p) override{std::free(p);}
    void discard_file_animations(AnmFile* file) override{w.engine.manager.registry.discard_file(file);}
    void unload_animation(AnmFile& file) override{file.release(w.engine.resources);}
};
struct Projectiles final:EnemyProjectileEnvironment {
    World& w;explicit Projectiles(World& world):w(world){player_position=&w.actors.player->position;rank=&w.state.game.rank;difficulty=&w.state.game.difficulty;}
    void fire(const BulletEmitter& emitter) override{w.fire(emitter);}
    u32 spawn_straight(const StraightLaserParameters& p) override{return w.create_laser(0,&p);}
    u32 spawn_timed(const TimedLaserParameters& p) override{return w.create_laser(1,&p);}
    EnemyLaser* laser(u32 id) override{return w.actors.lasers->find(id);}
    void cancel_rectangle(bool convert) override{w.cancel_bullet_rectangle(convert);w.cancel_lasers(convert);}
    void cancel_circle(const Vec3& p,float radius,bool convert) override{w.cancel_bullet_circle(p,radius,convert,false);w.cancel_laser_circle(p,radius,convert);}
};
struct Spawning final:EnemySpawnEnvironment {
    World& w;explicit Spawning(World& world):w(world){boss=w.actors.enemies->bosses;camera_origin=&w.engine.world.position;}
    void spawn(const char* name,const EnemySpawnParameters& p) override{w.spawn_enemy(name,p);}
    Vec3 project_world(const Vec3& p) override{w.engine.active=&w.engine.world;w.engine.configure_camera(false);w.engine.screen_space=0;Vec3 out;auto& camera=w.engine.world;Matrix4 identity{};for(u32 i=0;i<4;++i)identity.elements[i][i]=1;GraphicsMath::project(out,p,&camera.viewport,&camera.projection,&camera.view,&identity);return out;}
};
struct Scene final:EnemySceneEnvironment {
    World& w;explicit Scene(World& world):w(world){difficulty=&w.state.game.difficulty;game=&w.state.game;registry=&w.engine.manager.registry;spell_flags=&w.actors.spell->spell_flags;spell_bonus_animation=&w.actors.spell->circle_animation;}
    void screen_effect(i32 a,i32 b,i32 c) override{ScreenEffect::create(ScreenEffectKind::ShakeLinear,a,b,c,0,49,w.effects);}
    void start_dialogue(i32 id) override{w.hud->start_dialogue(id);}
    void cancel_projectiles() override{w.cancel_bullet_rectangle(0);w.cancel_lasers(0);}
    void clear_enemies() override{w.clear_enemies(false);}
    void start_spell(i32 id,i32 name_id,const char* name,i32 frames) override{w.start_spell(id,name_id,name,frames);}
    void end_spell() override{w.finish_spell();}
    void delete_lasers() override{w.actors.lasers->schedule_deletion();}
};
}
bool World::create_enemies(){Resources env(*this);return EnemyManager::create(reinterpret_cast<const char*>(state.current_stage->resources[3]),env)!=nullptr;}
void World::destroy_enemies(EnemyManager* p){Resources env(*this);p->shutdown(env);std::free(p);}
void World::clear_enemies(bool all){if(all){Resources env(*this);actors.enemies->clear_all(env);}else{Enemies env(*this);actors.enemies->clear_enemies(env);}}
i32 World::update_enemies(){Enemies env(*this);return actors.enemies->update(env);}
Enemy* World::spawn_enemy(const char* name,const EnemySpawnParameters& p){Enemies env(*this);return actors.enemies->spawn(name,p,env);}
void World::destroy_enemy(Enemy& enemy){Enemies env(*this);env.destroy_enemy(enemy);}
i32 World::update_enemy(Enemy& enemy){Frame env(*this);Script scripts(*this,&enemy);env.scripts=&scripts;return enemy.state.update(env);}
i32 World::enemy_command(EnemyState& enemy,EclContext& context,EclGlobals& globals){Frame frame(*this);Projectiles projectiles(*this);Spawning spawning(*this);Scene scene(*this);EnemyAnimationEnvironment animations{&engine.manager,actors.enemies->animation_files,&engine,&engine};EnemyCommandEnvironment env{frame,projectiles,animations,spawning,scene};return enemy.execute_command(context,globals,env);}
}
