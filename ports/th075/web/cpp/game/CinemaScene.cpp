#include "CinemaScene.hpp"
namespace th075 {
u16 CinemaScene::advance(const std::array<i32,2>& confirm,u32 now,const FighterActor* player,u32 scoreToBeat,u8 route,i8& replayDisposition,CinemaSignals& signals){
 signals.render_cinema();signals.present_cinema();if(!credits){if(confirm[0]==1||confirm[1]==1)return 0x2202;if(elapsed>=0xc15d)return 0xff02;elapsed=now-startedAt;return 0xd;}
 if(confirm[0]==1&&(elapsed>85000||elapsed<80000)){signals.select_music(40);signals.fade_music(-10000,-160,1,1);if(!player)return 0x1102;replayDisposition=route<3?0:-1;return scoreToBeat<u32(player->score)?0x110a:0x1107;}elapsed=now-startedAt;return 0xb;
}
void CinemaScene::render(ImageRenderer& images)const{if(credits)render_credits(images);else render_opening(images);}
}
