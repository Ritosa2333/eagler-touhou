#pragma once
#include "AnmLayout.hpp"
#include "GameValues.hpp"
#include "PresentationVisual.hpp"
#include <array>
namespace th08 {
struct ItemState {
    AnmVm animation;Vec3 position,velocity,target;Timer timer;
    i8 type=0,active=0,onscreen=0,state=0,max_value=0;u8 padding[3]{};ItemState* next=nullptr;ItemState* previous=nullptr;
};
static_assert(sizeof(ItemState)==0x2e4&&offsetof(ItemState,type)==0x2d4);
struct ItemPoolState {
    static constexpr u32 capacity=2096;ItemState items[capacity+1];i32 next_index=0;u32 count=0;ItemState head;ItemState* tail=nullptr;
    void reset(){std::memset(this,0,sizeof(*this));tail=&head;}
};
static_assert(sizeof(ItemPoolState)==0x17b094&&offsetof(ItemPoolState,next_index)==0x17ada4);
struct ItemPoolActions {
    virtual ~ItemPoolActions()=default;
    virtual void animation(AnmVm&,i32 script)=0;
    virtual void sprite(AnmVm&,i32 index)=0;
    virtual void effect(i32 kind,const Vec3& position,i32 count,u32 color)=0;
    virtual void draw(AnmVm&)=0;
};
class ItemPool {
    ItemPoolState& state;Rng& rng;ItemPoolActions& actions;
    struct PresentationSample {Vec3 position{};i32 age=0;i8 type=0;bool active=false;presentation::VisualSample visual;};
    std::array<PresentationSample,ItemPoolState::capacity+1> previous{};
    presentation::SnapshotMarker presentation_marker;
public:
    ItemPool(ItemPoolState& s,Rng& r,ItemPoolActions& a):state(s),rng(r),actions(a){}
    ItemState* spawn(const Vec3& position,i32 type,i32 mode,i32 power,i8 player_state);
    void remove(ItemState&);
    void collect_all();
    void cancel_homing();
    void convert_power(ItemState* except);
    i32 time_orb_count()const;
    void snapshot();
    void draw(const Vec2& offset);
};
void point_item_extend_threshold(GameGlobals&,i32 difficulty)noexcept;
}
