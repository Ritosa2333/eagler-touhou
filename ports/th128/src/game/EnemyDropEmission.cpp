#include "EnemyDropEmission.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

namespace th128::enemy {
namespace {
constexpr double Pi=3.1415927410125732421875,Tau=6.283185482025146484375;
bool bounded(float value){return std::isfinite(value)&&std::abs(value)<=4096;}
struct Guard {bool& state;explicit Guard(bool& value):state(value){state=true;}~Guard(){state=false;}};
}
float initialDropAngle(std::uint32_t random) noexcept {
    const float signedUnit=float(double(random)*0x1p-31-1.);
    return float(double(signedUnit)*Pi);
}
std::optional<float> advanceDropAngle(float angle,std::uint32_t random) noexcept {
    if(!bounded(angle))return std::nullopt;
    const float signedUnit=float(double(random)*0x1p-31-1.);
    float next=float(double(angle)+1.5707963705062866+double(signedUnit)*Pi*.25);
    int loops=0;
    while(double(next)>Pi&&loops++<=32)next=float(double(next)-Tau);
    while(double(next)<-Pi&&loops++<=32)next=float(double(next)+Tau);
    if(loops>33)return std::nullopt;
    return next;
}
std::optional<DropPosition> scatterDrop(DropPosition position,float angle,float width,float height,std::uint32_t random) noexcept {
    if(!bounded(position.x)||!bounded(position.y)||!bounded(position.z)||!bounded(angle)||!bounded(width)||!bounded(height))return std::nullopt;
    const float horizontal=float(std::cos(double(angle))*width),vertical=float(std::sin(double(angle))*height);
    const float fraction=float(double(random)*0x1p-32);
    const float radius=float(.5+double(fraction)*.5);
    const float dx=float(double(horizontal)*radius),dy=float(double(vertical)*radius);
    return DropPosition{float(double(position.x)+dx),float(double(position.y)+dy),float(double(position.z)+0.)};
}
AdditionalDropOperation::AdditionalDropOperation(DropState state,DropPosition position):state_(state),position_(position){}
bool AdditionalDropOperation::fail(std::string message,std::string& error){if(failure_.empty())failure_=std::move(message);error=failure_;return false;}
bool AdditionalDropOperation::run(RandomHost random,SpawnHost spawn,std::string& error){
    error.clear();
    if(started_||executing_||!failure_.empty()||!random||!spawn)return fail(failure_.empty()?"additional-drop operation is unavailable or already consumed":failure_,error);
    started_=true;Guard guard(executing_);
    unsigned total=0;
    for(std::size_t index=1;index<=9;++index){
        if(state_.words[index]<0||state_.words[index]>600)return fail("unsupported additional-drop count domain",error);
        total+=unsigned(state_.words[index]);
    }
    if(total>600||!bounded(state_.width)||!bounded(state_.height)||!bounded(position_.x)||!bounded(position_.y)||!bounded(position_.z))
        return fail("additional-drop geometry or count budget exceeded",error);
    try{
        float angle=initialDropAngle(random());
        for(int kind=1;kind<=9;++kind){
            for(int item=0;item<state_.words[std::size_t(kind)];++item){
                const auto position=scatterDrop(position_,angle,state_.width,state_.height,random());
                if(!position)return fail("unsupported additional-drop scatter",error);
                if(!spawn({kind,*position,-1.5707963705062866f,2.200000047683716f},error)||!error.empty()||!failure_.empty())
                    return fail(error.empty()?"required additional item birth rejected":error,error);
                const auto next=advanceDropAngle(angle,random());
                if(!next)return fail("unsupported additional-drop angle continuation",error);
                angle=*next;
            }
        }
    }catch(...){return fail("additional-drop random or item host threw",error);}
    resetAdditionalDrops(state_);return true;
}
const DropState& AdditionalDropOperation::state() const{return state_;}
}
