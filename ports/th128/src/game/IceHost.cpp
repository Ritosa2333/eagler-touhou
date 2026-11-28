#include "IceHost.hpp"
#include "IceContact.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>

namespace th128::ice {
namespace {
struct Guard {bool& value;explicit Guard(bool& flag):value(flag){value=true;}~Guard(){value=false;}};
constexpr std::array<std::size_t,5> MaskOffsets{0,84,184,316,708};
constexpr std::uint64_t OverlayBase=0x100000000ull;
bool contactCoordinate(float value){return std::isfinite(value)&&std::abs(value)<=65536.f;}
bool contactRadius(float value){return contactCoordinate(value)&&value>=0;}
void decrement(Timer& timer){timer.previous=timer.current;--timer.current;timer.time=float(timer.current);}
}
bool Host::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
bool Host::checked(std::string& error){
    if(!failure_.empty())return fail(failure_,error);
    const auto bank=scene_.registry.bank(36);
    for(const auto& [identifier,node]:scene_.nodes()){
        if(node->vm.faulted()||node->customGeometry||node->beforeTick||node->onInterrupt)
            return fail("unimplemented original ice ANM callback/geometry",error);
        const auto& state=node->vm.state();
        if(state.sprite>=0&&(state.spriteBank!=36||!bank||std::size_t(state.sprite)>=bank->sprites.size()))return fail("ice sprite bank mismatch",error);
        if(state.ended||state.deleteRequested)node->alive=false;
    }
    scene_.collect();return true;
}
bool Host::configure(std::span<const std::uint8_t> animation,std::span<const std::uint8_t> masks,
                     bullet::BulletVisuals& bullets,Services services,std::string& error){
    error.clear();if(configured_||executing_||masks.size()!=1228||!services.random||!services.displayRandom||!services.visual||!services.consume
        ||!services.sound||!services.damage||!services.expire||!services.complete){error="ice host requires actual masks and every owning service";return false;}
    anm::Registry candidate;if(!candidate.loadBank(36,animation.data(),animation.size(),error))return false;
    const auto bank=candidate.bank(36);if(!bank||bank->scripts.size()<=34){error="actual ice0..34 programs are required";return false;}
    scene_.registry=std::move(candidate);bitmaps_.assign(masks.begin(),masks.end());visuals_=&bullets;services_=std::move(services);
    scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.gameRandom32=[this]{return services_.random();};
    scene_.runtime.displayRandom32=[this]{return services_.displayRandom();};
    scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){if(failure_.empty())failure_="ice script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;};
    configured_=true;return true;
}
bool Host::bind(std::uint64_t key,int script,Position position,std::string& error){
    const auto* node=scene_.bind(key,std::uint32_t(key),36,script,anm::Membership::Primary,320+position.x,16+position.y,{},false,nullptr,0,0,23,false,position.z);
    if(!node||node->vm.faulted()||!failure_.empty())
        return fail(failure_.empty()?"original ice ANM binding failed":failure_,error);
    return true;
}
std::optional<std::uint32_t> Host::birth(IceBirthInput input,std::string& error){
    const auto selection=selectIceSlot(occupancy_,cursor_);
    if(!selection||selection->overwritesOccupied){fail("unproved occupied ice-slot overwrite",error);return std::nullopt;}
    auto& slot=slots_[selection->slot];
    const auto plan=prepareIceBirth(occupancy_,cursor_,input,slot.state,true);
    if(!plan){fail("unsupported original ice birth/bucket domain",error);return std::nullopt;}
    const auto bucket=(plan->requiredInsertions[0].managerHeadOffset-0x65a1cu)/12u;
    slot.state=plan->next;slot.bucket=bucket;
    buckets_[bucket].push_front(selection->slot);global_.push_front(selection->slot);slot.globalLink=global_.begin();
    occupancy_[selection->slot]=1;cursor_=plan->selection.nextCursor;++stats_.objects;
    return selection->slot;
}
bool Host::activate(int power,Position position,std::string& error){
    error.clear();if(!configured_||executing_||!failure_.empty()||power<3000||power>10000)
        return fail(failure_.empty()?"invalid original freeze activation":failure_,error);
    if(perfectBatch_){error="ordinary freeze cannot overlap a perfect freeze batch";return false;}
    Guard guard(executing_);const auto plan=planActivationIceBirth(power,position);
    if(!plan)return fail("unsupported activation position/power",error);
    const auto selected=std::find_if(activations_.begin(),activations_.end(),[](const auto& slot){return !slot.active;});
    if(selected==activations_.end())return fail("unproved saturated original activation-slot branch",error);
    *selected={nextAnimation_++,0,true};if(!selected->animation)return fail("ice animation handle overflow",error);
    if(!bind(selected->animation,33,position,error)||!services_.sound(47,position.x,error)||!error.empty())return fail(error,error);
    stats_.busy=1;stats_.power=power;stats_.elapsed=0;
    return bool(birth(plan->birth,error));
}
bool Host::charge(int held,Position position,std::string& error){
    error.clear();
    if(!configured_||executing_||!failure_.empty()||held< -1000000||held>=1000000
        ||!std::isfinite(position.x)||!std::isfinite(position.y)||!std::isfinite(position.z))
        return fail(failure_.empty()?"invalid original held-charge animation state":failure_,error);
    if(held<=0&&!charge_)return true;
    Guard guard(executing_);
    if(held>0){
        if(!charge_){charge_=nextAnimation_++;if(!charge_||!bind(charge_,30,position,error))return false;}
        auto* root=scene_.find(charge_);if(!root)return fail("live original ice charge animation disappeared",error);
        auto& state=root->vm.control();state.engineX=320+position.x;state.engineY=16+position.y;state.engineZ=position.z;
    }else if(charge_){scene_.interrupt(charge_,1);charge_=0;}
    return checked(error);
}
bool Host::contact(const bullet::Birth& bullet,const BucketHeads& heads,std::string& error) const {
    const auto position=bullet.state.position;
    if(!std::isfinite(position.x)||!std::isfinite(position.y)||!std::isfinite(bullet.state.hitboxSize)){error="nonfinite ice contact";return false;}
    const auto cell=iceCell({position.x,position.y,position.z});if(!cell){error="ice contact bucket overflow";return false;}
    const int column=(*cell)[0],row=(*cell)[1];
    if(column<0||column>=8||row<0||row>=10)return false;
    const bool validPoint=contactCoordinate(position.x)&&contactCoordinate(position.y)&&contactRadius(bullet.state.hitboxSize);
    constexpr std::array<std::array<int,2>,9> order{{{0,0},{-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}}};
    for(const auto& offset:order){
        const int horizontal=column+offset[0],vertical=row+offset[1];
        if(horizontal<0||horizontal>=8||vertical<0||vertical>=10)continue;
        const auto bucket=std::size_t(horizontal*10+vertical);const auto& slots=buckets_[bucket];
        for(auto it=validPoint?heads[bucket]:slots.cbegin();it!=slots.cend();++it){
            const auto& ice=slots_[*it].state;
            const auto overlap=contacts(ice.position,ice.value,ice.lifetime.current,ice.delay.current,
                {position.x,position.y,position.z},bullet.state.hitboxSize);
            if(!overlap){error="unsupported original ice overlap context";return false;}
            if(*overlap)return true;
        }
    }
    return false;
}
bool Host::convert(const bullet::Birth& bullet,std::span<std::uint8_t> mask,BucketHeads& heads,std::string& error){
    const auto* source=services_.visual(bullet.id);
    if(!source||source->faulted())return fail("live bullet VM missing before original freeze clone",error);
    const auto& position=bullet.state.position;
    const auto index=birth({{position.x,position.y,position.z},bullet.state.auxiliarySize,200,0},error);
    if(!index)return false;auto& slot=slots_[*index];slot.visual=*source;slot.visual->hasSecondary=false;
    // contacts() validates before checking the delay. Keep any out-of-domain
    // insertion in the scan so the original failure remains observable.
    if(slot.state.delay.current<=0||!contactCoordinate(slot.state.position.x)||!contactCoordinate(slot.state.position.y)||!contactRadius(slot.state.value))
        heads[slot.bucket]=buckets_[slot.bucket].cbegin();
    const auto rotation=bullet::directionalDrawAngle(bullet.angle,source->primary.state().rotation,source->primary.state().flags);
    if(!rotation)return fail("unsupported frozen bullet directional draw pose",error);
    slot.visual->primary.control().rotation=*rotation;
    slot.visual->primary.control().flags|=0x80000u;slot.visual->primary.control().layer=13;
    if(!services_.consume(bullet,error)||!error.empty())return fail(error.empty()?"owning bullet release rejected":error,error);
    if(!services_.sound(69,position.x,error)||!error.empty())return fail(error,error);
    const float radius=slot.state.value;
    const std::uint32_t selector=radius<=10.100000381469727f?1u:radius<=12.100000381469727f?2u:radius<=16.100000381469727f?3u:radius<=24.100000381469727f?4u:5u;
    const auto profile=selectIceMask(selector);
    const auto count=profile.height*profile.wordsPerRow*4;
    if(!updateIceMask(mask,{std::span(bitmaps_).subspan(MaskOffsets[selector-1],count),profile.height,profile.wordsPerRow},
        {position.x,position.y,position.z},IceMaskOperation::Merge))return fail("original bitmap merge rejected",error);
    maskPixelsDirty_=true;
    slot.state.maskSelector=selector;slot.state.animation54=nextAnimation_++;
    if(!bind(OverlayBase+slot.state.animation54,selector<=3?1:0,slot.state.position,error))return false;
    ++stats_.converted;return true;
}
bool Host::freezePerfect(std::span<const bullet::Birth> bullets,PerfectFreezeResult& result,std::string& error){
    error.clear();result=PerfectFreezeResult{};
    if(!configured_||executing_||!failure_.empty())return fail(failure_.empty()?"invalid perfect freeze host":failure_,error);
    if(perfectBatch_||stats_.busy||stats_.objects!=0)return fail("perfect freeze requires an exclusive empty ice batch",error);
    if(!services_.perfectComplete)return fail("perfect freeze requires its native batch completion owner",error);
    const auto existingMask=countIceMask(mask_);if(!existingMask||*existingMask!=0)
        return fail("perfect freeze requires an empty exclusive mask",error);
    scratchMask_.assign(IceMaskBytes,0);
    struct Eligible{const bullet::Birth* bullet;float rotation;};
    std::vector<Eligible> eligible;std::size_t skipped=0;
    Guard guard(executing_);
    try{
        for(const auto& bullet:bullets){
            if(!canFreezeBullet(bullet.state.flags,bullet.state.freezePolicy,bullet.lifeClock,true)){
                ++skipped;continue;
            }
            const auto cell=iceCell({bullet.state.position.x,bullet.state.position.y,bullet.state.position.z});
            if(!cell||(*cell)[0]<0||(*cell)[0]>7||(*cell)[1]<0||(*cell)[1]>9){++skipped;continue;}
            if(!std::isfinite(bullet.state.auxiliarySize)||std::abs(bullet.state.auxiliarySize)>0x1p24f)
                return fail("perfect freeze auxiliary size is outside the supported domain",error);
            const auto* source=services_.visual(bullet.id);
            if(!source||source->faulted())return fail("perfect freeze source visual is missing or faulted",error);
            const auto rotation=bullet::directionalDrawAngle(bullet.angle,source->primary.state().rotation,source->primary.state().flags);
            if(!rotation)return fail("unsupported perfect freeze directional draw pose",error);
            eligible.push_back({&bullet,*rotation});
        }
        if(eligible.empty()){
            perfectBatch_=true;stats_.busy=1;stats_.power=12000;stats_.elapsed=0;stats_.pixels=0;
            result={0,std::uint32_t(skipped),0,true};return true;
        }
        if(eligible.size()>SlotCount
            ||std::uint64_t(nextAnimation_)+eligible.size()>std::numeric_limits<std::uint32_t>::max())
            return fail("perfect freeze slot/effect budget exceeded",error);
        const auto bank=scene_.registry.bank(36);
        if(!bank||bank->scripts.size()<=1||bank->scripts[1].commands.empty())
            return fail("perfect freeze effect script1 is unavailable",error);
        const auto maskSelection=selectIceMask(1);
        if(maskSelection.disposition!=IceMaskDisposition::Known)
            return fail("perfect freeze selector1 mask is unavailable",error);
        const auto maskBytes=maskSelection.height*maskSelection.wordsPerRow*4;
        if(maskBytes==0||bitmaps_.size()<maskBytes)return fail("perfect freeze selector1 bitmap is truncated",error);
        for(const auto& entry:eligible){
            const auto& bullet=*entry.bullet;
            if(!updateIceMask(scratchMask_,{std::span<const std::uint8_t>(bitmaps_).subspan(0,maskBytes),
                maskSelection.height,maskSelection.wordsPerRow},{bullet.state.position.x,bullet.state.position.y,bullet.state.position.z},IceMaskOperation::Merge))
                return fail("perfect freeze selector1 mask preflight rejected",error);
        }
        std::uint32_t frozen=0;
        for(const auto& entry:eligible){
            const auto& bullet=*entry.bullet;
            const auto index=birth({{bullet.state.position.x,bullet.state.position.y,bullet.state.position.z},
                                    bullet.state.auxiliarySize,200,0},error);
            if(!index)return false;
            auto& slot=slots_[*index];perfectBatch_=true;
            const auto* source=services_.visual(bullet.id);
            if(!source||source->faulted())return fail("perfect freeze source visual disappeared before clone",error);
            slot.visual=*source;slot.visual->hasSecondary=false;
            slot.visual->primary.control().rotation=entry.rotation;
            slot.visual->primary.control().flags|=0x80000u;slot.visual->primary.control().layer=13;
            slot.state.maskSelector=1;slot.state.animation54=nextAnimation_++;
            if(!bind(OverlayBase+slot.state.animation54,1,slot.state.position,error))return false;
            if(!services_.consume(bullet,error)||!error.empty())
                return fail(error.empty()?"perfect freeze owning consume rejected":error,error);
            ++frozen;
        }
        const auto pixels=countIceMask(scratchMask_);if(!pixels)return fail("perfect freeze scratch mask count rejected",error);
        result={frozen,std::uint32_t(skipped),*pixels,true};
        std::copy(scratchMask_.begin(),scratchMask_.end(),mask_.begin());
        cachedMaskPixels_=*pixels;maskPixelsDirty_=false;
        perfectBatch_=true;stats_.busy=1;stats_.power=12000;stats_.elapsed=0;stats_.pixels=*pixels;
        std::fill(scratchMask_.begin(),scratchMask_.end(),0);return checked(error);
    }catch(...){return fail("perfect freeze owning callback threw",error);}
}
bool Host::shatterBatch(std::string& error){
    for(auto& bucket:buckets_)for(const auto index:bucket){
        auto& ice=slots_[index].state;const auto value=std::bit_cast<std::int32_t>(services_.random())%5;
        ice.lifetime={value-1,value,float(value),NativeClockSource,1};
        if(ice.mode==0){
            const auto radius=float(double(ice.value)*3.799999952316284);
            if(!services_.damage(ice.position,radius,error)||!error.empty())return fail(error.empty()?"original shatter damage source rejected":error,error);
            ++stats_.shattered;
        }
    }
    return true;
}
bool Host::shatter(std::uint64_t animation,std::string& error){
    scene_.interrupt(animation,1);if(!checked(error))return false;
    if(!shatterBatch(error))return false;
    if(!services_.sound(71,0,error)||!error.empty())return fail(error,error);
    return true;
}
bool Host::shatterPerfect(std::string& error){
    error.clear();
    if(!configured_||executing_||!failure_.empty())return fail(failure_.empty()?"invalid Perfect phase180 shatter":failure_,error);
    Guard guard(executing_);
    try{
        if(!services_.sound(71,0,error)||!error.empty())return fail(error.empty()?"original Perfect shatter sound rejected":error,error);
        return shatterBatch(error);
    }catch(...){return fail("original Perfect phase180 owning callback threw",error);}
}
bool Host::tick(std::span<const bullet::Birth> bullets,std::string& error){
    error.clear();if(!configured_||executing_||!failure_.empty())return fail(failure_.empty()?"invalid ice tick":failure_,error);
    if(perfectBatch_&&!services_.perfectComplete)for(const auto& bullet:bullets)
        if(canFreezeBullet(bullet.state.flags,bullet.state.freezePolicy,bullet.lifeClock)){
            error="perfect batch ordinary conversions require Services::perfectComplete";return false;
        }
    Guard guard(executing_);
    try {
        bool triggered=false;
        for(auto& activation:activations_)if(activation.active){
            if(activation.elapsed>=(stats_.power-2000)/80){
                if(!shatter(activation.animation,error))return false;activation.active=false;triggered=true;break;
            }
            stats_.elapsed=++activation.elapsed;
        }
        if(!triggered&&!bullets.empty()){
            // All conversions push to the front with a three-tick delay.
            // List iterators stay valid: a bullet need only see ice that can
            // already contact it, not ice just born earlier in this pass.
            BucketHeads heads;
            for(std::size_t i=0;i<buckets_.size();++i)heads[i]=buckets_[i].cbegin();
            for(const auto& bullet:bullets){
                if(!canFreezeBullet(bullet.state.flags,bullet.state.freezePolicy,bullet.lifeClock))continue;
                const bool overlap=contact(bullet,heads,error);if(!error.empty())return fail(error,error);
                if(overlap&&!convert(bullet,mask_,heads,error))return false;
            }
        }
        for(auto& bucket:buckets_)for(auto cursor=bucket.begin();cursor!=bucket.end();){
            const auto current=cursor++;const auto index=*current;auto& slot=slots_[index];decrement(slot.state.lifetime);
            if(slot.state.delay.current>0)decrement(slot.state.delay);
            if(slot.state.lifetime.current>0)continue;
            if(slot.state.mode==0&&(!services_.expire(slot.state.position,error)||!error.empty()))return fail(error.empty()?"original ice expiration rejected":error,error);
            if(slot.state.animation54)scene_.remove(OverlayBase+slot.state.animation54);
            slot.visual.reset();slot.state.occupied=0;slot.state.animation50=slot.state.animation54=0;
            slot.state.lifetime={-1,0,0,NativeClockSource,1};
            occupancy_[index]=0;bucket.erase(current);global_.erase(slot.globalLink);slot.globalLink={};--stats_.objects;
        }
        if(stats_.busy&&stats_.objects==0){
            const auto pixels=countIceMask(mask_);if(!pixels)return fail("original ice area mask invalid",error);
            const Completion completion{stats_.power,*pixels,float(double(*pixels)/172032.*100.)};
            const auto& complete=perfectBatch_?services_.perfectComplete:services_.complete;
            if(!complete||!complete(completion,error)||!error.empty())return fail(error.empty()?"freeze completion accounting/presentation rejected":error,error);
            stats_.pixels=*pixels;++stats_.completions;stats_.busy=0;mask_.fill(0);
            perfectBatch_=false;cachedMaskPixels_=0;maskPixelsDirty_=false;
        }
        return checked(error);
    }catch(...){return fail("ice owning callback threw",error);}
}
bool Host::tickAnimations(std::string& error){
    error.clear();if(!configured_||executing_||!failure_.empty())return fail(failure_.empty()?"invalid ice animation phase":failure_,error);
    Guard guard(executing_);
    for(auto& slot:slots_)if(slot.visual&&!visuals_->advance(*slot.visual,1,error))return fail(error,error);
    scene_.tickPrimary();scene_.tickSecondary();return checked(error);
}
Statistics Host::statistics() const {
    auto result=stats_;
    if(result.busy){
        if(maskPixelsDirty_){cachedMaskPixels_=countIceMask(mask_).value_or(0);maskPixelsDirty_=false;}
        result.pixels=cachedMaskPixels_;
    }
    return result;
}
std::vector<IceObjectState> Host::objects() const {std::vector<IceObjectState> result;for(const auto index:global_)result.push_back(slots_[index].state);return result;}
bool Host::pose(const anm::Node& node,Draw& draw,std::size_t depth) const {
    if(depth>=64)return false;const auto& state=node.vm.state();
    draw.state=state;draw.x=state.x+state.engineX+state.offsetX;draw.y=state.y+state.engineY+state.offsetY;draw.z=state.z+state.engineZ+state.offsetZ;
    if(node.parent){const auto parent=scene_.nodes().find(node.parent);Draw inherited;
        if(parent==scene_.nodes().end()||!parent->second->alive||!pose(*parent->second,inherited,depth+1))return false;
        draw.x+=inherited.x;draw.y+=inherited.y;draw.z+=inherited.z;
        draw.state.sx*=inherited.state.sx;draw.state.sy*=inherited.state.sy;if(state.mode==1)draw.state.rotation+=inherited.state.rotation;
    }
    return std::isfinite(draw.x)&&std::isfinite(draw.y)&&std::isfinite(draw.z);
}
std::vector<Draw> Host::draws() const {
    std::vector<Draw> result;const auto bank=scene_.registry.bank(36);if(!configured_||!bank)return result;
    for(const auto membership:{anm::Membership::Primary,anm::Membership::Secondary})for(const auto* node:scene_.chain(membership)){
        const auto& state=node->vm.state();if(!node->alive||state.sprite<0||std::size_t(state.sprite)>=bank->sprites.size())continue;
        Draw draw;draw.id=node->id;draw.sprite=bank->sprites[std::size_t(state.sprite)];if(pose(*node,draw))result.push_back(draw);
    }
    for(std::uint32_t index=0;index<SlotCount;++index){const auto& slot=slots_[index];if(!slot.visual)continue;
        const auto* state=visuals_->animationState(*slot.visual);const auto* sprite=state?visuals_->spriteInfo(state->sprite):nullptr;if(!sprite)continue;
        const auto& position=slot.state.position;result.push_back({100000+index,0,*state,*sprite,
            320+position.x+state->x+state->engineX+state->offsetX,16+position.y+state->y+state->engineY+state->offsetY,
            position.z+state->z+state->engineZ+state->offsetZ});
    }
    return result;
}
}
