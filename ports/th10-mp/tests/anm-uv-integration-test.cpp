#include "../th10_web/cpp/game/AnmEnvironment.hpp"
#include "../th10_web/cpp/game/GameMath.hpp"
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
using namespace th10;
extern "C" {extern std::uint_fast8_t softfloat_exceptionFlags,softfloat_detectTininess;}
namespace {
float from_bits(u32 bits){float value;std::memcpy(&value,&bits,4);return value;}
struct Environment final:AnmEnvironment {
    float speed=1,timer_speed=1;Rng rng{};Vec3 zero{};
    Environment(){rate=&speed;visual_rng=script_rng=&rng;camera_delta=default_tangent=&zero;reference_positions[0]=reference_positions[1]=&zero;}
    void bind_sprite(AnmVm&,i32) override{std::abort();}
    void change_draw_mode(AnmVm&) override{std::abort();}
    void* allocate_geometry(u32) override{std::abort();}
    AnmVm* spawn_child(AnmVm&,i32,u32) override{std::abort();}
};
float* rate_field(AnmVm& vm,Environment& env,unsigned alias){
    float* fields[]={&env.speed,&vm.scale.x,&vm.scale.y,&vm.rotation.x,&vm.rotation.y,&vm.rotation.z,&vm.uv_offset.x,&vm.scale_velocity.x,&vm.angular_velocity.z};
    return fields[alias];
}
// Independent original equations for the deliberately inactive-interpolation
// fixture. The real interpreter runs separately on an identical full VM.
void original_animate(AnmVm& vm,float* rate_field){
    const float saved_rate=*rate_field;const auto rate=number(saved_rate);
    float* rotations[]={&vm.rotation.x,&vm.rotation.y,&vm.rotation.z};
    const float velocities[]={vm.angular_velocity.x,vm.angular_velocity.y,vm.angular_velocity.z};
    for(unsigned axis=0;axis<3;++axis)if(velocities[axis]!=0){
        *rotations[axis]=add_angle(*rotations[axis],(rate*number(velocities[axis])).to_float()).to_float();vm.flags|=4;
    }
    if(vm.scale_velocity.y!=0){vm.scale.y=(rate*number(vm.scale_velocity.y)+number(vm.scale.y)).to_float();vm.flags|=8;}
    if(vm.scale_velocity.x!=0){vm.scale.x=(rate*number(vm.scale_velocity.x)+number(vm.scale.x)).to_float();vm.flags|=12;}
    const auto scroll=[&](float value,float velocity){
        auto next=rate*number(velocity)+number(value);
        if(number(1.f)<next||number(1.f)==next)next=next-number(1.f);
        else if(next<number(0.f))next=next+number(1.f);
        return next.to_float();
    };
    vm.uv_offset.x=scroll(vm.uv_offset.x,vm.uv_velocity.x);vm.uv_offset.y=scroll(vm.uv_offset.y,vm.uv_velocity.y);
    vm.script_timer.tick();*rate_field=saved_rate;
}
}
int main(){
    const std::array<u32,18> rates={0,0x80000000,1,0x80000001,0x007fffff,0x00800000,0x3f000000,0x3f800000,0x40000000,0xbf000000,0xbf800000,0x7f7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345,0x7f800001,0xff800001};
    unsigned checks=0;
    for(auto precision:{Precision::Single,Precision::Double,Precision::Extended})for(unsigned rounding=0;rounding<4;++rounding)for(unsigned tiny=0;tiny<2;++tiny)for(unsigned alias=0;alias<9;++alias)for(const auto input:rates){
        Environment env;AnmVm actual{};AnmInstruction future{0,8,100,0};
        actual.instruction=actual.script_begin=&future;actual.script_timer.rate=&env.timer_speed;actual.scale={.5f,.5f};actual.scale_velocity={1,1};actual.angular_velocity={.125f,.25f,.5f};actual.flags=7;
        actual.uv_offset={0,.5f};actual.uv_velocity={0,-0.f};env.rate=rate_field(actual,env,alias);*env.rate=from_bits(input);
        AnmVm expected=actual;
        arithmetic_mode(precision,static_cast<Rounding>(rounding));softfloat_detectTininess=tiny;softfloat_exceptionFlags=checks%32;errno=EDOM;
        original_animate(expected,rate_field(expected,env,alias));const auto flags=softfloat_exceptionFlags;const int error=errno;
        softfloat_exceptionFlags=checks%32;errno=EDOM;const i32 result=actual.update(env);
        if(result||std::memcmp(&actual,&expected,sizeof(actual))||softfloat_exceptionFlags!=flags||errno!=error||env.rng.seed){
            std::fprintf(stderr,"ANM UV integration mismatch: case=%u precision=%u rounding=%u tiny=%u alias=%u rate=%08x\n",checks,unsigned(precision),rounding,tiny,alias,input);std::abort();
        }
        ++checks;
    }
    std::printf("{\"suite\":\"anm-uv-interpreter-integration\",\"checks\":%u,\"full_vm_bytes_equal\":true,\"sticky_flags_rng_errno_equal\":true}\n",checks);
}
