#include "BulletUpdateHost.hpp"
#include "BulletExtension800000.hpp"
#include "BulletExtension256.hpp"
#include "BulletExtension8192.hpp"
#include "Extension16384.hpp"
#include "Extension1048576.hpp"
#include "Extension512.hpp"
#include "Extension4096.hpp"
#include "BulletTickRules.hpp"
#include "BulletCollisionRules.hpp"
#include "GrazeRules.hpp"
#include <cmath>
#include <numbers>
#include <sstream>

namespace th128::bullet {
bool updateBulletWithKnownMisses(Birth& bullet,BulletVisuals& visuals,BulletVisuals::Visual& visual,
                                 Position player,float profileHitRadius,bool& removed,std::string& error,
                                 const GrazeCallback& graze,const HitCallback& hit,const CancellationCallback& cancellation,const ExtensionSound& sound,
                                 const Manager::NestedEmitter& nestedEmitter,
                                 const std::function<const Profile*(int)>& profileForType,
                                 const RestartCallback& restart,const EnemySpawnCallback& spawnEnemy,
                                 const std::optional<Position>& playerPathStart) {
    removed=false;error.clear();
    if(bullet.updateExecuting){if(bullet.extensionFailure.empty())bullet.extensionFailure="bullet update reentered";error=bullet.extensionFailure;return false;}
    if(!bullet.extensionFailure.empty()){error=bullet.extensionFailure;return false;}
    struct Guard {bool& value;explicit Guard(bool& state):value(state){value=true;}~Guard(){value=false;}} guard(bullet.updateExecuting);
    if(visual.hasSecondary&&(!((bullet.type==24&&bullet.script==283
        &&visual.primary.state().script==283&&visual.secondary.state().script==284
        &&bullet.state.hitboxSize==6.f&&bullet.state.auxiliarySize==22.f&&bullet.state.freezePolicy==1)
        ||(bullet.type==25&&bullet.script==102
        &&visual.primary.state().script==102&&visual.secondary.state().script==103
        &&bullet.state.hitboxSize==6.f&&bullet.state.auxiliarySize==22.f&&bullet.state.freezePolicy==1)
        ||(bullet.type==30&&bullet.script==285
        &&visual.primary.state().script==285&&visual.secondary.state().script==286
        &&bullet.state.hitboxSize==4.f&&bullet.state.auxiliarySize==22.f&&bullet.state.freezePolicy==1)))){
        error="unsupported secondary bullet VM pairing";return false;
    }
    if(!std::isfinite(profileHitRadius)||profileHitRadius<0||profileHitRadius>collision::MaximumMagnitude
        ||(profileHitRadius!=0&&profileHitRadius<collision::MinimumNonzeroMagnitude)) {
        error="unsupported original player profile hit-size domain";return false;
    }
    const auto* animation=visuals.animationState(visual);
    if(!animation){error="bullet animation is stale or faulted before update";return false;}
    const auto* sprite=visuals.spriteInfo(animation->sprite);
    if(!sprite){error="bullet sprite metrics are unavailable before update";return false;}
    TickInput input;
    input.position=bullet.state.position;input.velocity=bullet.state.velocity;
    input.type=bullet.type;input.motionState=bullet.state.motionState;input.age=bullet.age;
    input.lifeClock=bullet.lifeClock;input.lifeTime=bullet.lifeTime;input.integer0=animation->integers[0];
    input.flags=bullet.state.flags;input.activeExtensions=bullet.state.activeExtensions;
    input.grace=bullet.grace;input.cooldown=bullet.cooldown;
    input.hasSprite=true;input.spriteWidth=sprite->width;input.spriteHeight=sprite->height;
    input.extensionListEmpty=bullet.extensionListEmpty;
    auto pending=preAnimationTick(input);
    if(!pending){error="unsupported bullet update numeric/state domain";return false;}
    const auto commit=[&bullet](const TickInput& state) {
        bullet.state.position=state.position;bullet.state.velocity=state.velocity;
        bullet.state.motionState=state.motionState;bullet.state.flags=state.flags;
        bullet.state.activeExtensions=state.activeExtensions;bullet.lifeClock=state.lifeClock;
        bullet.lifeTime=state.lifeTime;bullet.grace=state.grace;bullet.cooldown=state.cooldown;
    };
    unsigned contacts=0;
    for(unsigned phases=0;phases<8&&(pending->continuation==Continuation::NeedsCollision||pending->continuation==Continuation::NeedsExtension);++phases) {
        commit(pending->next);
        if(pending->continuation==Continuation::NeedsExtension){
            const auto failExtension=[&](std::string reason){bullet.extensionFailure=std::move(reason);error=bullet.extensionFailure;return false;};
            if(pending->boundary==0x408caa){
                const auto step=stepExtension4096(bullet.extension4096,bullet.state.position,sprite->width,sprite->height);
                if(!step)return failExtension("unsupported original type4096 wrap context");
                if(step->wrapped&&bullet.state.transformSound>=0){
                    try{if(!sound||!sound(bullet.state.transformSound,error)||!error.empty()||!bullet.extensionFailure.empty())
                        return failExtension(!bullet.extensionFailure.empty()?bullet.extensionFailure:error.empty()?"required extension4096 sound rejected":error);}
                    catch(...){return failExtension("extension4096 sound callback threw");}
                }
                bullet.extension4096=step->next;bullet.state.position=step->position;
                if(step->finished)bullet.state.activeExtensions&=~0x1000u;
                pending=resumeAfterExtension4096(*pending,bullet.state.position,bullet.state.activeExtensions);
                if(!pending)return failExtension("type4096 tail continuation was rejected");
                continue;
            }
            if(pending->boundary!=0x4088a6||(bullet.state.activeExtensions&~0x8000115du))
                return failExtension("unimplemented bullet active extension lifecycle");
            bool completedQueue=false;
            bool restartedScript=false;
            for(unsigned extensionPass=0;extensionPass<64;++extensionPass){
            while(!bullet.extensionListEmpty){
                const int index=bullet.state.extensionIndex;
                if(index<0||index>=18)return failExtension("invalid live extension index");
                const auto record=index==0?bullet.state.extension0:bullet.state.laterExtensions[std::size_t(index-1)];
                if((record.control==0&&(bullet.state.activeExtensions&~0x100u))||(unsigned(record.type)&bullet.state.activeExtensions))break;
                if((record.control!=0&&record.control!=1)||(record.type!=1&&record.type!=16&&record.type!=4&&record.type!=8&&record.type!=64&&record.type!=256&&record.type!=0x2000&&record.type!=16384&&record.type!=4096&&std::uint32_t(record.type)!=0x800000u&&std::uint32_t(record.type)!=0x80000000u&&std::uint32_t(record.type)!=0x100000u&&record.type!=512&&record.type!=1024&&std::uint32_t(record.type)!=0x01000000u))return failExtension("unimplemented extension record ordering/type="+std::to_string(record.type));
                if(std::uint32_t(record.type)==0x01000000u){
                    if(!spawnEnemy||!record.callbackName)return failExtension("original ETEX16777216 requires its actual ECL enemy owner");
                    try{if(!spawnEnemy(bullet,record,index,error)||!error.empty())
                        return failExtension(error.empty()?"original ETEX16777216 enemy construction rejected":error);}
                    catch(...){return failExtension("original ETEX16777216 enemy owner threw");}
                    if(bullet.state.extensionIndex!=index)return failExtension("ETEX16777216 parent extension index changed during enemy creation");
                    ++bullet.state.extensionIndex;bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
                    continue;
                }
                if(record.type==4096){
                    const auto initialized=initializeExtension4096(record);
                    if(!initialized)return failExtension("unsupported type4096 initialization");
                    bullet.extension4096=*initialized;bullet.state.activeExtensions|=0x1000u;
                    ++bullet.state.extensionIndex;bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
                    continue;
                }
                if(record.type==1024){
                    if(!restart)return failExtension("unimplemented original type1024 script restart owner");
                    if(!restart(bullet,visuals,visual,record,index,error))
                        return failExtension(error.empty()?"original type1024 script restart rejected":error);
                    if(bullet.state.motionState!=3||bullet.state.extensionIndex!=index)
                        return failExtension("type1024 restart did not commit the original state word and index");
                    restartedScript=true;
                    ++bullet.state.extensionIndex;
                    bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
                    continue;
                }
                if(std::uint32_t(record.type)==0x100000u){
                    const auto consumed=consumeExtension1048576(record,index,bullet.state.flags,error);
                    if(!consumed)return failExtension(error.empty()?"extension1048576 context rejected":error);
                    bullet.state.flags=consumed->flags;
                    bullet.state.extensionIndex=consumed->nextExtensionIndex;
                    bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
                    continue;
                }
                if(record.type==1){
                    const auto initialized=initializeExtension1(record);if(!initialized)return failExtension("unimplemented type1 initialization context");
                    bullet.extension1=*initialized;bullet.state.activeExtensions|=1u;
                }else if(record.type==16){
                    const auto initialized=initializeExtension16(record,bullet.speed,bullet.angle);
                    if(!initialized)return failExtension("unimplemented type16 initialization domain/mode/callback index="
                        +std::to_string(index)+" control="+std::to_string(record.control)
                        +" type="+std::to_string(record.type)+" A="+std::to_string(record.parameterA)
                        +" B="+std::to_string(record.parameterB)+" C="+std::to_string(record.parameterC)
                        +" D="+std::to_string(record.parameterD)+" R="+std::to_string(record.parameterR)
                        +" S="+std::to_string(record.parameterS)+" speed="+std::to_string(bullet.speed)
                        +" angle="+std::to_string(bullet.angle));
                    bullet.extension16=*initialized;bullet.state.activeExtensions|=16u;
                }else if(record.type==64){
                    const auto initialized=initializeExtension64(record);
                    if(!initialized)return failExtension("unimplemented type64 initialization domain");
                    bullet.extension64=*initialized;bullet.state.activeExtensions|=0x40u;
                }else if(record.type==256){
                    const auto initialized=initializeExtension256(record);
                    if(!initialized)return failExtension("unimplemented type256 initialization domain");
                    bullet.extension256=*initialized;bullet.state.activeExtensions|=0x100u;
                }else if(record.type==8){
                    const auto initialized=initializeExtension8(record);if(!initialized)return failExtension("invalid original ETEX8 context");bullet.extension8=*initialized;bullet.state.activeExtensions|=8u;
                    if(index!=0&&bullet.state.transformSound>=0){if(!sound||!sound(bullet.state.transformSound,error)||!error.empty())return failExtension(error.empty()?"ETEX8 transform sound rejected":error);}
                }else if(record.type==0x2000){
                    if(index+1>=18)return failExtension("ETEX8192 requires a next record");
                    const auto next=index+1==0?bullet.state.extension0:bullet.state.laterExtensions[std::size_t(index)];
                    if(next.type!=16384||(next.control!=0&&next.control!=1)||next.parameterA!=1
                        ||!std::isfinite(next.parameterR)||next.parameterR < -std::numbers::pi_v<float>
                        ||next.parameterR > std::numbers::pi_v<float>)
                        return failExtension("ETEX8192 next record is not an exact resolved type16384 marker/type="+std::to_string(next.type));
                    const auto state=decodeExtension8192(record);
                    if(!state||!nestedEmitter)return failExtension("unsupported ETEX8192 payload/nested emitter context");
                    const auto* childProfile=profileForType?profileForType(state->type):nullptr;
                    if(!childProfile||childProfile->type!=state->type)return failExtension("missing actual decoded child profile");
                    const int count=record.parameterB;
                    const auto request=makeExtension8192Request(*state,*childProfile,bullet,player,next.parameterR,count);
                    if(!request)return failExtension("unsupported ETEX8192 packed fields/profile/count/resolved next record angle");
                    if(!nestedEmitter(bullet,*request,error))return failExtension(error.empty()?"nested ETEX8192 emission rejected":error);
                    ++bullet.state.extensionIndex;
                    bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
                    continue;
                }else if(record.type==16384){
                    const auto initialized=initializeExtension16384(record,index);
                    if(!initialized)return failExtension("unimplemented exact type16384 marker context");
                    const auto step=consumeExtension16384(*initialized);
                    if(!step||step->activeChanged||step->tick||step->retired||step->parentChanged||step->hostCalls||step->rngCalls)
                        return failExtension("invalid type16384 marker no-op contract");
                    bullet.state.extensionIndex=step->next.nextExtensionIndex;
                    bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
                    continue;
                }else if(record.type==512){
                    if(!profileForType)return failExtension("type512 conversion requires an actual target profile");
                    const auto* targetProfile=profileForType(record.parameterA&0xffff);
                    if(!targetProfile||targetProfile->type!=(record.parameterA&0xffff))
                        return failExtension("type512 target profile is missing or mismatched");
                    const auto cancellation=targetProfile->cancellationScript(record.parameterB&0x7fff);
                    if(!cancellation)return failExtension("type512 target cancellation script is unavailable");
                    Extension512TargetProfile target;
                    target.type=targetProfile->type;
                    target.color=record.parameterB&0x7fff;
                    target.script=targetProfile->script;
                    target.secondaryScript=targetProfile->secondaryScript;
                    target.hitboxSize=targetProfile->tail.hitboxSize;
                    target.auxiliarySize=targetProfile->tail.auxiliarySize;
                    target.appearance=targetProfile->tail.value;
                    target.cancellationScript=*cancellation;
                    target.freezePolicy=targetProfile->tail.freezePolicy;
                    Extension512Parent parent;
                    parent.type=bullet.type;parent.color=bullet.color;parent.flags=bullet.state.flags;
                    parent.activeExtensions=bullet.state.activeExtensions;parent.extensionIndex=index;
                    const auto step=initializeExtension512(record,index,parent,target);
                    if(!step)return failExtension("unimplemented exact type512 conversion context");
                    BulletVisuals::CreateInfo info;
                    info.type=step->type;info.color=step->color;info.script=step->script;
                    info.deferPostBindAdvance=step->primaryInterrupt!=0;
                    if(step->secondaryVisualActive)info.secondaryScript=step->secondaryScript;
                    info.identitySpriteRemap=targetProfile->identitySpriteRemap();
                    for(std::size_t remapIndex=0;remapIndex<info.spriteRemap.size();++remapIndex){
                        const auto mapped=targetProfile->remap(step->color,std::int32_t(remapIndex));
                        if(!mapped)return failExtension("type512 target sprite remap is unavailable");
                        info.spriteRemap[remapIndex]=*mapped;
                    }
                    auto converted=visuals.create(info,error);
                    if(!converted)return failExtension(error.empty()?"type512 replacement visual was rejected":error);
                    if(step->primaryInterrupt!=0
                        &&!visuals.interrupt(*converted,step->primaryInterrupt,error))
                        return failExtension(error.empty()?"type512 owned primary interrupt was rejected":error);
                    visual=std::move(*converted);
                    bullet.type=step->type;bullet.color=step->color;bullet.script=step->script;
                    bullet.cancellationScript=step->cancellationScript;
                    bullet.state.hitboxSize=step->hitboxSize;
                    bullet.state.auxiliarySize=step->auxiliarySize;
                    bullet.state.profileValue=step->appearance;
                    bullet.state.freezePolicy=step->freezePolicy;
                    bullet.state.flags=step->flags;
                    bullet.state.extensionIndex=step->nextExtensionIndex;
                    bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
                    continue;
                }else if(std::uint32_t(record.type)==0x80000000u){
                    if(record.parameterA>65535)return failExtension("unsupported ETEX delay duration");
                    if(record.parameterA>0){bullet.delayRemaining=record.parameterA;bullet.state.activeExtensions|=0x80000000u;}
                }else if(std::uint32_t(record.type)==0x800000u){
                    Extension800000Context context;
                    context.extensionIndex=index;
                    context.activeExtensions=bullet.state.activeExtensions;
                    context.speed=bullet.speed;
                    context.angle=bullet.angle;
                    context.position=bullet.state.position;
                    const auto consumed=consumeExtension800000(record,context,error);
                    if(!consumed)return failExtension(error.empty()?"extension800000 context rejected":error);
                    bullet.extension800000=consumed->snapshot;
                    bullet.state.extensionIndex=consumed->nextExtensionIndex;
                    bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
                    continue;
                }else{
                    const auto initialized=initializeExtension4(record,bullet.angle,bullet.state.position,player);
                    if(!initialized)return failExtension("unimplemented type4 initialization domain/callback");
                    bullet.extension4=*initialized;bullet.state.activeExtensions|=4u;
                    if(index!=0&&bullet.state.transformSound>=0){
                        try{if(!sound||!sound(bullet.state.transformSound,error)||!error.empty()||!bullet.extensionFailure.empty())
                            return failExtension(!bullet.extensionFailure.empty()?bullet.extensionFailure:error.empty()?"required type4 initialization sound at 0x409c1d":error);}
                        catch(...){return failExtension("type4 initialization sound callback threw");}
                    }
                }
                ++bullet.state.extensionIndex;
                bullet.extensionListEmpty=nextExtensionEmpty(bullet.state);
            }
            const bool hadActive=bullet.state.activeExtensions!=0;
            bool finished=false;
            if(bullet.state.activeExtensions&1u){
                const auto step=stepExtension1(bullet.extension1,bullet.speed,bullet.angle,bullet.state.velocity);
                if(!step)return failExtension("unsupported type1 original burst update");
                bullet.extension1=step->next;bullet.state.velocity=step->velocity;if(step->finished){bullet.state.activeExtensions&=~1u;finished=true;}
            }
            if(bullet.state.activeExtensions&4u){
                const auto step=stepExtension4(bullet.extension4,bullet.speed,bullet.angle,bullet.state.velocity);
                if(!step)return failExtension("unsupported type4 unit update");
                bullet.extension4=step->next;bullet.speed=step->speed;bullet.angle=step->angle;bullet.state.velocity=step->velocity;
                if(step->finished){bullet.state.activeExtensions&=~4u;finished=true;}
            }
            if(bullet.state.activeExtensions&8u){
                const auto step=stepExtension8(bullet.extension8,bullet.speed,bullet.angle,bullet.state.velocity);if(!step)return failExtension("original ETEX8 unit update rejected");
                bullet.extension8=step->next;bullet.speed=step->speed;bullet.angle=step->angle;bullet.state.velocity=step->velocity;if(step->finished){bullet.state.activeExtensions&=~8u;finished=true;}
            }
            if(bullet.state.activeExtensions&16u){
                auto step=stepExtension16(bullet.extension16,bullet.speed,bullet.angle,bullet.state.velocity,bullet.state.position,player,bullet.state.transformSound,&bullet.extension800000.position);
                if(!step)return failExtension("unsupported type16 unit update");
                if(step->soundRequired){
                    try{if(!sound||!sound(bullet.state.transformSound,error)||!error.empty()||!bullet.extensionFailure.empty())return failExtension(!bullet.extensionFailure.empty()?bullet.extensionFailure:error.empty()?"required extension sound at 0x40b225":error);}
                    catch(...){return failExtension("extension sound callback threw");}
                    step=stepExtension16(bullet.extension16,bullet.speed,bullet.angle,bullet.state.velocity,bullet.state.position,player,-1,&bullet.extension800000.position);
                    if(!step)return failExtension("type16 post-sound continuation failed");
                }
                bullet.extension16=step->next;bullet.speed=step->speed;bullet.angle=step->angle;bullet.state.velocity=step->velocity;
                if(step->finished){bullet.state.activeExtensions&=~16u;finished=true;}
            }
            if(bullet.state.activeExtensions&0x40u){
                const auto step=stepExtension64(bullet.extension64,bullet.speed,bullet.angle,bullet.state.position,bullet.state.velocity,true);
                if(!step)return failExtension("unsupported type64 unit update");
                if(step->mutated&&bullet.state.transformSound>=0){
                    try{if(!sound||!sound(bullet.state.transformSound,error)||!error.empty())return failExtension(error.empty()?"required extension64 sound rejected":error);}
                    catch(...){return failExtension("extension64 sound callback threw");}
                }
                bullet.extension64=step->next;bullet.speed=step->speed;bullet.angle=step->angle;bullet.state.velocity=step->velocity;
                bullet.state.position=step->position;pending->next.position=step->position;
                if(step->finished){bullet.state.activeExtensions&=~0x40u;finished=true;}
            }
            if(bullet.state.activeExtensions&0x100u){
                const auto step=stepExtension256(bullet.extension256);
                if(!step)return failExtension("unsupported type256 unit update");
                bullet.extension256=step->next;
                if(step->finished){bullet.state.activeExtensions&=~0x100u;finished=true;}
            }
            if(bullet.state.activeExtensions&0x80000000u){
                if(bullet.delayRemaining<=0){bullet.state.activeExtensions&=~0x80000000u;finished=true;}
                else --bullet.delayRemaining;
            }
            if(hadActive&&pending->next.cooldown!=0)--pending->next.cooldown;
            if(!finished){completedQueue=true;break;}
            }
            if(!completedQueue)return failExtension("extension completion loop budget exceeded");
            if(restartedScript){
                pending=resumeAfterRestartedExtension(*pending,bullet.state.activeExtensions,bullet.extensionListEmpty);
                if(!pending)return failExtension("invalid original type1024 restart continuation");
                pending->next.flags=bullet.state.flags;
                continue;
            }
            pending=resumeAfterResolvedExtension(*pending,bullet.state.velocity,bullet.state.activeExtensions,bullet.extensionListEmpty);
            if(!pending)return failExtension("unimplemented later extension continuation");
            pending->next.flags=bullet.state.flags;
            continue;
        }
        if(++contacts>2){error="bullet collision pass budget exceeded";return false;}
        if(!(pending->next.flags&0x10U)||(pending->next.flags&0x40U)) {
            error="unimplemented rectangular or scaled bullet collision context";return false;
        }
        const collision::Vec2 center{pending->next.position.x,pending->next.position.y};
        auto geometry=collision::circleGeometry(center,
            bullet.state.hitboxSize,{player.x,player.y},profileHitRadius);
        if(!geometry){error="unsupported bullet/player collision numeric domain";return false;}
        // Only this newer unlimited-touch rule sweeps hits. Graze and all
        // normal or legacy Replay motion keep their original endpoints.
        if(playerPathStart&&*geometry!=collision::Geometry::PotentialHit){
            const auto hitOnPath=collision::circleSweepHit(center,bullet.state.hitboxSize,
                {playerPathStart->x,playerPathStart->y},{player.x,player.y},profileHitRadius);
            if(!hitOnPath){error="unsupported swept touch collision domain";return false;}
            if(*hitOnPath)geometry=collision::Geometry::PotentialHit;
        }
        if(*geometry==collision::Geometry::PotentialHit) {
            if(!hit){error="unimplemented player hit gate/damage lifecycle";return false;}
            const auto returned=hit(bullet,error);
            if(!returned){if(error.empty())error="player hit callback is incomplete";return false;}
            if(*returned==0){pending=resumeAfterCollisionMiss(*pending);if(!pending){error="invalid suppressed-hit continuation";return false;}continue;}
            if(*returned!=1){error="invalid hit helper result";return false;}
            const bool fullStep=pending->next.motionState==1;
            bullet.state.motionState=3;
            if(!visuals.interrupt(visual,1,error))return false;
            if(bullet.cancellationScript>=0){
                if(!cancellation){error="unimplemented bullet hit-effect callback";return false;}
                if(!cancellation(bullet,fullStep,error))return false;
            }
            pending=resumeAfterResolvedHit(*pending);
            if(!pending){error="invalid completed-hit continuation";return false;}
            continue;
        }
        if(*geometry==collision::Geometry::PotentialGraze) {
            const auto decision=th128::graze::grazeCallerDecision(pending->next.motionState==2?
                th128::graze::UpdateSite::HalfStep:th128::graze::UpdateSite::FullStep,2,pending->next.flags);
            if(!decision){error="unsupported graze continuation";return false;}
            if(decision->action==th128::graze::CallerAction::GrazeCallRequired) {
                if(!graze){error="unimplemented bullet graze side effects";return false;}
                if(!graze(bullet,error))return false;
                pending->next.flags|=decision->flagsSetAfterSuccess;
                bullet.state.flags=pending->next.flags;
            }
        }
        pending=resumeAfterCollisionMiss(*pending);
        if(!pending){error="invalid continuation after proven collision miss";return false;}
    }
    commit(pending->next);
    if(pending->continuation==Continuation::Removed){removed=true;return true;}
    if(pending->continuation!=Continuation::NeedsAnimation) {
        std::ostringstream message;message<<"unimplemented bullet update continuation at 0x"<<std::hex<<pending->boundary;
        error=message.str();return false;
    }
    if(!visuals.advance(visual,1,error))return false;
    animation=visuals.animationState(visual);
    if(!animation){error="bullet animation failed after update";return false;}
    removed=animation->ended||animation->deleteRequested;
    return true;
}
}
