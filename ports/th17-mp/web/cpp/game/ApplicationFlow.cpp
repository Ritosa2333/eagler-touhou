#include "ApplicationFlow.hpp"
namespace th17 {
bool ApplicationFlow::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool ApplicationFlow::create_game(bool replay){return check(host.begin_game(replay),"Application game construction failed");}
i32 ApplicationFlow::update(){
 if(!error.empty())return 5;if(current==pending)return 1;previous=current;background=0xff000000;
 switch(pending){
 case AppScene::Startup:{pending=AppScene::Loading;bool available=false;if(!check(host.begin_loading(available),"Application loading failed"))return 5;if(!available){pending=AppScene::Shutdown;if(!check(host.shutdown()&&host.shutdown_delay(5),"Application shutdown failed"))return 5;return 4;}break;}
 case AppScene::Shutdown:if(!check(host.shutdown()&&host.shutdown_delay(5),"Application shutdown failed"))return 5;return 4;
 case AppScene::Title:
  switch(current){case AppScene::Loading:case AppScene::External:break;
   case AppScene::Game:if(!check(host.release_game(),"Application run release failed"))return 5;break;
   case AppScene::Ending:if(!check(host.release_ending(),"Application ending release failed"))return 5;break;
   default:current=pending;return 1;}
  if(!check(host.begin_title(),"Application title construction failed"))return 5;break;
 case AppScene::Game:
  if(current==AppScene::Title&&!check(host.release_title(),"Application title release failed"))return 5;fresh=true;if(!create_game(false))return 5;break;
 case AppScene::Restart:case AppScene::ReplayRestart:case AppScene::RetrySelection:{
  const bool replay=pending==AppScene::ReplayRestart;const bool selection=pending==AppScene::RetrySelection;
  if(!check(host.release_game(),"Application restart release failed"))return 5;fresh=true;retry_selection=selection;pending=AppScene::Game;run.stage=run.initial_stage;if(!create_game(replay))return 5;break;}
 case AppScene::NextStage:{
  const bool replay=run.practice_record;fresh=false;if(current==AppScene::Game&&!check(host.release_game(),"Application outgoing stage release failed"))return 5;pending=AppScene::Game;if(!create_game(replay))return 5;break;}
 case AppScene::ReplayStart:
  if(current==AppScene::Title&&!check(host.release_title(),"Application replay title release failed"))return 5;pending=AppScene::Game;fresh=true;if(!create_game(true))return 5;break;
 case AppScene::ExtraGameOver:
  if(!check(host.release_game(),"Application Extra game-over release failed"))return 5;fresh=true;pending=AppScene::Game;if(!create_game(false))return 5;break;
 case AppScene::Ending:
  if(current==AppScene::Game&&!check(host.release_game(),"Application ending run release failed"))return 5;if(!check(host.begin_ending(),"Application ending construction failed"))return 5;break;
 case AppScene::Results:
  if(current!=AppScene::External){if(current==AppScene::Game){if(!check(host.release_game(),"Application results run release failed"))return 5;}else if(current==AppScene::Ending){if(!check(host.release_ending(),"Application results ending release failed"))return 5;}else break;}
  pending=AppScene::Title;title_return_reason=3;if(!check(host.begin_title(),"Application results title construction failed"))return 5;break;
 case AppScene::Error:if(!check(host.shutdown()&&host.shutdown_delay(5),"Application error shutdown failed"))return 5;return 5;
 default:break;
 }
 current=pending;return 1;
}
}
