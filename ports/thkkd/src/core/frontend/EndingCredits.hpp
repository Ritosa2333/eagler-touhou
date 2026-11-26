// 原669DD0结局及66C820片尾；页操作来自原跳表，字段在服务返回后按原顺序重读。
#pragma once
#include "core/frontend/EndingCreditsPageData.hpp"
#include "core/player/OptionsMotion.hpp"
#include <string_view>
namespace kaikeidou {
struct EndingCreditsServices {
    virtual ~EndingCreditsServices()=default;
    virtual std::uint32_t input_counter(std::uint32_t binding,bool replay_variant)=0;
    virtual std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
    virtual void blend(std::uint32_t mode,std::uint32_t opacity)=0;
    virtual void rectangle(std::uint32_t left,std::uint32_t top,std::uint32_t right,std::uint32_t bottom,std::uint32_t ink,std::uint32_t fill)=0;
    virtual void sprite_game(std::uint64_t x,std::uint64_t y,std::uint64_t scale,std::uint64_t angle,std::uint32_t graph,std::uint32_t transparent,std::uint32_t reverse)=0;
    virtual void draw_text(std::uint32_t x,std::uint32_t y,std::uint32_t text_address,std::uint32_t ink,std::uint32_t font,std::uint32_t edge,std::uint32_t vertical)=0;
    virtual std::uint32_t load_graph(std::string_view path,std::uint32_t flag)=0;
    virtual void music(std::uint32_t track,std::uint32_t mode)=0;
    virtual void stop_music()=0;
    virtual void return_title(bool replay_variant)=0;
    virtual void initialize_title()=0;
    // 记录原CRT入口；复制本身由下面已恢复的4/2/1字节写入执行。
    virtual void observe_text_copy(std::uint32_t,std::uint32_t){}
};
namespace ending_credits_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?static_cast<std::int64_t>(v)-0x100000000ll:v;}
template<class Memory>void copy_text(Memory& memory,std::uint32_t destination,std::uint32_t source){
    const auto& entry=text(source);const auto* bytes=text_bytes.data()+entry.offset;std::size_t at=0;
    while((source+static_cast<std::uint32_t>(at))&3u){
        const auto value=bytes[at];memory.byte(destination+static_cast<std::uint32_t>(at),value);++at;if(value==0)return;
    }
    while(entry.bytes-at>=4){
        memory.write(destination+static_cast<std::uint32_t>(at),bytes+at,4);at+=4;
        if(bytes[at-1]==0)return;
    }
    const auto remaining=entry.bytes-at;
    if(remaining==3){memory.write(destination+static_cast<std::uint32_t>(at),bytes+at,2);memory.byte(destination+static_cast<std::uint32_t>(at)+2u,0);}
    else if(remaining)memory.write(destination+static_cast<std::uint32_t>(at),bytes+at,remaining);
}
inline constexpr std::array<std::string_view,10> staff_paths{{
    "dat/img/ending/staff00.png","dat/img/ending/staff01.png","dat/img/ending/staff02.png","dat/img/ending/staff03.png",
    "dat/img/ending/staff04.png","dat/img/ending/staff05.png","dat/img/ending/staff06.png","dat/img/ending/staff07.png",
    "dat/img/ending/staff08.png","dat/img/ending/staff09.png"}};
}
template<class Memory>void update_ending(Memory& memory,EndingCreditsServices& api,bool replay_variant){
    using namespace ending_credits_detail;
    if(api.input_counter(memory.word(0xa733ec),replay_variant)==1){memory.word(0xa7e658,0);memory.word(0xa7e65c,memory.word(0xa7e65c)+1u);}
    const auto old_counter=memory.word(0xa7e658);memory.word(0xa7e658,old_counter+1u);
    if(old_counter==0){
        const auto variant=memory.word(0xa7e660);
        if(variant<variants.size()){
            const auto index=memory.word(0xa7e65c);const auto range=variants[variant];
            if(index<range.count){
                const auto page=pages[range.first+index];
                for(std::uint32_t n=0;n<page.count;++n){const auto& e=events[page.first+n];
                    if(e.kind==0){api.observe_text_copy(e.address,e.a);copy_text(memory,e.address,e.a);}
                    else if(e.kind==1){const auto ink=api.color(e.a,e.b,e.c);memory.word(e.address,ink);}
                    else if(e.kind==2)memory.word(e.address,memory.word(e.address)+e.a);
                    else memory.word(e.address,e.a);
                }
            }
        }
    }
    const auto character=memory.word(0xa74c0c);const auto picture=memory.word(0xa7e664);
    const auto graph=memory.word(0xa7e61cu+character*20u+picture*4u);
    api.sprite_game(0x4072000000000000ull,0x406c000000000000ull,0x3ff0000000000000ull,0,graph,1,0);
    for(std::uint32_t line=0;line<4;++line){
        const auto font=memory.word(0xa74bd8);const auto white=api.color(255,255,255);
        api.draw_text(58,375+line*20,0xa7e67cu+line*96u,white,font,0,0);
        const auto current_font=memory.word(0xa74bd8);const auto ink=memory.word(0xa7e668u+line*4u);
        api.draw_text(57,374+line*20,0xa7e67cu+line*96u,ink,current_font,0,0);
    }
    if(signed_word(memory.word(0xa7e678))>=1&&signed_word(memory.word(0xa7e678))<=260){
        api.blend(3,memory.word(0xa7e678));const auto white=api.color(255,255,255);api.rectangle(0,0,640,480,white,1);api.blend(0,255);
        memory.word(0xa7e678,memory.word(0xa7e678)+2u);
        if(signed_word(memory.word(0xa7e678))>=255){
            memory.word(0xa8ce58,102);api.music(16,1);memory.word(0xa7e658,0);
            for(std::uint32_t n=0;n<8;++n){const auto handle=api.load_graph(staff_paths[n],0);memory.word(0xa7e5ccu+n*4u,handle);}
        }
    }
}
template<class Memory>void update_credits(Memory& memory,EndingCreditsServices& api,unsigned precision,bool replay_variant){
    using namespace ending_credits_detail;const options_motion_detail::Arithmetic m{precision};
    const auto background=memory.word(0xa75874);const auto y=m.store(m.add(m.mul(m.i(memory.word(0xa7e658)),m.q(0x3fb999999999999aull)),m.q(0x405f000000000000ull)));
    api.sprite_game(0x4072000000000000ull,y,0x3ff0000000000000ull,0,background,1,0);
    api.blend(3,64);const auto white=api.color(255,255,255);api.rectangle(0,0,640,480,white,1);api.blend(0,255);
    constexpr std::array<std::uint32_t,10> starts{{100,600,1100,1500,2000,2400,2800,3200,3600,4200}};
    for(std::uint32_t n=0;n<starts.size();++n){
        const auto start=starts[n];
        if(signed_word(memory.word(0xa7e658))>=start&&signed_word(memory.word(0xa7e658))<start+672u){
            const auto slot=n<8?n:n-8u;const auto graph=memory.word(0xa7e5ccu+slot*4u);
            const auto row=binary_float::to_binary64(binary_float::integer(576u-memory.word(0xa7e658)+start));
            api.sprite_game(0x4072000000000000ull,row,0x3ff0000000000000ull,0,graph,1,0);
            if(n==4&&signed_word(memory.word(0xa7e658))<2031){
                auto handle=api.load_graph(staff_paths[8],0);memory.word(0xa7e5cc,handle);
                handle=api.load_graph(staff_paths[9],0);memory.word(0xa7e5d0,handle);
            }
        }
    }
    if(signed_word(api.input_counter(memory.word(0xa733fc),replay_variant))>=1)memory.word(0xa7e658,memory.word(0xa7e658)+9u);
    const auto old_counter=memory.word(0xa7e658);memory.word(0xa7e658,old_counter+1u);
    if(signed_word(old_counter)>=5000){api.stop_music();if(!replay_variant)memory.word(0xa8ce58,10);api.return_title(replay_variant);api.initialize_title();}
}
}
