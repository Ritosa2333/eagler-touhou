#include "MessageVisuals.hpp"
#include <algorithm>
#include <string_view>
#include <cmath>
#include <limits>

namespace th128::message {
namespace {constexpr std::uint64_t AuxiliaryFrontKey=13;
std::uint64_t rightPortraitKey(int family){return family==0?11u:family==1?14u:15u;}
struct Guard {bool& value;explicit Guard(bool& state):value(state){value=true;}~Guard(){value=false;}};}
void Visuals::runtime(){
    scene_.runtime={};scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.unsupported=[this](int bank,int script,int opcode,const char* reason){if(failure_.empty())failure_="MSG ANM bank="+std::to_string(bank)+" script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;};
}
bool Visuals::load(int bank,std::span<const std::uint8_t> bytes,std::string& error){
    error.clear();if(executing_||!scene_.nodes().empty()||(bank!=6&&bank!=7&&bank!=3&&bank!=5)){error="message visual loading requires original text/player/Boss banks before execution";return false;}
    if(bank==5&&bytes.size()>8u*1024u*1024u){error="original auxiliary front bank exceeds its existing capacity";return false;}
    if(!scene_.registry.loadBank(bank,bytes.data(),bytes.size(),error))return false;
    if(bank==3){
        constexpr std::string_view marker="face/face04/face04no.png";
        extraBossPortrait_=std::search(bytes.begin(),bytes.end(),marker.begin(),marker.end())!=bytes.end();
        if(extraBossPortrait_){
            const auto actual=scene_.registry.bank(3);
            if(!actual||actual->scripts.size()!=22||actual->sprites.size()!=46
                ||actual->sprites[18].textureEntry!=2||actual->sprites[26].textureEntry!=10
                ||actual->sprites[42].textureEntry!=19){
                error="original Marisa portrait resource identity rejected";return false;
            }
        }
    }
    return true;
}
bool Visuals::checked(std::string& error){
    if(!failure_.empty()){error=failure_;return false;}
    for(const auto& [identifier,node]:scene_.nodes()){
        const auto& state=node->vm.state();
        if(node->vm.faulted()||node->detached||node->customGeometry||node->beforeTick||node->onInterrupt){error="unsupported actual message ANM ownership or geometry";failure_=error;return false;}
        for(const auto value:{state.x,state.y,state.z,state.sx,state.sy,state.engineX,state.engineY,state.offsetX,state.offsetY})if(!std::isfinite(value)){error="nonfinite original message ANM pose";failure_=error;return false;}
        if(state.ended||state.deleteRequested)node->alive=false;
    }
    scene_.collect();return true;
}
bool Visuals::expression(int side,int value,std::string& error){
    if(value<0||value>8||side==1&&extraBossPortrait_&&value>7){error="unsupported original portrait expression";return false;}
    const auto* root=scene_.find(side==0?10:rightPortraitKey(rightFamily_));if(!root){error="original expression requires its existing portrait owner";return false;}
    const int alternativeRoot=side==1&&root->vm.state().bank==3?root->vm.state().script:-1;
    const bool alternative=alternativeRoot==24||alternativeRoot==28||alternativeRoot==32;
    if(alternative&&value>7){error="unsupported original alternative portrait expression";return false;}
    for(const auto* child:scene_.children(root->id)){
        auto& state=scene_.nodes().at(child->id)->vm.control();int sprite=-1;
        if(side==0&&state.bank==7&&(state.script==18||state.script==19))sprite=(state.script==18?45:54)+value;
        if(side==1&&extraBossPortrait_&&state.bank==3&&state.script>=12&&state.script<=14)sprite=18+(state.script-12)*8+value;
        if(side==1&&!extraBossPortrait_&&state.bank==3&&state.script>=6&&state.script<=8)sprite=11+(state.script-6)*9+value;
        if(alternativeRoot==24&&state.bank==3&&state.script>=21&&state.script<=23)sprite=33+(state.script-21)*8+value;
        if(alternativeRoot==28&&state.bank==3&&state.script>=25&&state.script<=27)sprite=58+(state.script-25)*8+value;
        if(alternativeRoot==32&&state.bank==3&&state.script>=29&&state.script<=31)sprite=83+(state.script-29)*8+value;
        if(sprite<0)continue;const auto bank=scene_.registry.bank(state.bank);
        if(!bank||std::size_t(sprite)>=bank->sprites.size()){error="unavailable original portrait sprite";return false;}
        state.sprite=sprite;state.spriteBank=state.bank;state.spriteWidth=bank->sprites[std::size_t(sprite)].width;state.spriteHeight=bank->sprites[std::size_t(sprite)].height;
    }
    return true;
}
bool Visuals::retireAuxiliary(std::string& error){
    auto* root=scene_.find(AuxiliaryFrontKey);
    if(!auxiliaryFront_){if(root){error="untracked original auxiliary front owner";return false;}return true;}
    if(!root||root->id!=*auxiliaryFront_||root->vm.state().bank!=5||(root->vm.state().script!=156&&root->vm.state().script!=157&&root->vm.state().script!=158&&root->vm.state().script!=159&&root->vm.state().script!=160&&root->vm.state().script!=161)){
        error="original auxiliary front identity changed";return false;
    }
    auto retired=scene_.children(root->id);retired.insert(retired.begin(),root);
    for(const auto* node:retired){auto& state=scene_.nodes().at(node->id)->vm.control();state.flags|=0x20000000u;}
    if(!scene_.releaseBinding(AuxiliaryFrontKey,root->id)){error="original auxiliary handle bridge release failed";return false;}
    auxiliaryFront_.reset();return true;
}
bool Visuals::setAuxiliaryExtent(float extent,std::string& error){
    auto* root=scene_.find(AuxiliaryFrontKey);
    if(!root||!auxiliaryFront_||root->id!=*auxiliaryFront_||!std::isfinite(extent)){error="original auxiliary extent requires its existing owner";return false;}
    const int selector=root->vm.state().script-156;
    if(selector<0||selector>5){error="unproved original auxiliary selector";return false;}
    std::array<std::uint32_t,2> targets{};unsigned count=0;
    for(const auto* child:scene_.children(root->id))if(child->vm.state().script==138+selector||child->vm.state().script==150+selector){
        if(count>=targets.size()||child->vm.state().bank!=5||child->owner!=root->owner){error="original auxiliary child ownership mismatch";return false;}
        targets[count++]=child->id;
    }
    if(count!=targets.size()){error="original auxiliary extent requires two authored children";return false;}
    for(const auto identifier:targets)scene_.nodes().at(identifier)->vm.control().floats[0]=extent;
    auxiliaryExtent_=extent;return true;
}
bool Visuals::constructAuxiliary(std::string& error){
    const int selector=textSpeaker_+2*textStyle_;
    if(selector<0||selector>5){error="unproved original auxiliary constructor selector";return false;}
    if(!scene_.registry.bank(5)){error="original MSG17 auxiliary front resource is not loaded";return false;}
    if(!retireAuxiliary(error))return false;
    auto* root=scene_.bind(AuxiliaryFrontKey,unsigned(AuxiliaryFrontKey),5,156+selector,anm::Membership::Primary,
        textPosition_[0],textPosition_[1],{},false,nullptr,0,0,23);
    if(!root){error="original MSG17 auxiliary front construction failed";return false;}
    auxiliaryFront_=root->id;
    if(!setAuxiliaryExtent(auxiliaryExtent_,error))return false;
    auxiliaryExtent_=0;return true;
}
bool Visuals::event(const Event& event,std::string& error){
    error.clear();if(executing_||!failure_.empty()){if(failure_.empty())failure_="message visuals reentered";error=failure_;return false;}
    Guard guard(executing_);
    if(event.opcode==-1){
        scene_.reset();textLine_=0;auxiliaryFront_.reset();auxiliaryExtent_=0;textSpeaker_=0;textStyle_=0;rightFamily_=0;textPosition_={400,480};runtime();
        for(unsigned index=0;index<4;++index)if(!scene_.bind(1+index,1+index,6,int(index),anm::Membership::Primary,0,0,{},false,nullptr,0,0,23)){error="original message text root binding failed";return false;}
    }else if(event.opcode==1||event.opcode==2){
        const int side=event.opcode==1?0:1;
        const int constructor=event.integers[0];
        if(constructor!=0&&!(side==1&&(constructor==1||constructor==2||constructor==3))){error="message portrait alternative constructor requires original mapping";return false;}
        if(side==1)rightFamily_=constructor>1?constructor-1:0;
        const auto key=std::uint64_t(side==0?10:rightPortraitKey(rightFamily_));
        if(side==1&&extraBossPortrait_&&constructor!=0){error="Marisa portrait requires its original default constructor";return false;}
        const int script=side==0?20:extraBossPortrait_?15:constructor==1?24:constructor==2?28:constructor==3?32:9;
        if(!scene_.bind(key,unsigned(key),side==0?7:3,script,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23)){error="original message portrait binding failed";return false;}
    }else if(event.opcode==13||event.opcode==14){if(!expression(event.opcode==13?0:1,event.integers[0],error))return false;
    }else if(event.opcode==7||event.opcode==8){
        textLine_=0;textSpeaker_=event.opcode==7?0:1;const bool left=event.opcode==7;scene_.interrupt(10,left?2:3,true);for(const auto key:{11u,14u,15u})scene_.interrupt(key,left?3:2,true);
        for(unsigned index=0;index<4;++index)if(auto* root=scene_.find(1+index))root->vm.control().offsetY=0;
    }else if(event.opcode==4){scene_.interrupt(10,1,true);
    }else if(event.opcode==5){for(const auto key:{11u,14u,15u})if(const auto* root=scene_.find(key)){const auto identifier=root->id;scene_.interrupt(key,1,true);if(!scene_.releaseBinding(key,identifier)){error="original right portrait handle release failed";return false;}}scene_.interrupt(12,1,true);
    }else if(event.opcode==6){if(!retireAuxiliary(error))return false;for(unsigned index=0;index<4;++index)scene_.interrupt(1+index,1,true);
    }else if(event.opcode==18){
        if(!retireAuxiliary(error))return false;
        for(unsigned index=0;index<4;++index)scene_.interrupt(1+index,3,false);
    }else if(event.opcode==20){
        scene_.remove(12);scene_.collect();
        if(!scene_.bind(12,12,3,extraBossPortrait_?16:10,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23)){error="original Boss introduction binding failed";return false;}
    }else if(event.opcode==11){
        textLine_=0;
        for(unsigned index=0;index<4;++index)if(auto* root=scene_.find(1+index))root->vm.control().offsetY=0;
    }else if(event.opcode==0){scene_.interrupt(10,1,true);for(const auto key:{11u,14u,15u})scene_.interrupt(key,1,true);for(unsigned index=0;index<4;++index)scene_.interrupt(1+index,1,true);
    }else if(event.opcode==17){
        if(!event.bytes.empty()&&event.bytes[0]=='|'){if(textLine_==0&&!auxiliaryFront_&&!constructAuxiliary(error))return false;scene_.interrupt(3+std::min(textLine_,1u),2,true);return checked(error);}
        if(textLine_>=2){error="original scrolling dialogue text layout is not yet registered";return false;}
        if(textLine_==0&&!constructAuxiliary(error))return false;
        const auto aligned=(event.bytes.size()*10u)&~std::size_t(15);
        const float extent=std::max(auxiliaryExtent_,float(aligned)-16.f);
        if(!setAuxiliaryExtent(extent,error))return false;
        const auto index=textLine_++;scene_.interrupt(1+index,2,true);scene_.interrupt(3+index,2,true);
    }else if(event.opcode==28){
        if(!std::isfinite(event.floats[0])||!std::isfinite(event.floats[1])){error="nonfinite original message text position";return false;}
        textPosition_=event.floats;
    }else if(event.opcode==29){textStyle_=event.integers[0]&3;
    }else if(event.opcode!=10&&event.opcode!=12&&event.opcode!=19){error="unregistered original message visual event "+std::to_string(event.opcode);return false;}
    return checked(error);
}
bool Visuals::tick(std::string& error){
    error.clear();if(executing_||!failure_.empty()){if(failure_.empty())failure_="message visual tick reentered";error=failure_;return false;}
    Guard guard(executing_);scene_.tickPrimary();scene_.tickSecondary();return checked(error);
}
bool Visuals::pose(const anm::Node& node,VisualSnapshot& result,unsigned depth) const{
    if(depth>32)return false;const auto& state=node.vm.state();result.id=node.id;result.state=state;
    result.x=state.x+state.engineX+state.offsetX;result.y=state.y+state.engineY+state.offsetY;result.z=state.z+state.engineZ+state.offsetZ;
    if(node.parent){const auto found=scene_.nodes().find(node.parent);VisualSnapshot parent;
        if(found==scene_.nodes().end()||!pose(*found->second,parent,depth+1))return false;
        result.x+=parent.x;result.y+=parent.y;result.z+=parent.z;result.state.sx*=parent.state.sx;result.state.sy*=parent.state.sy;if(state.mode==1)result.state.rotation+=parent.state.rotation;
    }
    const auto bank=scene_.registry.bank(state.spriteBank);if(bank&&state.sprite>=0&&std::size_t(state.sprite)<bank->sprites.size())result.sprite=bank->sprites[std::size_t(state.sprite)];
    return true;
}
std::array<TextPresentation,4> Visuals::textPresentation() const{
    std::array<TextPresentation,4> result{};
    for(const auto& [identifier,node]:scene_.nodes()){
        const auto& state=node->vm.state();
        if(!node->alive||node->parent||state.bank!=6||state.script<0||state.script>3)continue;
        auto& pose=result[std::size_t(state.script)];
        pose.visible=state.visible&&(state.flags&2u)&&!(state.flags&0x20000000u)&&!state.deleteRequested;
        pose.alpha=pose.visible?float(state.primary>>24)/255.f:0;
        pose.sx=state.sx;pose.sy=state.sy;pose.offsetY=state.offsetY;
    }
    return result;
}
std::vector<VisualSnapshot> Visuals::snapshots() const{
    std::vector<VisualSnapshot> result;
    for(const auto* node:scene_.chain(anm::Membership::Primary)){VisualSnapshot item;if(pose(*node,item))result.push_back(item);}
    return result;
}
}
