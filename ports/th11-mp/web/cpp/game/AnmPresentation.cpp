#include "AnmPresentation.hpp"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <unordered_map>
namespace th11::presentation {
namespace {
constexpr float tau=6.2831854820251465f;
struct Entry {std::uint64_t tick=0;Sample sample;};
std::unordered_map<std::uint64_t,Entry> samples;
std::uint64_t current_tick=1;
std::uint32_t stats[8]{},watch[10]{},watch_id=0;
std::uint64_t key(const AnmVm& vm) noexcept {
    // Pooled VMs keep a stable address; embedded copies share the authored id.
    // Prefer the id so attached visuals drawn from a stack copy still find the
    // endpoint published for their owner.
    return vm.id?0x8000000000000000ull|vm.id:std::uint64_t(reinterpret_cast<std::uintptr_t>(&vm));
}
u32 continuous_fields(const AnmVm& vm) noexcept {
    u32 fields=0;
    if(vm.rotation_interpolation.duration>0||vm.angular_velocity.x!=0||vm.angular_velocity.y!=0||vm.angular_velocity.z!=0)fields|=Rotation;
    if(vm.scale_interpolation.duration>0||vm.scale_velocity.x!=0||vm.scale_velocity.y!=0)fields|=Scale;
    if(vm.color_interpolation.duration>0)fields|=Rgb;
    if(vm.alpha_interpolation.duration>0)fields|=Opacity;
    if(vm.color2_interpolation.duration>0)fields|=Rgb2;
    if(vm.alpha2_interpolation.duration>0)fields|=Opacity2;
    if(vm.scroll_velocity.x!=0||vm.scroll_velocity.y!=0)fields|=Uv;
    return fields;
}
Sample sample_of(const AnmVm& vm) noexcept {
    Sample s;
    s.id=vm.id;s.file=vm.resource;s.script=vm.script_index;s.sprite=vm.sprite_index;
    s.visible=(vm.flags&3u)==3u;s.timer=vm.timer.current;
    s.position=vm.position;s.script_position=vm.script_position;s.child_position=vm.child_position;
    s.rotation=vm.rotation;s.scale=vm.scale;s.uv=vm.uv_offset;
    s.color=vm.color;s.secondary_color=vm.secondary_color;
    s.continuous=continuous_fields(vm);
    return s;
}
bool near(const Vec3& a,const Vec3& b) noexcept {
    const float x=b.x-a.x,y=b.y-a.y,z=b.z-a.z;
    return x*x+y*y+z*z<high_refresh::position_limit*high_refresh::position_limit;
}
Vec3 total(const Vec3& position,const Vec3& script,const Vec3& child) noexcept {
    return {float(double(position.x)+double(script.x)+double(child.x)),float(double(position.y)+double(script.y)+double(child.y)),float(double(position.z)+double(script.z)+double(child.z))};
}
float mix(float a,float b) noexcept {return high_refresh::lerp_world(a,b);}
Vec3 mix(const Vec3& a,const Vec3& b) noexcept {return {mix(a.x,b.x),mix(a.y,b.y),mix(a.z,b.z)};}
float cyclic(float a,float b,float period) noexcept {
    return a+std::remainder(b-a,period)*high_refresh::alpha;
}
u32 channel(u32 before,u32 current,u32 shift) noexcept {
    const float value=high_refresh::lerp(float((before>>shift)&255u),float((current>>shift)&255u));
    return u32(std::clamp(value,0.0f,255.0f))<<shift;
}
bool channel_near(u32 before,u32 current,u32 shift,u32 maximum) noexcept {
    const i32 a=i32((before>>shift)&255u),b=i32((current>>shift)&255u);
    return u32(std::abs(a-b))<=maximum;
}
bool all_near(u32 before,u32 current,u32 maximum) noexcept {
    return channel_near(before,current,0,maximum)&&channel_near(before,current,8,maximum)&&channel_near(before,current,16,maximum);
}
}
void begin_tick() noexcept {
#ifdef TH11_MULTIPLAYER
    // Netplay retains its confirmed-frontier clock and presentation ownership.
    return;
#endif
    ++current_tick;
    if(samples.size()>16384)for(auto it=samples.begin();it!=samples.end();)it=it->second.tick+120<current_tick?samples.erase(it):std::next(it);
}
void reset() noexcept {samples.clear();current_tick=1;}
void capture(const AnmVm& vm) noexcept {
#ifdef TH11_MULTIPLAYER
    // Netplay retains its confirmed-frontier clock and presentation ownership.
    return;
#endif
    auto& entry=samples[key(vm)];
    if(entry.tick==current_tick)return;
    entry.tick=current_tick;entry.sample=sample_of(vm);
}
void capture_if_needed(const AnmVm& vm) noexcept {
#ifdef TH11_MULTIPLAYER
    // Netplay retains its confirmed-frontier clock and presentation ownership.
    return;
#endif
    auto& entry=samples[key(vm)];
    if(entry.tick==current_tick)return;
    entry.tick=current_tick;entry.sample=sample_of(vm);
}
void snap(const AnmVm& vm) noexcept {
#ifdef TH11_MULTIPLAYER
    // Netplay retains its confirmed-frontier clock and presentation ownership.
    return;
#endif
    auto& entry=samples[key(vm)];
    entry.tick=current_tick;entry.sample=sample_of(vm);
}
bool present_position(const AnmVm& current,Vec3& out) noexcept {
    if(!high_refresh::interpolating())return false;
    const auto found=samples.find(key(current));
    if(found==samples.end()||found->second.tick!=current_tick)return false;
    const auto& before=found->second.sample;
    if(before.id!=current.id||before.file!=current.resource||before.script!=current.script_index)return false;
    const bool visible=(current.flags&3u)==3u;
    if(before.visible!=visible||!visible)return false;
    if(current.timer.current<before.timer)return false;
    Vec3 position=current.position;
    if(near(before.position,current.position))position=mix(before.position,current.position);
    Vec3 script=current.script_position;
    if(near(before.script_position,current.script_position))script=mix(before.script_position,current.script_position);
    Vec3 child=current.child_position;
    if(near(before.child_position,current.child_position))child=mix(before.child_position,current.child_position);
    out={float(double(position.x)+double(script.x)+double(child.x)),float(double(position.y)+double(script.y)+double(child.y)),float(double(position.z)+double(script.z)+double(child.z))};
    return true;
}
bool present(AnmVm& destination,const AnmVm& current) noexcept {
    if(!high_refresh::interpolating())return false;
    ++stats[6];
    const bool watched=watch_id&&(current.id==watch_id||current.reserved_420[0]==watch_id);
    if(watched)++watch[0];
    const auto found=samples.find(key(current));
    if(found==samples.end()){++stats[1];if(watched)++watch[1];return false;}
    const auto& entry=found->second;
    // A sample older than this tick has no prev/current pair to blend.
    if(entry.tick!=current_tick){++stats[2];if(watched)++watch[2];return false;}
    const auto& before=entry.sample;
    // Lifecycle boundaries snap instead of blending a stale incarnation.
    if(before.id!=current.id||before.file!=current.resource||before.script!=current.script_index)return false;
    const bool visible=(current.flags&3u)==3u;
    if(before.visible!=visible||!visible)return false;
    if(current.timer.current<before.timer)return false;
    const u32 continuous=before.continuous|continuous_fields(current);
    bool applied=false;
    const bool position_near=near(before.position,current.position),script_near=near(before.script_position,current.script_position),child_near=near(before.child_position,current.child_position);
    if(watched)++watch[6];
    if(position_near&&script_near&&child_near){
        if(position_near){destination.position=mix(before.position,current.position);applied=true;}
        if(script_near){destination.script_position=mix(before.script_position,current.script_position);applied=true;}
        if(child_near){destination.child_position=mix(before.child_position,current.child_position);applied=true;}
    }else if(!(std::fabs(double(current.position.x)+current.script_position.x+current.child_position.x-(double(before.position.x)+before.script_position.x+before.child_position.x))>=200.||
                 std::fabs(double(current.position.y)+current.script_position.y+current.child_position.y-(double(before.position.y)+before.script_position.y+before.child_position.y))>=200.||
                 std::fabs(double(current.position.z)+current.script_position.z+current.child_position.z-(double(before.position.z)+before.script_position.z+before.child_position.z))>=200.)){
        // Owners such as the 3D stage rebase world position and script offset in
        // opposite directions. The individual fields jump, but the composed
        // draw position remains continuous; blend the composed position instead
        // of snapping the whole object.
        const Vec3 before_total=total(before.position,before.script_position,before.child_position);
        const Vec3 current_total=total(current.position,current.script_position,current.child_position);
        if(near(before_total,current_total)){
            if(watched)++watch[7];
            const Vec3 mixed=mix(before_total,current_total);
            destination.position={float(double(mixed.x)-current.script_position.x-current.child_position.x),float(double(mixed.y)-current.script_position.y-current.child_position.y),float(double(mixed.z)-current.script_position.z-current.child_position.z)};
            applied=true;
        }
    }
    const float dx=std::remainder(current.rotation.x-before.rotation.x,tau);
    const float dy=std::remainder(current.rotation.y-before.rotation.y,tau);
    const float dz=std::remainder(current.rotation.z-before.rotation.z,tau);
    const bool rotation_eligible=(continuous&Rotation)!=0||((current.flags&4u)&&std::fabs(dz)<=1.5707964f);
    if(rotation_eligible&&std::fabs(dx)<=3.1415927f&&std::fabs(dy)<=3.1415927f){
        destination.rotation={before.rotation.x+dx*high_refresh::alpha,before.rotation.y+dy*high_refresh::alpha,before.rotation.z+dz*high_refresh::alpha};
        applied=true;
    }
    if((continuous&Scale)!=0&&before.scale.x*current.scale.x>=0&&before.scale.y*current.scale.y>=0){
        const float x=current.scale.x-before.scale.x,y=current.scale.y-before.scale.y;
        if(std::fabs(x)<=2.f&&std::fabs(y)<=2.f){destination.scale={high_refresh::lerp(before.scale.x,current.scale.x),high_refresh::lerp(before.scale.y,current.scale.y)};applied=true;}
    }
    if(((continuous&Rgb)!=0||all_near(before.color,current.color,64))&&all_near(before.color,current.color,96)){
        destination.color=(destination.color&0xff000000u)|channel(before.color,current.color,0)|channel(before.color,current.color,8)|channel(before.color,current.color,16);applied=true;
    }
    if(((continuous&Opacity)!=0||channel_near(before.color,current.color,24,64))&&channel_near(before.color,current.color,24,96)){
        destination.color=(destination.color&0x00ffffffu)|channel(before.color,current.color,24);applied=true;
    }
    if(((continuous&Rgb2)!=0||all_near(before.secondary_color,current.secondary_color,64))&&all_near(before.secondary_color,current.secondary_color,96)){
        destination.secondary_color=(destination.secondary_color&0xff000000u)|channel(before.secondary_color,current.secondary_color,0)|channel(before.secondary_color,current.secondary_color,8)|channel(before.secondary_color,current.secondary_color,16);applied=true;
    }
    if(((continuous&Opacity2)!=0||channel_near(before.secondary_color,current.secondary_color,24,64))&&channel_near(before.secondary_color,current.secondary_color,24,96)){
        destination.secondary_color=(destination.secondary_color&0x00ffffffu)|channel(before.secondary_color,current.secondary_color,24);applied=true;
    }
    if(before.sprite==current.sprite_index){
        const float u=std::remainder(current.uv_offset.x-before.uv.x,1.0f),v=std::remainder(current.uv_offset.y-before.uv.y,1.0f);
        if(((continuous&Uv)!=0||std::fabs(u)<=.125f)&&((continuous&Uv)!=0||std::fabs(v)<=.125f)){
            float nx=before.uv.x+u*high_refresh::alpha,ny=before.uv.y+v*high_refresh::alpha;
            if(nx>=1)nx-=1;else if(nx<0)nx+=1;if(ny>=1)ny-=1;else if(ny<0)ny+=1;
            destination.uv_offset={nx,ny};applied=true;
        }
    }
    if(applied){++stats[0];if(watched)++watch[8];}else ++stats[7];
    return applied;
}
}
#ifdef TH_PRESENTATION_AUDIT
extern "C" __attribute__((export_name("presentation_stats"))) const std::uint32_t* presentation_stats(){return th11::presentation::stats;}
extern "C" __attribute__((export_name("presentation_watch"))) void presentation_watch(std::uint32_t id){th11::presentation::watch_id=id;for(auto& value:th11::presentation::watch)value=0;}
extern "C" __attribute__((export_name("presentation_watch_stats"))) const std::uint32_t* presentation_watch_stats(){return th11::presentation::watch;}
#endif
