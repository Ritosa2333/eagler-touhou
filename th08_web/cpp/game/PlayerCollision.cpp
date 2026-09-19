#include "PlayerCollision.hpp"
#include "GameMath.hpp"
namespace th08 {
namespace {
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
i32 PlayerCollision::barrier(const Vec2& p){
    for(auto& r:regions.cancelling){if(!r.active)continue;bool hit=false;
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
    if(!overlap(xy(movement.bounds[0]),xy(movement.bounds[1]),box(xy(p),xy(size))))return 0;
    context.replay_flags|=2;if(life.state==0&&!actions.invincible()){actions.randomize_integrity();actions.die();}return 1;
}
i32 PlayerCollision::graze(const Vec3& p,const Vec3& size){
    cancel_item=6;if(barrier(xy(p)))return 2;
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
