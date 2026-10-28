#include "AnmGeometry.hpp"
#include "AnmEnvironment.hpp"
#include "SceneMatrices.hpp"

namespace th13 {
namespace {
Vec3 add(Vec3 a,Vec3 b){return{float(a.x+b.x),float(a.y+b.y),float(a.z+b.z)};}
Vec3 sub(Vec3 a,Vec3 b){return{float(a.x-b.x),float(a.y-b.y),float(a.z-b.z)};}
}
void AnmWindTrail::initialize(GameRandom& random,const AnmEnvironment& env) {
    *this={};age.set(1);interpolation.set(0);
    start=add(env.screen_translation,{float(env.reference_position.x*260.f),
        float(env.reference_position.y*260.f),float(env.reference_position.z*260.f)});
    start.y=float(double(start.y)-350.0);
    start.x+=float(double(random.signed_unit())*120.0);
    start.z+=float(double(random.signed_unit())*200.0);
    points.fill(start);tangents.fill({0,1,0});
    for(auto& vertex:vertices)vertex.color=0x80ffffff;
    // Keep draw order: three tangents, endpoint x/y, endpoint y jitter,
    // optional difficulty-dependent z, then three16-bit color draws.
    const float tx=random.signed_unit(),ty=random.unit(),tz=random.signed_unit();
    start_tangent={float(double(tx)*10),float(double(ty)*10),float(double(tz)*10)};
    float ex=float(double(random.unit())*800);if(start.x>0)ex=-ex;
    end_tangent={ex,float(double(random.unit())*1000),float(double(tz)*10)};
    const float ey=random.unit();float z=start.z;
    if(env.stage_difficulty>0)z=float(float(double(random.unit())*200)+800.f+z);
    end={float(double(start.x)*.25),float(float(double(ey)*1000)+2000.f+start.y),z};
    const u32 red=64+random.step16()%191,green=64+random.step16()%191,blue=64+random.step16()%191;
    color=0x80000000u|(red<<16)|(green<<8)|blue;
}
bool AnmWindTrail::update(const AnmEnvironment& env,u32 alpha,float rate) {
    for(size_t i=31;i>0;--i){tangents[i]=sub(points[i-1],points[i]);points[i]=add(points[i-1],env.background_delta);}
    drift=add(drift,env.background_delta);
    Vec3 point=end;bool endpoint=false;
    if(duration>0){
        interpolation.tick(&rate);
        if(interpolation.current>=duration){duration=0;endpoint=true;}
    }
    if(!endpoint) {
            // Retail402420 mode8 evaluates again with duration0 after its
            // endpoint tick. Preserve its nonfinite head until the alpha
            // history retires the effect; do not replace it with a held tip.
            // 402420 mode8: float weights, extended polynomial accumulation
            // for weights, then f32 product/add stores (4025a7..40266e).
            const float t=interpolation.fractional/float(duration);
            const double a=double(t)-1.0;
            const float h00=float((double(t)*2+1)*a*a);
            const float h01=float(double(t)*t*(3-double(t)*2));
            const float h10=float((1-double(t))*(1-double(t))*t);
            const float h11=float(double(a)*t*t);
            auto blend=[&](float s,float e,float u,float v){return float(float(float(float(h00*s)+float(h01*e))+float(h10*u))+float(h11*v));};
            point={blend(start.x,end.x,start_tangent.x,end_tangent.x),
                   blend(start.y,end.y,start_tangent.y,end_tangent.y),
                   blend(start.z,end.z,start_tangent.z,end_tangent.z)};
    }
    points[0]=add(point,drift);tangents[0]=sub(points[0],points[1]);
    const auto& d=env.stage_direction;
    for(int i=31;i>=0;--i){
        const auto& t=tangents[size_t(i)];
        Vec3 side{float(double(d.z)*t.y-double(d.y)*t.z),
                  float(double(d.x)*t.z-double(t.x)*d.z),
                  float(double(d.y)*t.x-double(t.y)*d.x)};
        side=normalize_scene_vector(side);
        side={float(side.x*5),float(side.y*5),float(side.z*5)};
        auto& left=vertices[size_t(i)*2];auto& right=vertices[size_t(i)*2+1];
        left.position=sub(points[size_t(i)],side);right.position=add(points[size_t(i)],side);
        left.color=i?vertices[size_t(i-1)*2].color:color;
        right.color=i?vertices[size_t(i-1)*2+1].color:color;
        const float u=float(1.0-double(31-i)*.03125);
        left.uv={u,1};right.uv={u,0};
    }
    color=(color&0xffffffu)|(alpha<<24);
    if(!(vertices[63].color>>24))return false;
    age.tick(&rate);return true;
}
}
