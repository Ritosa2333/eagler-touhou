#include "BossHud.hpp"
#include <limits>

namespace th128::hud {
namespace {
struct Guard {bool& active;explicit Guard(bool& value):active(value){active=true;}~Guard(){active=false;}};
std::uint64_t key(std::size_t slot,int index){return 10+slot*7+std::size_t(index);}
}
TimerNotice bossTimerNotice(int seconds,int previous,bool present,bool message,bool hidden) noexcept {
    if(seconds<0||!present||message||hidden||seconds==previous)return {};
    if(seconds>previous)return {7,-1};
    if(seconds<5)return {9,12};
    if(seconds<10)return {8,11};
    return {};
}
std::optional<std::array<float,2>> bossFill(int life,int maximum,float previous) noexcept {
    if(maximum<=0||!std::isfinite(previous))return std::nullopt;
    const float target=float(double(life)/maximum);float fill=previous;
    if(fill<target)fill=float(double(fill)+.02500000037252903);
    if(fill>target)fill=target;
    return std::array{fill,target};
}
std::array<float,3> bossMarkerPosition(float angle,float x,float y,float z) noexcept {
    const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));
    const float offsetX=float(-double(sine)*56),offsetY=float(double(cosine)*56);
    return {float(double(x)+offsetX),float(double(y)+offsetY),z};
}
bool BossHud::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
bool BossHud::checked(std::string& error){
    if(!failure_.empty())return fail(failure_,error);
    const auto bank=scene_.registry.bank(5);
    for(const auto& [identifier,node]:scene_.nodes()){
        if(node->vm.faulted()||node->detached||node->customGeometry)return fail("unsupported Boss HUD animation ownership",error);
        const auto& state=node->vm.state();
        if(state.sprite>=0&&(!bank||state.spriteBank!=5||std::size_t(state.sprite)>=bank->sprites.size()))return fail("unavailable original Boss HUD sprite",error);
        if(state.ended||state.deleteRequested)node->alive=false;
    }
    scene_.collect();return true;
}
bool BossHud::load(std::span<const std::uint8_t> bytes,std::string& error){
    error.clear();if(booted_||executing_){error="Boss HUD replacement requires a fresh host";return false;}
    anm::Registry candidate;if(!candidate.loadBank(5,bytes.data(),bytes.size(),error))return false;
    const auto bank=candidate.bank(5);if(!bank||bank->scripts.size()<=165){error="original front81 and162..165 are required";return false;}
    scene_.registry=std::move(candidate);scene_.runtime={};scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){if(failure_.empty())failure_="Boss HUD script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;};
    return true;
}
bool BossHud::boot(std::string& error){
    error.clear();if(booted_||executing_||!scene_.registry.bank(5))return fail("invalid Boss HUD startup",error);
    Guard guard(executing_);
    if(!scene_.bind(1,1,5,81,anm::Membership::Embedded,0,0,{},false,nullptr,0,0,23)||!checked(error))return fail(error.empty()?"front81 binding failed":error,error);
    booted_=true;return true;
}
bool BossHud::tick(const BossInput& input,std::string& error){
    error.clear();if(!booted_||executing_||!failure_.empty())return fail(failure_.empty()?"unavailable Boss HUD tick":failure_,error);
    if(!std::isfinite(input.playerX)||!std::isfinite(input.playerY))return fail("invalid Boss HUD player pose",error);
    Guard guard(executing_);scene_.tickEmbedded(1);if(!checked(error))return false;
    state_.seconds=input.countdown[0];state_.hundredths=input.countdown[1];state_.sound=-1;
    const bool present=input.slots[0].owner||input.slots[1].owner;
    state_.timerVisible=present&&!input.message&&!input.hidden&&state_.seconds>=0;
    if(state_.timerVisible){
        const auto notice=bossTimerNotice(state_.seconds,state_.previousSeconds,present,input.message,input.hidden);
        if(notice.interrupt)scene_.interrupt(1,notice.interrupt);
        state_.sound=notice.sound;state_.previousSeconds=state_.seconds;
    }
    for(std::size_t slot=0;slot<input.slots.size();++slot){
        const auto& enemy=input.slots[slot];
        if(!std::isfinite(enemy.x)||!std::isfinite(enemy.y)||!std::isfinite(enemy.z))return fail("nonfinite Boss ring pose",error);
        state_.visible[slot]=enemy.owner&&enemy.life<100000&&!(enemy.flags&0x11u)&&enemy.protection<=0&&!input.message&&!input.hidden;
        if(!state_.visible[slot]){
            for(int index=0;index<7;++index)scene_.remove(key(slot,index));
            if(!enemy.owner)state_.fills[slot]=state_.targets[slot]=0;
            continue;
        }
        const auto fill=bossFill(enemy.life,enemy.maximum,state_.fills[slot]);if(!fill)return fail("original Boss HP denominator unavailable",error);
        state_.fills[slot]=(*fill)[0];state_.targets[slot]=(*fill)[1];
        if(!scene_.find(key(slot,0))){
            for(int index=0;index<7;++index)if(!scene_.bind(key(slot,index),unsigned(slot+2),5,index<3?162+index:165,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23))return fail("original ring162..165 binding failed",error);
        }
        const float x=float(double(enemy.x)+320),y=float(double(enemy.y)+16);
        for(int index=0;index<7;++index){
            auto* node=scene_.find(key(slot,index));if(!node)return fail("original seven Boss ring owners are incomplete",error);
            auto& state=node->vm.control();state.engineX=x;state.engineY=y;state.engineZ=enemy.z;
            if(index==0){state.rx=float(-double(state_.fills[slot])*anm::Tau);state.flags|=4u;}
            if(index>=3){
                const auto marker=input.markers[slot][std::size_t(index-3)].ratio;
                if(!std::isfinite(marker))return fail("nonfinite original phase marker",error);
                if(marker>0&&marker<state_.fills[slot]){
                    state.flags|=6u;state.rotation=anm::normalized(float(-double(anm::Pi)-double(marker)*anm::Tau));
                    const auto position=bossMarkerPosition(state.rotation,x,y,enemy.z);
                    state.engineX=position[0];state.engineY=position[1];
                }else state.flags&=~2u;
            }
        }
        const float distance=float((double(enemy.x)-input.playerX)*(double(enemy.x)-input.playerX)+(double(enemy.y)-input.playerY)*(double(enemy.y)-input.playerY));
        const int interrupt=dimmed_[slot]?(distance>=9216?2:0):(distance<=6400?3:0);
        if(interrupt){for(int index=0;index<7;++index)scene_.interrupt(key(slot,index),interrupt);dimmed_[slot]=interrupt==3;}
    }
    scene_.tickPrimary();if(!checked(error))return false;
    const auto* timer=scene_.find(1);if(!timer)return fail("original countdown81 owner is missing",error);
    state_.timerColor=timer->vm.state().primary;state_.timerScale=timer->vm.state().sx;return true;
}
const BossHudState& BossHud::state() const{return state_;}
std::vector<BossHudSnapshot> BossHud::snapshots() const{
    std::vector<BossHudSnapshot> result;const auto bank=scene_.registry.bank(5);if(!bank)return result;
    for(const auto* node:scene_.chain(anm::Membership::Primary)){
        const auto& state=node->vm.state();if(state.sprite<0||std::size_t(state.sprite)>=bank->sprites.size())continue;
        result.push_back({node->id,state,bank->sprites[std::size_t(state.sprite)],float(double(state.engineX)+state.x+state.offsetX),float(double(state.engineY)+state.y+state.offsetY),float(double(state.engineZ)+state.z+state.offsetZ)});
    }
    return result;
}
std::vector<TimerGlyph> BossHud::glyphs() const{
    if(!state_.timerVisible||state_.seconds>99||state_.hundredths<0||state_.hundredths>99)return {};
    std::vector<TimerGlyph> result;const float scale=state_.timerScale;
    float x=float(344.-48.*scale);
    if(state_.seconds>=10)result.push_back({291+state_.seconds/10,x,16,scale,state_.timerColor});
    x=float(double(x)+20.*scale);result.push_back({291+state_.seconds%10,x,16,scale,state_.timerColor});
    x=float(double(x)+20.*scale);result.push_back({301,x,16,scale,state_.timerColor});
    result.push_back({291+state_.hundredths/10,344,26,.5f,state_.timerColor});result.push_back({291+state_.hundredths%10,354,26,.5f,state_.timerColor});return result;
}
}
