#include "EnemyCommands.hpp"
#include "SpellController.hpp"
#include "ScreenShake.hpp"
#include "Dialogue.hpp"
#include "ScreenDeformation.hpp"
#include "EnemyCallbacks.hpp"
#include "Stage.hpp"
namespace th11 {
namespace {
template<class T>T& destination(T* p){if(!p)__builtin_trap();return *p;}
i32 drop_type(i32 type,u32 flags){if(flags&0x80000){if(type==1)return 10;if(type==4)return 11;}return type;}
u32 rank_index(i32 rank){return rank>=600?4:rank>=200?3:rank>=-200?2:rank>=-400?1:0;}
}
i32 enemy_state_command(EnemyState& e,EclContext& c,EnemyGlobals& g,EnemyCommandEnvironment& w)noexcept{
    const u16 op=c.instruction->opcode;auto& world=g.environment;const float* rate=&world.rate;
    auto f=[&](u32 n){return float(c.float_argument(n,g));};auto i=[&](u32 n){return c.integer_argument(n,g);};
    switch(op){
    // Shipped scripts contain these labels; the original dispatcher explicitly
    // falls through to success without evaluating arguments.
    case 0x116:case 0x1f4:break;
    case 0x140:e.hitbox.x=f(0);e.hitbox.y=f(1);break;
    case 0x141:e.collision_box.x=f(0);e.collision_box.y=f(1);break;
    case 0x142:case 0x143:{
        const u32 bits=u32(i(0)),flags=op==0x142?e.flags|bits:e.flags&~bits;const bool visible=!(flags&0x20);
        const bool update=op==0x142?!visible:visible;
        if(update&&!w.animation_visibility)return -2;e.flags=flags;
        if(update)for(u32 id:e.animations)w.animation_visibility(id,visible,w.animation_user);break;
    }
    case 0x144:e.flags|=0x2000;e.clamp_center.x=f(0);e.clamp_center.y=f(1);e.clamp_size.x=f(2);e.clamp_size.y=f(3);break;
    case 0x145:e.flags&=~0x2000u;break;
    case 0x146:for(auto& count:e.drops.counts)count=0;break;
    case 0x147:{const i32 type=drop_type(i(0),e.flags),count=i(1);if(type<0||type>12)return -2;if(type==0)e.drops.primary=count;else e.drops.counts[type-1]=count;break;}
    case 0x148:{const float y=f(1);e.drops.spread.x=f(0);e.drops.spread.y=y;break;}
    case 0x14a:e.drops.primary=drop_type(i(0),e.flags);break;
    case 0x14b:{const i32 health=i(0);e.health=e.max_health=e.interrupt_health=health;e.scaled_health=signed_bits(u32(health)*7);
        if(e.flags&0x80000){for(auto& segment:w.health_segments)segment={0,0};e.flags|=0x4000000;}break;}
    case 0x14c:{const i32 slot=i(0);if(slot>=8)return -2;w.manager_flags&=~1u;
        if(slot<0){if(e.flags&0x80000){if(e.boss_slot<0||e.boss_slot>=8)return -2;w.bosses[e.boss_slot]=nullptr;}e.flags&=~0x80000u;}
        else {w.bosses[slot]=&e;e.boss_slot=slot;e.flags|=0x80000;}
        world.boss=w.bosses[0];break;}
    case 0x14e:{const i32 time=i(2),health=i(1),slot=i(0);if(slot<0||slot>=8)return -2;auto& interrupt=e.interrupts[slot];interrupt.health=health;
        if(health>=0){interrupt.time=time;interrupt.health_script=interrupt.timeout_script=reinterpret_cast<const char*>(c.instruction)+32;}break;}
    case 0x14f:e.damage_immunity.set(i(0),rate);break;
    case 0x14d:e.lifetime.set(0,rate);break;
    case 0x151:{if(!w.shakes)return -2;const i32 end=i(2),start=i(1),duration=i(0);w.shakes->start(duration,start,end,rate);break;}
    case 0x152:if(!w.dialogue||!w.dialogue->start_dialogue(i(0)))return -2;break;
    case 0x153:return !w.dialogue?-2:w.dialogue->dialogue_waiting()?-1:0;
    case 0x154:return w.bosses[0]?-1:0;
    case 0x155:{const i32 slot=i(0);if(slot<0||slot>=8)return -2;e.interrupts[slot].timeout_script=reinterpret_cast<const char*>(c.instruction)+24;break;}
    case 0x156:case 0x15c:case 0x165:case 0x166:case 0x167:{
        if(!w.spells||!w.bosses[0]||c.instruction->length<32)return -2;
        const i32 count=c.instruction->argument<i32>(3);
        if(count<=0||count>64||u32(count)>u32(c.instruction->length)-32)return -2;
        char name[65]{};u8 key=0x77,step=7;const auto* encoded=reinterpret_cast<const u8*>(c.instruction)+32;
        for(i32 j=0;j<count;++j){name[j]=char(encoded[j]^key);key=u8(key+step);step=u8(step+0x10);}
        if(!std::memchr(name,0,count))return -2;
        i32 id=i(0);const i32 base_id=id;if(op>=0x165)id=wrapping_add(id,wrapping_sub(world.difficulty,op-0x165));
        w.spells->translation_rank=id>=base_id?u32(id-base_id):0;
        (void)i(2);const i32 timeout=i(1);
        if(!w.spells->begin(id,timeout,name,w.bosses[0]->current.position,w.spells->bomb_active))return -2;
        e.health_flags|=1;e.scaled_health=signed_bits(u32(e.health)*7);e.lifetime.set(0,rate);break;
    }
    case 0x157:if(!w.spells||!w.spells->end())return -2;e.health_flags&=~1u;break;
    case 0x16a:if(!w.spells)return -2;w.spells->survival();break;
    case 0x16b:if(!w.spells)return -2;w.spells->hide_circle();break;
    case 0x158:{const i32 section=i(0);if(w.stage_section!=section)w.section_frame=0;w.stage_section=section;break;}
    case 0x15a:{const double radius=c.float_argument(0,g);e.exclusion_distance=float(radius*radius);break;}
    case 0x15b:{const i32 type=i(2);const float value=f(1);const i32 health=e.max_health,slot=i(0);if(slot<0||slot>=4)return -2;w.health_segments[slot]={float(double(value)/health),type};break;}
    case 0x15d:case 0x15e:{const u32 index=op==0x15d?(world.rank<512?0:2):rank_index(world.rank);auto* out=c.float_reference(0,g);destination(out)=f(index);break;}
    case 0x15f:{const double start=f(1),end=f(2);const float value=float((double(world.rank)+1024)*(end-start)*0.00048828125+start);destination(c.float_reference(0,g))=value;break;}
    case 0x160:case 0x161:{const u32 index=op==0x160?(world.rank<512?0:2):rank_index(world.rank);auto* out=c.integer_reference(0,g);destination(out)=i(index);break;}
    case 0x162:{const i32 start=i(1),end=i(2);const i32 product=signed_bits(u32(wrapping_sub(end,start))*u32(wrapping_add(world.rank,1024)));destination(c.integer_reference(0,g))=wrapping_add(product/2048,start);break;}
    case 0x163:{if(u32(world.difficulty)>4)break;const i32 value=i(world.difficulty>=3?4:world.difficulty+1);destination(c.integer_reference(0,g))=value;break;}
    case 0x164:{if(u32(world.difficulty)>4)break;const float value=f(world.difficulty>=3?4:world.difficulty+1);destination(c.float_reference(0,g))=value;break;}
    case 0x168:w.remaining_phases=i(0);break;
    case 0x169:e.collision_immunity.set(i(0),rate);break;
    case 0x16c:e.flags=(e.flags&~0x800000u)|((u32(i(0))&1)<<23);break;
    case 0x16e:e.flags=(e.flags&~0x1000000u)|((u32(i(0))&1)<<24);e.alternate_animation=i(1);e.flags&=~0x2000000u;e.saved_animation=e.base_animation;break;
    case 0x16f:world.rate=f(0);break;
    case 0x170:{const i32 difficulty=world.difficulty;const float frames=float(i(difficulty==0?0:difficulty==1?1:difficulty==2?2:3));c.time=float(double(c.time)-frames);break;}
    case 0x171:e.flags=(e.flags&~0x8000000u)|((u32(i(0))&1)<<27);break;
    case 0x172:e.animation_options=u32(i(0));break;
    case 0x174:e.facing=i(0);break;
    case 0x1b9:if(!w.stage||!w.stage->interrupt(i(0)))return -2;break;
    case 0x1ba:w.manager_flags=(w.manager_flags&~1u)|(u32(i(0))&1u);break;
    case 0x1bb:{const i32 id=i(0);if(u32(id)>u32(EnemyTick::TransformAngles))return -2;e.tick_callback=EnemyTick(id);break;}
    case 0x1bc:{const i32 id=i(0);if(u32(id)>u32(EnemyDamage::Arc))return -2;e.damage_callback=EnemyDamage(id);break;}
    case 0x1bd:{const i32 id=i(0);if(u32(id)>u32(EnemyCollision::Arc))return -2;e.collision_callback=EnemyCollision(id);break;}
    case 0x1c1:{const i32 id=i(0);i32 result=0;if(!id||u32(id)>u32(EnemyTick::TransformAngles)||!w.callbacks||!w.callbacks->invoke_tick(e,EnemyTick(id),result))return -2;break;}
    case 0x1b8:{
        if(!w.deformations)return -2;
        if(e.deformation&&!w.deformations->release_deformation(e))return -2;
        e.deformation=nullptr;e.deformation_target=f(0);e.deformation_radius=16;e.deformation_color=u32(i(1));
        e.deformation_phase_x=e.deformation_phase_y=normalize_angle(0);
        if(e.deformation_target>0&&!w.deformations->create_deformation(e))return -2;break;
    }
    default:return -2;
    }
    return 0;
}
}
