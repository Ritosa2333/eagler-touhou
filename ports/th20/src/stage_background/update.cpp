#include "update.hpp"
#include "vm.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include <bit>
#include <cstring>
namespace th20::source::background {
namespace n=th20::recovered;namespace m=th20::source::ecl::math;
void normalize_camera_vector(float (&output)[3],const float (&input)[3]){
    const float length=m::square_root(n::add32(n::add32(n::mul32(input[0],input[0]),n::mul32(input[1],input[1])),n::mul32(input[2],input[2])));
    const float absolute=std::bit_cast<float>(std::bit_cast<std::uint32_t>(length)&0x7fffffffu);
    if(absolute>=.01f)for(unsigned i=0;i<3;++i)output[i]=_mm_cvtss_f32(_mm_div_ss(_mm_set_ss(input[i]),_mm_set_ss(length)));
    else std::memmove(output,input,12);
}
int update_objects(Background& background){
    for(int i=0;i<background.file->object_count;++i){
        auto& object=*background.objects[i];
        if(object.flags&1){unsigned active=0;
            for(auto* primitive=reinterpret_cast<Primitive*>(&object+1);primitive->type>=0;primitive=reinterpret_cast<Primitive*>(reinterpret_cast<std::uint8_t*>(primitive)+primitive->size)){
                auto& animation=background.primitive_animations[primitive->animation_index];sprite::execute_animation(animation);
                if(std::bit_cast<std::int32_t>(animation.base.fields_10_28[6])>=0)++active;
            }
            if(!active)object.flags&=0xfe;
        }
        if(object.parameters[1]!=0.f&&!(background.state_flags&16))object.parameters[2]=m::wrap_angle(n::add32(object.parameters[2],object.parameters[1]));
    }return 0;
}
int Background::update(){
    if((state_flags&8)||((state_flags&4)&&fade_timer.current>=60))return 1;
    std::memset(state.camera.points,0,sizeof(state.camera.points));std::memset(state.camera.final_vector,0,sizeof(state.camera.final_vector));
    float direction[3];for(unsigned i=0;i<3;++i)direction[i]=n::add32(state.camera.vectors[1][i],state.camera.vectors[6][i]);normalize_camera_vector(state.camera.vectors[3],direction);
    state.overlay_color=0x00808080;
    if(!(state_flags&4)||fade_timer.current<30){update_objects(*this);execute_script(state,th20::source::state::timer_rate);}
    program_entry::graphics_state.viewports[3]=state.camera;
    for(auto& animation:state.animations)sprite::execute_animation(animation);
    update_mesh_distortion(state,th20::source::state::timer_rate);++frame_count;return 1;
}
}
