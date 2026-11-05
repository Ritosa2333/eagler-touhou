#pragma once
#include "BackgroundObjects.hpp"
#include "Chain.hpp"
#include <array>
namespace th08 {
class BackgroundScript;
struct BackgroundDrawActions {
    virtual ~BackgroundDrawActions()=default;
    virtual bool stage_finished()=0;
    virtual void moon(AnmVm& effect)=0;
    virtual void effects()=0;
};
class BackgroundView {
public:
    BackgroundView(BackgroundState& state,BackgroundScript& script,AnmRenderer& renderer,BackgroundDrawActions& actions):state(state),script(script),renderer(renderer),objects(state,renderer),actions(actions){}
    JobResult high();JobResult low();
    void snapshot_spell_presentation();
    AnmVm presentation_spell_vm(u32 index)const;
    void* callback_context=nullptr;
private:
    BackgroundState& state;BackgroundScript& script;AnmRenderer& renderer;BackgroundObjects objects;BackgroundDrawActions& actions;
    struct PresentationState {i32 spell_flag=0;ZunColor tint_color{};i32 use_tint=0,effect_visible=0;bool valid=false;} presentation;
    std::array<AnmVm,32> presentation_spell_vms{};bool presentation_spell_valid=false;
    th08::presentation::SnapshotMarker presentation_marker;
    struct RenderRestore {i32 spell_flag=0;ZunColor tint_color{};i32 use_tint=0,effect_visible=0,effect_flags=0;std::array<Vec3,32> effect_positions{};bool active=false;} restore;
    SceneCamera saved_camera{};bool camera_override=false;
    void layer(AnmVm& vm){renderer.draw_2d(vm);renderer.flush();}
    void rectangle(u32 color){const u32 colors[4]={color,color,color,color};renderer.draw_rectangle(32,16,416,464,colors);}
};
}
