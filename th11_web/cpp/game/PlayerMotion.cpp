#include "PlayerMotion.hpp"
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
void PlayerMotion::interrupt(u32 id,i16 label){family(animations.find(id),[&](AnmVm& a){a.pending_interrupt=label;});}
void PlayerMotion::erase(u32 id){family(animations.find(id),[](AnmVm& a){a.flags|=0x4000000;});}
void PlayerMotion::place(u32 id,Vec3 p){family(animations.find(id),[&](AnmVm& a){a.position=screen(p);});}
bool PlayerMotion::change_body(i32 script){if(!body.bind_script(player_resource,script,player_file,&animations.rate)||body.update(animations)<0){last_error=1;return false;}return true;}
bool PlayerMotion::afterimage(){auto* a=animations.create(player_resource,19,player_file,11);if(!a){last_error=2;return false;}if(a->resource&&!a->bind_sprite(body.sprite_index)){last_error=3;return false;}place(a->id,state.position);return true;}
void PlayerMotion::set_speeds(const ShtHeader& s)noexcept{state.normal_speed=truncate_int(double(s.speed)*128);state.focus_speed=truncate_int(double(s.focus_speed)*128);state.normal_diagonal=truncate_int(double(s.diagonal_speed)*128);state.focus_diagonal=truncate_int(double(s.focus_diagonal_speed)*128);}
// Original option creation (432cc0). The two formation tables are indexed by
// power level; Marisa/A has eight levels, while the other five have four.
bool PlayerMotion::rebuild_options(const ShtHeader& sht,const GameEconomy& economy,i32 combination){
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
    auto& s=state;
    // C is an explicit touch-accessible shortcut for the original second tap
    // at a horizontal boundary. Native enemy/Bomb checks and warp progression
    // below still own the action. Persistent mobile Shot/Focus must not block
    // this dedicated shortcut; ordinary double-tap input remains unchanged.
    const bool function_warp=(in.pressed&4)&&s.warp<99&&in.enemy_manager&&in.enemies&&!in.bomb&&
        (s.x<=-0x5c00||s.x>=0x5c00);
    if(function_warp){const bool left=s.x<=-0x5c00;s.warp=left?2:4;s.warp_timer=0;s.direction=left?3:4;}
    if(!s.warp)return true;
    auto reset=[&]{s.warp=0;s.warp_timer=0;};
    if(s.warp<99&&((!function_warp&&(in.held&9))||in.bomb||(in.enemy_manager&&!in.enemies)))reset();
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
    auto& s=state;s.direction=direction(in.held);const i32 combination=in.character*3+in.subtype;
    if(!in.enemy_manager||!in.enemies||s.transition_timer<4){s.focused=0;s.option_lerp=30;}else s.focused=(in.held>>3)&1;
    i32 dx=0,dy=0;
    if(s.warp==99||s.warp==100)dx=s.warp==99?-150:150;
    else {
        if(!s.focused){interrupt(focus_animation,1);focus_animation=0;}
        else if(!focus_animation){auto* a=animations.create(bullet_resource,74,bullet_file,11);if(!a){last_error=2;return false;}focus_animation=a->id;}
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
    const i32 base=s.flags&2?26:0;
    if(dx<0&&s.previous_dx>=0){if(!change_body(base+1))return false;}
    else if(dx>0&&s.previous_dx<=0){if(!change_body(base+3))return false;}
    else if(!dx&&s.previous_dx<0){if(!change_body(base+2))return false;}
    else if(!dx&&s.previous_dx>0){if(!change_body(base+4))return false;}
    s.previous_dx=dx;s.previous_dy=dy;
    if(combination==2){if(!(in.held&9)){s.fast_timer=wrapping_add(s.fast_timer,1);if(s.fast_timer>10){dx=wrapping_add(dx,dx);dy=wrapping_add(dy,dy);if(!afterimage())return false;}}else s.fast_timer=0;}
    s.delta.x=float(double(dx)*animations.rate);s.delta.y=float(double(dy)*animations.rate);if(s.direction)s.last_delta=s.delta;
    s.dx=truncate_int(s.delta.x);s.dy=truncate_int(s.delta.y);s.x=wrapping_add(s.x,s.dx);s.y=wrapping_add(s.y,s.dy);
    if(combination==0&&!update_warp(in))return false;
    if(combination==1){if(!s.direction&&!(in.held&9)){s.standing_timer=wrapping_add(s.standing_timer,1);if(s.standing_timer>10&&!world.attract_items()){last_error=6;return false;}}else s.standing_timer=0;}
    if(s.warp<99){if(s.x<-0x5c00){if(!s.warp&&!combination){s.warp=1;s.warp_timer=0;}s.x=-0x5c00;}else if(s.x>0x5c00){if(!s.warp&&!combination){s.warp=3;s.warp_timer=0;}s.x=0x5c00;}s.y=std::max(0x1000,std::min(0xd800,s.y));}
    s.position.x=float(double(s.x)/128);s.position.y=float(double(s.y)/128);
    if(animations.find(focus_animation))place(focus_animation,s.position);else focus_animation=0;
    if(combination==4&&!in.bomb){
        if(in.pressed&(0x400|4)){s.weapon_mode=wrapping_add(s.weapon_mode,1)%5;for(i32 i=0;i<s.option_count;++i){if(i>=8){last_error=7;return false;}auto& o=options[i];erase(o.animation);o.animation=0;auto* a=animations.create(player_resource,s.weapon_mode+34,player_file,11);if(!a){last_error=2;return false;}o.animation=a->id;}}
        if(s.weapon_mode<0||s.weapon_mode>=5){last_error=8;return false;}for(auto& o:options){o.normal_x=o.focus_x=o.modes[s.weapon_mode][0];o.normal_y=o.focus_y=o.modes[s.weapon_mode][1];}
    }
    if(s.flags&8)s.recall_timer=wrapping_add(s.recall_timer,1);
    for(auto& o:options){if(!o.state)continue;
        if(!(s.flags&8)){o.target_x=wrapping_add(s.x,s.focused?o.focus_x:o.normal_x);o.target_y=wrapping_add(s.y,s.focused?o.focus_y:o.normal_y);if(!update_option(o))return false;}
        else {o.target_x=s.x;o.target_y=s.y;if(s.recall_timer>=30){o.state=0;interrupt(o.animation,1);interrupt(o.auxiliary,1);s.option_count=0;continue;}}
        if(o.snap){o.snap=0;o.x=o.target_x;o.y=o.target_y;}
        else if(s.option_lerp>=30){const i32 x=signed_bits(u32(wrapping_add(o.target_x,-o.x))*u32(s.option_lerp))/100,y=signed_bits(u32(wrapping_add(o.target_y,-o.y))*u32(s.option_lerp))/100;if(!x&&!y){o.x=o.target_x;o.y=o.target_y;}else{o.x=wrapping_add(o.x,x);o.y=wrapping_add(o.y,y);}}
        const Vec3 p{float(double(o.x)/128),float(double(o.y)/128),0};place(o.animation,p);place(o.auxiliary,p);
    }return true;
}
void PlayerMotion::copy_shot_state(ShotPlayer& p)const noexcept {p.position=state.position;p.option_count=state.option_count;p.warp_state=state.warp;p.focused=state.focused;p.weapon_mode=state.weapon_mode;for(u32 i=0;i<8;++i){const auto& o=options[i];p.options[i]={o.x,o.y,o.angle,o.animation};}}
}
