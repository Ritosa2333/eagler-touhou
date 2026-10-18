#include "GuiFrame.hpp"
#include "Dialogue.hpp"
namespace th10 {
// 0x4054b0. The decimal separator is its own animation and is not rebound.
void Gui::update_power(i32 whole,i32 fraction,GuiScoreEnvironment& env){env.bind_digit(*animations,power_digits[0],wrapping_add(whole,8));env.bind_digit(*animations,power_digits[2],wrapping_add(fraction/10,8));env.bind_digit(*animations,power_digits[3],wrapping_add(fraction%10,8));}
// 0x414900. GUI update order is independent of score animation, which runs
// earlier in the gameplay chain. Object lookups after dialogue may see a new
// boss or no boss at all.
i32 Gui::update(GuiFrameEnvironment& env){
    if(display_flags&0x10){ending_frames=wrapping_add(ending_frames,1);if(ending_frames>=120)*env.pending_screen=(*env.engine_flags&0x1000)?2:14;}
    for(auto& vm:life_icons)env.update_animation(vm);
    for(auto& vm:power_digits)env.update_animation(vm);
    for(auto& vm:countdown_digits)env.update_animation(vm);
    if(auto* player=*env.player){
        if(!(display_flags&1)){if(player->position.y>416&&player->position.x<-128){for(auto& vm:faith_digits)vm.pending_interrupt=3;display_flags|=1;}}
        else if(player->position.y<400||player->position.x>-112){for(auto& vm:faith_digits)vm.pending_interrupt=2;display_flags&=~1u;}
    }
    i32 value=env.game->item_value,divisor=10000;
    env.update_animation(faith_digits[0]);
    for(u32 i=1;i<6;++i){const i32 digit=value/divisor;value%=divisor;env.bind_digit(*animations,faith_digits[i],wrapping_add(digit,30));divisor/=10;env.update_animation(faith_digits[i]);}
    env.update_animation(faith_digits[6]);
    auto& registry=*env.registry;
    auto* boss=*env.enemies?(*env.enemies)->bosses[0]:nullptr;
    if(boss&&!dialogue){
        boss_health_points=boss->state.health;
        const auto ratio=Extended::from_int(boss_health_points)/Extended::from_int(boss->state.maximum_health);target_boss_health=ratio.to_float();
        if(number(displayed_boss_health)<ratio)displayed_boss_health=Scalar::add(displayed_boss_health,.025f);
        if(displayed_boss_health>target_boss_health)displayed_boss_health=target_boss_health;
        const auto& position=(*env.player)->position;
        i32 interrupt=0;
        if(!(display_flags&8)){if(position.y<=64&&position.x<-64)interrupt=3;}
        else if(position.y>=80||position.x>0)interrupt=2;
        if(interrupt){for(i32 i=0;i<boss_lives;++i)registry.interrupt(boss_life_icons[i],interrupt);registry.interrupt(boss_name,interrupt);if(interrupt==3)display_flags|=8;else display_flags&=~8u;}
        if(!boss_name){
            i32 script=133;
            switch(env.game->stage){case 1:if(env.game->section>=24)script=134;break;case 2:script=env.game->section<24?-1:135;break;case 3:script=env.game->section<24?-1:136;break;case 4:script=137+(env.game->section>=24);break;case 5:script=139;break;case 6:script=140;break;case 7:script=140+(env.game->section>=24);break;}
            if(script>=0)boss_name=env.create_animation(*animations,script);
        }
        for(i32 i=0;i<10;++i){auto& id=boss_life_icons[i];if(i<boss_lives){if(!id)id=env.create_animation(*animations,i+91);}else{registry.interrupt(id,1);id=0;}}
    }else{
        registry.interrupt(boss_name,1);boss_name=0;displayed_boss_health=0;
        for(auto& bar:boss_health)bar.amount=0;
    }
    if(dialogue&&env.update_dialogue(*dialogue)){if(dialogue){dialogue->release(registry);env.release_dialogue(dialogue);}dialogue=nullptr;}
    if(*env.enemies){
        if(countdown>=0&&(*env.enemies)->bosses[0]&&!dialogue){
            if(countdown<previous_countdown){if(countdown<=5){for(auto& vm:countdown_digits)vm.pending_interrupt=9;env.play_sound(36);}else if(countdown<=10){for(auto& vm:countdown_digits)vm.pending_interrupt=8;env.play_sound(27);}}
            else if(countdown>previous_countdown)for(auto& vm:countdown_digits)vm.pending_interrupt=7;
            if(countdown!=previous_countdown){env.bind_digit(*animations,countdown_digits[0],wrapping_add(countdown/10,8));env.bind_digit(*animations,countdown_digits[1],wrapping_add(countdown%10,8));}
            previous_countdown=countdown;
        }
        boss=(*env.enemies)->bosses[0];
        if(boss&&!(boss->state.flags&0x11)){
            const auto phase=(display_flags>>1)&3;const bool spell=*env.spell_flags&1;const auto hp=boss->state.health_to_interrupt;
            if(phase==0&&hp<(spell?2000:700)){enemy_marker.pending_interrupt=7;display_flags=(display_flags&~4u)|2;}
            else if(phase==1&&hp<(spell?1000:400)){enemy_marker.pending_interrupt=8;display_flags=(display_flags&~2u)|4;}
            else if(phase==2&&hp<(spell?400:200)){enemy_marker.pending_interrupt=9;display_flags|=6;}
            else if(phase==3&&hp>(spell?400:200)){enemy_marker.pending_interrupt=10;display_flags&=~6u;}
            env.update_animation(enemy_marker);
            const float x=boss->state.current.position.x;
            enemy_marker.position.x=Scalar::add(x,224.0f);enemy_marker.position.y=480;
            auto distance=number(x)-number((*env.player)->position.x);if(distance<number(0))distance=-distance;
            const auto alpha=(distance<number(64)||distance.is_nan())?static_cast<u8>(64u-static_cast<u32>((distance*number(-2.984375f)).truncate_int())):255;
            enemy_marker.color=(enemy_marker.color&0xffffff)|(static_cast<u32>(alpha)<<24);
            if(x<-192||x>192)enemy_marker.color&=0xffffff;
        }
    }
    elapsed.tick();return 1;
}
}
