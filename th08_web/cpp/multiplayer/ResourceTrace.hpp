#pragma once
// Observation only. No recording or test input enters a shipped build.
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
#include <cstdint>
namespace th08 {
class BrowserRuntime;
struct PilotResources;
struct PlayerLifeContext;
struct ItemState;
namespace multiplayer::diagnostic {
void Begin(BrowserRuntime&,std::uint32_t frame,std::uint32_t predicted,bool correcting);
void End(BrowserRuntime&);
void Disable(BrowserRuntime&);
void Restored(BrowserRuntime&,std::uint32_t frame);
void Event(const char* name,int seat=-1,int argument=0,const ItemState* item=nullptr,bool after=false);
int Seat(const PilotResources*);
int Seat(const PlayerLifeContext*);
class Scope {
    const char* name;int seat,argument;const ItemState* item;
public:
    Scope(const char* n,int s=-1,int a=0,const ItemState* i=nullptr):name(n),seat(s),argument(a),item(i){Event(n,s,a,i);}
    ~Scope(){Event(name,seat,argument,item,true);}
};
}
}
#endif
