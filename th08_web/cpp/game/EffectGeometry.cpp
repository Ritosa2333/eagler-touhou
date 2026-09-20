#include "EffectGeometry.hpp"
#include "PresentationAudit.hpp"
#include "GameMath.hpp"
#include <cstdlib>
namespace th08 {
namespace {
constexpr float pi=3.1415927410125732f,tau=6.283185482025147f;
Vec2 ellipse(float angle,float x,float y){return {(cosine(angle)*number(x)).to_float(),(sine(angle)*number(y)).to_float()};}
Vec2 rotate(Vec2 v,float angle){const auto s=number(sine(angle).to_float()),c=number(cosine(angle).to_float());return {(c*number(v.x)-s*number(v.y)).to_float(),(c*number(v.y)+s*number(v.x)).to_float()};}
}
i32 EffectGeometry::initialize(EffectState& e,EffectDraw callback,bool alternative){
    e.vertices=static_cast<SpriteVertex*>(std::malloc(258*sizeof(SpriteVertex)));
    if(!e.vertices){if(alternative)e.alternative=1;return alternative?0:-1;}
    e.segments=3;e.center=e.position;e.direction={0,0,1};e.up={0,-1,0};
    e.angle=e.parameters.x;e.radius=e.parameters.y;e.width=e.parameters.z;
    AnmRenderer::texture_strip(e,e.vertices,6,false);e.geometry_dirty=1;e.draw=callback;
    e.height=e.angle_y=e.frequency=0;e.segments=24;if(alternative)e.alternative=1;return 0;
}
void EffectGeometry::release(EffectState& e){std::free(e.vertices);e.vertices=nullptr;}
i32 EffectGeometry::draw(EffectState& e){
    const i32 count=prepare(e);if(count<=0)return 0;
    TH08_AUDIT_EFFECT(e,e.vertices,u32(count));
    renderer.draw_vertices(e,e.vertices,count);return 1;
}
i32 EffectGeometry::prepare(EffectState& e){
    if(!e.vertices||e.segments<1||e.segments>128){invalid=true;return 0;}
    const i32 count=e.segments*2+2;
    if(e.geometry_dirty){
        const float step=(number(tau)/Extended::from_int(e.segments)).to_float();
        const float width=(number(e.width)/sine(((number(pi)-number(step))/number(2)).to_float())).to_float();
        const float outer=Scalar::add(width,e.radius),inner=Scalar::sub(e.radius,width);
        AnmRenderer::texture_strip(e,e.vertices,count,true);
        auto place=[&](SpriteVertex& vertex,Vec2 point,bool preserve_z){
            vertex.pos.x=Scalar::add(point.x,e.center.x);vertex.pos.y=Scalar::add(point.y,e.center.y);
            vertex.pos.x=Scalar::add(arcade.x,vertex.pos.x);vertex.pos.y=Scalar::add(arcade.y,vertex.pos.y);
            vertex.pos.z=preserve_z?Scalar::add(0,e.center.z):0;
        };
        if(e.height==0){
            float angle=e.angle;for(i32 i=0;i<count;i+=2){if(angle>=pi)angle=Scalar::sub(angle,tau);place(e.vertices[i],ellipse(angle,outer,outer),true);place(e.vertices[i+1],ellipse(angle,inner,inner),true);angle=Scalar::add(angle,step);}
        }else if(e.frequency==0){
            const float high=Scalar::add(width,e.height),low=Scalar::sub(e.height,width);
            float angle=0;for(i32 i=0;i<count;i+=2){place(e.vertices[i],rotate(ellipse(angle,outer,high),e.angle_y),false);place(e.vertices[i+1],rotate(ellipse(angle,inner,low),e.angle_y),false);angle=Scalar::add(angle,step);}
        }else{
            const float phase_step=(number(tau)*number(e.frequency)/Extended::from_int(e.segments)).to_float();float angle=e.angle,phase=e.angle_y;
            for(i32 i=0;i<count;i+=2){
                if(angle>=pi)angle=Scalar::sub(angle,tau);if(phase>=pi)phase=Scalar::sub(phase,tau);
                const float wave=(cosine(phase)*number(e.height)).to_float(),a=Scalar::add(outer,wave),b=Scalar::add(inner,wave);
                place(e.vertices[i],ellipse(angle,a,a),false);place(e.vertices[i+1],ellipse(angle,b,b),false);angle=Scalar::add(angle,step);phase=Scalar::add(phase,phase_step);
            }
        }
        e.geometry_dirty=0;
    }
    return count;
}
}
