#include "Hud.hpp"
#include "AnmVisualState.hpp"
namespace th17 {
bool Hud::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
AnmVm* Hud::find(u32& handle){auto* vm=animations.registry.find(handle);if(!vm)handle=0;return vm;}
bool Hud::root_interrupt(u32 handle,i32 label,bool immediate){auto* vm=animations.registry.find(handle);if(!vm)return true;if(!animations.interrupt_effect(*vm,label,animations.rate))return check(false,"HUD animation callback failed");vm->pending_interrupt=label;return !immediate||check(animations.tick_instance(*vm)>=0,"HUD animation update failed");}
bool Hud::visible(u32 handle,bool value){if(auto* vm=animations.registry.find(handle))return check(animations.registry.pause_tree(*vm,!value),"HUD animation visibility failed");return true;}
bool Hud::stock(std::array<u32,8>& handles,i32 count,i32 pieces,bool bomb){
    if(!handles[0])return true;if(count<0)count=0;if(count>(bomb?8:9))return check(false,"HUD stock outside TH17 range");
    static constexpr i32 life_labels[]={0,2,3},bomb_labels[]={0,2,4};if(count<8&&(pieces<0||pieces>=3))return check(false,"HUD piece count outside TH17 range");
    // Native practice allows nine lives. Its ninth life pointer aliases the
    // first bomb icon, whose interrupt is replaced by the following bomb HUD call.
    for(i32 i=0;i<count;i++)if(!root_interrupt(i<8?handles[i]:bomb_handles[0],2))return false;if(count>=8)return true;
    if(!root_interrupt(handles[count],7+(bomb?bomb_labels[pieces]:life_labels[pieces])))return false;
    for(i32 i=count+1;i<8;i++)if(!root_interrupt(handles[i],3))return false;return true;
}
bool Hud::initialize(const HudStartContext& context){
    if(!base){base=animations.create_overlay(resources.front,0);if(!check(base!=0,"HUD frame creation failed"))return false;}
    if(!life_handles[0]){
        for(i32 i=0;i<8;i++){life_handles[i]=animations.create_overlay(resources.front,30+i);if(!check(life_handles[i]!=0,"HUD life icon creation failed"))return false;}
        for(i32 i=0;i<8;i++){bomb_handles[i]=animations.create_overlay(resources.front,38+i);if(!check(bomb_handles[i]!=0,"HUD bomb icon creation failed"))return false;}
        for(i32 i=0;i<2;i++){clock_handles[i]=animations.create(resources.ascii,2+i,-1,0);auto* vm=find(clock_handles[i]);if(!check(vm!=nullptr,"HUD clock creation failed")||!visible(clock_handles[i],false))return false;vm->visual.render_flags&=~0xc0000u;}
    }
    if(!lives(session.extra_lives,session.life_pieces)||!bombs(session.bombs,session.bomb_pieces))return false;
    if(context.screen!=8&&!(context.mode_flags&0x40)&&(context.mode_flags&0x30)!=0x20&&!check(animations.create(resources.logo,1,-1,0)!=0,"Stage introduction creation failed"))return false;
    if((context.mode_flags&0x40)&&!check(animations.create(resources.front,122,-1,0)!=0,"Replay HUD creation failed"))return false;
    if(!indicator){indicator=animations.create(resources.front,112,-1,0);if(!check(indicator!=0,"Boss position indicator creation failed"))return false;}
    if(context.stage==1&&!context.practice_record&&!context.continues){const u32 h=animations.create(resources.front,69,-1,0);auto* vm=animations.registry.find(h);if(!check(vm!=nullptr,"Player introduction creation failed"))return false;vm->visual.translation={0,context.character==1?148.f:128.f,0};}
    if(context.game_active){difficulty_banner=animations.create(resources.front,81+context.difficulty,-1,0);if(!check(difficulty_banner!=0,"Difficulty banner creation failed")||!animations.interrupt(difficulty_banner,3))return false;}
    difficulty_label=animations.create(resources.front,87+context.difficulty,-1,0);loadout_label=animations.create(resources.front,113+context.character*3+context.animal,-1,0);
    if(!check(difficulty_label&&loadout_label,"HUD loadout creation failed"))return false;for(auto& gauge:gauges)gauge.active=0;phase_count=0;
    if(notice_handle){if(!animations.interrupt(notice_handle,1))return false;notice_handle=0;}return true;
}
bool Hud::prepare_spell(bool start){for(auto handle:clock_handles)if(!root_interrupt(handle,start?2:3,true))return false;return true;}
bool Hud::segment(i32 boss,i32 index,float fraction,u32 color){if(u32(boss)>=gauges.size()||u32(index)>=4)return check(false,"Boss health segment outside TH17 range");gauges[boss].marks[index]={fraction,color};return true;}
bool Hud::segments(i32 count){if(count<0||count>10)return check(false,"Boss phase count outside TH17 range");phase_count=count;return true;}
bool Hud::stage_logo(const HudStartContext& context){return context.screen==8||(context.mode_flags&0x40)||check(animations.create(resources.logo,0,-1,0)!=0,"Stage logo creation failed");}
bool Hud::ensure_boss_name(){if(find(boss_name))return true;const i32 script=resources.boss_names[enemies.current_chapter<41?1:0];if(script>=0){boss_name=animations.create(resources.front,script+167,-1,0);if(!check(boss_name!=0,"Boss name creation failed"))return false;}return true;}
bool Hud::retire_gauge(HudBossGauge& gauge){for(auto& h:gauge.handles)if(!animations.retire(h))return check(false,"Boss health animation retirement failed");gauge.active=0;return true;}
bool Hud::update_clock(float player_y){
    const auto* boss=enemies.boss_at(0);if(current_seconds<0||!boss||(enemies.manager_flags&1)||dialogue||(game_flags&0x10000)){
        for(auto h:clock_handles)if(!visible(h,false))return false;flags=(flags&~0x200u)|0x400;return true;
    }
    for(auto h:clock_handles)if(!visible(h,true))return false;
    const u32 mode=flags&0x600;const bool lower=spell.flags&0x100;
    if(!mode){if(lower?player_y>320:player_y<128){flags=(flags&~0x400u)|0x200;for(auto h:clock_handles)if(!root_interrupt(h,5))return false;}}
    else{
        if(mode==0x200){if(lower?player_y>288:player_y<160){for(auto h:clock_handles)if(!root_interrupt(h,4))return false;}else goto set_digits;}
        else {for(auto h:clock_handles)if(!root_interrupt(h,(spell.flags&1)?2:3,true))return false;for(auto h:clock_handles)if(!root_interrupt(h,4,true))return false;}
        flags&=~0x600u;
    }
set_digits:
    if(current_seconds<previous_seconds){i32 label=0,id=0;if(current_seconds<2){label=9;id=12;}else if(current_seconds<5){label=8;id=11;}if(label){for(auto h:clock_handles)if(!root_interrupt(h,label))return false;if(!sound||!sound(id))return check(false,"Boss countdown audio unavailable");}}
    else if(previous_seconds<current_seconds)for(auto h:clock_handles)if(!root_interrupt(h,7))return false;
    if(current_seconds!=previous_seconds)for(u32 i=0;i<2;i++)if(auto* vm=find(clock_handles[i])){if(!vm->select_sprite((i?current_seconds%10:current_seconds/10)+239))return check(false,"Boss countdown sprite unavailable");}
    previous_seconds=current_seconds;return true;
}
bool Hud::update_gauges(){
    if(enemies.manager_flags&1)return true;
    for(u32 index=0;index<2;index++){auto& gauge=gauges[index];auto* enemy=enemies.boss_at(index);
        if(!enemy){gauge.displayed=0;for(auto& mark:gauge.marks)mark.fraction=0;if(gauge.active&&!retire_gauge(gauge))return false;if(index==0&&!animations.retire(boss_name))return false;continue;}
        if(enemy->life>=100000||(enemy->flags&0x31)||enemy->collision_timer.current>0||dialogue){if(gauge.active&&!retire_gauge(gauge))return false;continue;}
        gauge.life=enemy->life;gauge.target=float(float(enemy->life)/float(enemy->initial_life));if(gauge.displayed<gauge.target)gauge.displayed=float(gauge.displayed+.025f);if(gauge.target<gauge.displayed)gauge.displayed=gauge.target;
        if(!gauge.active){for(u32 i=0;i<7;i++){gauge.handles[i]=animations.create(resources.front,i<4?391+i:394,-1,0);if(!check(gauge.handles[i]!=0,"Boss health ring creation failed"))return false;}gauge.active=1;}
        if(!ensure_boss_name())return false;
        const Vec3 p{float(enemy->motion.position.x*2.f),float(enemy->motion.position.y*2.f),enemy->motion.position.z};
        for(u32 i=0;i<3;i++){auto* vm=find(gauge.handles[i]);if(!check(vm!=nullptr,"Boss health ring unavailable"))return false;vm->visual.translation=p;if(!i){vm->visual.flags|=4;vm->variables.rotation.x=float(gauge.displayed*-6.2831855f);}}
        float z=0;for(u32 i=0;i<4;i++){auto* vm=find(gauge.handles[i+3]);if(!check(vm!=nullptr,"Boss health marker unavailable"))return false;const float value=gauge.marks[i].fraction;if(value==0||gauge.displayed<=value){if(!visible(gauge.handles[i+3],false))return false;continue;}if(!visible(gauge.handles[i+3],true))return false;
            const float angle=normalize_angle(float(-3.1415927f-float(value*6.2831855f)));vm->visual.flags|=4;vm->variables.rotation.z=angle;const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));z=float(z+p.z);vm->visual.translation={float(float(float(cosine*0.f)-float(sine*112.f))+p.x),float(float(float(sine*0.f)+float(cosine*112.f))+p.y),z};
        }
        const float x=float(enemy->motion.position.x-player.position.x),y=float(enemy->motion.position.y-player.position.y),distance=float(float(x*x)+float(y*y));const i32 label=gauge.faded?(distance>=9216?2:0):(distance<6400?3:0);if(label){for(auto h:gauge.handles)if(!animations.interrupt(h,label))return false;gauge.faded=label==3;}
    }return true;
}
bool Hud::update_indicator(){auto* boss=enemies.boss_at(0);auto* vm=find(indicator);if(!vm)return true;if(!boss||(boss->flags&0x21))return visible(indicator,false);if(!visible(indicator,true))return false;
    const i32 life=boss->phase_life;const bool active=spell.flags&1;u32 mode=flags&6; i32 label=0;
    if(mode==0&&life<(active?2000:700)){label=7;flags=(flags&~4u)|2;}else if(mode==2&&life<(active?1000:400)){label=8;flags=(flags&~2u)|4;}else if(mode==4&&life<(active?400:200)){label=9;flags|=6;}else if(mode==6&&life>(active?400:200)){label=10;flags&=~6u;}
    if(label&&!root_interrupt(indicator,label))return false;const float center=float(float(boss->motion.position.x+32.f)+192.f);vm->visual.translation={float(center+center),960,vm->visual.translation.z};const float distance=std::abs(float(boss->motion.position.x-player.position.x));const u32 alpha=distance<64?u32(truncate_int(float(float(distance*191.f)*.015625f))+64)&255:255;vm->visual.color=(vm->visual.color&0xffffffu)|(alpha<<24);if(boss->motion.position.x<-192||boss->motion.position.x>192)vm->visual.color&=0xffffffu;return true;
}
bool Hud::update(float rate){
    animations.rate=rate;if(!update_notices(rate))return false;if(result_active&&!find(result_panel))result_active=0;
    if(!update_clock(player.position.y)||!update_gauges())return false;
    for(u32 i=0;i<phase_handles.size();i++){auto& handle=phase_handles[i];if(i<u32(phase_count)){if(!handle){handle=animations.create(resources.front,70+i,-1,0);if(!check(handle!=0,"Boss phase marker creation failed"))return false;}}else if(handle){if(!animations.interrupt(handle,1))return false;handle=0;}}
    if(update_dialogue&&!update_dialogue())return check(false,"HUD dialogue update failed");if(!update_indicator())return false;age.tick(&rate);return true;
}
}
