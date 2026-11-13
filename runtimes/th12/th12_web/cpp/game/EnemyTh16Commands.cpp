#include "EnemyManager.hpp"
#include "BulletManager.hpp"
#include "LaserManager.hpp"
#include "ScreenDeformation.hpp"
#include "SpellController.hpp"
namespace th11 {
i32 enemy_th16_spell_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& world){
#if PORT_TITLE == 16
 const u16 op=c.instruction->opcode;
 if(op==568){e.health_flags=(e.health_flags&~1u)|(u32(c.integer_argument(0,g))&1);return 0;}
 if(!world.spells)return -2;
 if(op==542){world.spells->invalidate_capture();return 0;}
 if(op==543){world.spells->hide_circle();return 0;}
 if(op==567){world.spells->title_orientation(c.integer_argument(0,g));return 0;}
 if(op==523){if(!world.spells->end())return -2;e.health_flags&=~1u;return 0;}
 if(op!=522&&op!=537&&op!=538&&op!=539)return -2;
 const auto* instruction=c.instruction;
 if(!world.bosses[0]||instruction->length<32)return -2;
 const i32 size=instruction->argument<i32>(3);
 if(size<1||size>64||u32(size)>u32(instruction->length)-32)return -2;
 char name[64]{};u8 key=0x77,step=7;const auto* source=reinterpret_cast<const u8*>(instruction)+32;
 for(i32 n=0;n<size;++n){name[n]=char(source[n]^key);key=u8(key+step);step=u8(step+0x10);}
 if(!std::memchr(name,0,u32(size)))return -2;
 i32 id=c.integer_argument(0,g);if(op!=522)id=wrapping_add(id,wrapping_sub(g.environment.difficulty,i32(op)-537));
 const i32 variant=c.integer_argument(2,g),duration=c.integer_argument(1,g);
 world.spells->th16_effect_variant=variant;world.spells->run_flags=world.th16_run_flags;
 if(!world.spells->begin(id,duration,name,world.bosses[0]->current.position,world.spells->bomb_active))return -2;
 e.health_flags|=1;e.scaled_health=signed_bits(u32(e.health)*7);e.th16_phase_timer.set(0,&g.environment.rate);return 0;
#else
 (void)e;(void)c;(void)g;(void)world;return -2;
#endif
}
i32 enemy_th16_loose_animation_command(EclContext& c,EnemyGlobals& g,Th16LooseAnimationServices& services){
 if(c.instruction->opcode!=339&&c.instruction->opcode!=308)return -2;
 const i32 file=c.integer_argument(0,g),script=c.integer_argument(1,g);
 if(!services.construct(file,script)||!services.register_effect())return -2;
 if(c.instruction->opcode==308)return 0; // Own41e22b has no lookup or extra tick.
 if(!services.lookup_effect())return -2;
 i32 count=c.integer_argument(2,g);
 for(i32 n=0;n<count;++n){if(n>=4096||!services.tick())return -2;count=c.integer_argument(2,g);}
 return 0;
}
i32 enemy_th16_death_script_command(EnemyState& e,EclContext& c){
#if PORT_TITLE == 16
 if(c.instruction->opcode!=556||c.instruction->length<=20)return -2;
 const char* name=reinterpret_cast<const char*>(c.instruction)+20;
 const auto* end=static_cast<const char*>(std::memchr(name,0,c.instruction->length-20));
 if(!end||u32(end-name)>=sizeof(e.th16_death_script))return -2;
 std::memcpy(e.th16_death_script,name,u32(end-name)+1);return 0;
#else
 (void)e;(void)c;return -2;
#endif
}
bool enemy_th16_damage_drop(EnemyState& e,Rng& rng,Th16EnemyItemServices& services)noexcept{
#if PORT_TITLE == 16
    // Own41c72f..41c7a6. Both draws use the native4a6d88 stream.
    const i32 step=e.th16_damage_drop_step;
    if(step<=0)return true;
    while(e.th16_damage_drop_next<e.th16_damage_total){
        const i32 next=wrapping_add(e.th16_damage_drop_next,step);
        // The original can loop indefinitely after signed overflow.
        if(next<e.th16_damage_drop_next)return false;
        e.th16_damage_drop_next=next;
        const float speed=float(double(rng.unit())+double(1.2000000476837158f));
        const float angle=float(float(rng.next32())/683565248.f)-3.1415927410125732f;
        if(!services.spawn(e.current.position,angle,speed))return false;
    }
    return true;
#else
    (void)e;(void)rng;(void)services;return false;
#endif
}
i32 enemy_th16_deformation_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& world)noexcept{
#if PORT_TITLE == 16
    if(c.instruction->opcode!=629||!world.deformations)return -2;
    // Own4226d0: dispose old object before resolving new arguments.
    if(e.deformation&&!world.deformations->release_deformation(e))return -2;
    e.deformation=nullptr;e.deformation_target=float(c.float_argument(0,g));
    e.deformation_radius=16;e.deformation_color=u32(c.integer_argument(1,g));
    e.deformation_phase_x=e.deformation_phase_y=normalize_angle(0);
    return e.deformation_target>0&&!world.deformations->create_deformation(e)?-2:0;
#else
    (void)e;(void)c;(void)g;(void)world;return -2;
#endif
}
i32 enemy_th16_cancel_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& world)noexcept{
#if PORT_TITLE == 16
    const u16 op=c.instruction->opcode;
    if(op!=613&&op!=615&&op!=616)return -2;
    // 421655 / 42221d: own bullet consumers and then the laser walker.
    if(!world.bullets||!world.lasers)return -2;
    if(op==613)return world.bullets->cancel_all_th16()&&world.lasers->cancel_all(true,false)?0:-2;
    const float radius=float(c.float_argument(0,g));
    const u32 rewards=op==615?1u:0u;
    return world.bullets->cancel_circle_th16(e.current.position,radius,false,rewards)&&world.lasers->cancel_circle(e.current.position,radius,rewards,true)>=0?0:-2;
#else
    (void)e;(void)c;(void)g;(void)world;return -2;
#endif
}
void enemy_th16_bind_spawn(EnemyState& e,const EnemySpawn& p,const float* rate,i32 chapter)noexcept{
#if PORT_TITLE == 16
    // Retail41aa92..41ac45, before the first enemy frame.
    e.absolute.position=p.position;e.score=p.score;e.health=e.max_health=p.health;e.drops.primary=p.drop;e.drops.counts[15]=10;
    e.flags=(e.flags&~0x80000u)|((p.mirrored&1u)<<19);
    e.flags=(e.flags&~0x4000000u)|((p.world_coordinates&1u)<<26);
    if(p.health>=1000)e.flags|=0x40000000;
    std::memcpy(e.integers,p.integers,48);e.parent_instance_id=p.parent_instance_id;
    e.th16_time_drop_timer.set(60,rate);e.th16_time_drop_duration=60;e.th16_time_drop_minimum=1;
    e.damage_immunity.set(2,rate);e.th16_chapter_tag=chapter;
#else
    (void)e;(void)p;(void)rate;(void)chapter;
#endif
}
void enemy_th16_death_metadata(EnemyState& e)noexcept{
#if PORT_TITLE == 16
    // Retail41ac4a..41acf1, after the constructor's first frame.
    e.death_sound=i32(u32(e.instance_id)&1)+3;
    if(e.death_animation)return;
    e.death_animation=44;e.death_animation_file=1;
    if(e.bound_animation_file!=2)return;
    switch(e.animation_script){
    case 5:case 25:case 53:case 79:e.death_animation=40;break;
    case 10:case 56:case 83:e.death_animation=48;break;
    case 15:case 91:e.death_animation=52;break;
    case 30:e.death_animation=58;break;case 35:e.death_animation=57;break;case 40:e.death_animation=56;break;
    }
#else
    (void)e;
#endif
}
bool enemy_th16_clear_drops(EnemyState& e)noexcept{
#if PORT_TITLE == 16
    // Full41d900 preparation before controlled41d520 death consumer.
    if((e.flags&0xc004a0)&&!(e.flags&0x100))return false;
    e.drops.primary=0;std::memset(e.drops.counts,0,17*sizeof(i32));e.drops.spread={32,32};
    e.th16_death_reference={0,192};e.th16_chapter_credit=0;return true;
#else
    (void)e;return false;
#endif
}
i32 enemy_th16_chapter_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
#if PORT_TITLE == 16
    if(c.instruction->opcode==569){
        const i32 old=e.th16_chapter_credit;if(old!=0&&old!=1)return 0;
        const i32 value=c.integer_argument(0,g);e.th16_chapter_credit=value;
        if(old==1)w.th16_chapter_spawned=wrapping_add(wrapping_sub(w.th16_chapter_spawned,1),value);
        else if(value>0)w.th16_chapter_spawned=wrapping_add(w.th16_chapter_spawned,value);
        return 0;
    }
    if(c.instruction->opcode==570){
        if(e.th16_chapter_credit&&e.th16_chapter_tag==w.stage_section){w.th16_chapter_removed=wrapping_add(w.th16_chapter_removed,e.th16_chapter_credit);e.th16_chapter_credit=0;}
        return 0;
    }
    if(c.instruction->opcode!=524)return -2;
    const i32 tag=c.integer_argument(0,g);w.stage_section=w.th16_game_chapter=e.th16_chapter_tag=tag;e.th16_chapter_credit=0;return 0;
#else
    (void)e;(void)c;(void)g;(void)w;return -2;
#endif
}
void enemy_th16_chapter_death(EnemyState& e,EnemyCommandEnvironment& w)noexcept{
#if PORT_TITLE == 16
    // Own41d662..41d68a: consume positive credit only in the current chapter.
    if(e.th16_chapter_credit>0&&e.th16_chapter_tag==w.stage_section){w.th16_chapter_removed=wrapping_add(w.th16_chapter_removed,e.th16_chapter_credit);e.th16_chapter_credit=0;}
#else
    (void)e;(void)w;
#endif
}
i32 enemy_th16_difficulty_command(EclContext& c,EnemyGlobals& g)noexcept{
#if PORT_TITLE == 16
    // Own retail420711/42079e: resolve all four values before writing arg0.
    const u32 difficulty=u32(g.environment.difficulty),index=difficulty<4?difficulty:3;
    if(c.instruction->opcode==535){
        const i32 values[]={c.integer_argument(1,g),c.integer_argument(2,g),c.integer_argument(3,g),c.integer_argument(4,g)};
        auto* destination=c.integer_reference(0,g);if(!destination)return -2;*destination=values[index];return 0;
    }
    if(c.instruction->opcode==536){
        const float values[]={float(c.float_argument(1,g)),float(c.float_argument(2,g)),float(c.float_argument(3,g)),float(c.float_argument(4,g))};
        auto* destination=c.float_reference(0,g);if(!destination)return -2;*destination=values[index];return 0;
    }
#else
    (void)c;(void)g;
#endif
    return -2;
}
i32 enemy_th16_health_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
    if(c.instruction->opcode!=511)return -2;
    // Retail4205ee: boss registration gates the persistent health-bar bit.
    const i32 health=c.integer_argument(0,g);e.health=e.max_health=e.interrupt_health=health;
    if(e.flags&0x800000)e.flags|=0x40000000;
    e.scaled_health=signed_bits(u32(health)*7u);return 0;
}
i32 enemy_th16_drop_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
    switch(c.instruction->opcode){
    case 507:{ // Retail42052e: primary followed by seventeen extra counts.
        const i32 index=c.integer_argument(0,g),value=c.integer_argument(1,g);
        if(index<0||index>17)return -2;
        if(index)e.drops.counts[index-1]=value;else e.drops.primary=value;return 0;
    }
    case 508:e.drops.spread={float(c.float_argument(0,g)),float(c.float_argument(1,g))};return 0;
    case 510:e.drops.primary=c.integer_argument(0,g);return 0;
    default:return -2;
    }
}
i32 enemy_th16_health_marker(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
#if PORT_TITLE == 16
    if(c.instruction->opcode!=527)return -2;
    // Retail420246: max health is captured before resolving any arguments.
    const i32 maximum=e.max_health,type=c.integer_argument(2,g);
    const float fraction=float(float(c.float_argument(1,g))/float(maximum));
    const i32 index=c.integer_argument(0,g);
    if(e.boss_slot<0||e.boss_slot>=4||index<0||index>=4)return -2;
    w.th16_health_segments[e.boss_slot][index]={fraction,type};return 0;
#else
    (void)e;(void)c;(void)g;(void)w;return -2;
#endif
}
i32 enemy_th16_interrupt_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
#if PORT_TITLE == 16
    const u16 op=c.instruction->opcode;if(op!=514&&op!=521)return -2;
    // Retail4214f7 practice path creates different sub names and health. Keep
    // that path explicit until its stage/chapter-specific names are ported.
    if(op==514&&(w.th16_run_flags&0x30)==0x20&&(e.flags&0x800000))return -2;
    const i32 time=op==514?c.integer_argument(2,g):0;
    const i32 health=op==514?c.integer_argument(1,g):0,index=c.integer_argument(0,g);
    if(index<0||index>=8)return -2;
    auto& interrupt=e.interrupts[index];
    if(op==514&&health<0){interrupt.health=health;return 0;}
    const u32 offset=op==514?32:24,length=c.instruction->length;
    if(length<=offset)return -2;
    const char* name=reinterpret_cast<const char*>(c.instruction)+offset;
    const auto* end=static_cast<const char*>(std::memchr(name,0,length-offset));
    if(!end||end-name>=64)return -2;
    if(op==514){
        interrupt.health=health;interrupt.time=time;
        std::memcpy(e.th16_interrupt_names[index][0],name,u32(end-name)+1);
        interrupt.health_script=e.th16_interrupt_names[index][0];
    }
    std::memcpy(e.th16_interrupt_names[index][1],name,u32(end-name)+1);
    interrupt.timeout_script=e.th16_interrupt_names[index][1];return 0;
#else
    (void)e;(void)c;(void)g;(void)w;return -2;
#endif
}
i32 enemy_th16_hitbox_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
#if PORT_TITLE == 16
    const u16 op=c.instruction->opcode;if(op!=500&&op!=501)return -2;
    auto& box=op==500?e.hitbox:e.collision_box;box.x=float(c.float_argument(0,g));box.y=float(c.float_argument(1,g));
    if(op==500&&!e.th16_chapter_credit){e.th16_chapter_credit=1;w.th16_chapter_spawned=wrapping_add(w.th16_chapter_spawned,1);}
    return 0;
#else
    (void)e;(void)c;(void)g;(void)w;return -2;
#endif
}
i32 enemy_th16_time_drop_command(EnemyState& e,EclContext& c,EnemyGlobals& g)noexcept{
#if PORT_TITLE == 16
    if(c.instruction->opcode==1001){
        // Retail422a61..422a76 snapshots cumulative received damage.
        e.th16_damage_drop_step=c.integer_argument(0,g);e.th16_damage_drop_next=e.th16_damage_total;return 0;
    }
    if(c.instruction->opcode!=1000)return -2;
    const i32 duration=c.integer_argument(0,g);e.th16_time_drop_duration=duration;e.th16_time_drop_timer.set(duration,&g.environment.rate);
    e.drops.counts[15]=c.integer_argument(1,g);e.th16_time_drop_minimum=c.integer_argument(2,g);return 0;
#else
    (void)e;(void)c;(void)g;return -2;
#endif
}
void enemy_th16_time_drop_tick(EnemyState& e)noexcept{
#if PORT_TITLE == 16
    // Retail41d4f3 guard and complete40d490, all arithmetic is SSE float32.
    auto& t=e.th16_time_drop_timer;if(t.current<=0)return;
    t.previous=t.current;const float rate=t.rate?*t.rate:1.f;
    const float delta=rate>.99f&&rate<1.01f?1.f:float(rate*1.f);
    t.fractional=float(t.fractional-delta);t.current=truncate_int(t.fractional);
#else
    (void)e;
#endif
}
bool enemy_th16_time_drop_decay(EnemyState& e)noexcept{
#if PORT_TITLE == 16
    const i32 current=e.th16_time_drop_timer.current,minimum=e.th16_time_drop_minimum;
    if(current<=0){e.drops.counts[15]=minimum;return true;}
    const i32 dividend=signed_bits(u32(wrapping_sub(e.drops.counts[15],minimum))*u32(current));
    const i32 divisor=e.th16_time_drop_duration;
    // Retail IDIV faults on these malformed configurations; stop explicitly.
    if(!divisor||(dividend==INT32_MIN&&divisor==-1))return false;
    e.drops.counts[15]=wrapping_add(dividend/divisor,minimum);return true;
#else
    (void)e;return false;
#endif
}
bool enemy_th16_spawn_parameters(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemySpawn& value,const char*& name)noexcept{
#if PORT_TITLE == 16
    // Retail423050, including the native context's resolved instruction.
    const auto* instruction=c.instruction;const u16 op=instruction->opcode;
    if(op!=300&&op!=301&&op!=304&&op!=305&&op!=309&&op!=310&&op!=311&&op!=312&&op!=321)return false;
    if(instruction->length<20)return false;
    const u32 bytes=instruction->argument();
    if(!bytes||(bytes&3)||bytes>u32(instruction->length)-20)return false;
    const u32 start=20+bytes;
    if(u32(instruction->length)-start<20)return false;
    name=reinterpret_cast<const char*>(instruction)+20;
    if(!std::memchr(name,0,bytes))return false;
    auto raw=[&](u32 index){u32 v;std::memcpy(&v,reinterpret_cast<const u8*>(instruction)+start+(index-1)*4,4);return v;};
    auto f=[&](u32 index){const float v=float_bits(raw(index));return instruction->references&(1u<<index)?float(c.resolve_float(v,g)):v;};
    auto i=[&](u32 index){const i32 v=signed_bits(raw(index));return instruction->references&(1u<<index)?c.resolve_integer(v,g):v;};
    value={};value.position.x=f(1);value.position.y=f(2);
    if(op==300||op==309||op==321||op==311||op==304){value.position.x=float(e.current.position.x+value.position.x);value.position.y=float(e.current.position.y+value.position.y);}
    value.mirrored=op==311||op==304||op==312||op==305;
    if(e.flags&0x80000){value.position.x=float(value.position.x*-1.f);value.mirrored^=1;}
    value.health=i(3);value.score=i(4);value.drop=i(5);
    std::memcpy(value.integers,e.integers,48);value.parent_instance_id=u32(e.instance_id);
    return true;
#else
    (void)e;(void)c;(void)g;(void)value;(void)name;return false;
#endif
}
}
