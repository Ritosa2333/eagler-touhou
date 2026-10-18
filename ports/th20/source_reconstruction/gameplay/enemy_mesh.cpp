#include "enemy_mesh.hpp"
#include "enemy_variables.hpp"
#include "../ecl_vm/math.hpp"
#include <cmath>
namespace th20::source::gameplay {
void update_enemy_mesh(EnemyState& state,EnemyMeshServices& env){
    auto* owner=reinterpret_cast<EnemyMeshOwner*>(state.fields_2c8[3]);if(!owner)return;
    auto& mesh=*owner->mesh;const float radius=owner->current_radius;float phase_x=owner->phase_x,phase_y=owner->phase_y;
    if(owner->current_radius<owner->radius)owner->current_radius=owner->current_radius+env.clock_scale()*2.f;
    sprite::Vec3 center;std::memcpy(&center,state.motion_110.words,12);env.initialize(mesh,(center.x-radius)-20.f,(center.y-radius)-20.f,radius*2.f+40.f,radius*2.f+40.f);
    center.x=float(env.view_offset(int(state.fields_2c8[8]),0))+center.x;center.y=float(env.view_offset(int(state.fields_2c8[8]),1))+center.y;
    auto* vertex=mesh.vertices;auto* position=mesh.positions;
    for(int column=0;column<mesh.columns;++column)for(int row=0;row<mesh.rows;++row,++vertex,++position){
        sprite::Vec3 delta{position->x-center.x,position->y-center.y,position->z-center.z};float weight=radius*radius-(delta.x*delta.x+delta.y*delta.y);
        if(weight<0)vertex->color&=0x00ffffffu;
        else{
            weight=weight/(radius*radius);vertex->color=owner->color;
            for(unsigned shift:{16u,8u,0u}){const auto old=(vertex->color>>shift)&255;const auto value=unsigned(static_cast<int>(255.f-float(255-old)*weight))&255u;vertex->color=(vertex->color&~(255u<<shift))|(value<<shift);}
            vertex->color|=0xff000000u;
            //4532c0: length uses all three coordinates, unlike456130 above.
            const float length=ecl::math::square_root((delta.x*delta.x+delta.y*delta.y)+delta.z*delta.z);const float magnitude=weight*32.f;
            if(std::fabs(length)>=0.01f){delta.x=(delta.x/length)*magnitude;delta.y=(delta.y/length)*magnitude;delta.z=(delta.z/length)*magnitude;}else{delta.x=delta.x*magnitude;delta.y=delta.y*magnitude;delta.z=delta.z*magnitude;}
            delta.x=(ecl::math::sine(phase_x)*weight)*8.f+delta.x;delta.y=(ecl::math::sine(phase_y)*weight)*8.f+delta.y;
            vertex->x=vertex->x+delta.x;vertex->y=vertex->y+delta.y;vertex->z=0;position->z=0;
        }
        phase_x=ecl::math::wrap_angle(phase_x+3.1415927410125732f/32.f);phase_y=ecl::math::wrap_angle(phase_y-3.1415927410125732f/64.f);
        if(position->x>0){if(float(env.dimension(0))<=position->x)vertex->x=position->x=float(env.dimension(0))-1.f;}else vertex->x=position->x=1.f;
        if(position->y>0){if(float(env.dimension(1))<=position->y)vertex->y=position->y=float(env.dimension(1))-1.f;}else vertex->y=position->y=1.f;
        env.uv(*position,vertex->u,vertex->v);
    }
    owner->phase_x=ecl::math::wrap_angle(owner->phase_x+(3.1415927410125732f/16.f)*env.clock_scale());owner->phase_y=ecl::math::wrap_angle(owner->phase_y+(3.1415927410125732f/32.f)*env.clock_scale());env.update(mesh);
}
}
