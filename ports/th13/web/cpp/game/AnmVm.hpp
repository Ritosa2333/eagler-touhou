// ANM 虚拟机：脚本指令执行载体（生命周期见 AnmManager/AnmRegistry）。
// th13 差异（相对 th15 参照实现，依据 docs/plans/anm-pipeline.md §四/§五）：
//   - 前置回调 pre_tick（原版 vm+0x590，00462450.c:293-300：非零返回即整帧跳过）；
//   - 变量表无 10033-10035（见 AnmVariables.hpp）；
//   - 10026 父链旋转只看一层父（00461e80.c case 0x272a），不递归。
#pragma once
#include "AnmResource.hpp"
#include "AnmVariables.hpp"
#include "AnmVisualState.hpp"
#include "AnmInterpolators.hpp"
#include "AnmEnvironment.hpp"
#include "AnmGeometry.hpp"
#include <string>
namespace th13 {
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
    virtual i32 update_effect(AnmVm&,float){return -1;}
    virtual bool interrupt_effect(AnmVm&,i32,float){return true;}
};
// Animation instruction execution. Drawing/spawning commands are integration
// work; unsupported instructions stop explicitly and are never silently skipped.
class AnmVm {
    friend class AnmFile;
    std::vector<u8>* script=nullptr;u32 saved_offset=0;Timer saved_timer{};
public:
    inline static u64 presentation_counter=0;
    u64 presentation_generation=++presentation_counter;bool presentation_motion=false;u32 presentation_part=0;
    AnmVariables variables;AnmVisualState visual;AnmInterpolators interpolators;Timer timer,age;i32 instruction_offset=-1;bool visible=false;std::string error;
    i32 pending_interrupt=0;
    AnmGeometry geometry;
    float slowdown=0;
    // 前置回调（原版 vm+0x590）：tick 入口最先调用，非零返回则本帧不执行任何指令。
    i32 (*pre_tick)(AnmVm&)=nullptr;
    AnmEnvironment* environment=nullptr;AnmVm* rotation_parent=nullptr;AnmVm* creation_parent=nullptr;
    AnmObjectHost* object_host=nullptr;
    // Resource scripts can select logical sprite parameters. Bullet instances
    // resolve these against their current type and colour. Binding a new script
    // clears the source; its owner installs it after the bind.
    const AnmSpriteSource* sprite_source=nullptr;
    AnmResource* resource=nullptr;const AnmSprite* sprite=nullptr;AnmResource* sprite_resource=nullptr;
    i32 source_script=-1;
    i32 local_script_index(i32 index)const noexcept {
        return index<0||!resource||source_script<0 ? index :
            i32(resource->scripts[u32(source_script)].script_base)+index;
    }
    // Keep visual.sprite equal to the original file-local VM field. The
    // renderer and diagnostics share the resolved sprite, including fallback
    // selections and scripts that draw before their first sprite instruction.
    const AnmSprite* texture_sprite()const noexcept {
        if(sprite)return sprite;
        const auto* source=sprite_resource?sprite_resource:resource;
        if(!source||visual.sprite<0)return nullptr;
        u32 index=u32(visual.sprite);
        if(source==resource&&source_script>=0)
            index+=source->scripts[u32(source_script)].sprite_base;
        return index<source->sprites.size()?&source->sprites[index]:nullptr;
    }
    u32 ordering=0;
    void copy_checkpoint(const AnmVm& source){*this=source;timer.set(source.timer.current);age.set(source.age.current);rotation_parent=creation_parent=nullptr;}
    bool bind(AnmScript&);int tick(GameRandom&,float rate=1);
    bool bind(AnmResource&,u32 script);bool select_sprite(i32 index)noexcept;
    void prepare_template()noexcept{timer.set(-1);age.set(-1);}
    void reset_runtime_timers()noexcept{timer.set(0);age.set(0);}
    void initialize_unbound()noexcept{saved_timer={0,0,0,0,0};saved_offset=0;instruction_offset=0;visual.render_flags=0x4000;visual.sprite_matrix.identity();timer=Timer{};age=Timer{};}
    bool advance(float rate);
    Vec3 total_rotation(u32 depth=0)noexcept;
    float effective_slowdown()const noexcept;
};
}
