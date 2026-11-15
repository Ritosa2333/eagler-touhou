#pragma once
#include "../game/GameConfig.hpp"
#include "../game/RecordStore.hpp"
#include "../game/ReplayCatalog.hpp"
#include "../game/ReplayRecording.hpp"
namespace th17::sdl {
class FileStore {
 std::string directory;std::vector<u8> saved_score,saved_config;
 bool read(const std::string&,std::vector<u8>&,u32,bool&);bool write(const std::string&,const u8*,u32);static bool filename(const std::string&)noexcept;
public:
 std::string error;explicit FileStore(std::string root="/save"):directory(std::move(root)){}
 bool initialize(RecordStore&,GameConfig&,Rng&);bool save(RecordStore&,const GameConfig&);
 bool replay(const std::string&,std::shared_ptr<Replay>&);bool replay_slot(i32,std::shared_ptr<Replay>&);
 bool save_replay(i32,ReplayRecording&,const std::array<char,9>&,const ReplayExportDetails&);bool catalog(ReplayCatalog&);
};
}
