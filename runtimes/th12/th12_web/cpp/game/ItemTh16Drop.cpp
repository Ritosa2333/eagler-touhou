#include "ItemManager.hpp"
#include "EnemyState.hpp"
#include <cmath>
#if PORT_TITLE == 16
namespace th11 {
bool item_th16_scatter(EnemyDrop& drops,Vec3 position,Rng& random,Th16DropServices& services){
    constexpr float pi=3.1415927410125732f,half_pi=1.5707963705062866f;
    float angle=float(double(random.signed_unit())*double(pi));
    for(u32 index=0;index<16;++index)for(i32 n=0;n<drops.counts[index];++n){
        if(index==15){
            const float speed=float(double(random.unit())*double(1.899999976158142f)+double(.20000000298023224f));
            const float direction=float(float(random.next32())/683565248.f)-pi;
            if(!services.spawn(16,position,direction,speed))return false;
        }else{
            const Vec2 offset{float(std::cos(double(angle))*double(drops.spread.x)),float(std::sin(double(angle))*double(drops.spread.y))};
            const float radius=float(double(random.unit())*.5+.5);
            const Vec3 p{position.x+radius*offset.x,position.y+radius*offset.y,position.z+0.f};
            if(!services.spawn(i32(index)+1,p,-half_pi,2.200000047683716f))return false;
            const float delta=random.signed_unit();angle=normalize_angle((angle+half_pi)+(delta*pi)*.25f);
        }
    }
    for(u32 n=0;n<17;++n)drops.counts[n]=0;return true;
}
bool item_th16_drop(EnemyDrop& drops,Vec3 position,Rng& random,Th16DropServices& services){
    if(drops.primary&&!services.spawn(drops.primary,position,-1.5707963705062866f,2.200000047683716f))return false;
    if(!item_th16_scatter(drops,position,random,services))return false;drops.primary=0;return true;
}
bool ItemManager::drop_th16(EnemyDrop& drops,Vec3 position){
    struct Services final:Th16DropServices {ItemManager& manager;explicit Services(ItemManager& m):manager(m){}bool spawn(i32 type,Vec3 p,float angle,float speed)override{return manager.spawn(type,p,0xffffffff,angle,speed)==0;}} services(*this);
    return item_th16_drop(drops,position,animations.visual_rng,services);
}
}
#endif
