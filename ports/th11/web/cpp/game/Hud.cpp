#include "Hud.hpp"
#include "SpellTiming.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace th11 {
void HudScore::update(i32 current) noexcept {
    // 41ef00. Both extend tables start at 1,000,000,000 stored units;
    // add_score caps valid gameplay at 999,999,999, so neither is reachable.
    if(current!=displayed){
        const i32 difference=signed_bits(u32(current)-u32(displayed));
        i32 step=std::min(difference/32,0x8d55e);if(!step)step=1;
        speed=std::min(std::max(speed,step),difference);
        displayed=wrapping_add(displayed,speed);
        if(displayed>=current)speed=0;
    }
    if(high<displayed){high=displayed;high_continues=continues;flags|=4;}
}
Hud::Hud(AnmManager& a,AnmResource& f,AnmResource& t,GameEconomy& e,EnemyCommandEnvironment& n,StageCompletionState& c,HudEffects& fx)
    :animations(a),front(f),text(t),economy(e),enemies(n),completion(c),effects(fx){prepare_stage();}
Hud::~Hud(){for(auto& v:lives)animations.release_geometry(v);for(auto& v:digits)animations.release_geometry(v);for(auto& v:communication)animations.release_geometry(v);animations.release_geometry(indicator);}
bool Hud::bind(AnmVm& vm,AnmResource& resource,i32 script,u16 file){animations.release_geometry(vm);return vm.bind_script(resource,script,file,&animations.rate)&&vm.update(animations)!=-2;}
u32 Hud::create(AnmResource& resource,i32 script,u16 file){auto* vm=animations.create(resource,script,file,22);return vm?vm->id:0;}
void Hud::interrupt(u32 id,i16 label){auto* vm=animations.find(id);if(!vm)return;vm->pending_interrupt=label;if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->pending_interrupt=label;}
void Hud::prepare_stage(){elapsed.set(0,&animations.rate);previous_seconds=-1;score.displayed=economy.score_units;}
bool Hud::start_stage(AnmResource& logo,const HudInput& input,bool demo,bool initial,i32 control_mode,i32 continues){
    if(!frame_animation&&!(frame_animation=create(front,0,5)))return false;
    if(!(lives[0].flags&1)){
        for(i32 i=0;i<9;++i)if(!bind(lives[i],front,10+i,5))return false;
        for(i32 i=0;i<4;++i)if(!bind(communication[i],front,32+i,5))return false;
        for(i32 i=0;i<2;++i)if(!bind(digits[i],text,3+i,2))return false;
    }
    display_lives(economy.lives,economy.life_fragments);
    if(demo){if(!create(front,70,5))return false;}
    else if(!practice_skip_logo&&(!create(logo,0,27)||!create(logo,1,27)))return false;
    if(!bind(indicator,text,0,2))return false;
    if(input.stage==1&&!control_mode&&!continues&&!create(front,44,5))return false;
    if(initial){difficulty_intro=create(front,57+economy.difficulty,5);if(!difficulty_intro)return false;interrupt(difficulty_intro,3);}
    difficulty_label=create(front,62+economy.difficulty,5);if(!difficulty_label)return false;
    // Original interrupts the intro handle again, not the newly created label.
    interrupt(difficulty_intro,3);enemies.remaining_phases=0;return true;
}
void Hud::display_lives(i32 count,i32 fragments){
    // Only 0..8 are valid gameplay values; cap defensively before touching VMs.
    count=std::clamp(count,0,9);i32 i=0;
    for(;i<count;++i){lives[i].flags|=2;lives[i].pending_interrupt=2;}
    if(i==9)return;
    lives[i].flags|=2;lives[i++].pending_interrupt=i16(fragments+7);
    for(;i<9;++i)lives[i].flags&=~2u;
}
bool Hud::boss_visible(const HudInput& input)const{return enemies.bosses[0]&&!(enemies.manager_flags&1)&&!input.dialogue;}
bool Hud::notice(i32 type,i32 value){
    if(type<0||type>6)return false;
    if(type==5)return true;
    auto replace=[&](u32& id,AnmResource& resource,i32 script,u16 file){if(auto* old=animations.find(id))for(auto* node=&old->child;node;node=node->next)node->value->flags|=0x4000000;id=create(resource,script,file);return id!=0;};
    if(type>=2&&type<=4)return replace(item_notice,front,36+type,5);
    if(!replace(spell_notice,front,type==0?36:type==1?37:41,5))return false;
    if(type==6)return true;
    if(type==0){i32 divisor=10000000;bool significant=false;
        for(i32 i=0;i<8;++i){if(!replace(bonus_digits[i],text,5+i,2))return false;const i32 digit=value/divisor;value%=divisor;divisor/=10;significant=significant||digit!=0;
            auto* vm=animations.find(bonus_digits[i]);if(vm&&!vm->bind_sprite(243+digit))return false;
            if(vm)for(auto* node=&vm->child;node;node=node->next){if(significant)node->value->flags|=2;else node->value->flags&=~2u;}
        }
    }
    show_spell_time=true;spell_time_animation=create(front,71,5);return spell_time_animation!=0;
}
bool Hud::update(const HudInput& input){
    for(auto& vm:lives)if(vm.update(animations)==-2)return false;
    for(auto& vm:digits)if(vm.update(animations)==-2)return false;
    auto& flags=completion.hud_flags;
    if(input.player_present){
        const auto p=input.player_position;
        if(!(flags&1)&&p.y>416&&p.x<-64){for(auto& vm:communication)vm.pending_interrupt=3;flags|=1;}
        else if((flags&1)&&(p.y<400||p.x>-64)){for(auto& vm:communication)vm.pending_interrupt=2;flags&=~1u;}
    }
    for(auto& vm:communication)if(vm.update(animations)==-2)return false;
    const i32 percentage=signed_bits(u32(economy.communication)*100)/10000;
    if(elapsed.current>=20){
        const u32 mode=percentage<100?0:percentage<110?0x40:percentage<120?0x80:0xc0;
        if((flags&0x1c0)!=mode)communication[1].pending_interrupt=communication[2].pending_interrupt=i16(7+(mode>>6));
        flags=(flags&~0x1c0u)|mode;
    }
    const i32 blocks=(economy.communication>=10000?100:percentage)/10;
    auto& bar=communication[1];
    bar.uv[1].x=bar.uv[3].x=float(double(bar.uv[0].x)+.017578125+double(blocks)*4*.001953125);
    bar.flags|=8;bar.sprite_size.x=float(double(blocks)*4+9);
    communication[2].flags|=8;communication[2].sprite_size.x=0;
    if(boss_visible(input)){
        if(input.seconds>=0){
            i16 label=0;i32 sound=-1;
            if(input.seconds<previous_seconds){if(input.seconds<5){label=9;sound=36;}else if(input.seconds<10){label=8;sound=27;}}
            else if(input.seconds>previous_seconds)label=7;
            if(label)for(auto& vm:digits)vm.pending_interrupt=label;
            if(sound>=0&&!effects.hud_sound(sound))return false;
            if(input.seconds!=previous_seconds){if(digits[0].resource&&!digits[0].bind_sprite(243+input.seconds/10))return false;if(digits[1].resource&&!digits[1].bind_sprite(243+input.seconds%10))return false;}
            previous_seconds=input.seconds;
        }
        const auto& boss=*enemies.bosses[0];health=boss.health;
        target_health=float(double(health)/boss.max_health);
        if(displayed_health<target_health)displayed_health=float(double(displayed_health)+double(.025f));
        if(displayed_health>target_health)displayed_health=target_health;
        const auto p=input.player_position;
        const bool hide=!(flags&8)&&p.y<=64&&p.x<-64;
        const bool show=(flags&8)&&(p.y>80||p.x>0);
        if(hide||show){const i16 label=hide?3:2;for(i32 i=0;i<std::clamp(enemies.remaining_phases,0,10);++i)interrupt(stars[i],label);interrupt(boss_name,label);flags=hide?flags|8:flags&~8u;}
        if(!boss_name){i32 script=105;const bool late=input.section>=24;
            if(input.stage==1)script=late?106:105;
            else if(input.stage>=2&&input.stage<=5)script=late?105+input.stage:-1;
            else if(input.stage==6)script=late?111:110;
            else if(input.stage==7)script=late?113:112;
            if(script>=0&&!(boss_name=create(front,script,5)))return false;
        }
        for(i32 i=0;i<10;++i){if(i<enemies.remaining_phases){if(!stars[i]&&!(stars[i]=create(front,46+i,5)))return false;}else if(stars[i]){interrupt(stars[i],1);stars[i]=0;}}
    }else{
        if(boss_name){interrupt(boss_name,1);boss_name=0;}
        displayed_health=0;for(auto& segment:enemies.health_segments)segment.fraction=0;
    }
    return true;
}
bool Hud::finish_update(const HudInput& input){
    const auto* boss=enemies.bosses[0];
    if(boss&&!(boss->flags&0x21)){
        auto& flags=completion.hud_flags;const u32 mode=(flags>>1)&3;
        const i32 first=input.spell?2000:700,second=input.spell?1000:400,third=input.spell?400:200;
        const i32 value=boss->interrupt_health;
        if(mode==0&&value<first){indicator.pending_interrupt=7;flags=(flags&~6u)|2;}
        else if(mode==1&&value<second){indicator.pending_interrupt=8;flags=(flags&~6u)|4;}
        else if(mode==2&&value<third){indicator.pending_interrupt=9;flags|=6;}
        else if(mode==3&&value>third){indicator.pending_interrupt=10;flags&=~6u;}
        if(indicator.update(animations)==-2)return false;
        indicator.position.x=float(double(boss->current.position.x)+224);indicator.position.y=480;
        const float distance=std::abs(float(double(boss->current.position.x)-input.player_position.x));
        u32 alpha=distance<64?u8(64-truncate_int(double(distance)*-191*.015625)):255;
        if(boss->current.position.x<-192||boss->current.position.x>192)alpha=0;
        indicator.color=(indicator.color&0xffffff)|(alpha<<24);
    }
    elapsed.tick();return true;
}
bool Hud::draw_outer(AnmRenderer& renderer,const HudInput&){
    for(auto& vm:lives)if(renderer.draw(vm)==-2)return false;
    auto* boss=enemies.bosses[0];return !boss||(boss->flags&0x21)||renderer.draw(indicator)!=-2;
}
bool Hud::draw_inner(AnmRenderer& renderer,const HudInput& input){
    if(displayed_health>0){
        const float right=float(double(displayed_health)*330+41);
        renderer.solid_rectangle(41,23,right,25,0xff000000);
        renderer.solid_rectangle(40,22,float(double(right)-1),24,0xffffffff);
        for(const auto& segment:enemies.health_segments)if(segment.fraction!=0)
            renderer.solid_rectangle(40,22,float(double(std::min(displayed_health,segment.fraction))*330+40),24,u32(segment.type));
    }
    for(auto& vm:communication)if(renderer.draw(vm)==-2)return false;
    if(boss_visible(input)&&input.seconds>=0)for(auto& vm:digits)if(renderer.draw(vm)==-2)return false;
    return true;
}
bool Hud::queue_text(AsciiText& output,const HudInput& input){
    char buffer[128];AsciiStyle style;style.font=3;
    auto put=[&](float x,float y){return output.add(buffer,{x,y,0},style);};
    if(auto* result=animations.find(input.result_animation)){
        std::snprintf(buffer,sizeof(buffer),"%d",completion.displayed_bonus);
        style.pass=1;style.color=0xffffff|(result->color&0xff000000);
        if(!put(float(224-double(std::strlen(buffer))*.5*12),200))return false;
    }
    if(show_spell_time){
        auto* vm=animations.find(spell_time_animation);
        if(!vm)show_spell_time=false;
        else{
            const auto elapsed=decode_spell_time(input.spell_time);
            style.font=3;style.pass=1;
            for(u32 line=0;line<2;++line){
                style.scale={1,1};style.color=(vm->color&0xff000000)|(line?0x808080:0xffffff);
                const i32 sec=line?elapsed.seconds:std::min(input.spell_frames/60,999),hund=line?elapsed.hundredths:(input.spell_frames%60)*100/60;
                std::snprintf(buffer,sizeof(buffer),"%3d.",sec);if(!put(222,line?208:192))return false;
                style.scale={.6f,.6f};std::snprintf(buffer,sizeof(buffer),"%.2ds",hund);if(!put(266,line?214:198))return false;
            }
        }
    }
    style.scale={1,1};style.pass=0;style.color=0xffffff|(lives[0].color&0xff000000);
    const i32 high=input.practice?std::max(input.practice_high,score.displayed):score.high;
    std::snprintf(buffer,sizeof(buffer),high<100000000?" %.8d%d":"%.9d%d",high,input.practice?0:score.high_continues);
    if(!put(508,48))return false;
    std::snprintf(buffer,sizeof(buffer),score.displayed<100000000?" %.8d%d":"%.9d%d",score.displayed,score.continues);
    if(!put(508,72))return false;
    if(!economy.power_step)return false;
    std::snprintf(buffer,sizeof(buffer),"%d.",economy.power/economy.power_step);
    if(!put(520,128))return false;
    style.scale={.6f,.6f};
    std::snprintf(buffer,sizeof(buffer),"%.2d",signed_bits(u32(economy.power%economy.power_step)*100)/economy.power_step);
    if(!put(540,135))return false;
    style.scale={1,1};std::snprintf(buffer,sizeof(buffer),"/%d.",economy.max_power/economy.power_step);
    if(!put(554,128))return false;
    style.scale={.6f,.6f};if(!output.add("00",{586,135,0},style))return false;
    style.scale={1,1};std::snprintf(buffer,sizeof(buffer),"%d",economy.graze);
    if(!put(520,152))return false;
    style.font=2;style.pass=1;style.color=0xffffff|(communication[0].color&0xff000000);
    const i32 percentage=economy.communication<10000?signed_bits(u32(economy.communication)*100)/10000:100;
    const i32 factor=wrapping_add(percentage,std::min(economy.graze/100,899));
    const i32 points=economy.point_value/100;
    std::snprintf(buffer,sizeof(buffer),"%.6d*%1.2f",points-points%10,double(factor)/100);
    if(!put(48,455))return false;
    if(boss_visible(input)&&input.seconds>=0){
        style.font=3;style.color=digits[0].color;
        if(!output.add(".",{394,16,0},style))return false;
        style.scale={.6f,.6f};std::snprintf(buffer,sizeof(buffer),"%.2d",input.hundredths);
        if(!put(402,22))return false;
    }
    return true;
}
}
