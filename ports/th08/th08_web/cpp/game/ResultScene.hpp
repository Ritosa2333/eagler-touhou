#pragma once
#include "ResultView.hpp"
namespace th08 {
struct ResultPlatform:ResultActions {
    virtual std::vector<u8> read_asset(const char* path)=0;
    virtual std::vector<u8> read_score()=0;
    virtual bool write_score(const u8* data,u32 size)=0;
    virtual bool load_result_background()=0;
    virtual void release_result_background()=0;
};
class ResultScene {
    GameplaySession& session;AnmLibrary& library;AnmRenderer& renderer;ResultPlatform& platform;AnmExecutor animations;
    Chain* chain=nullptr;ChainElement calculation,drawing;bool resources=false,loaded=false,failed=false;
    void release();bool save();
public:
    ScoreStore scores;ResultScreen controls;ResultView view;
    ResultScene(GameplaySession& s,AnmLibrary& l,AnmRenderer& r,AsciiManager& a,TextWriter& t,ResultPlatform& p):session(s),library(l),renderer(r),platform(p),animations(s.random),scores(s),controls(s,scores,animations,t,p),view(controls,a,r){}
    ~ResultScene(){detach();}
    bool attach(Chain&,ResultScreenAction,const ResultContext&);
    void detach();bool invalid()const{return failed||animations.invalid;}
    bool active()const{return loaded;}
    void input(const InputFrame& value,const FrameTiming& timing){controls.context.input=value;animations.timing=timing;}
};
}
