#include "../web/cpp/game/ItemUpdate.hpp"
#include <cassert>
#include <memory>
#include <cstdio>
using namespace th08;
// Pool removal is outside the motion/collection seam under test.
void th08::ItemPool::remove(ItemState& item){item.active=0;}
struct PoolActions:ItemPoolActions {
    void animation(AnmVm&,i32)override{}void sprite(AnmVm&,i32)override{}
    void effect(i32,const Vec3&,i32,u32)override{}void draw(AnmVm&)override{}
};
struct Actions:ItemUpdateActions {
    unsigned forwarded=0,collected=0;
    u32 owner_for(ItemState&)override{return 0;}
    u32 auto_collect_owner(ItemState&)override{return 3;}
    u32 overflow_owner(ItemState& item,u32 owner)override{assert(owner==0);++forwarded;item.state=1;return 1;}
    bool touching(u32,const Vec3& p,const Vec3&)override{return p.y>=350;}
    void collect(u32,ItemState&)override{++collected;}
    void item_sound(u32,i32,i32)override{}void removed(ItemState&)override{}
    void animation_step(AnmVm&)override{}void sound(i32,i32)override{}void subtract_rank(i32)override{}
};
int main(){
    auto state=std::make_unique<ItemPoolState>();state->reset();Rng random;PoolActions pool_actions;
    auto pool=std::make_unique<ItemPool>(*state,random,pool_actions);Actions actions;
    ItemUpdateContext context;context.player={0,400,0};context.power=128;
    ShotProfile human{},focus{};human.item_radius=8;human.item_fall_speed=1;
    ItemUpdate update(*state,*pool,context,human,focus,actions);update.timing.rate=1;
    auto& item=state->items[0];item.active=1;item.type=0;item.position={0,349,0};item.velocity={0,2,0};
    state->head.next=&item;update.update();
    assert(item.position.y==351&&item.state==1&&item.active&&actions.forwarded==1&&actions.collected==0);
    std::puts("th08 post-motion manual Power overflow: PASS");
}
