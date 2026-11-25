#pragma once
#include "enemy_slots.hpp"
#include "enemy_math.hpp"
#include "enemy_effects.hpp"
#include <span>
#include <string>
namespace soew {
// Source: th02_main.asm sub_16E3D,16472..17249. No guessed byte widths.
inline unsigned enemy_opcode_width(unsigned op){
    constexpr unsigned low[]={2,4,2,6,3,4,4,5,4,3,2,6};
    if(op<12)return low[op];
    if(op==0x10||op==0x11)return 1;
    switch(op){case 0x20:return 1;case 0x21:case 0x22:return 2;case 0x24:case 0x25:return 3;case 0x26:return 5;case 0x27:return 6;}
    constexpr unsigned high[]={2,3,2,1,1,2,2,3,2,1,2,1,1,1,1,3};
    return op>=0xa0&&op<=0xaf?high[op-0xa0]:0;
}
struct EnemyPathContext {
    unsigned page=0,scroll_delta=0;int player_x=208,player_y=336;
    int scroll_line=0;
    const StageScript::EnemyTemplate *enemy_template=nullptr;
    EnemyEffectSink *effects=nullptr;
};
struct EnemyPathResult {
    enum Status {Timed,Ended,Blocked} status=Blocked;
    unsigned pc=0,opcode=0,instructions=0;
    std::string reason;
    bool repositioned=false;
};
// Fails closed before executing missing consumers (bullets/sound/tile changes).
// No fake terminal/padding beyond path end. Operand-entry jumps remain legal
// when dynamically decodable; low8 opcode fetch follows original u16-PC quirk.
inline EnemyPathResult run_enemy_path(EnemySlots::Slot &s,std::span<const std::uint8_t> path,RandomRing &rng,const EnemyPathContext &context){
    EnemyPathResult result;
    if(context.page>1){result.reason="invalid page";return result;}
    const unsigned xy=context.page*4;
    auto add=[&](unsigned offset,int delta){s.word(offset,static_cast<std::uint16_t>(s.word(offset)+delta));};
    auto move=[&](int dx,int dy){add(xy,dx);add(xy+2,dy);s.raw[0x1c]=static_cast<std::uint8_t>(dx);s.raw[0x1d]=static_cast<std::uint8_t>(dy);};
    for(unsigned budget=0;budget<4096;++budget){
        const auto full_pc=s.word(8);const unsigned pc=full_pc&255;result.pc=full_pc;
        if(pc>=path.size()){result.reason="path byte unavailable (no synthetic padding)";return result;}
        const auto op=path[pc];result.opcode=op;const auto width=enemy_opcode_width(op);
        if(!width){result.reason="unknown opcode";return result;}
        if(width>path.size()-pc){result.reason="truncated instruction";return result;}
        auto byte=[&](unsigned index){return unsigned(path[pc+index]);};
        auto fullbyte=[&](unsigned index){if(unsigned(full_pc)+index>=path.size())throw std::runtime_error("full-PC operand unavailable");return unsigned(path[full_pc+index]);};
        const bool firing=op>=0x20&&op<=0x27&&op!=0x23&&op!=0x25;
        const bool effect=firing||op==0x11||op==0xa8||op==0xaf;
        if(effect){
            bool handled=false;
            if(context.effects){
                if(op==0x11||op==0xa8){
                    if(context.effects->sound)handled=context.effects->sound(op==0x11?3:byte(1));
                }else if(op==0xaf){
                    if(context.effects->tile)handled=context.effects->tile({enemy_s16(s.word(xy)),byte(1),byte(2)});
                }else if(firing&&context.enemy_template&&context.effects->bullet){
                    const auto &raw=context.enemy_template->raw;
                    const auto emit_x=unsigned(raw[4])|(unsigned(raw[5])<<8),emit_y=unsigned(raw[6])|(unsigned(raw[7])<<8);
                    EnemyBulletRequest request;
                    request.left=enemy_s16(s.word(xy)+emit_x);request.top=enemy_s16(s.word(xy+2)+emit_y);
                    request.angle=static_cast<std::uint8_t>(op==0x20?0:op==0x22?s.word(0x16):byte(op==0x26||op==0x27?2:1));
                    request.group=s.raw[0x24];request.speed=s.raw[0x25];
                    if(op==0x24||op==0x27){request.kind=EnemyBulletRequest::Bullet16;request.pattern=static_cast<std::uint8_t>(byte(op==0x24?2:4));}
                    if(op==0x26||op==0x27){request.top=enemy_s16(byte(1)-context.scroll_line);if(request.top<0)request.top+=400;request.group=static_cast<std::uint8_t>(byte(3));request.speed=byte(op==0x26?4:5);}
                    handled=context.effects->bullet(request);
                }
            }
            if(!handled){result.reason="effect consumer not connected";return result;}
            ++result.instructions;
            if(op==0x11){s.word(0xa,0);s.raw[0x10]=1;result.status=EnemyPathResult::Ended;return result;}
            if(op==0x22)add(0x16,int(byte(1)));
            s.word(8,static_cast<std::uint16_t>(full_pc+width));
            continue;
        }
        // Divisor faults are diagnosed BEFORE any instruction side effects.
        if((op==5||op==6)&&byte(2)==0){result.reason="original random-position divide by zero";return result;}
        if((op==3||op==7)&&unsigned(full_pc)+(op==3?5:4)>=path.size()){result.reason="full-PC turn operand unavailable";return result;}
        if((op==3||op==7)&&fullbyte(op==3?4:3)==0){result.reason="original turn divide by zero";return result;}
        if((op==9&&unsigned(full_pc)+2>=path.size())||(op==11&&unsigned(full_pc)+5>=path.size())){result.reason="full-PC vector operand unavailable";return result;}
        ++result.instructions;
        bool timed=false,branch=false;
        if(op>=4&&op<=6)result.repositioned=true;
        switch(op){
        case 0:timed=true;break;
        case 1:move(enemy_s8(byte(2)),enemy_s8(byte(3)));timed=true;break;
        case 2:add(xy+2,int(context.scroll_delta));timed=true;break;
        case 3:case 7:{const unsigned offset=op==3?3:2;const int direction=op==3?(fullbyte(2)==0?-1:1):(s.word(0x18)==1?-1:1);if(enemy_s16(s.word(0xa))%int(fullbyte(offset+1))==0)add(0x16,direction);auto vector=enemy_vector(fullbyte(offset)+(s.word(0x16)&255),fullbyte(offset+2));move(vector[0],vector[1]);timed=true;break;}
        case 4:s.word(xy,static_cast<std::uint16_t>(byte(1)*8));s.word(xy+2,static_cast<std::uint16_t>(byte(2)*8));break;
        case 5:case 6:{const auto draw=rng.next16();const auto position=static_cast<std::uint16_t>(draw%(byte(2)*8)+byte(1)*8);s.word(xy+(op==5?0:2),position);s.word(xy+(op==5?2:0),static_cast<std::uint16_t>(byte(3)*8));if(op==5)s.word(0x18,enemy_s16(position)<208?1:0);break;}
        case 8:move(enemy_s8(static_cast<unsigned>(int(byte(2))*(s.word(0x18)==0?-1:1))),enemy_s8(byte(3)));timed=true;break;
        case 9:{const int dx=enemy_s16(unsigned(context.player_x+24)-s.word(xy)),dy=enemy_s16(unsigned(context.player_y+16)-s.word(xy+2));if(dx==-32768||dy==-32768){result.reason="unsupported original atan extreme";return result;}auto vector=enemy_vector(enemy_atan2(dy,dx),fullbyte(2));move(vector[0],vector[1]);timed=true;break;}
        case 10:add(xy,enemy_s8(s.raw[0x1c]));add(xy+2,enemy_s8(s.raw[0x1d]));timed=true;break;
        case 11:{const auto angle=fullbyte(3)+(s.word(0x16)&255);add(0x16,(fullbyte(2)==0?-1:1)*int(fullbyte(4)));auto vector=enemy_vector(angle,fullbyte(5));move(vector[0],vector[1]);timed=true;break;}
        case 0x10:s.raw[0xe]=2;result.status=EnemyPathResult::Ended;return result;
        case 0x25:s.raw[0x24]=static_cast<std::uint8_t>(byte(1));s.raw[0x25]=static_cast<std::uint8_t>(byte(2));break;
        case 0xa0:s.word(8,static_cast<std::uint16_t>(byte(1)));branch=true;break;
        case 0xa1:case 0xa7:{const unsigned counter=op==0xa1?0x16:0x1a;const int old=enemy_s16(s.word(counter));add(counter,1);if(old<int(byte(2)))s.word(8,static_cast<std::uint16_t>(byte(1)));else{s.word(8,static_cast<std::uint16_t>(full_pc+3));if(op==0xa7)s.word(counter,0);}branch=true;break;}
        case 0xa2:s.word(0x16,static_cast<std::uint16_t>(byte(1)));break;
        case 0xa3:s.word(0x14,0);break;case 0xa4:s.word(0x14,1);break;
        case 0xa5:case 0xa6:s.word(0x12,static_cast<std::uint16_t>(byte(1)));s.word(0x14,op==0xa5?0:2);break;
        case 0xa9:s.raw[0x1e]=1;break;
        case 0xaa:{auto vector=enemy_vector(rng.next8(),byte(1));s.raw[0x1c]=static_cast<std::uint8_t>(vector[0]);s.raw[0x1d]=static_cast<std::uint8_t>(vector[1]);break;}
        case 0xab:case 0xac:s.raw[0x22]=op==0xab;break;
        case 0xad:case 0xae:s.raw[0x23]=op==0xad;break;
        default:result.reason="unimplemented opcode";return result;
        }
        if(timed){add(0xa,1);if(enemy_s16(s.word(0xa))>=int(byte(1))){s.word(8,static_cast<std::uint16_t>(full_pc+width));s.word(0xa,0);}result.status=EnemyPathResult::Timed;return result;}
        if(!branch)s.word(8,static_cast<std::uint16_t>(full_pc+width));
    }
    result.pc=s.word(8);result.reason="instant instruction budget exceeded";return result;
}
}
