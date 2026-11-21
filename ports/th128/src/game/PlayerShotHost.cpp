#include "PlayerShotHost.hpp"
#include "PlayerShotMotion.hpp"
#include <bit>
#include <algorithm>
#include <limits>

namespace th128::playershot {
namespace {
std::uint32_t word(const std::array<std::uint8_t,ShotSlotBytes>& bytes,std::size_t offset){
    return std::uint32_t(bytes[offset])|std::uint32_t(bytes[offset+1])<<8|std::uint32_t(bytes[offset+2])<<16|std::uint32_t(bytes[offset+3])<<24;
}
ShotPosition positionAt(const std::array<std::uint8_t,ShotSlotBytes>& bytes,std::size_t offset){
    return {std::bit_cast<float>(word(bytes,offset)),std::bit_cast<float>(word(bytes,offset+4)),std::bit_cast<float>(word(bytes,offset+8))};
}
void store(std::array<std::uint8_t,ShotSlotBytes>& bytes,std::size_t offset,std::uint32_t value){
    for(std::size_t index=0;index<4;++index)bytes[offset+index]=std::uint8_t(value>>(8*index));
}
struct ExecutionGuard {bool& state;explicit ExecutionGuard(bool& executing):state(executing){state=true;}~ExecutionGuard(){state=false;}};
}
bool PlayerShotHost::fail(std::string message,std::string& error){if(failure_.empty())failure_=std::move(message);error=failure_;return false;}
bool PlayerShotHost::load(std::span<const std::uint8_t> shot,std::span<const std::uint8_t> animation,std::string& error){
    error.clear();
    if(executing_||bank_){error="player shot resources require a fresh inactive host";return false;}
    auto candidate=std::make_unique<ShtBank>();
    if(!candidate->load(shot,error)||!registry_.loadBank(7,animation.data(),animation.size(),error))return false;
    bank_=std::move(candidate);return true;
}
bool PlayerShotHost::setCallbackBindings(std::array<std::uint32_t,4> addresses){if(executing_)return false;callbacks_=addresses;return true;}
bool PlayerShotHost::setSoundHost(SoundHost sound){if(executing_)return false;sound_=std::move(sound);return true;}
bool PlayerShotHost::setBoundsHost(BoundsHost bounds){if(executing_)return false;bounds_=std::move(bounds);return true;}
bool PlayerShotHost::setImpactHost(ImpactHost impact){if(executing_)return false;impact_=std::move(impact);return true;}
bool PlayerShotHost::setDamageSourceHost(DamageSourceHost sources){if(executing_)return false;damageSources_=std::move(sources);return true;}
bool PlayerShotHost::setScoreHost(ScoreHost score){if(executing_)return false;score_=std::move(score);return true;}
bool PlayerShotHost::enableContactAnimations(){if(executing_||!bank_||!failure_.empty())return false;contactAnimations_=true;return true;}
anm::Runtime PlayerShotHost::runtime(Slot& slot){
    anm::Runtime context;
    context.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());context.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    context.unsupported=[&slot](int,int,int opcode,const char* reason){if(slot.fault.empty())slot.fault="player shot ANM opcode="+std::to_string(opcode)+": "+reason;};
    return context;
}
bool PlayerShotHost::emitCycle(float cumulativeArea,int difficulty,int shotCount,ShotPosition position,std::string& error){
    error.clear();
    if(executing_||!bank_||!failure_.empty())return fail(failure_.empty()?"player shot host is unavailable or reentered":failure_,error);
    if(!callbacks_){error="explicit live shot callback bindings are required";return false;}
    if(std::any_of(callbacks_->begin(),callbacks_->end(),[](std::uint32_t target){return target!=0;})){
        error="unimplemented live player shot callback";return false;
    }
    const auto selected=planShotGroup(*bank_,cumulativeArea,difficulty);
    if(!selected||!selected->group){error="unsupported player-shot group context";return false;}
    const auto* group=bank_->group(*selected->group);
    ExecutionGuard guard(executing_);
    for(std::uint32_t index=0;index<group->records.size();++index){
        const auto due=shotRecordDue(group->records[index],shotCount);
        if(!due)return fail("unsupported shot record schedule",error);
        if(!*due)continue;
        std::array<std::int32_t,ShotSlotCount> occupancy{};
        for(std::size_t slot=0;slot<slots_.size();++slot)occupancy[slot]=slots_[slot]?std::int32_t(word(slots_[slot]->plan.slotImage,0x48)):0;
        const auto firstFree=std::find(occupancy.begin(),occupancy.end(),0);
        const auto available=std::size_t(firstFree-occupancy.begin());
        if(available<slots_.size()&&slots_[available]&&slots_[available]->primaryAlive)
            return fail("inactive player shot still owns a live primary animation",error);
        const ShotBirthInput birthInput{position,shotCount,0,0,true};
        const auto plan=available<slots_.size()&&slots_[available]&&!slots_[available]->primaryAlive
            ?planRecycledShotBirth(*bank_,*selected->group,index,occupancy,birthInput,slots_[available]->plan.slotImage)
            :planFirstShotBirth(*bank_,*selected->group,index,occupancy,birthInput);
        if(!plan)return fail("unsupported first-use shot birth",error);
        if(plan->continuation==ShotBirthContinuation::PoolFull)break;
        for(const auto& callback:plan->requiredCallbacks)if(callback.fileId!=0)return fail("unresolved nonzero shot callback file ID",error);
        const auto motion=computeShotMotion(plan->motion.angle,plan->motion.speed);
        const auto completed=motion?completeFirstShotMotion(*bank_,*plan,*motion):std::nullopt;
        if(!completed)return fail("unsupported actual player shot motion",error);
        if(nextId_==0||nextVisualHandle_==0)return fail("player shot identity budget",error);
        auto candidate=std::make_unique<Slot>();candidate->plan=*completed;candidate->id=nextId_;candidate->visualHandle=nextVisualHandle_++;
        const auto actualPosition=positionAt(completed->slotImage,0x14);
        auto context=runtime(*candidate);candidate->visual.control().layer=completed->animation.layer;
        try{
            if(!candidate->visual.bind(registry_,7,completed->animation.script,context)||candidate->visual.faulted()||!candidate->fault.empty())
                return fail(candidate->fault.empty()?"player shot ANM binding failed":candidate->fault,error);
            if(completed->sound>=0&&(!sound_||!sound_(completed->sound,actualPosition.x,error)||!error.empty()||!failure_.empty()))
                return fail(error.empty()?"required player shot sound host is missing":error,error);
        }catch(...){return fail("player shot birth host callback threw",error);}
        store(candidate->plan.slotImage,0x4c,candidate->visualHandle);
        candidate->visual.control().engineX=320+actualPosition.x;candidate->visual.control().engineY=16+actualPosition.y;candidate->visual.control().engineZ=actualPosition.z;
        slots_[completed->slot]=std::move(candidate);++nextId_;
    }
    return true;
}
bool PlayerShotHost::tickUnit(std::string& error){
    error.clear();
    if(executing_||!bank_||!failure_.empty())return fail(failure_.empty()?"player shot tick is unavailable or reentered":failure_,error);
    if(!callbacks_)return fail("explicit live shot callback bindings are required",error);
    ExecutionGuard guard(executing_);
    try{
        for(auto& owned:slots_){
            if(!owned||word(owned->plan.slotImage,0x48)==0)continue;
            auto& slot=*owned;
            auto next=prepareOrdinaryShotTick(slot.plan.slotImage,ShotTickRecordBinding{slot.plan.sourceRecordOffset,slot.plan.record.raw[0x1d],(*callbacks_)[1]});
            if(!next||next->continuation!=ShotTickContinuation::NeedsMotion)return fail("unimplemented player shot record/motion callback",error);
            const auto motion=computeShotMotion(next->required.angle,next->required.speed);
            next=motion?completeOrdinaryShotMotion(*next,{motion->velocity.x,motion->velocity.y,motion->velocity.z}):std::nullopt;
            if(!next)return fail("unsupported player shot tick motion",error);
            slot.plan.slotImage=next->next;
            const bool found=slot.primaryAlive&&word(slot.plan.slotImage,0x4c)==slot.visualHandle;
            next=completeShotAnimationLookup(*next,found);
            if(!next)return fail("player shot ANM lookup continuation failed",error);
            if(!found){
                if(word(slot.plan.slotImage,0x50)!=0)return fail("unimplemented secondary player shot ANM cleanup",error);
                next=completeShotAnimationCleanup(*next);
            }else{
                const auto resource=registry_.bank(7);
                const auto& state=slot.visual.state();
                if(!resource||state.sprite<0||std::size_t(state.sprite)>=resource->sprites.size())return fail("player shot ANM sprite is unavailable",error);
                const auto& sprite=resource->sprites[std::size_t(state.sprite)];
                std::optional<ShotAnimationBounds> bounds;
                if(bounds_)bounds=bounds_(state,sprite,error);
                else{
                    ShotAxisAlignedAnimationInput input;
                    input.position={state.engineX,state.engineY,state.engineZ};
                    input.spriteSize={state.spriteWidth<0?sprite.width:state.spriteWidth,state.spriteHeight<0?sprite.height:state.spriteHeight};
                    input.scale={state.sx,state.sy};
                    input.flags=(std::uint32_t(state.mode)<<24)|(std::uint32_t(state.anchorX)<<20)|(std::uint32_t(state.anchorY)<<22);
                    input.noParentAndZeroOffsetsConfirmed=state.x==0&&state.y==0&&state.z==0&&state.offsetX==0&&state.offsetY==0&&state.offsetZ==0;
                    if(state.mode==0)bounds=axisAlignedShotAnimationBounds(input);
                    else if(state.mode==1&&std::abs(state.rotation)<=3.1415927410125732421875f){
                        const auto basis=computeShotMotion(state.rotation,1);
                        if(basis)bounds=rotatedShotAnimationBounds({input,{basis->velocity.x,basis->velocity.y},true});
                    }
                }
                next=bounds&&error.empty()&&failure_.empty()?completeShotAnimationBounds(*next,*bounds):std::nullopt;
                if(!next)return fail(error.empty()?"unimplemented actual player shot ANM bounds mode":error,error);
                if(next->continuation==ShotTickContinuation::NeedsAnimationDelete){
                    slot.primaryAlive=false;
                    next=completeShotAnimationDelete(*next);
                }else if(next->continuation==ShotTickContinuation::NeedsAnimationSync){
                    if(next->required.secondaryHandle)return fail("unimplemented secondary player shot ANM synchronization",error);
                    auto& control=slot.visual.control();
                    control.engineX=next->required.animationPosition.x;control.engineY=next->required.animationPosition.y;control.engineZ=next->required.animationPosition.z;
                    if(control.flags&0x40000000u){control.rotation=next->required.angle;control.flags|=4u;}
                    next=completeShotAnimationSync(*next);
                    next=next?finishShotTickAge(*next):std::nullopt;
                }else return fail("unexpected player shot bounds continuation",error);
            }
            if(!next)return fail("player shot lifecycle continuation failed",error);
            slot.plan.slotImage=next->next;
        }
    }catch(...){return fail("player shot tick host callback threw",error);}
    return true;
}
bool PlayerShotHost::tickAnimationsUnit(std::string& error){
    error.clear();
    if(executing_||!bank_||!failure_.empty())return fail(failure_.empty()?"player shot animation pass is unavailable or reentered":failure_,error);
    ExecutionGuard guard(executing_);
    std::vector<Slot*> ordered;
    for(auto& owned:slots_)if(owned&&owned->primaryAlive)ordered.push_back(owned.get());
    std::sort(ordered.begin(),ordered.end(),[](const Slot* first,const Slot* second){return first->visualHandle<second->visualHandle;});
    for(auto* slot:ordered){
        auto context=runtime(*slot);slot->visual.tick(context);
        if(slot->visual.faulted()||!slot->fault.empty())return fail(slot->fault.empty()?"player shot animation tick fault":slot->fault,error);
        if(slot->visual.state().ended||slot->visual.state().deleteRequested)slot->primaryAlive=false;
    }
    return true;
}
std::optional<shotenemy::ShotEnemyQueryPlan> PlayerShotHost::queryEnemy(
    const shotenemy::EnemyContactShape& enemy,std::uint32_t previousTime,std::uint32_t currentTime,
    std::span<const std::uint32_t> actualDamageSourceFlags,std::string& error){
    error.clear();
    if(executing_||!bank_||!callbacks_||!failure_.empty()){
        fail(failure_.empty()?"player shot query is unavailable or reentered":failure_,error);return std::nullopt;
    }
    ExecutionGuard guard(executing_);
    std::vector<shotenemy::ShotImage> images(ShotSlotCount);
    std::vector<shotenemy::ShotCollisionRecordBinding> records;
    for(std::size_t index=0;index<slots_.size();++index){
        if(!slots_[index])continue;
        const auto& slot=*slots_[index];images[index]=slot.plan.slotImage;
        const auto identity=slot.plan.sourceRecordOffset;
        if(std::none_of(records.begin(),records.end(),[identity](const auto& record){return record.recordIdentity==identity;}))
            records.push_back({identity,slot.plan.record.raw[0x1d],(*callbacks_)[3]});
    }
    auto plan=shotenemy::planShotEnemyQuery({enemy,previousTime,currentTime,true,0,images,records,std::nullopt});
    if(previousTime==currentTime)return plan;
    std::uint32_t accumulated=0;
    for(std::size_t contacts=0;contacts<ShotSlotCount&&plan.selectedShot;++contacts){
        const auto index=*plan.selectedShot;
        if(index>=slots_.size()||!slots_[index])break;
        auto& slot=*slots_[index];slot.plan.slotImage=plan.selectedContact.next;
        if(!contactAnimations_||plan.selectedContact.continuation!=shotenemy::ContactContinuation::NeedsAnimationCleanup)break;
        if(!slot.primaryAlive||word(slot.plan.slotImage,0x4c)!=slot.visualHandle){fail("shot hit primary ANM lookup failed",error);return std::nullopt;}
        const float rotation=slot.visual.state().rotation;
        slot.primaryAlive=false;
        if(nextVisualHandle_==0){fail("shot hit animation identity budget",error);return std::nullopt;}
        slot.visualHandle=nextVisualHandle_++;
        const auto& raw=slot.plan.record.raw;
        const auto scriptWord=std::uint16_t(std::uint16_t(raw[0x20])|std::uint16_t(raw[0x21])<<8);
        const auto script=std::int32_t(std::bit_cast<std::int16_t>(scriptWord))+5;
        slot.visual=anm::VM{};slot.visual.control().layer=23;
        auto context=runtime(slot);
        try{
            if(!slot.visual.bind(registry_,7,script,context)||slot.visual.faulted()||!slot.fault.empty()){
                fail("required shot hit replacement ANM binding failed",error);return std::nullopt;
            }
        }catch(...){fail("shot hit ANM callback threw",error);return std::nullopt;}
        slot.primaryAlive=true;
        const auto completed=shotenemy::completeOrdinaryShotContactAnimation(plan.selectedContact,slot.visualHandle);
        if(!completed){fail("unsupported shot contact animation tail",error);return std::nullopt;}
        slot.plan.slotImage=completed->next;slot.plan.animation.script=script;
        const auto position=positionAt(slot.plan.slotImage,0x14);
        auto& state=slot.visual.control();state.rotation=rotation;state.flags|=4u;
        state.engineX=float((double(position.x)+128.)+192.);state.engineY=float(double(position.y)+16.);state.engineZ=position.z;
        accumulated+=std::uint32_t(completed->completedDamage);images[index]=slot.plan.slotImage;
        plan=shotenemy::planShotEnemyQuery({enemy,previousTime,currentTime,true,0,images,records,std::nullopt});
    }
    if(accumulated){
        if(plan.continuation!=shotenemy::QueryContinuation::Complete&&
           plan.continuation!=shotenemy::QueryContinuation::NeedsDamageSourceSnapshot&&
           plan.continuation!=shotenemy::QueryContinuation::NeedsDamageSourceContact){
            fail("unimplemented later shot contact before query impact",error);return std::nullopt;
        }
        const auto damage=std::bit_cast<std::int32_t>(accumulated);
        if(damage>0){
            try{if(!impact_||!impact_(error)||!error.empty()||!failure_.empty()){fail(error.empty()?"required positive shot query impact at 0x43fcb3":error,error);return std::nullopt;}}
            catch(...){fail("shot query impact host threw",error);return std::nullopt;}
        }
    }
    if(plan.continuation==shotenemy::QueryContinuation::NeedsDamageSourceSnapshot&&damageSources_){
        try{
            const auto sources=damageSources_(enemy,plan.outFlag,error);
            if(!sources||!error.empty()||!failure_.empty()){
                fail(error.empty()?"required damage-source query rejected":error,error);return std::nullopt;
            }
            const auto finished=damagefield::finishDamageQuery(std::bit_cast<std::int32_t>(accumulated),sources->damage,0);
            if(!finished){fail("unsupported combined damage query total",error);return std::nullopt;}
            plan.continuation=shotenemy::QueryContinuation::Complete;plan.required={};
            plan.completedDamage=finished->damage;plan.outFlag=sources->outFlag;
            plan.examinedDamageSources=damagefield::Capacity;
            if(plan.completedDamage&&(!score_||!score_(plan.completedDamage,error)||!error.empty()||!failure_.empty())){
                fail(error.empty()?"required damage query score tail rejected":error,error);return std::nullopt;
            }
            return plan;
        }catch(...){fail("damage-source or score host threw",error);return std::nullopt;}
    }
    if(plan.continuation==shotenemy::QueryContinuation::NeedsDamageSourceSnapshot)
        plan=shotenemy::planShotEnemyQuery({enemy,previousTime,currentTime,true,0,images,records,actualDamageSourceFlags});
    if(plan.continuation==shotenemy::QueryContinuation::Complete)
        plan.completedDamage=std::min(std::bit_cast<std::int32_t>(accumulated),160);
    if(plan.continuation!=shotenemy::QueryContinuation::Complete &&
       plan.continuation!=shotenemy::QueryContinuation::NeedsDamageSourceSnapshot)
        fail(plan.selectedContact.required.callSite==0x43ec8a?"unimplemented shot hit animation lifecycle at 0x43ec8a":"unimplemented player shot enemy query continuation",error);
    return plan;
}
std::vector<ShotSnapshot> PlayerShotHost::snapshots() const{
    std::vector<ShotSnapshot> output;
    const auto resource=registry_.bank(7);
    if(!resource)return output;
    for(std::uint32_t index=0;index<slots_.size();++index){
        if(!slots_[index]||!slots_[index]->primaryAlive||word(slots_[index]->plan.slotImage,0x48)==0)continue;
        const auto& slot=*slots_[index];const auto& state=slot.visual.state();
        if(state.sprite<0||std::size_t(state.sprite)>=resource->sprites.size())continue;
        output.push_back({slot.id,index,slot.visualHandle,std::bit_cast<std::int32_t>(word(slot.plan.slotImage,0x60)),slot.plan.animation.script,std::bit_cast<std::int32_t>(word(slot.plan.slotImage,4)),word(slot.plan.slotImage,0x58),std::bit_cast<std::int32_t>(word(slot.plan.slotImage,0x48)),std::bit_cast<float>(word(slot.plan.slotImage,0x2c)),
            positionAt(slot.plan.slotImage,0x14),positionAt(slot.plan.slotImage,0x20),state,resource->sprites[std::size_t(state.sprite)]});
    }
    return output;
}
}
