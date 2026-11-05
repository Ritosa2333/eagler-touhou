#include "PauseDraw.hpp"
#include <cstdio>
namespace th15 {
namespace {
constexpr char alphabet[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
constexpr const char* characters[]={"Reimu  ","Marisa ","Sanae  ","Reisen "};
constexpr const char* ranks[]={"E ","N ","H ","L ","EX"};
constexpr const char* completed[]={"tst","St1","St2","St3","St4","St5","St6","Ex ","All","ExA"};
constexpr const char* stages[]={"test   ","Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6","Extra  ","Clear  ","ExClear"};
u32 word(const u8* p){u32 value;std::memcpy(&value,p,4);return value;}
std::string name(const char* p){u32 n=0;while(n<8&&p[n])n++;return {p,n};}
}
bool PauseDraw::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool PauseDraw::emit(HudDrawServices& sink){return check(sink.hud_text(text),"Pause ASCII text submission failed");}
bool PauseDraw::name_grid(Vec3 position,HudDrawServices& sink){
 text.position=position;text.text=name(state.name.data());if(!emit(sink))return false;
 text.position.x=float(position.x+float(wrapping_mul(state.name_length,9)));if(state.name_length==8)text.position.x=float(text.position.x-9.f);text.style.color=0xffffff00;text.text="_";if(!emit(sink))return false;
 for(i32 i=0;i<91;i++){text.position={float(112+(i%13)*18),float(320+(i/13)*16),0};text.style.color=state.names.cursor==i?0xffffff00:0xff808080;text.text.assign(1,i<88?alphabet[i]:char(i==88?0x81:i==89?0x7f:0x80));if(!emit(sink))return false;}
 text.style.color=0xffffffff;return true;
}
bool PauseDraw::caption(i32 slot,const u8* metadata,ReplayCalendarServices& dates){
 i64 timestamp;std::memcpy(&timestamp,metadata+0xc,8);ReplayCalendar date;if(!check(dates.calendar(timestamp,date),"Pause replay date unavailable"))return false;
 const u32 character=word(metadata+0x8c)+word(metadata+0x90),rank=word(metadata+0x94),clear=word(metadata+0x98);if(character>=4)return check(false,"Pause replay character outside original range");
 const auto title=name(reinterpret_cast<const char*>(metadata));char buffer[256];
 if(metadata[10]&2)std::snprintf(buffer,sizeof buffer,"No.%.2d %s %.2d/%.2d/%.2d %s Sp %3d",slot+1,title.c_str(),date.year%100,date.month,date.day,characters[character],wrapping_add(signed_bits(word(metadata+0xa0)),1));
 else {if(rank>=5||clear>=10)return check(false,"Pause replay rank/completion outside original range");std::snprintf(buffer,sizeof buffer,"No.%.2d %s %.2d/%.2d/%.2d %s %s %s",slot+1,title.c_str(),date.year%100,date.month,date.day,characters[character],ranks[rank],completed[clear]);}
 text.text=buffer;return true;
}
bool PauseDraw::slots(HudDrawServices& sink,ReplayCalendarServices& dates){
 for(i32 row=0;row<25;row++){text.position={48,float(64+row*15),0};text.style.color=state.menu.cursor==row?0xffffff00:0xff808080;
 if(replays[row]){const auto& bytes=replays[row]->decoded();if(bytes.size()<0xa4||!caption(row,bytes.data(),dates))return check(false,"Pause replay description unavailable");}
 else {char buffer[128];std::snprintf(buffer,sizeof buffer,"No.%.2d -------- --/--/-- ------- -- St-",row+1);text.text=buffer;}
 if(!emit(sink))return false;}text.style.color=0xffffffff;return true;
}
bool PauseDraw::replay_name(std::array<u8,0xa4>& replay,HudDrawServices& sink,ReplayCalendarServices& dates){
 const float origin=float(float(float(state.menu.cursor)*15.f)+64.f);const float y=state.age.current<10?float(float(float(float(224.f-origin)*float(state.age.current))/10.f)+origin):224.f;
 if(!name_grid({102,y,0},sink))return false;std::fill_n(replay.data(),8,u8(' '));replay[8]=0;text.position={48,y,0};return caption(state.menu.cursor,replay.data(),dates)&&emit(sink);
}
bool PauseDraw::ranking(HudDrawServices& sink,ReplayCalendarServices& dates){
 text.position={48,64,0};text.text="            Score Ranking!!";if(!emit(sink))return false;i32 selected=state.menu.cursor;
 if(!state.name_not_required){if(!name_grid({75,float(float(float(selected)*18.f)+96.f),0},sink))return false;}else selected=-1;
 const i32 character=progress.character+progress.subcharacter,rank=progress.difficulty;if(character<0||character>=5||rank<0||rank>=6)return check(false,"Pause score bank outside original range");const auto& rows=records.characters[character].modes[(player.mode_flags&0x300)?0:1].scores[rank];
 for(i32 i=0;i<10;i++){const auto& row=rows[i];text.position={48,float(96+i*18),0};text.style.color=selected==i?0xffffff00:0xff808080;char buffer[256];const auto title=name(row.name.data());
 if(row.details[0]||row.details[1]){const i64 timestamp=i64(u64(u32(row.details[0]))|(u64(u32(row.details[1]))<<32));ReplayCalendar date;if(!check(dates.calendar(timestamp,date),"Pause score date unavailable"))return false;const i32 stage=i8(row.stage);if(stage<0||stage>=10)return check(false,"Pause score stage outside original range");std::snprintf(buffer,sizeof buffer,"%2d %s %9d%d %.2d/%.2d/%.2d %s",i+1,title.c_str(),row.score,i32(row.continues),date.year%100,date.month,date.day,stages[stage]);}
 else std::snprintf(buffer,sizeof buffer,"%2d %s %9d%d --/--/-- Stage -",i+1,title.c_str(),row.score,i32(row.continues));text.text=buffer;if(!emit(sink))return false;}
 text.style.color=0xffffffff;return true;
}
bool PauseDraw::draw(HudDrawServices& sink,ReplayCalendarServices& dates,std::array<u8,0xa4>* replay,AnmVm* captured){
 if(!error.empty())return false;text=HudTextDraw{};text.style.font=0;text.style.shadow=true;
 auto* root=animations.registry.find(state.snapshot_animation);if(!root)state.snapshot_animation=0;else if(captured){if(auto* child=animations.registry.find_child_script(*root,57,0))captured->visual.color=child->visual.color|0xff000000;}
 if(state.screen==PauseScreen::GameOver&&state.phase==15&&!ranking(sink,dates))return false;
 if(state.screen==PauseScreen::Pause||state.screen==PauseScreen::GameOver||state.screen==PauseScreen::Results){const u32 overlay=state.flags&3;
 if(overlay==1)return slots(sink,dates);if(overlay==2)return replay?replay_name(*replay,sink,dates):check(false,"Live replay unavailable for pause name caption");
 if(state.screen==PauseScreen::GameOver&&!(player.mode_flags&0x300)&&state.phase!=14){char buffer[48];std::snprintf(buffer,sizeof buffer,"Credit %d",progress.continue_budget);text.position={184,448,0};text.text=buffer;return emit(sink);}}
 return true;
}
}
