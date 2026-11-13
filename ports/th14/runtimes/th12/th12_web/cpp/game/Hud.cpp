#include "Hud.hpp"
#include "SpellTiming.hpp"
#include "PortTitle.hpp"
#include "HudTh14Text.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace th11 {
void HudScore::update(const i32& current) noexcept {
#if PORT_TITLE==14
    th14_hud_score_update([&](HudScoreAccessTh14,HudScoreWordTh14 word)->i32{
        switch(word){
        case HudScoreWordTh14::Current:return th14_hud_load(current);
        case HudScoreWordTh14::Displayed:return displayed_value();
        case HudScoreWordTh14::Speed:return th14_hud_load(speed);
        case HudScoreWordTh14::High:return high_value();
        case HudScoreWordTh14::Continues:return continues_value();
        case HudScoreWordTh14::HighContinues:return th14_hud_load(high_continues);
        case HudScoreWordTh14::Flags:return signed_bits(th14_hud_load(flags));
        }
        return 0;
    },[&](HudScoreAccessTh14,HudScoreWordTh14 word,i32 value){
        switch(word){
        case HudScoreWordTh14::Displayed:th14_hud_store(displayed,value);break;
        case HudScoreWordTh14::Speed:th14_hud_store(speed,value);break;
        case HudScoreWordTh14::High:th14_hud_store(high,value);break;
        case HudScoreWordTh14::HighContinues:th14_hud_store(high_continues,value);break;
        case HudScoreWordTh14::Flags:th14_hud_store(flags,u32(value));break;
        default:break;
        }
    });
#else
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
#endif
}
Hud::Hud(AnmManager& a,AnmResource& f,AnmResource& t,GameEconomy& e,EnemyCommandEnvironment& n,StageCompletionState& c,HudEffects& fx)
    :animations(a),front(f),text(t),economy(e),enemies(n),completion(c),effects(fx){prepare_stage();}
Hud::~Hud(){for(auto& v:lives)animations.release_geometry(v);for(auto& v:digits)animations.release_geometry(v);for(auto& v:communication)animations.release_geometry(v);animations.release_geometry(indicator);
#if PORT_TITLE == 16
    for(auto handle:th16_icon_handles)if(auto* vm=animations.find(handle))animations.destroy(*vm);
#endif
#if PORT_TITLE == 14
    for(auto handle:icon_handles)if(auto* vm=animations.find(handle))animations.destroy(*vm);
    for(auto handle:digit_handles)if(auto* vm=animations.find(handle))animations.destroy(*vm);
#endif
}
bool Hud::bind(AnmVm& vm,AnmResource& resource,i32 script,u16 file){animations.release_geometry(vm);
#if PORT_TITLE == 14
    const bool bound=vm.copy_script(resource,script,file,&animations.rate);
#else
    const bool bound=vm.bind_script(resource,script,file,&animations.rate);
#endif
    return bound&&vm.update(animations)!=-2;
}
u32 Hud::create(AnmResource& resource,i32 script,u16 file){auto* vm=animations.create(resource,script,file,22);return vm?vm->id:0;}
void Hud::interrupt(u32 id,i16 label){auto* vm=animations.find(id);if(!vm)return;vm->interrupt(label);if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->interrupt(label);}
void Hud::prepare_stage(i32* current_seconds){
#if PORT_TITLE==14
    th14_prepare_hud_fields(elapsed,&animations.rate,score.displayed,current_seconds,previous_seconds,
        [this](){return th14_hud_load(economy.score_units);},
        [](HudPrepareWordTh14,auto& field,auto value){th14_hud_store(field,value);});
#else
    elapsed.set(0,&animations.rate);previous_seconds=-1;score.displayed=economy.score_units;
    if(current_seconds)*current_seconds=-1;
#endif
}
#if PORT_TITLE == 16
bool Hud::bind_icons_th16(){
    // 426df0/426e27 call full42c920: scripts30-45, managed stock VMs.
    const Vec3 origin{};const float rotation=0;
    for(u32 i=0;i<16;++i){
        if(auto* old=icon_th16(i))animations.destroy(*old);
        auto* vm=animations.create(front,i32(30+i),5,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation,0x400);
        th16_icon_handles[i]=vm?vm->id:0;if(!vm)return false;
        vm->interrupt(4);vm->flags2&=~0xc000u;
    }return true;
}
void Hud::display_bombs(i32 count,i32 fragments){
    // Complete42c390 within the eight gameplay stock slots.
    if(!icon_th16(8))return;count=std::clamp(count,0,8);i32 i=0;
    for(;i<count;++i)icon_th16(u32(i+8))->interrupt(2);
    if(i==8)return;icon_th16(u32(i+8))->interrupt(i16(fragments)+7);++i;
    for(;i<8;++i)icon_th16(u32(i+8))->interrupt(3);
}
bool Hud::update_season_th16(){
    auto* root=animations.find(th16_season_root);if(!root)return false;
    AnmVm* bar=nullptr;AnmVm* levels=nullptr;
    for(auto* n=root->child.next;n;n=n->next){if(n->value->script_index==115)bar=n->value;if(n->value->script_index==116)levels=n->value;}
    if(!bar||!levels)return false;
    struct Services final:Th16SeasonGaugeServices{
        AnmManager& manager;explicit Services(AnmManager& m):manager(m){}
        bool children(AnmVm& vm,bool visible)override{
            for(auto* n=vm.child.next;n;n=n->next){
                bool descendant=false;for(auto* parent=n->value->parent;parent;parent=parent->parent)if(parent==&vm){descendant=true;break;}
                if(descendant){if(visible)n->value->flags|=2;else n->value->flags&=~2u;}
            }return true;
        }
        bool interrupt(AnmVm& vm,i32)override{return vm.th16_callbacks[3]==0;}
        bool update(AnmVm& vm)override{return vm.update(manager)>=0;}
    } services(animations);
    return season_th16_gauge(economy.th16_season,th16_season_active,*bar,*levels,services);
}
#endif
bool Hud::start_stage(AnmResource& logo,const HudInput& input,bool demo,bool initial,i32 control_mode,i32 continues){
#if PORT_TITLE == 14
    const Vec3 origin{};const float rotation=0;
    auto create_at_origin=[&](i32 script){auto* vm=animations.create(front,script,5,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation);return vm?vm->id:0;};
    // Retail42ed3a..42edf6 retains the first sidebar root in HUD+120.
    if(!frame_animation&&!(frame_animation=create_at_origin(0)))return false;
#else
    if(!frame_animation&&!(frame_animation=create(front,0,5)))return false;
#endif
#if PORT_TITLE == 16
    if(!th16_season_root){
        // 4275b1 binds front113, flags2|400, raw entity origin and rotation0.
        const Vec3 origin{};const float rotation=0;
        auto* root=animations.create(front,113,5,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation,0x400);
        if(!root)return false;th16_season_root=root->id;
        if(!update_season_th16())return false;
    }
#endif
#if PORT_TITLE == 14
    if(!icon_th14(0)){
        if(!bind_icons_th14())return false;
#elif PORT_TITLE == 16
    if(!icon_th16(0)){
        if(!bind_icons_th16())return false;
#else
    if(!(lives[0].flags&1)){
        for(i32 i=0;i<9;++i)if(!bind(lives[i],front,10+i,5))return false;
        for(i32 i=0;i<4;++i)if(!bind(communication[i],front,32+i,5))return false;
#endif
        for(i32 i=0;i<2;++i){
#if PORT_TITLE == 14
            // Retail42efe0..42f09f registers the two ASCII digit roots.
            auto* vm=animations.create(text,2+i,2,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation);
            digit_handles[i]=vm?vm->id:0;if(!vm)return false;vm->flags&=~2u;vm->flags2&=~0x60000u;
#elif PORT_TITLE == 16
            // 42efb5..42f09f: the two timer VMs use ASCII 2/3. They begin
            // invisible with origin bits cleared; 41c130 interrupts them at spell begin.
            auto& vm=digits[i];animations.release_geometry(vm);vm.position={};
            if(!vm.copy_script(text,2+i,2,&animations.rate))return false;
            vm.flags2|=0x200;if(vm.update(animations)<0)return false;
            vm.flags&=~2u;vm.flags2&=~0x60000u;
#else
            if(!bind(digits[i],text,3+i,2))return false;
#endif
        }
    }
    display_lives(economy.lives,economy.life_fragments);
#if PORT_TITLE == 14
    display_bombs(economy.th14_bombs,economy.th14_bomb_fragments);
#elif PORT_TITLE == 16
    display_bombs(economy.th16_bombs,economy.th16_bomb_fragments);
#endif
#if PORT_TITLE == 14
    // Retail42f0cf..42f19e: stage logo script1 has its own resource.
    if(enemies.th14_runtime_mode!=8&&!demo&&!(enemies.th14_run_flags&0x40)&&(enemies.th14_run_flags&0x30)!=0x20){
        if(!animations.create(logo,1,6,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation))return false;
    }
    // Retail42f19e..42f430: the attract marker is94; a separate overlay111
    // is retained in HUD+a0. First-stage collection hint67 excludes replays
    // and continues and moves lower for Marisa after its initial update.
    if(demo&&!create_at_origin(94))return false;
    if(!th14_overlay&&!(th14_overlay=create_at_origin(111)))return false;
    if(input.stage==1&&!control_mode&&!continues){
        const u32 hint=create_at_origin(67);if(!hint)return false;
        if(auto* vm=animations.find(hint))vm->position={0,input.character==1?148.f:128.f,0};
    }
#else
    if(demo){if(!create(front,70,5))return false;}
#endif
    else if constexpr(kPortTitle!=14){if(!create(logo,0,27)||!create(logo,1,27))return false;}
    if constexpr(kPortTitle!=14)if(!bind(indicator,text,0,2))return false;
    if constexpr(kPortTitle!=14)if(input.stage==1&&!control_mode&&!continues&&!create(front,44,5))return false;
#if PORT_TITLE == 14
    // Retail42f430..42f68b. The old57/62 bands are spell-result notices
    // in this title; using them leaves a false capture/stage-clear message.
    if(initial){difficulty_intro=create_at_origin(79+economy.difficulty);if(!difficulty_intro)return false;interrupt(difficulty_intro,3);}
    difficulty_label=create_at_origin(85+economy.difficulty);if(!difficulty_label)return false;
#else
    if(initial){difficulty_intro=create(front,57+economy.difficulty,5);if(!difficulty_intro)return false;interrupt(difficulty_intro,3);}
    difficulty_label=create(front,62+economy.difficulty,5);if(!difficulty_label)return false;
#endif
    // Original interrupts the intro handle again, not the newly created label.
    interrupt(difficulty_intro,3);enemies.remaining_phases=0;
#if PORT_TITLE == 14
    // Retail42f695..42f6b3 resets active bits before the next stage.
    for(auto& row:th14_boss)row.active=false;
#endif
    return true;
}
void Hud::display_lives(i32 count,i32 fragments){
#if PORT_TITLE == 16
    // Complete42c280: unsigned five-fold fragment numerator / next extend.
    {
    if(!icon_th16(0))return;count=std::clamp(count,0,8);i32 i=0;
    for(;i<count;++i)icon_th16(u32(i))->interrupt(2);
    if(i==8)return;
    constexpr u32 normal[]={500000,1000000,2000000,4000000,7000000,10000000,15000000,25000000,50000000,100000000,1000000000};
    constexpr u32 extra[]={1000000,2000000,4000000,6000000,8000000,10000000,1000000000};
    const bool ex=economy.difficulty==4;const u32 size=ex?7:11;
    if(u32(economy.th16_extend_index)>=size)return;
    const u32 threshold=(ex?extra:normal)[economy.th16_extend_index];
    icon_th16(u32(i++))->interrupt(i16(u32(fragments)*5/threshold)+7);
    for(;i<8;++i)icon_th16(u32(i))->interrupt(3);return;}
#endif
#if PORT_TITLE == 14
    {if(!icon_th14(0))return;count=std::clamp(count,0,8);i32 i=0;
    auto signal=[&](i32 index,i32 label){if(auto* vm=icon_th14(u32(index)))vm->interrupt(label);};
    for(;i<count;++i)signal(i,2);
    if(i==8)return;
    signal(i++,i32(i16(signed_bits(u32(fragments)*5)/3))+7);
    for(;i<8;++i)signal(i,3);return;}
#endif
    // Only 0..8 are valid gameplay values; cap defensively before touching VMs.
    count=std::clamp(count,0,9);i32 i=0;
    for(;i<count;++i){lives[i].flags|=2;lives[i].interrupt(2);}
    if(i==9)return;
    lives[i].flags|=2;lives[i++].interrupt(i16(fragments+7));
    for(;i<9;++i)lives[i].flags&=~2u;
}
#if PORT_TITLE == 14
bool Hud::bind_icons_th14(){
    const Vec3 position{};const float rotation=0;
    for(u32 i=0;i<16;++i){
        if(auto* old=icon_th14(i))animations.destroy(*old);
        auto* vm=animations.create(front,i32(29+i),5,0,false,false,&position,nullptr,nullptr,false,&rotation);
        icon_handles[i]=vm?vm->id:0;if(!vm)return false;
    }return true;
}
void Hud::display_bombs(i32 count,i32 fragments){
    // 434fd0 returns immediately until the native icon array is bound.
    if(!icon_th14(8))return;count=std::clamp(count,0,8);i32 i=0;
    auto signal=[&](i32 index,i32 label){if(auto* vm=icon_th14(u32(index+8)))vm->interrupt(label);};
    for(;i<count;++i)signal(i,2);
    if(i==8)return;
    const i32 numerator=signed_bits(u32(fragments)*6);signal(i++,i32(i16(numerator/8))+7);
    for(;i<8;++i)signal(i,3);
}
#endif
bool Hud::boss_visible(const HudInput& input)const{
#if PORT_TITLE == 14
    if(!enemies.th14_owner())return false;
#endif
    return enemies.bosses[0]&&!(enemies.manager_flags&1)&&!input.dialogue;
}
bool Hud::notice(i32 type,i32 value){
#if PORT_TITLE == 16
    if(type==2||type==4){
        if(auto* old=animations.find(item_notice))animations.destroy(*old);item_notice=0;
        const Vec3 origin{};const float rotation=0;
        auto* vm=animations.create(front,61+type,5,u32(-1),false,false,&origin,nullptr,nullptr,false,&rotation,0x400);
        item_notice=vm?vm->id:0;return vm!=nullptr;
    }
#endif
#if PORT_TITLE == 14
    return notice_th14(type,value);
#endif
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
#if PORT_TITLE != 14 && PORT_TITLE != 16
    for(auto& vm:lives)if(vm.update(animations)==-2)return false;
#endif
#if PORT_TITLE != 14
    for(auto& vm:digits)if(vm.update(animations)==-2)return false;
#endif
    auto& flags=completion.hud_flags;
#if PORT_TITLE != 14 && PORT_TITLE != 16
    if(input.player_present){
        const auto p=input.player_position;
        if(!(flags&1)&&p.y>416&&p.x<-64){for(auto& vm:communication)vm.interrupt(3);flags|=1;}
        else if((flags&1)&&(p.y<400||p.x>-64)){for(auto& vm:communication)vm.interrupt(2);flags&=~1u;}
    }
    for(auto& vm:communication)if(vm.update(animations)==-2)return false;
    const i32 percentage=signed_bits(u32(economy.communication)*100)/10000;
    if(elapsed.current>=20){
        const u32 mode=percentage<100?0:percentage<110?0x40:percentage<120?0x80:0xc0;
        if((flags&0x1c0)!=mode){communication[2].interrupt(i16(7+(mode>>6)));communication[1].interrupt(i16(7+(mode>>6)));}
        flags=(flags&~0x1c0u)|mode;
    }
    const i32 blocks=(economy.communication>=10000?100:percentage)/10;
    auto& bar=communication[1];
    bar.uv[1].x=bar.uv[3].x=float(double(bar.uv[0].x)+.017578125+double(blocks)*4*.001953125);
    bar.flags|=8;bar.sprite_size.x=float(double(blocks)*4+9);
    communication[2].flags|=8;communication[2].sprite_size.x=0;
#endif
#if PORT_TITLE == 14
    if(!timer_th14(input)||!bosses_th14(input)||!remaining_th14())return false;
#else
    if(boss_visible(input)){
        if(input.seconds>=0){
            i16 label=0;i32 sound=-1;
            if(input.seconds<previous_seconds){if(input.seconds<5){label=9;sound=36;}else if(input.seconds<10){label=8;sound=27;}}
            else if(input.seconds>previous_seconds)label=7;
            if(label)for(auto& vm:digits)vm.interrupt(label);
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
#endif
    return true;
}
bool Hud::finish_update(const HudInput& input){
#if PORT_TITLE == 14
    if(!indicator_th14(input))return false;th14_hud_elapsed_tick(elapsed);return true;
#else
    const auto* boss=enemies.bosses[0];
    if(boss&&!(boss->flags&0x21)){
        auto& flags=completion.hud_flags;const u32 mode=(flags>>1)&3;
        const i32 first=input.spell?2000:700,second=input.spell?1000:400,third=input.spell?400:200;
        const i32 value=boss->interrupt_health;
        if(mode==0&&value<first){indicator.interrupt(7);flags=(flags&~6u)|2;}
        else if(mode==1&&value<second){indicator.interrupt(8);flags=(flags&~6u)|4;}
        else if(mode==2&&value<third){indicator.interrupt(9);flags|=6;}
        else if(mode==3&&value>third){indicator.interrupt(10);flags&=~6u;}
        if(indicator.update(animations)==-2)return false;
        indicator.position.x=float(double(boss->current.position.x)+224);indicator.position.y=480;
        const float distance=std::abs(float(double(boss->current.position.x)-input.player_position.x));
        u32 alpha=distance<64?u8(64-truncate_int(double(distance)*-191*.015625)):255;
        if(boss->current.position.x<-192||boss->current.position.x>192)alpha=0;
        indicator.color=(indicator.color&0xffffff)|(alpha<<24);
    }
    elapsed.tick();return true;
#endif
}
bool Hud::draw_outer(AnmRenderer& renderer,const HudInput&){
#if PORT_TITLE != 14 && PORT_TITLE != 16
    for(auto& vm:lives)if(renderer.draw(vm)==-2)return false;
#endif
    if constexpr(kPortTitle==14)return true;
    auto* boss=enemies.bosses[0];return !boss||(boss->flags&0x21)||renderer.draw(indicator)!=-2;
}
bool Hud::draw_inner(AnmRenderer& renderer,const HudInput& input){
    if(kPortTitle!=14&&displayed_health>0){
        const float right=float(double(displayed_health)*330+41);
        renderer.solid_rectangle(41,23,right,25,0xff000000);
        renderer.solid_rectangle(40,22,float(double(right)-1),24,0xffffffff);
        for(const auto& segment:enemies.health_segments)if(segment.fraction!=0)
            renderer.solid_rectangle(40,22,float(double(std::min(displayed_health,segment.fraction))*330+40),24,u32(segment.type));
    }
    if constexpr(kPortTitle!=14&&kPortTitle!=16)for(auto& vm:communication)if(renderer.draw(vm)==-2)return false;
    if constexpr(kPortTitle!=14)if(boss_visible(input)&&input.seconds>=0)for(auto& vm:digits)if(renderer.draw(vm)==-2)return false;
    return true;
}
bool Hud::queue_text(AsciiText& output,const HudInput& input){
    // TH13 HUD 427d27 selects native font 4 for its 12-pixel digits.
    constexpr i32 digit_font=kPortTitle==13||kPortTitle==14?4:3;
    char buffer[128];AsciiStyle style;style.font=digit_font;
    auto put=[&](float x,float y){return output.add(buffer,{x,y,0},style);};
#if PORT_TITLE == 14
    Th14HudAwards awards;awards.bonus=completion.displayed_bonus;awards.frames=input.spell_frames;awards.encoded=input.spell_time;
    if(auto* result=animations.find(input.result_animation)){awards.result=true;awards.result_alpha=u8(result->color>>24);}
    if(show_spell_time){
        if(auto* vm=animations.find(spell_time_animation)){awards.time=true;awards.time_alpha=u8(vm->color>>24);}
        else show_spell_time=false;
    }
    if(!th14_hud_awards(output,awards))return false;
#else
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
            style.font=digit_font;style.pass=1;
            for(u32 line=0;line<2;++line){
                style.scale={1,1};style.color=(vm->color&0xff000000)|(line?0x808080:0xffffff);
                const i32 sec=line?elapsed.seconds:std::min(input.spell_frames/60,999),hund=line?elapsed.hundredths:(input.spell_frames%60)*100/60;
                std::snprintf(buffer,sizeof(buffer),"%3d.",sec);if(!put(222,line?208:192))return false;
                style.scale={.6f,.6f};std::snprintf(buffer,sizeof(buffer),"%.2ds",hund);if(!put(266,line?214:198))return false;
            }
        }
    }
#endif
#if PORT_TITLE == 14
    {const auto* first=icon_th14(0);const u8 alpha=first?u8(first->color>>24):0;
    if(input.practice){
        HudScore counters;counters.displayed=score.displayed_value();
        counters.high=std::max(input.practice_high,counters.displayed);counters.high_continues=0;
        counters.continues=score.continues_value();
        if(!th14_hud_counters(output,economy,counters,alpha))return false;
    }else if(!th14_hud_counters(output,economy,score,alpha))return false;}
#else
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
#endif
#if PORT_TITLE == 14
    if(!timer_text_th14(output,input))return false;
#else
    if(boss_visible(input)&&input.seconds>=0){
        style.font=digit_font;
        style.color=digits[0].color;
        if(!output.add(".",{394,16,0},style))return false;
        style.scale={.6f,.6f};std::snprintf(buffer,sizeof(buffer),"%.2d",input.hundredths);
        if(!put(402,22))return false;
    }
#endif
    return true;
}
}
