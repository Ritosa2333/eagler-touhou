#pragma once
#include "AnmGeometry.hpp"
#include "AnmTrace.hpp"
#include "PortTitle.hpp"
#if PORT_TITLE == 14
#include "DeferredCaptureQueueTh14.hpp"
#if defined(TH14_CPP_STAGE_DECODE)
#include "ControlStateTh14.hpp"
#endif
#include "NativeStackTh14.hpp"
#include <functional>
#endif
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace th11 {
class AnmManager final:public AnmEnvironment {
public:
    ~AnmManager() override;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    // Native4f56c8 spans stage, title and ending owners. Generic ANM clear
    // must retain it; only the actual native producer sites reset this word.
    LoadingWordTh14<u32> th14_effect_stop=0;
    ControlPublicationTh14 th14_control;
    bool th14_control_vm_blocked()const noexcept override{
        const auto* control=th14_control.get();return control&&(u32(control->flags)&2u);
    }
#endif
#if PORT_TITLE == 14
    bool prepare_resource(AnmResource&,u16 file_id);
    std::function<bool(const AnmVm&)> before_initial_update;
    std::function<bool(const AnmVm&,Rng&,RngBoundary)> before_rng_boundary;
    bool th14_rng_boundary(const AnmVm& vm,Rng& rng,RngBoundary point)override{
        return (!before_rng_boundary||before_rng_boundary(vm,rng,point))&&
               (!before_initial_update||before_initial_update(vm));
    }
    bool th14_instruction_boundary(const AnmVm& vm)override{return !before_initial_update||before_initial_update(vm);}
    // One reference process context; clearing a scene does not clear its stack.
    NativeStackTh14 th14_native_stack;
    void th14_interpolation_scratch(u32 word,u32 mask=~u32(0))noexcept override{
        // This retained word models Main's ECL animation-factory stack.
        // Loading callbacks do not execute that Main factory; do not inspect
        // or mutate its counters/word from their separate native call stack.
        if(AnmExecutionContextTh14::owns(static_cast<AnmEnvironment*>(this)))return;
        if(th14_native_stack.ecl_animation_factory_depth&&th14_native_stack.enemy_spawn_depth==0&&depth()==1)
            th14_native_stack.ecl_factory_interpolation(word,mask);
    }
#endif
    AnmVm* create(AnmResource& resource,i32 script,u16 file_id,u32 layer,bool overlay=false,bool prepend=false,const Vec3* initial_position=nullptr,const AnmVm* detached_source=nullptr,AnmVm* attached_parent=nullptr,bool th14_playfield_layer=false,const float* initial_rotation=nullptr,u32 th16_initial_flags2=0);
    AnmVm* spawn(AnmVm& parent,i32 script,u32 opcode) override;
    AnmVm* find(u32 id) noexcept override;
    AnmVm* spawn_effect(AnmVm& source,i32 script) override;
    void destroy(AnmVm& vm);
    void clear(u32 keep_handle=0,u32 keep_family=0);
    void retire_resource(const AnmResource&);
    bool references(const AnmResource&)const noexcept;
    bool update(bool overlay);
#if PORT_TITLE == 14
    enum class VmBoundaryTh14:u32 {BeforeCall,AfterReturn};
    using VmBoundaryCallbackTh14=std::function<bool(const AnmVm&,VmBoundaryTh14)>;
    bool update(bool overlay,const std::function<bool()>& after_vm);
    bool update(bool overlay,const VmBoundaryCallbackTh14& boundary);
#endif
    bool change_draw_mode(AnmVm& vm) override;
    bool allocate_geometry(AnmVm& vm,u32 bytes) override;
    bool update_geometry(AnmVm& vm) override;
    void release_geometry(AnmVm& vm);
    u32 geometry_bytes(AnmVm& vm)const noexcept {
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif
const auto it=geometry_sizes.find(&vm);return it==geometry_sizes.end()?0:it->second;}
    AnmVm* layer_first(u32 layer)const noexcept {return layer<layers.size()?layers[layer]:nullptr;}
    u32 active_count()const noexcept {
#if defined(TH14_NATIVE_CRITICAL_SECTIONS)
    th14_critical_sections::ContainerGuard guard(th14_critical_sections::containers);
#endif
return active.size();}
    u32 depth()const noexcept {return spawn_depth;}
    u32 last_error=0;
    // Where inside its own script the failing instruction sat, for a report that has to
    // separate an unknown command from a walk that left the instruction stream.
    u32 last_error_offset=0;
    // The other three fields of that same instruction header, as the *loaded* bytes hold them. A
    // position reached through a jump, a command of zero length, and a byte the loader rewrote all
    // read differently there than in the title's file, so the report has to carry them to be
    // comparable with a dump of the file.
    u32 last_error_length=0;
    i32 last_error_time=0;
    u32 last_error_references=0;
    // The commands the interpreter dispatched last, oldest first, copied at the failure so the
    // report can say what led to the position instead of only naming it.
    u32 last_path_count=0;
    AnmTraceEntry last_path[anm_trace_size];
    // Which animation failed. An update failure happens to a VM the caller never named - it is found
    // by walking a layer - so the manager records the id and layer of the one that failed.
    u32 last_error_vm=0;
    u32 last_error_layer=0;
    // The resource and script the failing VM had bound, as the VM itself names them. The file's own
    // dump can then be compared script for script: without this, a report names only the shape of a
    // trajectory, and a position whose loaded bytes differ from the file's is hard to place.
    u32 last_error_file=0;
    u32 last_error_script=0;
    // The loaded bytes around that command: the word before it, the failing instruction, and the
    // word after. The failing header says one field differs from the title's file; a window says
    // how far the loaded copy and the file agree byte for byte, which is what separates a rewritten
    // field from a copy that no longer holds the file's bytes.
    u32 last_bytes_offset=0;
    u32 last_bytes_count=0;
    u8 last_bytes[24];
    // The loader's own hash of the bound script and a hash of the bytes as the vm's pointer holds
    // them now. Equal means the loader left the bytes as they are; different means something wrote
    // into a loaded script during the session, or that the pointer no longer holds it.
    u32 last_script_hash_load=0;
    u32 last_script_hash_now=0;
    u32 last_script_address=0;
    u32 last_resource_address=0;
    u32 last_script_size=0;
    // Those fields as one line, for a report that has to carry a manager failure upward.
    std::string last_error_text()const;
private:
    void record_failure(const AnmVm& vm);
    std::vector<std::unique_ptr<AnmVm>> storage;
    std::vector<AnmVm*> available;
    std::unordered_map<u32,AnmVm*> active;
    std::unordered_map<AnmVm*,u32> geometry_sizes;
    AnmVm::Node* heads[2]{},*tails[2]{};
    std::array<AnmVm*,kPortTitle==16?43:kPortTitle==14?36:kPortTitle==13?33:31> layers{};
    u32 next_id=0,spawn_depth=0;
#if PORT_TITLE == 14
    // 47fcb0 uses8191 pool slots;8191 identifies a heap allocation.
    // Preserve a pooled VM's slot across its LIFO retire/reuse cycle.
    std::unordered_map<AnmVm*,u32> th14_native_slots;
    u32 th14_unused_slots=8191;
#endif
#if PORT_TITLE == 14
public:
    // Native capture records belong to this manager, not to a menu, a
    // staged resource job, or a GPU completion counter. VM clear preserves it.
    DeferredCaptureQueueTh14 deferred_captures;
    i32 first_deferred_capture_file_index()const noexcept{return deferred_captures.first_file_index();}
    // Headless has no copy target. This explicitly retires request records,
    // never simulates Native GPU completion or a hardware/resource fence.
    void retire_deferred_captures_without_target(){deferred_captures.consume([](const auto&){});}
#endif
};
// Provenance of every animation creation, so a VM found on a render layer can be traced to the call
// that put it there. kind: 0 script, 1 layer, 2 file id, 3 resource present, 4 spawn depth, 5 vm id.
u32 anm_creation_count()noexcept;
i32 anm_creation_value(u32 index,u32 kind);
// The creation record of one live animation, by its id: what the call asked for, beside what the
// animation is bound to now. A difference is a rebind, and the two numbers name which one.
bool anm_creation_of(u32 id,i32& script,u32& layer,u32& file);
}
