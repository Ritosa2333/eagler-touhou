#include "PerfectHud.hpp"
#include <limits>
#include <utility>

namespace th128::hud {
namespace {
struct Guard {bool& state;explicit Guard(bool& value):state(value){state=true;}~Guard(){state=false;}};
}
bool PerfectHud::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
void PerfectHud::runtime(){
    scene_.runtime={};scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.gameRandom32=[this]{if(failure_.empty())failure_="unproved HUD game RNG operation";return 0u;};
    scene_.runtime.displayRandom32=[this]{if(failure_.empty())failure_="unproved HUD display RNG operation";return 0u;};
    scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){
        if(failure_.empty())failure_="front HUD script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;
    };
}
bool PerfectHud::checked(std::string& error){
    if(!failure_.empty())return fail(failure_,error);
    for(const auto& [id,node]:scene_.nodes()){
        if(node->vm.faulted()||node->detached||node->customGeometry)return fail("unimplemented HUD Scene geometry/callback",error);
        const auto& state=node->vm.state();const auto resource=scene_.registry.bank(5);
        if(state.sprite>=0&&(!resource||std::size_t(state.sprite)>=resource->sprites.size()||state.spriteBank!=5))return fail("HUD sprite binding unavailable",error);
        if(state.ended||state.deleteRequested)node->alive=false;
    }
    scene_.collect();return true;
}
bool PerfectHud::load(std::span<const std::uint8_t> bytes,std::string& error){
    error.clear();if(executing_||booted_){error="front HUD resource replacement requires a fresh host";return false;}
    anm::Registry candidate;
    if(!candidate.loadBank(5,bytes.data(),bytes.size(),error))return false;
    const auto bank=candidate.bank(5);
    if(!bank||bank->scripts.size()<=47){error="actual front0/47 scripts are required";return false;}
    scene_.registry=std::move(candidate);loaded_=true;runtime();return true;
}
bool PerfectHud::boot(int value,std::string& error){
    error.clear();if(!loaded_||booted_||executing_||value<0||value>30000||!failure_.empty())return fail("invalid fresh front HUD startup",error);
    {Guard guard(executing_);
    if(!scene_.bind(1,1,5,0,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23)||!checked(error))return fail(error.empty()?"front root0 binding failed":error,error);}
    booted_=true;return charge(value,error);
}
bool PerfectHud::rank(int value,std::string& error){
    error.clear();if(!booted_||executing_||rank_!=-1||value<0||value>4){error="invalid fresh original rank HUD binding";return false;}
    Guard guard(executing_);
    if(!scene_.bind(3,3,5,66+value,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23))return fail("original rank66..70 binding failed",error);
    scene_.interrupt(3,3,true);
    if(!checked(error))return false;
    rank_=value;return true;
}
bool PerfectHud::charge(int value,std::string& error){
    error.clear();if(value<0||value>30000){error="invalid bounded HUD charge";return false;}
    if(!booted_||executing_||!failure_.empty())return fail(failure_.empty()?"unavailable HUD charge operation":failure_,error);
    const auto* root=scene_.find(1);if(!root)return fail("required front root is unavailable",error);
    std::array<unsigned,3> found{};
    for(const auto* child:scene_.children(root->id)){
        const int script=child->vm.state().script;
        if(script<12||script>14)continue;
        ++found[std::size_t(script-12)];
        auto& state=scene_.nodes().at(child->id)->vm.control();
        if(value>=(script-11)*10000)state.flags|=2u;else state.flags&=~2u;
    }
    if(found!=std::array<unsigned,3>{1,1,1})return fail("required native charge child12/13/14 is missing or ambiguous",error);
    return true;
}
bool PerfectHud::notice(std::string& error){return replaceNotice(47,error);}
bool PerfectHud::motivationNotice(std::string& error){return replaceNotice(49,error);}
bool PerfectHud::replaceNotice(int script,std::string& error){
    error.clear();if(!booted_||executing_||!failure_.empty())return fail(failure_.empty()?"unavailable front notification":failure_,error);
    // Original 0x424d6c/type2 and 0x424dca/type4 replace the same field +0x588c.
    Guard guard(executing_);scene_.remove(2);scene_.collect();
    if(!scene_.bind(2,2,5,script,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23))return fail("required front"+std::to_string(script)+" notification bind failed",error);
    return checked(error);
}
bool PerfectHud::tick(std::string& error){
    error.clear();if(!booted_||executing_||!failure_.empty())return fail(failure_.empty()?"unavailable front primary pass":failure_,error);
    Guard guard(executing_);scene_.tickPrimary();return checked(error);
}
std::vector<Snapshot> PerfectHud::snapshots() const {
    std::vector<Snapshot> result;const auto bank=scene_.registry.bank(5);if(!bank)return result;
    for(const auto* node:scene_.chain(anm::Membership::Primary)){
        const auto& state=node->vm.state();std::optional<anm::SpriteInfo> sprite;
        if(state.sprite>=0&&std::size_t(state.sprite)<bank->sprites.size())sprite=bank->sprites[std::size_t(state.sprite)];
        float x=state.x+state.engineX+state.offsetX,y=state.y+state.engineY+state.offsetY,z=state.z+state.engineZ+state.offsetZ;
        if(node->parent){const auto parent=scene_.nodes().find(node->parent);if(parent!=scene_.nodes().end()){
            const auto& owner=parent->second->vm.state();x+=owner.x+owner.engineX+owner.offsetX;y+=owner.y+owner.engineY+owner.offsetY;z+=owner.z+owner.engineZ+owner.offsetZ;
        }}
        result.push_back({node->id,node->parent,state,sprite,x,y,z});
    }
    return result;
}
}
