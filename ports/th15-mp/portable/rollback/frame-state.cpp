#include "multiplayer/FrameState.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace th15;
using th15::multiplayer::FrameState;
namespace {
void require(bool ok,const char* why){if(!ok){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
struct Owner {
    FrameCallback callback;
    std::vector<int>* trace=nullptr;
    int id=0,initialized=0,cleaned=0;
    static i32 tick(void* p){auto& o=*static_cast<Owner*>(p);o.trace->push_back(o.id);return i32(FrameAction::Continue);}
    static i32 init(void* p){++static_cast<Owner*>(p)->initialized;return 0;}
    static i32 cleanup(void* p){++static_cast<Owner*>(p)->cleaned;return 0;}
    Owner(std::vector<int>& t,int value):trace(&t),id(value){callback.owner=this;callback.run=tick;callback.initialize=init;callback.cleanup=cleanup;callback.enabled=true;}
};
}
int main(){
    std::vector<int> trace;Owner a(trace,1),b(trace,2),c(trace,3),d(trace,4),noise(trace,99),replacement(trace,2);
    FrameScheduler scheduler;
    require(scheduler.add(a.callback,FramePass::Update,9)>=0&&scheduler.add(b.callback,FramePass::Update,23)>=0&&scheduler.add(c.callback,FramePass::Update,23)>=0&&scheduler.add(d.callback,FramePass::Draw,72)>=0,"register native callbacks");
    // Equal priorities insert at the head of that priority band.
    scheduler.update();scheduler.draw();require(trace==std::vector<int>({1,3,2,4}),"native traversal baseline");trace.clear();
    FrameState saved;require(saved.capture(scheduler)&&saved.size()==4,"capture callback graph");
    b.callback.enabled=false;scheduler.remove(c.callback);require(scheduler.add(noise.callback,FramePass::Update,1)>=0,"add discarded-future callback");
    require(saved.restore(scheduler),"restore callbacks");scheduler.update();scheduler.draw();
    require(trace==std::vector<int>({1,3,2,4}),"exact priority, order and enabled state");
    require(a.initialized==1&&b.initialized==1&&c.initialized==1&&d.initialized==1&&a.cleaned==0&&b.cleaned==0,"undo does not run init/cleanup hooks");
    require(!scheduler.contains(noise.callback),"discarded-future callback detached");trace.clear();
    FrameState::detach(scheduler);
    require(saved.restore(scheduler,[&](FrameCallback* old){return old==&b.callback?&replacement.callback:old;},[&](void* old){return old==&b?static_cast<void*>(&replacement):old;}),"rebind recreated callback and owner");
    scheduler.update();scheduler.draw();require(trace==std::vector<int>({1,3,2,4})&&scheduler.contains(replacement.callback)&&!scheduler.contains(b.callback),"dynamic owner binding");
    require(replacement.initialized==0&&replacement.cleaned==0,"reconstruction does not initialize twice");
    std::puts("TH15 callback rollback preserves order, bindings and side-effect ownership");
}
