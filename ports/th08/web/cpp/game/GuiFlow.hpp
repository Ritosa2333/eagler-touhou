#pragma once
#include "GuiController.hpp"
#include "Chain.hpp"
namespace th08 {
struct GuiLoadContext {bool initial=true,keep_resources=false,release_resources=true;u8 section_warp=0;i32 spell_number=0;};
struct GuiResources {
    virtual ~GuiResources()=default;
    virtual AnmLoaded* load(i32 index,const char* path)=0;
    virtual void release(i32 index)=0;
    virtual std::vector<u8> message(const char* path)=0;
};
class GuiFlow {
public:
    GuiFlow(GuiController& controller,Dialogue& dialogue,GuiResources& resources):controller(controller),dialogue(dialogue),resources(resources){}
    ~GuiFlow(){detach();}
    GuiLoadContext context;
    bool setup();
    void release();
    void update();
    void draw();
    bool attach(Chain& owner);
    void detach();
private:
    GuiController& controller;Dialogue& dialogue;GuiResources& resources;
    Chain* chain=nullptr;ChainElement calculation,drawing;
};
}
