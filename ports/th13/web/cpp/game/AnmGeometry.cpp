#include "AnmGeometry.hpp"
#include "AnmOverlay.hpp"
#include <cmath>
namespace th13 {
AnmGeometry::AnmGeometry()=default;AnmGeometry::~AnmGeometry()=default;
AnmGeometry::AnmGeometry(AnmGeometry&&)noexcept=default;AnmGeometry& AnmGeometry::operator=(AnmGeometry&&)noexcept=default;
AnmGeometry::AnmGeometry(const AnmGeometry& other){*this=other;}
AnmGeometry& AnmGeometry::operator=(const AnmGeometry& other){if(this!=&other){screen_vertices=other.screen_vertices;world_vertices=other.world_vertices;allocation_bytes=other.allocation_bytes;distortion=other.distortion?std::make_unique<AnmDistortion>(*other.distortion):nullptr;trail=other.trail?std::make_unique<AnmTrail>(*other.trail):nullptr;circle=other.circle?std::make_unique<AnmCircle>(*other.circle):nullptr;wind=other.wind?std::make_unique<AnmWindTrail>(*other.wind):nullptr;ribbon=other.ribbon?std::make_unique<AnmRibbon>(*other.ribbon):nullptr;gather=other.gather?std::make_unique<AnmGatherEffect>(*other.gather):nullptr;overlay=other.overlay?std::make_unique<AnmOverlay>(*other.overlay):nullptr;}return *this;}
namespace {Vec2 radial(float angle,float radius){return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius))};}}
bool AnmGeometry::allocate(i32 count,bool world){if(count<0||count>0x100000)return false;clear();allocation_bytes=u32(count)*(world?48:56);if(world)world_vertices.resize(u32(count)*2);else screen_vertices.resize(u32(count)*2);return true;}
void AnmGeometry::clear(){screen_vertices.clear();world_vertices.clear();distortion.reset();trail.reset();circle.reset();gather.reset();ribbon.reset();wind.reset();overlay.reset();allocation_bytes=0;}
void AnmCircle::initialize(const Vec3& position,float r){center=position;radius=r;angle=0;age=Timer{};age.set(0);rebuild();}
void AnmCircle::rebuild(){vertices[0].position={center.x,center.y,0};vertices[0].reciprocal_w=1;vertices[0].color=color;for(u32 i=0;i<=32;++i){const float a=float(angle+6.2831853071795864769*double(i)/32.0);auto& v=vertices[i+1];v.position={float(center.x+std::cos(double(a))*double(radius)),float(center.y+std::sin(double(a))*double(radius)),0};v.reciprocal_w=1;v.color=rim_color;}}
void AnmDistortion::initialize(const Vec3& position,const Vec3& translation,GameRandom& random){
    *this={};uv_speed.x=float(random.signed_unit()*.008333333767950535f);uv_speed.y=float(random.signed_unit()*.008333333767950535f);
    vertices[0].position={float(position.x+translation.x),float(position.y+translation.y),float(position.z+translation.z)};vertices[0].reciprocal_w=1;vertices[0].uv={.5f,.5f};
    float angle=-3.1415927410125732421875f,speed=float(random.signed_unit()*.066666670143604279f);
    for(u32 i=0;i<31;i++){if(angle>=3.1415927410125732421875f)angle=float(angle-6.283185482025146484375f);auto& v=vertices[i+1];v.reciprocal_w=1;const auto uv=radial(angle,.5f);v.uv={float(uv.x+.5f),float(uv.y+.5f)};
        radius[i]=float(float(random.signed_unit()*8.f)+80.f);radial_speed[i]=speed;speed=float(float(random.signed_unit()*.03333333507180214f)+speed);if(speed<-.066666670143604279f)speed=-.066666670143604279f;else if(speed>.066666670143604279f)speed=.066666670143604279f;
        const auto xy=radial(angle,radius[i]);v.position={float(xy.x+float(translation.x+position.x)),float(float(position.y+translation.y)+xy.y),float(translation.z+position.z)};angle=float(angle+.202683404088020325f);
    }
}
void AnmDistortion::update(const Vec3& position,const Vec3& translation,u32 color){
    const Vec3 center{float(translation.x+position.x),float(translation.y+position.y),float(translation.z+position.z)};vertices[0].position=center;
    auto move_uv=[&](AnmGeometryVertex& vertex,u32 coordinate){float& value=coordinate?vertex.uv.y:vertex.uv.x;value=float(value+uv_speed.x);if(value<0)for(auto& v:vertices){float& all=coordinate?v.uv.y:v.uv.x;all=float(all+1.f);}};
    move_uv(vertices[0],0);move_uv(vertices[0],1);vertices[0].color=color;float angle=-3.1415927410125732421875f;
    for(u32 i=0;i<31;i++){auto& v=vertices[i+1];move_uv(v,0);move_uv(v,1);v.color=color&0xffffff;radius[i]=float(radial_speed[i]+radius[i]);const auto xy=radial(angle,radius[i]);v.position={float(xy.x+center.x),float(xy.y+center.y),float(v.position.z+center.z)};angle=float(angle+.202683404088020325f);}
    vertices[32]=vertices[1];
}
}
