#include "StageEntry.hpp"
#include "../game/GameState.hpp"
#include "../game/GameSession.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
namespace th12::multiplayer {
StageEntry stageEntry;
std::optional<StageEntry> pendingStageEntry;
namespace {
template<class C> bool values(C& c, StageEntry& s) {
  if (!c.Values(s.version,s.stage,s.seed,s.displaySeed,s.displayCalls,s.ticks,s.lifePieces,s.bombPieces,s.score,s.rank)) return false;
  if (!c.Value(s.continuation)||!c.Array(s.nativeGlobals))return false;
  for (auto& r : s.resources) if (!c.Values(r.power,r.maxPower,r.powerStep,r.scoreStored,r.pointValueStored,r.maxPointValueStored,r.lives,r.bombs,r.lifeFragments,r.bombFragments,r.rank,r.difficulty)) return false;
  auto& i=s.inventory;
  if (!c.Array(s.graze)||!c.Array(s.misses)||!c.Array(i.colors)||!c.Values(i.count,i.displayState,i.gateTimer,i.combination,i.rotation,i.nextHudSequence,i.nextOverlayId)) return false;
  if(s.version>=2&&(!c.Array(s.x)||!c.Array(s.y)||!c.Array(s.focused)))return false;
  uint32_t count=uint32_t(i.hudEvents.size());
  if(!c.Value(count)||count>64) return false;
  i.hudEvents.resize(count);
  for(auto& e:i.hudEvents) if(!c.Values(e.sequence,e.target,e.operation,e.logicalSlot,e.physicalSlot,e.script,e.layer,e.value,e.overlayId,e.immediate))return false;
  return true;
}
}
void recordStageEntry(int seed, const SessionCarry* carry) {
  StageEntry s; s.stage=stageNumber; s.seed=seed; s.ticks=tickNumber;
  s.continuation=carry!=nullptr; s.nativeGlobals=replayStageGlobals;
  if(pendingStageEntry)s.continuation=pendingStageEntry->continuation;
  s.displaySeed=displayRandom.seed; s.displayCalls=displayRandom.calls;
  s.lifePieces=lifeFragments; s.bombPieces=bombFragments; s.score=score; s.rank=teamRank;
  for (unsigned p=0;p<options.count;++p) { s.resources[p]=pilots[p].resources; s.graze[p]=pilots[p].graze; s.misses[p]=pilots[p].misses;s.x[p]=pilots[p].px;s.y[p]=pilots[p].py;s.focused[p]=pilots[p].playerFocused; }
  s.inventory=ufoManager.inventory;
  stageEntry=std::move(s);
}
std::vector<uint8_t> encodeStageEntry() {
  auto copy=stageEntry; Netplay::SpectatorScalars::Writer writer;
  if (!values(writer,copy)||!writer.Good()) return {};
  return writer.Bytes();
}
bool decodeStageEntry(const std::vector<uint8_t>& bytes,StageEntry& out) {
  StageEntry s; Netplay::SpectatorScalars::Reader reader(bytes);
  if (!values(reader,s)||!reader.Done()||(s.version!=1&&s.version!=2)||s.stage<1||s.stage>7||s.seed>65535||s.displaySeed>65535||s.lifePieces>=5||s.bombPieces>=5||s.rank<-1024||s.rank>1024||s.score<0||s.score>999999999) return false;
  for(unsigned p=0;p<3;++p)if(s.x[p]<-32768||s.x[p]>32767||s.y[p]<0||s.y[p]>65535||(s.focused[p]!=0&&s.focused[p]!=1))return false;
  for(const auto& r:s.resources) if(r.maxPower!=400||r.powerStep!=100||r.power<0||r.power>400||r.lives<-1||r.lives>9||r.bombs<0||r.bombs>9||r.difficulty<0||r.difficulty>4) return false;
  if(s.inventory.count<0||s.inventory.count>3) return false;
  for(auto color:s.inventory.colors) if(color<0||color>3) return false;
  out=std::move(s); return true;
}
bool restoreStageEntry(const StageEntry& entry) {
  if(stageNumber!=int(entry.stage)||(entry.stage==7)!=(options.difficulty==4)) return false;
  pendingStageEntry=entry; score=entry.score; teamRank=entry.rank; tickNumber=entry.ticks;
  lifeFragments=entry.lifePieces; bombFragments=entry.bombPieces;
  startGame(options.selections[0]/2,options.selections[0]%2,options.difficulty,entry.seed);
  pendingStageEntry.reset(); setFullGameMode(false); spellState.replayWallClock=true; paused=finished=false;
  fullGameFlow.entryFadeAge=entry.continuation?0:30;
  fullGameFlow.totalFrames=entry.ticks;
  return true;
}
}
