#include "TitleSpellPractice.hpp"
#include "SpellCatalog.hpp"
#include <cstdio>
namespace th17 {
bool TitleSpellPractice::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool TitleSpellPractice::visual(bool ok){return check(ok,visuals.error.c_str());}
bool TitleSpellPractice::text(i32 row,const std::string& bytes,u32 color){auto* vm=animations.registry.find(state.handles[291+row]);if(!vm){state.handles[291+row]=0;return check(false,"Spell practice text row unavailable");}TextureText request;request.bytes=bytes;request.color=color;return check(host.text(*vm,request),"Spell practice text rendering failed");}
bool TitleSpellPractice::paint(i32 selected_group){
 if(stage<0||stage>=7||selected_group<0||selected_group>=SpellCatalog::groups[stage]||state.spell_character.selected<0||state.spell_character.selected>=9)return check(false,"Spell practice group outside original range");
 spells.fill(-2);for(i32 i=262;i<=268;i++){u32 old=state.handles[i];if(!check(animations.retire(old),"Spell practice label retirement failed"))return false;}
 for(i32 i=0;i<5;i++){if(!visuals.present(291+i)){state.handles[291+i]=animations.create(title_bank,192+i,-1,0);if(!check(state.handles[291+i]!=0,"Spell practice row creation failed"))return false;}if(!visual(visuals.interrupt(291+i,2))||!text(i," Nothing ... ",0x808080))return false;}
 if(stage!=6)for(i32 i=262;i<266;i++)if(!visual(visuals.create(i))||!visual(visuals.interrupt(i,3,true)))return false;
 const auto raw=SpellCatalog::group(stage,selected_group);i32 highest=0;
 for(i32 n=0;n<5&&raw[n]>=0;n++){
  const i32 id=raw[n],difficulty=SpellCatalog::rank(id),slot=stage==6?(difficulty==4?0:1):difficulty;
  if(stage==6&&!visual(visuals.create(difficulty==4?266:268)))return false;
  const auto& all=records.spells.aggregate[id];const auto& personal=records.spells.loadouts[state.spell_character.selected][id];char prefix[32];std::snprintf(prefix,sizeof prefix," No.%3d  ",id+1);
  if(all.attempts[0]){spells[slot]=id;std::string name=all.name;while(name.size()<42)name+=' ';if(!text(slot,std::string(prefix)+name,personal.captures[1]?0xffff80:0xefefef))return false;}
  else {spells[slot]=-1;std::string unknown;for(i32 i=0;i<11;i++){unknown+=char(0x81);unknown+=char(0x48);}if(!text(slot,std::string(prefix)+unknown,0xb0b0b0))return false;}
  if(!visual(visuals.interrupt(291+slot,2)))return false;highest=std::max(highest,slot);
 }
 for(i32 i=0;i<(stage==6?2:5);i++)if(!visual(visuals.interrupt(291+i,3)))return false;
 for(i32 i=highest+1;i<5;i++)if(!check(animations.retire(state.handles[291+i]),"Spell practice unused row retirement failed"))return false;
 return true;
}
bool TitleSpellPractice::paint_selection(){
 for(i32 i=0;i<5;i++){
  const i32 label=i==state.menu.selected?2:3;if(visuals.present(291+i)&&!visual(visuals.interrupt(291+i,label)))return false;
  // Both empty sentinels use the original default rank label. Later empty
  // rows may dim that label again; preserve this ordering on partially seen groups.
  const i32 difficulty=spells[i]<0?0:SpellCatalog::rank(spells[i]);if(difficulty>=0&&!visual(visuals.interrupt(262+difficulty,label,true)))return false;
 }return true;
}
bool TitleSpellPractice::horizontal(u32 input){
 auto& cursor=state.spell_character;cursor.reserved=cursor.selected;
 auto move=[&](i32 direction){if(!check(host.sound(10),"Spell practice loadout sound failed")||!visual(visuals.interrupt(287,cursor.selected+(direction<0?37:27),true)))return false;cursor.step(direction);if(!visual(visuals.interrupt(287,cursor.selected+(direction<0?17:7),true)))return false;selection.character=cursor.selected;settings.last_spell_character=cursor.selected;
  if(state.screen==TitleScreen::SpellGroup)return paint(state.menu.selected);
  if(state.screen==TitleScreen::SpellRank)return paint(group)&&paint_selection();return true;};
 if((input&64)&&!move(-1))return false;if((input&128)&&!move(1))return false;return true;
}
bool TitleSpellPractice::enter_groups(){if(!visual(visuals.retire(102)))return false;state.change_screen(TitleScreen::SpellGroup);stage=state.menu.selected;state.menu.push();state.menu.select(0);return true;}
bool TitleSpellPractice::enter_ranks(){state.change_screen(TitleScreen::SpellRank);group=state.menu.selected;state.menu.push();if(!visual(visuals.retire(96)))return false;state.menu.select(0);return true;}
bool TitleSpellPractice::stage_menu(u32 pressed,u32 repeated){
 switch(state.substate){
 case 0:
  if(!visual(visuals.prompt()))return false;state.menu.count=7;
  if(!visuals.present(287)&&!visual(visuals.create(287)))return false;if(!visuals.present(208)&&!visual(visuals.create(208)))return false;state.change_substate(1);
  if(saved.stage>=0){state.menu.select(saved.stage);saved.stage=-1;returning=true;state.spell_character.count=9;state.spell_character.select(selection.character*3+selection.animal);
   if(!visual(visuals.interrupt(208,3,true))||!visual(visuals.interrupt(208,state.menu.selected+7,true))||!visual(visuals.interrupt(208,6,true))||!visual(visuals.interrupt(287,3,true))||!visual(visuals.interrupt(287,state.spell_character.selected+7)))return false;return enter_groups();}
  if(!visual(visuals.create(102)))return false;[[fallthrough]];
 case 1:if(state.age.current>10){state.change_substate(2);if(!visual(visuals.interrupt(208,3,true))||!visual(visuals.interrupt(208,state.menu.selected+7))||!visual(visuals.interrupt(287,3,true))||!visual(visuals.interrupt(287,state.spell_character.selected+7)))return false;}break;
 case 2:
  state.menu.reserved=state.menu.selected;if((pressed|repeated)&16)state.menu.step(-1);if((pressed|repeated)&32)state.menu.step(1);
  if(state.menu.reserved!=state.menu.selected&&(!check(host.sound(10),"Spell stage navigation sound failed")||!visual(visuals.interrupt(208,3,true))||!visual(visuals.interrupt(208,state.menu.selected+7))))return false;
  if(!horizontal(pressed|repeated))return false;
  if(pressed&0x102){state.change_substate(4);saved.last_stage=state.menu.selected;return check(host.sound(9),"Spell stage cancel sound failed");}
  if(pressed&0x80001){if(!visual(visuals.interrupt(208,6)))return false;state.change_substate(3);saved.last_stage=state.menu.selected;return check(host.sound(7),"Spell stage confirm sound failed");}break;
 case 3:if(state.age.current>19)return enter_groups();break;
 case 4:if(state.age.current>5){for(i32 id:{102,208,287})if(!visual(visuals.retire(id)))return false;state.change_screen(TitleScreen::Main);state.menu.pop();if(!visual(visuals.retire(TitleAnimations::prompt_slot)))return false;}break;
 }return true;
}
bool TitleSpellPractice::group_menu(u32 pressed,u32 repeated){
 if(stage<0||stage>=7)return check(false,"Spell stage selection outside original range");
 switch(state.substate){
 case 0:
  state.menu.count=SpellCatalog::groups[stage];if(!visuals.present(96)&&!visual(visuals.create(96)))return false;if(!visuals.present(209)&&!visual(visuals.create(209)))return false;state.change_substate(1);
  for(i32 i=0;i<13-SpellCatalog::groups[stage];i++)if(!visual(visuals.interrupt(209,39-i,true)))return false;
  if(saved.group>=0){state.menu.select(saved.group);saved.group=-1;if(!paint(state.menu.selected)||!visual(visuals.interrupt(209,3,true))||!visual(visuals.interrupt(209,state.menu.selected+7,true))||!visual(visuals.interrupt(209,6,true)))return false;return enter_ranks();}
  if(!paint(state.menu.selected))return false;[[fallthrough]];
 case 1:if(state.age.current>10){state.change_substate(2);if(!visual(visuals.interrupt(209,3,true))||!visual(visuals.interrupt(209,state.menu.selected+7)))return false;}break;
 case 2:{state.menu.reserved=state.menu.selected;if((pressed|repeated)&16)state.menu.step(-1);if((pressed|repeated)&32)state.menu.step(1);
  if(state.menu.reserved!=state.menu.selected&&(!check(host.sound(10),"Spell group navigation sound failed")||!visual(visuals.interrupt(209,3,true))||!visual(visuals.interrupt(209,state.menu.selected+7))||!paint(state.menu.selected)))return false;
  if(!horizontal(pressed|repeated))return false;
  if(pressed&0x102){state.change_substate(4);return check(host.sound(9),"Spell group cancel sound failed");}
  if(pressed&0x80001){const auto ids=SpellCatalog::group(stage,state.menu.selected);bool unlocked=false;for(i32 id:ids){if(id<0)break;if(records.spells.aggregate[id].attempts[0]){unlocked=true;break;}}
   if(unlocked){if(!visual(visuals.interrupt(209,6)))return false;state.change_substate(3);return check(host.sound(7),"Spell group confirm sound failed");}return check(host.sound(16),"Spell group locked sound failed");}break;}
 case 3:if(state.age.current>=14)return enter_ranks();break;
 case 4:if(state.age.current>5){for(i32 i=0;i<5;i++)if(!visual(visuals.interrupt(291+i,1)))return false;for(i32 i=262;i<=268;i++){u32 handle=state.handles[i];if(!check(animations.retire(handle),"Spell group label retirement failed"))return false;}for(i32 id:{96,209,208})if(!visual(visuals.retire(id)))return false;state.change_screen(TitleScreen::SpellPractice);state.menu.pop();}break;
 }return true;
}
bool TitleSpellPractice::rank_menu(u32 pressed,u32 repeated){
 switch(state.substate){
 case 0:
  state.menu.count=5;for(i32 i=0;i<5;i++)if(spells[i]<0)state.menu.disable(i);state.menu.select(0);if(saved.rank>=0){state.menu.select(saved.rank);saved.rank=-1;}if(!paint_selection())return false;state.change_substate(1);[[fallthrough]];
 case 1:if(state.age.current>10)state.change_substate(2);break;
 case 2:
  state.menu.reserved=state.menu.selected;if((pressed|repeated)&16)state.menu.step(-1);if((pressed|repeated)&32)state.menu.step(1);
  if(state.menu.reserved!=state.menu.selected&&(!check(host.sound(10),"Spell rank navigation sound failed")||!paint_selection()))return false;
  if(!horizontal(pressed|repeated))return false;
  if(pressed&0x102){state.change_substate(4);return check(host.sound(9),"Spell rank cancel sound failed");}
  if(pressed&0x80001){if(!visual(visuals.interrupt(291+state.menu.selected,6)))return false;state.change_substate(3);return check(host.sound(7)&&host.fade_menu_music()&&host.sound(50),"Spell practice launch sound failed");}break;
 case 3:
  if(state.age.current==10&&(!check(host.show_loading_indicator()&&host.begin_transition(transition),"Spell practice transition failed")||!check(animations.interrupt(transition,7),"Spell practice transition animation failed")))return false;
  if(state.age.current>39){const i32 choice=state.menu.selected;if(choice<0||choice>=5||spells[choice]<0)return check(false,"Spell practice launch has no selected card");state.menu.push();state.change_screen(TitleScreen::Quit);selection.stage=stage+1;selection.spell_id=spells[choice];selection.character=state.spell_character.selected/3;selection.animal=state.spell_character.selected%3;selection.difficulty=SpellCatalog::rank(selection.spell_id);state.return_reason=5;saved.stage=stage;saved.group=group;saved.rank=choice;return check(host.start_game(selection.stage),"Spell practice game creation failed");}break;
 case 4:if(state.age.current>5){if(!visual(visuals.retire(209)))return false;state.change_screen(TitleScreen::SpellGroup);state.menu.pop();}break;
 }return true;
}
bool TitleSpellPractice::update(u32 pressed,u32 repeated){if(!error.empty())return false;switch(state.screen){case TitleScreen::SpellPractice:return stage_menu(pressed,repeated);case TitleScreen::SpellGroup:return group_menu(pressed,repeated);case TitleScreen::SpellRank:return rank_menu(pressed,repeated);default:return check(false,"Unknown spell practice menu");}}
bool TitleSpellPractice::draw(HudDrawServices& sink){
 if(!error.empty())return false;const i32 phase=state.substate;if(phase<=0||phase>3||(phase==3&&state.screen==TitleScreen::SpellRank)||state.screen==TitleScreen::SpellPractice)return true;
 if(state.spell_character.selected<0||state.spell_character.selected>=9)return check(false,"Spell practice statistics loadout outside range");HudTextDraw out;out.style.font=2;out.style.shadow=true;char line[128];
 for(i32 row=0;row<5;row++){const i32 id=spells[row];if(id<=-2)continue;const bool selected=state.screen==TitleScreen::SpellRank&&state.menu.selected==row;out.position={330,float(191+44*row),0};out.style.color=selected?0xffb0b0b0:0xff404040;
  if(id<0){out.text="SCORE        00  ----/----";if(!check(sink.hud_text(out),"Locked spell statistics failed"))return false;continue;}
  const auto& personal=records.spells.loadouts[state.spell_character.selected][id];const auto& all=records.spells.aggregate[id];if(personal.captures[1])out.style.color=selected?0xff90d0ff:0xff60a0c0;
  if(!all.attempts[0]&&!all.attempts[1]){out.text="SCORE        00  ----/----";if(!check(sink.hud_text(out),"Spell statistics failed"))return false;continue;}
  std::snprintf(line,sizeof line,"SCORE %8d0  %4d/%4d",records.game.spell_best[state.spell_character.selected][id],personal.captures[1],personal.attempts[1]);out.text=line;if(!check(sink.hud_text(out),"Spell score statistics failed"))return false;
  if(SpellCatalog::rank(id)<5){out.position.y+=10;out.style.color=personal.captures[0]?0xff206060:0xff404040;std::snprintf(line,sizeof line,"GAME MODE        %4d/%4d",personal.captures[0],personal.attempts[0]);out.text=line;if(!check(sink.hud_text(out),"Spell attempt statistics failed"))return false;}
 }return true;
}
}
