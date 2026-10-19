#include "AnmFile.hpp"
#include "GameMath.hpp"
namespace th10 {
// 0x43e5a0. Matrix dimensions and UV scale retain their original store order.
i32 AnmFile::bind_sprite(AnmVm& vm,i32 index) noexcept {
    if(!loaded||unavailable)return -1;
    vm.sprite_index=static_cast<std::int16_t>(index);vm.animation_file=this;vm.sprite=&sprites[index];
    vm.sprite_size={vm.sprite->width,vm.sprite->height};
    vm.sprite_matrix.identity();vm.uv_matrix.identity();
    vm.sprite_matrix.elements[0][0]=Scalar::mul(vm.sprite_size.x,0.00390625f);
    vm.sprite_matrix.elements[1][1]=Scalar::mul(vm.sprite_size.y,0.00390625f);
    vm.uv_matrix.elements[0][0]=(number(vm.sprite->scale_x)/number(vm.sprite->texture_width)*number(vm.sprite_size.x)).to_float();
    const auto scale_y=number(vm.sprite->scale_y)/number(vm.sprite->texture_height);
    vm.transform_matrix=vm.sprite_matrix;
    vm.uv_matrix.elements[1][1]=(scale_y*number(vm.sprite_size.y)).to_float();
    return 0;
}
// Ring mesh portion of 0x43ee30, 0x441169..0x44133e.
void AnmVm::update_ring_geometry() noexcept {
    auto* vertices=static_cast<AnmVertex*>(geometry);
    const i32 segments=wrapping_add(integer_variables[0],-1);
    const auto denominator=Extended::from_int(segments);
    const float angular_step=(number(6.283185482025146484375f)/denominator).to_float();
    const float texture_step=(Extended::from_int(integer_variables[1])/denominator).to_float();
    float angle=rotation.z,texture_v=0;
    auto* output=vertices;
    for(i32 segment=0;segment<segments;++segment){
        for(unsigned side=0;side<2;++side){
            auto& vertex=*output++;vertex.reciprocal_w=1;vertex.color=color;
            vertex.uv={Scalar::add(side?sprite->u1:sprite->u0,uv_offset.x),Scalar::add(texture_v,uv_offset.y)};
            const auto half_width=number(scale.x)*number(0.5f);
            const float radius=(side?number(scale.y)-half_width:half_width+number(scale.y)).to_float();
            const auto point=polar(angle,radius);
            const auto y=number(Scalar::add(position.y,script_position.y));
            vertex.position={
                (number(position.x)+number(script_position.x)+number(point.x)).to_float(),
                (y+number(point.y)).to_float(),
                Scalar::add(position.z,script_position.z)};
        }
        texture_v=Scalar::add(texture_step,texture_v);
        angle=add_angle(angle,angular_step).to_float();
    }
    output[0]=vertices[0];output[0].uv.y=Scalar::add(texture_v,uv_offset.y);
    output[1]=vertices[1];output[1].uv.y=Scalar::add(texture_v,uv_offset.y);
}
}
