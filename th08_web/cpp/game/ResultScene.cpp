#include "ResultScene.hpp"
namespace th08 {
bool ResultScene::save(){if(!controls.state.scoreDat)return true;auto bytes=scores.save(platform.now_ms());const bool saved=!bytes.empty()&&platform.write_score(bytes.data(),bytes.size());failed|=!saved;return saved;}
bool ResultScene::attach(Chain& owner,ResultScreenAction action,const ResultContext& context){
    if(loaded||chain)return false;failed=false;animations.invalid=false;controls.state.reset();controls.context=context;
    if(action==RESULT_SCREEN_ACTION_GAME_RESULTS)controls.state.currentState=!(context.flags&1)?RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME:context.flags&0x4000?RESULT_SCREEN_STATE_SPELL_PRACTICE:RESULT_SCREEN_STATE_PRACTICE;
    else if(action==RESULT_SCREEN_ACTION_SAVE_SCORE)controls.state.currentState=RESULT_SCREEN_STATE_SCORE_SAVE;
    if(action!=RESULT_SCREEN_ACTION_SAVE_SCORE){
        if(!platform.load_result_background())return false;resources=true;
        for(const auto& pair:{std::pair<i32,const char*>{21,"result00.anm"},{22,"resulttext.anm"}}){const auto bytes=platform.read_asset(pair.second);if(!library.load(pair.first,bytes.data(),bytes.size())){release();return false;}}
    }
    const auto bytes=platform.read_score();if(!controls.initialize(library,bytes.data(),bytes.size())){release();return false;}
    if(action==RESULT_SCREEN_ACTION_SAVE_SCORE){const bool result=save();release();return result;}
    chain=&owner;loaded=true;
    calculation.set_callback([](void* p){return static_cast<ResultScene*>(p)->controls.update();});calculation.argument=this;calculation.deleted=[](void* p){auto& scene=*static_cast<ResultScene*>(p);scene.save();scene.release();return 0;};
    drawing.set_callback([](void* p){auto& scene=*static_cast<ResultScene*>(p);return scene.view.draw()==1?JobResult::Continue:JobResult::Error;});drawing.argument=this;
    owner.add(&calculation,16);owner.add(&drawing,18,true);return true;
}
void ResultScene::release(){if(loaded)renderer.flush();library.release(21);library.release(22);if(resources)platform.release_result_background();resources=false;if(chain)chain->cut(&drawing);chain=nullptr;loaded=false;controls.state.scoreDat=nullptr;}
void ResultScene::detach(){if(chain)chain->cut(&calculation);}
}
