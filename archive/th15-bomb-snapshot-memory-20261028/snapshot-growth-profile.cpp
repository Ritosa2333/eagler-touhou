#include "multiplayer/AnimationState.hpp"
#include "game/AnmVm.hpp"
#include <vector>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <new>
static bool tracking=false;static size_t largest=0;
void* operator new(size_t n){if(tracking)largest=std::max(largest,n);if(auto* p=std::malloc(n?n:1))return p;
#ifdef __EMSCRIPTEN__
std::abort();
#else
throw std::bad_alloc();
#endif
}
void operator delete(void* p)noexcept{std::free(p);}void operator delete(void* p,size_t)noexcept{std::free(p);}
void* operator new[](size_t n){return ::operator new(n);}void operator delete[](void* p)noexcept{std::free(p);}void operator delete[](void* p,size_t)noexcept{std::free(p);}
int main(){using namespace th15;std::vector<AnmVm> roots(8193);std::vector<AnmVm*> refs;AnmRegistry registry;std::array<multiplayer::AnimationState,13> frames;
 for(unsigned n:{2048u,2049u,4097u,8193u}){refs.clear();for(unsigned i=0;i<n;i++){roots[i].visual.translation.x=float(i);refs.push_back(&roots[i]);}largest=0;tracking=true;for(auto& frame:frames)if(!frame.capture(registry,refs))return 1;tracking=false;size_t bytes=0;for(auto& frame:frames)bytes+=frame.retained_bytes();std::printf("{\"roots\":%u,\"snapshots\":13,\"retainedPayloadBytes\":%zu,\"largestAllocation\":%zu}\n",n,bytes,largest);
 for(auto& vm:roots)vm.visual.translation.x=-1;for(auto& frame:frames){if(!frame.restore(registry))return 2;for(unsigned i=0;i<n;i++)if(roots[i].visual.translation.x!=float(i))return 3;}}
}
