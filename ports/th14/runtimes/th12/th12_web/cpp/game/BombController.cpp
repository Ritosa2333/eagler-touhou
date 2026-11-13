#include "BombController.hpp"
#include "EnemyAnimations.hpp"
#include "GraphicsMath.hpp"
#include <cmath>
namespace th11 {
namespace {constexpr i32 marisa_b_scripts[]={48,50,52,54,56};}
BombController::BombController(AnmManager& a,AnmResource& r,PlayerFrame& p,BombWorld& w,u16 file,AnmResource* text):animations(a),resource(r),player(p),world(w),file_id(file),text_resource(text){
#if PORT_TITLE == 14
    // Retail411bc0 constructs these fields on every controller creation,
    // including incoming stages; they are not an initial-entry-only fixup.
    state.elapsed.set(0,&animations.rate);
    state.secondary.previous=0;
#endif
}
BombController::~BombController(){
    if(auto* vm=animations.find(state.animation))if(vm->after_update==ring_noise){vm->after_update=nullptr;vm->reserved_418=0;}
#if PORT_TITLE == 14
    // Retail411980 marks its live primary/auxiliary families before unregistering
    // callbacks. A shield can outlive its controller at a stage transition.
    EnemyAnimations helper(animations);helper.erase(state.animation);helper.erase(state.auxiliary);
#endif
}
bool BombController::sound(i32 id,bool positional){return world.bomb_sound(id,positional?state.position.x:0,positional);}
bool BombController::create(u32& id,i32 script,bool positioned){
    Vec3 p={float((double(state.position.x)+32)+192),float(double(state.position.y)+16),state.position.z};
    AnmVm* vm;
#if PORT_TITLE == 14
    if(selection>=0&&selection<=5){const Vec3 raw=positioned?state.position:Vec3{};const float angle=0;vm=animations.create(resource,script,file_id,u32(-1),false,false,&raw,nullptr,nullptr,false,&angle);}
    else
#endif
    vm=animations.create(resource,script,file_id,22,false,false,positioned?&p:nullptr);
    // Keep what the ANM manager saw when it refused: an opcode means the script itself failed, a depth of 128
    // means the recursion guard tripped, and neither is visible from the caller afterwards.
    if(!vm){create_script=script;create_opcode=animations.last_error;create_depth=animations.depth();create_scripts=u32(resource.scripts.size());return false;}
    id=vm->id;return true;
}
bool BombController::body(i32 script){
#if PORT_TITLE == 14
    const bool bound=player.motion.body.copy_script(resource,script,file_id,&animations.rate);
#else
    const bool bound=player.motion.body.bind_script(resource,script,file_id,&animations.rate);
#endif
    return bound&&player.motion.body.update(animations)>=0;
}
void BombController::invincible(i32 frames){player.state.invincibility.set(frames,&animations.rate);}
bool BombController::fail_spell(){auto& spell=player.spell;if(spell.flags&1){if(spell.elapsed>=60){spell.bonus=0;spell.flags&=~0x22u;}else if(state.active&&(kPortTitle==14||selection!=5))spell.flags|=0x20;}return true;}
void BombController::interrupt(u32 id,i16 label){if(auto* vm=animations.find(id))for(auto* n=&vm->child;n;n=n->next)n->value->interrupt(label);}
AnmVm* BombController::child(i32 script){auto* vm=animations.find(state.animation);if(!vm){state.animation=0;return nullptr;}for(auto* n=&vm->child;n;n=n->next)if(n->value->script_index==script)return n->value;return nullptr;}
Vec3 BombController::game_position(const AnmVm& vm)const{
    const Vec3 sum={float(double(vm.script_position.x)+vm.position.x),float(double(vm.script_position.y)+vm.position.y),float(double(vm.script_position.z)+vm.position.z)};
    return {float(double(float(double(sum.x)+vm.child_position.x))-224),float(double(float(double(sum.y)+vm.child_position.y))-16),float(double(sum.z)+vm.child_position.z)};
}
i32 BombController::start(){
    // A start that cannot finish must leave no trace behind. The original has no failure path here, so a Bomb
    // left active with no animation would be the port's own invention - and it never ends, because the tick
    // finds no animation to retire. Reset the state on every refusal and let the caller decide what to do.
    const auto fail=[&](i32 code){
        last_error=code;state.active=0;state.animation=0;state.auxiliary=0;state.elapsed.set(0,&animations.rate);
        return -2;
    };
    if(state.active)return -1;if(!supported())return fail(-2);
    state.active=1;state.elapsed.set(0,&animations.rate);
    state.started_during_spell=(player.spell.flags&1)&&player.spell.elapsed>=60;
    if(!world.bomb_sound(kPortTitle==14?44:38,player.motion.state.position.x,true))return fail(-3);
    state.radius=32;state.growth=10;state.position=selection==0?Vec3{0,224,0}:player.motion.state.position;
#if PORT_TITLE == 14
    if(selection==0){
        state.position=player.motion.state.position;
        if(!sound(49))return fail(-4);fail_spell();auto& count=world.bomb_count();count=wrapping_add(count,1);
        th14_reimu_areas.fill(256);th14_reimu_angles.fill(0);th14_reimu_shift={};th14_reimu_allocated=true;
        if(!create(state.animation,14,true)||!create(state.auxiliary,24,true))return fail(-4);
        constexpr float heights[]={96,80,64,40};constexpr i32 duration[]={300,100,70,50};
        for(u32 i=0;i<4;++i){
            if(!player.shots.th14_reimu_bomb_area(state.position,{128,heights[i]},duration[i],th14_reimu_areas[i]))return fail(-4);
            if(i)player.shots.th14_bomb_area(th14_reimu_areas[i])->timer.advance(-float(i*3));
        }
        th14_reimu_positions.fill(state.position);player.th14_scale.flags|=4;return 0;
    }
    if(selection==1){
        if(!sound(49))return fail(-4);fail_spell();auto& count=world.bomb_count();count=wrapping_add(count,1);
        th14_orbs={};th14_orbs_allocated=true;if(!create(state.auxiliary,24,true))return fail(-4);return 0;
    }
    if(selection==2){
        state.growth=-1.5707963705062866f;if(!sound(49)||!create(state.animation,19,true))return fail(-4);
        fail_spell();invincible(120);auto& count=world.bomb_count();count=wrapping_add(count,1);
        if(!world.th14_bomb_shake(3,60,240,30)||!create(state.auxiliary,27,true))return fail(-4);
        player.th14_scale.flags|=4;return 0;
    }
    if(selection==4){
        fail_spell();auto& count=world.bomb_count();count=wrapping_add(count,1);invincible(120);
        if(!create(state.animation,16,true)||!create(state.auxiliary,19,true)||!sound(50))return fail(-4);
        player.th14_scale.flags|=8;state.th14_mode=0;return 0;
    }
    if(selection==3){
        if(!sound(49)||!create(state.animation,28,true))return fail(-4);
        fail_spell();invincible(200);auto& count=world.bomb_count();count=wrapping_add(count,1);
        if(!create(state.auxiliary,33,true)){EnemyAnimations helper(animations);helper.erase(state.animation);return fail(-4);}
        state.growth=0;return 0;
    }
    if(selection==5){
        fail_spell();auto& count=world.bomb_count();count=wrapping_add(count,1);invincible(180);
        if(!create(state.animation,21,false)||!create(state.auxiliary,20,true)||!sound(51)){
            EnemyAnimations helper(animations);helper.erase(state.animation);helper.erase(state.auxiliary);return fail(-4);
        }
        state.growth=0;return 0;
    }
#endif
    if(selection==0){
        // Each step records its own code: a bare -4 for the whole chain cannot say which of the five calls
        // failed, and a bomb that stops activating from one point in a stage on is exactly that question.
        if(!sound(49))return fail(-41);
        if(!create(state.animation,35,false))return fail(-42);
        if(!create(state.auxiliary,36,false))return fail(-43);
        if(!fail_spell())return fail(-44);
        if(!body(26))return fail(-45);
        player.motion.state.flags|=2;player.motion.state.previous_dx=player.motion.state.previous_dy=0;
    }else if(selection==1){if(!sound(49)||!create(state.animation,33,true)||!fail_spell())return fail(-4);}
    else if(selection==2){
        if(!sound(49)||!create(state.animation,37,true)||!create_distortion()||!fail_spell())return fail(-4);
        auto* vm=animations.find(state.animation);vm->after_update=ring_noise;vm->reserved_418=u32(reinterpret_cast<uintptr_t>(this));
    }
    else if(selection==3){
        state.position.y=float(double(state.position.y)-32);
        if(!create(state.animation,43,true)||!create_distortion()||!fail_spell())return fail(-4);
        invincible(120);
    }
    else if(selection==4){if(!create(state.animation,47,true)||!fail_spell())return fail(-4);}
    else if(selection==5){
        state.secondary.set(0,&animations.rate);
        if(!create(state.animation,58,true))return fail(-4);
        player.motion.state.flags|=4;
        // Nitori's shield counts as a used Bomb only when it is hit. An
        // unused shield refunds ten power when its animation expires.
        return 0;
    }
    auto& count=world.bomb_count();count=wrapping_add(count,1);return 0;
}
i32 BombController::tick(){
    AnmVm* vm=nullptr;
    if(kPortTitle!=14||selection!=4||!state.th14_mode){vm=animations.find(state.animation);if(!vm)state.animation=0;}
#if PORT_TITLE == 14
    if(selection==0){
        invincible(40);const auto position=player.motion.state.position;
        if(auto* a=animations.find(state.auxiliary))a->position=position;
        player.motion.state.th14_movement_factor=.33000001311302185f;
        const i32 t=state.elapsed.current,phase=t%60;
        if(phase<=20||(phase>=30&&phase<=50)){
            if(phase==0||phase==30){auto& v=th14_reimu_shift;v.start[0]=0;v.end[0]=phase==0?-12.f:12.f;v.duration=20;v.mode=InterpolationMode(22);v.timer.set(0,&animations.rate);}
            player.motion.state.th14_bomb_shift={sample(th14_reimu_shift,&animations.rate),0,0};
        }
        if(vm){
            vm->position=position;
            // 413840 uses the first script -2 match, or the final non-root
            // child when no match exists (the retail script 14 spawns 13).
            AnmVm* part=nullptr;
            for(auto* n=&vm->child;n;n=n->next){if(!n->value||n->value==vm)continue;part=n->value;if(part->script_index==-2||!n->next)break;}
            if(!part){last_error=-6;return -2;}
            float angle=part->rotation.z;if(!(part->scale.x>0))angle-=3.1415927410125732f;
            const Vec3 p{position.x+float(std::cos(double(angle))*112.),position.y+float(std::sin(double(angle))*112.),float(position.z+animations.th14_native_stack.controller_polar_z())};
            if(t==0)for(u32 i=1;i<10;++i){th14_reimu_positions[i]=p;th14_reimu_angles[i]=angle;}
            for(u32 i=9;i>0;--i){th14_reimu_positions[i]=th14_reimu_positions[i-1];th14_reimu_angles[i]=th14_reimu_angles[i-1];}
            th14_reimu_positions[0]=p;th14_reimu_angles[0]=angle;
            for(u32 i=0;i<4;++i){auto* a=player.shots.th14_bomb_area(th14_reimu_areas[i]);if(!a)return -2;
                a->position=th14_reimu_positions[i*3];a->angle=normalize_angle(th14_reimu_angles[i*3]);
                if(t%30==i32(20+i*3))a->period=10;
                if(t%30==i32(15+i*3))a->period=1;
            }
        }
        if(t>=250){
            for(auto index:th14_reimu_areas)if(auto* a=player.shots.th14_bomb_area(index))a->flags&=~1u;
            if(vm){vm->flags2|=0x20;if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->flags2|=0x20;}
            state.animation=0;interrupt(state.auxiliary,1);th14_reimu_allocated=false;player.th14_scale.flags&=~4u;return -1;
        }
        if(t%30==20){
            if(!world.bomb_sound(27,position.x,true))return -2;
            const float zero=0;if(!animations.create(resource,15,file_id,0,false,false,&position,nullptr,nullptr,false,&zero)||!world.th14_bomb_shake_short(8,10,10))return -2;
        }
        for(auto index:th14_reimu_areas){const auto* a=player.shots.th14_bomb_area(index);if(!a||!world.th14_reimu_cancel_rectangle(a->position,a->size,a->angle,!(player.spell.flags&1)))return -2;}
        return 0;
    }
    if(selection==1){
        invincible(40);if(auto* a=animations.find(state.auxiliary))a->position=player.motion.state.position;
        const i32 t=state.elapsed.current;
        if(t>=200){
            for(auto& o:th14_orbs)if(!th14_orb_explode(o))return -2;
            interrupt(state.auxiliary,1);th14_orbs_allocated=false;return world.th14_bomb_shake_short(8,6,6)?-1:-2;
        }
        if(t==0&&t!=state.elapsed.previous){
            float angle=0;for(u32 i=0;i<8;++i){auto& o=th14_orbs[i];o.anchor=player.motion.state.position;o.index=i;o.active=true;o.timer.set(0,&animations.rate);
                const float zero=0;auto* a=animations.create(resource,16,file_id,0,false,false,&o.position,nullptr,nullptr,false,&zero);
                if(!a){create_script=16;create_opcode=animations.last_error;return -2;}o.animation=a->id;
                if(!player.shots.th14_bomb_orb_area(o.position,o.area))return -2;
                o.flags=(o.flags&0xfffffff2u)|2;o.radius=0;o.angle=normalize_angle(angle);o.turn=.04908738657832146f;
                angle=normalize_angle(angle+.7853981852531433f);
            }
        }
        for(auto& o:th14_orbs){if(!o.active)continue;
            th14_reimu_orb_tick(o,player.motion.state.position,world.th14_bomb_enemies(),true,&animations.rate);
            if(auto* a=animations.find(o.animation))a->position=o.position;
            auto* area=player.shots.th14_bomb_area(o.area);if(!area)return -2;
            if(area->total>=300){
                if(!th14_orb_explode(o)||!world.bomb_sound(27,o.position.x,true)||!world.th14_bomb_shake_short(8,6,6))return -2;
            }else area->position=o.position;
        }
        for(u32 i=0;i<8;++i)if(th14_orbs[i].active&&t%8==i32(i))if(!world.th14_bomb_cancel(th14_orbs[i].position,64,!(player.spell.flags&1)))return -2;
        return 0;
    }
    if(selection==2){
        invincible(40);if(!vm){interrupt(state.auxiliary,1);return -1;}
        vm->flags|=4;vm->rotation.z=state.growth;
        const float dx=player.motion.state.delta.x;if(dx<0)state.growth-=.0026179938577115536f;else if(dx>0)state.growth+=.0026179938577115536f;
        if(state.elapsed.current>300){interrupt(state.animation,1);interrupt(state.auxiliary,1);player.th14_scale.flags&=~4u;player.motion.state.th14_movement_factor=1;return -1;}
        state.position=player.motion.state.position;player.motion.state.th14_movement_factor=.20000000298023224f;
        const i32 t=state.elapsed.current;
        if(t!=state.elapsed.previous&&t%3==0){
            const float distances[]={208,240,304},heights[]={32,128,256};const i32 damages[]={60,20,20};
            const double c=std::cos(double(state.growth)),s=std::sin(double(state.growth));
            // 412e30 multiplies in x87 precision before storing each float. Its reused vector
            // writes only x/y, so the caller accumulates position.z across the three requests.
            float z=0;for(u32 i=0;i<3;++i){z+=state.position.z;const Vec3 p{state.position.x+float(c*distances[i]),state.position.y+float(s*distances[i]),z};
                if(!world.th14_bomb_rectangle(p,{512,heights[i]},state.growth,0,damages[i]))return -2;}
        }
        vm->position=state.position;for(auto* n=vm->child.next;n;n=n->next){auto& a=*n->value;if(a.script_index!=26)continue;
            if(!world.th14_bomb_cancel_rectangle(th14_raw_position(a),{a.scale.x*48.f,a.scale.y*160.f},state.growth,!(player.spell.flags&1)))return -2;}
        return 0;
    }
    if(selection==4){
        const auto cancel_auxiliary=[&](){auto* a=animations.find(state.auxiliary);if(!a){state.auxiliary=0;return true;}
            return a->age.current>=40||world.th14_bomb_cancel(state.position,a->scale.x*.30000001192092896f,!(player.spell.flags&1));};
        if(!state.th14_mode){
            if(!vm)return -1;state.position=player.motion.state.position;vm->position=state.position;
            if(player.state.life_state==4){
                if(!create(state.auxiliary,19,true))return -2;interrupt(state.animation,2);
                player.state.state_timer.set(60,&animations.rate);player.state.life_state=1;
                state.active=1;state.th14_mode=1;state.elapsed.set(0,&animations.rate);invincible(120);player.th14_scale.flags&=~8u;
                if(!sound(66,true))return -2;
            }else{
                const i32 t=state.elapsed.current;if(t<60)player.motion.state.th14_movement_factor=0;else if(t==120)state.active=2;
                if(t==720||world.th14_bomb_dialogue()){
                    if(!sound(47,true))return -2;EnemyAnimations helper(animations);helper.erase(state.animation);state.animation=0;player.th14_scale.flags&=~8u;
                    Vec3 p=state.position;p.x-=16.f;p.y-=64.f;p.z=0;
                    if(!world.th14_bomb_item(6,p,-1.5707963705062866f,2.200000047683716f))return -2;
                    p.x+=32.f;if(!world.th14_bomb_item(6,p,-1.5707963705062866f,2.200000047683716f))return -2;
                    p.x-=16.f;p.y-=32.f;if(!world.th14_bomb_item(6,p,-1.5707963705062866f,2.200000047683716f))return -2;return -1;
                }
                if(t<=720){vm->flags|=8;vm->scale.x=1.f-(state.elapsed.fractional/720.f)*.8999999761581421f;}
                return cancel_auxiliary()?0:-2;
            }
        }
        if(state.elapsed.current>=60){player.th14_scale.flags&=~8u;return -1;}
        player.motion.state.th14_movement_factor=0;
        if(auto* a=animations.find(state.auxiliary)){if(!world.th14_bomb_cancel(state.position,a->scale.x,!(player.spell.flags&1)))return -2;}
        else state.auxiliary=0;
        return cancel_auxiliary()?0:-2;
    }
    if(selection==3){
        if(!vm){interrupt(state.auxiliary,1);return -1;}
        state.growth+=.0020000000949949026f;state.position.y-=state.growth;vm->position=state.position;
        const i32 t=state.elapsed.current;const float radius=vm->scale.x*72.f;
        if(t!=state.elapsed.previous&&t%4==0&&!world.th14_bomb_circle(state.position,radius,0,0,90))return -2;
        if(t==180){interrupt(state.auxiliary,1);return world.th14_bomb_cancel(state.position,radius,2)?0:-2;}
        if(t==500||world.th14_bomb_dialogue()){interrupt(state.animation,1);interrupt(state.auxiliary,1);return -1;}
        return world.th14_bomb_cancel(state.position,radius,2)?0:-2;
    }
    if(selection==5){
        if(!vm)return -1;
        const i32 t=state.elapsed.current;
        if(t<110)return world.th14_bomb_freeze(true)?0:-2;
        if(t==110){
            if(!sound(27)||!sound(41)||!world.th14_bomb_freeze(false)||!world.th14_bomb_cancel(state.position,600.f,3))return -2;
        }
        return 0;
    }
#endif
    if(selection==0){
        if(!vm){if(!body(0))return -2;player.motion.state.flags&=~2u;player.motion.state.previous_dx=player.motion.state.previous_dy=0;invincible(40);return -1;}
        if(state.elapsed.current>=180){
            if(state.elapsed.current==180){if(!sound(38)||!world.bomb_stop_sound(49))return -2;player.shots.damage_areas.circle(state.position,32,10,30,50,&animations.rate);}
            if(!world.bomb_cancel(state.position,state.radius,!(player.spell.flags&1),false))return -2;
            state.radius=float(double(state.radius)+state.growth);
        }return 0;
    }
    if(selection==3){
        if(!vm)return -1;
        if(state.elapsed.current>90){interrupt(state.animation,1);mesh.reset();return -1;}
        update_distortion();return world.bomb_cancel_beam(state.position,!(player.spell.flags&1))?0:-2;
    }
    if(selection==5){
        const i32 t=state.elapsed.current,s=state.secondary.current;
        const u32 shade=u32(s>1?(s*96)/30+32:t>=890?((t*3-2670)*32)/30+32:t<30?128-(t*96)/30:32);
        if(!world.bomb_background_color((((shade|0xffffff00u)<<8)|shade)<<8|shade))return -2;
        if(!vm){
            player.motion.state.flags&=~4u;
            if(!world.bomb_sound(51,player.motion.state.position.x,true)||!world.bomb_refund_power())return -2;
            return -1;
        }
        state.position=player.motion.state.position;
        EnemyAnimations helper(animations);helper.position(state.animation,state.position,true);
        if(!(player.motion.state.flags&4)||player.state.life_state==4||player.state.life_state==2){
            fail_spell();auto& count=world.bomb_count();count=wrapping_add(count,1);player.state.life_state=1;
            if(!state.secondary.current){
                state.secondary.set(1,&animations.rate);
                if(!sound(38))return -2;
                player.shots.damage_areas.circle(state.position,32,20,30,75,&animations.rate);
            }
        }
        if(state.secondary.current>0){
            player.motion.state.flags&=~4u;interrupt(state.animation,1);
            if(!world.bomb_cancel(state.position,state.radius,!(player.spell.flags&1),false))return -2;
            state.radius=float(double(state.radius)+state.growth);state.secondary.tick();invincible(40);
            if(state.secondary.current>30){helper.erase(state.animation);state.animation=0;return -1;}
        }
        return 0;
    }
    invincible(40);if(!vm){if(selection==2)mesh.reset();return -1;}
    if(selection==2){
        state.position=player.motion.state.position;EnemyAnimations helper(animations);helper.position(state.animation,state.position,true);
        if(!world.bomb_cancel(state.position,state.radius,!(player.spell.flags&1),false))return -2;
        state.radius=vm->scale.y;update_distortion();return 0;
    }
    if(selection==1){
        if(!world.bomb_cancel(state.position,96,2u|!(player.spell.flags&1),true))return -2;
        if(state.elapsed.current>=200){
            if(state.elapsed.current==200){if(!sound(38)||!world.bomb_stop_sound(49))return -2;interrupt(state.animation,1);}
            state.radius=float(double(state.radius)+state.growth);
        }return 0;
    }
    if(selection==4){
        const i32 t=state.elapsed.current;if(t%10==0&&t<50&&t>1&&!sound(38,true))return -2;
        for(const auto script:marisa_b_scripts)if(auto* part=child(script))if(!world.bomb_cancel(game_position(*part),part->scale.y,!(player.spell.flags&1),false))return -2;
        state.radius=vm->scale.y;return 0;
    }return -2;
}
bool BombController::update(){
    // An inactive controller has nothing to update and must not answer for a refusal: `last_error` stays set so
    // the reason a Bomb could not start is still reportable, but it cannot be latched into a failed frame.
    if(!state.active)return true;if(last_error)return false;if(!supported()){last_error=-2;return false;}
    const i32 result=(kPortTitle==14||state.active==1)?tick():0;if(result==-2){last_error=-5;return false;}
    if(result){state.active=0;return true;}state.elapsed.tick();return true;
}
bool BombController::damage(const Vec3& p,const Vec2& size,i32& value){
    value=0;if(!state.active)return true;if(!supported())return false;
#if PORT_TITLE == 14
    // All six retail vtable +0xc methods return zero (413eb0, 4150c0,
    // 4129c0, 4130d0, 415a80, 415f00). TH14 Bomb damage is delivered
    // through the player's damage-area pool, rather than this virtual.
    return true;
#endif
    if(selection==2||selection==3){
        const double dx=double(p.x)-state.position.x,dy=double(p.y)-state.position.y;
        const float d=float(dx*dx+dy*dy);const double radius=selection==2?state.radius:48;
        if(double(d)<radius*radius){value=selection==2?17:50;return true;}
        if(selection==3){
            const float left=float(double(state.position.x)-16),right=float(double(state.position.x)+16),top=float(double(state.position.y)-448),bottom=state.position.y;
            const float minx=float(double(p.x)-double(size.x)*.5),maxx=float(double(p.x)+double(size.x)*.5),miny=float(double(p.y)-double(size.y)*.5),maxy=float(double(p.y)+double(size.y)*.5);
            if(left<=maxx&&top<=maxy&&minx<=right&&miny<=bottom)value=8;
        }
    }
    if(selection==4)for(const auto script:marisa_b_scripts)if(auto* vm=child(script)){
        const auto center=game_position(*vm);const double dx=double(p.x)-center.x,dy=double(p.y)-center.y;
        const float d=float(dx*dx+dy*dy);if(double(d)<double(vm->scale.y)*vm->scale.y)value+=20;
    }return true;
}
#if PORT_TITLE == 14
bool BombController::th14_orb_explode(Th14ReimuOrb& o){
    if(o.animation){
        if(!world.bomb_sound(27,o.position.x,true)||!world.th14_bomb_cancel(o.position,128,1)||!world.th14_bomb_circle(o.position,64,8,11,100))return false;
    }
    interrupt(o.animation,1);o.animation=0;o.active=false;
    if(auto* area=player.shots.th14_bomb_area(o.area))area->flags&=~1u;o.area=256;return true;
}
#endif
Vec3 BombController::th14_raw_position(const AnmVm& vm)const{
    Vec3 p{(vm.script_position.x+vm.position.x)+vm.child_position.x,(vm.script_position.y+vm.position.y)+vm.child_position.y,(vm.script_position.z+vm.position.z)+vm.child_position.z};
    if(vm.parent&&!(vm.flags2&0x8000)){
        if(vm.flags2&0x400000){const float s=float(std::sin(double(vm.parent->rotation.z))),c=float(std::cos(double(vm.parent->rotation.z)));const float x=p.x*c-p.y*s,y=p.x*s+p.y*c;p.x=x;p.y=y;}
        const auto q=th14_raw_position(*vm.parent);p.x+=q.x;p.y+=q.y;p.z+=q.z;
    }return p;
}
bool BombController::create_distortion(){
    if(!text_resource)return false;mesh=std::make_unique<ScreenDeformation>(animations);
    if(selection==2){
        if(!mesh->initialize(*text_resource,13,13))return false;
        mesh->rectangle(float(double(state.position.x)-17),float(double(state.position.y)-17),34,34);
    }else{
        const float bottom=float(double(state.position.y)+64);
        const i32 rows=(truncate_int(bottom)+32)/32+2;
        if(rows<3||!mesh->initialize(*text_resource,4,u32(rows),true))return false;
        mesh->rectangle(float(double(state.position.x)-60),0,120,float(double(bottom)+32));
        for(u32 row=0;row<mesh->rows;++row){
            for(u32 col=0;col<4;++col){auto& p=mesh->positions[col*mesh->rows+row];const float jitter=float(double(animations.script_rng.signed_unit())*4);p.x=float(double(p.x)+jitter);}
            const u32 color=row+1<mesh->rows?0xffff8080:0xffffffff;
            mesh->vertices[mesh->rows+row].color=mesh->vertices[2*mesh->rows+row].color=color;
        }
    }return true;
}
void BombController::update_distortion(){
    if(!mesh)return;
    if(selection==3){
        for(u32 row=0;row+1<mesh->rows;++row)for(u32 col=1;col<3;++col){const u32 n=col*mesh->rows+row;
            const float jitter=float(double(animations.script_rng.signed_unit())*8);mesh->vertices[n].position.x=float(double(mesh->positions[n].x)+jitter);
        }return;
    }
    const float radius=float(double(state.radius)*.5+state.radius),extent=float(double(radius)*2+40);
    mesh->rectangle(float(double(state.position.x)-radius-20),float(double(state.position.y)-radius-20),extent,extent);
    const Vec3 center{float(double(state.position.x)+32+192),float(double(state.position.y)+16),state.position.z};const double squared=double(radius)*radius;
    for(u32 i=0;i<mesh->vertices.size();++i){auto& v=mesh->vertices[i];auto& p=mesh->positions[i];
        const Vec3 delta{float(double(p.x)-center.x),float(double(p.y)-center.y),float(double(p.z)-center.z)};
        const float distance=float(double(delta.x)*delta.x+double(delta.y)*delta.y),remaining=float(squared-distance);
        if(remaining<0)v.color&=0xffffff;
        else{
            const float strength=float(double(float(double(remaining)/squared))*64);auto direction=GraphicsMath::normalize(delta);
            direction.x=float(double(direction.x)*strength);direction.y=float(double(direction.y)*strength);
            v.color=0xffff4040;v.position.x=float(double(v.position.x)+direction.x);v.position.y=float(double(v.position.y)+direction.y);v.position.z=p.z=0;
        }
    }
}
int BombController::ring_noise(AnmVm& vm){
    auto& owner=*reinterpret_cast<BombController*>(uintptr_t(vm.reserved_418));auto* vertices=static_cast<AnmVertex*>(vm.geometry);
    const i32 count=vm.integers[0]-1;if(count<=0||!vertices)return 0;
    const float step=float(double(vm.integers[1])/count);
    for(i32 i=0;i<count*2;++i){const float dx=float(double(owner.animations.script_rng.signed_unit())*8),dy=float(double(owner.animations.script_rng.signed_unit())*8);
        vertices[i].position.x=float(double(vertices[i].position.x)+dx);vertices[i].position.y=float(double(vertices[i].position.y)+dy);
    }
    const float end=float(double(vertices[count*2-1].uv.y)+step);
    vertices[count*2]=vertices[0];vertices[count*2+1]=vertices[1];vertices[count*2].uv.y=vertices[count*2+1].uv.y=end;return 0;
}
}
