#include "PlayerHitHost.hpp"
#include "PlayerRespawnRules.hpp"
#include "LaserSelector3Rules.hpp"
#include <utility>
#include <cmath>
#include <limits>

namespace th128::player {
namespace {
struct ExecutionGuard {
    bool& executing;
    explicit ExecutionGuard(bool& state):executing(state){executing=true;}
    ~ExecutionGuard(){executing=false;}
};
}
bool PlayerHitHost::fail(std::string message,std::string& output){
    if(error_.empty())error_=std::move(message);
    output=error_;return false;
}
anm::Runtime PlayerHitHost::runtime(){
    anm::Runtime context;
    context.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    context.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    context.unsupported=[this](int,int,int opcode,const char* reason){
        if(error_.empty())error_="player ANM opcode="+std::to_string(opcode)+": "+reason;
    };
    return context;
}
bool PlayerHitHost::configure(const void* animation,std::size_t size,HitHostSnapshot snapshot,
                              EffectHost effects,SoundHost sound,std::string& error){
    error.clear();
    if(configured_||executing_||!error_.empty()||!effects||!sound){error="player hit host requires fresh explicit configuration";return false;}
    if(snapshot.gate.counter438dc!=snapshot.protection.current||!hit::timerResetPrefix(snapshot.stateTimer,snapshot.soundControl)
        ||!hit::counterResetPrefix(snapshot.protection)||!hit::windowPrefix(4,snapshot.stateTimer,snapshot.heldTimer,{false,0,0,0})
        ||!hit::tailPrefix(snapshot.tail,snapshot.tailControl)){
        error="inconsistent or unsupported explicit hit-context snapshot";return false;
    }
    if(!registry_.loadBank(7,static_cast<const std::uint8_t*>(animation),size,error))return false;
    auto context=runtime();
    try{if(!animation_.bind(registry_,7,0,context)||animation_.faulted()||!error_.empty())return fail("player idle ANM is unsupported",error);}
    catch(...){return fail("player ANM callback threw",error);}
    state_=snapshot;effects_=std::move(effects);sound_=std::move(sound);configured_=true;return true;
}
std::optional<int> PlayerHitHost::confirmedOverlap(hit::Position player,hit::HitControl control,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()){
        fail(error_.empty()?"player hit host is unconfigured or reentered":error_,error);return std::nullopt;
    }
    const auto decision=hit::gateDecision(state_.gate,control);
    if(decision.helperReturn)return *decision.helperReturn;
    const auto entry=hit::entryPrefix(state_.gate.stateF78,player);
    if(!entry){fail("unsupported player hit position domain",error);return std::nullopt;}
    ExecutionGuard guard(executing_);
    state_.gate.stateF78=entry->stateF78;
    try {
        if(!effects_(83,entry->deferredEffectPosition,0,error)||!error_.empty()){
            fail(error.empty()?"required player hit effect rejected":error,error);return std::nullopt;
        }
        for(unsigned particle=0;particle<32;++particle){
            if(!effects_(84,entry->deferredEffectPosition,0,error)||!error_.empty()){
                fail(error.empty()?"required player hit particle rejected":error,error);return std::nullopt;
            }
        }
        const auto timer=hit::timerResetPrefix(state_.stateTimer,state_.soundControl);
        if(!timer){fail("unsupported player hit state timer",error);return std::nullopt;}
        state_.stateTimer=timer->updated;
        if(timer->requiredCall&&(!sound_(timer->requiredCall->argument,error)||!error_.empty())){
            fail(error.empty()?"required hit sound rejected":error,error);return std::nullopt;
        }
        const auto counter=hit::counterResetPrefix(state_.protection);
        if(!counter){fail("unsupported player hit protection timer",error);return std::nullopt;}
        state_.protection=counter->updated;state_.gate.counter438dc=state_.protection.current;
        auto context=runtime();
        if(!animation_.bind(registry_,7,0,context)||animation_.faulted()||!error_.empty()){
            fail("required player hit ANM reset failed",error);return std::nullopt;
        }
        const auto tail=hit::tailPrefix(state_.tail,state_.tailControl);
        if(!tail){fail("unsupported player hit tail controller",error);return std::nullopt;}
        state_.tail=*tail;
    } catch(...){fail("player hit host callback threw",error);return std::nullopt;}
    return 1;
}
bool PlayerHitHost::assignPerfectProtection200(std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=1)
        return fail(error_.empty()?"perfect protection requires configured normal live player":error_,error);
    auto timer=state_.protection;
    timer.previous=199;timer.current=200;timer.fractional=200.f;
    timer.clockSource=0x4b6f58;timer.flags|=1U;
    state_.protection=timer;state_.gate.counter438dc=timer.current;return true;
}
bool PlayerHitHost::assignPerfectProtection10(std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=1)
        return fail(error_.empty()?"perfect protection requires configured normal live player":error_,error);
    auto timer=state_.protection;
    timer.previous=9;timer.current=10;timer.fractional=10.f;
    timer.clockSource=0x4b6f58;timer.flags|=1U;
    state_.protection=timer;state_.gate.counter438dc=timer.current;return true;
}
bool PlayerHitHost::stepProtection(float clock,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty())return fail(error_.empty()?"invalid protection timer phase":error_,error);
    const auto next=hit::counterStep(state_.protection,clock);
    if(!next)return fail("unsupported player protection timer",error);
    state_.protection=*next;state_.gate.counter438dc=next->current;return true;
}
bool PlayerHitHost::tickAnimation(std::string& error){
    auto context=runtime();
    try{animation_.tick(context);}catch(...){return fail("player ANM callback threw",error);}
    if(animation_.faulted()||!error_.empty())return fail("player animation phase is unsupported",error);
    return true;
}
bool PlayerHitHost::advanceAnimationUnit(std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty())return fail(error_.empty()?"invalid player animation phase":error_,error);
    ExecutionGuard guard(executing_);return tickAnimation(error);
}
bool PlayerHitHost::advanceNormalTimersUnit(std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=1)
        return fail(error_.empty()?"normal timers require configured state1":error_,error);
    for(auto* timer:{&state_.stateTimer}){
        if(timer->clockSource!=0x4b6f58||!(timer->flags&1u)||timer->current<0||timer->current>=1000000
            ||timer->fractional!=float(timer->current))return fail("unsupported normal-state timer",error);
    }
    if(focusCounter_>=1000000)return fail("normal focus timer budget",error);
    for(auto* timer:{&state_.stateTimer}){
        timer->previous=timer->current;++timer->current;timer->fractional+=1;
    }
    ++focusCounter_;
    return true;
}
bool PlayerHitHost::setHeldTimer(hit::Timer timer,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||timer.clockSource!=0x4b6f58||!(timer.flags&1u)
        ||timer.current< -1000000||timer.current>=1000000||timer.fractional!=float(timer.current))
        return fail(error_.empty()?"unsupported owning player held timer":error_,error);
    state_.heldTimer=timer;return true;
}
bool PlayerHitHost::setSoundOperationHost(SoundOperationHost host,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||soundOperation_||!host){error="hit sound operation requires fresh configured player host";return false;}
    soundOperation_=std::move(host);return true;
}
bool PlayerHitHost::syncPlayerFlags(std::uint32_t flags,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty())return fail(error_.empty()?"configured player flags are unavailable":error_,error);
    state_.gate.flags438ec=flags;return true;
}
bool PlayerHitHost::applyStageClearStart(std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty())return fail(error_.empty()?"stage-clear player start requires a configured host":error_,error);
    state_.gate.flags438ec|=kPlayerStageClearFlag;
    state_.stageClearCounter438f0=0;return true;
}
bool PlayerHitHost::advanceStageClearUnit(std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty())return fail(error_.empty()?"stage-clear player counter requires a configured host":error_,error);
    if(state_.gate.flags438ec&kPlayerStageClearFlag)++state_.stageClearCounter438f0;return true;
}
int PlayerHitHost::focusCounter() const{return focusCounter_;}
bool PlayerHitHost::advanceEntryTimersUnit(std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=0||state_.stateTimer.current<0||state_.stateTimer.current>=60
       ||state_.stateTimer.fractional!=float(state_.stateTimer.current)||state_.stateTimer.clockSource!=0x4b6f58||!(state_.stateTimer.flags&1u))
        return fail(error_.empty()?"unsupported respawn entry timer phase":error_,error);
    state_.stateTimer.previous=state_.stateTimer.current;++state_.stateTimer.current;state_.stateTimer.fractional+=1;
    ++focusCounter_;return true;
}
std::optional<death::RespawnPositionPrefix> PlayerHitHost::beginRespawn(death::Position position,int motivation,int mode04,RespawnCallHost calls,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=2||state_.stateTimer.current<30||!calls){
        fail(error_.empty()?"unsupported respawn gate context":error_,error);return std::nullopt;
    }
    const auto gate=death::respawnGatePrefix(2,state_.stateTimer.current,motivation,{false,0},position);
    if(!gate||gate->outcome!=death::RespawnOutcome::RespawnFieldRequired||!gate->requiredCall){
        fail("unimplemented game-over scene continuation at 0x438030",error);return std::nullopt;
    }
    ExecutionGuard guard(executing_);state_.gate.stateF78=gate->stateF78;
    try{
        if(!calls(*gate->requiredCall,error)||!error.empty()||!error_.empty()){
            fail(error.empty()?"required respawn field rejected":error,error);return std::nullopt;
        }
        const auto afterField=death::respawnAfterFieldPrefix(mode04);
        if(afterField.requiredCall&&(!calls(*afterField.requiredCall,error)||!error.empty()||!error_.empty())){
            fail(error.empty()?"required post-respawn control refresh rejected":error,error);return std::nullopt;
        }
    }catch(...){fail("respawn call host threw",error);return std::nullopt;}
    const auto convert=[](hit::Timer timer){return death::Timer{timer.previous,timer.current,timer.fractional,timer.clockSource,timer.flags};};
    const auto prefix=death::respawnPositionPrefix(0,position,convert(state_.protection),convert(state_.stateTimer));
    if(!prefix){fail("unsupported respawn position continuation",error);return std::nullopt;}
    const auto assign=[](death::Timer timer){return hit::Timer{timer.previous,timer.current,timer.fractional,timer.clockSource,timer.flags};};
    state_.protection=assign(prefix->protectionTimer);state_.gate.counter438dc=state_.protection.current;
    state_.stateTimer=assign(prefix->stateTimer);previousHorizontal_=0;return prefix;
}
std::optional<RawPosition> PlayerHitHost::entryUnit(EntryHost entry,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=0||state_.stateTimer.current<0||state_.stateTimer.current>60||!entry){
        fail(error_.empty()?"unsupported respawn entry context":error_,error);return std::nullopt;
    }
    ExecutionGuard guard(executing_);const auto elapsed=state_.stateTimer.current;
    const auto plan=respawn::entryPlan(elapsed);
    if(!plan){fail("unsupported respawn entry elapsed timer",error);return std::nullopt;}
    const RawPosition position{0,plan->rawVertical};entryDirty_.fill(1);
    const float radius=plan->laserRadius;
    try{if(!entry(elapsed,radius,error)||!error.empty()||!error_.empty()){
        fail(error.empty()?"required respawn entry cancellation rejected":error,error);return std::nullopt;
    }}catch(...){fail("respawn entry callback threw",error);return std::nullopt;}
    if(elapsed>=60){state_.gate.stateF78=1;state_.stateTimer={-1,0,0,0x4b6f58,1};}
    return position;
}
bool PlayerHitHost::advanceWindowUnit(hit::WindowControl control,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=4)
        return fail(error_.empty()?"only configured state4 unit-window steps are supported":error_,error);
    const auto window=hit::windowPrefix(4,state_.stateTimer,state_.heldTimer,control);
    if(!window)return fail("unsupported player state4 window snapshot",error);
    state_.stateTimer=window->stateTimer;state_.heldTimer=window->heldTimer;
    ExecutionGuard guard(executing_);
    if(window->requiredCall){
        if(window->outcome==hit::WindowOutcome::DeathCommitRequired)return fail("unimplemented player death commit lifecycle at 0x43cdd0",error);
        if(window->outcome==hit::WindowOutcome::DeathbombSystemRequired)return fail("unimplemented deathbomb system lifecycle at 0x407810",error);
        if(!soundOperation_)return fail("unimplemented held-freeze window sound operation at 0x458590",error);
        try{
            if(!soundOperation_(1,31,error)||!error.empty()||!error_.empty())return fail(error.empty()?"required hit charge sound stop rejected":error,error);
        }catch(...){return fail("required hit charge sound stop threw",error);}
    }
    auto& timer=state_.stateTimer;
    if(timer.clockSource!=0x4b6f58||!(timer.flags&1U)||timer.current<0||timer.current>65535
        ||!std::isfinite(timer.fractional)||timer.fractional!=float(timer.current))
        return fail("state4 increment requires a known initialized integral unit-clock timer",error);
    const auto protection=hit::counterStep(state_.protection,1);
    if(!protection)return fail("unsupported state4 protection timer",error);
    state_.protection=*protection;state_.gate.counter438dc=protection->current;
    if(!tickAnimation(error))return false;
    timer.previous=timer.current;++timer.current;timer.fractional=float(double(timer.fractional)+1.0);
    return true;
}
bool PlayerHitHost::commitDeathState(int state,hit::Timer timer,hit::Timer protection,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=4||state_.stateTimer.current<8
        ||state!=2||timer.previous!=-1||timer.current!=0||timer.fractional!=0||timer.clockSource!=0x4b6f58||!(timer.flags&1U)
        ||protection.previous!=179||protection.current!=180||protection.fractional!=180||protection.clockSource!=0x4b6f58||!(protection.flags&1U))
        return fail(error_.empty()?"unsupported committed death-state context":error_,error);
    ExecutionGuard guard(executing_);
    state_.gate.stateF78=2;state_.stateTimer=timer;state_.protection=protection;state_.gate.counter438dc=180;
    auto context=runtime();
    try{if(!animation_.bind(registry_,7,0,context)||animation_.faulted()||!error_.empty())return fail("death player ANM reset failed",error);}
    catch(...){return fail("death player ANM reset threw",error);}
    return true;
}
bool PlayerHitHost::advancePostDeathUnit(std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=2||state_.stateTimer.current<0
        ||state_.stateTimer.current>=30||state_.stateTimer.fractional!=float(state_.stateTimer.current)
        ||state_.stateTimer.clockSource!=0x4b6f58||!(state_.stateTimer.flags&1U))
        return fail(error_.empty()?"unsupported post-death unit timer phase":error_,error);
    ExecutionGuard guard(executing_);
    const auto protection=hit::counterStep(state_.protection,1);
    if(!protection)return fail("unsupported post-death protection timer",error);
    state_.protection=*protection;state_.gate.counter438dc=protection->current;
    if(!tickAnimation(error))return false;
    state_.stateTimer.previous=state_.stateTimer.current;
    ++state_.stateTimer.current;state_.stateTimer.fractional=float(double(state_.stateTimer.fractional)+1.0);
    return true;
}
std::optional<Movement> PlayerHitHost::moveUnit(RawPosition position,Input input,FocusContext focus,std::string& error){
    error.clear();
    if(!configured_||executing_||!error_.empty()||state_.gate.stateF78!=1){
        fail(error_.empty()?"player movement requires explicit normal-state context":error_,error);return std::nullopt;
    }
    if(position.horizontal<MinimumHorizontalRaw||position.horizontal>MaximumHorizontalRaw
        ||position.vertical<MinimumVerticalRaw||position.vertical>MaximumVerticalRaw){
        fail("player movement raw position is outside the normal-state domain",error);return std::nullopt;
    }
    const auto movement=moveOneTick(position,input,focus);
    const auto horizontal=movement.delta.horizontal;
    int script=-1;
    if(horizontal<0&&previousHorizontal_>=0)script=1;
    else if(horizontal>0&&previousHorizontal_<=0)script=3;
    else if(horizontal==0&&previousHorizontal_<0)script=2;
    else if(horizontal==0&&previousHorizontal_>0)script=4;
    ExecutionGuard guard(executing_);
    if(script>=0){
        auto context=runtime();
        try{if(!animation_.bind(registry_,7,script,context)||animation_.faulted()||!error_.empty()){
            fail("player directional animation binding failed",error);return std::nullopt;
        }}catch(...){fail("player directional animation callback threw",error);return std::nullopt;}
    }
    previousHorizontal_=horizontal;
    if(!tickAnimation(error))return std::nullopt;
    return movement;
}
HitHostSnapshot PlayerHitHost::snapshot() const{return state_;}
const anm::State* PlayerHitHost::animationState() const{return configured_&&!animation_.faulted()?&animation_.state():nullptr;}
std::optional<PowerOutline> PlayerHitHost::powerOutline(int power) const{
    const auto* state=animationState();const auto bank=registry_.bank(7);
    if(!state||!bank||power<0||power>10000||state->sprite<0||state->sprite>27||std::size_t(state->sprite+79)>=bank->sprites.size())return std::nullopt;
    PowerOutline result{*state,bank->sprites[std::size_t(state->sprite+79)]};
    result.state.sprite+=79;result.state.sx=result.state.sy=power<3000?1.1f:1.3f;
    result.state.primary=power<3000?0x00ffffffu:power<10000?0x50ffff80u:0xc0ffff00u;
    result.state.blend=1;result.state.flags=(result.state.flags&~0xe0u)|0x28u;result.state.flags2&=~4u;
    return result;
}
const anm::SpriteInfo* PlayerHitHost::spriteInfo() const{
    const auto* state=animationState();const auto bank=registry_.bank(7);
    return state&&bank&&state->sprite>=0&&std::size_t(state->sprite)<bank->sprites.size()?&bank->sprites[std::size_t(state->sprite)]:nullptr;
}

bool PlayerHitHost::checkLaserEffects(std::string& error){
    if(!error_.empty())return fail(error_,error);
    for(const auto& [identifier,node]:laserEffects_.nodes()){
        if(node->vm.faulted()||node->customGeometry||node->beforeTick||node->onInterrupt)
            return fail("player17 effect requested unsupported native callback or geometry",error);
        const auto& state=node->vm.state();const auto bank=laserEffects_.registry.bank(state.spriteBank);
        if(state.sprite>=0&&(!bank||std::size_t(state.sprite)>=bank->sprites.size()))return fail("player17 sprite resource is unavailable",error);
    }
    laserEffects_.collect();return true;
}
bool PlayerHitHost::configureLaserEffects(std::function<std::uint32_t()> random,std::string& error){
    error.clear();if(!configured_||executing_||!error_.empty()||laserEffectsConfigured_||!random){error="player17 requires fresh configured actual player resource/random owner";return false;}
    laserEffects_.registry=registry_;laserRandom_=std::move(random);
    laserEffects_.runtime.gameRandom32=[this]{return laserRandom_();};
    laserEffects_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){if(error_.empty())error_="player17 script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;};
    laserEffects_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    laserEffects_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    laserEffectsConfigured_=true;return true;
}
bool PlayerHitHost::beginLaserContactPhase(std::string& error){
    error.clear();if(!configured_||executing_||!error_.empty()||!laserEffectsConfigured_||laserPhaseOpen_)return fail("player laser prefix requires actual idle native fields",error);
    laserPhasePrevious_=laser::selector3::beginContactPhase(state_.gate,state_.laserContactCounter43a90);
    laserPhaseOpen_=true;return true;
}
bool PlayerHitHost::laserColumnContactRequired(unsigned columns) const{
    return configured_&&laserPhaseOpen_&&error_.empty()&&laser::selector3::contactSoundRequired(columns,state_.gate);
}
bool PlayerHitHost::applyLaserColumnContact(unsigned columns,std::int32_t& motivation,std::int32_t& flag,std::string& error){
    error.clear();if(!laserEffectsConfigured_||!laserPhaseOpen_||executing_||!error_.empty()||columns==0||columns>512)return fail("selector3 player tail requires current actual contact phase",error);
    if(!laserColumnContactRequired(columns))return true;
    const auto drain=std::int32_t(columns*5u);
    if(!flag&&motivation<std::numeric_limits<std::int32_t>::min()+drain)return fail("selector3 motivation integer underflow",error);
    if(!flag){motivation-=drain;if(motivation>100000)motivation=100000;else if(motivation<0)flag=1;}
    if(motivation<0){motivation=0;flag=0;}
    state_.gate.flags438ec|=8u;
    state_.laserContactCounter43a90=std::bit_cast<std::int32_t>(std::uint32_t(state_.laserContactCounter43a90)+1u);return true;
}
bool PlayerHitHost::endLaserContactPhase(hit::Position position,std::string& error){
    error.clear();if(!laserEffectsConfigured_||!laserPhaseOpen_||executing_||!error_.empty())return fail("selector3 player manager tail has no current phase",error);
    laserPhaseOpen_=false;
    if(laser::selector3::contactEntryEffectRequired(laserPhasePrevious_,state_.laserContactCounter43a90)){
        if(!std::isfinite(position.horizontal)||!std::isfinite(position.vertical)||!std::isfinite(position.depth)||!laserEffectSerial_)return fail("player17 effect has invalid actual position/identity",error);
        const auto key=std::uint64_t(laserEffectSerial_++);
        const auto* node=laserEffects_.bind(key,std::uint32_t(key),7,17,anm::Membership::Primary,
            320+position.horizontal,16+position.vertical,{},false,nullptr,0,0,23,false,position.depth);
        if(!node)return fail(error_.empty()?"player17 real ANM constructor failed":error_,error);
    }
    return checkLaserEffects(error);
}
bool PlayerHitHost::tickLaserEffects(std::string& error){
    error.clear();if(!laserEffectsConfigured_||executing_||!error_.empty())return fail("player17 actual primary phase is unavailable",error);
    laserEffects_.tickPrimary();laserEffects_.tickSecondary();return checkLaserEffects(error);
}
std::vector<LaserContactEffect> PlayerHitHost::laserEffectSnapshots(std::string& error) const{
    error.clear();std::vector<LaserContactEffect> output;
    if(!laserEffectsConfigured_||!error_.empty()){error=error_.empty()?"player17 real resource owner is unavailable":error_;return output;}
    for(const auto membership:{anm::Membership::Primary,anm::Membership::Secondary})for(const auto* node:laserEffects_.chain(membership)){
        const auto& state=node->vm.state();if(!node->alive||state.sprite<0)continue;
        const auto bank=laserEffects_.registry.bank(state.spriteBank);
        if(node->parent||!bank||std::size_t(state.sprite)>=bank->sprites.size()){error="player17 actual draw parent/resource owner is unsupported";return {};}
        output.push_back({node->id,state,bank->sprites[std::size_t(state.sprite)],state.x+state.engineX+state.offsetX,state.y+state.engineY+state.offsetY,state.z+state.engineZ+state.offsetZ});
    }
    return output;
}
}
