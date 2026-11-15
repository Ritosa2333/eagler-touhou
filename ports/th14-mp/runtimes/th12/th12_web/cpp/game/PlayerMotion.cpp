#include "ShotFiles.hpp"
#include "PlayerMotion.hpp"
#include "PowerPointWordsTh14.hpp"
#include "HudScalarTh14.hpp"
#include <algorithm>
#include <cmath>
namespace th11 {
namespace {
template<class F>void family(AnmVm* vm,F f){if(!vm)return;f(*vm);if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)f(*n->value);}
Vec3 screen(Vec3 p){return{float((double(p.x)+32)+192),float(double(p.y)+16),p.z};}
i32 direction(u32 held){if((held&0x50)==0x50)return 5;if((held&0x60)==0x60)return 7;if((held&0x90)==0x90)return 6;if((held&0xa0)==0xa0)return 8;if(held&0x20)return 2;if(held&0x10)return 1;if(held&0x40)return 3;if(held&0x80)return 4;return 0;}
float difference(float a,float b){constexpr double pi=3.1415927410125732,tau=6.2831854820251465;if(double(a)-b>pi)return float(double(a)-(double(b)+tau));if(double(b)-a>pi)return float(double(a)-(double(b)-tau));return float(double(a)-b);}
}
PlayerMotion::PlayerMotion(AnmResource& p,AnmResource& b,AnmManager& a,PlayerMotionWorld& w,u16 pf,u16 bf):player_resource(p),bullet_resource(b),animations(a),world(w),player_file(pf),bullet_file(bf){body.initialize();}
#if PORT_TITLE == 14
void player_th14_bomb_displacement(i32& dx,i32& dy,Vec3 shift,float factor)noexcept{
    // Retail 44d621 uses the negative fixed-point scale at 4c1c94 before
    // subtraction, then rounds each SSE conversion and product.
    dx=truncate_int(float(wrapping_sub(dx,truncate_int(shift.x*-128.f)))*factor);
    dy=truncate_int(float(wrapping_sub(dy,truncate_int(shift.y*-128.f)))*factor);
}
#endif
void PlayerMotion::interrupt(u32 id,i16 label){family(animations.find(id),[&](AnmVm& a){a.interrupt(label);});}
void PlayerMotion::erase(u32 id){family(animations.find(id),[](AnmVm& a){if constexpr(kPortTitle==14)a.flags2|=0x20;else a.flags|=0x4000000;});}
void PlayerMotion::place(u32 id,Vec3 p){family(animations.find(id),[&](AnmVm& a){a.position=kPortTitle==14?p:screen(p);});}
bool PlayerMotion::change_body(i32 script){
#if PORT_TITLE == 14
    const bool bound=body.copy_script(player_resource,script,player_file,&animations.rate);
#else
    const bool bound=body.bind_script(player_resource,script,player_file,&animations.rate);
#endif
    if(!bound){last_error=1;return false;}
#if PORT_TITLE == 16
    body.parent=body.th16_transform_parent=nullptr;
#endif
    if(body.update(animations)<0){last_error=1;return false;}return true;
}
bool PlayerMotion::afterimage(){auto* a=animations.create(player_resource,19,player_file,11);if(!a){last_error=2;return false;}if(a->resource&&!a->bind_sprite(body.sprite_index)){last_error=3;return false;}place(a->id,state.position);return true;}
void PlayerMotion::set_speeds(const ShtHeader& s)noexcept{state.normal_speed=truncate_int(double(s.speed)*128);state.focus_speed=truncate_int(double(s.focus_speed)*128);state.normal_diagonal=truncate_int(double(s.diagonal_speed)*128);state.focus_diagonal=truncate_int(double(s.focus_diagonal_speed)*128);}
#if PORT_TITLE == 14
bool PlayerMotion::update_th14(const PlayerMotionInput& in){
    // Complete44d330 keyboard consumer. ANM services use the production manager.
    auto& s=state;s.direction=direction(in.held);
    if(!in.enemy_manager||!in.enemies||s.transition_timer<4){s.focused=0;s.option_lerp=30;}else s.focused=(in.held>>3)&1;
    s.weapon_mode=s.focused;
    if(s.focused){
        if(!focus_animation){focus_animation=world.create_focus();if(!focus_animation){last_error=2;return false;}}
        if(auto* vm=animations.find(focus_animation)){
            const float scale=s.th14_scale_flags&0x10?(s.th14_scale_factor-1.f)*2.f+1.f:1.f;
            vm->extra_scale={scale,scale};vm->flags|=8;
        }else focus_animation=0;
    }else{if(animations.find(focus_animation))interrupt(focus_animation,1);focus_animation=0;}
    const i32 speed=s.focused?(s.direction>=5?s.focus_diagonal:s.focus_speed):(s.direction>=5?s.normal_diagonal:s.normal_speed);
    constexpr i32 signs[9][2]={{0,0},{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{1,-1},{-1,1},{1,1}};
    i32 dx=signed_bits(u32(speed)*u32(signs[s.direction][0])),dy=signed_bits(u32(speed)*u32(signs[s.direction][1]));
    if(in.touch_mode&&std::isfinite(in.touch_x)&&std::isfinite(in.touch_y)){
        double x=double(in.touch_x)*128-s.x,y=double(in.touch_y)*128-s.y;
        if(in.touch_mode!=2){const double limit=s.focused?s.focus_speed:s.normal_speed,length=std::sqrt(x*x+y*y);if(length>limit&&length>0){x*=limit/length;y*=limit/length;}}
        dx=truncate_int(x);dy=truncate_int(y);s.direction=direction((dx<0?0x40:dx>0?0x80:0)|(dy<0?0x10:dy>0?0x20:0));
    }
    player_th14_bomb_displacement(dx,dy,s.th14_bomb_shift,s.th14_movement_factor);
    if(dx<0&&s.previous_dx>=0){if(!change_body(1))return false;}
    else if(dx>0&&s.previous_dx<=0){if(!change_body(3))return false;}
    else if(!dx&&s.previous_dx<0){if(!change_body(2))return false;}
    else if(!dx&&s.previous_dx>0){if(!change_body(4))return false;}
    s.previous_dx=dx;s.previous_dy=dy;s.delta.x=float(dx)*animations.rate;s.delta.y=float(dy)*animations.rate;
    if(s.direction)s.last_delta=s.delta;s.dx=truncate_int(s.delta.x);s.dy=truncate_int(s.delta.y);
    s.x=std::max(-0x5c00,std::min(0x5c00,wrapping_add(s.x,s.dx)));s.y=std::max(0x1000,std::min(0xd800,wrapping_add(s.y,s.dy)));
    s.position.x=float(s.x)*.0078125f;s.position.y=float(s.y)*.0078125f;
    if(auto* vm=animations.find(focus_animation))vm->position=s.position;else focus_animation=0;
    if(dx||dy){
        i32 end=32;const i32 retained=s.option_count*8;
        if(retained<32&&retained>=0){while(end>retained){s.th14_history[end]=s.th14_history[retained];--end;}}
        while(end>0){s.th14_history[end]=s.th14_history[end-1];--end;}
    }
    s.th14_history[0]={s.x,s.y};
    // The portable recall bit is8; native182d4 uses bit2 for this consumer.
    if(s.flags&8)s.recall_timer=wrapping_add(s.recall_timer,1);
    for(auto& o:options){if(!o.state)continue;
        if(!(s.flags&8)){
            o.target_x=wrapping_add(s.x,s.focused?o.focus_x:o.normal_x);o.target_y=wrapping_add(s.y,s.focused?o.focus_y:o.normal_y);
            if(o.behavior!=OptionBehavior::None){last_error=18;return false;}
        }else{o.target_x=s.x;o.target_y=s.y;if(s.recall_timer>=30){o.state=0;interrupt(o.animation,1);interrupt(o.auxiliary,1);s.option_count=0;continue;}}
        if(o.snap){o.snap=0;o.x=o.target_x;o.y=o.target_y;}
        else if(s.option_lerp>=30){
            const i32 x=signed_bits(u32(wrapping_sub(o.target_x,o.x))*u32(s.option_lerp))/100,y=signed_bits(u32(wrapping_sub(o.target_y,o.y))*u32(s.option_lerp))/100;
            if(!x&&!y){o.x=o.target_x;o.y=o.target_y;}else{o.x=wrapping_add(o.x,x);o.y=wrapping_add(o.y,y);}
        }
        const Vec3 p{float(o.x)*.0078125f,float(o.y)*.0078125f,0};
        // Native writes each root VM's position, leaving children untouched.
        for(u32 id:{o.animation,o.auxiliary})if(auto* vm=animations.find(id))vm->position=p;
    }return true;
}
#endif
#if PORT_TITLE == 16
bool PlayerMotion::update_th16(const PlayerMotionInput& input){
    // Full441cf0. Body script and effect delivery use the own ANM manager;
    // the twelve option consumers are442380, not the old character specials.
    auto& s=state;s.direction=direction(input.held);
    if(!input.enemy_manager||!input.enemies||s.transition_timer<4){s.focused=0;s.option_lerp=30;}else s.focused=(input.held>>3)&1;
    if(s.focused){
        if(!focus_animation){focus_animation=world.create_movement_effect(26);if(!focus_animation){last_error=2;return false;}}
        if(auto* vm=animations.find(focus_animation)){const float scale=s.flags&0x10?(s.th16_scale_factor-1.f)*2.f+1.f:1.f;vm->extra_scale={scale,scale};vm->flags|=8;}
        else focus_animation=0;
    }else{if(animations.find(focus_animation))interrupt(focus_animation,1);focus_animation=0;}
    const i32 speed=s.focused?(s.direction>=5?s.focus_diagonal:s.focus_speed):(s.direction>=5?s.normal_diagonal:s.normal_speed);
    constexpr i32 signs[9][2]={{0,0},{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{1,-1},{-1,1},{1,1}};
    i32 dx=signed_bits(u32(speed)*u32(signs[s.direction][0])),dy=signed_bits(u32(speed)*u32(signs[s.direction][1]));
    dx=truncate_int(float(wrapping_sub(dx,truncate_int(s.th16_displacement.x*-128.f)))*s.th16_speed_multiplier);
    dy=truncate_int(float(wrapping_sub(dy,truncate_int(s.th16_displacement.y*-128.f)))*s.th16_speed_multiplier);
    // The browser touch input shares this fixed-point displacement boundary.
    if(input.touch_mode&&std::isfinite(input.touch_x)&&std::isfinite(input.touch_y)){
        double x=double(input.touch_x)*128-s.x,y=double(input.touch_y)*128-s.y;
        if(input.touch_mode!=2){const double limit=s.focused?s.focus_speed:s.normal_speed,length=std::sqrt(x*x+y*y);if(length>limit&&length>0){x*=limit/length;y*=limit/length;}}
        dx=truncate_int(x);dy=truncate_int(y);
    }
    if(dx<0&&s.previous_dx>=0){if(!change_body(1))return false;}
    else if(dx>0&&s.previous_dx<=0){if(!change_body(3))return false;}
    else if(!dx&&s.previous_dx<0){if(!change_body(2))return false;}
    else if(!dx&&s.previous_dx>0){if(!change_body(4))return false;}
    s.previous_dx=dx;s.previous_dy=dy;s.delta.x=float(dx)*animations.rate;s.delta.y=float(dy)*animations.rate;
    if(s.direction)s.last_delta=s.delta;s.dx=truncate_int(s.delta.x);s.dy=truncate_int(s.delta.y);
    s.x=std::max(-0x5c00,std::min(0x5c00,wrapping_add(s.x,s.dx)));s.y=std::max(0x1000,std::min(0xd800,wrapping_add(s.y,s.dy)));
    s.position.x=float(s.x)*.0078125f;s.position.y=float(s.y)*.0078125f;
    if(auto* vm=animations.find(focus_animation))vm->position=s.position;else focus_animation=0;
    if(s.flags&2)s.recall_timer=wrapping_add(s.recall_timer,1);
    if(!update_options_th16(0,4)||!update_options_th16(4,8))return false;
    if(s.recall_timer>=30){s.option_count=0;th16_season_count=0;}
    auto& timer=s.th16_charge_timer;
    if(timer.current>0){
        if(!animations.find(s.th16_charge_animation)){s.th16_charge_animation=world.create_movement_effect(27);if(!s.th16_charge_animation){last_error=2;return false;}}
        if(auto* vm=animations.find(s.th16_charge_animation))vm->position=s.position;
        const float rate=timer.rate?th14_rate_load(timer.rate):1.f;timer.previous=timer.current;
        timer.fractional-=rate>.99f&&rate<1.01f?1.f:rate;timer.current=truncate_int(timer.fractional);
        if(timer.current<=0){interrupt(s.th16_charge_animation,1);timer.set(0,&animations.rate);s.th16_charge_animation=0;}
    }return true;
}
bool PlayerMotion::update_options_th16(u32 base,u32 count){
    // Complete442380 with the optional per-option callback explicitly bounded
    // to zero. Native coordinates are copied only into each root VM.
    if(base+count>options.size()){last_error=17;return false;}
    for(u32 i=0;i<count;++i){auto& o=options[base+i];if(!o.state)continue;
        if(!(state.flags&2)){
            o.target_x=wrapping_add(state.x,state.focused?o.focus_x:o.normal_x);
            o.target_y=wrapping_add(state.y,state.focused?o.focus_y:o.normal_y);
            if(o.behavior!=OptionBehavior::None){last_error=18;return false;}
        }else{
            o.target_x=state.x;o.target_y=state.y;
            if(state.recall_timer>=30){o.state=0;interrupt(o.animation,1);interrupt(o.auxiliary,1);continue;}
        }
        if(o.snap){o.snap=0;o.x=o.target_x;o.y=o.target_y;}
        else if(state.option_lerp>=30){
            const i32 dx=signed_bits(u32(wrapping_sub(o.target_x,o.x))*u32(state.option_lerp))/100;
            const i32 dy=signed_bits(u32(wrapping_sub(o.target_y,o.y))*u32(state.option_lerp))/100;
            if(!dx&&!dy){o.x=o.target_x;o.y=o.target_y;}else{o.x=wrapping_add(o.x,dx);o.y=wrapping_add(o.y,dy);}
        }
        const Vec3 position{float(o.x)*.0078125f,float(o.y)*.0078125f,0};
        for(u32 id:{o.animation,o.auxiliary})if(auto* vm=animations.find(id))vm->position=position;
    }return true;
}
bool PlayerMotion::rebuild_options_th16(const ShtHeader& main,const ShtHeader& season,AnmResource& season_resource,const GameEconomy& economy,i32 character,i32 subseason){
    // Complete finite 4440e0: four main slots immediately followed by eight
    // seasonal slots. The transform/animation consumers are independent.
    constexpr i32 formation[8]={0,1,3,6,10,11,13,16},season_formation[8]={0,1,3,6,10,15,21,28};
    constexpr i32 scripts[4]={8,7,11,11},auxiliary[4]={9,8,12,13},season_scripts[5]={1,1,1,18,1};
    if(character<0||character>=4||subseason<0||subseason>=5||economy.power_step<=0){last_error=9;return false;}
    const i32 count=economy.power/economy.power_step,season_count=economy.th16_season.level();
    if(count<0||count>4||season_count<0||season_count>6){last_error=10;return false;}
    th16_requested_count=count;
    auto retire=[&](u32 id){auto* vm=animations.find(id);if(!vm||(vm->flags2&0x4000000))return;
        vm->flags2=(vm->flags2&~0x40u)|0x20;
        if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)family(n->value,[](AnmVm& child){child.flags2=(child.flags2&~0x40u)|0x20;});
    };
    const Vec3 origin{};const float rotation=0;
    auto create=[&](AnmResource& resource,i32 script,u16 file,u32 layer,bool prepend,u32 flags){
        auto* vm=animations.create(resource,script,file,layer,false,prepend,&origin,nullptr,nullptr,false,&rotation,flags);
        if(vm)vm->position.y=-32;return vm;
    };
    if(economy.power>=economy.max_power){
        for(i32 i=0;i<count;++i){auto& o=options[i];retire(o.auxiliary);o.auxiliary=0;
            auto* vm=create(player_resource,auxiliary[character],player_file,u32(-1),true,0x400);if(!vm){last_error=2;return false;}o.auxiliary=vm->id;}
    }else for(i32 i=0;i<4;++i){interrupt(options[i].auxiliary,1);options[i].auxiliary=0;}
    auto rebuild=[&](i32 base,i32 slots,i32 requested,i32& current,const ShtHeader& sht,const i32* table,AnmResource& resource,i32 script,u16 file){
        if(current==requested)return true;
        for(i32 i=0;i<requested;++i){auto& o=options[base+i];o.x=state.x;o.y=state.y;retire(o.animation);o.animation=0;o.index=i;
            const i32 index=table[requested-1]+i;
            if(index<0||index+21>=i32(sht.option_offsets.size())){last_error=11;return false;}
            const auto normal=sht.option_offsets[index],focus=sht.option_offsets[index+21];
            o.normal_x=truncate_int(normal.x*128.f);o.normal_y=truncate_int(normal.y*128.f);
            o.focus_x=truncate_int(focus.x*128.f);o.focus_y=truncate_int(focus.y*128.f);
            // Retail seasonal loop keeps esp+10 rooted at the main array.
            // It reads and writes main slot i, even while building seasonal i.
            auto& target=options[i];
            target.target_x=wrapping_add(state.x,state.focused?target.focus_x:target.normal_x);
            target.target_y=wrapping_add(state.y,state.focused?target.focus_y:target.normal_y);
            o.x=target.target_x;o.y=target.target_y;
            auto* vm=create(resource,script,file,14,false,0x40400);if(!vm){last_error=2;return false;}
            o.animation=vm->id;o.state=2;
        }
        for(i32 i=requested;i<slots;++i){options[base+i].state=0;interrupt(options[base+i].animation,1);}
        current=requested;for(i32 i=0;i<slots;++i)options[base+i].snap=1;return true;
    };
    return rebuild(0,4,count,state.option_count,main,formation,player_resource,scripts[character],player_file)
        &&rebuild(4,8,season_count,th16_season_count,season,season_formation,season_resource,season_scripts[subseason],30);
}
#endif
// Original option creation (432cc0). The two formation tables are indexed by
// power level; Marisa/A has eight levels, while the other five have four.
bool PlayerMotion::rebuild_options(const ShtHeader& sht,const GameEconomy& economy,i32 combination){
#if PORT_TITLE == 16
    // The TH16 caller must supply both families and their own resources.
    (void)sht;(void)economy;(void)combination;last_error=16;return false;
#endif
    if constexpr(kPortTitle==14){
        // Retail 44f970: both weapons share their character's option script.
        // All six formation index tables are identical (4d9d60 + slot*40).
        constexpr i32 offsets[8]={0,1,3,6,10,11,13,16};
        constexpr i32 scripts[3]={10,12,12},auxiliary[3]={11,14,13};
        if(combination<0||combination>=6){last_error=9;return false;}
#if PORT_TITLE==14
        const auto power=th14_option_power_words([&](OptionPowerAccessTh14 access){
            switch(access){
            case OptionPowerAccessTh14::Power:return th14_hud_load(economy.power);
            case OptionPowerAccessTh14::Step:return th14_hud_load(economy.power_step);
            default:return th14_hud_load(economy.max_power);
            }
        });
        if(!power.valid){last_error=9;return false;}
        const i32 count=power.count,character=combination/2;
#else
        if(economy.power_step<=0){last_error=9;return false;}
        const i32 count=economy.power/economy.power_step,character=combination/2;
#endif
        if(count<0||count>8){last_error=10;return false;}
        auto create=[&](i32 script,bool prepend){
            auto* a=animations.create(player_resource,script,player_file,14,false,prepend,nullptr,nullptr,nullptr,true);
            if(a)a->position.y=-32;
            return a;
        };
#if PORT_TITLE==14
        if(power.full){
#else
        if(economy.power>=economy.max_power){
#endif
            for(i32 i=0;i<count;++i){auto& o=options[i];erase(o.auxiliary);o.auxiliary=0;
                auto* a=create(auxiliary[character],true);if(!a){last_error=2;return false;}o.auxiliary=a->id;}
        }else for(auto& o:options)interrupt(o.auxiliary,1);
        auto& s=state;if(!s.option_count)s.option_angle=0;
        if(s.option_count==count)return true;
        for(i32 i=0;i<count;++i){
            auto& o=options[i];o.x=s.x;o.y=s.y;erase(o.animation);o.animation=0;o.index=i;
            const i32 index=offsets[count-1]+i;
            if(index+10>=i32(sht.option_offsets.size())){last_error=11;return false;}
            const auto normal=sht.option_offsets[index],focus=sht.option_offsets[index+10];
            // SSE mulss/cvttss2si; preserve the intermediate float rounding.
            o.normal_x=truncate_int(float(normal.x*128.f));o.normal_y=truncate_int(float(normal.y*128.f));
            o.focus_x=truncate_int(float(focus.x*128.f));o.focus_y=truncate_int(float(focus.y*128.f));
            o.behavior=OptionBehavior::None;
            o.target_x=wrapping_add(s.x,s.focused?o.focus_x:o.normal_x);o.target_y=wrapping_add(s.y,s.focused?o.focus_y:o.normal_y);
            o.x=o.target_x;o.y=o.target_y;
            auto* a=create(scripts[character],false);if(!a){last_error=2;return false;}o.animation=a->id;o.state=2;
        }
        for(i32 i=count;i<8;++i){options[i].state=0;interrupt(options[i].animation,1);}
        s.option_count=count;for(auto& o:options)o.snap=1;return true;
    }
    constexpr i32 ordinary_offsets[8]={0,1,3,6,10,11,13,16};
    constexpr i32 marisa_offsets[8]={0,1,3,6,10,15,21,28};
    constexpr i32 marisa_scripts[8][8]={{0},{2,1},{0,8,7},{2,1,4,3},{0,8,7,10,9},{2,1,4,3,6,5},{0,8,7,10,9,12,11},{2,1,4,3,6,5}};
    constexpr i32 auxiliary_scripts[6]={23,24,25,40,41,42};
    if(combination<0||combination>=6||economy.power_step<=0){last_error=9;return false;}
    const i32 count=economy.power/economy.power_step;
    if(count<0||count>8){last_error=10;return false;}
    if(economy.power>=economy.max_power){
        for(i32 i=0;i<count;++i){auto& o=options[i];erase(o.auxiliary);o.auxiliary=0;auto* a=animations.create(player_resource,auxiliary_scripts[combination],player_file,11,false,true);if(!a){last_error=2;return false;}o.auxiliary=a->id;}
    }else for(auto& o:options)interrupt(o.auxiliary,1);
    auto& s=state;if(!s.option_count)s.option_angle=normalize_angle(0);
    if(s.option_count==count)return true;
    for(i32 i=0;i<count;++i){
        auto& o=options[i];o.x=s.x;o.y=s.y;erase(o.animation);o.animation=0;o.index=i;
        const i32 index=(combination==3?marisa_offsets:ordinary_offsets)[count-1]+i;
        const i32 focus_base=combination==3?36:combination==4?0:10;
        if(index<0||index+focus_base>=i32(sht.option_offsets.size())){last_error=11;return false;}
        if(combination==2){o.behavior=OptionBehavior::Directional;o.polar_normal=sht.option_offsets[index];o.polar_focus=sht.option_offsets[index+10];}
        else {
            const auto normal=sht.option_offsets[index],focus=sht.option_offsets[index+focus_base];
            o.normal_x=truncate_int(double(normal.x)*128);o.normal_y=truncate_int(double(normal.y)*128);
            o.focus_x=truncate_int(double(focus.x)*128);o.focus_y=truncate_int(double(focus.y)*128);
        }
        i32 script=20+combination;
        switch(combination){
        case 0:o.behavior=OptionBehavior::Orbit;o.angle=normalize_angle(float(double(i)*6.2831854820251465/count-1.5707963705062866));break;
        case 3:script=21+marisa_scripts[count-1][i];break;
        case 4:
            script=34+s.weapon_mode;
            for(i32 mode=0;mode<5;++mode){const auto p=sht.option_offsets[index+mode*10];o.modes[mode]={truncate_int(double(p.x)*128),truncate_int(double(p.y)*128)};}break;
        case 5:script=39;break;
        default:break;
        }
        o.target_x=wrapping_add(s.x,s.focused?o.focus_x:o.normal_x);o.target_y=wrapping_add(s.y,s.focused?o.focus_y:o.normal_y);
        // Marisa/B starts at the player; its first movement frame snaps to target.
        if(combination!=4){o.x=o.target_x;o.y=o.target_y;}
        auto* a=animations.create(player_resource,script,player_file,11);if(!a){last_error=2;return false;}o.animation=a->id;o.state=2;
    }
    for(i32 i=count;i<8;++i){options[i].state=0;interrupt(options[i].animation,1);}
    s.option_count=count;for(auto& o:options)o.snap=1;return true;
}
bool PlayerMotion::update_warp(const PlayerMotionInput& in){
    auto& s=state;if(!s.warp)return true;
    auto reset=[&]{s.warp=0;s.warp_timer=0;};
    if(s.warp<99&&((in.held&9)||in.bomb||(in.enemy_manager&&!in.enemies)))reset();
    else switch(s.warp){
    case 1:if(!s.direction){s.warp=2;s.warp_timer=0;}else if(s.direction!=3)s.warp=0;break;
    case 3:if(!s.direction){s.warp=4;s.warp_timer=0;}else if(s.direction!=4)s.warp=0;break;
    case 2:case 4:
        if(s.warp_timer>8)reset();
        else if(s.direction==(s.warp==2?3:4)&&!in.bomb&&in.enemy_manager&&in.enemies){const bool left=s.warp==2;s.warp=left?99:100;s.warp_timer=0;if(!world.sound(left?47:48)||!world.rebuild_options()){last_error=4;return false;}}break;
    case 99:case 100:
        if((s.warp==99&&s.x<=-0x6800)||(s.warp==100&&s.x>=0x6800)){s.x=wrapping_add(s.x,s.warp==99?0xd000:-0xd000);for(auto& o:options)o.snap=1;}
        if(s.warp_timer>=45)reset();break;
    default:break;
    }
    s.warp_timer=wrapping_add(s.warp_timer,1);return true;
}
bool PlayerMotion::update_option(PlayerOption& o){
    auto& s=state;
    if(o.behavior==OptionBehavior::Orbit){const auto v=polar(o.angle,s.focused?24:64);o.target_x=wrapping_add(s.x,-truncate_int(double(v.x)*-128));o.target_y=wrapping_add(s.y,-truncate_int(double(v.y)*-128));o.angle=normalize_angle(float(double(o.angle)+0.10471975803375244));
        if(o.target_x<-0x6600){o.flags|=1;o.snap=1;o.target_x=wrapping_add(o.target_x,0xcc00);}
        else if(o.target_x>0x6600){o.flags|=1;o.snap=1;o.target_x=wrapping_add(o.target_x,-0xcc00);}
        else {if(o.flags&1)o.snap=1;o.flags&=~1u;}
    }else if(o.behavior==OptionBehavior::Directional){
        if(o.index==0&&!s.focused&&(std::abs(s.delta.x)>.001f||std::abs(s.delta.y)>.001f)){
            const float target=float(std::atan2(double(s.last_delta.y),double(s.last_delta.x))),d=normalize_angle(difference(target,s.option_angle));
            if(std::abs(d)<0.19634954631328583)s.option_angle=target;
            else s.option_angle=normalize_angle(float(double(normalize_angle(float(double(d)/(std::abs(d)>=0.7853981852531433?5:3))))+s.option_angle));
        }
        const auto p=s.focused?o.polar_focus:o.polar_normal;o.angle=normalize_angle(float(double(p.x)+s.option_angle));const auto v=polar(o.angle,p.y);o.target_x=wrapping_add(s.x,-truncate_int(double(v.x)*-128));o.target_y=wrapping_add(s.y,-truncate_int(double(v.y)*-128));
    }else if(o.behavior!=OptionBehavior::None){last_error=5;return false;}
    return true;
}
bool PlayerMotion::update(const PlayerMotionInput& in){
#if PORT_TITLE == 14
    return update_th14(in);
#endif
#if PORT_TITLE == 16
    return update_th16(in);
#endif
    auto& s=state;s.direction=direction(in.held);const i32 combination=i32(shot_file_slot(u32(in.character),u32(in.subtype)));
    if(!in.enemy_manager||!in.enemies||s.transition_timer<4){s.focused=0;s.option_lerp=30;}else s.focused=(in.held>>3)&1;
    i32 dx=0,dy=0;
    if(s.warp==99||s.warp==100)dx=s.warp==99?-150:150;
    else {
        if(!s.focused){interrupt(focus_animation,1);focus_animation=0;}
        else if(!focus_animation){
#if PORT_TITLE == 14
            // Retail 44d464 uses effect.anm script 26 on layer 14.
            focus_animation=world.create_focus();if(!focus_animation){last_error=2;return false;}
#else
            auto* a=animations.create(bullet_resource,74,bullet_file,11);if(!a){last_error=2;return false;}focus_animation=a->id;
#endif
        }
        const i32 speed=s.focused?s.focus_speed:s.normal_speed,diagonal=s.focused?s.focus_diagonal:s.normal_diagonal;
        switch(s.direction){case 1:dy=-speed;break;case 2:dy=speed;break;case 3:dx=-speed;break;case 4:dx=speed;break;case 5:dx=dy=-diagonal;break;case 6:dx=diagonal;dy=-diagonal;break;case 7:dx=-diagonal;dy=diagonal;break;case 8:dx=dy=diagonal;break;}
    }
    // Platform touch targets enter at the original movement callback; keyboard
    // and original replay paths retain the unmodified integer movement above.
    if(in.touch_mode&&s.warp<99&&std::isfinite(in.touch_x)&&std::isfinite(in.touch_y)){
        double x=double(in.touch_x)*128-s.x,y=double(in.touch_y)*128-s.y;
        if(in.touch_mode!=2){const double speed=s.focused?s.focus_speed:s.normal_speed,length=std::sqrt(x*x+y*y);if(length>speed&&length>0){x*=speed/length;y*=speed/length;}}
        dx=truncate_int(x);dy=truncate_int(y);s.direction=direction((dx<0?0x40:dx>0?0x80:0)|(dy<0?0x10:dy>0?0x20:0));
    }
#if PORT_TITLE == 14
    player_th14_bomb_displacement(dx,dy,s.th14_bomb_shift,s.th14_movement_factor);
#endif
    const i32 base=s.flags&2?26:0;
    if(dx<0&&s.previous_dx>=0){if(!change_body(base+1))return false;}
    else if(dx>0&&s.previous_dx<=0){if(!change_body(base+3))return false;}
    else if(!dx&&s.previous_dx<0){if(!change_body(base+2))return false;}
    else if(!dx&&s.previous_dx>0){if(!change_body(base+4))return false;}
    s.previous_dx=dx;s.previous_dy=dy;
    if(kPortTitle!=14&&combination==2){if(!(in.held&9)){s.fast_timer=wrapping_add(s.fast_timer,1);if(s.fast_timer>10){dx=wrapping_add(dx,dx);dy=wrapping_add(dy,dy);if(!afterimage())return false;}}else s.fast_timer=0;}
    s.delta.x=float(double(dx)*animations.rate);s.delta.y=float(double(dy)*animations.rate);if(s.direction)s.last_delta=s.delta;
    s.dx=truncate_int(s.delta.x);s.dy=truncate_int(s.delta.y);s.x=wrapping_add(s.x,s.dx);s.y=wrapping_add(s.y,s.dy);
    if(kPortTitle!=14&&combination==0&&!update_warp(in))return false;
    if(kPortTitle!=14&&combination==1){if(!s.direction&&!(in.held&9)){s.standing_timer=wrapping_add(s.standing_timer,1);if(s.standing_timer>10&&!world.attract_items()){last_error=6;return false;}}else s.standing_timer=0;}
    if(s.warp<99){if(s.x<-0x5c00){if(!s.warp&&!combination){s.warp=1;s.warp_timer=0;}s.x=-0x5c00;}else if(s.x>0x5c00){if(!s.warp&&!combination){s.warp=3;s.warp_timer=0;}s.x=0x5c00;}s.y=std::max(0x1000,std::min(0xd800,s.y));}
    s.position.x=float(double(s.x)/128);s.position.y=float(double(s.y)/128);
    if(animations.find(focus_animation))place(focus_animation,s.position);else focus_animation=0;
    if(kPortTitle!=14&&combination==4&&!in.bomb){
        if(in.pressed&0x400){s.weapon_mode=wrapping_add(s.weapon_mode,1)%5;for(i32 i=0;i<s.option_count;++i){if(i>=8){last_error=7;return false;}auto& o=options[i];erase(o.animation);o.animation=0;auto* a=animations.create(player_resource,s.weapon_mode+34,player_file,11);if(!a){last_error=2;return false;}o.animation=a->id;}}
        if(s.weapon_mode<0||s.weapon_mode>=5){last_error=8;return false;}for(auto& o:options){o.normal_x=o.focus_x=o.modes[s.weapon_mode][0];o.normal_y=o.focus_y=o.modes[s.weapon_mode][1];}
    }
#if PORT_TITLE == 16
    return update_options_th16(0,4)&&update_options_th16(4,8);
#endif
    if(s.flags&8)s.recall_timer=wrapping_add(s.recall_timer,1);
    for(auto& o:options){if(!o.state)continue;
        if(!(s.flags&8)){o.target_x=wrapping_add(s.x,s.focused?o.focus_x:o.normal_x);o.target_y=wrapping_add(s.y,s.focused?o.focus_y:o.normal_y);if(!update_option(o))return false;}
        else {o.target_x=s.x;o.target_y=s.y;if(s.recall_timer>=30){o.state=0;interrupt(o.animation,1);interrupt(o.auxiliary,1);s.option_count=0;continue;}}
        if(o.snap){o.snap=0;o.x=o.target_x;o.y=o.target_y;}
        else if(s.option_lerp>=30){const i32 x=signed_bits(u32(wrapping_add(o.target_x,-o.x))*u32(s.option_lerp))/100,y=signed_bits(u32(wrapping_add(o.target_y,-o.y))*u32(s.option_lerp))/100;if(!x&&!y){o.x=o.target_x;o.y=o.target_y;}else{o.x=wrapping_add(o.x,x);o.y=wrapping_add(o.y,y);}}
        const Vec3 p{float(double(o.x)/128),float(double(o.y)/128),0};place(o.animation,p);place(o.auxiliary,p);
    }return true;
}
void PlayerMotion::copy_shot_state(ShotPlayer& p)const noexcept {p.position=state.position;p.option_count=state.option_count;p.warp_state=state.warp;p.focused=state.focused;p.weapon_mode=state.weapon_mode;for(u32 i=0;i<p.options.size();++i){const auto& o=options[i];p.options[i]={o.x,o.y,o.angle,o.animation};}}
}
