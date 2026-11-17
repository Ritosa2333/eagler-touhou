#include "game/PerfectFreezeHost.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
using namespace th128::perfect;
Context context(int charge){Context c;c.perfectCharge=charge;c.x=0;c.y=400;c.radius=4;c.keys=2;c.systemPresent=true;c.playerState=1;c.mode=Mode::Normal;c.lasers=LaserContext::Empty;return c;}
int main(int argc,char**argv){assert(argc==2);std::ifstream f(argv[1],std::ios::binary);const std::vector<unsigned char> ice(std::istreambuf_iterator<char>(f),{});assert(!ice.empty());std::string error;unsigned protection=0,freeze=0,shatter=0;
 Services s;s.protect=[&](int ticks,std::string&){assert(ticks==ProtectionTicks||ticks==PhaseFreezeProtectionTicks);++protection;return true;};s.freezeBullets=[&](const BulletRequest&q,BulletResult&r,std::string&){assert(q.overrideImmune);++freeze;r={2,0,1,true};return true;};s.shatterIce=[&](std::string&){++shatter;return true;};
 Host h;assert(h.loadNativeEffect(ice,error)&&h.configure(context(30000),s,error));assert(h.activate(context(30000),error));assert(h.status().charge==20000&&h.status().active&&h.status().phase==0&&protection==1);assert(h.tick(true,error)&&h.status().phase==0);
 for(int i=1;i<=200;++i){assert(h.tick(false,error));assert(freeze==unsigned(i>=NativeOverridePhase));assert(shatter==unsigned(i>=NativeLaserPhase));}assert(!h.status().active&&h.status().charge==20000);
 auto neutral=context(20000);neutral.keys=0;assert(!h.activate(neutral,error)&&!h.status().failed);auto held=context(20000);held.xPressed=false;assert(h.activate(held,error));assert(h.status().charge==10000);for(int i=0;i<200;++i)assert(h.tick(false,error));assert(h.activate(context(10000),error));for(int i=0;i<200;++i)assert(h.tick(false,error));assert(!h.activate(context(0),error)&&h.status().charge==0);assert(h.shutdown(error)&&h.effectSnapshots().empty());
 Host missing;auto incomplete=s;incomplete.shatterIce={};assert(!missing.configure(context(10000),incomplete,error));Host laser;auto populated=context(10000);populated.lasers=LaserContext::Populated;populated.laserCount=2;assert(!laser.configure(populated,s,error));Host window;auto hit=context(10000);hit.mode=Mode::DeathbombWindow;hit.playerState=4;assert(!window.configure(hit,s,error));
 Host busy;auto b=context(10000);b.busy=1;assert(busy.configure(b,s,error)&&!busy.activate(b,error)&&!busy.status().failed);std::cout<<"current actual Perfect native effect/phase120/180/defaultspend/heldgate/unsupportedguards passed\n";
}
