#pragma once
#include "GameWorld.hpp"
#include "ReplayArchive.hpp"
#include "PlayerRecords.hpp"
#include "Ending.hpp"
#include "TouchMotionTrack.hpp"
namespace th09 {
enum class SessionPhase {inactive,match,game_over,match_complete,ending,finished};
// Owns a complete run. Platform code renders the active world or ending and
// persists the resulting native-compatible files; it does not select routes.
class GameSession:public MotionSource {
    EclWorldState& state;GameResources& resources;AnmExecutor& animations;
    WorldPresentation& output;EndingServices& ending_output;PlayerRecords& records;
    WorldConfiguration initial,current;ReplayMetadata replay_metadata;
    ReplayFile replay;ReplayPlayback playback;GameInput inputs[3];
    std::array<ScoreCounter,2> scores{};u32 starting_replay_stage=0;bool demo=false;InputFrame demo_input;u32 demo_frames=0;
    i32 warmed_stage=-1;
    bool start_stage(bool first);bool restore_replay();void capture_result();
    bool fail(const std::string& message){error=message;return false;}
public:
    std::unique_ptr<GameWorld> world;std::unique_ptr<Ending> ending;ReplayArchive recording;
    ScoreEntry result;SessionPhase phase=SessionPhase::inactive;
    u8 continues=0;bool is_replay=false,recordable=true;u32 frames=0;std::string error;
    FrameTiming timing;touch::MotionTrack motion;
    // `target` means (x,y) is an absolute field position the player walks toward,
    // not a pre-scaled velocity. Networked gestures ship the target because a
    // velocity sampled from the sender's position is stale by the lockstep input
    // delay, which makes the player orbit the finger instead of reaching it.
    struct MotionSample {bool enabled=false;float x=0,y=0;bool unlimited=false;bool target=false;};MotionSample motion_input[2];
    bool movement(const PlayerMotion&,float,float,float&,float&)override;
    void clear_motion(){motion_input[0]=motion_input[1]={};}
    GameSession(EclWorldState& w,GameResources& r,AnmExecutor& a,WorldPresentation& p,EndingServices& e,PlayerRecords& s):state(w),resources(r),animations(a),output(p),ending_output(e),records(s){}
    bool begin(const WorldConfiguration&,const ReplayMetadata&,const char* date);
    bool play(const ReplayFile&,u32 stage,bool attract=false);
    bool update(u16 left,u16 right,u16 menu);
    void warm_resources();
    bool retry();bool continue_game();
    void finish();
    bool is_demo()const{return demo;}
    u32 replay_frame()const{return playback.frame();}
    u32 replay_length()const{return playback.length();}
    u32 playback_schedule()const{return world?playback.after_update(world->battle->state.flags,world->dialogue->id):1;}
    ReplayFile save_replay(const char* name);
};
}
