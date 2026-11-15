#pragma once
#include "animation/AnmScene.hpp"
#include "PercentHud.hpp"
#include <span>

namespace th128::hud {
struct Snapshot {
    std::uint32_t id=0,parent=0;
    anm::State state;
    std::optional<anm::SpriteInfo> sprite;
    float x=0,y=0,z=0;
};
class PerfectHud {
public:
    bool load(std::span<const std::uint8_t> bytes,std::string& error);
    bool boot(int charge,std::string& error);
    bool charge(int value,std::string& error);
    bool rank(int value,std::string& error);
    void score(int value){score_.update(value);}
    int displayedScore() const{return score_.value;}
    bool notice(std::string& error);
    bool motivationNotice(std::string& error);
    bool tick(std::string& error);
    std::vector<Snapshot> snapshots() const;
private:
    anm::Scene scene_;
    bool loaded_=false,booted_=false,executing_=false;
    std::string failure_;
    int rank_=-1;
    OriginalScoreDisplay score_;
    bool fail(std::string reason,std::string& error);
    bool checked(std::string& error);
    void runtime();
    bool replaceNotice(int script,std::string& error);
};
}
