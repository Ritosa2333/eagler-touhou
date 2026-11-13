#include "EnemyCommands.hpp"
namespace th11 {
i32 enemy_th16_emitter_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
#if PORT_TITLE == 16
    const u16 op=c.instruction->opcode;
    if(op!=600&&(op<602||op>612)&&op!=614&&(op<624||op>628)&&op!=641&&op!=700&&op!=701)return -2;
    auto i=[&](u32 n){return c.integer_argument(n,g);};auto f=[&](u32 n){return float(c.float_argument(n,g));};
    i32 index=i(0);
    if(op>=609&&op<=612&&(index<0||index>=18))index=0;
    if(u32(index)>=16)return -2;
    auto& emitter=e.th16_emitters.records[index];auto& offset=e.th16_emitters.bases[index];auto& origin=e.th16_emitters.origins[index];auto& cursor=e.th16_emitters.cursors[index];
    switch(op){
    case 600:
        std::memset(&emitter,0,sizeof emitter);emitter.speed=2;emitter.count=emitter.layers=1;
        emitter.fire_sound=21;emitter.transform_sound=38;emitter.flags=0x23;
        offset.x=offset.y=0;origin={};cursor=0;return 0;
    case 602:emitter.sprite=i(1);emitter.color=i(2);return 0;
    case 603:offset.x=f(1);offset.y=f(2);return 0;
    case 604:emitter.angle=f(1);emitter.spread=f(2);return 0;
    case 605:emitter.speed=f(1);emitter.slow_speed=f(2);return 0;
    case 606:emitter.count=i16(i(1));emitter.layers=i16(i(2));return 0;
    case 607:emitter.aim=i16(i(1));return 0;
    case 608:emitter.fire_sound=i(1);emitter.transform_sound=i(2);return 0;
    case 609:case 610:case 611:case 612:{
        const bool append=op==611||op==612,wide=op==610||op==612;
        const i32 at=append?cursor:i(1);if(u32(at)>=18)return -2;auto& t=emitter.transforms[at];u32 argument=append?1:2;
        t.concurrent=i(argument++);t.opcode=u32(i(argument++));t.p=i(argument++);t.q=i(argument++);
        if(wide){t.r=i(argument++);t.s=i(argument++);}
        t.a=f(argument++);t.b=f(argument++);if(wide){t.c=f(argument++);t.d=f(argument++);}
        cursor=wrapping_add(at,1);return 0;
    }
    case 614:{const u32 from=u32(i(1));if(from>=16)return -2;emitter=e.th16_emitters.records[from];offset=e.th16_emitters.bases[from];origin=e.th16_emitters.origins[from];return 0;}
    case 624:case 625:{
        const i32 difficulty=g.environment.difficulty;const u32 n=difficulty==0?1:difficulty==1?2:difficulty==2?3:4;
        if(op==624){emitter.speed=f(n);emitter.slow_speed=f(n+4);}else{emitter.count=i16(i(n));emitter.layers=i16(i(n+4));}return 0;
    }
    case 627:emitter.radius=f(1);return 0;
    case 626:{const float angle=f(1),radius=f(2);const auto p=polar(angle,radius);offset.x=p.x;offset.y=p.y;return 0;}
    case 628:origin.x=f(1);origin.y=f(2);origin.z=origin.x<-990.f?0.f:1.f;return 0;
    case 641:cursor=wrapping_add(cursor,-1);if(cursor<0)cursor=0;return 0;
    // Own42142b / 42148d write the shared descriptor's laser fields.
    case 700:emitter.position.x=f(1);emitter.position.y=f(2);emitter.position.z=f(3);emitter.values_340[0]=float_bits(f(4));return 0;
    case 701:for(u32 n=0;n<5;++n)emitter.values_340[4+n]=u32(i(n+1));return 0;
    default:return -2;
    }
#else
    (void)e;(void)c;(void)g;return -2;
#endif
}
}
