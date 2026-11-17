#include "SpellRecords.hpp"
#include "FrameImage.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <new>
static size_t allocations=0,allocated_bytes=0;
void* operator new(size_t size){
    ++allocations;allocated_bytes+=size;
    if(void* p=std::malloc(size))return p;
    std::abort();
}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete(void* p,size_t)noexcept{std::free(p);}
int main(){
    using namespace th17;
    constexpr unsigned ticks=24000;
    for(unsigned trial=0;trial<6;++trial)for(unsigned depth:{1u,12u})for(bool shared:{false,true}){
        SpellRecords records;
        for(int id=0;id<10;++id)records.begin(id,0,false,"A long spell card title for retained history");
        std::array<multiplayer::FrameImage,12> ring;
        const auto before=allocations,bytes=allocated_bytes;
        const auto start=std::chrono::steady_clock::now();
        for(unsigned frame=0;frame<ticks;++frame){
            auto& image=ring[frame%depth];
            image.clear();
            if(frame%600==0){
                bool all=false;
                if(!records.capture(0,0,false,all))return 1;
            }
            if(shared)records.save_rollback(image);
            else image.save(records.loadouts[0],records.aggregate);
        }
        const auto ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
        std::printf("{\"trial\":%u,\"depth\":%u,\"shared\":%s,\"ticks\":%u,\"ms\":%.3f,\"allocations\":%zu,\"allocatedBytes\":%zu}\n",
            trial,depth,shared?"true":"false",ticks,ms,allocations-before,allocated_bytes-bytes);
    }
}
