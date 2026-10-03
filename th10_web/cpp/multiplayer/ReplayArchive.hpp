#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer Replay must not enter an ordinary build
#endif
#include "SessionSetup.hpp"
#include "NetplayRuntime.hpp"
#include "InputLanes.hpp"
#include "../game/ApplicationConfig.hpp"
#include "../game/Replay.hpp"
#include "../game/Rng.hpp"
#include <eagler/netplay/InputReplay.hpp>

namespace th10::multiplayer {
struct ReplayDescription {
    SessionSetup setup{};
    ApplicationConfig configuration{};
    char name[12]{};
    i32 timestamp=0,score=0,scoreUnits=0,lastStage=1;
};

struct ReplayCheckpointSeat {
    std::int16_t lives=0,power=0;
    u8 character=0,shot=0,lifeState=0,rescueTicks=0;
    std::int8_t rescueTarget=-1;
    u8 waitingForFocusRelease=0;
};
struct ReplayCheckpointCooperation {
    u8 seatCount=0,retryPending=0;
    u16 wipeTicks=0;
    ReplayCheckpointSeat seats[Netplay::MAX_PLAYERS]{};
};

// Portable stage-entry state. ReplayStage is already TH10's retail on-disk
// stage snapshot (no pointers); the extra values preserve state needed when
// multiplayer Replay bootstraps directly into a later stage.
struct ReplayCheckpoint {
    u32 label=0,firstFrame=Netplay::INVALID_FRAME;
    Rng scriptRandom{},visualRandom{};
    Rng activationScriptRandom{},activationVisualRandom{};
    bool activationRandomValid=false;
    u32 faithCursor=0,laserLastId=0;
    u32 reservedStage=1;
    bool retainedStateValid=false;
    GameInput inputSeats[Netplay::MAX_PLAYERS]{};
    ReplayCheckpointCooperation cooperation{};
    ReplayStage pilots[Netplay::MAX_PLAYERS]{};
    u16 reservedPower[Netplay::MAX_PLAYERS]{};
    bool cheatMovementUsed=false;
};

// A global recording timeline survives native network-frame resets. Menus,
// restart and continue are inputs in that timeline, not an external sidecar.
class ReplayArchive {
public:
    struct SaveRequest {
        char file[256]{},name[12]{};
        u32 frame=Netplay::INVALID_FRAME;
        i32 score=0,scoreUnits=0,lastStage=1;
        bool pending=false;
    } save;

    bool Begin(const SessionSetup&,const ApplicationConfig&);
    bool Load(const u8*,std::size_t);
    void Clear();
    bool Stamp(u32 localFrame,u32 stage);
    bool Commit(const NetplayRuntime&);
    bool NextGeneration(u32 generation);
    bool CaptureCheckpoint(const ReplayCheckpoint&);
    bool SelectCheckpoint(u32 stage);
    const Netplay::InputReplay::Frame* PlaybackFrame(u32 localFrame,u32 stage) const;
    bool Played(u32 localFrame);
    bool Encode(std::vector<u8>&,const ReplayDescription* description=nullptr,u32 frameCount=Netplay::INVALID_FRAME) const;
    static bool Inspect(const u8*,std::size_t,Netplay::InputReplayInfo&,ReplayDescription&);
    static bool SafePath(const char*);
    bool RequestSave(const char* file,const char* name,u32 frame,i32 score,i32 units,i32 stage);

    bool Recording()const{return tape_.Recording();}
    bool Playing()const{return tape_.Loaded();}
    bool Complete()const{return Playing()&&cursor_==tape_.Info().frameCount;}
    u32 Cursor()const{return cursor_;}
    u32 Frames()const{return tape_.Info().frameCount;}
    u32 Base()const{return base_;}
    u32 Generation()const{return generation_;}
    bool SaveCommitted(u32 frame)const{return saved_!=Netplay::INVALID_FRAME&&base_+frame<=saved_;}
    void MarkSaveCommitted(u32 frame){saved_=base_+frame;}
    u32 SeekFrame(u32 stage)const;
    const ReplayCheckpoint* Checkpoint(u32 stage)const;
    const ReplayCheckpoint* SelectedCheckpoint()const{
        return selected_checkpoint_<checkpoint_count_?&checkpoints_[selected_checkpoint_]:nullptr;
    }
    void DeselectCheckpoint(){selected_checkpoint_=0xff;}
    const ReplayDescription& Description()const{return description_;}
    const Netplay::InputReplayInfo& Info()const{return tape_.Info();}

private:
    struct StampEntry {u32 frame=Netplay::INVALID_FRAME,label=0;};
    Netplay::InputReplay tape_;
    ReplayDescription description_{};
    std::array<StampEntry,Netplay::INPUT_HISTORY_SIZE> stamps_{};
    std::array<ReplayCheckpoint,7> checkpoints_{};
    u32 base_=0,next_=0,generation_=0,cursor_=0,saved_=Netplay::INVALID_FRAME;
    u32 format_version_=4;
    u8 checkpoint_count_=0,selected_checkpoint_=0xff;
};
}
