#include "GameplayControl.hpp"
#include "Presentation.hpp"
namespace th08 {
namespace {
template<class T,u32 N>bool outside(const T (&data)[N]){for(const auto value:data)if(value<6543||value>106543)return true;return false;}
constexpr i32 stage_music[]{1,3,5,7,7,10,12,12,16};
}
void GameplayControl::advance_stage(){
    switch(game.stage){case 0:game.stage=1;break;case 1:game.stage=2;break;
    case 2:game.stage=(game.shot==0||game.shot==3||game.shot==4||game.shot==5||game.shot==10||game.shot==11)?4:3;break;
    case 3:case 4:game.stage=5;break;case 5:game.stage=(game.game_flags&0x1800)?7:6;break;case 6:game.stage=7;break;}
}
void GameplayControl::refresh_integrity(){
    numbers.integrity_value=u32(numbers.rng1[2]);const i32 sum=values.checksum();
    values.expected_checksum=(number(Extended::from_int(sum).to_float())+number(Extended::from_int(numbers.rng7[3]).to_float())).to_float();
    if(outside(numbers.rng1)||outside(numbers.rng3)||outside(numbers.rng2)||outside(numbers.rng7))values.expected_checksum=-9999;
}
void GameplayControl::late_integrity(){if(outside(numbers.rng4)||outside(numbers.rng5)||outside(numbers.rng8))values.expected_checksum=-9999;}
void GameplayControl::display_score(){
    auto& n=numbers;if(n.score>999999999)n.score=999999999;if(n.display_score==n.score)return;
    if(n.score<n.display_score)n.score=n.display_score;u32 increase=(n.score-n.display_score)/32;
    if(increase>=578910)increase=578910;else if(!increase)increase=1;
    if(u32(n.score_increment)<increase)n.score_increment=signed_bits(increase);
    if(n.display_score+u32(n.score_increment)>n.score)n.score_increment=signed_bits(n.score-n.display_score);
    n.display_score+=u32(n.score_increment);if(n.display_score>=n.score){n.score_increment=0;n.display_score=n.score;}
    if(n.high_score<n.display_score){n.high_score=n.display_score;n.high_score_retries=n.retries;}
}
JobResult GameplayControl::update(){
    if(game.stage>=9||game.difficulty>=5||game.shot<0||game.shot>=12||!clears)return JobResult::Error;
    auto& s=state;auto& flags=game.game_flags;s.frames=wrapping_add(s.frames,1);
    if(flags&0x60){
        if((flags&0x60)==0x40){
            flags|=0x60;s.load_state=1;s.next_scene=-1;
            if(!(flags&8)){
                for(const i32 shot:{game.shot,12}){auto& record=clears[shot];if(!numbers.retries)record.without_retries[game.difficulty]|=s.stage_mask;record.with_retries[game.difficulty]|=s.stage_mask;}
            }
            numbers.display_score=numbers.score;
            if(flags&1){s.next_scene=6;return JobResult::Break;}
            if(game.stage<6){
                if(flags&8){i32 next=0;for(i32 stage=game.stage+1;stage<9;stage++)if(actions.replay_stage(stage)){next=stage;break;}if(!next)menu.supervisor_state=7;else{game.stage=next;menu.supervisor_state=3;}}
                else{if(numbers.clock_time>=12){flags&=~16u;s.next_scene=9;return JobResult::Break;}advance_stage();menu.supervisor_state=3;}
            }else if(flags&8)s.next_scene=7;
            else{
                if(game.difficulty>=4){if(game.difficulty==4){clears[game.shot].without_retries[4]|=0x8000;clears[12].with_retries[4]|=0x8000;}++play.counts[game.difficulty].clears;flags|=16;numbers.display_score=numbers.score;s.next_scene=6;return JobResult::Break;}
                flags|=16;s.next_scene=9;return JobResult::Break;
            }
            if(s.next_scene<0)actions.capture_arcade();
        }
        if(menu.pressed(4097)||(flags&8)||game.stage>=6){flags&=~0x60u;if(s.next_scene>=0)menu.supervisor_state=s.next_scene;}
    }
    if(s.load_state){if(s.load_state==2)return JobResult::Exit;s.load_frames=wrapping_add(s.load_frames,1);return JobResult::Break;}
    if(s.start_music){actions.update_enemy_name();actions.release_loading_surface();s.loading_images_setup=false;if(s.start_music==1)actions.play_music(0,(flags&0x4000)?spell_music(game.current_spell).song:stage_music[game.stage]);s.start_music=0;}
    if(!menu.show_retry&&!menu.pause_state&&!(flags&2)&&!s.sticky_input&&menu.pressed(8)){
        menu.pause_state=1;menu.arcade_origin={32,16};menu.arcade_size={384,448};s.restore_viewport=1;
        actions.pause_audio();actions.sound(34);actions.update_game_time();random.save();high_score.pauses=wrapping_add(high_score.pauses,1);values.update_integrity();random.restore();
    }
    renderer.viewport={u32(Scalar::truncate(menu.arcade_origin.x)),u32(Scalar::truncate(menu.arcade_origin.y)),u32(Scalar::truncate(menu.arcade_size.x)),u32(Scalar::truncate(menu.arcade_size.y)),0,1};renderer.camera_mode=0xff;
    if((flags&8)&&s.replay_mode==1&&!input.dialogue){
        ++s.slow_frames;const i32 fps=input.replay_fps;
        if((fps<20&&s.slow_frames%3)||(fps>=20&&fps<30&&s.slow_frames%2)||(fps>=30&&fps<40&&s.slow_frames%3==0)||(fps>=40&&fps<50&&s.slow_frames%6==0))return JobResult::Break;
    }
    if(flags&2){
        if(menu.keys&&menu.keys!=menu.previous_keys)menu.supervisor_state=1;s.demo_frames=wrapping_add(s.demo_frames,1);
        constexpr i32 durations[]{6000,4800,4920,6900};if(u32(s.demo_index)<4){const i32 end=durations[s.demo_index];if(s.demo_frames==end)actions.demo_fade();if(s.demo_index==3?s.demo_frames==end+120:s.demo_frames>=end+120){menu.supervisor_state=1;return JobResult::Break;}}
    }
    refresh_integrity();flags=(flags&~4u)|u32(!menu.show_retry&&!menu.pause_state)*4u;
    renderer.clear_target(2,input.fog);
    if(menu.pause_state==1||menu.pause_state==2||menu.show_retry)return JobResult::Break;
    display_score();late_integrity();
    if(config.slow_mode){s.sticky_input=false;++s.slow_frames;const i32 count=input.active_bullets;
        if((count>=320&&s.slow_frames%3==0)||(count<320&&count>=224&&s.slow_frames%4==0)||(count<224&&count>=128&&s.slow_frames%5==0)){s.sticky_input=true;return JobResult::Break;}if(count<128)s.slow_frames=0;}
    values.tampered();++s.play_frames;return JobResult::Continue;
}
JobResult GameplayControl::draw(){if(!presentation::render_only&&menu.pause_state)menu.pause_state=2;if(menu.supervisor_state!=2||(game.game_flags&0x60)==0x20||state.load_state)return JobResult::Break;return JobResult::Continue;}
}
