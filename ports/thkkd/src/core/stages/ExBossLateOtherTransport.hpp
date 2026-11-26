#pragma once
#include "core/stages/ExBossLateOtherEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_other_detail {
inline void Engine::transport(){
    if(b().word(0x78)==1){b().word(0x30,b().word(0x30)-25u);if(b().word(12)==10){b().word(0x78,2);b().quad(0x40,constant(0x6d1ae0));b().quad(0x48,constant(0x6d1ae0));b().word(0x30,0);}}
    else if(b().word(0x78)==2){const auto parent=b().word(0x7c);if(at(parent).word(0x78)==2&&at(parent).word(12)==b().word(0x80)){b().word(0x78,3);put(0x40,m.add(m.q(at(parent).quad(0x40)),q(0xa0)));put(0x48,m.add(m.q(at(parent).quad(0x48)),q(0xa8)));b().word(12,0);}}
    else if(b().word(0x78)==3){b().word(0x30,b().word(0x30)+25u);if(b().word(12)==10){b().word(0x78,0);b().word(0x3c,0);b().word(0x34,0);b().word(0x2c,0);b().word(0x30,255);}}
}
inline void Engine::normal167(){if(b().word(0x78)!=0){transport();return;}put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));if(compare(m.c(0x6d1a48),q(0x48))<0)b().word(0,0);}
inline void Engine::normal169(){if(b().word(0x78)!=0){transport();return;}put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));
 if(compare(m.i(0),q(0x58))<=0){if(compare(m.c(0x6d1a68),q(0x48))<0)b().word(0,0);}else if(compare(m.c(0x6d1a80),q(0x48))>0)b().word(0,0);
 if(compare(m.i(0),q(0x50))<=0){if(compare(m.c(0x6d1a78),q(0x40))<0)b().word(0,0);}else if(compare(m.c(0x6d1a80),q(0x40))>0)b().word(0,0);
}
inline void Engine::normal168(){
 if(b().word(0x78)==0){if(age()<30)return;const auto radius=m.q(m.store(m.div(m.mul(m.i(60u-b().word(12)),q(0xa0)),m.c(0x6b25f8))));
  for(std::uint32_t i=0;i<3000;++i){auto c=at(i);if(c.word(0)!=1||c.word(4)==29||c.word(0x78)!=0)continue;const auto dx=m.store(m.sub(q(0x40),m.q(c.quad(0x40)))),dy=m.store(m.sub(q(0x48),m.q(c.quad(0x48))));if(compare(m.mul(radius,radius),m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))))<0)continue;
   c.word(0x78,1);c.word(0x7c,index);c.word(0x80,b().word(12));c.quad(0xa0,m.store(m.sub(m.q(c.quad(0x40)),q(0x40))));c.quad(0xa8,m.store(m.sub(m.q(c.quad(0x48)),q(0x48))));c.word(0x3c,1);c.word(0x34,1);c.word(12,0);c.word(0x2c,1);
  }if(b().word(12)==59){b().word(0x78,1);b().word(12,0);}
 }else if(b().word(0x78)==2){if(b().word(12)==59)b().word(0,0);}
 else if(b().word(12)==39){b().word(0x78,2);b().word(12,0);if(b().word(0x7c)==0)b().word(0,0);else if(b().word(0x7c)==1)b().quad(0x40,real(0));else if(b().word(0x7c)==2){b().quad(0x40,real(0));b().quad(0x48,real(1));}}
}
}}
