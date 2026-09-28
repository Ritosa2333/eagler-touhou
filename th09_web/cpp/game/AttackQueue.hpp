#pragma once
#include "AttackQueueActions.hpp"
#include "AnmLayout.hpp"
#include "StateChecksum.hpp"
#include "PoolCheckpoint.hpp"
#include <array>
#include <memory>
#include <vector>
namespace th09 {
struct AttackActor;struct AttackServices;
struct AttackState {
    virtual ~AttackState()=default;
    virtual std::unique_ptr<AttackState> clone()const=0;
    virtual u32 checksum()const=0;
    virtual const void* type_key()const=0;
    virtual void copy_into(std::unique_ptr<AttackState>&)const=0;
};
// Each concrete state supplies its own copy operation. Checkpoints preserve
// the dynamic type without copying ownership words or requiring RTTI.
template<class T,class Base=AttackState>struct CopyableAttackState:Base {
    inline static const char key=0;
    const void* type_key()const override{return &key;}
    void copy_into(std::unique_ptr<AttackState>& out)const override{
        if(out&&out->type_key()==type_key())static_cast<T&>(*out)=static_cast<const T&>(*this);
        else out=clone();
    }
    u32 checksum()const override{return static_cast<const T&>(*this).state_checksum();}
    std::unique_ptr<AttackState> clone()const override{return std::make_unique<T>(static_cast<const T&>(*this));}
};
// These point to authored C++ routines. Executable addresses exist only in the
// development comparison harness, never in the game dispatch table.
struct AttackBehavior {
    bool (*initialize)(AttackActor&,AttackServices&)=nullptr;
    bool (*update)(AttackActor&,AttackServices&)=nullptr;
    void (*draw)(AttackActor&,AttackServices&)=nullptr;
    void (*dispose)(AttackActor&,AttackServices&)=nullptr;
};
struct AttackActor {
    i32 layer=0,destination_side=0,source_side=0,active=0;
    Timer time{0,0,0};Vec3 position;union {u32 color=0;float angle;};
    std::vector<AnmVm> animations;std::unique_ptr<AttackState> state;
    const AttackBehavior* behavior=nullptr;
    // Init-time arguments have distinct types: most attacks need no extra
    // data, directional attacks receive a point, and split attacks a parent.
    const Vec3* extra_position=nullptr;const AttackActor* parent=nullptr;
};
struct AttackServices {
    virtual ~AttackServices()=default;
    virtual void play_sound(i32 sound,i32 pan)=0;
    virtual void advance_animation(AnmVm&)=0;
};
class AttackQueue:public AttackQueueActions {
    const std::array<AttackBehavior,27>& behaviors;AttackServices& actions;
    void dispose(AttackActor&);
public:
    static constexpr u32 capacity=256;
    PoolCheckpoint* checkpoint=nullptr;
    void before_write(AttackActor& a){if(checkpoint)checkpoint->BeforeAttack(*this,&a-actors.data());}
    std::array<AttackActor,capacity+1> actors;
    std::array<std::vector<AttackActor*>,3> draw_lists;
    i32 counts[2]{},limits[2]{};
    bool invalid=false;
    AttackQueue(const std::array<AttackBehavior,27>&,AttackServices&);
    AttackActor* create(i32 kind,i32 source_side,const Vec3&,const Vec3* extra=nullptr,const AttackActor* parent=nullptr);
    void queue_attack(i32 kind,i32 side,const Vec3& p,const Vec3* extra)override{create(kind,side,p,extra);}
    bool update(const FrameTiming&,u32 game_flags,u32 first_playfield_flags);
    void clear();
};
}
