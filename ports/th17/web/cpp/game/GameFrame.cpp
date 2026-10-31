#include "GameFrame.hpp"
namespace th17 {
i32 GameFrame::fail(const char* message){if(error.empty())error=message;return i32(FrameAction::Error);}
i32 GameFrame::update(const GameFrameContext& input){
    if(run.application_flags&0x180)return i32(FrameAction::Remove);
    if(run.scene_flags&0x4000){
        if(run.scene_flags&0x10)return i32(FrameAction::StopSuccess);
        run.ending_age=wrapping_add(run.ending_age,1);
        if(run.practice_record&&run.ending_age==120&&!host.return_to_title())return fail("Ending return unavailable");
        if(run.ending_age==180&&!host.transition({5,200,83,0xffffffff}))return fail("Ending transition unavailable");
        if(run.ending_age>=380)run.screen=(run.application_flags&0x2000)?2:(run.difficulty==4?16:15);
    }
    if((hud.flags&0x100)&&hud.notice_age.current<120&&!(run.scene_flags&0x10))return i32(FrameAction::Continue);
    if(age.current==0){const i32 result=host.initialize_stage();if(result<0)return fail("Stage initialization failed");if(result)return i32(FrameAction::Continue);}
    else if(age.current==30&&!host.start_stage())return fail("Stage start unavailable");
    if(host.loading_state()==2&&!host.retire_loading())return fail("Loading scene retirement unavailable");
    if(age.current==5)global_phase=2;
    if(run.scene_flags&4){run.scene_flags|=0x80;return i32(FrameAction::Continue);}
    if(run.scoring.mode_flags&0x40){
        if((input.pressed&0x80103)||(run.scene_flags&0x70))run.screen=(run.application_flags&0x2000)?2:4;
        if(age.current==3840){if(!host.transition({5,60,52,0}))return fail("Demo transition unavailable");}
        else if(age.current==3900)run.screen=(run.application_flags&0x2000)?2:4;
    }
    hud.update_score(score.score,run.scoring);
    if(run.scene_flags&0x70)return i32(FrameAction::StopSuccess);
    if(run.scene_flags&0x10000){
        if(!reset_age){if(enemies.current_chapter<43&&!host.pause_stage_music())return fail("Scene music pause unavailable");if(!host.interrupt_stage_visual())return fail("Scene visual interrupt unavailable");}
        reset_age=wrapping_add(reset_age,1);
        if(reset_age<reset_duration&&reset_age>1)return i32(FrameAction::StopSuccess);
        if(reset_age>=reset_duration){
            if(enemies.current_chapter<43){const auto* stage=StageConfiguration::get(run.stage);if(!stage)return fail("Scene stage configuration unavailable");if(!host.reload_stage_music(stage->stage_music_index)||!host.flush_audio())return fail("Scene audio reset unavailable");if(enemies.current_chapter<43&&!host.resume_environment(double(counters[0])/60.0))return fail("Scene environment resume unavailable");}
            run.scene_flags&=~0x10000u;reset_age=0;
        }
    }
    if(!host.advance_scene_animations())return fail("Scene animation update unavailable");
    for(auto& counter:counters)counter=wrapping_add(counter,1);if(shutdown_frames)shutdown_frames=wrapping_sub(shutdown_frames,1);age.tick(&input.rate);return i32(FrameAction::Continue);
}
}
