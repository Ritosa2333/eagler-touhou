#include "../th08_web/cpp/multiplayer/FileEvents.hpp"
#include <cassert>
#include <cstdio>
using namespace th08;using namespace th08::multiplayer;
struct Output:FileEventOutput {
    std::vector<std::string> paths;std::vector<std::vector<u8>> values;bool reject=false;
    bool apply_file_event(const char* path,const u8* data,u32 size)override{
        if(reject)return false;paths.emplace_back(path);values.emplace_back(data,data+size);return true;
    }
};
int main(){
    const u8 old[]{1,2,3},good[]{7,8},bad[]{9};FileEvents e;Output out;
    e.Reset();assert(e.BeginFrame(0)&&e.Write("score.dat",old,3)&&e.EndFrame());
    assert(e.BeginFrame(1)&&e.Write("score.dat",bad,1)&&e.EndFrame());
    assert(e.Pending("score.dat")->at(0)==9&&out.paths.empty());
    assert(e.CommitThrough(0,1,out)&&out.paths.size()==1);
    assert(e.DiscardFrom(1)&&!e.Pending("score.dat"));
    assert(!e.DiscardFrom(0));
    assert(e.BeginFrame(1)&&e.Write("score.dat",good,2)&&e.EndFrame());
    assert(e.CommitThrough(1,1,out)&&out.values.back()==std::vector<u8>(good,good+2));
    assert(e.CommitThrough(1,1,out)&&out.paths.size()==2&&!e.Pending("score.dat"));
    e.Reset();assert(e.BeginFrame(0)&&e.Write("score.dat",old,3)&&e.EndFrame());out.reject=true;
    assert(!e.CommitThrough(0,0,out)&&e.Failed());out.reject=false;
    assert(!e.CommitThrough(0,0,out)&&out.paths.size()==2);
    e.Reset();assert(e.BeginFrame(0)&&!e.Write("../score.dat",old,3)&&e.Failed());
    e.Reset();for(u32 frame=0;frame<16;++frame)assert(e.BeginFrame(frame)&&e.EndFrame());
    assert(!e.BeginFrame(16));
    std::puts("TH08 confirmed files, read-your-writes, rollback replacement and failure fences: PASS");
}
