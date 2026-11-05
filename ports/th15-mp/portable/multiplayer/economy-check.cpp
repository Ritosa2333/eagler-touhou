#include "../../web/cpp/game/MultiplayerEconomy.hpp"
#include "../../web/cpp/game/ItemCollection.hpp"
#include "../../web/cpp/game/SpellPresentationClock.hpp"
#include "../../web/cpp/game/MultiplayerBombProtection.hpp"
#include "../../web/cpp/game/MultiplayerTransferFeedback.hpp"
#include "../../web/cpp/game/MultiplayerVisualPolicy.hpp"
#include <cassert>
#include <cstdio>
using namespace th15;
struct Host:ItemCollectionHost {
 bool options_changed()override{return true;}bool notice(i32)override{return true;}bool sound(i32,bool)override{return true;}
 bool popup(const Vec3&,i32,u32)override{return true;}bool life_hud(i32,i32)override{return true;}bool bomb_hud(i32,i32)override{return true;}
};
int main(){
 for(int power=100;power<200;++power)assert(!multiplayer_can_give_power(power,100));
 assert(multiplayer_can_give_power(200,100)&&multiplayer_can_give_power(400,100));
 assert(!multiplayer_can_give_power(400,0));
 assert(multiplayer_player_opacity(true,false,0)==255);
 assert(multiplayer_player_opacity(false,false,2303)==72);
 assert(multiplayer_player_opacity(false,false,2304)==160);
 assert(multiplayer_player_opacity(true,true,0)==112);
 assert(multiplayer_player_opacity(false,true,9999)==88);
 // A later non-grazing seat must not erase another seat's red glow/jitter.
 for(unsigned count:{2u,3u})for(unsigned grazing=0;grazing<count;++grazing){
  MultiplayerGrazeVisual glow;u32 flags=3,color=0;Vec3 jitter{};
  for(unsigned seat=0;seat<count;++seat){flags=seat==grazing?0x20003u:3u;color=seat==grazing?0xffff0080u:0;jitter=seat==grazing?Vec3{1,2,0}:Vec3{};glow.observe(flags,color,jitter);}
  glow.apply(flags,color,jitter);assert((flags&0x60000)==0x20000&&color==0xffff0080u&&jitter.x==1&&jitter.y==2);
 }
 using Feedback=MultiplayerTransferFeedback;
 assert(multiplayer_transfer_feedback(true,0,false,2).kind==Feedback::none);
 assert(multiplayer_transfer_feedback(true,0,false,3).value==3);
 assert(multiplayer_transfer_feedback(true,0,false,4).kind==Feedback::power);
 assert(multiplayer_transfer_feedback(true,120,true,4).kind==Feedback::life);
 assert(multiplayer_transfer_feedback(true,120,true,4).value==3);
 assert(multiplayer_transfer_feedback(true,152,true,0).value==4);
 assert(multiplayer_transfer_feedback(true,45,false,0).kind==Feedback::none);
 assert(multiplayer_transfer_feedback(false,45,true,4).kind==Feedback::none);
 assert(multiplayer_chapter_threshold(1)==1000000);
 assert(multiplayer_chapter_threshold(2)==1250000);
 assert(multiplayer_chapter_threshold(3)==1500000);
 for(unsigned seats:{2u,3u}){
  const auto threshold=multiplayer_chapter_threshold(seats);
  assert(!(threshold-1>=multiplayer_chapter_threshold(seats)));
  assert(threshold>=multiplayer_chapter_threshold(seats));
 }
 for(i32 character:{0,1,2}){
  i32 timer=multiplayer_bomb_start_protection(character);
  // Protection must survive a bomb longer than the old 180-frame grant.
  for(i32 frame=0;frame<600;++frame){timer=multiplayer_protection_timer(timer-1,multiplayer_bomb_protection(character,true,false));assert(timer>0);}
  assert(multiplayer_bomb_protection(character,false,false)==0);
  assert(multiplayer_protection_timer(280,40)==280);
 }
 assert(multiplayer_bomb_start_protection(3)==30);
 assert(multiplayer_bomb_protection(3,true,false)==0);
 assert(multiplayer_bomb_protection(3,true,true)==30);
 assert(multiplayer_bomb_protection(3,false,false)==0);
 // Confirmed-frame time is independent of host/network waiting. A completed
 // spell must carry a valid native time rather than the 999.99 fallback.
 for(double offset:{0.0,150.0}){
  SpellPresentationClock clock;u32 flags=1;
  assert(clock.sample(flags,1,offset)==SpellPresentationClock::Event::started);
  assert(clock.sample(flags,300,offset+5.0)==SpellPresentationClock::Event::idle);
  flags&=~1u;
  assert(clock.sample(flags,301,offset+5.0)==SpellPresentationClock::Event::completed);
  assert(clock.completed_frames==301&&!(flags&0x40));
  assert(SpellPresentationClock::valid(clock.encoded));
  assert(clock.encoded!=SpellPresentationClock::encode(999,99));
  // Native 0.0167-second quantization rounds five seconds to 4.9933.
  assert(clock.encoded==SpellPresentationClock::encode(4,99));
 }
 // Shared chapter events must dispatch the same native collection rule once
 // per seat, including different cap and Extra-tier histories.
 for(unsigned count:{1u,2u,3u}){
  ItemScoreState score;PlayerMotion motion;Host host;
  std::array<PlayerLifeSession,3> stock{};std::array<i32,3> tiers{};
  for(unsigned seat=0;seat<count;++seat){
   stock[seat].extra_lives=seat?7:8;stock[seat].bombs=2;stock[seat].life_pieces=2;
   ItemCollection collection(stock[seat],score,motion,host,0,&tiers[seat]);ItemState fragment;fragment.kind=4;
   assert(collection.award(fragment));
   assert(stock[seat].extra_lives==8&&stock[seat].life_pieces==0);
   assert(stock[seat].bomb_pieces==(seat?0:1)&&tiers[seat]==(seat?1:0));
   assert(collection.award(fragment)&&stock[seat].bomb_pieces==(seat?1:2));
   stock[seat].extra_lives=7;
   assert(collection.award(fragment)&&stock[seat].life_pieces==1&&stock[seat].extra_lives==7);
  }
 }
 for(int count:{2,3}){
  PlayerLifeSession player;ItemScoreState score;score.max_power=400;PlayerMotion motion;motion.position.y=100;Host host;
  ItemCollection collection(player,score,motion,host,0);collection.small_power_amount=count;
  ItemState item;item.kind=1;player.power=100;assert(collection.award(item)&&player.power==100+count);
  item.kind=3;assert(collection.award(item)&&player.power==200+count);
  player.power=399;item.kind=1;assert(collection.award(item)&&player.power==400);
  // A collector forwards a life-piece event once, rather than locally adding
  // a second fragment after the shared callback updated all seats.
  unsigned calls=0;collection.shared_life_piece=[&](){++calls;return true;};item.kind=4;assert(collection.award(item)&&calls==1);
 }
 std::puts("PASS: native per-seat fragment conversion for 1/2/3 seats, 0.02/0.03 small P, 1.00 large P");
}
