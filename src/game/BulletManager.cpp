#include "BulletManager.hpp"
#include "Extension1048576.hpp"

#include <bit>
#include <cmath>
#include <limits>
#include <utility>

namespace th128::bullet {

bool initializeBirthExtension1048576(Birth& birth, Position player,
    const std::function<bool(int, std::string&)>& sound, std::string& error) {
    error.clear();
    const auto& first = birth.state.extension0;
    const auto& following = birth.state.laterExtensions[0];
    if (birth.type != 20 || birth.script != 74 || birth.age != 0 || birth.lifeClock != 0
        || birth.lifeTime != 0 || birth.state.motionState != 1 || birth.updateExecuting
        || !birth.extensionFailure.empty() || birth.state.extensionIndex != 0
        || birth.state.activeExtensions != 0 || birth.extension4.active
        || first.type != 0x100000 || first.callback != 0
        || following.type != 4 || following.control != 0 || following.callback != 0) {
        error = "initial1048576 requires fresh type20/74 and original following4 owner";
        return false;
    }
    const auto consumed = consumeExtension1048576(first, 0, birth.state.flags, error);
    if (!consumed) return false;
    const auto initialized = initializeExtension4(following, birth.angle, birth.state.position, player);
    if (!initialized) {
        error = "initial1048576 following4 initialization rejected";
        return false;
    }
    birth.state.flags = consumed->flags;
    birth.state.extensionIndex = consumed->nextExtensionIndex;
    birth.extension4 = *initialized;
    birth.state.activeExtensions |= 4u;
    if (birth.state.transformSound >= 0) {
        try {
            if (!sound || !sound(birth.state.transformSound, error) || !error.empty()) {
                if (error.empty()) error = "initial1048576 following4 sound owner rejected";
                birth.extensionFailure = error;
                return false;
            }
        } catch (...) {
            error = "initial1048576 following4 sound owner threw";
            birth.extensionFailure = error;
            return false;
        }
    }
    ++birth.state.extensionIndex;
    birth.extensionListEmpty = nextExtensionEmpty(birth.state);
    return true;
}

bool Profile::load(const void* bytes, std::size_t size, std::string& error) {
    return loadTyped(8,bytes,size,error);
}
bool Profile::loadTyped(int selectedType,const void* bytes,std::size_t size,std::string& error){
    error.clear();
    if (!bytes || size != 288||(selectedType!=4&&selectedType!=6&&selectedType!=8&&selectedType!=9&&selectedType!=10&&selectedType!=13&&selectedType!=16&&selectedType!=18&&selectedType!=20&&selectedType!=21&&selectedType!=23&&selectedType!=24&&selectedType!=25&&selectedType!=29&&selectedType!=30&&selectedType!=32&&selectedType!=33)) { error = "supported typed profile requires exactly 288 original bytes"; return false; }
    const auto* input = static_cast<const unsigned char*>(bytes);
    Profile candidate;
    candidate.type=selectedType;
    for (std::size_t index = 0; index < candidate.words_.size(); ++index) {
        const auto offset = index * 4;
        candidate.words_[index] = std::uint32_t(input[offset]) | (std::uint32_t(input[offset + 1]) << 8)
            | (std::uint32_t(input[offset + 2]) << 16) | (std::uint32_t(input[offset + 3]) << 24);
    }
    candidate.script = std::bit_cast<std::int32_t>(candidate.words_[0]);
    candidate.secondaryScript = std::bit_cast<std::int32_t>(candidate.words_[0x11c / 4]);
    candidate.cancellationMode_ = std::bit_cast<std::int32_t>(candidate.words_[0x110 / 4]);
    candidate.tail = {selectedType, std::bit_cast<float>(candidate.words_[0x104 / 4]),
                      std::bit_cast<float>(candidate.words_[0x108 / 4]),
                      std::bit_cast<std::int32_t>(candidate.words_[0x10c / 4])};
    candidate.tail.freezePolicy=std::bit_cast<std::int32_t>(candidate.words_[0x118 / 4]);
    const bool typed24=selectedType==24,typed25=selectedType==25,typed30=selectedType==30;
    if (candidate.script < 0 || candidate.script > 4095
        || (typed24 ? candidate.secondaryScript != 284 : typed25 ? candidate.secondaryScript != 103 : typed30 ? candidate.secondaryScript != 286 : candidate.secondaryScript != 0)
        || candidate.cancellationMode_ != (typed24 ? 7 : typed25 ? 8 : typed30 ? 9 : 6) || !std::isfinite(candidate.tail.hitboxSize)
        || candidate.tail.hitboxSize <= 0 || candidate.tail.hitboxSize > 4096
        || !std::isfinite(candidate.tail.auxiliarySize) || candidate.tail.auxiliarySize < 0
        || candidate.tail.auxiliarySize > 4096) {
        error = "unsupported typed profile layout or secondary ANM";
        return false;
    }
    if(selectedType==4&&(candidate.script!=39||candidate.tail.hitboxSize!=4||candidate.tail.auxiliarySize!=16||candidate.tail.value!=3)){
        error="type4 requires original script39/profile fields";return false;
    }
    if(selectedType==9&&(candidate.script!=44||candidate.secondaryScript!=0
        ||candidate.tail.hitboxSize!=2.4f||candidate.tail.auxiliarySize!=16.f
        ||candidate.tail.value!=4||candidate.tail.freezePolicy!=0
        ||candidate.cancellationMode_!=6||candidate.identitySpriteRemap())){
        error="type9 requires its original script44/profile fields";return false;
    }
    if(selectedType==10&&(candidate.script!=45||candidate.tail.hitboxSize!=2.4f||candidate.tail.auxiliarySize!=16||candidate.tail.value!=4)){
        error="type10 requires original script45/profile fields";return false;
    }
    if(selectedType==6&&(candidate.script!=41||candidate.secondaryScript!=0
        ||candidate.tail.hitboxSize!=4.0f||candidate.tail.auxiliarySize!=16.0f
        ||candidate.tail.value!=3||candidate.tail.freezePolicy!=0
        ||candidate.cancellationMode_!=6)){
        error="type6 requires its original script41/profile fields";return false;
    }
    if(selectedType==13&&(candidate.script!=48||candidate.secondaryScript!=0
        ||candidate.tail.hitboxSize!=2.4f||candidate.tail.auxiliarySize!=16
        ||candidate.tail.value!=4||candidate.tail.freezePolicy!=0||candidate.cancellationMode_!=6)){
        error="type13 requires its original script48/profile fields";return false;
    }
    if(selectedType==16&&(candidate.script!=51||candidate.secondaryScript!=0
        ||candidate.tail.hitboxSize!=4.f||candidate.tail.auxiliarySize!=16
        ||candidate.tail.value!=3||candidate.tail.freezePolicy!=0
        ||candidate.cancellationMode_!=6)){
        error="type16 requires its original script51/profile fields";return false;
    }
    if(selectedType==18&&(candidate.script!=72||candidate.tail.hitboxSize!=8.5f||candidate.tail.auxiliarySize!=22||candidate.tail.value!=1)){
        error="type18 requires its original script72/profile fields";return false;
    }
    if(selectedType==20&&(candidate.script!=74||candidate.tail.hitboxSize!=7.f||candidate.tail.auxiliarySize!=22||candidate.tail.value!=2)){
        error="type20 requires its original script74/profile fields";return false;
    }
    if(selectedType==21&&(candidate.script!=75||candidate.secondaryScript!=0
        ||candidate.tail.hitboxSize!=6.f||candidate.tail.auxiliarySize!=22
        ||candidate.tail.value!=2||candidate.tail.freezePolicy!=0
        ||candidate.cancellationMode_!=6)){
        error="type21 requires its original script75/profile fields";return false;
    }
    if(selectedType==23&&(candidate.script!=77||candidate.tail.hitboxSize!=7.f||candidate.tail.auxiliarySize!=22||candidate.tail.value!=1)){error="type23 requires its original profile";return false;}
    if(selectedType==24&&(candidate.script!=283||candidate.secondaryScript!=284
        ||candidate.tail.hitboxSize!=6.f||candidate.tail.auxiliarySize!=22
        ||candidate.tail.value!=2||candidate.tail.freezePolicy!=1
        ||candidate.cancellationMode_!=7||!candidate.identitySpriteRemap())){
        error="type24 requires its original 283/284 mode7 profile";return false;
    }
    if(selectedType==29&&(candidate.script!=282||candidate.tail.hitboxSize!=2.4f||candidate.tail.auxiliarySize!=12||candidate.tail.value!=5)){error="type29 requires its original profile";return false;}
    if(selectedType==25&&(candidate.script!=102||candidate.secondaryScript!=103
        ||candidate.tail.hitboxSize!=6.f||candidate.tail.auxiliarySize!=22
        ||candidate.tail.value!=2||candidate.tail.freezePolicy!=1
        ||candidate.cancellationMode_!=8||candidate.words_[1]<0x80000000u)){
        error="type25 requires its original profile";return false;
    }
    if(selectedType==30&&(candidate.script!=285||candidate.secondaryScript!=286
        ||candidate.tail.hitboxSize!=4.f||candidate.tail.auxiliarySize!=22
        ||candidate.tail.value!=2||candidate.tail.freezePolicy!=1
        ||candidate.cancellationMode_!=9||candidate.words_[1]<0x80000000u)){
        error="type30 requires its original profile";return false;
    }
    if(selectedType==32&&(candidate.script!=54||candidate.secondaryScript!=0
        ||candidate.tail.hitboxSize!=3.2f||candidate.tail.auxiliarySize!=16
        ||candidate.tail.value!=4||candidate.tail.freezePolicy!=0
        ||candidate.cancellationMode_!=6||candidate.identitySpriteRemap())){
        error="type32 requires its original profile";return false;
    }
    if(selectedType==33&&(candidate.script!=55||candidate.secondaryScript!=0
        ||candidate.tail.hitboxSize!=4.f||candidate.tail.auxiliarySize!=16.f
        ||candidate.tail.value!=3||candidate.tail.freezePolicy!=0
        ||candidate.cancellationMode_!=6||candidate.identitySpriteRemap())){
        error="type33 requires its original script55/profile fields";return false;
    }
    *this = candidate;
    return true;
}

bool Profile::identitySpriteRemap() const noexcept {
    return words_[1] >= 0x80000000u;
}

std::optional<std::int32_t> Profile::remap(std::int32_t color, std::int32_t sprite) const {
    if (script < 0 || color < 0 || color > 15 || sprite < 0 || sprite > 65535) return std::nullopt;
    if (identitySpriteRemap()) return sprite;
    if (sprite > 3) return std::nullopt;
    const auto selected = std::bit_cast<std::int32_t>(words_[1 + std::size_t(color) * 4 + std::size_t(sprite)]);
    if (selected < 0 || selected > 65535) return std::nullopt;
    return selected;
}
std::optional<std::int32_t> Profile::cancellationScript(std::int32_t color) const {
    if(script<0||color<0||color>15)return std::nullopt;
    if(cancellationMode_==7)return 0x177;
    if(cancellationMode_==8)return 0x173;
    if(cancellationMode_==9)return 0x17d;
    if(cancellationMode_!=6)return std::nullopt;
    const auto selected=std::bit_cast<std::int32_t>(words_[4+std::size_t(color)*4]);
    if(selected< -1||selected>4095)return std::nullopt;
    return selected;
}

void Manager::reset() {
    if (executing_) return;
    for (auto& slot : slots_) slot.reset();
    sounds_.clear();
    constructor_ = {};
    updater_ = {};
    soundSink_={};
    cursor_ = births_ = 0;
    nextId_ = 1;
    saturated_ = false;
    nestedActive_ = false;
    nestedConstruction_ = false;
    nestedParentSlot_ = Capacity;
    nestedParentId_ = 0;
    cancellationFailure_.clear();
}

bool Manager::setConstructor(Constructor constructor) {
    if (executing_) return false;
    constructor_ = std::move(constructor);
    return true;
}

bool Manager::setUpdater(Updater updater) {
    if(executing_)return false;
    updater_=std::move(updater);return true;
}
bool Manager::setUpdater(LegacyUpdater updater) {
    if(executing_||!updater)return false;
    auto legacy=std::move(updater);
    return setUpdater(Updater([legacy=std::move(legacy)](Birth& birth,bool& removed,std::string& error,const NestedEmitter&){
        return legacy(birth,removed,error);
    }));
}
bool Manager::setSoundSink(SoundSink sink){if(executing_)return false;soundSink_=std::move(sink);return true;}
bool Manager::hasUpdater() const {return bool(updater_);}

bool Manager::isCurrentUpdatingBirth(const Birth& birth) const {
    return executing_&&nestedActive_&&!nestedConstruction_&&birth.slot<slots_.size()
        &&nestedParentSlot_==birth.slot&&nestedParentId_==birth.id
        &&slots_[birth.slot]&&(&*slots_[birth.slot]==&birth);
}

bool Manager::tick(std::string& error) {
    error.clear();
    if(!cancellationFailure_.empty()){error=cancellationFailure_;return false;}
    if(executing_){error="bullet manager update is not reentrant";return false;}
    if(activeCount()==0)return true;
    if(!updater_){error="bullet update callback is not installed";return false;}
    executing_=true;
    if(nestedEpoch_==std::numeric_limits<std::uint64_t>::max()){executing_=false;error="bullet nested epoch exhausted";return false;}
    ++nestedEpoch_;
    bool success=true;
    for(std::size_t slotIndex=0;slotIndex<slots_.size();++slotIndex) {
        auto& slot=slots_[slotIndex];
        if(!slot)continue;
        nestedActive_=true;
        nestedParentSlot_=slotIndex;
        nestedParentId_=slot->id;
        Birth* expectedParent=&*slot;
        const auto epoch=nestedEpoch_;
        const NestedEmitter nested=[this,epoch,slotIndex,parentId=nestedParentId_,expectedParent](Birth& parent,const NestedEmissionRequest& request,std::string& nestedError){
            if(!nestedActive_||nestedConstruction_||!executing_||nestedEpoch_!=epoch||nestedParentSlot_!=slotIndex
                ||nestedParentId_!=parentId||&parent!=expectedParent){
                nestedError="nested emission requires the canonical current parent slot/id/update epoch";return false;
            }
            return emitNestedLocked(request,nestedError);
        };
        bool removed=false;
        try{success=updater_(*slot,removed,error,nested);}
        catch(...){error="bullet updater threw";success=false;}
        nestedActive_=false;
        nestedParentSlot_=Capacity;
        nestedParentId_=0;
        if(!success){if(error.empty())error="bullet update host rejected";break;}
        if(removed){slot.reset();continue;}
        if(slot->age==std::numeric_limits<std::int32_t>::max()){error="bullet age budget exceeded";success=false;break;}
        ++slot->age;
    }
    nestedActive_=false;
    nestedParentSlot_=Capacity;
    nestedParentId_=0;
    executing_=false;
    return success;
}
bool Manager::cancelAll(Cancellation cancellation,std::string& error){
    error.clear();
    if(!cancellationFailure_.empty()){error=cancellationFailure_;return false;}
    if(executing_||!cancellation){error="bullet cancellation requires a non-reentered effect host";return false;}
    executing_=true;bool success=true;
    for(auto& slot:slots_){
        if(!slot||(slot->state.motionState!=1&&slot->state.motionState!=2))continue;
        slot->state.motionState=3;
        try{success=cancellation(*slot,error)&&error.empty();}
        catch(...){error="bullet cancellation host threw";success=false;}
        if(!success){if(error.empty())error="required bullet cancellation rejected";cancellationFailure_=error;break;}
        slot->age=0;
    }
    executing_=false;return success;
}
bool Manager::cancelRadius(Position center,float radius,Cancellation cancellation,std::string& error){
    error.clear();
    if(!cancellationFailure_.empty()){error=cancellationFailure_;return false;}
    if(executing_||!cancellation||!std::isfinite(radius)||std::abs(radius)>65536||!std::isfinite(center.x)||!std::isfinite(center.y)){
        error="radius cancellation requires bounded geometry and a non-reentered effect host";return false;
    }
    executing_=true;bool success=true;
    for(auto& slot:slots_){
        if(!slot||(slot->state.motionState!=1&&slot->state.motionState!=2))continue;
        const float horizontal=float(double(slot->state.position.x)-center.x),vertical=float(double(slot->state.position.y)-center.y);
        const float reach=float(double(slot->state.hitboxSize)*.5+radius);
        const float squared=float(double(horizontal)*horizontal+double(vertical)*vertical);
        if(double(squared)>double(reach)*reach)continue;
        try{success=cancellation(*slot,error)&&error.empty();}
        catch(...){error="radius cancellation host threw";success=false;}
        if(!success){if(error.empty())error="required radius cancellation rejected";cancellationFailure_=error;break;}
        auto& position=slot->state.position;const auto velocity=slot->state.velocity;
        position.x=float(double(position.x)+float(double(velocity.x)*.5));position.y=float(double(position.y)+float(double(velocity.y)*.5));position.z=float(double(position.z)+float(double(velocity.z)*.5));
        slot->state.motionState=4;
    }
    executing_=false;return success;
}
bool Manager::consume(std::size_t slot,std::uint32_t identifier,std::string& error){
    error.clear();
    if(executing_||!cancellationFailure_.empty()||slot>=slots_.size()||!slots_[slot]||slots_[slot]->id!=identifier){
        error=cancellationFailure_.empty()?"original bullet consume requires matching live slot/identity":cancellationFailure_;return false;
    }
    slots_[slot].reset();return true;
}

std::optional<CallbackSnapshot> Manager::callbackSnapshot(std::size_t slot) const {
    if(slot>=slots_.size()||!slots_[slot])return std::nullopt;
    const auto& birth=*slots_[slot];
    return CallbackSnapshot{slot,birth.id,birth.type,birth.state.motionState,
        birth.state.position,birth.angle,birth.speed,birth.state.velocity};
}
bool Manager::applyCallbackMutations(std::span<const CallbackMutation> mutations,std::string& error){
    error.clear();
    if(executing_){error="bullet callback mutation is not reentrant";return false;}
    std::array<bool,Capacity> seen{};
    for(const auto& mutation:mutations){
        if(mutation.slot>=Capacity||mutation.slot>=slots_.size()||!slots_[mutation.slot]||
            slots_[mutation.slot]->id!=mutation.id){
            error="bullet callback mutation requires matching live slot/identity";return false;
        }
        if(seen[mutation.slot]){
            error="bullet callback mutation repeats a slot";return false;
        }
        seen[mutation.slot]=true;
        if(!std::isfinite(mutation.angle)||!std::isfinite(mutation.velocity.x)||
            !std::isfinite(mutation.velocity.y)||(mutation.speed&&!std::isfinite(*mutation.speed))){
            error="bullet callback mutation requires finite angle/x-y velocity";return false;
        }
    }
    executing_=true;
    for(const auto& mutation:mutations){
        auto& birth=*slots_[mutation.slot];
        birth.angle=mutation.angle;
        if(mutation.speed)birth.speed=*mutation.speed;
        birth.state.velocity.x=mutation.velocity.x;
        birth.state.velocity.y=mutation.velocity.y;
    }
    executing_=false;
    return true;
}
bool Manager::convertCallbackType25(std::size_t slot,std::uint32_t identifier,const Profile& profile,Constructor visualOwner,std::string& error){
    error.clear();
    if(!cancellationFailure_.empty()){error=cancellationFailure_;return false;}
    if(executing_||!visualOwner||slot>=Capacity||!slots_[slot]||slots_[slot]->id!=identifier){
        error="type25 conversion requires current non-reentered slot/id and real visual owner";return false;
    }
    auto& birth=*slots_[slot];
    if(!(birth.state.flags&1u)||birth.state.motionState!=1||birth.type==25||birth.updateExecuting
        ||!birth.extensionFailure.empty()||birth.color<0||birth.color>15
        ||profile.type!=25||profile.script!=102||profile.secondaryScript!=103
        ||profile.tail.hitboxSize!=6.f||profile.tail.auxiliarySize!=22.f||profile.tail.value!=2
        ||profile.tail.freezePolicy!=1||!profile.identitySpriteRemap()||profile.cancellationScript(birth.color)!=371){
        error="type25 conversion original live/profile domain rejected";return false;
    }
    executing_=true;
    birth.type=25;birth.script=102;
    birth.state.hitboxSize=6.f;birth.state.auxiliarySize=22.f;
    birth.state.profileValue=2;birth.state.freezePolicy=1;
    bool success=false;
    try{success=visualOwner(birth,error)&&error.empty();}
    catch(...){error="type25 conversion visual owner threw after original profile write";}
    if(success)birth.state.flags|=0x10u;
    else{if(error.empty())error="type25 conversion real visual owner rejected";cancellationFailure_=error;}
    executing_=false;
    return success;
}
bool Manager::emit(const Emitter& emitter, const Profile& profile, const Position& player,
                   float exclusionDistance, std::string& error) {
    return emitInternal(emitter,profile,player,exclusionDistance,false,0,error);
}
bool Manager::emitNestedLocked(const NestedEmissionRequest& request, std::string& error) {
    if(!executing_||!nestedActive_||nestedConstruction_){error="nested emission is only valid from the current canonical updater";return false;}
    return emitInternal(request.emitter,request.profile,request.player,0,true,request.birthStartIndex,error);
}
bool Manager::emitInternal(const Emitter& emitter, const Profile& profile, const Position& player,
                   float exclusionDistance, bool nested, int birthStartIndex, std::string& error) {
    error.clear();
    if(!cancellationFailure_.empty()){error=cancellationFailure_;return false;}
    if (!nested && executing_) { error = "bullet manager emit is not reentrant"; return false; }
    if (nested && (!executing_ || !nestedActive_)) { error = "nested emission is not active"; return false; }
    if (!constructor_) { error = "bullet constructor is not installed"; return false; }
    const bool initial1048576 = !nested && emitter.type == 20 && emitter.extension0.type == 0x100000
        && (emitter.extension0.control == 0 || emitter.extension0.control == 1)
        && (emitter.extension0.parameterA == 0 || emitter.extension0.parameterA == 1)
        && emitter.extension0.callback == 0 && emitter.laterExtensions[0].type == 4
        && emitter.laterExtensions[0].control == 0 && emitter.laterExtensions[0].callback == 0;
    if (!std::isfinite(exclusionDistance)||exclusionDistance<0 || (emitter.type != 4&&emitter.type != 6&&emitter.type != 8&&emitter.type!=9&&emitter.type!=10&&emitter.type!=13&&emitter.type!=16&&emitter.type!=18&&emitter.type!=20&&emitter.type!=21&&emitter.type!=23&&emitter.type!=24&&emitter.type!=25&&emitter.type!=29&&emitter.type!=30&&emitter.type!=33) || profile.script < 0 || profile.type != emitter.type
        || emitter.color < 0 || emitter.color > 15 || (emitter.aimMode > 3 && emitter.aimMode != 5)
        || (emitter.extension0.type != 0 && emitter.extension0.type != 2 && !initial1048576)) {
        error = "unsupported bullet type/color/aim/exclusion/extension lifecycle";
        return false;
    }
    const Position origin{emitter.positionX, emitter.positionY, emitter.positionZ};
    const auto aim = targetAnglePrefix(origin, player);
    const auto count = prefixCount(emitter);
    const auto cancellation=profile.cancellationScript(emitter.color);
    if(!cancellation){error="unsupported cancellation effect profile";return false;}
    if (!aim || !count || sounds_.size() >= 65536) { error = "bullet numeric or volley/sound budget"; return false; }
    if(!nested) executing_ = true;
    saturated_ = false;
    bool success = true;
    for (std::int32_t layer = 0; *count != 0 && layer < emitter.layers && success && !saturated_; ++layer) {
        for (std::int32_t index = 0; index < emitter.count && success; ++index) {
            std::size_t selected = cursor_;
            std::size_t scanned = 0;
            while (scanned < Capacity && slots_[selected]) { selected = (selected + 1) % Capacity; ++scanned; }
            if (scanned == Capacity) { saturated_ = true; break; }
            const auto geometry = emissionPrefix(emitter, index, layer, *aim);
            const auto position=geometry?spawnPositionPrefix(emitter,*geometry):std::nullopt;
            if(position&&exclusionDistance!=0){
                const double horizontal=double(position->x)-player.x,vertical=double(position->y)-player.y;
                const double distanceSquared=double(float(horizontal*horizontal+vertical*vertical));
                if(!std::isfinite(distanceSquared)){error="bullet exclusion numeric domain rejected";success=false;break;}
                if(distanceSquared<exclusionDistance)continue;
            }
            Emitter tailEmitter=emitter;
            int tailStart=0;
            if(nested){
                if(birthStartIndex<0||birthStartIndex>=18){error="nested birth start index domain";success=false;break;}
                const auto selected=extensionRecord(emitter,birthStartIndex);
                if(!selected){error="nested birth start record unavailable";success=false;break;}
                tailEmitter.extension0=*selected;
                tailStart=birthStartIndex;
            }
            const auto tail = geometry&&position ? spawnTailPrefix(tailEmitter, profile.tail, *position,
                {geometry->velocityX, geometry->velocityY, 0}, 0x13) : std::nullopt;
            if (!tail || nextId_ == 0 || births_ == std::numeric_limits<std::uint32_t>::max()) {
                error = "unsupported bullet geometry or birth budget type="+std::to_string(emitter.type)
                    +" color="+std::to_string(emitter.color)+" aimMode="+std::to_string(emitter.aimMode)
                    +" count="+std::to_string(emitter.count)+" layers="+std::to_string(emitter.layers)
                    +" index="+std::to_string(index)+" layer="+std::to_string(layer)
                    +" angle="+std::to_string(emitter.angle)+" spacing="+std::to_string(emitter.spacing)
                    +" speed="+std::to_string(emitter.speed)+" endSpeed="+std::to_string(emitter.endSpeed)
                    +" aim="+std::to_string(*aim)+" origin="+std::to_string(emitter.positionX)+","+std::to_string(emitter.positionY)+","+std::to_string(emitter.positionZ)
                    +" radius="+std::to_string(emitter.spawnRadius)+" geometry="+std::to_string(bool(geometry))
                    +" position="+std::to_string(bool(position))+" tail="+std::to_string(bool(tail))
                    +" nextId="+std::to_string(nextId_)+" births="+std::to_string(births_)
                    +" profileType="+std::to_string(profile.tail.type)+" hitbox="+std::to_string(profile.tail.hitboxSize)+" aux="+std::to_string(profile.tail.auxiliarySize)
                    +" ext0="+std::to_string(tailEmitter.extension0.type)+" extR="+std::to_string(tailEmitter.extension0.parameterR)+" extS="+std::to_string(tailEmitter.extension0.parameterS);
                success = false; break;
            }
            Birth birth{nextId_, std::uint32_t(selected), emitter.type, emitter.color, profile.script, *tail};
            birth.angle=geometry->angle;
            birth.speed=geometry->speed;
            if(nested){
                birth.state.position.z=emitter.positionZ;
                birth.state.extension0=emitter.extension0;
                birth.state.laterExtensions=emitter.laterExtensions;
                const int liveIndex=tailStart+tail->extensionIndex;
                if(liveIndex<0||liveIndex>18){error="nested live extension index domain";success=false;break;}
                birth.state.extensionIndex=liveIndex;
            }
            birth.extensionListEmpty=nextExtensionEmpty(birth.state);
            birth.cancellationScript=*cancellation;
            nestedConstruction_=true;
            try {
                const bool constructed=constructor_(birth, error);
                nestedConstruction_=false;
                if (!constructed) {
                    if (error.empty()) error = "bullet animation constructor rejected birth";
                    success = false;
                    break;
                }
            } catch (...) {
                nestedConstruction_=false;
                error = "bullet animation constructor threw"; success = false; break;
            }
            if (initial1048576 && (birth.state.extensionIndex != 2 || birth.state.activeExtensions != 4u
                || !birth.extension4.active || birth.extension4.time != 0 || birth.age != 0 || birth.lifeClock != 0)) {
                error = "initial1048576 birth constructor did not commit its actual dispatcher owner";
                success = false;
                break;
            }
            slots_[selected] = birth;
            ++nextId_;
            ++births_;
            cursor_ = (selected + 1) % Capacity;
        }
    }
    if(success&&(emitter.flags&0x20U)){
        const SoundRequest request{emitter.shotSound,emitter.positionX};
        const auto soundIndex=sounds_.size();sounds_.push_back(request);
        if(soundSink_){
#if defined(__cpp_exceptions)
            try{success=soundSink_(request,error)&&error.empty();}catch(...){error="bullet sound sink threw";success=false;}
#else
            success=soundSink_(request,error)&&error.empty();
#endif
            if(success)sounds_.erase(sounds_.begin()+static_cast<std::ptrdiff_t>(soundIndex));
        }
    }
    if(!nested) executing_ = false;
    return success;
}

bool Manager::release(std::size_t slot) {
    if (executing_ || slot >= Capacity || !slots_[slot]) return false;
    slots_[slot].reset();
    return true;
}
std::uint32_t Manager::activeCount() const {
    std::uint32_t count = 0;
    for (const auto& slot : slots_) if (slot) ++count;
    return count;
}
std::uint32_t Manager::birthCount() const { return births_; }
std::size_t Manager::cursor() const { return cursor_; }
bool Manager::saturated() const { return saturated_; }
const std::vector<SoundRequest>& Manager::sounds() const { return sounds_; }
std::vector<Birth> Manager::snapshots() const {
    std::vector<Birth> result;
    result.reserve(activeCount());
    for (const auto& slot : slots_) if (slot) result.push_back(*slot);
    return result;
}

}
