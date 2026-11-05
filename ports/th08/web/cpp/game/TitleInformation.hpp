#pragma once
#include "AnmRenderer.hpp"
#include "InputController.hpp"
namespace th08 {
struct TitleInformationActions {
    virtual ~TitleInformationActions()=default;
    virtual void load_image(const char* path)=0;
    virtual void release_image()=0;
    virtual void begin_frame()=0;
    virtual void background()=0;
    virtual u16 poll_input()=0;
    virtual bool present()=0;
    virtual void reset_device()=0;
    virtual void confirm_sound()=0;
    virtual void process_sounds()=0;
};
// The original modal loop is represented one presented frame at a time so
// that the browser can service events between frames without changing timing.
class TitleInformation {
public:
    TitleInformation(AnmRenderer& renderer,InputFrame& input,TitleInformationActions& actions):renderer(renderer),input(input),actions(actions){}
    i32 frame=0;bool active=false;
    void start(const char* path);
    bool step();
private:
    AnmRenderer& renderer;InputFrame& input;TitleInformationActions& actions;
};
}
