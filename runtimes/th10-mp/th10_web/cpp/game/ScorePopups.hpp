#pragma once
#include "Player.hpp"
namespace th10 {
struct ScorePopupEnvironment;
struct ScorePopup {
    u8 digits[12];
    Vec3 position;
    u32 color;
    Timer elapsed;
    u32 timer_flags,reserved_030[2];
    u8 active,length,reserved_03a[6];
};
static_assert(sizeof(ScorePopup)==64);
struct ScorePopups {
    u32 flags,manager_state;
    UpdateChainEntry *update_entry,*draw_entry;
    AnmFile* animation_file;
    i32 cursor;
    AnmVm animation;
    ScorePopup pool[723];
    void initialize(ScorePopups** current) noexcept;
    i32 start(ScorePopupEnvironment& environment);
    void shutdown(ScorePopupEnvironment& environment);
    static ScorePopups* create(ScorePopupEnvironment& environment);
    void spawn(i32 score,const Vec3& position,u32 color,const float* rate) noexcept;
    i32 update(const float* rate) noexcept;
    i32 draw(ScorePopupEnvironment& environment);
};
static_assert(offsetof(ScorePopups,pool)==0x3c4 && sizeof(ScorePopups)==0xb884);
struct ScorePopupEnvironment {
    ScorePopups** current;
    AnmFile** effects;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callback,draw_callback;
    Player** player;
    const u32* display_flags;
    u32* fog;
    virtual ScorePopups* allocate()=0;
    virtual void delete_object(void* object)=0;
    virtual void free_bytes(void* memory)=0;
    virtual void flush()=0;
    virtual void disable_fog()=0;
    virtual void draw_animation(AnmVm& vm)=0;
    virtual bool presentation(const ScorePopup& popup,Vec3& position,float& elapsed)=0;
};
}
