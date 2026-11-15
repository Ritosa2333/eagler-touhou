#include "ItemRules.hpp"
#include <cmath>
#include <algorithm>
#include <cstdint>
namespace th128::item {
namespace {
bool valid(Position position){return std::isfinite(position.x)&&std::isfinite(position.y)&&std::isfinite(position.z)&&
    std::abs(position.x)<=4096&&std::abs(position.y)<=4096&&std::abs(position.z)<=4096;}
}
std::optional<MotionResult> ordinaryMotion(const MotionInput& input) noexcept {
    if(!valid(input.position)||!valid(input.velocity)||!std::isfinite(input.playerY)||input.playerState<0||input.playerState>4)return std::nullopt;
    MotionResult result{MotionContinuation::Contact,input.position,input.velocity};
    if(input.playerState!=2&&input.playerState!=4&&input.playerY<128){result.continuation=MotionContinuation::AutoAttraction;return result;}
    result.position={float(double(input.position.x)+input.velocity.x),float(double(input.position.y)+input.velocity.y),float(double(input.position.z)+input.velocity.z)};
    result.velocity.y=std::min(float(double(input.velocity.y)+0.029999999329447746),2.f);
    if(result.velocity.y>=0)result.velocity.x=0;
    if(result.position.y>472)result.continuation=MotionContinuation::Removed;
    return result;
}
std::optional<Contact> contactDecision(Position item,Position player,float collectFullSize,float attractHalfSize,bool focused) noexcept {
    if(!valid(item)||!valid(player)||!std::isfinite(collectFullSize)||!std::isfinite(attractHalfSize)||
       collectFullSize<0||collectFullSize>4096||attractHalfSize<0||attractHalfSize>4096)return std::nullopt;
    const float half=collectFullSize*.5f;
    const float minimumX=float(double(player.x)-half),maximumX=float(double(player.x)+half);
    const float minimumY=float(double(player.y)-half),maximumY=float(double(player.y)+half);
    if(item.x>=minimumX&&item.x<=maximumX&&item.y>=minimumY&&item.y<=maximumY)return Contact::Collect;
    if(item.x>=float(double(player.x)-attractHalfSize)&&item.x<=float(double(player.x)+attractHalfSize)&&
       item.y>=float(double(player.y)-attractHalfSize)&&item.y<=float(double(player.y)+attractHalfSize))return Contact::Attract;
    if(focused)return Contact::None;
    return Contact::None;
}
std::optional<int> pointReward(int pointValue,float playerY,int state,int kind) noexcept {
    if(pointValue<0||pointValue>100000000||!std::isfinite(playerY)||playerY<0||playerY>4096||(kind!=2&&kind!=4))return std::nullopt;
    const int base=pointValue/100-(pointValue/100)%10;
    int reward=base;
    if(playerY>128&&state!=3){
        const int scaled=base*3/4;
        const auto reduction=std::int64_t(-scaled)*(int(playerY)-128)/450;
        reward=int(scaled+reduction);
    }
    reward=std::max(10,reward/10*10);
    return kind==4?reward*4:reward;
}
}
