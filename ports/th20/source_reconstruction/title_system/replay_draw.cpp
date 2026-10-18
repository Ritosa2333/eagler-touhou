#include "replay_menu.hpp"
#include "replay_draw_data.hpp"
#include "../progress_state/records.hpp"
#include <algorithm>
#include <ctime>
#include <cstring>
#include <stdexcept>
namespace th20::source::title {
namespace d=replay_draw_data;namespace n=recovered;
char replay_filename_fragment[5]{}; // original5c6100, shared scratch storage
const char* replay_filename_suffix(const replay::ReplayInf& replay){
    std::memcpy(replay_filename_fragment,replay.filename+7,4);replay_filename_fragment[4]=0;return replay_filename_fragment; //50a220
}
namespace {
template<std::size_t N>const char* label(const char*const(&table)[N],int index){if(index<0||static_cast<std::size_t>(index)>=N)throw std::out_of_range("Replay display label index");return table[index];}
replay::ReplayInf* entry(TitleInf& owner,int index){if(index<0||index>=100)throw std::out_of_range("Replay display slot");return static_cast<replay::ReplayInf*>(owner.metadata[index]);}
void header(text::Renderer& renderer,replay::ReplayInf& replay,int page,int index,const sprite::Vec3& position){
    auto& user=*replay.user;std::tm date{};_localtime64_s(&date,&user.timestamp);
    const auto* character=label(d::characters,static_cast<int>(user.fields_d0[2]));const auto* stone=label(d::stones,static_cast<int>(user.stones[0]));
    const auto* name=reinterpret_cast<const char*>(&user);const double slowdown=static_cast<double>(progress::read<float>(&user,0xd0));
    if(page==0){
        if(!(user.flags&2))renderer.write_ascii_format(position,d::formats[0],n::signed_bits(static_cast<unsigned>(index)+1u),name,date.tm_year%100,date.tm_mon+1,date.tm_mday,date.tm_hour,date.tm_min,character,stone,label(d::difficulties,user.difficulty),label(d::finished_stages,user.finished_stage),slowdown);
        else renderer.write_ascii_format(position,d::formats[4],n::signed_bits(static_cast<unsigned>(index)+1u),name,date.tm_year%100,date.tm_mon+1,date.tm_mday,date.tm_hour,date.tm_min,character,stone,n::signed_bits(static_cast<unsigned>(user.spell)+1u),slowdown);
    }else{
        const auto* difficulty=label(d::difficulties,user.difficulty);const auto* stage=label(d::finished_stages,user.finished_stage);const auto* suffix=replay_filename_suffix(replay);
        //5751c4 has one excess %s: original5244d7/52493a consume the low
        //double word as a pointer and then read past their argument list.
        //The original demos (and 1.0/12.5 probes) produce "(null) 0.0%".
        //Actual 0.1/0.001 original calls fault at5482f0. Preserve the observed
        //null-pointer display; reject arbitrary pointer dereferences explicitly.
        std::uint64_t bits;std::memcpy(&bits,&slowdown,8);
        if(static_cast<std::uint32_t>(bits)!=0)throw std::domain_error("Original custom replay format dereferences the slowdown value as a string pointer (5751c4)");
        renderer.write_ascii_format(position,d::formats[2],suffix,name,date.tm_year%100,date.tm_mon+1,date.tm_mday,date.tm_hour,date.tm_min,character,stone,difficulty,stage,static_cast<const char*>(nullptr),0.0);
    }
}
}
int draw_replay_menu(TitleInf& owner){
    auto& renderer=*text::renderer;
    if(owner.phase==2){
        sprite::Vec3 position{d::f_0056cd94,d::f_0056fa30,0};renderer.fields_1a1d4[2]=1;const int page=owner.cursor108.current;
        if(page<0||page>=4)throw std::out_of_range("Replay display page");
        for(int index=page*25;index<page*25+25;++index){
            renderer.color=owner.cursor.selected(index%25)?0xffffff00:0xff808080;
            if(auto* replay=entry(owner,index))header(renderer,*replay,page,index,position);
            else renderer.write_ascii_format(position,d::formats[page==0?1:3],index+1);
            position.y=n::add32(position.y,d::f_0056f2fc);
        }
        renderer.color=0xffffffff;renderer.fields_1a1d4[2]=0;
    }else if(owner.phase==4){
        sprite::Vec3 position{d::f_0056cd94,d::f_0056fa30,0};const int index=n::signed_bits(owner.words5734[1]);auto* replay=entry(owner,index);if(!replay||!replay->user)throw std::logic_error("Selected replay has no metadata");
        if(owner.age.current<10){
            const auto row=n::int_float((index%25)*15);const auto remaining=_mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(d::f_0056e734),_mm_set_ss(owner.age.current_f)));
            position.y=n::add32(_mm_cvtss_f32(_mm_div_ss(_mm_set_ss(n::mul32(row,remaining)),_mm_set_ss(d::f_0056e734))),d::f_0056fa30);
        }
        renderer.fields_1a1d4[2]=1;header(renderer,*replay,owner.cursor108.current,index,position);
        if(owner.age.current>=10){
            position.x=d::f_00575678;position.y=d::f_0056cd98;
            for(int stage=1;stage<8;++stage){
                renderer.color=owner.cursor.selected(stage-1)?0xffffff00:0xff808080;
                if(!replay->playback[stage].stage)renderer.write_ascii_format(position,d::s_0057527c,label(d::stages,stage));
                else if(stage<7&&replay->playback[stage+1].stage){
                    auto& table=replay->playback[stage+1].stage->player_table;table.continue_count=std::clamp(table.continue_count,0,9);
                    renderer.write_ascii_format(position,d::s_0057528c,label(d::stages,stage),progress::read<std::uint64_t>(&table,0),table.continue_count);
                }else renderer.write_ascii_format(position,d::s_0057528c,label(d::stages,stage),progress::read<std::uint64_t>(replay->user,0x18),replay->user->field_f8);
                position.y=n::add32(position.y,d::f_00572644);
            }
        }
        renderer.color=0xffffffff;renderer.fields_1a1d4[2]=0;
    }
    return 1;
}
}
