#include "../th08_web/cpp/game/Archive.hpp"
#include <cstdio>
#include <cstdlib>
using namespace th08;
static u8 input[64*1024*1024];static u32 input_size;
extern "C" __attribute__((export_name("test_input"))) u8* test_input(u32 size){if(size>sizeof(input))return nullptr;input_size=size;return input;}
struct BoundedSource final:ArchiveSource {
    u32 length=input_size;bool fail=false;u32 largest=0;
    u32 size()const override{return length;}
    bool read(u32 offset,u8* out,u32 count)override{largest=std::max(largest,count);if(fail||offset>length||count>length-offset)return false;std::memcpy(out,input+offset,count);return true;}
};
int main(){
    BoundedSource file;Archive memory,stream;if(!memory.open(input,input_size)||!stream.open(file))return 2;
    if(memory.contents().size()!=stream.contents().size())return 3;
    unsigned count=0;unsigned long long compared=0;
    for(const auto& entry:memory.contents())for(bool decrypt:{false,true}){
        std::vector<u8> a,b;if(!memory.read(entry.name.c_str(),a,decrypt)||!stream.read(entry.name.c_str(),b,decrypt)||a!=b)return 4;
        compared+=a.size();++count;
    }
    if(file.largest>=file.size())return 5;
    // Short reads during payload and index loading must fail cleanly. A failed
    // open must leave no entries or dangling pointer to the transient stream.
    file.fail=true;std::vector<u8> output{1,2,3};if(stream.read(memory.contents()[0].name.c_str(),output)||!output.empty())return 6;
    if(stream.open(file)||!stream.contents().empty()||stream.read("missing",output))return 7;
    file.fail=false;file.length=8;if(stream.open(file))return 8;
    std::printf("{\"passed\":true,\"comparisons\":%u,\"bytesCompared\":%llu,\"largestRead\":%u}\n",count,compared,file.largest);return 0;
}
