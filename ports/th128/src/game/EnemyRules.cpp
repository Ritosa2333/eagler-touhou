#include "EnemyRules.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>

namespace th128::enemy {
std::optional<HitFeedbackPlan> planHitFeedback(const HitFeedbackInput& input,
    std::uint32_t primaryFlags,std::uint32_t primaryColor) noexcept {
    HitFeedbackPlan next{input.counter,primaryFlags,primaryColor,std::nullopt};
    if(!input.primaryPresent)return next;
    if(input.counter!=0){
        next.counter=std::bit_cast<std::int32_t>(std::uint32_t(input.counter)-1u);
        next.primaryFlags&=~0x30000u;return next;
    }
    if(input.auxiliaryFlags&1u){
        if(input.phaseClock%4==0){next.primaryFlags=(next.primaryFlags&~0x20000u)|0x10000u;next.primaryColor=0xffff00ffu;}
        else next.primaryFlags&=~0x30000u;
    }
    if(input.enemyFlags&0x400000u){
        next.primaryFlags=(next.primaryFlags&~0x20000u)|0x10000u;next.primaryColor=0xff0000ffu;next.counter=4;
        std::int32_t identifier=input.soundOverride;
        if(identifier<0){
            identifier=35;
            if(input.enemyFlags&0x81000000u){
                if(!input.phaseFlags)return std::nullopt;
                const bool active=(*input.phaseFlags&1u)!=0;
                if(!(active&&(*input.phaseFlags&0x10u))&&input.lifeWord500c<(active?200:900))identifier=36;
            }
        }
        next.sound=identifier;return next;
    }
    if(input.phaseClock%4!=0){next.primaryFlags&=~0x30000u;return next;}
    if(input.enemyFlags&0x81000000u){
        if(!input.phaseFlags)return std::nullopt;
        const bool active=(*input.phaseFlags&1u)!=0;
        if(!(active&&(*input.phaseFlags&0x10u))&&input.lifeWord500c<(active?100:500)){
            next.primaryFlags=(next.primaryFlags&~0x20000u)|0x10000u;next.primaryColor=0xff0000ffu;
        }
    }
    return next;
}

bool bossWaitPending(const BossSlots& manager) noexcept {
    return manager.owners[0]!=0||manager.owners[1]!=0||manager.owners[2]!=0;
}
namespace {

float normalizeOnce(float angle) noexcept {
    std::int32_t iterations = 0;
    while (angle > Pi) {
        angle = static_cast<float>(static_cast<double>(angle) - static_cast<double>(Tau));
        if (iterations++ > 32) {
            break;
        }
    }
    while (angle < -Pi) {
        angle = static_cast<float>(static_cast<double>(angle) + static_cast<double>(Tau));
        if (iterations++ > 32) {
            break;
        }
    }
    return angle;
}

float normalizeTwice(float angle) noexcept {
    return normalizeOnce(normalizeOnce(angle));
}

float mirrorAngle(float angle) noexcept {
    constexpr float halfPi = 1.5707963705062866f;
    const float difference = static_cast<float>(static_cast<double>(angle) - static_cast<double>(halfPi));
    return normalizeOnce(static_cast<float>(static_cast<double>(halfPi) - normalizeOnce(difference)));
}

bool finite(const Polar& polar) noexcept {
    return std::isfinite(polar.angle) && std::isfinite(polar.speed);
}

bool finite(const Position& position) noexcept {
    return std::isfinite(position.x) && std::isfinite(position.y) && std::isfinite(position.z);
}

bool quantize(Position& position) noexcept {
    const float scaledX = static_cast<float>(static_cast<double>(position.x) * 100.0);
    const float scaledY = static_cast<float>(static_cast<double>(position.y) * 100.0);
    if (!finite(position) || !std::isfinite(scaledX) || !std::isfinite(scaledY)) {
        return false;
    }
    const float floorX = static_cast<float>(std::floor(static_cast<double>(scaledX)));
    const float floorY = static_cast<float>(std::floor(static_cast<double>(scaledY)));
    position.x = static_cast<float>(static_cast<double>(floorX) / 100.0);
    position.y = static_cast<float>(static_cast<double>(floorY) / 100.0);
    return true;
}

float interpolate(float start, float target, float ratio) noexcept {
    const float difference = static_cast<float>(static_cast<double>(target) - static_cast<double>(start));
    const float scaled = static_cast<float>(static_cast<double>(difference) * static_cast<double>(ratio));
    return static_cast<float>(static_cast<double>(start) + static_cast<double>(scaled));
}

}

FlagsChange setFlags(std::uint32_t flags, std::uint32_t mask) noexcept {
    const std::uint32_t result = flags | mask;
    return {result, result & 0x20u ? VisibilityEffect::Hide : VisibilityEffect::None};
}

FlagsChange clearFlags(std::uint32_t flags, std::uint32_t mask) noexcept {
    const std::uint32_t result = flags & ~mask;
    return {result, result & 0x20u ? VisibilityEffect::None : VisibilityEffect::Show};
}

bool setBounds(Bounds& bounds, float width, float height) noexcept {
    if (!std::isfinite(width) || !std::isfinite(height)) {
        return false;
    }
    bounds = {width, height};
    return true;
}

bool setBossGauge(BossGaugeState& state,int boss,int slot,float life,std::int32_t maximum,std::uint32_t color) noexcept {
    if(boss<0||boss>=int(state.values.size())||slot<0||slot>=int(state.values[0].size())||maximum<=0||!std::isfinite(life))return false;
    const auto ratio=float(double(life)/double(maximum));
    if(!std::isfinite(ratio))return false;
    state.values[std::size_t(boss)][std::size_t(slot)]={ratio,color,true};return true;
}
void clearUnownedBossGauges(BossGaugeState& state,const BossSlots& owners) noexcept {
    for(std::size_t boss=0;boss<3;++boss)if(!owners.owners[boss])for(auto& segment:state.values[boss])segment.ratio=0;
}

bool setBossSlot(BossSlots& manager,std::uint32_t& flags,std::int32_t& retainedSlot,
                 std::uint32_t owner,std::int32_t slot) noexcept {
    if(slot>=int(manager.owners.size())||(slot>=0&&owner==0)
        ||(slot<0&&(flags&0x1000000u)&&(retainedSlot<0||retainedSlot>=int(manager.owners.size()))))return false;
    manager.flags&=~1u;
    if(slot<0){
        if(flags&0x1000000u)manager.owners[std::size_t(retainedSlot)]=0;
        flags&=~0x1000000u;
    }else{
        manager.owners[std::size_t(slot)]=owner;
        flags|=0x1000000u;retainedSlot=slot;
    }
    return true;
}

void setLife(std::array<std::uint32_t,6>& words,std::uint32_t& flags,std::int32_t life) noexcept {
    const auto value=std::uint32_t(life);
    words[0]=words[1]=words[2]=value;words[3]=value*7u;
    if(flags&0x1000000u)flags|=0x80000000u;
}

bool setPolar(Polar& polar, float angle, float speed, std::uint32_t enemyFlags) noexcept {
    if (!finite(polar) || !std::isfinite(angle) || !std::isfinite(speed)) {
        return false;
    }
    if (angle > UnchangedPolar) {
        polar.angle = normalizeTwice(enemyFlags & 0x100000u ? mirrorAngle(angle) : angle);
    }
    if (speed > UnchangedPolar) {
        polar.speed = speed;
    }
    polar.modeFlags &= ~3u;
    return true;
}

bool beginPolarInterpolation(Polar& polar, PolarInterpolation& interpolation,
                             std::int32_t duration, std::int32_t mode,
                             float targetAngle, float targetSpeed,
                             std::uint32_t enemyFlags) noexcept {
    if (duration <= 0) {
        interpolation.duration = 0;
        return true;
    }
    if ((mode != 0&&mode!=1&&mode!=7&&mode!=9) || !finite(polar) || !std::isfinite(targetAngle) || !std::isfinite(targetSpeed)) {
        return false;
    }
    float startAngle = polar.angle;
    if (targetAngle <= UnchangedPolar) {
        targetAngle = mode==7?0.f:polar.angle;
    } else if (mode!=7&&(enemyFlags & 0x100000u)) {
        targetAngle = mirrorAngle(targetAngle);
    }
    if(mode==7)targetSpeed=std::max(targetSpeed,0.f);
    else if (targetSpeed <= UnchangedPolar) {
        targetSpeed = polar.speed;
    }
    const float difference = static_cast<float>(static_cast<double>(startAngle) - static_cast<double>(targetAngle));
    if (!std::isfinite(difference)) {
        return false;
    }
    if (std::abs(difference) >= Pi) {
        if (targetAngle > startAngle) {
            startAngle = static_cast<float>(static_cast<double>(startAngle) + static_cast<double>(Tau));
        } else {
            targetAngle = static_cast<float>(static_cast<double>(targetAngle) + static_cast<double>(Tau));
        }
    }
    interpolation = {startAngle, polar.speed, targetAngle, targetSpeed, duration, 0, 0,mode};
    polar.modeFlags &= ~3u;
    return true;
}

bool stepPolarInterpolation(Polar& polar, PolarInterpolation& interpolation) noexcept {
    if (interpolation.duration == 0) {
        return true;
    }
    if (interpolation.duration < 0 || interpolation.elapsed < 0 || interpolation.elapsed >= interpolation.duration
        ||(interpolation.mode!=0&&interpolation.mode!=1&&interpolation.mode!=4&&interpolation.mode!=7&&interpolation.mode!=9&&!(interpolation.circular&&interpolation.mode==2))
        || !finite(polar) || !std::isfinite(interpolation.startAngle) || !std::isfinite(interpolation.startSpeed)
        || !std::isfinite(interpolation.targetAngle) || !std::isfinite(interpolation.targetSpeed)
        || !std::isfinite(interpolation.elapsedFloat) || interpolation.elapsedFloat < 0) {
        return false;
    }
    const std::int32_t elapsed = interpolation.elapsed + 1;
    const float elapsedFloat = static_cast<float>(static_cast<double>(interpolation.elapsedFloat) + 1.0);
    float angle = interpolation.targetAngle;
    float speed = interpolation.targetSpeed;
    float radius=interpolation.targetRadius,radialSpeed=interpolation.targetRadialSpeed;
    if(interpolation.mode==7){
        angle=interpolation.startAngle;speed=interpolation.startSpeed;
        if(elapsed<interpolation.duration){
            angle=float(double(angle)+interpolation.targetAngle);
            speed=float(double(speed)+interpolation.targetSpeed);
        }
    }else if (elapsed < interpolation.duration) {
        float ratio = static_cast<float>(static_cast<double>(elapsedFloat) / interpolation.duration);
        if(interpolation.mode==1)ratio=float(double(ratio)*double(ratio));
        if(interpolation.mode==2)ratio=float(double(ratio)*double(ratio)*double(ratio));
        if(interpolation.mode==4)ratio=float(1.0-(1.0-double(ratio))*(1.0-double(ratio)));
        if(interpolation.mode==9){
            const float twice=float(double(ratio)*2.0);
            if(twice<1.0f)ratio=float(0.5*(double(twice)*double(twice)));
            else{
                const float complement=float(2.0-double(twice));
                ratio=float(0.5*(2.0-(double(complement)*double(complement))));
            }
        }
        angle = interpolate(interpolation.startAngle, interpolation.targetAngle, ratio);
        speed = interpolate(interpolation.startSpeed, interpolation.targetSpeed, ratio);
        if(interpolation.circular){
            radius=interpolate(interpolation.startRadius,interpolation.targetRadius,ratio);
            radialSpeed=interpolate(interpolation.startRadialSpeed,interpolation.targetRadialSpeed,ratio);
        }
    }
    if (!std::isfinite(angle) || !std::isfinite(speed)) {
        return false;
    }
    if (!(polar.modeFlags & 3u)) {
        polar.angle = normalizeTwice(angle);
    }
    polar.speed = speed;
    if(interpolation.circular){
        if((polar.modeFlags&3u)!=1u||!std::isfinite(radius)||!std::isfinite(radialSpeed))return false;
        polar.radius=radius;polar.radialSpeed=radialSpeed;
    }
    if(interpolation.mode==7){interpolation.startAngle=angle;interpolation.startSpeed=speed;}
    interpolation.previous=interpolation.elapsed;
    interpolation.elapsed = elapsed;
    interpolation.elapsedFloat = elapsedFloat;
    if (elapsed >= interpolation.duration) {
        interpolation.duration = 0;
    }
    return true;
}

namespace {
bool projectCircular(Position& position,const Polar& polar) noexcept {
    if((polar.modeFlags&3u)!=1u||!finite(polar)||!finite(polar.origin)
        ||!std::isfinite(polar.radius)||!std::isfinite(polar.radialSpeed))return false;
    const float offsetX=float(std::cos(double(polar.angle))*double(polar.radius));
    const float offsetY=float(std::sin(double(polar.angle))*double(polar.radius));
    Position next{float(double(polar.origin.x)+offsetX),float(double(polar.origin.y)+offsetY),polar.origin.z};
    if(!finite(next)||!quantize(next))return false;
    position=next;return true;
}
}

bool beginCircularMotion(Position& position,Polar& polar,float angle,float angularSpeed,float radius,float radialSpeed) noexcept {
    if(!finite(position)||!finite(polar)||!std::isfinite(angle)||!std::isfinite(angularSpeed)
        ||!std::isfinite(radius)||!std::isfinite(radialSpeed)||(polar.modeFlags&2u))return false;
    auto next=polar;
    if(!(next.modeFlags&1u))next.origin=position;
    if(angle>UnchangedPolar)next.angle=normalizeTwice(angle);
    if(angularSpeed>UnchangedPolar)next.speed=angularSpeed;
    if(radius>UnchangedPolar)next.radius=radius;
    if(radialSpeed>UnchangedPolar)next.radialSpeed=radialSpeed;
    next.modeFlags|=1u;Position projected=position;
    if(!projectCircular(projected,next))return false;
    polar=next;position=projected;return true;
}

bool beginCircularInterpolation(Polar& polar,PolarInterpolation& interpolation,std::int32_t duration,std::int32_t mode,
                                float angularSpeed,float radius,float radialSpeed) noexcept {
    if((mode!=0&&mode!=2&&mode!=9)||(polar.modeFlags&3u)!=1u||!finite(polar)||!std::isfinite(angularSpeed)
        ||!std::isfinite(radius)||!std::isfinite(radialSpeed))return false;
    if(duration<=0){interpolation.duration=0;return true;}
    const float targetSpeed=angularSpeed>UnchangedPolar?angularSpeed:polar.speed;
    const float targetRadius=radius>UnchangedPolar?radius:polar.radius;
    const float targetRadial=radialSpeed>UnchangedPolar?radialSpeed:polar.radialSpeed;
    PolarInterpolation next{0,polar.speed,0,targetSpeed,duration,0,0};next.circular=true;next.mode=mode;
    next.startRadius=polar.radius;next.startRadialSpeed=polar.radialSpeed;
    next.targetRadius=targetRadius;next.targetRadialSpeed=targetRadial;
    interpolation=next;return true;
}

bool advanceCircularPosition(Position& position,Position& velocity,Polar& polar) noexcept {
    if(&position==&velocity||!finite(position)||!finite(polar)||(polar.modeFlags&3u)!=1u)return false;
    auto next=polar;next.radius=float(double(next.radialSpeed)+next.radius);
    next.angle=normalizeTwice(float(double(next.speed)+next.angle));Position projected=position;
    if(!projectCircular(projected,next))return false;
    const Position nextVelocity{float(double(projected.x)-position.x),float(double(projected.y)-position.y),float(double(projected.z)-position.z)};
    if(!finite(nextVelocity))return false;
    polar=next;position=projected;velocity=nextVelocity;return true;
}

void setAnimationBank(AnimationState& animation, std::int32_t bank) noexcept {
    animation.bank = bank;
}

std::optional<AnimationRequest> animationSlot(const AnimationState& animation,
                                              std::int32_t slot, std::int32_t script,
                                              std::uint32_t enemyFlags) noexcept {
    if (slot < 0 || slot >= static_cast<std::int32_t>(AnimationSlotCount) || script < 0 || animation.bank < 0
        || animation.layerOffset > std::numeric_limits<std::int32_t>::max() - 7) {
        return std::nullopt;
    }
    return AnimationRequest{animation.bank, slot, script, animation.layerOffset + 7,
                            enemyFlags & 0x20u ? VisibilityEffect::Hide : VisibilityEffect::None, slot == 0};
}

bool beginPositionInterpolation(PositionInterpolation& interpolation,const Position& position,
                                std::int32_t duration,std::int32_t mode,float x,float y) noexcept {
    if(!finite(position)||!std::isfinite(x)||!std::isfinite(y))return false;
    if(duration<=0){interpolation.duration=0;return true;}
    if(mode!=1&&mode!=4&&mode!=9&&mode!=11)return false;
    interpolation={position,{x>UnchangedPolar?x:position.x,y>UnchangedPolar?y:position.y,0},duration,mode,-1,0,0};return true;
}

std::optional<Position> stepPositionInterpolation(PositionInterpolation& interpolation) noexcept {
    if((interpolation.mode!=1&&interpolation.mode!=4&&interpolation.mode!=9&&interpolation.mode!=11)||interpolation.duration<=0||interpolation.elapsed<0
        ||interpolation.elapsed>interpolation.duration||!std::isfinite(interpolation.elapsedFloat)
        ||!finite(interpolation.start)||!finite(interpolation.target))return std::nullopt;
    if(interpolation.elapsed==std::numeric_limits<std::int32_t>::max())return std::nullopt;
    interpolation.previous=interpolation.elapsed;++interpolation.elapsed;
    interpolation.elapsedFloat=float(double(interpolation.elapsedFloat)+1.0);
    if(interpolation.elapsed>=interpolation.duration){
        interpolation.elapsed=interpolation.duration;interpolation.previous=interpolation.duration-1;
        interpolation.elapsedFloat=float(interpolation.duration);interpolation.duration=0;return interpolation.target;
    }
    const auto ratio=float(double(interpolation.elapsedFloat)/double(interpolation.duration));
    float eased=0;
    if(interpolation.mode==1)eased=float(double(ratio)*double(ratio));
    else if(interpolation.mode==4){
        eased=float(1.0-(1.0-double(ratio))*(1.0-double(ratio)));
    }else if(interpolation.mode==9){
        const float twice=float(double(ratio)*2.0);
        if(twice<1.0f){
            eased=float(0.5*(double(twice)*double(twice)));
        }else{
            const float complement=float(2.0-double(twice));
            eased=float(0.5*(2.0-(double(complement)*double(complement))));
        }
    }else if(interpolation.mode==11){
        if(ratio<0.5f){
            const double square=double(ratio)*double(ratio);
            eased=float(8.0*square*square);
        }else{
            const double complement=1.0-double(ratio);
            eased=float(1.0-8.0*complement*complement*complement*complement);
        }
    }
    const auto value=[eased](float start,float target){const auto difference=float(double(target)-double(start));
        const auto scaled=float(double(difference)*double(eased));return float(double(start)+double(scaled));};
    const Position result{value(interpolation.start.x,interpolation.target.x),value(interpolation.start.y,interpolation.target.y),value(interpolation.start.z,interpolation.target.z)};
    if(!finite(result))return std::nullopt;
    return result;
}

bool advanceInterpolatedPosition(Position& position,Position& velocity,PositionInterpolation& interpolation) noexcept {
    if(&position==&velocity||!finite(position))return false;
    auto candidate=interpolation;const auto desired=stepPositionInterpolation(candidate);if(!desired)return false;
    const Position nextVelocity{float(double(desired->x)-double(position.x)),float(double(desired->y)-double(position.y)),float(double(desired->z)-double(position.z))};
    Position next{float(double(position.x)+double(nextVelocity.x)),float(double(position.y)+double(nextVelocity.y)),float(double(position.z)+double(nextVelocity.z))};
    if(!finite(nextVelocity)||!quantize(next))return false;
    interpolation=candidate;position=next;velocity=nextVelocity;return true;
}

bool advancePolarPosition(Position& position, Position& velocity, const Polar& polar) noexcept {
    if (&position == &velocity || !finite(position) || !finite(polar) || (polar.modeFlags & 3u)
        || std::abs(polar.angle)>2048.f) {
        return false;
    }
    const Position nextVelocity{
        static_cast<float>(std::cos(static_cast<double>(polar.angle)) * static_cast<double>(polar.speed)),
        static_cast<float>(std::sin(static_cast<double>(polar.angle)) * static_cast<double>(polar.speed)), 0};
    Position nextPosition{
        static_cast<float>(static_cast<double>(position.x) + static_cast<double>(nextVelocity.x)),
        static_cast<float>(static_cast<double>(position.y) + static_cast<double>(nextVelocity.y)),
        static_cast<float>(static_cast<double>(position.z) + static_cast<double>(nextVelocity.z))};
    if (!finite(nextVelocity) || !quantize(nextPosition)) {
        return false;
    }
    position = nextPosition;
    velocity = nextVelocity;
    return true;
}

bool combinePosition(Position& combined, Position& velocity, const Position& absolute,
                     const Position& relative, std::uint32_t enemyFlags) noexcept {
    if (&combined == &velocity || !finite(combined) || !finite(absolute) || !finite(relative)
        || (enemyFlags & (0x00040000u | 0x08000000u))) {
        return false;
    }
    const Position desired{
        static_cast<float>(static_cast<double>(absolute.x) + static_cast<double>(relative.x)),
        static_cast<float>(static_cast<double>(absolute.y) + static_cast<double>(relative.y)),
        static_cast<float>(static_cast<double>(absolute.z) + static_cast<double>(relative.z))};
    const Position nextVelocity{
        static_cast<float>(static_cast<double>(desired.x) - static_cast<double>(combined.x)),
        static_cast<float>(static_cast<double>(desired.y) - static_cast<double>(combined.y)),
        static_cast<float>(static_cast<double>(desired.z) - static_cast<double>(combined.z))};
    Position nextPosition{
        static_cast<float>(static_cast<double>(combined.x) + static_cast<double>(nextVelocity.x)),
        static_cast<float>(static_cast<double>(combined.y) + static_cast<double>(nextVelocity.y)),
        static_cast<float>(static_cast<double>(combined.z) + static_cast<double>(nextVelocity.z))};
    if (!finite(desired) || !finite(nextVelocity) || !quantize(nextPosition)) {
        return false;
    }
    combined = nextPosition;
    velocity = nextVelocity;
    return true;
}

std::optional<MovementArea> prepareMovementArea(float centerX,float centerY,float width,float height) noexcept {
    for(const float value:{centerX,centerY,width,height})if(!std::isfinite(value)||std::abs(value)>65536)return std::nullopt;
    return MovementArea{centerX,centerY,width,height};
}
bool combinePositionWithArea(Position& combined,Position& velocity,Position& absolute,const Position& relative,
                             const MovementArea& area) noexcept {
    if(&combined==&velocity||&combined==&absolute||&velocity==&absolute||&relative==&combined
        ||&relative==&velocity||&relative==&absolute||!prepareMovementArea(area.centerX,area.centerY,area.width,area.height))return false;
    Position nextCombined=combined,nextVelocity;
    if(!combinePosition(nextCombined,nextVelocity,absolute,relative))return false;
    const double halfWidth=double(area.width)*0.5,halfHeight=double(area.height)*0.5;
    const double left=double(area.centerX)-halfWidth,right=double(area.centerX)+halfWidth;
    const double top=double(area.centerY)-halfHeight,bottom=double(area.centerY)+halfHeight;
    if(double(nextCombined.x)<left)nextCombined.x=float(left);
    else if(double(nextCombined.x)>right)nextCombined.x=float(right);
    if(double(nextCombined.y)<top)nextCombined.y=float(top);
    else if(double(nextCombined.y)>bottom)nextCombined.y=float(bottom);
    const Position nextAbsolute{float(double(nextCombined.x)-relative.x),float(double(nextCombined.y)-relative.y),
        float(double(nextCombined.z)-relative.z)};
    if(!finite(nextCombined)||!finite(nextVelocity)||!finite(nextAbsolute))return false;
    combined=nextCombined;velocity=nextVelocity;absolute=nextAbsolute;return true;
}

std::optional<SpawnRequest> prepareSpawn(std::uint16_t opcode, std::string_view subroutine,
                                        const SpawnParent& parent, float x, float y,
                                        std::int32_t life, std::int32_t score, std::int32_t drop) noexcept {
    if ((opcode != 256 && opcode != 257 && opcode != 260&&opcode!=261) || subroutine.empty() || subroutine.size() > 255
        || subroutine.find('\0') != std::string_view::npos || !std::isfinite(x) || !std::isfinite(y)
        || (opcode == 256 && (parent.flags & 0x08000000u))) {
        return std::nullopt;
    }
    if (opcode != 257&&opcode!=261) {
        if (!std::isfinite(parent.combinedPosition.x) || !std::isfinite(parent.combinedPosition.y)) {
            return std::nullopt;
        }
        x = static_cast<float>(static_cast<double>(x) + static_cast<double>(parent.combinedPosition.x));
        y = static_cast<float>(static_cast<double>(y) + static_cast<double>(parent.combinedPosition.y));
        if (!std::isfinite(x) || !std::isfinite(y)) {
            return std::nullopt;
        }
    }
    return SpawnRequest{subroutine, {x, y, 0}, life, score, drop, opcode == 260||opcode==261 ? 1u : 0u,
                        parent.inheritedWords, parent.contextTag};
}

}
