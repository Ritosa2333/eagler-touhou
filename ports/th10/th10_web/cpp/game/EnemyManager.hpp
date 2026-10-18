#pragma once
#include "Enemy.hpp"
#include "AnmRegistry.hpp"
#include "UpdateChain.hpp"
namespace th10 {
struct Player;
struct EnemySystemsEnvironment;
struct EnemySpawnParameters {
    Vec3 position;
    i32 score,drop_kind,health,mirror;
    u32 flags;
    i32 integer_variables[4];
    float float_variables[4];
};
static_assert(sizeof(EnemySpawnParameters)==0x40);
struct EnemyManagerEnvironment {
    const float* rate;
    const i32* difficulty;
    AnmRegistry* registry;
    Player* player;
    EclServices* scripts;
    // Development object layout metadata, not executable callbacks.
    void* enemy_type_table;
    void* script_type_table;
    virtual Enemy* allocate_enemy()=0;
    virtual void release_enemy(Enemy* enemy)=0;
    virtual i32 update_enemy(Enemy& enemy)=0;
    virtual void destroy_enemy(Enemy& enemy)=0;
    virtual void spawn_death_effect(const EnemyState& enemy)=0;
};
struct EnemyManager {
    u32 flags,reserved_004;
    UpdateChainEntry* update_handle;
    UpdateChainEntry* draw_handle;
    Enemy* bosses[8];
    AnmFile* animation_files[4];
    Timer lifetime;
    u32 lifetime_flags;
    EclProgram* program;
    ListNode<Enemy>* head;
    ListNode<Enemy>* tail;
    i32 count;
    u32 spawn_count;
    void initialize() noexcept;
    Enemy* spawn(const char* subroutine,const EnemySpawnParameters& parameters,EnemyManagerEnvironment& environment);
    i32 update(EnemyManagerEnvironment& environment);
    void clear_enemies(EnemyManagerEnvironment& environment);
    i32 start(const char* script,EnemySystemsEnvironment& environment);
    void activate(bool enabled) noexcept;
    void clear_all(EnemySystemsEnvironment& environment);
    void shutdown(EnemySystemsEnvironment& environment);
    static EnemyManager* create(const char* script,EnemySystemsEnvironment& environment);
};
static_assert(sizeof(EnemyManager)==0x68);
static_assert(offsetof(EnemyManager,program)==0x54);
}
