#include "PlayerBounds.hpp"
namespace th15 {
void PlayerBounds::update(PlayerMotion& motion,const Timer& age,AnmVm& vm,float rate){
    const bool enlarged=motion.behavior_flags&16;float scale=1;vm.visual.flags|=8;
    if(enlarged){if(enlargement.duration!=0)motion.enlargement=enlargement.step(rate)[0];scale=motion.enlargement;const float shown=enlargement.duration!=0&&age.current%3==0?1.f:scale;vm.visual.scale={shown,shown};}else vm.visual.scale={1,1};
    const auto& p=motion.position;const Vec3 h{float(hit_half_size.x*scale),float(hit_half_size.y*scale),float(hit_half_size.z*scale)};
    hit.minimum={float(p.x-h.x),float(p.y-h.y),float(p.z-h.z)};hit.maximum={float(h.x+p.x),float(h.y+p.y),float(p.z+h.z)};
    const Vec3 near{float(float(item_half_size.x*.5f)*scale),enlarged?float(float(scale*item_half_size.y)*.5f):float(item_half_size.y*.5f),enlarged?float(float(scale*item_half_size.z)*.5f):float(item_half_size.z*.5f)};
    item_near.minimum={float(p.x-near.x),float(p.y-near.y),float(p.z-near.z)};item_near.maximum={float(near.x+p.x),float(p.y+near.y),float(p.z+near.z)};
    const Vec3 g{float(graze_half_size.x*scale),float(graze_half_size.y*scale),float(graze_half_size.z*scale)};
    graze.minimum={float(p.x-g.x),float(p.y-g.y),float(p.z-g.z)};graze.maximum={float(p.x+g.x),float(p.y+g.y),float(p.z+g.z)};
    const Vec3 item{float(item_half_size.x*scale),float(item_half_size.y*scale),float(item_half_size.z*scale)};
    item_full.minimum={float(p.x-item.x),float(p.y-item.y),float(p.z-item.z)};item_full.maximum={float(item.x+p.x),float(p.y+item.y),float(p.z+item.z)};
}
}
