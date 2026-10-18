#include "Camera.hpp"
#include "GraphicsMath.hpp"
#include "GameMath.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

#ifndef TH_CAMERA_CACHE_TEST
#error This standalone oracle requires TH_CAMERA_CACHE_TEST.
#endif

using namespace th08;

namespace {
// Frozen pre-cache Camera formulas. These always recompute through the unchanged
// low-level math helpers; they never call Camera or inspect either cache.
namespace legacy {
void screen(Matrix4& view,Matrix4& projection,const Viewport& viewport) {
    const float x=(Extended::from_int64(viewport.width)/number(2)).to_float();
    const float y=(Extended::from_int64(viewport.height)/number(2)).to_float();
    const float aspect=(Extended::from_int64(viewport.width)/Extended::from_int64(viewport.height)).to_float();
    constexpr float fov=.3141592741012573f;
    const auto tan=tangent(Scalar::div(fov,2));
    const float distance=(number(y)/tan).to_float();
    GraphicsMath::look_at(view,{x,y,distance},{x,y,0},{0,-1,0});
    GraphicsMath::perspective(projection,fov,aspect,1,10000);
}
void scene(Matrix4& view,Matrix4& projection,const Viewport& viewport,SceneCamera& data) {
    const auto add=[](const Vec3& a,const Vec3& b) { return Vec3{Scalar::add(a.x,b.x),Scalar::add(a.y,b.y),Scalar::add(a.z,b.z)}; };
    GraphicsMath::look_at(view,add(data.eye_offset,data.position),add(data.target_offset,data.position),data.up);
    const float aspect=(Extended::from_int64(viewport.width)/Extended::from_int64(viewport.height)).to_float();
    GraphicsMath::perspective(projection,data.field_of_view,aspect,30,1800);
    const auto& a=data.target_offset; const auto& b=data.up;
    data.right={(number(a.y)*number(b.z)-number(a.z)*number(b.y)).to_float(),
                (number(a.z)*number(b.x)-number(a.x)*number(b.z)).to_float(),
                (number(a.x)*number(b.y)-number(a.y)*number(b.x)).to_float()};
    GraphicsMath::normalize(data.right,data.right);
}
}

u32 checks=0;
u32 bits(float value) {u32 result;std::memcpy(&result,&value,sizeof(result));return result;}
float value(u32 raw) {float result;std::memcpy(&result,&raw,sizeof(result));return result;}
[[noreturn]] void fail(const char* label,const char* detail) {
    std::fprintf(stderr,"camera-cache case=%u precision=%u rounding=%u graphics=%u %s: %s\n",
        checks,unsigned(arithmetic_precision()),unsigned(arithmetic_rounding()),
        unsigned(GraphicsMath::arithmetic()),label,detail);
    std::exit(1);
}
void require(bool condition,const char* label,const char* detail) {if(!condition)fail(label,detail);}
void same_matrix(const Matrix4& expected,const Matrix4& actual,const char* label) {
    for(u32 row=0;row<4;++row)for(u32 column=0;column<4;++column){
        const u32 want=bits(expected.m[row][column]),got=bits(actual.m[row][column]);
        if(want!=got){
            std::fprintf(stderr,"matrix[%u][%u] expected=%08x actual=%08x\n",row,column,want,got);
            fail(label,"matrix bits differ");
        }
    }
}
std::array<u32,19> camera_bits(const SceneCamera& camera) {
    return {bits(camera.position.x),bits(camera.position.y),bits(camera.position.z),
        bits(camera.target_offset.x),bits(camera.target_offset.y),bits(camera.target_offset.z),
        bits(camera.up.x),bits(camera.up.y),bits(camera.up.z),
        bits(camera.unused24.x),bits(camera.unused24.y),bits(camera.unused24.z),
        bits(camera.right.x),bits(camera.right.y),bits(camera.right.z),
        bits(camera.eye_offset.x),bits(camera.eye_offset.y),bits(camera.eye_offset.z),
        bits(camera.field_of_view)};
}
void same_camera(const SceneCamera& expected,const SceneCamera& actual,const char* label) {
    const auto want=camera_bits(expected),got=camera_bits(actual);
    for(u32 index=0;index<want.size();++index)if(want[index]!=got[index]){
        std::fprintf(stderr,"camera[%u] expected=%08x actual=%08x\n",index,want[index],got[index]);
        fail(label,"camera bits differ, including right or an unrelated field");
    }
}
void poison(Matrix4& matrix) {
    for(u32 row=0;row<4;++row)for(u32 column=0;column<4;++column)
        matrix.m[row][column]=value(0x4a000000u+(checks&0xffffu)*16+row*4+column);
}
void check_stats(const CameraCacheStats& before,const CameraCacheStats& after,bool scene,bool hit,const char* label) {
    const u64 expected_hits=hit?1:0,expected_computations=hit?0:1;
    require(after.scene_hits-before.scene_hits==(scene?expected_hits:0),label,"unexpected scene hit count");
    require(after.scene_computations-before.scene_computations==(scene?expected_computations:0),label,"unexpected scene computation count");
    require(after.screen_hits-before.screen_hits==(!scene?expected_hits:0),label,"unexpected screen hit count");
    require(after.screen_computations-before.screen_computations==(!scene?expected_computations:0),label,"unexpected screen computation count");
}
void check_screen(const Viewport& viewport,bool hit,const char* label) {
    ++checks;
    Matrix4 view,projection,expected_view,expected_projection;
    poison(view);poison(projection);
    const auto before=Camera::cache_stats();
    Camera::screen(view,projection,viewport);
    check_stats(before,Camera::cache_stats(),false,hit,label);
    legacy::screen(expected_view,expected_projection,viewport);
    same_matrix(expected_view,view,label);same_matrix(expected_projection,projection,label);
}
void check_scene(const SceneCamera& input,const Viewport& viewport,bool hit,const char* label) {
    ++checks;
    auto actual=input,expected=input;
    // A hit must restore this output instead of depending on the caller's old
    // right vector. All other SceneCamera fields are compared independently.
    actual.right={value(0x7fc10000u+(checks&0xffffu)),value(0x7f800000u),value(0x80000000u)};
    Matrix4 view,projection,expected_view,expected_projection;
    poison(view);poison(projection);
    const auto before=Camera::cache_stats();
    Camera::scene(view,projection,viewport,actual);
    check_stats(before,Camera::cache_stats(),true,hit,label);
    legacy::scene(expected_view,expected_projection,viewport,expected);
    same_matrix(expected_view,view,label);same_matrix(expected_projection,projection,label);
    same_camera(expected,actual,label);
}
SceneCamera base_camera() {
    SceneCamera camera{};
    camera.position={123.375f,-19.625f,1000.25f};
    camera.eye_offset={.25f,-.125f,17.75f};
    camera.target_offset={-3.375f,14.5f,-200.25f};
    camera.up={.125f,1.125f,.0625f};
    camera.field_of_view=.517123401f;
    camera.unused24={value(0x80000000u),value(0x7fc54321u),77.25f};
    return camera;
}
float& input_field(SceneCamera& camera,u32 index) {
    // Deliberately enumerate the public input members; no object-layout casts.
    switch(index){
    case 0:return camera.position.x;case 1:return camera.position.y;case 2:return camera.position.z;
    case 3:return camera.eye_offset.x;case 4:return camera.eye_offset.y;case 5:return camera.eye_offset.z;
    case 6:return camera.target_offset.x;case 7:return camera.target_offset.y;case 8:return camera.target_offset.z;
    case 9:return camera.up.x;case 10:return camera.up.y;case 11:return camera.up.z;
    default:return camera.field_of_view;
    }
}
void check_aliases(const SceneCamera& input,const Viewport& viewport) {
    ++checks;
    Matrix4 actual,expected;poison(actual);
    auto before=Camera::cache_stats();
    Camera::screen(actual,actual,viewport);
    check_stats(before,Camera::cache_stats(),false,false,"screen alias bypass");
    legacy::screen(expected,expected,viewport);
    same_matrix(expected,actual,"screen alias oracle");

    ++checks;
    auto actual_camera=input,expected_camera=input;poison(actual);
    before=Camera::cache_stats();
    Camera::scene(actual,actual,viewport,actual_camera);
    check_stats(before,Camera::cache_stats(),true,false,"scene alias bypass");
    legacy::scene(expected,expected,viewport,expected_camera);
    same_matrix(expected,actual,"scene alias oracle");
    same_camera(expected_camera,actual_camera,"scene alias camera oracle");
}
void arithmetic_transition(const SceneCamera& camera,const Viewport& viewport,Precision precision,Rounding rounding,GraphicsArithmetic graphics) {
    arithmetic_mode(precision,rounding);GraphicsMath::arithmetic(graphics);
    require(GraphicsMath::arithmetic()==graphics,"graphics getter","setter/getter disagree");
    check_scene(camera,viewport,false,"arithmetic transition scene miss");
    check_scene(camera,viewport,true,"arithmetic transition scene hit");
    check_screen(viewport,false,"arithmetic transition screen miss");
    check_screen(viewport,true,"arithmetic transition screen hit");
}
void check_combination(Precision precision,Rounding rounding,GraphicsArithmetic graphics) {
    const auto camera=base_camera();
    const Viewport viewport{32,16,384,448,0,1};
    arithmetic_transition(camera,viewport,precision,rounding,graphics);

    // Each arithmetic key component must invalidate independently. Merely
    // enumerating combinations while also changing graphics would miss a
    // forgotten precision or rounding key.
    const auto other_precision=precision==Precision::Single?Precision::Double:Precision::Single;
    arithmetic_transition(camera,viewport,other_precision,rounding,graphics);
    arithmetic_transition(camera,viewport,precision,rounding,graphics);
    const auto other_rounding=rounding==Rounding::NearestEven?Rounding::Down:Rounding::NearestEven;
    arithmetic_transition(camera,viewport,precision,other_rounding,graphics);
    arithmetic_transition(camera,viewport,precision,rounding,graphics);
    const auto other_graphics=graphics==GraphicsArithmetic::Simd?GraphicsArithmetic::Scalar:GraphicsArithmetic::Simd;
    arithmetic_transition(camera,viewport,precision,rounding,other_graphics);
    arithmetic_transition(camera,viewport,precision,rounding,graphics);

    check_scene(camera,viewport,true,"scene survives screen cache use");
    check_screen(viewport,true,"screen survives scene cache use");
    auto unrelated=camera;
    unrelated.unused24={value(0xffc54321u),19.25f,value(0x80000000u)};
    unrelated.right={17,18,19};
    check_scene(unrelated,viewport,true,"right/unused24 are not cache inputs");
    check_scene(camera,viewport,true,"unrelated fields preserved per caller");

    for(u32 field=0;field<13;++field){
        auto changed=camera;auto& component=input_field(changed,field);
        component=value(bits(component)^1u);
        check_scene(changed,viewport,false,"each input bit invalidates");
        check_scene(changed,viewport,true,"changed input repeats");
        check_scene(camera,viewport,false,"rollback to previous camera");
        check_scene(camera,viewport,true,"restored camera repeats");
    }

    const std::array<Viewport,4> dimensions{{
        {32,16,385,448,0,1},{32,16,384,449,0,1},
        {0,0,1280,720,0,1},{0,0,0xffffffffu,0xfffffffeu,0,1}}};
    for(const auto& changed:dimensions){
        check_scene(camera,changed,false,"scene viewport change");
        check_scene(camera,changed,true,"scene viewport repeat");
        check_screen(changed,false,"screen viewport change");
        check_screen(changed,true,"screen viewport repeat");
        check_scene(camera,viewport,false,"scene viewport rollback");
        check_scene(camera,viewport,true,"scene viewport rollback repeat");
        check_screen(viewport,false,"screen viewport rollback");
        check_screen(viewport,true,"screen viewport rollback repeat");
    }
    auto moved=viewport;moved.x=91;moved.y=47;moved.min_z=.25f;moved.max_z=.75f;
    check_scene(camera,moved,true,"scene ignores unused viewport fields");
    check_screen(moved,true,"screen ignores unused viewport fields");

    for(u32 field=0;field<12;++field){
        auto positive=camera,negative=camera;
        input_field(positive,field)=value(0x00000000u);
        input_field(negative,field)=value(0x80000000u);
        check_scene(positive,viewport,false,"positive zero input");
        check_scene(positive,viewport,true,"positive zero repeat");
        check_scene(negative,viewport,false,"negative zero has a distinct key");
        check_scene(negative,viewport,true,"negative zero repeat");
        check_scene(positive,viewport,false,"signed zero rollback");
        check_scene(positive,viewport,true,"signed zero rollback repeat");
        check_scene(camera,viewport,false,"restore after zero input");
        check_scene(camera,viewport,true,"restore after zero repeat");
    }

    for(u32 raw:{0x00000001u,0x80000001u,0x007fffffu,0x807fffffu}){
        auto tiny=camera;tiny.position.x=value(raw);
        check_scene(tiny,viewport,false,"finite subnormal input");
        check_scene(tiny,viewport,true,"finite subnormal repeats");
        check_scene(camera,viewport,false,"restore after subnormal");
        check_scene(camera,viewport,true,"restore after subnormal repeats");
    }

    const u32 exceptional[]{0x7f800000u,0xff800000u,0x7fc12345u,0xffc54321u,0x7f812345u,0xff812345u};
    for(u32 field=0;field<13;++field)for(u32 raw:exceptional){
        auto invalid=camera;input_field(invalid,field)=value(raw);
        check_scene(invalid,viewport,false,"nonfinite input bypass");
        check_scene(invalid,viewport,false,"nonfinite input never cached");
        check_scene(camera,viewport,true,"nonfinite input did not poison valid entry");
    }
    // Zero FOV is finite input but produces a nonfinite projection. Its result
    // must not replace the preceding valid entry, for either sign of zero.
    for(u32 raw:{0x00000000u,0x80000000u}){
        auto singular=camera;singular.field_of_view=value(raw);
        check_scene(singular,viewport,false,"nonfinite result bypass");
        check_scene(singular,viewport,false,"nonfinite result never cached");
        check_scene(camera,viewport,true,"nonfinite result did not poison valid entry");
    }

    const std::array<Viewport,3> invalid_dimensions{{
        {0,0,0,448,0,1},{0,0,384,0,0,1},{0,0,0,0,0,1}}};
    for(const auto& invalid:invalid_dimensions){
        check_scene(camera,invalid,false,"invalid scene viewport bypass");
        check_scene(camera,invalid,false,"invalid scene viewport never cached");
        check_screen(invalid,false,"invalid screen viewport bypass");
        check_screen(invalid,false,"invalid screen viewport never cached");
        check_scene(camera,viewport,true,"invalid viewport retains scene entry");
        check_screen(viewport,true,"invalid viewport retains screen entry");
    }

    check_aliases(camera,viewport);
    check_aliases(camera,viewport);
    check_scene(camera,viewport,true,"alias bypass retains scene entry");
    check_screen(viewport,true,"alias bypass retains screen entry");
}
}

int main() {
    constexpr Precision precisions[]{Precision::Single,Precision::Double,Precision::Extended};
    constexpr Rounding roundings[]{Rounding::NearestEven,Rounding::TowardZero,Rounding::Down,Rounding::Up};
    constexpr GraphicsArithmetic graphics[]{GraphicsArithmetic::Simd,GraphicsArithmetic::Scalar};
    u32 combinations=0;
    for(auto precision:precisions)for(auto rounding:roundings)for(auto path:graphics){
        check_combination(precision,rounding,path);++combinations;
    }
    const auto stats=Camera::cache_stats();
    require(combinations==24&&stats.scene_hits>0&&stats.screen_hits>0,"coverage","missing arithmetic combination or cache hits");
    std::printf("camera-cache: PASS combinations=%u checks=%u sceneHits=%llu sceneComputations=%llu screenHits=%llu screenComputations=%llu\n",
        combinations,checks,static_cast<unsigned long long>(stats.scene_hits),static_cast<unsigned long long>(stats.scene_computations),
        static_cast<unsigned long long>(stats.screen_hits),static_cast<unsigned long long>(stats.screen_computations));
    return 0;
}

