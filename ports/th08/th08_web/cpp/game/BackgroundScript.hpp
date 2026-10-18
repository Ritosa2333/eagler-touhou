#pragma once
#include "BackgroundState.hpp"
#include "Chain.hpp"
#include "Presentation.hpp"
namespace th08 {
struct BackgroundContext {i32 stage=0;bool paused=false,practice=false,youkai=false;};
struct BackgroundActions {
    virtual ~BackgroundActions()=default;
    virtual AnmVm* moon()=0;
    virtual void move_effects(const Vec3& offset)=0;
    virtual void sparkle(const Vec3& position)=0;
    virtual bool integrity_failed()=0;
};
class BackgroundScript {
public:
    BackgroundScript(BackgroundState& state,BackgroundContext& context,AnmExecutor& executor,BackgroundActions& actions)
        :state(state),context(context),anm(executor),actions(actions){}
    StageProgram program;
    bool load(const u8* bytes,u32 size,AnmLoaded& background,AnmLoaded& text);
    void reset_camera();
    void release(bool keep_program=false);
    JobResult update();
    void update_objects();
    void interpolate(u32 index,Vec3& output,const Vec3& initial,const Vec3& final,const Vec3& initial_derivative,const Vec3& final_derivative);
    void tint(u32 color);
    SceneCamera presentation_camera()const;
#if defined(TH_PRESENTATION_AUDIT)
    bool audit_cameras(SceneCamera& previous,SceneCamera& current,bool& rebased)const;
#endif
    bool invalid=false;
    BackgroundState& state;BackgroundContext& context;
private:
    AnmExecutor& anm;BackgroundActions& actions;std::vector<AnmVm> quads;
    SceneCamera presentation_previous_camera{};bool presentation_camera_valid=false,presentation_camera_rebased=false;
    presentation::SnapshotMarker presentation_marker;
    void start(AnmLoaded* file,AnmVm& vm,i32 script,bool base_index);
    float progress(u32 index,bool vector);
    void finish_frame();
};
}
