#include "EclNative.hpp"
#include "EclSpawn.hpp"
#include "BulletState.hpp"
#include "GameMath.hpp"
namespace th08 {
namespace {
void scale(Vec3& p,float amount){p.x=Scalar::mul(p.x,amount);p.y=Scalar::mul(p.y,amount);p.z=Scalar::mul(p.z,amount);}
Vec3 subtract(const Vec3& a,const Vec3& b){return {Scalar::sub(a.x,b.x),Scalar::sub(a.y,b.y),Scalar::sub(a.z,b.z)};}
bool inside(const Vec3& p,const float (&box)[4]){return p.x>box[0]&&p.x<box[1]&&p.y>box[2]&&p.y<box[3];}
struct Warp {float inner[4],outer[4],inner_radius,outer_radius,center_y,previous_left;};
void warp(BulletManagerState& state,i32 id){
    constexpr Warp patterns[]{
        {{124.117744f,259.88226f,140.11774f,275.88226f},{56.23549f,327.76453f,72.23549f,343.76453f},67.882256f,135.76451f,208,56.23549f},
        {{56.23549f,327.76453f,88.23549f,359.76453f},{-32,416,0,448},135.76451f,224,224,-31.100006f},
        {{112.80403f,271.19595f,128.80403f,287.19595f},{33.60807f,350.39194f,49.60807f,366.39194f},79.19597f,158.39194f,208,33.60807f}
    };const auto& p=patterns[id==4?0:id==7?1:2];float previous_outer[4];std::memcpy(previous_outer,p.outer,sizeof(previous_outer));previous_outer[0]=p.previous_left;
    for(u32 i=0;i<1536;++i){auto& b=state.bullets[i];if(!b.state)continue;if(b.barrier_cooldown){b.barrier_cooldown=wrapping_sub(b.barrier_cooldown,1);continue;}
        const auto previous=subtract(b.position,b.velocity);const i32 current_zone=inside(b.position,p.inner)?0:inside(b.position,p.outer)?1:2,previous_zone=inside(previous,p.inner)?0:inside(previous,previous_outer)?1:2;
        if(current_zone==previous_zone)continue;b.barrier_cooldown=2;scale(b.velocity,-1);const bool expand=!current_zone||!previous_zone;
        const auto numerator=number(expand?p.outer_radius:p.inner_radius),denominator=number(expand?p.inner_radius:p.outer_radius);
        b.position.x=((number(b.position.x)-number(192))*numerator/denominator+number(192)).to_float();b.position.y=((number(b.position.y)-number(p.center_y))*numerator/denominator+number(p.center_y)).to_float();b.angle=add_angle(b.angle,3.1415927410125732f);
    }
}
void direction(Vec3& p,float angle,float speed){p.x=(cosine(angle)*number(speed)).to_float();p.y=(sine(angle)*number(speed)).to_float();}
}
bool execute_ecl_native_projectiles(EclVm& vm,i32 id,const EclInstruction& ins,EclNativeServices& services){
    auto* pool=services.projectiles;if(!pool){vm.invalid=true;vm.failure=EclVm::Failure::MissingNativeServices;return false;}auto& locals=vm.context().locals;
    if(id==4||id==7||id==21){warp(*pool,id);return true;}
    if(id==16){
        for(u32 i=0;i<1536;++i){auto& b=pool->bullets[i];if(!b.state||!(b.flags&0x100000))continue;u32 visited=0;
            for(auto* enemy=vm.next_familiar;enemy;enemy=enemy->next_familiar){if(++visited>480){vm.invalid=true;return false;}auto& target=enemy->context().locals;if(target.counters[2])continue;const auto d=subtract(b.position,enemy->position);if(number(d.x)*number(d.x)+number(d.y)*number(d.y)+number(d.z)*number(d.z)<number(4096)){target.counters[2]=60;target.integers[7]=locals.integers[7];}}
        }return true;
    }
    if(id==27){
        auto* spawn=vm.environment->enemy_actions;if(!spawn){vm.invalid=true;vm.failure=EclVm::Failure::MissingEnemyActions;return false;}
        for(u32 i=0;i<1536;++i){auto& b=pool->bullets[i];if(!b.state||!(b.flags&0x100000))continue;locals.floats[0]=b.angle;const auto result=spawn->spawn({locals.counters[2],b.position,800,-2,10},locals);if(!result.enemy||result.enemy->invalid){vm.invalid=true;return false;}b.flags&=~0x100000u;}return true;
    }
    if(!services.timing){vm.invalid=true;vm.failure=EclVm::Failure::MissingNativeServices;return false;}auto& timing=*services.timing;
    const auto sprite=[&](AnmVm& animation,i32 index){if(!pool->animation||pool->animation->SetSprite(&animation,index)){vm.invalid=true;vm.failure=EclVm::Failure::MissingAnimation;return false;}return true;};
    if(id==28||id==29){
        if(!services.actions){vm.invalid=true;vm.failure=EclVm::Failure::MissingNativeServices;return false;}if(ins.size<20){vm.invalid=true;return false;}i32 divisor;std::memcpy(&divisor,reinterpret_cast<const u8*>(&ins)+16,4);
        if(id==28){timing.rate=(number(1)/Extended::from_int(divisor)).to_float();services.actions->background_interrupt(2);}
        const float multiplier=id==28?timing.rate:Scalar::div(1,timing.rate);
        for(u32 i=0;i<1536;++i){auto& b=pool->bullets[i];if(!b.state)continue;scale(b.velocity,multiplier);auto& animation=b.sprites.animation[0];if(id==28)animation.baseSpriteIndex=animation.activeSpriteIndex;if(animation.activeSpriteIndex>=96&&animation.activeSpriteIndex<=111)if(!sprite(animation,id==28?111:animation.baseSpriteIndex))return false;}
        if(id==29){if(number(1)/Extended::from_int(divisor)<number(1))timing.force_step=true;timing.rate=1;services.actions->background_interrupt(1);}return true;
    }
    for(u32 i=0;i<1536;++i){auto& b=pool->bullets[i];if(!b.state||!(b.flags&u32(locals.integers[0])))continue;auto& a=b.sprites.animation[0];
        if(a.type==1){a.type=0;a.blendMode=1;if(id==14)a.color1.a=0;if(!sprite(a,a.activeSpriteIndex+16))return false;b.reisen_illusion=1;direction(b.velocity,id==12?locals.floats[0]:b.angle,Scalar::mul(timing.rate,locals.floats[1]));}
        else if(id==14&&a.type==0){a.type=2;a.color1.a=0;a.interpCurrentTimers[AnmInterp_Alpha1].set(0);a.interpEndTimers[AnmInterp_Alpha1].set(15);a.interpModes[AnmInterp_Alpha1]=0;a.color1Initial.a=0;a.color1Final.a=255;}
        else{a.type=1;a.blendMode=0;if(!sprite(a,a.activeSpriteIndex-16))return false;b.reisen_illusion=0;direction(b.velocity,b.angle,Scalar::mul(timing.rate,b.speed));}
    }
    if(id==12){u32 visited=0;for(auto* enemy=vm.next_familiar;enemy;enemy=enemy->next_familiar){if(++visited>480){vm.invalid=true;return false;}if(locals.integers[1])enemy->flags2&=~128u;else enemy->flags2|=128;}
        if(!services.actions){vm.invalid=true;vm.failure=EclVm::Failure::MissingNativeServices;return false;}services.actions->background_interrupt(locals.integers[1]?1:2);
    }return true;
}
}
