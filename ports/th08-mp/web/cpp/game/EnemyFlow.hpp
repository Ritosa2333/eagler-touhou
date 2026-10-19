#pragma once
#include "EnemySystem.hpp"
namespace th08 {
struct EnemyResources {
    virtual ~EnemyResources()=default;
    virtual AnmLoaded* load(i32 slot,const char* name)=0;
    virtual AnmLoaded* get(i32 slot)=0;
    virtual void release(i32 slot)=0;
    virtual std::vector<u8> ecl(const char* name)=0;
};
struct EnemyLoadContext {bool initial=true,keep_resources=false,release_resources=true;};
// Original 0042ebf0 / 0042ee80 scene lifetime and resource selection.
class EnemyFlow {
    EnemySystem& enemies;EclGlobals& globals;EclProgram& program;Rng& random;AsciiManager& ascii;EnemyResources& resources;
    Chain* chain=nullptr;ChainElement calculation,high,low;
    void hide_markers(){for(auto& vm:ascii.state.boss_markers)vm.pos={-999,-999,-999};}
public:
    EnemyLoadContext context;
    EnemyFlow(EnemySystem& e,EclGlobals& g,EclProgram& p,Rng& r,AsciiManager& a,EnemyResources& files):enemies(e),globals(g),program(p),random(r),ascii(a),resources(files){}
    ~EnemyFlow(){detach();}
    bool setup();void release();bool attach(Chain&);void detach();
};
}
