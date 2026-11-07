#include "FrameImage.hpp"
#include "../game/RunInput.hpp"
#include <algorithm>

namespace th17 {
void StageGameplay::save_rollback(multiplayer::FrameImage& image){
    image.save(input,prepared,started,suspended,capture_times,background_draw,alternate_music,error,
        controller->age,controller->counters,controller->shutdown_frames,controller->global_phase,
        controller->reset_age,controller->reset_duration,controller->error,progression->error);
    image.save(deformation.mesh,deformation.error,deformation.enabled,popups.glyph,popups.next_number,popups.entries,popups.error);
    auto& b=background;
    image.save(b.embedded,b.object_flags,b.instance_flags,b.initialized,b.deforming,b.rate,b.view_direction,
        b.script.state,b.script.error,b.error,b.color,b.frames,b.drawn_instances,b.culled_instances,b.drawn_primitives,b.draw_objects);
    image.restore_with([&b,primitives=b.primitives]{
        std::copy(primitives.begin(),primitives.end(),b.primitives.begin());
        return true;
    });
    for(auto* vm:scene_animations)if(vm)image.save(*vm);
    battle.save_rollback(image);
}

void RunInput::save_rollback(multiplayer::FrameImage& image){
    image.save(snapshot,active,entered,physical,practice_tools,fps,touch,pending_spirit,
        input,auto_focus,recorded_fps,lock_frames,error);
}
}
