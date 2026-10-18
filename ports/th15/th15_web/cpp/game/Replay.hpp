#pragma once
#include "Types.hpp"
#include "PlayerTouch.hpp"
#include <array>
#include <string>
#include <vector>
namespace th15 {
struct ReplayInput {u16 held=0,pressed=0,released=0;u8 fps=0;bool end=false;PlayerTouch touch;};
struct ReplayTouch {u32 frame=0;PlayerTouch motion;};
struct ReplayStage {u16 number=0,seed=0;u32 offset=0,frames=0,payload_size=0;};
// Original t15r version 3. A tick stores held/pressed/released independently.
// Stage headers preserve the original chapter/checkpoint state for restoration.
class Replay {
    std::vector<u8> bytes;std::array<ReplayStage,8> stages{};std::array<std::vector<ReplayTouch>,8> touches;u32 count=0,selected=0,cursor=0,touch_cursor=0;ReplayInput input{};std::string failure;
public:
    bool open(const u8*,u32);bool select(u32);ReplayInput tick(bool active=true);
    const std::vector<u8>& decoded()const{return bytes;}
    const ReplayStage* stage(u32)const;const u8* header(u32)const;
    u32 stage_count()const{return count;}u32 frame()const{return cursor;}
    u32 character()const;u32 difficulty()const;u32 mode_flags()const;
    bool uses_touch()const noexcept{for(const auto& stream:touches)if(!stream.empty())return true;return false;}
    const std::string& error()const{return failure;}
};
}
