#include "FrameImage.hpp"
#include "FrameState.hpp"
#include "../sdl/ApplicationState.hpp"

namespace th17::sdl {
bool ApplicationState::can_rollback()const{
    const auto* stage=scene();
    return room_seats&&stage&&stage->active()&&!stage->battle.dialogue_present()&&!run->previous_scene()&&
        flow.current==AppScene::Game&&flow.pending==flow.current&&loading_destination==AppScene::Uninitialized&&
        !exiting&&failure.empty()&&fades.empty()&&pending_page<0&&pending_bank<0&&
        pause&&pause->state.screen==PauseScreen::Inactive;
}

bool ApplicationState::save_rollback(multiplayer::FrameImage& image){
    if(!can_rollback())return false;
    auto& stage=*scene();
    const auto loadout=stage.battle.character*3+stage.battle.session.animal;
    if(u32(loadout)>=SpellRecords::loadout_count)return false;
    auto& schedule=stage.frame_scheduler();
    multiplayer::FrameState callbacks;
    if(!callbacks.capture(schedule))return false;
    image.restore_with([&schedule]{multiplayer::FrameState::detach(schedule);return true;});
    image.save(progress,environment,visual,
        touch,physical_input,room_partners,manual_pressed,manual_repeated,frames,failure,
        flow.pending,flow.title_return_reason,pause->state);
    // A live room cannot write the score file or enter the ranking menus
    // speculatively. SpellCard writes the primary loadout and aggregate rows.
    image.save(records.game,records.spells.loadouts[loadout],records.spells.aggregate,
        records.music,records.trophies,records.error);
    run_input->save_rollback(image);
    stage.save_rollback(image);
    if(!animations.save_rollback(image))return false;
    image.restore_with([this,&schedule,callbacks=std::move(callbacks)]{
        renderer.invalidate();
        return callbacks.restore(schedule);
    });
    return true;
}

bool ApplicationState::rollback_boundary(){
    if(!room_speculative){
        graphics.logical_drawing=false;
        return true;
    }
    room_barrier=true;
    return false;
}
}
