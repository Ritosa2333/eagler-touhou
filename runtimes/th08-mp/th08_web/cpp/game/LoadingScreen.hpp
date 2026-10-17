#pragma once
#include "AnmLibrary.hpp"
#include "AsciiManager.hpp"
namespace th08 {
struct LoadingActions {
    virtual ~LoadingActions()=default;
    virtual void capture_screen()=0;
    virtual bool has_capture()=0;
    virtual void draw_capture()=0;
    virtual void release_capture()=0;
    virtual void draw_text()=0;
};
class LoadingScreen {
    AnmLibrary& library;AnmExecutor& animations;AnmRenderer& renderer;AsciiManager& ascii;LoadingActions& actions;
public:
    i32 phase=0;AnmVm vms[3];bool software_texturing=false;
    LoadingScreen(AnmLibrary& l,AnmExecutor& a,AnmRenderer& r,AsciiManager& t,LoadingActions& p):library(l),animations(a),renderer(r),ascii(t),actions(p){}
    bool show(const Vec3&,bool capture=false);
    void fade();void hide();void update();void draw();void background();
};
}
