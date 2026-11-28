#include "TransientEffects.hpp"
#include <algorithm>
#include <cmath>

namespace th128 {
bool TransientEffects::setDisplaySeed(std::uint16_t seed){
    if(executing_||!effects_.empty())return false;
    displayRandom_=Random{};displayRandom_.seed=seed;return true;
}
bool TransientEffects::setGameRandom(std::function<std::uint32_t()> random){
    if(executing_||!effects_.empty())return false;
    gameRandom_=std::move(random);return true;
}
bool TransientEffects::load(const void* bytes,std::size_t size,std::uint16_t displaySeed,std::string& error) {
    error.clear();
    if(executing_||!effects_.empty()){error="effect loading requires an inactive empty sidecar";return false;}
    if(!registry_.loadBank(0,static_cast<const std::uint8_t*>(bytes),size,error))return false;
    displayRandom_=Random{};displayRandom_.seed=displaySeed;nextId_=1;return true;
}
anm::Runtime TransientEffects::runtime(Effect& effect) {
    anm::Runtime result;
    if(gameRandom_)result.gameRandom32=[this]{return gameRandom_();};
    result.displayRandom32=[this]{return displayRandom_.next32();};
    result.unsupported=[&effect](int,int,int opcode,const char* reason){
        if(effect.error.empty())effect.error="effect ANM opcode="+std::to_string(opcode)+": "+reason;
    };
    return result;
}
bool TransientEffects::spawn(int script,bullet::Position position,int layer,std::string& error) {
    error.clear();
    const bool known=(script==175&&layer==23)||((script==83||script==84)&&layer==0);
    if(executing_||!known||effects_.size()>=4096||nextId_==0||!std::isfinite(position.x)
        ||!std::isfinite(position.y)||!std::isfinite(position.z)||std::abs(position.x)>65536
        ||std::abs(position.y)>65536||std::abs(position.z)>65536) {
        error="unsupported effect request or sidecar budget";return false;
    }
    Effect candidate;candidate.id=nextId_;candidate.script=script;candidate.position=position;
    candidate.machine.control().layer=layer;
    auto context=runtime(candidate);
    executing_=true;
    bool bound=false;
    try{bound=candidate.machine.bind(registry_,0,script,context);}
    catch(...){executing_=false;error="effect animation callback threw";return false;}
    executing_=false;
    if(!bound||candidate.machine.faulted()||!candidate.error.empty()) {
        error=candidate.error.empty()?"effect ANM binding failed":candidate.error;return false;
    }
    effects_.push_back(std::move(candidate));++nextId_;return true;
}
bool TransientEffects::tick(std::string& error) {
    error.clear();
    if(executing_){error="effect tick is not reentrant";return false;}
    executing_=true;
    for(auto& effect:effects_) {
        if(!effect.error.empty()){error=effect.error;executing_=false;return false;}
        auto context=runtime(effect);
        try{effect.machine.tick(context);}
        catch(...){effect.error="effect animation callback threw";error=effect.error;executing_=false;return false;}
        if(effect.machine.faulted()||!effect.error.empty()) {
            error=effect.error.empty()?"effect animation fault":effect.error;executing_=false;return false;
        }
    }
    effects_.erase(std::remove_if(effects_.begin(),effects_.end(),[](const Effect& effect){
        return effect.machine.state().ended||effect.machine.state().deleteRequested;
    }),effects_.end());
    executing_=false;return true;
}
std::vector<EffectSnapshot> TransientEffects::snapshots() const {
    std::vector<EffectSnapshot> output;output.reserve(effects_.size());
    for(const auto& effect:effects_) {
        const auto& state=effect.machine.state();
        output.push_back({effect.id,effect.script,state.sprite,state.ticks,
            320+effect.position.x+state.x+state.engineX+state.offsetX,
            16+effect.position.y+state.y+state.engineY+state.offsetY,
            effect.position.z+state.z+state.engineZ+state.offsetZ,state});
    }
    return output;
}
std::uint32_t TransientEffects::displayRandomCalls() const{return displayRandom_.calls;}
const anm::SpriteInfo* TransientEffects::spriteInfo(int sprite) const {
    const auto bank=registry_.bank(0);
    return bank&&sprite>=0&&std::size_t(sprite)<bank->sprites.size()?&bank->sprites[std::size_t(sprite)]:nullptr;
}
}
