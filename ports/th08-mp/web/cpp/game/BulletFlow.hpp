#pragma once
#include "BulletSystem.hpp"
#include "EffectFlow.hpp"
#include <string>
namespace th08 {
struct BulletLoadContext {bool initial=true,release_resources=true;};
class BulletFlow {
    BulletManagerState& state;BulletSystem& system;ItemSystem& items;EffectPoolState& effects;Rng& random;EffectResources& resources;
    Chain* chain=nullptr;ChainElement calculation,drawing;std::string filename;
public:
    BulletLoadContext context;
    BulletFlow(BulletManagerState& s,BulletSystem& b,ItemSystem& i,EffectPoolState& e,Rng& r,EffectResources& files):state(s),system(b),items(i),effects(e),random(r),resources(files){}
    ~BulletFlow(){detach();}
    bool setup();void release();bool attach(Chain&,const char* requested="etama.anm");void detach();
};
}
