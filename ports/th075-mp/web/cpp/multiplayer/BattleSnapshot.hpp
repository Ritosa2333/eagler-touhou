#pragma once
#include "../game/BattleScene.hpp"
#include <variant>
#include "../game/ReimuCharacter.hpp"
#include "../game/ReimuObjects.hpp"
#include "../game/MarisaCharacter.hpp"
#include "../game/MarisaObjects.hpp"
#include "../game/SakuyaCharacter.hpp"
#include "../game/SakuyaObjects.hpp"
#include "../game/AliceCharacter.hpp"
#include "../game/AliceObjects.hpp"
#include "../game/PatchouliCharacter.hpp"
#include "../game/PatchouliObjects.hpp"
#include "../game/YoumuCharacter.hpp"
#include "../game/YoumuObjects.hpp"
#include "../game/RemiliaCharacter.hpp"
#include "../game/RemiliaObjects.hpp"
#include "../game/YuyukoCharacter.hpp"
#include "../game/YuyukoObjects.hpp"
#include "../game/YukariCharacter.hpp"
#include "../game/YukariObjects.hpp"
#include "../game/SuikaCharacter.hpp"
#include "../game/SuikaObjects.hpp"
#include "../game/MeilingCharacter.hpp"
#include "../game/MeilingObjects.hpp"
namespace th075 {
// Only immutable asset pointers survive a snapshot. Live fighter addresses stay
// stable; object/effect lists are rebuilt by value, never memcpy'd containers.
class BattleSnapshot {
 using Character = std::variant<ReimuCharacter, MarisaCharacter, SakuyaCharacter, AliceCharacter, PatchouliCharacter, YoumuCharacter, RemiliaCharacter, YuyukoCharacter, YukariCharacter, SuikaCharacter, MeilingCharacter>;
 using Object = std::variant<ReimuObject, MarisaObject, SakuyaObject, AliceObject, PatchouliObject, YoumuObject, RemiliaObject, YuyukoObject, YukariObject, SuikaObject, MeilingObject>;
 std::array<Character,2> characters;
 std::array<std::array<std::vector<Object>,4>,2> objects;
 std::array<std::vector<SharedEffect>,2> effects;
 BattleProgress progress;BattleSession session;BattleControls controls;BattleFrameState timing;u8 foreground;
 ReplayStreams replay;std::array<std::size_t,4> replaySizes{};
 BattleContacts contacts;RoundRules rules;BodySeparation bodies;CharacterAIContext ai;StoryEnvironment story;
 GameMode mode;u8 replayMode,difficulty;
 static Character character_image(u8 id,const CharacterController& character){switch(id){
case 0: return static_cast<const ReimuCharacter&>(character);
case 1: return static_cast<const MarisaCharacter&>(character);
case 2: return static_cast<const SakuyaCharacter&>(character);
case 3: return static_cast<const AliceCharacter&>(character);
case 4: return static_cast<const PatchouliCharacter&>(character);
case 5: return static_cast<const YoumuCharacter&>(character);
case 6: return static_cast<const RemiliaCharacter&>(character);
case 7: return static_cast<const YuyukoCharacter&>(character);
case 8: return static_cast<const YukariCharacter&>(character);
case 9: return static_cast<const SuikaCharacter&>(character);
case 10: return static_cast<const MeilingCharacter&>(character);
}return ReimuCharacter{};}
 static Object object_image(u8 id,const FighterObject& object){switch(id){
case 0: return static_cast<const ReimuObject&>(object);
case 1: return static_cast<const MarisaObject&>(object);
case 2: return static_cast<const SakuyaObject&>(object);
case 3: return static_cast<const AliceObject&>(object);
case 4: return static_cast<const PatchouliObject&>(object);
case 5: return static_cast<const YoumuObject&>(object);
case 6: return static_cast<const RemiliaObject&>(object);
case 7: return static_cast<const YuyukoObject&>(object);
case 8: return static_cast<const YukariObject&>(object);
case 9: return static_cast<const SuikaObject&>(object);
case 10: return static_cast<const MeilingObject&>(object);
}return ReimuObject{};}
public:
 BattleSnapshot(const BattleScene& scene,const std::array<u8,2>& ids):progress(scene.progress),session(scene.session),controls(scene.controls),timing(scene.timing),foreground(scene.foreground),
 replay(scene.simulation.replay),contacts(scene.simulation.contacts),rules(scene.simulation.rules),bodies(scene.simulation.bodies),ai(scene.simulation.ai),story(scene.simulation.story),mode(scene.simulation.mode),replayMode(scene.simulation.replayMode),difficulty(scene.simulation.difficulty){
  const auto& sim=scene.simulation;
  for(u8 side=0;side<2;++side){characters[side]=character_image(ids[side],*sim.characters[side]);
   for(u8 layer=0;layer<4;++layer)for(const auto& object:sim.objects[side].layers[layer])objects[side][layer].push_back(object_image(ids[side],*object));}
  for(u8 layer=0;layer<2;++layer)for(const auto& effect:sim.effects.layers[layer])effects[layer].push_back(*effect);
  if(replay.round){const auto& r=*replay.round;replaySizes={r.playerInputs.size(),r.randomValues.size(),r.auxiliaryInputs.size(),r.events.size()};}
 }
 void restore(BattleScene& scene)const{
  auto& sim=scene.simulation;
  for(u8 side=0;side<2;++side){std::visit([&](const auto& value){using T=std::decay_t<decltype(value)>;static_cast<T&>(*sim.characters[side])=value;},characters[side]);
   for(u8 layer=0;layer<4;++layer){auto& list=sim.objects[side].layers[layer];list.clear();for(const auto& value:objects[side][layer])std::visit([&](const auto& item){list.push_back(std::make_unique<std::decay_t<decltype(item)>>(item));},value);}}
  for(u8 layer=0;layer<2;++layer){auto& list=sim.effects.layers[layer];list.clear();for(const auto& value:effects[layer])list.push_back(std::make_unique<SharedEffect>(value));}
  sim.rules=rules;sim.contacts=contacts;sim.bodies=bodies;sim.ai=ai;sim.story=story;sim.mode=mode;sim.replayMode=replayMode;sim.difficulty=difficulty;sim.replay=replay;
  if(replay.round){auto& r=*replay.round;r.playerInputs.resize(replaySizes[0]);r.randomValues.resize(replaySizes[1]);r.auxiliaryInputs.resize(replaySizes[2]);r.events.resize(replaySizes[3]);}
  // BattleProgress refers to the live rules owner; copy its values explicitly.
  scene.progress.victorySlide=progress.victorySlide;
  scene.progress.textSlide=progress.textSlide;
  scene.progress.dialogueProgress=progress.dialogueProgress;
  scene.progress.victoryLine=progress.victoryLine;
  scene.progress.paused=progress.paused;
  scene.progress.trainingInstantRecovery=progress.trainingInstantRecovery;
  scene.progress.pauseChoice=progress.pauseChoice;
  scene.progress.pauseFade=progress.pauseFade;
  scene.progress.overlayTime=progress.overlayTime;
  scene.progress.demoFrames=progress.demoFrames;
  scene.progress.healthFade=progress.healthFade;
  scene.progress.savedInputs=progress.savedInputs;
  scene.session=session;scene.controls=controls;scene.timing=timing;scene.foreground=foreground;
 }
};
}
