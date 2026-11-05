#include "practice.hpp"
#include "practice_data.hpp"
#include <stdexcept>
namespace th20::source::title {
int draw_practice_scores(TitleInf& o,text::Renderer& renderer,const progress::Profile& fallback,const progress::Profile& current){
    if(o.phase!=1&&o.phase!=2&&(o.phase!=3||o.state==20))return 1;
    sprite::Vec3 position{practice_data::f_00575684,practice_data::f_00570ae0,0};
    renderer.fields_1a1d4[3]=2;renderer.fields_1a1d4[2]=1;
    for(int i=0;i<5;++i){
        const int id=recovered::signed_bits(o.words58d8[3+i]);
        if(id>=0){
            if(id>=113)throw std::out_of_range("Spell practice score index");
            const auto* record=current.bytes+0xb08+id*0xe0;const auto* unlocked=fallback.bytes+0xb08+id*0xe0;
            const bool captured=progress::read<std::uint32_t>(record,0xc4)!=0;
            renderer.color=o.state==20&&o.cursor.current==i?(captured?0xff90d0ff:0xffb0b0b0):(captured?0xff60a0c0:0xff404040);
            if(progress::read<std::uint32_t>(unlocked,0xc8)==0&&progress::read<std::uint32_t>(unlocked,0xcc)==0){renderer.write_ascii_format(position,practice_data::s_005755b0);}
            else{
                renderer.write_ascii_format(position,practice_data::s_0057557c,progress::read<std::uint64_t>(record,0xd8),progress::read<std::int32_t>(record,0xc4),progress::read<std::int32_t>(record,0xcc));
                renderer.color=progress::read<std::uint32_t>(record,0xc0)!=0?0xff206060:0xff404040;
                position.y=recovered::add32(position.y,practice_data::f_0056e734);
                if(practice_data::difficulties[id]<5)renderer.write_ascii_format(position,practice_data::s_00575594,progress::read<std::int32_t>(record,0xc0),progress::read<std::int32_t>(record,0xc8));
                position.y=_mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(position.y),_mm_set_ss(practice_data::f_0056e734)));
            }
        }
        position.y=recovered::add32(position.y,practice_data::f_00570acc);
    }
    renderer.fields_1a1d4[3]=0;renderer.fields_1a1d4[2]=0;renderer.color=0xffffffff;return 1;
}
}
