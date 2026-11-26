// Replay文件扫描和显示后的逐帧缓冲推进；原地址访问通过宿主有界视图。
#pragma once
#include <cstdint>
#include <initializer_list>
#include <vector>
namespace kaikeidou {
enum class ReplayFileOperation:std::uint32_t {create_file=1,seek_file=2,read_file=3,close_file=4};
struct ReplayFileServices {
    virtual ~ReplayFileServices()=default;
    // ReadFile实际写入由服务通过同一内存视图按原顺序实施，返回原EAX位型。
    virtual std::uint32_t replay_file_call(ReplayFileOperation operation,const std::vector<std::uint32_t>& arguments)=0;
};

template<class Memory>
void scan_replay_files(Memory& memory,ReplayFileServices& services){
    std::uint32_t index=0;
    do{
        memory.word(0xb43908,index);
        const auto handle=services.replay_file_call(ReplayFileOperation::create_file,
            {memory.word(0xb4337f+index*4u),0xc0000000u,0,0,3,0x80,0});
        if(handle!=0xffffffffu){
            memory.word(0xb43910,handle);
            auto saved_handle=memory.word(0xb43910);
            services.replay_file_call(ReplayFileOperation::seek_file,{saved_handle,0xffffffdeu,0,2});
            for(std::uint32_t field=0;field<3;++field){
                index=memory.word(0xb43908);
                saved_handle=memory.word(0xb43910);
                services.replay_file_call(ReplayFileOperation::read_file,
                    {saved_handle,memory.word(0xb43501+field*52u+index*4u),field==2?10u:8u,0xb4390c,0});
            }
            services.replay_file_call(ReplayFileOperation::close_file,{saved_handle});
        }
        index=memory.word(0xb43908);
        // 原CreateFile失败分支也走到B42674，照常写valid=1。
        memory.word(0xb43944+index*4u,1);
        ++index;
    }while(index!=14);
}

template<class Memory>
void advance_replay_after_presentation(Memory& memory,std::uint32_t presentation_ecx){
    auto cursor=memory.word(0xb43110);
    if(memory.word(0xb4310c)==0){
        presentation_ecx=(presentation_ecx&0xffffff00u)|memory.byte(0xb43108);
        memory.byte(memory.word(0xb43100)+cursor,static_cast<std::uint8_t>(presentation_ecx));
    }
    memory.word(0xb43110,cursor+1u);
    memory.word(0xb43c04,memory.word(0xb43c04)+presentation_ecx);
}
}
