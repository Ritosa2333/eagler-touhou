#pragma once
#include "Types.hpp"
#include "AnmResource.hpp"
#include <vector>
#include <string>
#include <unordered_map>
namespace th19 {
// Global script location: createChild (opcode 500/501/502/504) references script
// ids that are GLOBAL across the whole .anm file (an entry's root script can
// create a child script that lives in a DIFFERENT entry). Each entry resolves
// to an absolute stream pointer + the entry's own script table (for grandchildren).
struct AnmScriptLoc { const u8* stream=nullptr; u32 len=0; const std::vector<AnmScript>* table=nullptr; };
// ANM v8 sprite-animation VM. Runs one script's instruction stream, ticking
// interpolation/velocity each frame and children after the parent.
struct AnmVm {
    // transform state
    float x=0,y=0,z=0;
    float rot_x=0,rot_y=0,rot_z=0;
    float sx=1,sy=1;
    i32 alpha=255;u8 r=255,g=255,b=255;
    i32 alpha2=255;u8 r2=255,g2=255,b2=255;
    i32 coordinate_mode=1; // ANM313: native position/geometry coordinate scale
    i32 color_mode=0;
    u32 color()const{return color_mode==1?(u32(alpha2)<<24)|(u32(r2)<<16)|(u32(g2)<<8)|b2:(u32(alpha)<<24)|(u32(r)<<16)|(u32(g)<<8)|b;}
    bool ignore_parent_transform=false; // ANM507, native flag 0x100
    i32 rotation_mode=0; // opcode 424: orientation relative to projectile velocity
    i32 render_mode=0;
    i32 special_effect=0;
    float effect_angle=0; // native float variable 10035 (+0x488), charge-point bend
    i32 address_u=1,address_v=1;
    i32 rotation_order=0;
    bool depth_write=true;
    float u_offset=0,v_offset=0,u_speed=0,v_speed=0;
    i32 blend_mode=0;              // 0 normal, 1 additive
    i32 layer=0;
    i32 sprite=-1;                 // global sprite id (>=0 means draw)
    u8 anchor_x=0, anchor_y=0;     // UV anchor (opcode 421): 0=center, 1=top-left
    // procedural effect (opcode 608): effect_type != 0 means no bitmap sprite —
    // draw a procedural glow instead. effect 0x16(22) = gold circular ripple.
    i32 effect_type=0;
    i32 geometry=0,segments=0; // native 604 circle, 606 horizontal gradient, 615 ring
    float geometry_radius=0,geometry_width=0,geometry_height=0;
    float geometry_stroke=0,geometry_corner=0,secondary_sx=1,secondary_sy=1,child_x=0,child_y=0;
    float effect_w=128.0f, effect_h=96.0f;
    bool visible=true;
    bool halted=false;             // stop / stop2 (pause instruction stream; interp continues)
    // angle velocity (opcode 415): rot += angleVel each frame
    float angle_vel_x=0, angle_vel_y=0, angle_vel_z=0;
    // ANM variable store (opcodes 100-121): float vars 10000+n, int vars 10000+n.
    // ids: 10000..10003 int, 10004..10007 float, 10010 signed angle, 10011 [0,1), 10012 [-1,1),
    //      10013/10014/10015 = x/y/z, 10038 random.
    float fvar[8]={0,0,0,0,0,0,0,0};
    i32  ivar[4]={0,0,0,0};
    i32 extra_ivar[3]={0,0,65536}; // native +0x48c/+0x490/+0x49c: 10008/10009/10029
    float child_var_x=0,child_var_y=0; // native +0x480/+0x484: 10033/10034
    float random_scale=1,random_angle_scale=3.141592654f; // +0x494/+0x498, 0x404cf0
    u16  cur_mask=0;               // param mask of the executing instruction
    // script state
    const u8* stream=nullptr;u32 stream_len=0;u32 ip=0;i32 time=0;
    std::vector<AnmVm> children;
    // ANM504 / 0x4d09b0: independent instance, captured before its first tick.
    bool detached_spawn=false;
    float spawn_x=0,spawn_y=0,spawn_rot_x=0,spawn_rot_y=0,spawn_rot_z=0;
    // global script id -> location (shared across all VMs in one .anm file)
    const std::unordered_map<i32, AnmScriptLoc>* global_scripts=nullptr;

    // interpolation state (posTime 407 / rgbTime 408 / alphaTime 409 / scaleTime 412)
    struct Interp {
        bool active=false;u32 dur=0;u32 elapsed=0;u32 mode=0;
        float v0=0,v1=0,v2=0;      // start values (3 channels)
        float t0=0,t1=0,t2=0;      // target values
        float tangent0[3]{},tangent1[3]{}; // ANM 420 / native vector mode 8
    } ip_pos, ip_rgb, ip_alpha, ip_scale, ip_rot, ip_rgb2, ip_alpha2, ip_secondary_scale;

    u32 return_ip=0; i32 return_time=0; bool return_halted=false,has_return=false;
    void tick(const AnmVm* parent=nullptr);
    void bind(const u8* s,u32 len){stream=s;stream_len=len;ip=0;time=0;}
    void bind_entry(const u8* s,u32 len,const std::unordered_map<i32,AnmScriptLoc>* table){
        bind(s,len);global_scripts=table;
    }
    // 中断：跳到流内 "case case_id" 标签并恢复执行（源码 FUN_004cb830(vm, case)）。
    void interrupt(i32 case_id);
    void interrupt_returning(i32 case_id);
    void interrupt_all(i32 case_id);
    // variable access helpers
    float read_var_f(i32 id) const;
    void  write_var_f(i32 id, float v);
    i32   read_var_i(i32 id) const;
    void  write_var_i(i32 id, i32 v);
    static float rand_unit();
    static void seed_random(u32 seed);
    static u32 random_checkpoint();
    static void restore_random(u32 state);
private:
    const AnmVm* execution_parent_=nullptr; // valid only while tick() executes; never retained across moves
    bool creation_registers_=false;
    void run_instruction(u16 op,u32 instr_start);
    void jump_to(i32 offset,i32 new_time);
    void advance_interp();
    u32 next_u16();i32 next_i16();u32 next_u32();float next_f32();
};
}
