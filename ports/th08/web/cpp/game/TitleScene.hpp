#pragma once
#include "TitleFlow.hpp"
#include "TitleView.hpp"
#include "TitleInformation.hpp"
#include "GameplaySession.hpp"
#include "ScreenEffects.hpp"
#include "AnmLibrary.hpp"
namespace th08 {
struct TitlePlatform:TitleActions,TitleInformationActions {
    virtual std::vector<u8> read_asset(const char* path)=0;
    virtual std::vector<u8> read_score()=0;
    virtual void discard_graphics()=0;
    virtual void show_loading(bool capture)=0;
    virtual void start_transition_effect()=0;
    virtual void fade_loading()=0;
    virtual bool capture_pending()=0;
    void background()override=0;
    void process_sounds()override=0;
};
// Owns the title chain and the original modal unlock notices. The platform
// supplies images, input, text and audio; menu decisions remain in C++.
class TitleScene:private TitleFlowActions {
    GameplaySession& session;AnmLibrary& library;AnmRenderer& renderer;TitlePlatform& platform;ScreenEffects& screen;AnmExecutor animations;
    Chain* chain=nullptr;ChainElement calculation,drawing;bool pending_load=false,failed=false;
    std::vector<u8> read_score()override{return platform.read_score();}
    AnmLoaded* preload_animation(i32 index,const char* path)override;
    bool preload_background(const char* path)override{return platform.load_surface(0,path)==0;}
    void loading(bool capture)override;
    void start_effect()override{platform.start_transition_effect();}
    void fade_in()override{screen.create(ScreenEffectType::FadeIn,70,0xffffff,0,0,21);}
    void fade_loading()override{platform.fade_loading();}
    void begin_loading()override;
    void publish_records();void release();bool setup();
public:
    TitleContext context;TitleMenus menus;TitleFlow flow;TitleView view;TitleInformation information;
    TitleScene(GameplaySession&,AnmLibrary&,AnmRenderer&,AsciiManager&,const AsciiContext&,TextWriter&,ScreenEffects&,TitlePlatform&);
    ~TitleScene(){detach();}
    bool attach(Chain&);void detach();
    bool service();bool modal_step();
    bool modal()const{return information.active;}
    bool active()const{return chain!=nullptr;}
    bool invalid()const{return failed||animations.invalid;}
    void input(const InputFrame& input,const FrameTiming& timing){context.input=input;animations.timing=timing;}
};
}
