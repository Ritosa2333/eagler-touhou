#pragma once
#include "StageResource.hpp"
#include "GameMath.hpp"
#include <array>
namespace th09 {
struct BackgroundCamera {Vec3 position,direction,up,unit_direction,offset;float field_of_view=0;};
struct BackgroundFog {float near_plane=0,far_plane=0;u32 color=0;};
struct BackgroundServices {
    FrameTiming timing;virtual ~BackgroundServices()=default;
    virtual void start_animation(AnmVm&,bool boss,i32 script)=0;
    virtual void advance_animation(AnmVm&)=0;
};
class Background {
    BackgroundServices& services;
    float interpolation(u32);
    void interpolate_camera();
public:
    explicit Background(BackgroundServices& s):services(s){for(auto& a:overlays)std::memset(&a,0,sizeof(a));for(auto& a:boss_animations)std::memset(&a,0,sizeof(a));}
    StageResource resource;std::vector<AnmVm> primitives;
    BackgroundCamera camera;Vec3 goals[3]{},starts[3]{},end_tangents[3]{},start_tangents[3]{};
    float fov_goal=0,fov_start=0;i32 durations[5]{};Timer interpolation_times[5]{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};i32 interpolation_modes[5]{};
    Timer script_time{0,0,0};i32 instruction=0,frame=0;Vec3 position;
    u32 clear_color=0;BackgroundFog fog,fog_start,fog_goal;i32 fog_duration=0;Timer fog_time{0,0,0};
    i32 requested_label=0,transition_state=0,transition_frames=0;
    std::array<AnmVm,3> overlays;std::array<AnmVm,32> boss_animations;
    i32 boss_state=0,boss_frames=0,boss_count=0,boss_parameter=0;u32 tint=0;
    Vec3 next_position; i32 next_position_time=0;Vec3 previous_position;i32 previous_position_time=0;u8 jumped=0,sway=0;
    float distance_squared=0;bool invalid=false;
    bool load(const u8*,u32,i32 background);
    void jump_label();
    void execute_script();
    void update(u32 game_flags,u32 field_flags);
    void begin_boss(i32 script){boss_state=1;boss_frames=0;for(i32 n=0;n<boss_count&&n<32;++n)services.start_animation(boss_animations[n],true,script+boss_parameter+n);}
    void end_boss(){boss_state=0;}
};
}
