#include "PlayerCollision.hpp"
#include "GameMath.hpp"
#include <cmath>
#ifdef TH_MULTIPLAYER_FIXTURES
#include <array>
#endif
namespace th08 {
namespace {
bool collision_broadphase_enabled=true;
bool barrier_cache_enabled=true;
#ifdef TH_MULTIPLAYER_FIXTURES
std::array<double,4> broadphase_counts{};
bool audit_barrier_cache=false;
u32 barrier_cache_mismatches=0;
#endif
// Reject only widely separated, ordinary finite coordinates. The two-unit
// guard is much larger than float endpoint rounding within this bounded
// domain; near boundaries and unusual values retain the exact original box.
bool definitely_outside(const Vec3& p,const Vec3& size,const Vec3& lower,const Vec3& upper,float margin){
    const float values[]{p.x,p.y,size.x,size.y,lower.x,lower.y,upper.x,upper.y};
    for(float v:values)if(!std::isfinite(v)||std::fabs(v)>8192.f)return false;
    if(size.x<0||size.y<0||lower.x>upper.x||lower.y>upper.y)return false;
    const float x=size.x*.5f+margin+2.f,y=size.y*.5f+margin+2.f;
    return p.x+x<lower.x||p.x-x>upper.x||p.y+y<lower.y||p.y-y>upper.y;
}
struct Box {Vec2 lower,upper;};
Vec2 rotate(const Vec2& point,float angle){
    // 0043ee30 stores both trigonometric results before multiplying.
    const auto s=number(sine(angle).to_float()),c=number(cosine(angle).to_float());
    return {(c*number(point.x)-s*number(point.y)).to_float(),(c*number(point.y)+s*number(point.x)).to_float()};
}
Box box(const Vec2& p,const Vec2& size,float margin=0){
    return {{{(number(p.x)-number(size.x)/number(2)-number(margin)).to_float()},
             {(number(p.y)-number(size.y)/number(2)-number(margin)).to_float()}},
            {{(number(size.x)/number(2)+number(p.x)+number(margin)).to_float()},
             {(number(size.y)/number(2)+number(p.y)+number(margin)).to_float()}}};
}
Box vector_box(const Vec2& p,const Vec2& size){
    // The vector division helper rounds the half-size before vector addition.
    const auto x=number(Scalar::mul(size.x,.5f)),y=number(Scalar::mul(size.y,.5f));
    return {{(number(p.x)-x).to_float(),(number(p.y)-y).to_float()},
            {(number(p.x)+x).to_float(),(number(p.y)+y).to_float()}};
}
bool overlap(const Vec2& lower,const Vec2& upper,const Box& b){
    // These original branches permit unordered comparisons. Do not replace
    // them with four ordered >= tests: malformed/NaN data would differ.
    return !(lower.x>b.upper.x||upper.x<b.lower.x||lower.y>b.upper.y||upper.y<b.lower.y);
}
Vec2 xy(const Vec3& p){return {p.x,p.y};}
}
#ifdef TH_MULTIPLAYER_FIXTURES
bool fixture_collision_broadphase(bool enabled) noexcept {collision_broadphase_enabled=enabled;return collision_broadphase_enabled;}
bool fixture_barrier_cache(bool enabled) noexcept {barrier_cache_enabled=enabled;return enabled;}
void fixture_barrier_cache_audit(bool enabled) noexcept {audit_barrier_cache=enabled;}
u32 fixture_barrier_cache_mismatches() noexcept {return barrier_cache_mismatches;}
const double* fixture_collision_broadphase_counts() noexcept {return broadphase_counts.data();}
bool fixture_collision_broadphase_probe() noexcept {
    // Include far, near, touching, and representative maximum-domain values.
    constexpr float coordinates[]{-8192.f,-448.f,-32.f,-2.f,0.f,2.f,32.f,384.f,8192.f};
    constexpr float sizes[]{0.f,.1f,2.f,16.f,64.f,1024.f,8192.f};
    for(float px:coordinates)for(float py:coordinates)for(float sx:sizes)for(float sy:sizes){
        const Vec3 p{px,py,0},size{sx,sy,0};
        for(float margin:{0.f,20.f}){
            const Box exact=box({px,py},{sx,sy},margin);
            for(float bx:coordinates)for(float by:coordinates){
                const Vec3 lower{bx,by,0},upper{bx+4.f,by+4.f,0};
                if(definitely_outside(p,size,lower,upper,margin)&&
                   overlap({lower.x,lower.y},{upper.x,upper.y},exact))return false;
            }
        }
    }
    return true;
}
#endif
PlayerCollision::BarrierBatch::BarrierBatch(PlayerCollision* value):owner(value){
    if(!owner)return;
    previous=owner->barrier_batch;
    if(!barrier_cache_enabled)return;
    for(u16 slot=0;slot<192;++slot)if(owner->regions.cancelling[slot].active)slots[count++]=slot;
    owner->barrier_batch=this;
}
PlayerCollision::BarrierBatch::~BarrierBatch(){if(owner)owner->barrier_batch=previous;}
i32 PlayerCollision::barrier(const Vec2& p){
#ifdef TH_MULTIPLAYER_FIXTURES
    if(barrier_batch&&audit_barrier_cache){
        u32 found=0;
        for(u16 slot=0;slot<192;++slot)if(regions.cancelling[slot].active){
            if(found>=barrier_batch->count||barrier_batch->slots[found]!=slot)++barrier_cache_mismatches;
            ++found;
        }
        if(found!=barrier_batch->count)++barrier_cache_mismatches;
    }
#endif
    const u32 count=barrier_batch?barrier_batch->count:192;
    for(u32 i=0;i<count;++i){auto& r=regions.cancelling[barrier_batch?barrier_batch->slots[i]:i];
        if(!r.active)continue;bool hit=false;
        if(r.radius!=0){
            const auto x=number(Scalar::sub(p.x,r.position.x)),y=number(Scalar::sub(p.y,r.position.y));
            hit=x*x+y*y<number(r.radius)*number(r.radius);
        }else if(r.angle!=0){
            const Vec2 q=rotate({Scalar::sub(p.x,r.position.x),Scalar::sub(p.y,r.position.y)},-r.angle);
            const float x=Scalar::div(r.dimensions.x,2),y=Scalar::div(r.dimensions.y,2);
            hit=-x<=q.x&&q.x<=x&&-y<=q.y&&q.y<=y;
        }else{const Box b=box(r.position,r.dimensions);hit=overlap(p,p,b);}
        if(hit){cancel_item=r.cancel_item;r.damage_dealt=wrapping_add(r.damage_dealt,1);return 2;}
    }return 0;
}
i32 PlayerCollision::bullet(const Vec3& p,const Vec3& size,bool cancellation){
    cancel_item=6;if(cancellation&&barrier(xy(p)))return 2;
#ifdef TH_MULTIPLAYER_FIXTURES
    ++broadphase_counts[0];
#endif
    if(collision_broadphase_enabled&&definitely_outside(p,size,movement.bounds[0],movement.bounds[1],0)){
#ifdef TH_MULTIPLAYER_FIXTURES
        ++broadphase_counts[1];
#endif
        return 0;
    }
    if(!overlap(xy(movement.bounds[0]),xy(movement.bounds[1]),box(xy(p),xy(size))))return 0;
    context.replay_flags|=2;if(life.state==0&&!actions.invincible()){actions.randomize_integrity();actions.die();}return 1;
}
i32 PlayerCollision::graze(const Vec3& p,const Vec3& size){
    cancel_item=6;if(barrier(xy(p)))return 2;
#ifdef TH_MULTIPLAYER_FIXTURES
    ++broadphase_counts[2];
#endif
    if(collision_broadphase_enabled&&definitely_outside(p,size,movement.bounds[2],movement.bounds[3],20)){
#ifdef TH_MULTIPLAYER_FIXTURES
        ++broadphase_counts[3];
#endif
        return 0;
    }
    const Box b=box(xy(p),xy(size),20);if(life.state==2||life.state==1)return 0;
    if(!overlap(xy(movement.bounds[2]),xy(movement.bounds[3]),b))return 0;
    actions.graze(p,false);return 1;
}
bool PlayerCollision::item(const Vec3& p,const Vec3& size)const{
    if(life.state!=0&&life.state!=3&&life.state!=4)return false;
    return overlap(xy(movement.bounds[4]),xy(movement.bounds[5]),vector_box(xy(p),xy(size)));
}
i32 PlayerCollision::laser(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool grazing){
    Vec2 p=rotate({Scalar::sub(movement.position.x,origin.x),Scalar::sub(movement.position.y,origin.y)},-angle);
    p.x=Scalar::add(p.x,origin.x);p.y=Scalar::add(p.y,origin.y);
    const auto& half=movement.half_boxes[0];
    const Vec2 lower{Scalar::sub(p.x,half.x),Scalar::sub(p.y,half.y)},
               upper{Scalar::add(p.x,half.x),Scalar::add(p.y,half.y)};
    Box b=vector_box(center,size);
    if(overlap(lower,upper,b)){
        context.replay_flags|=2;if(life.state!=0||actions.invincible())return 0;actions.randomize_integrity();actions.die();return 1;
    }
    if(!grazing)return 0;b.lower.x=Scalar::sub(b.lower.x,48);b.lower.y=Scalar::sub(b.lower.y,48);
    b.upper.x=Scalar::add(b.upper.x,48);b.upper.y=Scalar::add(b.upper.y,48);
    if(!overlap(lower,upper,b)||life.state==2||life.state==1)return 0;actions.graze(movement.position,true);return 2;
}
}
