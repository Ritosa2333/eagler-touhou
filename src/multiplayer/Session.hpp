#pragma once
#include "NetplayRuntime.hpp"
#include "ReplayArchive.hpp"
#include "WorldState.hpp"
#include "StageEntry.hpp"
#include "../game/SoundQueue.hpp"
#include <optional>
namespace th12::multiplayer {
class Session {
  struct Record {
    uint32_t frame, stage;
    std::optional<WorldState> before;
    std::array<Netplay::FrameInput,3> inputs;
    std::vector<uint8_t> initializer;
    std::vector<sound_system::Event> sounds;
  };
  std::deque<Record> records_;
  ReplayArchive replay_;
  ReplayArchive playback_;
  unsigned pendingStage_=0;
  uint32_t committed_ = 0, restores_ = 0, resimulated_ = 0, checkpoints_ = 0;
  bool started_ = false, loading_ = false;
  std::string error_;
  std::vector<sound_system::Event> audio_;
  bool fail(const char*);
  bool simulate(const Netplay::FrameDecision&);
  bool commit();
  bool spectatorStep();
  bool replayStep();
public:
  NetplayRuntime network;
  bool configure(const uint32_t*,unsigned);
  bool start();
  bool pump();
  bool step(uint32_t held,bool pause,int motion,float x,float y,double nowMs);
  bool stageNeeded() const;
  unsigned nextStage() const;
  bool beginStageLoad();
  bool stageLoaded();
  void stop();
  const char* error() const;
  const std::vector<sound_system::Event>& audio() const {return audio_;}
  void clearAudio(){audio_.clear();}
  bool replayBytes(std::vector<uint8_t>&,const char* name);
  bool loadReplay(const uint8_t*,size_t,const uint32_t* build);
  bool seekReplay(uint32_t frame);
  bool restart();
  uint32_t replayFrames()const{return playback_.FrameCount();}
  uint32_t committed() const{return committed_;}
  uint32_t restores() const{return restores_;}
  uint32_t resimulated() const{return resimulated_;}
  uint32_t checkpoints() const{return checkpoints_;}
  bool settled() const{return network.InputsReconciledBeforeNext()&&!network.Correcting();}
};
extern Session session;
}
