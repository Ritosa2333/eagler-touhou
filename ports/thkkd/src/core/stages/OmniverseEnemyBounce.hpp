#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::enemy142(){
 auto b=object();put(56,m.add(q(56),q(72)));put(64,m.add(q(64),q(80)));if(lt(q(56),m.c(0x6d1898))||gt(q(56),m.c(0x6d20e8)))put(72,m.mul(q(72),m.c(0x6aca58)));if(lt(q(64),m.c(0x6d1898))||gt(q(64),m.c(0x6d20e0)))put(80,m.mul(q(80),m.c(0x6aca58)));
 switch(frame()){case 500:b.word(4,103);break;case 1000:b.word(4,102);break;case 1500:b.word(4,104);break;case 2000:b.word(4,100);break;case 2500:b.word(4,105);break;default:break;}
 std::uint32_t kind,limit,blend=0;std::array<std::uint32_t,3> color{};
 switch(b.word(4)){
 case 103:if(remainder(age(),110)>12||remainder(age(),3)!=0)return;kind=4;limit=120;color={{0,255,255}};break;
 case 102:if(remainder(age(),130)>24||remainder(age(),3)!=0)return;kind=4;limit=120;color={{0,255,0}};break;
 case 104:if(remainder(age(),80)!=0)return;kind=0;limit=180;color={{255,255,0}};break;
 case 100:if(remainder(age(),120)!=0&&remainder(age(),120)!=6)return;kind=1;limit=180;color={{255,0,0}};break;
 case 105:if(remainder(age(),150)!=0)return;kind=2;limit=16;blend=2;color={{255,0,255}};break;
 default:return;
 }
 const auto slot=allocate(kind,0,color,blend,1);if(slot==-1)return;auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(64,b.quad(56));child.quad(72,b.quad(64));child.word(8,202);child.word(120,limit);
}
}
