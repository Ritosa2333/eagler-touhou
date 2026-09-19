#include "TitleScene.hpp"
namespace th08 {
TitleScene::TitleScene(GameplaySession& s,AnmLibrary& l,AnmRenderer& r,AsciiManager& a,const AsciiContext& ac,TextWriter& t,ScreenEffects& e,TitlePlatform& p)
 :session(s),library(l),renderer(r),platform(p),screen(e),animations(s.random),menus(context,s.display_config,animations,t,p),flow(menus,a,ac,*this),view(menus,a,r),information(r,context.input,p){menus.practice=&s.practice;}
AnmLoaded* TitleScene::preload_animation(i32 index,const char* path){const auto bytes=platform.read_asset(path);return library.load(index,bytes.data(),bytes.size());}
void TitleScene::loading(bool capture){platform.show_loading(capture);}
void TitleScene::begin_loading(){pending_load=true;}
void TitleScene::publish_records(){
    std::memcpy(session.records,context.spells,sizeof(session.records));std::memcpy(session.clears,context.clears,sizeof(session.clears));std::memcpy(session.practices,context.practice_scores,sizeof(session.practices));session.last_words=context.last_words;
}
bool TitleScene::setup(){
    if(!pending_load)return true;flow.state.capture_pending=platform.capture_pending();if(!flow.setup())return true;
    pending_load=false;publish_records();failed|=menus.state.state==TitleScreenState_Close;return !invalid();
}
bool TitleScene::attach(Chain& owner){
    if(chain)return false;menus.release();menus.state.reset();flow.state={};animations.invalid=failed=false;pending_load=false;
    context.textAnm=library.get(0);if(!context.textAnm)return false;
    std::memcpy(context.spells,session.records,sizeof(session.records));std::memcpy(context.clears,session.clears,sizeof(session.clears));std::memcpy(context.practice_scores,session.practices,sizeof(session.practices));context.last_words=session.last_words;context.play=session.statistics;
    const char* notice=flow.begin();session.config=flow.game_config;session.numbers=flow.game_values;
    chain=&owner;if(notice)information.start(notice);else if(!setup()){release();return false;}
    calculation.set_callback([](void* p){auto& s=*static_cast<TitleScene*>(p);if(s.information.active||s.pending_load)return JobResult::Continue;
        auto& practice=s.session.practice;
        if(practice.enabled&&s.menus.state.currentScreen==TitleCurrentScreen_PracticeStageSelect){
            if(practice.accepted){practice.accepted=false;practice.menu=false;s.context.currentStage=practice.run.stage;
                if(practice.run.stage==8)s.context.difficulty=4;s.context.flags.isPracticeMode=true;s.context.flags.isReplay=false;s.context.flags.isSpellPractice=false;
                s.context.supervisor_state=2;s.platform.stop_audio();return JobResult::Remove;}
            practice.menu=true;s.menus.state.practiceState=0;return JobResult::Continue;
        }
        practice.menu=false;const auto result=s.menus.update();return s.invalid()?JobResult::Error:result;});calculation.argument=this;calculation.deleted=[](void* p){static_cast<TitleScene*>(p)->release();return 0;};
    drawing.set_callback([](void* p){auto& s=*static_cast<TitleScene*>(p);if(!s.information.active)s.view.draw();return s.invalid()?JobResult::Error:JobResult::Continue;});drawing.argument=this;
    menus.state.calcChain=&calculation;menus.state.drawChain=&drawing;owner.add(&calculation,4);owner.add(&drawing,3,true);return true;
}
bool TitleScene::service(){return !invalid()&&setup();}
bool TitleScene::modal_step(){
    if(!information.active)return false;if(information.step())return true;
    if(const char* next=flow.complete_information())information.start(next);else setup();return information.active;
}
void TitleScene::release(){
    renderer.flush();platform.discard_graphics();library.release(20);library.release(22);platform.release_image();information.active=false;
    if(chain)chain->cut(&drawing);chain=nullptr;pending_load=false;menus.state.calcChain=menus.state.drawChain=nullptr;menus.release();
}
void TitleScene::detach(){if(chain)chain->cut(&calculation);}
}
