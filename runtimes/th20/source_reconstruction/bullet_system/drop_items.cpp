#include "bullet.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::bullet {
void drop_items(Bullet& b,const sprite::Vec3& p,std::int32_t mode){
    if(mode==0||outside_viewport(p,32,32))return;
    auto& owner=*static_cast<Controller*>(b.context->primary_owner);++owner.item_counter;
    auto* items=b.context->objects_04[2];
    if(mode==1){unrecovered::item_spawn_004c45b0(items,p,1,13);return;}
    if(mode==4){
        const auto residue=recovered::signed_bits(owner.item_counter)%5;
        if(residue==0||(residue==2&&state::next(state::random_streams[0])%2==0)||(residue==4&&state::next(state::random_streams[0])%3==0))
            unrecovered::item_spawn_004c3c90(items,2,p,-1,-1.5707963705062866f,2.200000047683716f,0,0,-1);
        else unrecovered::item_spawn_004c45b0(items,p,1,13);
    }
}
}
