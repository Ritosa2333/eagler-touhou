// Exact first-write journal, with an explicit read-only capture phase.
// Runs in both WASI and actual Emscripten/JS to exercise the bridge, not mocks.
#include <eagler/netplay/RollbackJournal.hpp>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <vector>
#ifdef __EMSCRIPTEN__
#include <emscripten/heap.h>
#endif
using Netplay::RollbackJournal;
static RollbackJournal make(std::size_t bytes=2*1024*1024,std::size_t blocks=4096){
    RollbackJournal j;assert(j.Reset({4,bytes,blocks,true,true}));return j;
}
int main(){
    for(bool batch:{false,true}){
        auto j=make();std::vector<std::uint32_t> values(300000);
        for(unsigned i=0;i<values.size();++i)values[i]=i*17+23;
        const auto original=values;
        assert(j.BeginFrame(0));
        assert(j.Touch(&values[0],4));values[0]=91; // earlier immediate preimage survives arena growth
        if(batch)assert(j.BeginReadOnlyCapture());
        for(unsigned i=1;i<300;++i){assert(j.Touch(&values[i*1000],4000));assert(j.Touch(&values[i*1000],4000));}
        assert(j.BytesForFrame(0)==4+299*4000&&j.BlocksForFrame(0)==300);
        if(batch){
            assert(!j.BeginReadOnlyCapture());assert(!j.EndFrame());assert(!j.UndoTo(0));
#ifdef __EMSCRIPTEN__
            assert(emscripten_resize_heap(emscripten_get_heap_size()+32*1024*1024));
#endif
            assert(j.EndReadOnlyCapture());assert(!j.EndReadOnlyCapture());
        }
        for(unsigned i=1;i<300;++i)std::fill_n(&values[i*1000],1000,0xdeadbeefu);
        assert(j.EndFrame()&&j.UndoTo(0));assert(values==original);
    }
    {
        auto j=make();std::uint32_t values[]{1,2,3,4};
        assert(j.BeginFrame(10)&&j.Touch(values,4));values[0]=9;assert(j.EndFrame());
        assert(j.BeginFrame(11,true)&&j.BeginReadOnlyCapture());
        assert(j.Touch(values,4)&&j.Touch(values+1,4)&&j.EndReadOnlyCapture());
        values[0]=10;values[1]=11;assert(j.EndFrame());
        assert(j.BytesForFrame(11)==8&&j.BlocksForFrame(11)==2&&j.UndoTo(11));
        assert(values[0]==1&&values[1]==2);
    }
    {
        auto j=make(16,2);std::uint32_t values[]{1,2,3,4};
        assert(j.BeginFrame(0)&&j.BeginReadOnlyCapture());
        assert(j.Touch(values,8));assert(!j.Touch(values+1,8)&&j.Failed());
        assert(!j.EndReadOnlyCapture()&&!j.EndFrame()&&!j.UndoTo(0));
        j.Clear();assert(!j.ReadOnlyCaptureActive());
    }
    {
        auto j=make(8,2);std::uint32_t values[]{1,2,3};
        assert(j.BeginFrame(0)&&j.BeginReadOnlyCapture());
        assert(j.Touch(values,8));assert(!j.Touch(values+2,4)&&j.Failed());
        assert(!j.EndReadOnlyCapture());
    }
    {
        auto j=make();std::uint32_t x=17,y=23;
        for(unsigned frame=0;frame<8;++frame){
            assert(j.BeginFrame(frame)&&j.BeginReadOnlyCapture());
            assert(j.Touch(&x,4)&&j.EndReadOnlyCapture());x+=1;
            assert(j.Touch(&y,4));y+=2;assert(j.EndFrame());
            assert(j.UndoTo(frame)&&x==17&&y==23);
        }
    }
    std::puts("PASS read-only batch: exact preimages, duplicates, arena and Wasm heap growth, extension, overlap/capacity refusal, empty/repeated phases and immediate write path");
}
