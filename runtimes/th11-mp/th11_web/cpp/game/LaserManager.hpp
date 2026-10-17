#pragma once
#include "LaserState.hpp"
#ifdef TH11_MULTIPLAYER
#include <array>
#include <memory>
namespace Netplay {class RollbackJournal;}
#endif
namespace th11 {
class LaserManager final:public LaserWorld {
public:
    static constexpr u32 capacity=256;
    LaserManager(AnmResource&,AnmEnvironment&,LaserWorld&,u16 file_id=6);
    ~LaserManager();
    LaserManager(const LaserManager&)=delete;LaserManager& operator=(const LaserManager&)=delete;
#ifdef TH11_MULTIPLAYER
    // Fresh per-frame checkpoints only. The manager and external resource /
    // ANM owners must remain alive until all referring checkpoints retire.
    // End detaches the observer even after Begin/Touch failure; the caller
    // must reject a failed journal instead of committing partial capture.
    bool BeginRollbackFrame(Netplay::RollbackJournal&);
    void EndRollbackFrame()noexcept;
    bool RollbackFrameActive()const noexcept{return rollback_journal!=nullptr;}
#endif
    LaserState* first()const noexcept{return sentinel.next;}
    LaserState* last()const noexcept{return tail==&sentinel?nullptr:tail;}
    u32 active_count=0;i32 next_id=0x10000,last_error=0;Vec3 last_center{},last_size{};
    i32 spawn(const LaserLineParameters&);
    i32 spawn(const LaserInfiniteParameters&);
    bool update(bool paused=false,bool frozen=false);
    bool draw(AnmRenderer&,bool paused=false);
    bool update_warning(float angular_delta);
    LaserState* find(i32 id)const noexcept;
    bool cancel_id(i32 id);
    void mark_all()noexcept;
    void clear()noexcept;
    bool cancel_all(bool reward,bool skip_protected);
    i32 cancel_rectangle(Vec3 center,Vec3 size,bool reward);
    i32 cancel_circle(Vec3 center,float radius,u32 rewards,bool skip_protected);
    bool sound(i32 id,float x,bool positional)override{return world.sound(id,x,positional);}
    bool change_appearance(LaserLine&,i32)override;
    bool spawn_line(const LaserLineParameters&)override;
    bool cancel_effect(Vec3 p,i32 script)override{return world.cancel_effect(p,script);}
    bool cancel_reward(Vec3 p)override{return world.cancel_reward(p);}
    bool cancel_shot(Vec3 p,float angle)override{return world.cancel_shot(p,angle);}
    i32 collision(Vec3 p,float angle,float width,float length)override{const auto result=world.collision(p,angle,width,length);
#ifdef TH11_MULTIPLAYER
        if(world.multiplayer_world())player=world.player;
#endif
        return result;}
    i32 warning_collision(Vec3 p,float angle,float width,float length)override{return world.warning_collision(p,angle,width,length);}
    bool cut(LaserLine&,Vec3,Vec3,bool,bool)override;
    bool cut_infinite(LaserInfinite&,Vec3,Vec3,bool,bool)override;
    bool graze_effect(Vec3 p)override{return world.graze_effect(p);}
    bool graze_reward()override{return world.graze_reward();}
    bool graze_register()override{return world.graze_register();}
    bool boss_position(Vec3& p)override{return world.boss_position(p);}
private:
    friend struct LaserRollbackTestAccess;
    LaserState sentinel{};LaserState* tail=&sentinel;
    AnmResource& resource;AnmEnvironment& animations;LaserWorld& world;u16 file_id;
    bool updating=false;
    void sync()noexcept{rate=world.rate;player=world.player;}
    void append(LaserState&)noexcept;
    void release(LaserState&)noexcept;
    i32 issue_id()noexcept;
#ifdef TH11_MULTIPLAYER
    static constexpr std::size_t slot_bytes=sizeof(LaserLine)>sizeof(LaserInfinite)?sizeof(LaserLine):sizeof(LaserInfinite);
    static constexpr std::size_t slot_alignment=alignof(LaserLine)>alignof(LaserInfinite)?alignof(LaserLine):alignof(LaserInfinite);
    struct alignas(slot_alignment) Storage {unsigned char bytes[slot_bytes];};
    // Only occupied is logical state. Physical storage is allocated lazily
    // once and is never freed or relocated while a checkpoint may refer to it.
    std::unique_ptr<Storage[]> rollback_storage;
    std::array<bool,capacity> rollback_occupied{};
    Netplay::RollbackJournal* rollback_journal=nullptr;
    std::array<bool,capacity> rollback_captured{};
    Storage* allocate_rollback_slot()noexcept;
    void preserve_rollback_slot(u32)noexcept;
    u32 rollback_slot(const LaserState&)const noexcept;
#endif
};
}
