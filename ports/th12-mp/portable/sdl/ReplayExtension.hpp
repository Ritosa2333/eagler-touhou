#pragma once
#include "Platform.hpp"
#include <array>
#include <map>
#include <optional>

namespace th12host {
struct ReplayMotion {u32 frame=0;i32 x=0,y=0;};
struct ReplayToolEvent {u32 frame=0;std::array<i32,3> state{};};
struct ReplayExtension {
    bool present=false,unlimited=false,assisted=false;
    std::optional<std::array<i32,20>> practice;
    std::map<i32,std::vector<ReplayMotion>> motion;
    std::map<i32,std::vector<i32>> damage;
    std::map<i32,std::vector<ReplayToolEvent>> tools;
    std::array<i32,3> tool_state(i32 stage,u32 frame)const;
    std::array<i32,2> delta(i32 stage,u32 frame)const;
};
// TH12WEB1 + byte length + CRC32 + UTF-8 JSON. Identical to replay-motion.mjs.
bool read_replay_extension(const std::vector<u8>& file,size_t offset,
    const std::map<i32,u32>& stages,ReplayExtension& out,std::string& error);
void append_replay_extension(std::vector<u8>& file,const ReplayExtension& extension);
}
