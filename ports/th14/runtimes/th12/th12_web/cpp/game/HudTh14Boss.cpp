#include "Hud.hpp"
#include <cmath>
#include <cstdio>
#if PORT_TITLE == 14
namespace th11 {
bool Hud::timer_text_th14(AsciiText& output,const HudInput& input){
    // Retail4318b6..431a29 follows the animated first countdown digit.
    if(!boss_visible(input)||input.seconds<0)return true;
    const auto* digit=digit_th14(0);if(!digit)return false;
    AsciiStyle style;style.font=4;style.pass=2;style.color=digit->color;
    const float x=float(digit->script_position.x+16.f),y=float(digit->script_position.y-7.f);
    if(!output.add(".",{x,y,0},style))return false;
    style.scale={.6f,.6f};char text[32];std::snprintf(text,sizeof text,"%.2d",input.hundredths);
    return output.add(text,{float(x+8.f),float(y+6.f),0},style);
}
bool Hud::boss_name_th14(const HudInput& input){
    // Full retail4350b0; stage descriptors at4d5988+stage*cc,+5c/+80.
    if(animations.find(boss_name))return true;boss_name=0;
    if(input.stage<1||input.stage>7)return false;
    if(input.stage==4&&input.section<5)return true;
    constexpr i32 names[7][2]={{1,0},{2,-1},{3,-1},{4,5},{6,-1},{7,6},{9,8}};
    const bool alternate=input.section<5||(input.stage==4&&((input.spell_id>=52&&input.spell_id<=63)||(input.spell_id<0&&input.subtype==1)));
    const i32 name=names[input.stage-1][alternate?1:0];if(name<0)return true;
    const Vec3 origin{};const float rotation=0;
    auto* vm=animations.create(front,152+name,5,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation);
    boss_name=vm?vm->id:0;return vm!=nullptr;
}
bool Hud::remaining_th14(){
    // Retail430c55..430d6b runs independently of boss presence/dialogue.
    const Vec3 origin{};const float rotation=0;
    for(i32 i=0;i<10;++i){
        if(i<enemies.remaining_phases){if(!stars[i]){auto* vm=animations.create(front,68+i,5,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation);if(!vm)return false;stars[i]=vm->id;}}
        else if(stars[i]){interrupt(stars[i],1);stars[i]=0;}
    }return true;
}
namespace {
void retire(AnmVm* vm){if(!vm)return;vm->flags2|=0x20;if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->flags2|=0x20;}
float angle(float value){constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;u32 loops=0;while(value>pi){value=float(value-tau);if(loops++>32)break;}while(value<-pi){value=float(value+tau);if(loops++>32)break;}return value;}
}
bool Hud::bosses_th14(const HudInput& input){
    // Retail43014e..430c55 has two boss rows, seven ring/marker VMs each.
    if(!enemies.th14_owner())return true;
    if(enemies.manager_flags&1)return true;
    for(u32 slot=0;slot<2;++slot){
        auto& row=th14_boss[slot];const auto* boss=enemies.bosses[slot];
        auto close=[&](){if(row.active){for(auto& id:row.handles){retire(animations.find(id));id=0;}row.active=false;}};
        if(!boss){row.displayed=0;for(auto& mark:enemies.th14_health_segments[slot])mark.fraction=0;close();if(!slot){retire(animations.find(boss_name));boss_name=0;}continue;}
        if(boss->health>=100000||(boss->flags&0x31)||boss->damage_immunity.current>0||input.dialogue){close();continue;}
        row.health=boss->health;row.target=float(float(boss->health)/float(boss->max_health));
        if(row.displayed<row.target)row.displayed=float(row.displayed+.025f);if(row.displayed>row.target)row.displayed=row.target;
        if(!row.active){const Vec3 origin{};const float rotation=0;for(u32 n=0;n<7;++n){auto* vm=animations.create(front,n<3?232+i32(n):235,5,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation);if(!vm)return false;row.handles[n]=vm->id;}row.active=true;}
        if(!boss_name_th14(input))return false;
        AnmVm* vms[7]{};for(u32 n=0;n<7;++n){vms[n]=animations.find(row.handles[n]);if(!vms[n]){row.handles[n]=0;return false;}}
        vms[0]->flags|=4;vms[0]->rotation.x=float(row.displayed*-6.2831854820251465f);
        const Vec3 position{float(boss->current.position.x*2.f),float(boss->current.position.y*2.f),boss->current.position.z};
        for(u32 n=0;n<3;++n)vms[n]->position=position;
        float z=0;
        for(u32 n=0;n<4;++n){auto& vm=*vms[n+3];const float fraction=enemies.th14_health_segments[slot][n].fraction;
            if(fraction!=0&&fraction<row.displayed){vm.flags|=6;vm.rotation.z=angle(float(-3.1415927410125732f-float(fraction*6.2831854820251465f)));
                const float c=float(std::cos(double(vm.rotation.z))),s=float(std::sin(double(vm.rotation.z)));
                const float x=float(float(float(c*0.f)-float(s*112.f))+position.x),y=float(float(float(c*112.f)+float(s*0.f))+position.y);
                z=float(position.z+z);vm.position={x,y,z};
            }else vm.flags&=~2u;
        }
        const float dx=float(boss->current.position.x-input.player_position.x),dy=float(boss->current.position.y-input.player_position.y),distance=float(float(dx*dx)+float(dy*dy));
        if(row.hidden?distance>=9216.f:distance<6400.f){for(auto id:row.handles)interrupt(id,row.hidden?2:3);row.hidden=!row.hidden;}
    }return true;
}
bool Hud::indicator_th14(const HudInput& input){
    // Retail430dfe..4310bd updates the retained front111 root, not ASCII0.
    if(!enemies.th14_owner())return true;
    const auto* boss=enemies.bosses[0];auto* vm=animations.find(th14_overlay);
    if(!vm){th14_overlay=0;return true;}
    if(!boss||(boss->flags&0x21)){vm->flags&=~2u;if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->flags&=~2u;return true;}
    vm->flags|=2;auto& flags=completion.hud_flags;const u32 mode=(flags>>1)&3;
    const i32 first=input.spell?2000:700,second=input.spell?1000:400,third=input.spell?400:200,value=boss->interrupt_health;
    if(mode==0&&value<first){vm->interrupt(7);flags=(flags&~6u)|2;}
    else if(mode==1&&value<second){vm->interrupt(8);flags=(flags&~6u)|4;}
    else if(mode==2&&value<third){vm->interrupt(9);flags|=6;}
    else if(mode==3&&value>third){vm->interrupt(10);flags&=~6u;}
    vm->position.x=float(float(float(boss->current.position.x+32.f)+192.f)*2.f);vm->position.y=960.f;
    const float distance=std::abs(float(boss->current.position.x-input.player_position.x));
    u32 alpha=distance<64.f?u8(truncate_int(float(float(distance*-191.f)*-.015625f))+64):255;
    if(boss->current.position.x<-192.f||boss->current.position.x>192.f)alpha=0;vm->color=(vm->color&0xffffff)|(alpha<<24);return true;
}
bool Hud::timer_th14(const HudInput& input){
    // Retail42fec5..43014e visibility, position hysteresis and warnings.
    auto* first=digit_th14(0);auto* second=digit_th14(1);if(!first||!second)return false;
    auto& flags=completion.hud_flags;
    if(!boss_visible(input)||input.seconds<0){first->flags&=~2u;second->flags&=~2u;flags=(flags&~0x200u)|0x400;return true;}
    first->flags|=2;second->flags|=2;const u32 mode=(flags>>9)&3;const bool flipped=(input.spell_flags&0x100)!=0;
    auto signal=[&](i32 label,bool update){for(auto* vm:{first,second}){vm->interrupt(label);if(update&&vm->update(animations)<0)return false;}return true;};
    if(mode==0){if((!flipped&&input.player_position.y<128.f)||(flipped&&input.player_position.y>320.f)){flags=(flags&~0x400u)|0x200;if(!signal(5,false))return false;}}
    else if(mode==1){if((!flipped&&input.player_position.y<160.f)||(flipped&&input.player_position.y>288.f)){if(!signal(4,false))return false;flags&=~0x600u;}}
    else {if(!signal(input.spell?2:3,true)||!signal(4,true))return false;flags&=~0x600u;}
    i32 label=0,sound=-1;if(input.seconds<th14_hud_load(previous_seconds)){if(input.seconds<5){label=9;sound=12;}else if(input.seconds<10){label=8;sound=11;}}
    else if(input.seconds>th14_hud_load(previous_seconds))label=7;
    if(label&&!signal(label,false))return false;if(sound>=0&&!effects.hud_sound(sound))return false;
    if(input.seconds!=th14_hud_load(previous_seconds)){if(first->resource&&!first->bind_sprite(239+input.seconds/10))return false;if(second->resource&&!second->bind_sprite(239+input.seconds%10))return false;}
    th14_hud_store(previous_seconds,input.seconds);return true;
}
}
#endif
