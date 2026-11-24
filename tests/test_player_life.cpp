#include "player_life.hpp"
#include <iostream>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Player life contract failed check="+std::to_string(checks));}
void phase(soew::PlayerLife &life,unsigned page){life.begin_frame(page);life.input_allowed();life.player_phase_begin();life.player_phase_end(page,0);}
int main(){try{
 {soew::PlayerLife l;check(l.invincibility==0&&l.current.x==208&&l.current.y==336);l.register_hit();for(unsigned frame=1;frame<=68;++frame){const unsigned page=(frame-1)&1;phase(l,page);if(frame<24){check(l.pattern&&*l.pattern==4+(frame>>2));check(l.lives==2);}if(frame==24)check(!l.pattern&&l.invincibility==220&&l.playperf==-2&&l.lives==2);if(frame==25)check(l.current.y==336&&l.positions[page].y==340);if(frame==42)check(l.positions[page].y==408);if(frame==43)check(l.positions[0].y==366&&l.positions[1].y==366&&l.lives==1&&l.bombs==3&&!l.pattern);if(frame==44)check(l.draw.y==364&&l.current.y==366&&l.pattern==0);if(frame==67)check(l.draw.y==318);if(frame==68)check(!l.hit&&!l.miss_active&&l.miss_frame==0&&!l.pattern&&l.invincibility==176);}check(l.stage_misses==1&&l.total_misses==1);phase(l,0);check(!l.pattern&&l.invincibility==175&&l.lives==1);}
 {soew::PlayerLife l(0,3);l.register_hit();for(unsigned i=0;i<24;++i)phase(l,i&1);check(l.game_over&&l.miss_frame==0&&l.lives==0&&!l.hit&&l.miss_active);check(l.effects.back().kind==soew::PlayerLife::MissItems&&l.effects.back().gameover);}
 {soew::PlayerLife l(1,2);l.register_hit();for(unsigned i=0;i<43;++i)phase(l,i&1);check(l.lives==0&&l.bombs==4&&!l.game_over);}
 for(unsigned timer=1;timer<=220;++timer){soew::PlayerLife l;l.invincibility=static_cast<std::uint8_t>(timer);l.register_hit();check(l.input_allowed()&&!l.hit);l.player_phase_begin();l.player_phase_end(0,0);check(l.invincibility==timer-1);check(bool(l.pattern)==(((timer-1)&3)<3));}
 {soew::PlayerLife l;l.invincibility=220;l.miss_active=true;l.hit=true;l.miss_frame=25;check(!l.input_allowed()&&l.hit);phase(l,0);check(l.miss_frame==26&&l.hit);}
 for(int perf=-6;perf<=16;++perf){soew::PlayerLife l;l.playperf=perf;l.register_hit();for(unsigned i=0;i<24;++i)phase(l,i&1);const int expected=perf>2?0:perf-2<-6?-6:perf-2;check(l.playperf==expected);}
 {soew::PlayerLife l;l.bomb_invincible=true;l.register_hit();check(l.input_allowed()&&!l.hit);l.player_phase_begin();l.player_phase_end(0,3);check(l.pattern==3);}
 for(unsigned difficulty=0;difficulty<4;++difficulty){soew::PlayerLife l;l.playperf=-6;l.advance_playperf(1499,difficulty);check(l.playperf==-6);l.advance_playperf(1500,difficulty);check(l.playperf==-5);for(unsigned cycle=1;cycle<60;++cycle)l.advance_playperf(cycle*1800+1500,difficulty);check(l.playperf==(difficulty==0?4:16));l.advance_playperf(0,difficulty);check(l.playperf==(difficulty==0?4:16));}
 std::cout<<"player_life_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
