#include "ReplayConfiguration.hpp"
namespace th075 {
void ReplayConfiguration::record(ReplayFile& file,ReplayRound& round,const GameSelection& selection,const SelectionSettings& settings,const RoundRules& rules,const FighterActor& player){
 auto& h=file.header;u8 side=0;
 if(selection.mode==GameMode::Versus){h[13]=0;h[15]=selection.activePlayer;side=selection.activePlayer;}
 else if(selection.mode==GameMode::VersusCpu){h[13]=0;h[15]=255;}
 else if(selection.mode==GameMode::Arcade){h[13]=u8(selection.difficulty+5);h[15]=0;round.initialScore=u32(player.score);}
 else if(selection.mode==GameMode::Story){h[13]=u8(selection.difficulty+1);h[15]=255;round.initialHealth=player.combat.health;round.initialLives=u8(rules.remainingLives);round.initialScore=u32(player.score);}
 h[10]=selection.character[side];h[11]=selection.firstSpell[side];h[12]=selection.secondSpell[side];h[16]=selection.alternatePalette[side];
 if(selection.mode!=GameMode::Story){h[17]=settings.easyCommands[side];h[18]=settings.handicap[side];}
 const u8 other=u8(1-side);round.opponentOptions[0]=selection.character[other];round.opponentOptions[1]=selection.firstSpell[other];round.opponentOptions[2]=selection.secondSpell[other];round.opponentOptions[3]=selection.alternatePalette[other];
 if(selection.mode!=GameMode::Story){round.opponentOptions[4]=settings.easyCommands[other];round.opponentOptions[5]=settings.handicap[other];}
 round.stage=selection.mode==GameMode::Story?selection.storyStage:selection.background;
}
bool ReplayConfiguration::load(ReplayFile& file,u32 index,GameSelection& selection,SelectionSettings& settings,RoundRules& rules,ReplayStreams& streams){
 if(file.header[13]==0)index=0;if(index>=file.rounds.size())return false;
 auto& round=file.rounds[index];const auto& h=file.header;u8 side=0;
 if(h[13]==0){selection.mode=i8(h[15])<0?GameMode::VersusCpu:GameMode::Versus;if(selection.mode==GameMode::Versus){if(h[15]>1)return false;side=h[15];selection.activePlayer=side;}}
 else{selection.activePlayer=0;if(h[13]<5){selection.mode=GameMode::Story;selection.difficulty=u8(h[13]-1);selection.storyStage=round.stage;rules.remainingLives=i8(round.initialLives);}else{selection.mode=GameMode::Arcade;selection.difficulty=u8(h[13]-5);}}
 if(selection.difficulty>3)return false;
 selection.character[side]=h[10];selection.firstSpell[side]=h[11];selection.secondSpell[side]=h[12];selection.alternatePalette[side]=h[16];
 if(selection.mode!=GameMode::Story){settings.easyCommands[side]=h[17];settings.handicap[side]=h[18];const u8 other=u8(1-side);selection.character[other]=round.opponentOptions[0];selection.firstSpell[other]=round.opponentOptions[1];selection.secondSpell[other]=round.opponentOptions[2];selection.alternatePalette[other]=round.opponentOptions[3];settings.easyCommands[other]=round.opponentOptions[4];settings.handicap[other]=round.opponentOptions[5];selection.background=round.stage;}
 streams.bind(ReplayMode::Playback,&round,streams.random.state);return true;
}
}
