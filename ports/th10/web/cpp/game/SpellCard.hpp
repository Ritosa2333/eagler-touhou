#pragma once
#include "Stage.hpp"
#include "GameEconomy.hpp"
namespace th10 {
struct SpellEnvironment;
struct SpellRecord {char name[128];i32 captures,attempts;u32 reserved[2];};
static_assert(sizeof(SpellRecord)==0x90);
enum class SpellAnimationFile {Interface,Effects,Text,Bullets,Boss};
struct SpellCard {
    u32 flags,state;
    UpdateChainEntry* update_entry;
    UpdateChainEntry* background_entry;
    AnmVm backgrounds[2];
    u32 title_animations[3],circle_animation;
    AnmVm bonus_digits[8],record_digits[5];
    Timer elapsed;u32 elapsed_flags;
    char name[64];
    i32 number;
    u32 spell_flags;
    i32 bonus,initial_bonus,duration;
    Vec3 circle_position;
    u32 reserved_37a8;
    UpdateChainEntry* foreground_entry;
    void initialize(SpellCard** current) noexcept;
    i32 attach(SpellEnvironment& environment);
    void release(SpellEnvironment& environment);
    void activate() noexcept;
    static SpellCard* create(SpellEnvironment& environment);
    i32 update(SpellEnvironment& environment);
    i32 draw_backgrounds(SpellEnvironment& environment);
    i32 draw_digits(SpellEnvironment& environment);
    void start(i32 id,i32 name_id,const char* title,i32 frames,SpellEnvironment& environment);
    void finish(SpellEnvironment& environment);
};
static_assert(sizeof(SpellCard)==0x37b0&&offsetof(SpellCard,elapsed)==0x3734);
static_assert(offsetof(SpellCard,bonus_digits)==0x778&&offsetof(SpellCard,record_digits)==0x24d8);
struct SpellEnvironment {
    SpellCard** current;
    Stage** stage;
    GameEconomy* game;
    const i32* replay_mode;
    const Vec3* player_position;
    AnmRegistry* registry;
    float* rate;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,background_callback,foreground_callback;
    u32* notification_animation;
#ifdef TH_ENABLE_THPRAC
    // F4 time lock (thprac_th10.cpp:525, PATCH_HK 0x408D93). When true the
    // spell bonus decay block is skipped while `elapsed` still ticks.
    virtual bool time_locked() const { return false; }
#endif
    virtual SpellCard* allocate()=0;
    virtual void release_memory(void* memory)=0;
    virtual const Vec3& boss_position()=0;
    virtual SpellRecord& record(i32 id,bool combined)=0;
    virtual i32 update_animation(AnmVm& vm)=0;
    virtual void draw_animation(AnmVm& vm)=0;
    virtual void initialize_animation(AnmVm& vm,SpellAnimationFile file,i32 script,bool embedded)=0;
    virtual void bind_digit(AnmVm& vm,i32 sprite)=0;
    virtual u32 create_animation(SpellAnimationFile file,i32 script)=0;
    virtual void draw_name(AnmVm* vm,const char* name)=0;
    virtual void play_sound(i32 id)=0;
    virtual void show_bonus(i32 bonus)=0;
};
}
