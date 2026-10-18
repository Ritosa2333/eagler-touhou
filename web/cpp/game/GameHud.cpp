#include "GameHud.hpp"
namespace th15 {
bool GameHud::check(bool ok){if(!ok&&error.empty())error=animations.error.empty()?"HUD animation unavailable":animations.error;return ok;}
u32 GameHud::create(i32 bank,i32 script,u32 ordering){const u32 handle=animations.create(bank,script,-1,ordering);check(handle!=0);return handle;}
u32 GameHud::widget(i32 script){const u32 handle=create(front,script,4);if(auto* vm=animations.registry.find(handle))vm->visual.render_flags&=~0xc000u;return handle;}
bool GameHud::direct_interrupt(u32 handle,i32 label){auto* vm=animations.registry.find(handle);if(!vm)return check(false);vm->pending_interrupt=label;return true;}
bool GameHud::life(i32 lives,i32 pieces){
 if(!life_icons[0])return true;if(lives>8||score.life_piece_tier<0||(progress.difficulty==4&&score.life_piece_tier>5)){error="HUD life values outside original limits";return false;}
 // Native 43a850 starts its icon cursor at zero when stock is negative.
 // Replay playback can continue past game over; keep the simulation stock
 // unchanged and use that original cursor for the HUD only.
 const i32 displayed=lives>0?lives:0;
 for(i32 i=0;i<displayed;i++)if(!direct_interrupt(life_icons[u32(i)],2))return false;if(displayed>7)return true;
 const i32 threshold=progress.difficulty==4?(score.life_piece_tier<5?5:99999999):3;
 if(!direct_interrupt(life_icons[u32(displayed)],i32(i16(wrapping_mul(pieces,5)/threshold))+7))return false;
 for(i32 i=displayed+1;i<8;i++)if(!direct_interrupt(life_icons[u32(i)],3))return false;return true;
}
bool GameHud::bombs(i32 stock,i32 pieces){
 if(!bomb_icons[0])return true;if(stock<0||stock>8){error="HUD bomb stock outside original limits";return false;}
 for(i32 i=0;i<stock;i++)if(!direct_interrupt(bomb_icons[u32(i)],2))return false;if(stock>7)return true;
 if(!direct_interrupt(bomb_icons[u32(stock)],i32(i16(pieces))+7))return false;
 for(i32 i=stock+1;i<8;i++)if(!direct_interrupt(bomb_icons[u32(i)],3))return false;return true;
}
bool GameHud::clear_intro(){if(!check(animations.interrupt(chapter_notice,1))||!check(animations.interrupt(retry_intro,1)))return false;flags&=~0x100u;intro_age.set(0);return true;}
bool GameHud::stage_logo(i32 scene_destination){return scene_destination==8||(player.mode_flags&0x40)||create(logo,0)!=0;}
bool GameHud::name_banner(const std::array<i32,2>& banners){if(animations.registry.find(boss_banner))return true;boss_banner=0;const i32 banner=banners[progress.chapter>=41?0:1];return banner<0||(boss_banner=create(front,wrapping_add(banner,157)))!=0;}
bool GameHud::initialize(const HudEntryContext& context){
 if(!error.empty())return false;if(!root&&(root=widget(0))==0)return false;
 if(!life_icons[0]){
  for(u32 i=0;i<8;i++){life_icons[i]=create(front,i32(i)+30,4);auto* vm=animations.registry.find(life_icons[i]);if(!vm)return false;vm->visual.render_flags&=~0xc000u;}
  for(u32 i=0;i<8;i++){bomb_icons[i]=create(front,i32(i)+38,4);auto* vm=animations.registry.find(bomb_icons[i]);if(!vm)return false;vm->visual.render_flags&=~0xc000u;}
  for(u32 i=0;i<2;i++){boss_icons[i]=create(ascii,i32(i)+2);auto* vm=animations.registry.find(boss_icons[i]);if(!vm)return false;if(!check(animations.pause(boss_icons[i],true)))return false;vm->visual.render_flags&=~0xc0000u;}
 }
 if((player.mode_flags&0x300)&&!mode_notice&&(mode_notice=widget(15))==0)return false;
 if(!life(player.extra_lives,player.life_pieces)||!bombs(player.bombs,player.bomb_pieces))return false;
 if(context.scene_destination!=8&&!(player.mode_flags&0x40)&&(player.mode_flags&0x30)!=0x20&&!create(logo,1))return false;
 if((player.mode_flags&0x40)&&!create(front,112))return false;
 if(!pointdevice&&(pointdevice=create(front,111))==0)return false;
 if(progress.stage==1&&!progress.transition&&!progress.continues){const u32 handle=create(front,68);auto* vm=animations.registry.find(handle);if(!vm)return false;vm->visual.translation={0,progress.character==1?148.f:128.f,0};}
 if(progress.new_run){difficulty_notice=create(front,progress.difficulty+80);if(!difficulty_notice||!check(animations.interrupt(difficulty_notice,3)))return false;}
 difficulty_label=create(front,progress.difficulty+86);if(!difficulty_label)return false;
 if(!check(animations.interrupt(difficulty_notice,3)))return false;
 boss_state=spell_state=tutorial_state=collection_state=0;return true;
}
}
