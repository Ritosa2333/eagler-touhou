#pragma once
#include "CharacterFactory.hpp"
#include "BattleScene.hpp"
#include "GameSettings.hpp"
#include "GameProfile.hpp"
#include "BackgroundStage.hpp"
namespace th075 {
struct ArcadeRoute {
 std::array<u8,10> opponents{};u8 available=0;
 void advance(GameSelection&,const SelectionUnlocks&,ReplayRandom&);
};
bool choose_background(u8 index,bool animated,BackgroundKind& result);
struct BattleLoadSignals {
 virtual ~BattleLoadSignals()=default;
 virtual bool load_character(u8 side,const CharacterDefinition&,u8 palette,CharacterController&)=0;
 virtual void load_story_artwork(CharacterController&,u8 stage)=0;
 virtual FighterCoreEvents& reset_events(u8 side)=0;
 virtual void save_profile(const std::vector<u8>&)=0;
 virtual void prepare_round_recording(const GameSelection&,const BattleSession&,const RoundRules&,const FighterActor&)=0;
};
// Owns the actual character controllers. Story and arcade transitions retain
// the player object so story progress, score and resources survive stage loads.
class BattleLoading {
public:
 std::array<std::unique_ptr<CharacterController>,2> characters;
 ArcadeRoute arcade;StoryEnvironment story;std::string error;
 bool load(GameSelection&,const GameSettings&,const SelectionSettings&,GameProfile&,ReplayStreams&,BattleSession&,RoundRules&,BattleLoadSignals&);
 std::array<CharacterController*,2> controllers()const{return {characters[0].get(),characters[1].get()};}
};
}
