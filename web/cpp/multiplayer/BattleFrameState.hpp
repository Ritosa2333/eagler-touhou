#pragma once
#include "../game/GameBattle.hpp"
#if TH15_MULTIPLAYER
namespace th15::multiplayer {
// Exact frame state for a stable battle owner. Scene factories/checkpoint
// retries are confirmed barriers and start a new owner/window.
class BattleFrameState {
    struct Data;
    std::unique_ptr<Data> saved_;
    static auto battle_fields(GameBattle&);
    static auto pilot_fields(MultiplayerPilot&);
    static auto visual_fields(PlayerAnmHost&);
    static auto life_fields(PlayerLife&);
    static auto frame_fields(PlayerFrame&);
    static auto shot_context_fields(PlayerShotContext&);
    static auto shot_fields(PlayerShots&);
    static auto option_fields(PlayerOptions&);
    static auto spell_fields(SpellCard&);
    static auto bomb_fields(BombController&);
public:
    mutable std::string error;
    BattleFrameState();~BattleFrameState();
    BattleFrameState(BattleFrameState&&)noexcept;
    BattleFrameState& operator=(BattleFrameState&&)noexcept;
    bool capture(GameBattle&,const std::vector<AnmVm*>& extra_roots={});
    bool restore(GameBattle&)const;
    void discard()noexcept;
};
}
#endif
