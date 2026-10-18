#include "Camera.hpp"
#include "GraphicsMath.hpp"
#include "GameMath.hpp"
#include <array>
namespace th08 {
namespace {
u32 float_bits(float value) noexcept {u32 bits;std::memcpy(&bits,&value,sizeof(bits));return bits;}
bool finite_bits(u32 bits) noexcept {return (bits&0x7f800000u)!=0x7f800000u;}
bool finite_matrix(const Matrix4& value) noexcept {
    for(const auto& row:value.m)for(float element:row)if(!finite_bits(float_bits(element)))return false;
    return true;
}
bool finite_vector(const Vec3& value) noexcept {
    return finite_bits(float_bits(value.x))&&finite_bits(float_bits(value.y))&&finite_bits(float_bits(value.z));
}
struct ArithmeticKey {
    u32 width=0,height=0;
    Precision precision{};Rounding rounding{};GraphicsArithmetic graphics{};
    bool operator==(const ArithmeticKey& other)const noexcept {
        return width==other.width&&height==other.height&&precision==other.precision&&
            rounding==other.rounding&&graphics==other.graphics;
    }
};
ArithmeticKey arithmetic_key(const Viewport& viewport) noexcept {
    return {viewport.width,viewport.height,arithmetic_precision(),arithmetic_rounding(),GraphicsMath::arithmetic()};
}
struct SceneKey {
    ArithmeticKey arithmetic;
    std::array<u32,13> inputs{};
    bool operator==(const SceneKey& other)const noexcept {return arithmetic==other.arithmetic&&inputs==other.inputs;}
    bool finite()const noexcept {for(u32 bits:inputs)if(!finite_bits(bits))return false;return true;}
};
SceneKey scene_key(const Viewport& viewport,const SceneCamera& data) noexcept {
    // right is an output; unused24 and viewport origin/depth do not enter the
    // original formulas. Compare input bits, including signed zero, not floats
    // or the SceneCamera object's padding/derived state.
    return {arithmetic_key(viewport),{
        float_bits(data.position.x),float_bits(data.position.y),float_bits(data.position.z),
        float_bits(data.eye_offset.x),float_bits(data.eye_offset.y),float_bits(data.eye_offset.z),
        float_bits(data.target_offset.x),float_bits(data.target_offset.y),float_bits(data.target_offset.z),
        float_bits(data.up.x),float_bits(data.up.y),float_bits(data.up.z),float_bits(data.field_of_view)}};
}
// One fixed entry per formula, containing only derived values. The renderer
// still reapplies matrices and state on every call; rollback needs no cache
// restore because every numeric input and arithmetic mode is part of the key.
struct ScreenCache {ArithmeticKey key;Matrix4 view,projection;bool valid=false;};
struct SceneCache {SceneKey key;Matrix4 view,projection;Vec3 right;bool valid=false;};
static_assert(sizeof(ScreenCache)+sizeof(SceneCache)<=512,"Camera caches must remain bounded value storage.");
ScreenCache screen_cache;
SceneCache scene_cache;
#ifdef TH_CAMERA_CACHE_TEST
CameraCacheStats test_cache_stats;
#endif
}
#ifdef TH_CAMERA_CACHE_TEST
CameraCacheStats Camera::cache_stats() noexcept {return test_cache_stats;}
#endif
void Camera::screen(Matrix4& view,Matrix4& projection,const Viewport& viewport) {
    const auto key=arithmetic_key(viewport);
    const bool cacheable=viewport.width!=0&&viewport.height!=0&&(&view!=&projection);
    if(cacheable&&screen_cache.valid&&screen_cache.key==key){
        view=screen_cache.view;projection=screen_cache.projection;
#ifdef TH_CAMERA_CACHE_TEST
        ++test_cache_stats.screen_hits;
#endif
        return;
    }
#ifdef TH_CAMERA_CACHE_TEST
    ++test_cache_stats.screen_computations;
#endif
    const float x=(Extended::from_int64(viewport.width)/number(2)).to_float();
    const float y=(Extended::from_int64(viewport.height)/number(2)).to_float();
    const float aspect=(Extended::from_int64(viewport.width)/Extended::from_int64(viewport.height)).to_float();
    constexpr float fov=.3141592741012573f;
    const auto tan=tangent(Scalar::div(fov,2));
    const float distance=(number(y)/tan).to_float();
    GraphicsMath::look_at(view,{x,y,distance},{x,y,0},{0,-1,0});
    GraphicsMath::perspective(projection,fov,aspect,1,10000);
    if(cacheable&&finite_matrix(view)&&finite_matrix(projection)){
        screen_cache.key=key;screen_cache.view=view;screen_cache.projection=projection;screen_cache.valid=true;
    }
}
void Camera::scene(Matrix4& view,Matrix4& projection,const Viewport& viewport,SceneCamera& data) {
    const auto key=scene_key(viewport,data);
    const bool cacheable=viewport.width!=0&&viewport.height!=0&&(&view!=&projection)&&key.finite();
    if(cacheable&&scene_cache.valid&&scene_cache.key==key){
        view=scene_cache.view;projection=scene_cache.projection;data.right=scene_cache.right;
#ifdef TH_CAMERA_CACHE_TEST
        ++test_cache_stats.scene_hits;
#endif
        return;
    }
#ifdef TH_CAMERA_CACHE_TEST
    ++test_cache_stats.scene_computations;
#endif
    const auto add=[](const Vec3& a,const Vec3& b) { return Vec3{Scalar::add(a.x,b.x),Scalar::add(a.y,b.y),Scalar::add(a.z,b.z)}; };
    GraphicsMath::look_at(view,add(data.eye_offset,data.position),add(data.target_offset,data.position),data.up);
    const float aspect=(Extended::from_int64(viewport.width)/Extended::from_int64(viewport.height)).to_float();
    GraphicsMath::perspective(projection,data.field_of_view,aspect,30,1800);
    const auto& a=data.target_offset; const auto& b=data.up;
    data.right={(number(a.y)*number(b.z)-number(a.z)*number(b.y)).to_float(),
                (number(a.z)*number(b.x)-number(a.x)*number(b.z)).to_float(),
                (number(a.x)*number(b.y)-number(a.y)*number(b.x)).to_float()};
    GraphicsMath::normalize(data.right,data.right);
    if(cacheable&&finite_matrix(view)&&finite_matrix(projection)&&finite_vector(data.right)){
        scene_cache.key=key;scene_cache.view=view;scene_cache.projection=projection;scene_cache.right=data.right;scene_cache.valid=true;
    }
}
}
