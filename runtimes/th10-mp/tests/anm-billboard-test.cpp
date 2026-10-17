#include "../th10_web/cpp/game/AnmProjection.hpp"
#include "../th10_web/cpp/game/GameMath.hpp"
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

using namespace th10;
extern "C" {
extern std::uint_fast8_t softfloat_exceptionFlags,softfloat_detectTininess;
}
namespace {
// Freeze only the original billboard expression, not a second renderer. This
// oracle deliberately repeats products and retains every original float store.
int original_billboard(AnmRenderer& renderer,AnmProjectionEnvironment& environment,const AnmVm& vm){
    const auto sum=[](float a,float b,float c){return number(a)+number(b)+number(c);};
    const auto anchor=[](u32 mode,Extended size,Extended& first,Extended& last){
        switch(mode){
        case 0:first=number((size*number(-.5f)).to_float());last=size*number(.5f);break;
        case 1:first=number(0);last=size;break;
        case 2:first=number((-size).to_float());last=number(0);break;
        default:std::abort();
        }
    };
    Matrix4 world;world.identity();
    world.elements[3][0]=sum(vm.child_position.x,vm.position.x,vm.script_position.x).to_float();
    world.elements[3][1]=sum(vm.child_position.y,vm.position.y,vm.script_position.y).to_float();
    world.elements[3][2]=sum(vm.child_position.z,vm.position.z,vm.script_position.z).to_float();
    Vec3 center,reference;const Vec3 origin{};
    environment.project(center,origin,world);if(center.z<0||center.z>1)return -1;
    environment.project(reference,*environment.camera_unit,world);
    const auto dx=number(Scalar::sub(reference.x,center.x));
    const auto dy=number(Scalar::sub(reference.y,center.y));
    const auto dz=number(Scalar::sub(reference.z,center.z));
    const auto ratio=(dz*dz+dy*dy+dx*dx).square_root()*number(.5f);
    const auto width=number((number(vm.sprite_size.x)*number(vm.scale.x)*ratio).to_float());
    const auto height=number((number(vm.sprite_size.y)*number(vm.scale.y)*ratio).to_float());
    auto* q=renderer.environment.quad;for(u32 i=0;i<4;++i)q[i].position.z=center.z;
    const auto c=number(cosine(number(vm.rotation.z)).to_float());
    const auto s=number(sine(number(vm.rotation.z)).to_float()),x=number(center.x),y=number(center.y);
    Extended left,right,top,bottom;
    anchor((vm.flags>>18)&3,width,left,right);anchor((vm.flags>>20)&3,height,top,bottom);
    const auto r=number(right.to_float()),b=number(bottom.to_float());
    q[0].position.x=(left*c-top*s+x).to_float();q[0].position.y=(left*s+top*c+y).to_float();
    q[1].position.x=(right*c-top*s+x).to_float();q[1].position.y=(top*c+right*s+y).to_float();
    q[2].position.x=(left*c-bottom*s+x).to_float();q[2].position.y=(left*s+c*bottom+y).to_float();
    q[3].position.x=(r*c-b*s+x).to_float();q[3].position.y=(r*s+b*c+y).to_float();
    return 0;
}
struct Render final:AnmRenderEnvironment {
    PipelineState& pipeline()override{std::abort();}
    void set_texture(void*)override{std::abort();}
    void vertex_format(LayoutParameter)override{std::abort();}
    void draw_triangles(TopologyParameter,u32,const void*,u32)override{std::abort();}
    void set_transform(MatrixParameter,const Matrix4&)override{std::abort();}
    void stream_source(void*,u32)override{std::abort();}
    void draw_buffer(TopologyParameter,u32,u32)override{std::abort();}
    i32 special_draw(AnmManager&,AnmVm&,u32)override{std::abort();}
};
struct Project final:AnmProjectionEnvironment {
    Vec3 center{120,170,.5f},reference{122,172,.5f},unit{1,1,1};
    u32 calls=0;Matrix4 worlds[2]{};Vec3 inputs[2]{};
    Project(){camera_unit=&unit;}
    void rotation(Matrix4&,u32,float)override{std::abort();}
    void multiply(Matrix4&,const Matrix4&,const Matrix4&)override{std::abort();}
    void project(Vec3& out,const Vec3& input,const Matrix4& world)override{
        if(calls>=2)std::abort();
        worlds[calls]=world;inputs[calls]=input;out=calls++?reference:center;
    }
    void transform(float*,const Vec3&,const Matrix4&)override{std::abort();}
};
struct Input {float values[17];u32 anchor;};
struct Result {
    AnmVertex quad[4];Matrix4 worlds[2];Vec3 inputs[2];
    u32 calls;int returned;unsigned flags;int error;
};
float value(u32 bits){float result;std::memcpy(&result,&bits,sizeof(result));return result;}
u32 random_bits(u32& state){state^=state<<13;state^=state>>17;state^=state<<5;return state;}
Result evaluate(AnmManager& manager,const Input& input,bool candidate,unsigned seed,int error){
    const auto* v=input.values;AnmVm vm{};
    vm.position={v[0],v[1],v[2]};vm.script_position={v[3],v[4],v[5]};
    vm.child_position={v[6],v[7],v[8]};vm.sprite_size={v[9],v[10]};
    vm.scale={v[11],v[12]};vm.rotation.z=v[13];
    vm.flags=(input.anchor%3)<<18|(input.anchor/3)<<20;
    Result result{};std::memset(result.quad,0xa5,sizeof(result.quad));
    Render render;render.quad=result.quad;AnmRenderer renderer{manager,render};
    Project environment;environment.center={v[14],v[15],v[16]};
    softfloat_exceptionFlags=seed;errno=error;
    result.returned=candidate?AnmProjection{renderer,environment}.billboard_geometry(vm)
        :original_billboard(renderer,environment,vm);
    result.flags=softfloat_exceptionFlags;result.error=errno;result.calls=environment.calls;
    std::memcpy(result.worlds,environment.worlds,sizeof(result.worlds));
    std::memcpy(result.inputs,environment.inputs,sizeof(result.inputs));
    return result;
}
bool same(const Result& a,const Result& b){
    return a.calls==b.calls&&a.returned==b.returned&&a.flags==b.flags&&a.error==b.error
        &&!std::memcmp(a.quad,b.quad,sizeof(a.quad))
        &&!std::memcmp(a.worlds,b.worlds,sizeof(a.worlds))
        &&!std::memcmp(a.inputs,b.inputs,sizeof(a.inputs));
}
}
int main(){
    auto manager=std::make_unique<AnmManager>();
    // Signed zero, subnormal/normal boundaries, representative rotations,
    // extreme finite values, infinities and quiet/signaling NaN payloads.
    const std::array<u32,34> special={
        0,0x80000000,1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,
        0x00800001,0x3e000000,0xbe000000,0x3f000000,0xbf000000,0x3f800000,
        0xbf800000,0x3fc90fdb,0xbfc90fdb,0x40490fdb,0xc0490fdb,0x40c90fdb,
        0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc00000,0x7fc12345,
        0xffc12345,0x7f800001,0xff800001,0x33800000,0xb3800000,0x4b800000,
        0xcb800000,0x3f7fffff};
    Input common{{1,2,3,4,5,6,7,8,9,32,64,1.5f,.7f,.15f,120,170,.5f},0};
    std::vector<Input> cases;
    for(u32 anchor=0;anchor<9;++anchor){
        common.anchor=anchor;
        for(u32 bits:special)for(unsigned index=9;index<17;++index){
            auto input=common;input.values[index]=value(bits);cases.push_back(input);
        }
    }
    u32 state=0x12345678;
    for(unsigned i=0;i<4096;++i){
        auto input=common;input.anchor=random_bits(state)%9;
        for(float& component:input.values)component=value(random_bits(state));
        if(i%2)input.values[16]=.5f; // Exercise accepted and rejected depth paths.
        cases.push_back(input);
    }
    unsigned checks=0;
    for(auto precision:{Precision::Single,Precision::Double,Precision::Extended})
        for(unsigned rounding=0;rounding<4;++rounding)for(unsigned tiny=0;tiny<2;++tiny)
            for(unsigned seed:{0,1,16,31}){
                arithmetic_mode(precision,static_cast<Rounding>(rounding));softfloat_detectTininess=tiny;
                for(unsigned i=0;i<cases.size();++i){
                    const auto expected=evaluate(*manager,cases[i],false,seed,i%2?ERANGE:0);
                    const auto actual=evaluate(*manager,cases[i],true,seed,i%2?ERANGE:0);
                    if(!same(expected,actual)){
                        std::fprintf(stderr,"ANM billboard mismatch: case=%u precision=%u rounding=%u tininess=%u seed=%u return=%d/%d flags=%u/%u errno=%d/%d\n",
                            i,unsigned(precision),rounding,tiny,seed,expected.returned,actual.returned,
                            expected.flags,actual.flags,expected.error,actual.error);
                        return 1;
                    }
                    ++checks;
                }
            }
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(__wasm__)
    constexpr bool product_reuse_enabled=true;
#else
    constexpr bool product_reuse_enabled=false;
#endif
    std::printf("{\"suite\":\"anm-billboard\",\"product_reuse_enabled\":%s,\"differential_checks\":%u,\"input_cases\":%zu,\"precisions\":3,\"roundings\":4,\"tininess_modes\":2,\"initial_flag_patterns\":4,\"anchor_pairs\":9,\"vertices_bit_equal\":true,\"projection_calls_and_inputs_equal\":true,\"return_values_equal\":true,\"sticky_flags_equal\":true,\"errno_equal\":true}\n",
        product_reuse_enabled?"true":"false",checks,cases.size());
}
