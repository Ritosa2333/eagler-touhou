#pragma once
#include "SpellSystem.hpp"
#include "SpellDrawing.hpp"
namespace th08 {
struct SpellResources {
    virtual ~SpellResources()=default;
    virtual AnmLoaded* load(i32 slot,const char* path)=0;
    virtual AnmLoaded* get(i32 slot)=0;
    virtual void release(i32 slot)=0;
};
struct SpellLoadContext {bool initial=true,keep_resources=false,release_resources=true;};
class SpellFlow {
    EclGlobals& state;SpellSystem& spells;SpellPresentation& presentation;SpellDrawing& drawing;AnmExecutor& anm;SpellResources& resources;
    Chain* chain=nullptr;ChainElement calculation,display;
    bool start(i32 slot,AnmLoaded*,i32 script);
public:
    SpellLoadContext context;
    SpellFlow(EclGlobals& s,SpellSystem& sys,SpellPresentation& p,SpellDrawing& d,AnmExecutor& a,SpellResources& r):state(s),spells(sys),presentation(p),drawing(d),anm(a),resources(r){}
    ~SpellFlow(){detach();}
    bool setup();void release();bool attach(Chain&);void detach();
};
}
