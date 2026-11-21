#include "FocusIndicator.hpp"
#include <cmath>
#include <limits>
namespace th128::player {
namespace {struct Guard {bool& state;explicit Guard(bool& value):state(value){state=true;}~Guard(){state=false;}};}
bool FocusIndicator::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
bool FocusIndicator::configure(const anm::Registry& registry,std::string& error){
    error.clear();const auto bank=registry.bank(0);
    if(configured_||executing_||!bank||bank->scripts.size()<=80||bank->sprites.size()<=444){error="focus indicator requires fresh original bullet80/79 resource";return false;}
    scene_.registry=registry;scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){if(failure_.empty())failure_="focus script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;};
    configured_=true;return true;
}
bool FocusIndicator::checked(std::string& error){
    if(!failure_.empty())return fail(failure_,error);const auto bank=scene_.registry.bank(0);
    for(const auto& [identifier,node]:scene_.nodes()){
        const auto& state=node->vm.state();
        if(node->vm.faulted()||node->customGeometry||node->detached||node->beforeTick||node->onInterrupt)return fail("unsupported original focus Scene callback",error);
        if(state.sprite>=0&&(state.spriteBank!=0||std::size_t(state.sprite)>=bank->sprites.size()))return fail("unavailable original focus sprite",error);
        if(state.ended||state.deleteRequested)node->alive=false;
    }
    scene_.collect();return true;
}
bool FocusIndicator::update(bool focused,float horizontal,float vertical,float depth,std::string& error){
    error.clear();if(!configured_||executing_||!failure_.empty()||!std::isfinite(horizontal)||!std::isfinite(vertical)||!std::isfinite(depth)
        ||std::abs(horizontal)>65536||std::abs(vertical)>65536||std::abs(depth)>65536)return fail(failure_.empty()?"invalid focus indicator context":failure_,error);
    Guard guard(executing_);
    if(focused){
        if(!active_){
            if(!next_||scene_.nodes().size()>64)return fail("focus indicator animation budget",error);active_=next_++;
            if(!scene_.bind(active_,active_,0,80,anm::Membership::Primary,0,0,{},false,nullptr,0,0,12))return fail("original focus80 bind failed",error);
        }
        auto* node=scene_.find(active_);if(!node)return fail("live focus marker disappeared",error);
        auto& state=node->vm.control();state.engineX=320+horizontal;state.engineY=16+vertical;state.engineZ=depth;
    }else if(active_){scene_.interrupt(active_,1);active_=0;}
    return checked(error);
}
bool FocusIndicator::tick(std::string& error){
    error.clear();if(!configured_||executing_||!failure_.empty())return fail(failure_.empty()?"invalid focus indicator tick":failure_,error);
    Guard guard(executing_);scene_.tickPrimary();scene_.tickSecondary();return checked(error);
}
bool FocusIndicator::pose(const anm::Node& node,FocusDraw& draw,std::size_t depth) const {
    if(depth>=64)return false;const auto& state=node.vm.state();draw.state=state;
    draw.x=state.x+state.engineX+state.offsetX;draw.y=state.y+state.engineY+state.offsetY;draw.z=state.z+state.engineZ+state.offsetZ;
    if(node.parent){const auto parent=scene_.nodes().find(node.parent);FocusDraw inherited;
        if(parent==scene_.nodes().end()||!pose(*parent->second,inherited,depth+1))return false;
        draw.x+=inherited.x;draw.y+=inherited.y;draw.z+=inherited.z;draw.state.sx*=inherited.state.sx;draw.state.sy*=inherited.state.sy;
        if(state.mode==1)draw.state.rotation+=inherited.state.rotation;
    }
    return std::isfinite(draw.x)&&std::isfinite(draw.y)&&std::isfinite(draw.z);
}
std::vector<FocusDraw> FocusIndicator::draws() const {
    std::vector<FocusDraw> result;const auto bank=scene_.registry.bank(0);if(!bank)return result;
    for(const auto* node:scene_.chain(anm::Membership::Primary)){
        const auto& state=node->vm.state();if(!node->alive||state.sprite<0||std::size_t(state.sprite)>=bank->sprites.size())continue;
        FocusDraw draw;draw.id=node->id;draw.parent=node->parent;draw.sprite=bank->sprites[std::size_t(state.sprite)];
        if(pose(*node,draw))result.push_back(draw);
    }
    return result;
}
}
