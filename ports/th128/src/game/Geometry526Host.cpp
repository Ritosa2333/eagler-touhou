#include "Geometry526Host.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace th128::geometry526 {
namespace {
struct Guard {bool& executing;explicit Guard(bool& value):executing(value){executing=true;}~Guard(){executing=false;}};
}
bool Host::fail(std::string reason,std::string& error){
    if(failure_.empty())failure_=std::move(reason);
    error=failure_;return false;
}
bool Host::ready(std::string& error) const {
    error.clear();
    if(!loaded_||executing_||!failure_.empty()){
        error=failure_.empty()?"geometry526 requires a loaded, non-reentrant text host":failure_;return false;
    }
    return true;
}
bool Host::checked(std::string& error){
    if(!failure_.empty()){error=failure_;return false;}
    for(const auto& [identity,node]:scene_.nodes()){
        const auto& state=node->vm.state();
        if(node->vm.faulted()||node->detached||node->parent||state.ended||state.deleteRequested)
            return fail("geometry526 text82 encountered unproved child/deletion state",error);
        if(state.bank!=BankId||state.spriteBank!=BankId||state.sprite!=83||state.script!=82
        ||state.layer!=29||state.blend!=(node->customGeometry?0:3))
            return fail("geometry526 text82 resource/layer/blend changed unexpectedly",error);
        for(const float value:{state.x,state.y,state.z,state.sx,state.sy,state.rotation,state.u,state.v,state.uvScaleX,state.uvScaleY})
            if(!std::isfinite(value))return fail("non-finite geometry526 text82 state",error);
    }
    return true;
}
bool Host::load(std::span<const std::uint8_t> bytes,std::string& error){
    error.clear();
    if(loaded_||executing_||!effects_.empty()){error="geometry526 text replacement requires a fresh host";return false;}
    anm::Registry candidate;
    if(!candidate.loadBank(BankId,bytes.data(),bytes.size(),error))return false;
    const auto bank=candidate.bank(BankId);
    if(!bank||bank->scripts.size()<=82||bank->sprites.size()<=83){error="geometry526 requires actual text82/sprite83";return false;}
    const auto& sprite=bank->sprites[83];
    if(sprite.textureEntry!=2||sprite.width!=384||sprite.height!=448
        ||sprite.u0!=.2f||sprite.v0!=float(16.0/480.0)||sprite.u1!=.8f||sprite.v1!=float(464.0/480.0)){
        error="geometry526 requires actual640x480 text@R entry2 rectangle";return false;
    }
    scene_.registry=std::move(candidate);
    scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.gameRandom32=[this]{if(failure_.empty())failure_="unproved geometry526 game RNG";return 0u;};
    scene_.runtime.displayRandom32=[this]{if(failure_.empty())failure_="unproved geometry526 display RNG";return 0u;};
    scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){
        if(failure_.empty())failure_="geometry526 text script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;
    };
    if(!main_.bind(scene_.registry,BankId,82,scene_.runtime)||main_.faulted()||!failure_.empty())
        return fail(failure_.empty()?"actual Main82 binding failed":failure_,error);
    loaded_=true;return true;
}
bool Host::create(std::uint32_t owner,float radius,std::uint32_t color,std::string& error){
    if(!ready(error))return false;
    if(!owner||!std::isfinite(radius)||std::abs(radius)>4096){error="invalid bounded geometry526 owner/radius";return false;}
    if(radius>0&&!effects_.contains(owner)&&effects_.size()>=64){error="geometry526 owning effect budget exceeded";return false;}
    if(!removeOwner(owner,error))return false;
    if(radius<=0)return true;
    Guard guard(executing_);Effect effect;effect.state={radius,16,color,{0,0}};
    for(unsigned column=0;column<effect.rows.size();++column){
        const auto key=(std::uint64_t(owner)<<8)|(16u+column);effect.rows[column]=key;
        auto* node=scene_.bind(key,owner,BankId,82,anm::Membership::Primary,0,0,{},false,nullptr,0,0,29);
        if(!node||!checked(error))return fail(error.empty()?"geometry526 row binding failed":error,error);
        auto& state=node->vm.control();state.flags=((state.flags&0xecffffffu)|0x0c000000u)&0xffffff1fu;
        state.mode=12;state.blend=0;state.geometryCount=17;node->customGeometry=true;
    }
    effects_.emplace(owner,std::move(effect));return true;
}
bool Host::update(std::uint32_t owner,const std::array<float,3>& position,float delta,std::string& error){
    if(!ready(error))return false;
    const auto found=effects_.find(owner);
    if(found==effects_.end()){error="geometry526 update owner is unavailable";return false;}
    const auto result=stepEffect(found->second.state,position,delta);
    if(!result){error="invalid bounded geometry526 update";return false;}
    if(!checked(error))return false;
    found->second.state=result->next;found->second.grid=result->grid;return true;
}
bool Host::tickPrimary(std::string& error){
    if(!ready(error))return false;
    for(const auto& [owner,effect]:effects_)if(!effect.grid){error="geometry526 primary pass precedes required grid initialization";return false;}
    Guard guard(executing_);scene_.tickPrimary();return checked(error);
}
bool Host::removeOwner(std::uint32_t owner,std::string& error){
    if(!ready(error))return false;
    if(const auto found=effects_.find(owner);found!=effects_.end()){
        for(const auto key:found->second.rows)scene_.remove(key);
        effects_.erase(found);scene_.collect();
    }
    return true;
}
std::optional<EffectState> Host::effectState(std::uint32_t owner) const {
    const auto found=effects_.find(owner);return found==effects_.end()?std::nullopt:std::optional{found->second.state};
}
std::optional<MainSnapshot> Host::mainSnapshot(std::string& error) const {
    if(!ready(error))return std::nullopt;
    const auto bank=scene_.registry.bank(BankId);const auto& state=main_.state();
    if(!bank||main_.faulted()||state.script!=82||state.sprite!=83||state.spriteBank!=BankId
        ||state.mode!=0||state.layer!=29||state.blend!=3||state.ended||state.deleteRequested){
        error="actual held Main82 compositor is unavailable";return std::nullopt;
    }
    return MainSnapshot{state,bank->sprites[83]};
}
std::optional<std::vector<MeshSnapshot>> Host::snapshots(std::string& error) const {
    if(!ready(error))return std::nullopt;
    std::map<std::uint32_t,std::vector<Vertex>> strips;
    for(const auto& [owner,effect]:effects_){
        if(!effect.grid){error="geometry526 draw precedes required grid initialization";return std::nullopt;}
        auto copied=rowStrips(*effect.grid);
        if(!copied){error="geometry526 strip copy bounds invalid";return std::nullopt;}
        strips.emplace(owner,std::move(*copied));
    }
    std::vector<MeshSnapshot> result;result.reserve(effects_.size()*16);
    const auto bank=scene_.registry.bank(BankId);
    if(!bank){error="geometry526 text bank unavailable";return std::nullopt;}
    for(const auto* node:scene_.chain(anm::Membership::Primary)){
        const auto effect=effects_.find(node->owner);
        if(effect==effects_.end()){error="geometry526 row owner unavailable";return std::nullopt;}
        const auto row=std::find(effect->second.rows.begin(),effect->second.rows.end(),node->key);
        if(row==effect->second.rows.end()){error="geometry526 row identity unavailable";return std::nullopt;}
        const auto column=unsigned(std::distance(effect->second.rows.begin(),row));
        const auto& vertices=strips.at(node->owner);
        const auto begin=vertices.begin()+column*34;
        result.push_back({node->id,node->owner,column,node->vm.state(),bank->sprites[83],{begin,begin+34}});
    }
    if(result.size()!=effects_.size()*16){error="geometry526 live row count differs from ownership";return std::nullopt;}
    return result;
}
}
