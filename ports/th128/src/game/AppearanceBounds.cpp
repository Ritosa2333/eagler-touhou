#include "AppearanceBounds.hpp"
#include <bit>
#include <cmath>

namespace th128::enemy {
std::optional<Bounds> scaledSpriteBounds(float width,float height,float scaleX,float scaleY) noexcept {
    const auto bounded=[](float value){return std::isfinite(value)&&std::abs(value)<=4096;};
    if(!bounded(width)||!bounded(height)||!bounded(scaleX)||!bounded(scaleY))return std::nullopt;
    const Bounds result{std::abs(float(double(height)*scaleY)),std::abs(float(double(width)*scaleX))};
    if(!bounded(result.width)||!bounded(result.height))return std::nullopt;
    return result;
}
std::optional<Bounds> constantSpriteBounds(const anm::Bank& bank,int script) {
    if(script<0||std::size_t(script)>=bank.scripts.size())return std::nullopt;
    const auto& program=bank.scripts[std::size_t(script)];
    if(program.commands.empty()||program.commands.size()>4096||program.commands.front().time!=0)return std::nullopt;
    std::optional<Bounds> invariant;
    for(const auto& command:program.commands) {
        if(command.mask!=0)return std::nullopt;
        if(command.op==3) {
            if(command.words.size()!=1)return std::nullopt;
            if(!invariant&&command.time!=0)return std::nullopt;
            const auto sprite=std::bit_cast<std::int32_t>(command.words[0]);
            if(sprite<0||std::size_t(sprite)>=bank.sprites.size())return std::nullopt;
            const auto& info=bank.sprites[std::size_t(sprite)];
            const auto metrics=scaledSpriteBounds(info.width,info.height,1,1);
            if(!metrics||info.width<=0||info.height<=0)return std::nullopt;
            if(invariant&&(invariant->width!=metrics->width||invariant->height!=metrics->height))return std::nullopt;
            invariant=*metrics;
        } else if(command.op==68&&command.words.size()==1) {
            const auto layer=std::bit_cast<std::int32_t>(command.words[0]);
            if(layer<0||layer>31)return std::nullopt;
        } else if(command.op==58&&command.words.size()==3) {
            const auto duration=std::bit_cast<std::int32_t>(command.words[0]),mode=std::bit_cast<std::int32_t>(command.words[1]);
            if(duration<0||duration>65535||mode<0||mode>16)return std::nullopt;
        } else if(command.op==63&&command.words.empty()) {
            if(!invariant)return std::nullopt;
        } else if(command.op==61&&command.words.empty()) {
            continue;
        } else if(command.op==56&&command.words.size()==5) {
            const auto duration=std::bit_cast<std::int32_t>(command.words[0]),mode=std::bit_cast<std::int32_t>(command.words[1]);
            if(!invariant||duration<0||duration>65535||mode<0||mode>16)return std::nullopt;
            for(std::size_t index=2;index<5;++index)if(!std::isfinite(std::bit_cast<float>(command.words[index])))return std::nullopt;
        } else if(command.op==4) {
            if(!invariant||command.words.size()!=2)return std::nullopt;
            const auto found=program.offsets.find(command.words[0]);
            if(found==program.offsets.end()||found->second>=program.commands.size())return std::nullopt;
            const auto& target=program.commands[found->second];
            const auto clock=std::bit_cast<std::int32_t>(command.words[1]);
            if(target.offset>=command.offset||target.time>=command.time
                ||clock<0||clock>target.time)return std::nullopt;
        } else if(command.op==-1 && &command==&program.commands.back()&&command.words.empty()
            &&program.commands.size()>1&&(program.commands[program.commands.size()-2].op==4||program.commands[program.commands.size()-2].op==63)) {
            continue;
        } else {
            return std::nullopt;
        }
    }
    return invariant;
}
}
