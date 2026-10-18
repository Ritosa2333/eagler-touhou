#include "DamageRegions.hpp"
#include "Arithmetic.hpp"
namespace th08 {
namespace {
DamageRegion& allocate(DamageRegion* pool,const Vec2& position,i32 value,i32 lifetime,bool damaging){
    i32 index=0;while(index<191&&pool[index].active)++index;auto& area=pool[index];area.reset();area.active=1;area.position=position;area.lifetime=lifetime;
    if(damaging)area.hit_damage=value;else area.cancel_item=value;return area;
}
}
DamageRegion& DamageRegions::rectangle(bool damage,const Vec2& position,float width,float height,i32 value,i32 lifetime){auto& area=allocate(damage?damaging:cancelling,position,value,lifetime,damage);area.dimensions={width,height};return area;}
DamageRegion& DamageRegions::circle(bool damage,const Vec2& position,float radius,float growth,i32 value,i32 lifetime){auto& area=allocate(damage?damaging:cancelling,position,value,lifetime,damage);area.radius=radius;area.radius_growth=growth;return area;}
void DamageRegions::update(){
    DamageRegion* pools[]{damaging,cancelling};for(auto* pool:pools)for(u32 i=0;i<192;++i){auto& area=pool[i];if(area.lifetime<0)continue;
        area.lifetime=wrapping_sub(area.lifetime,1);area.radius=Scalar::add(area.radius,area.radius_growth);
        area.dimensions.x=Scalar::add(area.dimensions.x,area.size_growth.x);area.dimensions.y=Scalar::add(area.dimensions.y,area.size_growth.y);
        if(area.lifetime<1)area.active=0;
    }
}
}
