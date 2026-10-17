#pragma once
#include "StageRuntime.hpp"
#include "EclVm.hpp"
#include "PlayerRuntime.hpp"
#include "EnemyPopulation.hpp"
#include "AnmLibrary.hpp"
#include "BulletUpdate.hpp"
#include "BulletCancel.hpp"
#include "LaserRuntime.hpp"
#include "PlayerShotSystem.hpp"
#include "EclSpawn.hpp"
#include "EffectSystem.hpp"

namespace th08 {
class GameRuntime:private TimelineActions,private BulletEmissionActions,private EnemyPhaseActions,private EnemySpawnActions,private LaserEmissionActions,private BulletCreationActions,private BulletUpdateActions,private BulletCancelActions {
    Rng random;FrameTiming timing;EclGlobals globals;CombatRuntime combat;EclExecutor executor;EclTimelineExecutor timeline_executor;PlayerRuntime player;
    GameGlobals values;EclProgram ecl;EnemyPopulation enemies{executor,ecl};StageRuntime stage{random};EclVm enemy_vm;EclTimelineVm timeline_vm;
    TextureStore enemy_textures;AnmLibrary enemy_animations{enemy_textures};
    PlayerShotSystem player_shots{enemy_animations,random};
    std::unique_ptr<BulletManagerState> projectiles=std::make_unique<BulletManagerState>();
    BulletCreation bullet_creation{*projectiles,random};LaserRuntime laser_runtime{*projectiles,random};
    struct ProjectileRequest {u32 kind;Vec3 position;i32 value,mode;};std::vector<ProjectileRequest> projectile_requests;
    BulletUpdateActions* projectile_collision_actions=nullptr;LaserActions* laser_collision_actions=nullptr;
    bool projectiles_loaded=false,projectile_failure=false;u16 projectile_replay_flags=0;
    struct SceneRequest {u32 kind;i32 value;};std::vector<SceneRequest> requests;
    std::vector<BulletEmission> emitted_bullets;
    void spawn(const TimelineSpawn& request)override{enemies.spawn(request);}
    EffectSystem* enemy_effects=nullptr;
    EnemySpawnResult spawn(const TimelineSpawn& request,const EclContext::Locals& locals)override{auto* enemy=enemies.spawn_inherited(request,locals);return {enemy,enemies.spawn_failed};}
    AnmVm* overlay(i32 kind,const Vec3& position,i32 count,u32 color)override{if(!enemy_effects){projectile_failure=true;return nullptr;}return enemy_effects->overlay(kind,position,count,color);}
    void panned_sound(i32 index,float x)override{sound(index,x,true);}
    void effect(i32 kind,const Vec3& p,i32 count,u32 color)override{if(enemy_effects)enemy_effects->spawn(kind,p,count,color);else projectile_failure=true;}
    void sound(i32 index,i32 mode)override{projectile_requests.push_back({4,{},index,mode});}
    void message(i32 entry)override{requests.push_back({1,entry});}
    void set_power(i32 value)override{requests.push_back({2,value});}
    void emit(BulletEmission& parameters)override;
    void clear(i32 mode)override;
    LaserState* laser(BulletEmission& parameters)override;
    void sound(i32 index,float position,bool panned)override{projectile_requests.push_back({1,{position,0,0},index,i32(panned)});}
    void reemit(BulletEmission& parameters)override{emit(parameters);}
    i32 collision(i32 kind,BulletState& b)override{return projectile_collision_actions?projectile_collision_actions->collision(kind,b):0;}
    i32 barrier(BulletState& b)override{return collision(0,b);}
    void item(const Vec3& position,i32 type)override{drop(position,type,1);}
    void drop(const Vec3& position,i32 type,i32 mode)override{projectile_requests.push_back({2,position,type,mode});}
    void clear_familiars(EclVm& enemy,bool reward)override{requests.push_back({reward?4u:5u,enemy.pool_index});}
    bool enemy_running=false,timeline_running=false,stage_loaded=false,ecl_loaded=false;
    u32 frame=0;
public:
    GameRuntime():executor(random,timing,globals),timeline_executor(random,timing,globals),player(combat){projectiles->reset();bullet_creation.actions=this;globals.combat=&combat;globals.values=&values;globals.timeline_actions=this;globals.bullet_actions=this;globals.phase_actions=this;globals.laser_actions=this;globals.enemy_actions=this;}
    void set_enemy_effects(EffectSystem* effects)noexcept{enemy_effects=effects;}
    void reset() noexcept;
    bool load_ecl(const u8* bytes,u32 size);
    bool load_enemy_animation(i32 slot,const u8* bytes,u32 size);
    bool load_projectile_animation(const u8* bytes,u32 size);
    PlayerShotSystem& player_shot_system()noexcept{return player_shots;}
    const BulletManagerState& projectile_state()const noexcept{return *projectiles;}
    bool projectile_ready()const noexcept{return projectiles_loaded;}
    const void* pending_projectile_requests()const noexcept{return projectile_requests.data();}
    u32 projectile_request_count()const noexcept{return projectile_requests.size();}
    void clear_projectile_requests()noexcept{projectile_requests.clear();}
    void set_projectile_collisions(BulletUpdateActions* bullets,LaserActions* lasers)noexcept{projectile_collision_actions=bullets;laser_collision_actions=lasers;}
    bool load_stage_animation(i32 slot,const u8* bytes,u32 size){return stage.load_animation(slot,bytes,size);}
    bool load_stage(const u8* bytes,u32 size,i32 index=0,bool practice=false);
    StageRuntime& stage_runtime()noexcept{return stage;}
    bool start_timeline(i32 index,u32 difficulty);
    bool start_enemy_subroutine(i32 index);
    bool update(u16 buttons);
    CombatRuntime& combat_runtime() noexcept{return combat;}
    PlayerRuntime& player_runtime() noexcept{return player;}
    const EclGlobals& ecl_globals()const noexcept{return globals;}
    const EnemyPopulation& enemy_population()const noexcept{return enemies;}
    u32 scene_request_count()const noexcept{return requests.size();}
    const void* scene_requests()const noexcept{return requests.data();}
    void clear_scene_requests()noexcept{requests.clear();}
    u32 emission_count()const noexcept{return emitted_bullets.size();}
    const BulletEmission* emissions()const noexcept{return emitted_bullets.data();}
    void clear_emissions()noexcept{emitted_bullets.clear();}
    u32 current_frame()const noexcept{return frame;}
    bool ecl_running()const noexcept{return enemy_running||timeline_running;}
    bool stage_ready()const noexcept{return stage_loaded;}
};
}
