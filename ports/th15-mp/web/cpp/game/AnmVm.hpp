#pragma once
#include "AnmResource.hpp"
#include "AnmVariables.hpp"
#include "AnmVisualState.hpp"
#include "AnmInterpolators.hpp"
#include "AnmEnvironment.hpp"
#include "AnmGeometry.hpp"
#include <string>
#include <type_traits>
namespace th15 {
struct AnmSpriteSource {
    virtual ~AnmSpriteSource()=default;
    virtual i32 sprite_index(i32 parameter)const noexcept=0;
};
class AnmVm;
struct AnmObjectHost {
    virtual ~AnmObjectHost()=default;
    virtual AnmVm* spawn_child(AnmVm& source,i32 script,u32 ordering)=0;
    virtual AnmVm* spawn_detached(AnmVm& source,i32 script)=0;
    virtual bool spawn_effect(AnmVm& source,i32 script)=0;
    virtual bool release_tree(AnmVm&)=0;
    virtual i32 update_effect(AnmVm& source,float rate){return -1;}
    virtual bool interrupt_effect(AnmVm& source,i32 label,float rate){return true;}
};
// Animation instruction execution. Drawing/spawning commands are integration
// work; unsupported instructions stop explicitly and are never silently skipped.
// Motion is written each tick. Curve and atlas state changes on commands or
// active interpolation, and carries a monotonic identity across corrections.
class AnmVmMotion {
    friend class AnmFile;
    friend class AnmVm;
    std::vector<u8>* script=nullptr;u32 saved_offset=0;Timer saved_timer{};
public:
    inline static u64 presentation_counter=0;
    u64 presentation_generation=++presentation_counter;bool presentation_motion=false;u32 presentation_part=0;
    AnmVariables variables;Timer timer,age;i32 instruction_offset=-1;bool visible=false;
    i32 pending_interrupt=0;
    float slowdown=0;
    AnmEnvironment* environment=nullptr;AnmVm* rotation_parent=nullptr;AnmVm* creation_parent=nullptr;
    AnmObjectHost* object_host=nullptr;
    // Resource scripts can select logical sprite parameters. Bullet instances
    // resolve these against their current type and colour. Binding a new script
    // clears the source; its owner installs it after the bind.
    const AnmSpriteSource* sprite_source=nullptr;
    AnmResource* resource=nullptr;const AnmSprite* sprite=nullptr;AnmResource* sprite_resource=nullptr;
    i32 source_script=-1;
    u32 ordering=0;
    u64 program_version=0;bool interpolation_idle=false;
};
static_assert(std::is_trivially_copyable_v<AnmVmMotion>);
struct AnmVmState:AnmVmMotion {AnmVisualState visual;AnmInterpolators interpolators;};
static_assert(std::is_trivially_copyable_v<AnmVmState>);
class AnmVm:public AnmVmState {
    friend class AnmFile;
    inline static u64 program_serial=0;
    void changed_program()noexcept{program_version=++program_serial;}
    int execute(Rng&,float rate,bool tracked_interpolators);
public:
    std::string error;
    AnmGeometry geometry;
    void copy_checkpoint(const AnmVm& source){*this=source;timer.set(source.timer.current);age.set(source.age.current);rotation_parent=creation_parent=nullptr;}
    bool bind(AnmScript&);int tick(Rng&,float rate=1);
    int tick_tracked(Rng&,float rate=1);
    bool bind(AnmResource&,u32 script);bool select_sprite(i32 index)noexcept;
    void prepare_template()noexcept{timer.set(-1);age.set(-1);}
    void reset_runtime_timers()noexcept{timer.set(0);age.set(0);}
    void initialize_unbound()noexcept{saved_timer={0,0,0,0,0};saved_offset=0;instruction_offset=0;visual.render_flags=0x4000;visual.sprite_matrix.identity();timer=Timer{};age=Timer{};}
    bool advance(float rate,bool tracked_interpolators=false);
    Vec3 total_rotation(u32 depth=0)noexcept;
    float effective_slowdown()const noexcept;
};
}
