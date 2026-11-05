#include "practice.hpp"
#include "practice_data.hpp"
#include "../pause_system/draw.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>
namespace th20::source::title {
namespace {
const std::uint8_t* card_record(const progress::Profile& profile,int id){
    if(id<0||id>=113)throw std::out_of_range("Spell practice card index");
    return profile.bytes+0xb08+id*0xe0;
}
const int* group(int stage,int boss){
    if(stage<0||stage>=7||boss<0||boss>=13)throw std::out_of_range("Spell practice group index");
    return practice_data::groups[stage][boss];
}
bool attempted(const std::uint8_t* record){return progress::read<std::uint32_t>(record,0xc8)!=0||progress::read<std::uint32_t>(record,0xcc)!=0;}
}
bool practice_group_available(const progress::Profile& fallback,int stage,int boss){
    const auto* ids=group(stage,boss);
    for(int i=0;i<5&&ids[i]>=0;++i)if(progress::read<std::uint32_t>(card_record(fallback,ids[i]),0xc8)!=0)return true;
    return false;
}
int select_practice_card(TitleInf& o,int selected,PracticeEnvironment& environment){
    for(int i=0;i<5;++i){
        if(environment.card_exists(o,i))environment.card_interrupt(o,i,i==selected?2:3);
        const int id=recovered::signed_bits(o.words58d8[3+i]);
        if(id>=0){if(id>=113)throw std::out_of_range("Spell practice card index");environment.selection.signal(o,practice_data::difficulties[id]+127,i==selected?2:3,true);}
    }
    return 0;
}
int refresh_practice_cards(TitleInf& o,text::Renderer& renderer,const progress::Profile& fallback,const progress::Profile& current,int stage,int boss,int selected,PracticeEnvironment& environment){
    const auto* ids=group(stage,boss);
    for(int i=0;i<5;++i)o.words58d8[3+i]=0xfffffffeu;
    if(o.age.current<2){
        for(int i=127;i<134;++i)environment.delete_animation(o,i);
        if(stage==6){environment.selection.main.spawn(o,131);environment.selection.signal(o,131,3,true);}
        else for(int i=127;i<131;++i){environment.selection.main.spawn(o,i);environment.selection.signal(o,i,3,true);}
    }
    sprite::Vec3 position{practice_data::f_00575678,practice_data::f_00571058,0};
    for(int i=0;i<5&&ids[i]>=0;++i){
        const int id=ids[i];const auto* record=card_record(fallback,id);const int difficulty=practice_data::difficulties[id];
        const int slot=stage==6?int(difficulty!=4):std::min(difficulty,4);
        renderer.fields_1a1d4[4]=4;
        if(progress::read<std::uint32_t>(record,0xc8)==0){
            o.words58d8[3+slot]=0xffffffffu;renderer.color=0xff404040;
            renderer.write_text(position,practice_data::s_0057555c,id+1);
        }else{
            o.words58d8[3+slot]=static_cast<std::uint32_t>(id);
            char name[193];strcpy_s(name,sizeof(name),reinterpret_cast<const char*>(record));
            auto length=std::strlen(name);while(length<42)name[length++]=' ';name[length]='\0';
            const bool unavailable=!attempted(card_record(current,id));
            renderer.color=i==selected?(unavailable?0xff808040:0xffffff00):(unavailable?0xff404040:0xff808080);
            renderer.write_text(position,practice_data::s_00575528,id+1,name);
            if(i==selected&&unavailable){
                renderer.color=0xffffffff;renderer.fields_1a1d4[4]=4;
                renderer.write_text_literal({practice_data::f_0057567c,practice_data::f_00571060,0},practice_data::s_00575534);
            }
        }
        position.y=recovered::add32(position.y,practice_data::f_0057565c);
    }
    pause::reset_pause_text(renderer);return 0;
}
}
