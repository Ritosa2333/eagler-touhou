#ifdef TH11_MULTIPLAYER
#include "PlayerFrame.hpp"
#include "RollbackCapture.hpp"
namespace th11 {
bool ShotManager::CaptureRollbackFrame(Netplay::RollbackJournal& journal){
    // Shots carry pointers into stable enemy/spec owners. Those owners and
    // their resources must survive the complete shared checkpoint history.
    return rollback_capture_values(journal,player,schedule,laser_active,
        damage_areas,locked_target,target_locked,last_error,shots,converted_laser_spec,activity);
}
bool PlayerFrame::CaptureRollbackFrame(Netplay::RollbackJournal& journal){
    const bool values=rollback_capture_values(journal,practice,state,input,spell,last_error,
        motion.state,motion.options,motion.body,motion.focus_animation,motion.last_error);
    // motion/body is embedded: AnmManager owns only its geometry allocation,
    // not this VM payload. Reference-bound services remain constructor-owned.
    const bool projectiles=shots.CaptureRollbackFrame(journal);
    return values&&projectiles;
}
}
#endif
