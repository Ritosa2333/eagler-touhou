#pragma once
#ifdef TH11_MULTIPLAYER
#include "GameSession.hpp"
#include "StageRollback.hpp"
#include "EnemyRollback.hpp"
namespace th11 {
class WorldRollback;

// A checkpoint is a pre-tick state. Keep every crossed checkpoint alive until
// the complete journal UndoTo AND all typed owner restores have completed.
class WorldRollbackFrame final {
public:
    WorldRollbackFrame()=default;
    WorldRollbackFrame(const WorldRollbackFrame&)=delete;
    WorldRollbackFrame& operator=(const WorldRollbackFrame&)=delete;
    bool valid()const noexcept{return ready_&&!identity_.expired();}
    u32 frame()const noexcept{return frame_;}
private:
    friend class WorldRollback;
    friend struct WorldRollbackTestAccess;
    WorldRollback* owner_=nullptr;
    std::weak_ptr<const unsigned char> identity_;
    u32 frame_=0,seat_count_=0;
    bool ready_=false;
    GameBattle* battle_=nullptr;
    Stage* stage_=nullptr;
    Stage* outgoing_stage_=nullptr;
    StageResources* resource_=nullptr;
    StageResources* previous_resource_=nullptr;
    Dialogue* dialogue_=nullptr;
    std::array<MultiplayerPilot*,3> pilots_{};
    std::array<PlayerFrame*,3> players_{};
    std::array<BombController*,3> bombs_{};
    AnmRollbackFrame animations_,presentation_,hitbox_;
    EnemyRollbackFrame enemies_;
    StageRollbackFrame stage_state_;
    StageRollbackFrame outgoing_stage_state_;
    DeformationRollbackFrame deformations_;
    std::array<DeformationRollbackFrame,3> bomb_deformations_;
    std::vector<BattleEvent> events_;
    std::vector<DialogueText> text_requests_;
    std::vector<AsciiRequest> ascii_requests_;
    std::vector<ScreenFade> fades_;
    std::vector<ScreenShake> shakes_;
    std::unordered_map<Enemy*,unsigned> damage_remainders_;
    std::string session_error_,battle_error_,dialogue_error_;
};

// This title adapter covers a stable multiplayer combat resource graph.
// Pause, ending/results and structural stage changes require confirmed history.
// Stable outgoing/incoming stage overlap retains both Stage owners. Host input,
// netplay, clocks, graphics and output watermarks are NOT
// byte-restored here. Host texture writes require their separate frame owner.
class WorldRollback final {
public:
    explicit WorldRollback(GameSession& session):session_(session),identity_(std::make_shared<const unsigned char>(0)){}
    ~WorldRollback(){EndRollbackFrame();}
    WorldRollback(const WorldRollback&)=delete;
    WorldRollback& operator=(const WorldRollback&)=delete;
    bool CanCapture()const noexcept;
    bool BeginRollbackFrame(Netplay::RollbackJournal&,WorldRollbackFrame&);
    void EndRollbackFrame()noexcept;
    bool ValidateRollbackFrame(const WorldRollbackFrame&,u32 restored_frame)const noexcept;
    bool RestoreRollbackFrame(const WorldRollbackFrame&,u32 restored_frame);
    bool RollbackFrameActive()const noexcept{return active_!=nullptr;}
    bool BoundaryRequested()const noexcept;
private:
    GameSession& session_;
    std::shared_ptr<const unsigned char> identity_;
    WorldRollbackFrame* active_=nullptr;
    bool ObserversIdle()const noexcept;
};
}
#endif
