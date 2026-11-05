#pragma once
// Diagnostic wall time only, excluded from gameplay/checkpoints. Component
// and ordinary builds compile the timer away; Web MP defaults to disabled.
#if defined(__EMSCRIPTEN__) && defined(TH11_MULTIPLAYER)
#include <emscripten.h>
#include <array>
namespace th11::runtime_cost {
inline bool enabled=false;
inline std::array<double,64> bins{{1,64}};
inline void begin(){if(enabled){bins.fill(0);bins[0]=1;bins[1]=64;}}
struct Scope {
 unsigned bin;double started;
 explicit Scope(unsigned i):bin(i),started(enabled?emscripten_get_now():-1){}
 ~Scope(){if(started>=0)bins[bin]+=emscripten_get_now()-started;}
};
template<class F>auto run(unsigned bin,F&& f)->decltype(f()){Scope scope(bin);return f();}
}
#else
namespace th11::runtime_cost {
struct Scope{explicit Scope(unsigned){}};
template<class F>auto run(unsigned,F&& f)->decltype(f()){return f();}
}
#endif
