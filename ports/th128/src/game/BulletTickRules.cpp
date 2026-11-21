#include "BulletTickRules.hpp"
#include <cmath>

namespace th128::bullet {

namespace {

bool bounded(float value) noexcept {
    const auto magnitude = std::abs(value);
    return std::isfinite(value) && magnitude <= 16777216.f && (value == 0 || magnitude >= 0x1p-16f);
}

bool bounded(const Position& position) noexcept {
    const auto component=[](float value){return std::isfinite(value)&&std::abs(value)<=16777216.f;};
    return component(position.x)&&component(position.y)&&component(position.z);
}

void move(Position& position, const Position& velocity, float scale) noexcept {
    const auto deltaX = float(double(velocity.x) * scale);
    const auto deltaY = float(double(velocity.y) * scale);
    const auto deltaZ = float(double(velocity.z) * scale);
    position.x = float(double(position.x) + deltaX);
    position.y = float(double(position.y) + deltaY);
    position.z = float(double(position.z) + deltaZ);
}

bool outside(const TickInput& state) noexcept {
    const auto halfWidth = double(state.spriteWidth) * .5;
    const auto halfHeight = double(state.spriteHeight) * .5;
    return double(state.position.x) + halfWidth <= -192. ||
           double(state.position.x) - halfWidth >= 192. ||
           double(state.position.y) + halfHeight <= -64. ||
           double(state.position.y) - halfHeight >= 448.;
}

bool validInput(const TickInput& input) noexcept {
    return !((input.type != 4&&input.type != 6&&input.type != 8&&input.type!=9&&input.type!=10&&input.type!=13&&input.type!=16&&input.type!=18&&input.type!=20&&input.type!=21&&input.type!=23&&input.type!=24&&input.type!=25&&input.type!=29&&input.type!=30&&input.type!=32&&input.type!=33) || (input.motionState != 1 && input.motionState != 2 && input.motionState != 3 && input.motionState != 4) ||
        input.age < 0 || input.age > 4096 || input.lifeClock < 0 || input.lifeClock > 4096 ||
        input.clock != 1 || !bounded(input.lifeTime) || input.lifeTime < 0 ||
        !bounded(input.position) || !bounded(input.velocity) ||
        !bounded(input.spriteWidth) || !bounded(input.spriteHeight) ||
        input.spriteWidth < 0 || input.spriteHeight < 0 ||
        input.grace < -4096 || input.grace > 4096 || input.cooldown < -4096 || input.cooldown > 4096);
}

PreAnimationTick stop(PreAnimationTick result, Continuation continuation, std::uint32_t boundary) noexcept {
    result.continuation = continuation;
    result.boundary = boundary;
    return result;
}

PreAnimationTick remove(PreAnimationTick result) noexcept {
    result.next.motionState = 0;
    result.next.flags &= ~1u;
    result.next.age = 0;
    result.next.lifeClock = 0;
    result.next.lifeTime = 0;
    result.previousLifeClock = -1;
    return stop(result, Continuation::Removed, 0x4086c6);
}

std::uint32_t collisionSite(const TickInput& state, bool halfStep) noexcept {
    if (halfStep)
        return state.flags & 0x10u ? 0x40880fu : state.flags & 0x40u ? 0x4087f0u : 0x4087b4u;
    return state.flags & 0x10u ? 0x408a9fu : state.flags & 0x40u ? 0x408a80u : 0x408a44u;
}

PreAnimationTick tail(PreAnimationTick result) noexcept {
    auto& state = result.next;
    if (state.hasSprite) {
        if (state.activeExtensions & 0x1000u)
            return stop(result, Continuation::NeedsExtension, 0x408caa);
        if (!(state.activeExtensions & 0x100u) && state.grace <= 0 && outside(state))
            return remove(result);
    }
    if (state.cooldown != 0)
        --state.cooldown;
    if (state.grace > 0)
        --state.grace;
    return stop(result, Continuation::NeedsAnimation, 0x408d09);
}

PreAnimationTick normalMovement(PreAnimationTick result) noexcept {
    auto& state = result.next;
    move(state.position, state.velocity, 1.f);
    if (state.flags & 2u)
        return stop(result, Continuation::NeedsCollision, collisionSite(state, false));
    return tail(result);
}
PreAnimationTick normal(PreAnimationTick result) noexcept {
    if(!result.next.extensionListEmpty||result.next.activeExtensions!=0)return stop(result,Continuation::NeedsExtension,0x4088a6);
    return normalMovement(result);
}

PreAnimationTick afterHalf(PreAnimationTick result) noexcept {
    if (result.next.integer0 != 0) {
        result.next.motionState = 1;
        return normal(result);
    }
    return tail(result);
}

bool validPendingCollision(const PreAnimationTick& pending) noexcept {
    const auto& state=pending.next;
    auto original=state;
    original.position={};original.lifeClock=pending.previousLifeClock;original.lifeTime=0;
    const auto reachable=[](float value){return std::isfinite(value)&&std::abs(value)<=3.f*16777216.f;};
    if(pending.continuation!=Continuation::NeedsCollision||(state.motionState!=1&&state.motionState!=2)
        ||!validInput(original)||state.lifeClock!=pending.previousLifeClock+1||!bounded(state.lifeTime)||state.lifeTime<1
        ||!(state.flags&2U)||(state.flags&8U)||(state.activeExtensions&0x400000U)
        ||!reachable(state.position.x)||!reachable(state.position.y)||!reachable(state.position.z))return false;
    const bool half=state.motionState==2;
    return pending.boundary==collisionSite(state,half)&&(!half||state.age>=8)
        &&(half||(state.extensionListEmpty&&state.activeExtensions==0)
            ||(pending.extensionResolved&&!(state.activeExtensions&~0x8000115du)&&(state.extensionListEmpty||state.activeExtensions!=0)));
}

}

std::optional<PreAnimationTick> preAnimationTick(const TickInput& input) noexcept {
    if (!validInput(input))
        return std::nullopt;
    PreAnimationTick result;
    result.next = input;
    result.previousLifeClock = input.lifeClock;
    auto& state = result.next;
    ++state.lifeClock;
    state.lifeTime = float(double(state.lifeTime) + 1.);
    if (state.flags & 8u)
        return remove(result);
    if (state.activeExtensions & 0x400000u)
        return stop(result, Continuation::NeedsExtension, 0x4086d9);
    if(state.motionState==3){move(state.position,state.velocity,.5f);return tail(result);}
    if(state.motionState==4)return tail(result);
    if (state.motionState == 1)
        return normal(result);
    move(state.position, state.velocity, .5f);
    if (state.age >= 8 && (state.flags & 2u))
        return stop(result, Continuation::NeedsCollision, collisionSite(state, true));
    return afterHalf(result);
}

std::optional<PreAnimationTick> resumeAfterCollisionMiss(const PreAnimationTick& pending) noexcept {
    if(!validPendingCollision(pending))return std::nullopt;
    return pending.next.motionState==2?afterHalf(pending):tail(pending);
}

std::optional<PreAnimationTick> resumeAfterResolvedHit(const PreAnimationTick& pending) noexcept {
    if(!validPendingCollision(pending))return std::nullopt;
    auto result=pending;result.next.motionState=3;return tail(result);
}
std::optional<PreAnimationTick> resumeAfterResolvedExtension(const PreAnimationTick& pending,Position velocity,std::uint32_t activeExtensions,bool nextRecordEmpty) noexcept {
    if(pending.continuation!=Continuation::NeedsExtension||pending.boundary!=0x4088a6||pending.next.motionState!=1
        ||pending.previousLifeClock<0||pending.previousLifeClock>4096||pending.next.lifeTime<1||!bounded(pending.next.lifeTime)
        ||pending.next.flags&8u
        ||pending.next.lifeClock!=pending.previousLifeClock+1||!bounded(velocity)||(activeExtensions&~0x8000115du)||(!nextRecordEmpty&&activeExtensions==0))return std::nullopt;
    auto result=pending;result.next.velocity=velocity;result.next.activeExtensions=activeExtensions;result.next.extensionListEmpty=nextRecordEmpty;
    result.extensionResolved=true;return normalMovement(result);
}

std::optional<PreAnimationTick> resumeAfterRestartedExtension(const PreAnimationTick& pending,std::uint32_t activeExtensions,bool nextRecordEmpty) noexcept {
    if(pending.continuation!=Continuation::NeedsExtension||pending.boundary!=0x4088a6||pending.next.motionState!=1
        ||pending.previousLifeClock<0||pending.previousLifeClock>4096||pending.next.lifeTime<1||!bounded(pending.next.lifeTime)
        ||pending.next.flags&8u||pending.next.lifeClock!=pending.previousLifeClock+1
        ||(activeExtensions&~0x8000115du)||(!nextRecordEmpty&&activeExtensions==0))return std::nullopt;
    auto result=pending;result.next.motionState=3;result.next.activeExtensions=activeExtensions;
    result.next.extensionListEmpty=nextRecordEmpty;result.extensionResolved=true;return tail(result);
}

std::optional<PreAnimationTick> resumeAfterExtension4096(const PreAnimationTick& pending,Position position,std::uint32_t activeExtensions) noexcept {
    if(pending.continuation!=Continuation::NeedsExtension||pending.boundary!=0x408caa
        ||!(pending.next.activeExtensions&0x1000u)||!bounded(position)
        ||(activeExtensions&~0x1000u)!=(pending.next.activeExtensions&~0x1000u)
        ||!validInput(pending.next))return std::nullopt;
    auto result=pending;
    result.next.position=position;result.next.activeExtensions=activeExtensions;
    if(!(activeExtensions&0x100u)&&result.next.grace<=0&&outside(result.next))return remove(result);
    if(result.next.cooldown!=0)--result.next.cooldown;
    if(result.next.grace>0)--result.next.grace;
    return stop(result,Continuation::NeedsAnimation,0x408d09);
}

}
