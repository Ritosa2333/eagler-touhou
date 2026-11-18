#include "PerfectFreezeHost.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace th128::perfect {
namespace {
struct Guard {
    bool& state;
    explicit Guard(bool& value):state(value){state=true;}
    ~Guard(){state=false;}
};

bool finite(float value){return std::isfinite(value)&&std::abs(value)<=65536.f;}
bool validCharge(int value){return value>=0&&value<=freeze::PerfectFreezeCapRaw;}

bool validPose(const Context& context){
    return finite(context.x)&&finite(context.y)&&finite(context.radius)&&context.radius>0.f;
}

bool validLaser(const Context& context,bool allowPopulated=false){
    return (context.lasers==LaserContext::Empty&&context.laserCount==0)
        ||(allowPopulated&&context.lasers==LaserContext::Populated&&context.laserCount>0);
}
}

Host::~Host(){removeEffect();}

bool Host::fail(std::string reason,std::string& error){
    if(failure_.empty())failure_=std::move(reason);
    error=failure_;return false;
}

bool Host::deny(std::string reason,std::string& error){
    lastDenied_=std::move(reason);error=lastDenied_;return false;
}

bool Host::reject(std::string reason,std::string& error){
    error.clear();
    if(!configured_||executing_){error="perfect freeze host is not configured";return false;}
    return deny(std::move(reason),error);
}

bool Host::configure(const Context& context,Services services,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(configured_||executing_){error="perfect freeze host requires a fresh configuration";return false;}
    if(!validCharge(context.perfectCharge)||context.mode!=Mode::Normal||!validPose(context)
        ||!services.freezeBullets||!services.protect||!services.shatterIce||!validLaser(context)){
        error="invalid perfect freeze context or required services";return false;
    }
    context_=context;services_=std::move(services);charge_=context.perfectCharge;
    configured_=true;lastDenied_.clear();return true;
}

bool Host::loadNativeEffect(std::span<const std::uint8_t> animation,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(configured_||executing_||effectResources_||animation.empty()){
        error="invalid fresh perfect freeze effect resource";return false;
    }
    anm::Registry candidate;
    if(!candidate.loadBank(NativeEffectBank,animation.data(),animation.size(),error)){
        return fail(error.empty()?"perfect freeze effect bank load failed":error,error);
    }
    const auto loaded=candidate.bank(NativeEffectBank);
    if(!loaded||std::size_t(NativeEffectScript)>=loaded->scripts.size()
        ||loaded->scripts[std::size_t(NativeEffectScript)].commands.empty()){
        error="perfect freeze effect script is missing";return false;
    }
    scene_.registry=std::move(candidate);
    scene_.runtime={};
    scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.gameRandom32=[this]{if(failure_.empty())failure_="unproven perfect freeze effect game RNG";return 0u;};
    scene_.runtime.displayRandom32=[this]{if(failure_.empty())failure_="unproven perfect freeze effect display RNG";return 0u;};
    scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){
        if(failure_.empty())failure_="perfect freeze effect script="+std::to_string(script)+
            " opcode="+std::to_string(opcode)+": "+(reason?reason:"unsupported");
    };
    effectResources_=true;return true;
}

bool Host::setCharge(int charge,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_||executing_||active_||!validCharge(charge)){error="invalid perfect freeze charge update";return false;}
    charge_=charge;context_.perfectCharge=charge;return true;
}

void Host::removeEffect(){
    if(effectHandle_){
        scene_.remove(effectKey_);
        scene_.collect();
    }
    effectHandle_=0;effectKey_=0;
}

bool Host::callEffect(const EffectRequest& request,std::string& error){
    if(!services_.effect){error="perfect freeze effect service is unavailable";return false;}
    try{
        if(!services_.effect(request,error)||!error.empty()){
            return fail(error.empty()?"required perfect freeze effect rejected":error,error);
        }
    }catch(...){return fail("perfect freeze effect threw",error);}
    return true;
}

bool Host::activate(const Context& input,std::string& error,bool keepCharge){return activateOwned(input,error,keepCharge,false);}
bool Host::activatePracticeDeathbomb(const Context& input,std::string& error,bool keepCharge){return activateOwned(input,error,keepCharge,true);}
bool Host::activateOwned(const Context& input,std::string& error,bool keepCharge,bool deathbomb){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_||executing_||active_)return deny("perfect freeze host is not ready for a fresh activation",error);
    if(input.mode!=(deathbomb?Mode::DeathbombWindow:Mode::Normal))return deny("unsupported owned perfect freeze activation mode",error);
    if(!validCharge(input.perfectCharge)||input.perfectCharge!=charge_){
        return deny("perfect freeze charge context is stale",error);
    }
    if(!(input.keys&XKeyBit))return deny("perfect freeze requires the X key",error);
    if(!input.systemPresent||input.systemMode3c!=0||input.busy!=0||input.playerState!=(deathbomb?4:1)
        ||!validPose(input)||!validLaser(input,!deathbomb&&bool(services_.freezeLasers))){
        return deny("perfect freeze input gate is not satisfied",error);
    }
    if(input.perfectCharge<CostRaw)return deny("perfect freeze charge stock is insufficient",error);
    if(!effectResources_&&!services_.effect)return deny("perfect freeze effect owner is unavailable",error);
    const auto remaining=freeze::spendPerfectFreezeRaw(input.perfectCharge);
    if(!remaining)return deny("unsupported perfect freeze charge spend",error);
    Guard guard(executing_);
    BulletResult bullets;
    int lasers=0;
    bulletRequest_={input.x,input.y,input.radius,ProtectionTicks,true,false};
    try{
        if(!services_.protect(deathbomb?DeathbombProtectionTicks:ProtectionTicks,error)||!error.empty()){
            return fail(error.empty()?"required perfect freeze protection rejected":error,error);
        }
        const EffectRequest effectRequest{input.x,input.y,
                                          effectResources_?NativeEffectLayer:-1,
                                          effectResources_?NativeEffectScript:-1};
        if(effectResources_){
            removeEffect();
            effectKey_=1;
            const auto* root=scene_.bind(effectKey_,1,NativeEffectBank,NativeEffectScript,anm::Membership::Primary,
                                        0.f,0.f,{},false,nullptr,224,16,NativeEffectLayer);
            if(!root){return fail("perfect freeze effect root bind failed",error);}
            effectHandle_=root->id;
            scene_.tickPrimary();
            scene_.collect();
            if(!failure_.empty())return fail(failure_,error);
        }else if(!callEffect(effectRequest,error))return false;
    }catch(...){return fail("perfect freeze required service threw",error);}
    charge_=keepCharge?input.perfectCharge:*remaining;
    context_=input;context_.perfectCharge=charge_;context_.xPressed=false;
    protection_=ProtectionTicks;active_=true;phase_=0;freezeDone_=false;
    lastDenied_.clear();
    activation_=Activation{input.perfectCharge,charge_,ProtectionTicks,bullets,lasers,true};
    return true;
}

bool Host::tick(bool paused,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_||executing_){error="perfect freeze host is not configured";return false;}
    if(!active_||paused)return true;
    Guard guard(executing_);
    if(protection_>0)--protection_;
    if(phase_<ProtectionTicks)++phase_;
    if(phase_<=NativeLaserPhase){
        try{
            if(!services_.protect(PhaseFreezeProtectionTicks,error)||!error.empty()){
                return fail(error.empty()?"required native protection10 rejected":error,error);
            }
        }catch(...){return fail("perfect freeze native protection10 service threw",error);}
    }
    if(phase_==NativeOverridePhase&&!freezeDone_){
        bulletRequest_.overrideImmune=true;
        BulletResult bullets;
        error.clear();
        try{
            if(services_.freezeLasers&&!services_.freezeLasers(context_.laserCount,error)){
                return fail(error.empty()?"required perfect laser manager rejected":error,error);
            }
            if(!services_.freezeBullets(bulletRequest_,bullets,error)||!error.empty()||!bullets.completed){
                return fail(error.empty()?"required perfect freeze bullet owner rejected":error,error);
            }
        }catch(...){return fail("perfect freeze bullet service threw",error);}
        freezeDone_=true;
        removeEffect();
        if(activation_)activation_->bullets=bullets;
    }
    if(phase_==NativeLaserPhase){
        try{
            if(!services_.shatterIce(error)||!error.empty()){
                return fail(error.empty()?"required native phase180 ice shatter rejected":error,error);
            }
        }catch(...){return fail("perfect freeze native phase180 shatter threw",error);}
    }
    if(effectHandle_){
        scene_.tickPrimary();
        scene_.collect();
        if(!failure_.empty())return fail(failure_,error);
        if(!scene_.nodes().count(effectHandle_))effectHandle_=0;
    }
    if(protection_==0)active_=false;
    return true;
}

bool Host::shutdown(std::string& error){
    error.clear();
    if(executing_){error="perfect freeze shutdown during an active callback";return false;}
    removeEffect();
    active_=false;protection_=0;phase_=0;freezeDone_=false;activation_.reset();lastDenied_.clear();
    return true;
}

Status Host::status() const{
    Status status;
    status.configured=configured_;status.active=active_;status.failed=!failure_.empty();
    status.charge=charge_;status.phase=phase_;
    status.protectionTicks=0;
    if(active_){
        if(phase_==0)status.protectionTicks=ProtectionTicks;
        else if(phase_<=NativeLaserPhase)status.protectionTicks=PhaseFreezeProtectionTicks;
        else status.protectionTicks=std::max(0,PhaseFreezeProtectionTicks-(phase_-NativeLaserPhase));
    }
    status.lastDenied=lastDenied_;
    if(activation_){
        status.frozen=activation_->bullets.frozen;status.cleared=activation_->bullets.cleared;
        status.immune=activation_->bullets.immune;status.lasers=activation_->lasers;status.effect=activation_->effect;
    }
    return status;
}

const std::optional<Activation>& Host::activation() const{return activation_;}

std::vector<EffectSnapshot> Host::effectSnapshots() const{
    std::vector<EffectSnapshot> result;
    const auto bank=scene_.registry.bank(NativeEffectBank);
    for(const auto& [id,node]:scene_.nodes()){
        if(!node||!node->alive)continue;
        const auto& state=node->vm.state();
        std::optional<anm::SpriteInfo> sprite;
        if(bank&&state.sprite>=0&&std::size_t(state.sprite)<bank->sprites.size())sprite=bank->sprites[std::size_t(state.sprite)];
        float x=state.x+state.engineX+state.offsetX,y=state.y+state.engineY+state.offsetY,z=state.z+state.engineZ+state.offsetZ;
        if(node->parent){const auto parent=scene_.nodes().find(node->parent);if(parent!=scene_.nodes().end()){
            const auto& owner=parent->second->vm.state();x+=owner.x+owner.engineX+owner.offsetX;y+=owner.y+owner.engineY+owner.offsetY;z+=owner.z+owner.engineZ+owner.offsetZ;
        }}
        result.push_back({id,state.bank,state.script,NativeEffectLayer,state,sprite,x,y,z});
    }
    return result;
}

}
