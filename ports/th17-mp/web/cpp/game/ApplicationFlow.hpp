#pragma once
#include "GameProgression.hpp"
namespace th17 {
enum class AppScene:i32 {Uninitialized=-2,Startup=0,Loading=1,External=2,Shutdown=3,Title=4,Game=7,Restart=10,ReplayRestart=11,NextStage=12,ReplayStart=13,ExtraGameOver=14,Ending=15,Results=16,Error=17,RetrySelection=19};
struct ApplicationFlowServices {
 virtual ~ApplicationFlowServices()=default;
 virtual bool begin_loading(bool& available)=0;virtual bool shutdown()=0;virtual bool shutdown_delay(i32)=0;
 virtual bool release_game()=0;virtual bool release_title()=0;virtual bool release_ending()=0;
 virtual bool begin_game(bool playback)=0;virtual bool begin_title()=0;virtual bool begin_ending()=0;
};
// Original scene dispatcher, separated from SDL resource owners. A destination
// is consumed at a frame boundary, before retiring any callback's owner.
class ApplicationFlow {
 GameRun& run;ApplicationFlowServices& host;bool check(bool,const char*);bool create_game(bool);
public:
 AppScene current=AppScene::Uninitialized,pending=AppScene::Startup,previous=AppScene::Uninitialized;
 bool fresh=false,retry_selection=false;u32 background=0;std::string error;i32 title_return_reason=0;
 ApplicationFlow(GameRun& r,ApplicationFlowServices& h):run(r),host(h){}
 // 1: continue; 4: normal shutdown; 5: failed shutdown, matching callback actions.
 i32 update();
};
}
