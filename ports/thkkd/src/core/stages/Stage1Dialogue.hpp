// 恢复644BB0的完整公共前后段及模式0/1三角色脚本，图形/音频调用保持原边界。
#pragma once
#include "core/shared/DialogueState.hpp"
#include "core/shared/DialogueContext.hpp"
#include "core/shared/DialogueConstants.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/math/BinaryFloat.hpp"
#include <cstring>

namespace kaikeidou {
namespace dialogue_detail {
inline double real(std::uint64_t bits) { double value;std::memcpy(&value,&bits,8);return value; }
inline std::int64_t signed_word(std::uint32_t value) { return value>>31?std::int64_t(value)-0x100000000ll:value; }
struct Lines { std::string_view first,second; };
struct ScriptStep {
    std::uint32_t portrait=0xffffffffu,speaker=0,expression=0,text1=0,text2=0;
    unsigned action=0; // 1分配Boss，2标题，4切换BGM，8结束立绘。
};
inline constexpr std::array<ScriptStep,14> reimu_before{{
    {0,1,2,0x6c8348,0x6c835c,0},{0xffffffffu,0,0,0,0,1},{0,2,0,0x6c837c,0,0},
    {0xffffffffu,2,0,0x6c8388,0,0},{0xffffffffu,1,4,0x6c839c,0x6c83b0,2},{0xffffffffu,2,2,0x6c83cc,0x6c83dc,0},
    {0xffffffffu,1,0,0x6c83ec,0x6c8418,0},{0xffffffffu,1,3,0x6c8438,0,0},{0xffffffffu,2,1,0x6c8464,0x6c8490,0},
    {0xffffffffu,1,7,0x6c84a0,0x6c84b8,0},{0xffffffffu,2,2,0x6c84d4,0x6c84e8,0},
    {0xffffffffu,1,1,0x6c84fc,0,0},{0xffffffffu,2,0,0x6c851c,0,4},{0xffffffffu,0,0,0,0,8}
}};
inline constexpr std::array<ScriptStep,13> marisa_before{{
    {1,1,0,0x6c8540,0x6c8558,0},{0xffffffffu,0,0,0,0,1},{0,2,0,0x6c8570,0,0},
    {0xffffffffu,2,0,0x6c857c,0,0},{0xffffffffu,1,3,0x6c858c,0,2},{0xffffffffu,2,2,0x6c85a8,0x6c85bc,0},
    {0xffffffffu,1,7,0x6c85cc,0,0},{0xffffffffu,2,0,0x6c85ec,0,0},{0xffffffffu,1,1,0x6c8608,0,0},
    {0xffffffffu,2,1,0x6c8634,0x6c864c,0},{0xffffffffu,1,0,0x6c865c,0,0},
    {0xffffffffu,2,2,0x6c8670,0x6c8680,4},{0xffffffffu,0,0,0,0,8}
}};
inline constexpr std::array<ScriptStep,15> third_before{{
    {2,1,0,0x6c86a4,0,0},{0xffffffffu,1,0,0x6c86d0,0,0},{0xffffffffu,0,0,0,0,1},
    {0,2,0,0x6c86f0,0,0},{0xffffffffu,2,0,0x6c86fc,0,0},{0xffffffffu,1,1,0x6c8708,0,2},
    {0xffffffffu,2,1,0x6c873c,0x6c8754,0},{0xffffffffu,1,7,0x6c8778,0x6c8788,0},
    {0xffffffffu,2,2,0x6c87a4,0,0},{0xffffffffu,1,2,0x6c87b8,0,0},{0xffffffffu,2,0,0x6c87d0,0x6c87e8,0},
    {0xffffffffu,1,6,0x6c8800,0x6c8814,0},{0xffffffffu,2,2,0x6c882c,0,4},
    {0xffffffffu,2,0,0x6c8848,0x6c8864,0},{0xffffffffu,0,0,0,0,8}
}};
inline Lines stage1_script(DialogueState& state,DialogueContext& context,EnemyPool& enemies,Enemy& prefix,DialogueServices& services) {
    auto& f=state.fields;Lines text;ScriptStep step;const auto phase=f.word(8);bool selected=false;
    if(f.word(0)==0) {
        const auto choose=[&](const auto& rows){if(phase>=1&&phase<=rows.size()){step=rows[phase-1];selected=true;}};
        if(context.character==0)choose(reimu_before);else if(context.character==1)choose(marisa_before);else if(context.character==2)choose(third_before);
    } else if(f.word(0)==1) {
        if(context.character<3) {
            if(phase==1) {
                constexpr std::array<ScriptStep,3> rows{{{0,1,7,0x6c8880,0x6c8894,0},{1,1,1,0x6c88a0,0,0},{2,1,3,0x6c88c0,0x6c88d4,0}}};
                step=rows[context.character];selected=true;
            } else if(phase==2){step.action=8;selected=true;}
        }
    } else if(f.word(0)<24&&(f.word(0)<16||f.word(0)>=20)) throw std::runtime_error("Dialogue script remains to be recovered");
    if(!selected)return text;
    if(step.action==1) {
        if(context.scene==0) {
            f.word(0x0c,85);const auto index=services.allocate_boss(0);
            auto& enemy=index==-1?prefix:enemies.enemies.at(static_cast<std::size_t>(index));
            enemy.word(8,14);enemy.quad(0x38,constant(0x6d1df0));enemy.quad(0x40,constant(0x6d1848));
            enemy.quad(0xa8,constant(0x6d1a60));enemy.quad(0xb0,constant(0x6d1a90));
        }
        return text;
    }
    if(step.action==8){f.word(0x10,3);return text;}
    if(step.portrait!=0xffffffffu)f.word(step.speaker==1?0x14:0x20,step.portrait);
    f.word(0x10,step.speaker);f.word(step.speaker==1?0x2c:0x38,step.expression);
    text.first=line(step.text1);text.second=line(step.text2);
    if(step.action==2){state.word_a7e7fc=400;state.word_a78e30=0;}
    if(step.action==4&&f.word(4)==0)services.music(2,1);
    return text;
}
}

using DialogueScript = dialogue_detail::Lines(*)(DialogueState&,DialogueContext&,EnemyPool&,Enemy&,DialogueServices&);
template<class Script = DialogueScript>
inline void update_stage1_dialogue(DialogueState& state,DialogueContext& context,EnemyPool& enemies,
                                   Enemy& enemy_prefix,DialogueServices& services,unsigned precision,
                                   Script script=dialogue_detail::stage1_script) {
    using namespace binary_float;using namespace dialogue_detail;
    auto& f=state.fields;if(f.word(8)==0)return;
    const auto c=[](std::uint32_t address){return constant(address);};
    const auto sum=[&](std::uint64_t a,std::uint64_t b){return to_binary64(add(binary64(a),binary64(b),precision));};
    const auto subtract=[&](std::uint64_t a,std::uint64_t b){auto n=binary64(b);n.negative=!n.negative;return to_binary64(add(binary64(a),n,precision));};
    const auto move=[&](std::size_t x,std::size_t y,unsigned speaker,bool right) {
        const auto current=real(f.quad(x));bool change=speaker==3;
        if(speaker==1)change=current<real(c(right?0x6d23e0:0x6d1a90));
        else if(speaker==2)change=current>real(c(right?0x6d2110:0x6d1ae8));
        else if(speaker==4)change=right?current<real(c(0x6d23d0)):current>real(c(0x6d1ef0));
        if(change) {
            const auto dx=c(speaker==3?0x6d1ba8:0x6d1910),dy=c(speaker==3?0x6d1b88:0x6d1c18);
            const bool add_x=right?(speaker!=2):(speaker==1),add_y=right?(speaker!=2):(speaker!=1);
            f.quad(x,add_x?sum(f.quad(x),dx):subtract(f.quad(x),dx));
            f.quad(y,add_y?sum(f.quad(y),dy):subtract(f.quad(y),dy));
        }
    };
    const auto player_scale=[&](std::uint32_t kind){return kind==2?to_binary64(multiply(integer(1),binary64(c(0x6d1890)),precision)):0x3ff0000000000000ull;};
    const auto draw_player=[&](unsigned speaker) {
        if(f.word(0x14)==3) {
            for(const auto which:{2u,1u}) {
                const auto kind_offset=which==2?0x1cu:0x18u,x=which==2?0x68u:0x58u,y=x+8;
                if(f.word(kind_offset)!=0xffffffffu) {
                    const auto handle=context.player_portraits.at(f.word(kind_offset)*8u+f.word(which==2?0x34:0x30));
                    const auto scale=player_scale(f.word(kind_offset));
                    services.sprite(f.quad(x),subtract(f.quad(y),c(which==2?0x6d1ab0:0x6d1a90)),scale,0,handle,1,0);
                    move(x,y,speaker,false);
                }
            }
            const auto handle=context.player_portraits.at(context.character*8u+f.word(0x2c));
            const auto scale=player_scale(context.character);
            services.sprite(f.quad(0x48),sum(f.quad(0x50),f.quad(0x78)),scale,0,handle,1,0);
        } else {
            const auto handle=context.player_portraits.at(f.word(0x14)*8u+f.word(0x2c));
            const auto scale=player_scale(context.character);
            services.sprite(f.quad(0x48),speaker==4?sum(f.quad(0x50),f.quad(0x78)):f.quad(0x50),scale,0,handle,1,0);
        }
    };
    const auto draw_boss=[&](unsigned speaker) {
        const auto kind=f.word(0x20);
        if((speaker==4&&kind!=7)||(speaker!=4&&kind==8)) {
            for(const auto which:{2u,1u}) {
                const auto kind_offset=which==2?0x28u:0x24u,x=which==2?0xa0u:0x90u,y=x+8;
                if(f.word(kind_offset)!=0xffffffffu) {
                    const auto handle=context.boss_portraits.at(35+f.word(which==2?0x40:0x3c));
                    services.sprite(f.quad(x),subtract(f.quad(y),c(which==2?0x6d1f60:0x6d1b38)),f.quad(0xb8),0,handle,1,0);
                    move(x,y,speaker,true);
                }
            }
            const auto handle=context.boss_portraits.at(35+f.word(0x38));
            services.sprite(f.quad(0x80),sum(f.quad(0x88),f.quad(0xb0)),f.quad(0xb8),0,handle,1,0);
        } else {
            const auto handle=context.boss_portraits.at(kind*5u+f.word(0x38));
            auto x=f.quad(0x80),y=f.quad(0x88);
            if(kind==4)x=subtract(x,c(0x6d1d50));else if(kind==5)x=subtract(x,c(0x6b25b8));
            else if(kind==6)y=subtract(y,c(0x6d1a88));else if(kind==7)y=subtract(y,c(0x6b25b8));
            services.sprite(x,y,0x3ff0000000000000ull,0,handle,1,0);
        }
    };
    auto outline=services.color(0,0,0);
    const auto speaker=f.word(0x10);
    if(speaker==1) {
        const auto character=f.word(0x14)==3?f.word(0x44):context.character;
        if(character==0)outline=services.color(170,0,0);else if(character==1)outline=services.color(0,0,170);else if(character==2)outline=services.color(0,170,0);
        if(f.word(0x20)!=0xffffffffu){services.brightness(128,128,128);draw_boss(1);services.brightness(254,254,254);move(0x80,0x88,1,true);}
        if(f.word(0x14)!=0xffffffffu){draw_player(1);move(0x48,0x50,1,false);}
    } else if(speaker==2) {
        outline=services.color(128,128,0);
        if(f.word(0x14)!=0xffffffffu){services.brightness(128,128,128);draw_player(2);services.brightness(254,254,254);move(0x48,0x50,2,false);}
        if(f.word(0x20)!=0xffffffffu){draw_boss(2);move(0x80,0x88,2,true);}
    } else if(speaker==3) {
        if(f.word(0x14)!=0xffffffffu){services.brightness(128,128,128);draw_player(3);services.brightness(254,254,254);move(0x48,0x50,3,false);}
        if(f.word(0x20)!=0xffffffffu){services.brightness(128,128,128);draw_boss(3);services.brightness(254,254,254);move(0x80,0x88,3,true);}
        if((f.word(0x14)!=0xffffffffu&&real(f.quad(0x48))<real(c(0x6d2090)))||
           (f.word(0x20)!=0xffffffffu&&real(f.quad(0x80))>real(c(0x6d23d8)))) {
            f.word(8,0);++context.subphase;context.game_frame=0;
        }
        return;
    } else if(speaker==4) {
        if(f.word(0x14)!=0xffffffffu){services.brightness(128,128,128);draw_player(4);services.brightness(254,254,254);move(0x48,0x50,4,false);}
        if(f.word(0x20)!=0xffffffffu){services.brightness(128,128,128);draw_boss(4);services.brightness(254,254,254);move(0x80,0x88,4,true);}
    }
    if(state.word_a7e7fc!=0) {
        services.blend(1,state.word_a78e30);services.sprite(c(0x6d18d8),c(0x6d22c8),0x3ff0000000000000ull,0,context.titles.at(context.stage),1,0);
        if(signed_word(state.word_a7e7fc)>=240){if(signed_word(state.word_a78e30)<255)state.word_a78e30+=15;}
        else if(signed_word(state.word_a7e7fc)<=60)state.word_a78e30-=5;
        --state.word_a7e7fc;
    }
    services.blend(1,128);services.sprite(c(0x6d1a60),c(0x6d1b80),0x3ff0000000000000ull,0,context.panel,1,0);
    services.blend(2,255);services.sprite(c(0x6d1a60),c(0x6d1b80),0x3ff0000000000000ull,0,context.panel,1,0);
    services.sprite(c(0x6d1a60),c(0x6d1b80),0x3ff0000000000000ull,0,context.panel,1,0);services.blend(0,255);
    if(context.scene==0&&f.word(0x0c)!=0) {
        f.word(0x0c,f.word(0x0c)-1);
        if(f.word(0x0c)==0)f.word(8,f.word(8)+1);
        else {
            if(f.quad(0x78)!=c(0x6d1e28)&&real(f.quad(0x78))<real(c(0x6b25b8)))f.quad(0x78,sum(f.quad(0x78),c(0x6d1910)));
            if(f.quad(0xb0)!=c(0x6d1e28)&&real(f.quad(0xb0))<real(c(0x6b25b8))) {
                f.quad(0xb0,sum(f.quad(0xb0),c(0x6d1910)));f.quad(0xb8,subtract(f.quad(0xb8),c(0x6b2620)));
            }
            return;
        }
    }
    const auto lines=script(state,context,enemies,enemy_prefix,services);
    for(unsigned row=0;row<2;++row) {
        const auto line_text=row?lines.second:lines.first;const auto y=row?0x189u:0x175u;
        services.text(0x38,y,line_text,outline,context.font,0,0);services.text(0x3a,y+2,line_text,outline,context.font,0,0);
        services.text(0x38,y+2,line_text,outline,context.font,0,0);services.text(0x3a,y,line_text,outline,context.font,0,0);
        const auto font=context.font;const auto white=services.color(255,255,255);
        services.text(0x39,y+1,line_text,white,font,0,0);
    }
    if(context.scene==0&&f.word(0x0c)==0) {
        bool advance=false;
        if(context.replay_variant) {
            const auto counter=services.read_counter(context.skip_binding);
            if(context.replay_mode!=0)advance=((context.replay_buttons>>6)&1)!=0;
            else {context.replay_buttons|=counter==1?64u:0u;advance=counter==1;}
        } else advance=services.read_counter(context.confirm_binding)==1||services.read_counter(context.skip_binding)!=0;
        if(advance){f.word(8,f.word(8)+1);f.word(4,0);}else f.word(4,f.word(4)+1);
    }
}
}
