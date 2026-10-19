#include "BattleLoading.hpp"
namespace th075 {
void ArcadeRoute::advance(GameSelection& selection,const SelectionUnlocks& unlocks,ReplayRandom& random){
 selection.storyStage=u8(selection.storyStage+1);
 if(selection.storyStage==1){u32 used=0;available=0;for(u8 slot=0;slot<10;slot++){u8 id=slot;if(slot<6){do{id=u8(random.next()%6);}while(used&(1u<<id));used|=1u<<id;}if(unlocks.character_available(i8(id),false)){opponents[slot]=id;++available;}}}
 if(selection.storyStage<1||selection.storyStage>10)return;
 selection.character[1]=opponents[selection.storyStage-1];selection.background=selection.character[1];
 if(selection.character[1]<6){const auto roll=random.next();if(roll<0x4000||selection.character[0]==u8(CharacterId::Remilia))selection.background=u8(selection.background+10);}
 selection.alternatePalette[1]=selection.character[0]==selection.character[1]?u8(1-selection.alternatePalette[0]):0;
 selection.firstSpell[1]=u8(random.next()%3);selection.secondSpell[1]=u8(random.next()%3);
}
bool choose_background(u8 index,bool animated,BackgroundKind& result){switch(index){
 case 0:result=animated?BackgroundKind::BG00a:BackgroundKind::BG00x;return true;
 case 1:result=animated?BackgroundKind::BG01a:BackgroundKind::BG01x;return true;
 case 2:result=animated?BackgroundKind::BG02a:BackgroundKind::BG02x;return true;
 case 3:result=animated?BackgroundKind::BG03a:BackgroundKind::BG03x;return true;
 case 4:result=animated?BackgroundKind::BG04a:BackgroundKind::BG04x;return true;
 case 5:result=animated?BackgroundKind::BG05a:BackgroundKind::BG05x;return true;
 case 6:result=animated?BackgroundKind::BG06a:BackgroundKind::BG06x;return true;
 case 7:result=animated?BackgroundKind::BG07a:BackgroundKind::BG07x;return true;
 case 8:result=animated?BackgroundKind::BG08a:BackgroundKind::BG08x;return true;
 case 9:result=animated?BackgroundKind::BG09a:BackgroundKind::BG09x;return true;
 case 10:result=animated?BackgroundKind::BG00b:BackgroundKind::BG00y;return true;
 case 11:result=animated?BackgroundKind::BG01b:BackgroundKind::BG01y;return true;
 case 12:result=animated?BackgroundKind::BG02b:BackgroundKind::BG02y;return true;
 case 13:result=animated?BackgroundKind::BG03b:BackgroundKind::BG03y;return true;
 case 14:result=animated?BackgroundKind::BG04b:BackgroundKind::BG04y;return true;
 case 15:result=animated?BackgroundKind::BG05b:BackgroundKind::BG05y;return true;
 case 20:result=animated?BackgroundKind::BG00c:BackgroundKind::BG00z;return true;
 case 22:result=animated?BackgroundKind::BG02c:BackgroundKind::BG02z;return true;
 }return false;}
bool BattleLoading::load(GameSelection& selection,const GameSettings& settings,const SelectionSettings& playerSettings,GameProfile& profile,ReplayStreams& replay,BattleSession& session,RoundRules& rules,BattleLoadSignals& platform){
 error.clear();session.mode=selection.mode;story.difficulty=selection.difficulty;story.background=selection.background;
 if(session.replayMode!=2&&selection.mode==GameMode::Arcade){arcade.advance(selection,profile.selection_unlocks(),replay.random);session.totalRounds=i8(arcade.available);session.roundIndex=i8(selection.storyStage);}
 for(u8 side=0;side<2;side++){
  const bool retain=side==0&&(selection.mode==GameMode::Story||selection.mode==GameMode::Arcade)&&characters[0];
  if(!retain){const auto* definition=character_definition(selection.character[side]);if(!definition){error="Invalid character selection";return false;}characters[side]=create_character(selection.character[side]);auto& actor=characters[side]->actor;actor.side=side;actor.spellChoices={i16(i8(selection.firstSpell[side])),i16(i8(selection.secondSpell[side]))};
   if(!platform.load_character(side,*definition,selection.alternatePalette[side],*characters[side])){error="Character resource load failed";return false;}actor.input=settings.mappings[side];
   if(selection.mode==GameMode::Story||(selection.mode==GameMode::Arcade&&side==1)){actor.damage.sourceModifier=1;actor.guardEstablished=selection.difficulty==0;}
   else{constexpr std::array<float,5> handicap{0.5f,0.75f,1,1.5f,2};if(playerSettings.handicap[side]<handicap.size())actor.damage.sourceModifier=handicap[playerSettings.handicap[side]];actor.guardEstablished=playerSettings.easyCommands[side]!=0;}
  }
  auto& character=*characters[side];character.set_difficulty(selection.difficulty);
  if(side==0&&selection.mode==GameMode::Story){
   if(session.replayMode!=2){selection.resumeStory=false;if(selection.startingStage!=0){selection.resumeStory=true;session.continueCredits=0;rules.remainingLives=2;character.set_next_story_stage(selection.startingStage);selection.startingStage=0;character.actor.score=0;character.actor.combat.health=character.actor.combat.maximumHealth=10000;}selection.storyStage=character.next_story_stage();}
   session.roundIndex=i8(selection.storyStage);story.stage=selection.storyStage;character.prepare_story(story.stage);selection.character[1]=character.story_opponent();
  }
  character.reset(platform.reset_events(side));
 }
 const auto mode=selection.mode;
 const bool oneCpu=mode==GameMode::Training||mode==GameMode::Versus||mode==GameMode::Story||mode==GameMode::Arcade;
 for(u8 side=0;side<2;side++)characters[side]->actor.combat.storyOpponent=oneCpu?(side!=selection.activePlayer):mode==GameMode::Demonstration;
 story.camera.initialize(characters[0]->actor.motion,characters[1]->actor.motion);
 auto& first=characters[0]->actor;auto& other=characters[1]->actor;
 if(mode==GameMode::Story){
  story.fadeAlpha=0;characters[0]->initialize_story(other,story);platform.load_story_artwork(*characters[0],selection.storyStage);other.combat.maximumHealth=other.combat.health=other.phaseMaximum[other.combat.spellPhase];
  if(selection.storyStage==1||selection.storyStage>7||selection.resumeStory){session.continueCredits=selection.resumeStory?0:3;rules.remainingLives=2;profile.select(selection.character[0],selection.difficulty);first.score=0;rules.highScore=profile.selected().ranking[0].score;session.scoreToBeat=profile.selected().ranking[9].score;if(session.replayMode!=2)profile.begin_attempt();first.combat.health=first.combat.maximumHealth=10000;}
  rules.awardedSpellTime=0;session.maximumStoryBonus=0;for(u8 phase=1;phase<=other.combat.spellPhase&&phase<other.phaseBonuses.size();phase++)session.maximumStoryBonus=increment_wrapped(session.maximumStoryBonus,other.phaseBonuses[phase]);
  rules.bonusValue=rules.bonusTimer=0;
  if(session.replayMode==2){rules.highScore=session.scoreToBeat=0;if(!replay.round){error="Missing replay round";return false;}first.combat.health=first.combat.maximumHealth=replay.round->initialHealth;first.score=i32(replay.round->initialScore);}
  else{profile.complete_stage(selection.storyStage);platform.save_profile(profile.save(replay.random));}
  profile.load_spell_records(rules);
 }else {first.score=other.score=0;rules.highScore=session.scoreToBeat=0;story.fadeAlpha=0;first.combat.health=first.combat.maximumHealth=other.combat.health=other.combat.maximumHealth=10000;}
 story.completedStages=u8(rules.remainingLives);story.route=u8(session.continueCredits);
 story.background=mode==GameMode::Story?story.background:selection.background;selection.background=story.background;session.characters=selection.character;
 if(session.replayMode==1)platform.prepare_round_recording(selection,session,rules,first);
 return true;
}
}
