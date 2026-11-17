#include "multiplayer/AnimationState.hpp"
#include <emscripten/heap.h>
#include <malloc.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>
int main(){
    using namespace th15;
    AnmRegistry registry;
    std::vector<AnmVm> roots(4096);
    std::vector<AnmVm*> input;
    for(unsigned i=0;i<roots.size();++i){roots[i].visual.translation.x=float(i);input.push_back(&roots[i]);}
    const auto start=mallinfo().uordblks;
    std::array<multiplayer::AnimationState,13> frames;
    for(auto& frame:frames)if(!frame.capture(registry,input))return 1;
    const auto retained=mallinfo().uordblks-start;
    for(auto& root:roots)root.visual.translation.x=-1;
    for(auto& frame:frames){
        if(!frame.restore(registry))return 2;
        for(unsigned i=0;i<roots.size();++i)if(roots[i].visual.translation.x!=float(i))return 3;
    }
    std::printf("{\"roots\":4096,\"snapshots\":13,\"vmBytes\":%zu,\"snapshotAllocatedBytes\":%d,\"heapBytes\":%zu,\"restored\":true}\n",sizeof(AnmVm),retained,emscripten_get_heap_size());
}
