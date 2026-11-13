#pragma once
#include "AnmResource.hpp"
#include "AnmTrace.hpp"
#include "PortTitle.hpp"
#include "Interpolation.hpp"
#include "Rng.hpp"
#include "AnmExecutionContextTh14.hpp"
#include "AnmFlagWordTh14.hpp"
#include "LoadingWordTh14.hpp"
namespace th11 {
struct AnmEnvironment;
// Native TH14 display globals, expressed in the backend's logical space.
// The current browser canvas is 640x480: both origin selections are (320,16).
// Callers with another logical resolution can supply its scale and origins.
struct AnmRenderSpace {float scale=1;Vec2 play_origin{320,16},scaled_origin{320,16};};
struct Matrix4 {float m[16];void identity() noexcept {std::memset(m,0,sizeof m);m[0]=m[5]=m[10]=m[15]=1;}};
struct AnmVm {
    u32 id;
    struct Node {AnmVm* value;Node* next;Node* previous;} registry,child;
    AnmVm* draw_next;
    u32 layer;
    Vec3 rotation,angular_velocity;
    Vec2 scale,scale_velocity,sprite_size,uv_offset;
    Timer timer;
    Vec2 uv[4];
    Vec3Interpolator position_interpolation;
    RgbInterpolator color_interpolation;
    AlphaInterpolator alpha_interpolation;
    Vec3Interpolator rotation_interpolation;
    Vec2Interpolator scale_interpolation;
    RgbInterpolator color2_interpolation;
    AlphaInterpolator alpha2_interpolation;
    FloatInterpolator scroll_x_interpolation,scroll_y_interpolation;
    Vec2 scroll_velocity;
    Matrix4 sprite_matrix,transform_matrix,uv_matrix;
    u32 color,secondary_color;
#if PORT_TITLE == 14
    LoadingWordTh14<i32> pending_interrupt;
#else
    i16 pending_interrupt;u16 reserved_37e;
#endif
    Timer saved_timer;
    AnmInstruction* saved_instruction;
    i32 sprite_frame;
    i16 sprite_index;u16 file_index,reserved_3a0; i16 script_index;
    AnmInstruction* script_begin;AnmInstruction* instruction;
    const AnmSprite* sprite;AnmResource* resource;
    i32 integers[4];float floats[4];i32 extra_integers[2];
    Vec3 script_position,position,child_position;
    void* geometry;
    // Native TH13 flags that have no matching bit in the older canonical VM.
    AnmFlagWordTh14 flags,flags2,th13_native_flags;
    void (*before_update)(AnmVm&);
    void (*draw_callback)(AnmVm&);
    u32 reserved_418;
    u8 rectangle_columns,rectangle_rows;u16 reserved_41e;
    // TH13's live parent transform (+570 in its native VM). Keep the shared
    // 32-bit comparison layout by using the former reserved words.
    AnmVm* parent;u32 th13_effect;
    int (*after_update)(AnmVm&);
    i32 (*sprite_callback)(AnmVm&,i32);
    u32 reserved_430;
#if PORT_TITLE == 13 || PORT_TITLE == 14
    // Modern ANM state follows the comparison prefix; TH12 retains its ABI.
    Vec2 extra_scale,texture_scale,pivot;
    Vec2Interpolator extra_scale_interpolation,texture_scale_interpolation;
    Vec3 native_prefix_position;
#endif
#if PORT_TITLE == 16
    Vec2 pivot;
    Vec2 extra_scale;
    Vec2 texture_scale;
    Vec2Interpolator texture_scale_interpolation;
    u32 th16_callbacks[6];
    // Remaining six words of the native 64-byte variable-copy region.
    // Keep them opaque until their individual resolver semantics are proved.
    u32 th16_variable_tail[6];
    float slowdown_factor;
    // Native+5ac transforms/slowdown, distinct from the+5b0 scale parent.
    AnmVm* th16_transform_parent;
#endif
#if PORT_TITLE == 14 || PORT_TITLE == 16
    // A negative sprite command binds the global ASCII fallback without
    // changing the script's owner resource (which child creation still needs).
    AnmResource* sprite_resource;
#endif
#if PORT_TITLE == 14
    Vec3 extra_floats;
    Vec2 random_scale;
    i32 random_bound;
    float slowdown_factor;
    Timer age;
    u32 inherited_color;
    // Retail +4f8..524: last quad submitted, written before visibility clipping.
    Vec3 screen_quad_positions[4];
    void* dialogue_owner;
#endif
    void initialize() noexcept;
    bool bind_sprite(i32 index) noexcept;
    bool bind_sprite_from(AnmResource& file,i32 index) noexcept;
    AnmResource* bound_sprite_resource() const noexcept {
#if PORT_TITLE == 14 || PORT_TITLE == 16
        if(sprite_resource)return sprite_resource;
#endif
        return resource;
    }
    bool bind_script(AnmResource& file,i32 script,u16 file_id,const float* rate) noexcept;
#if PORT_TITLE == 14
    bool copy_script(AnmResource& file,i32 script,u16 file_id,const float* rate) noexcept;
#endif
    int update(AnmEnvironment& env);
    // This VM's own dispatch ring (AnmTrace.hpp). A push that repeats the previous entry is dropped:
    // a command of length zero would otherwise fill the ring with itself, and the ring holds the
    // distinct positions the interpreter visited, which is what a path is.
    void push_trace(u32 offset,u16 opcode,u16 length) noexcept {
        if(trace_count){
            const AnmTraceEntry& last=trace[(trace_count-1)&(anm_trace_size-1)];
            if(last.offset==offset&&last.opcode==opcode)return;
        }
        trace[trace_count++&(anm_trace_size-1)]={offset,opcode,length};
    }
    // Port-only fields, so they live in the extension (see the layout assertion below) rather than in
    // the region the original's own structure occupies.
    AnmTraceEntry trace[anm_trace_size] = {};
    u32 trace_count = 0;
    i32 integer_value(i32 value,AnmEnvironment& env);
    double float_value(float value,AnmEnvironment& env);
    i32* integer_destination(i32* argument) noexcept;
    float* float_destination(float* argument) noexcept;
    Vec3 render_position(const AnmRenderSpace& space={}) const noexcept;
    Vec3 transform_position(Vec3 local,const AnmRenderSpace& space={}) const noexcept;
    void interrupt(i32 label) noexcept;
    float inherited_slowdown() const noexcept;
    u32 blend_mode() const noexcept {return ((flags>>4)&7)|(kPortTitle==14||kPortTitle==16?((th13_native_flags>>5)&8):0);}
    u32 drawing_mode() const noexcept {return ((flags>>22)&15)|(kPortTitle==13||kPortTitle==14||kPortTitle==16?((th13_native_flags>>25)&16):((flags>>25)&16));}
    // This title's own 302 shifts the whole five-bit mode by 0x19 into its context field (0x4623dc), and
    // this title's own update reads the fifth bit back out of flags. The low four bits live at 22..25
    // here; without the fifth being written the mode's top bit is set by nobody.
    void set_drawing_mode(u32 mode) noexcept {flags=(flags&~0x3c00000u)|((mode&15)<<22);if constexpr(kPortTitle==13||kPortTitle==14||kPortTitle==16)th13_native_flags=(th13_native_flags&~0x20000000u)|((mode&16)<<25);}
};
// Layout assertions apply to the 32-bit comparison/WebAssembly target only.
#if UINTPTR_MAX == UINT32_MAX
inline constexpr u32 kAnmVmExtensionBytes=((kPortTitle==13||kPortTitle==14)?3*sizeof(Vec2)+2*sizeof(Vec2Interpolator)+sizeof(Vec3):0)+(kPortTitle==16?3*sizeof(Vec2)+sizeof(Vec2Interpolator)+12*sizeof(u32)+sizeof(float)+sizeof(AnmVm*):0)+((kPortTitle==14||kPortTitle==16)?sizeof(AnmResource*):0)+(kPortTitle==14?sizeof(Vec3)+sizeof(Vec2)+sizeof(i32)+sizeof(float)+sizeof(Timer)+sizeof(u32)+sizeof(void*)+4*sizeof(Vec3):0)+sizeof(AnmTraceEntry)*anm_trace_size+sizeof(u32);
static_assert(sizeof(AnmVm)==0x434+kAnmVmExtensionBytes);
static_assert(offsetof(AnmVm,uv)==0x70);
static_assert(offsetof(AnmVm,position_interpolation)==0x90);
static_assert(offsetof(AnmVm,color)==0x374);
static_assert(offsetof(AnmVm,instruction)==0x3a8);
static_assert(offsetof(AnmVm,flags)==0x404);
#endif
#if PORT_TITLE == 14
struct AnmTemplatesTh14 {std::vector<AnmVm> scripts;};
#endif
struct AnmEnvironment {
    AnmRateTh14 rate=1;
    Rng script_rng,visual_rng;
#if PORT_TITLE == 14
    // Main ANM thread: device creation selects PC24, floating RNG helpers
    // 403470/4034b0 execute FNINIT to PC64. Integer draws leave it unchanged.
    bool texture_high_precision=false;
    bool& th14_execution_precision()noexcept{
        return AnmExecutionContextTh14::precision(this,texture_high_precision);
    }
#endif
    Vec3 reference_positions[2]{},camera_delta{},default_tangent{};
    AnmResource* fallback_sprites=nullptr;
    AnmRenderSpace render_space;
    virtual ~AnmEnvironment()=default;
    Rng& random(const AnmVm& vm) noexcept {return vm.flags&0x40000000?visual_rng:script_rng;}
    // Effects requiring a manager or renderer are explicit integration points.
    // Returning false aborts the update rather than silently dropping an effect.
#if PORT_TITLE == 14
    // Initialization can complete between executed ANM instructions.
    virtual bool th14_instruction_boundary(const AnmVm&){return true;}
    virtual bool th14_rng_boundary(const AnmVm&,Rng&,RngBoundary){return true;}
    virtual void th14_interpolation_scratch(u32,u32 mask=~u32(0))noexcept{}
#endif
    virtual AnmVm* spawn(AnmVm&,i32,u32){return nullptr;}
    virtual AnmVm* spawn_effect(AnmVm&,i32){return nullptr;}
    virtual AnmVm* find(u32) noexcept{return nullptr;}
    virtual bool change_draw_mode(AnmVm&){return false;}
#if PORT_TITLE == 16
    // Retail 469e20 is a separate initializer, not TH12's ripple factory.
    virtual bool th16_mode10(AnmVm&){return false;}
    // Distinguish an unavailable port callback from a retail callback asking
    // for retirement. Retail treats every nonzero callback return as one.
    static constexpr int th16_callback_unavailable=INT32_MIN;
    virtual int th16_pre_update(AnmVm&){return th16_callback_unavailable;}
#endif
    virtual bool allocate_geometry(AnmVm&,u32){return false;}
    virtual bool update_geometry(AnmVm&){return false;}
    virtual bool screen_uv(AnmVm&);
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    virtual bool th14_control_vm_blocked()const noexcept{return false;}
#endif
};
}
