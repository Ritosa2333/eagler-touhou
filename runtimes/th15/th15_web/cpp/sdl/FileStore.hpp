#pragma once
#include "../game/GameConfig.hpp"
#include "../game/RecordStore.hpp"
#include "../game/ReplayCatalog.hpp"
#include "../game/ReplayRecording.hpp"
#include "../game/CheckpointFile.hpp"
namespace th15::sdl {
// Files live in the player's mounted save directory. Native codecs stay in
// the game layer; the browser launcher owns IDBFS synchronization and imports.
class FileStore {
 std::string directory;std::vector<u8> saved_score,saved_config;
 bool read(const std::string&,std::vector<u8>&,u32 limit,bool& present);bool write(const std::string&,const u8*,u32);
 static bool filename(const std::string&)noexcept;
public:
 std::string error;explicit FileStore(std::string root="/save"):directory(std::move(root)){}
 bool initialize(RecordStore&,GameConfig&,Rng&);bool save(RecordStore&,const GameConfig&);
 bool replay(const std::string&,std::shared_ptr<Replay>&);bool replay_slot(i32,std::shared_ptr<Replay>&);
 bool save_replay(i32,ReplayRecording&,const std::array<char,9>&,const ReplayExportDetails&);
 bool catalog(ReplayCatalog&);bool remove_checkpoint(i32 character,i32 difficulty);
 bool checkpoint(i32 character,i32 difficulty,std::vector<u8>&,bool& present);
 bool checkpoint_header(i32 character,i32 difficulty,CheckpointHeader&,bool& present);
 bool save_checkpoint(i32 character,i32 difficulty,const std::vector<u8>&);
};
}
