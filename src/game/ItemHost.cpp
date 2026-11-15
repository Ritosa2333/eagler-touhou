#include "ItemHost.hpp"
#include "PlayerShotMotion.hpp"
#include <algorithm>
#include <limits>

namespace th128::item {
namespace {
bool valid(Position position){return std::isfinite(position.x)&&std::isfinite(position.y)&&std::isfinite(position.z)&&
    std::abs(position.x)<=4096&&std::abs(position.y)<=4096&&std::abs(position.z)<=4096;}
struct Guard {bool& executing;explicit Guard(bool& state):executing(state){executing=true;}~Guard(){executing=false;}};
}
bool ItemHost::fail(std::string reason,std::string& error){if(failure_.empty())failure_=std::move(reason);error=failure_;return false;}
anm::Runtime ItemHost::runtime(Item& item){
    anm::Runtime context;context.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    context.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    context.unsupported=[&item](int,int,int opcode,const char* reason){if(item.fault.empty())item.fault="item ANM opcode="+std::to_string(opcode)+": "+reason;};
    return context;
}
bool ItemHost::load(std::span<const std::uint8_t> animation,std::string& error){
    error.clear();if(executing_||loaded_){error="item resources require a fresh inactive host";return false;}
    loaded_=registry_.loadBank(0,animation.data(),animation.size(),error);return loaded_;
}
bool ItemHost::setCollectHost(CollectHost host){if(executing_)return false;collect_=std::move(host);return true;}
bool ItemHost::setMissHost(std::function<bool(std::string&)> host){if(executing_)return false;miss_=std::move(host);return true;}
bool ItemHost::enableIceItems(float attractionSpeed,std::string& error){
    error.clear();
    if(!loaded_||executing_||iceEnabled_||!failure_.empty()||!std::isfinite(attractionSpeed)||attractionSpeed<=0||attractionSpeed>128){
        error="ice items require fresh owning bullet bank and actual SHT attraction speed";return false;
    }
    nativeAttractionSpeed_=attractionSpeed;iceEnabled_=true;return true;
}
IceState ItemHost::iceState() const {
    unsigned pending=0;for(std::size_t index=Capacity;index<slots_.size();++index)if(slots_[index]&&slots_[index]->state==5)++pending;
    return {iceCursor_,iceBirths_,pending};
}
bool ItemHost::spawn(int kind,Position position,float angle,float speed,std::string& error){
    error.clear();if(executing_||!loaded_||!failure_.empty())return fail(failure_.empty()?"item host unavailable or reentered":failure_,error);
    if(kind<1||kind>9||kind==4||kind==5||kind==6||kind==7||!valid(position)
        ||(kind==9&&(!iceEnabled_||angle!=0||speed!=0)))return fail("unimplemented item birth kind/position",error);
    auto free=slots_.end();int delay=0;
    if(kind==9){
        if(iceBirths_>=1000000)return fail("ice item birth-counter budget",error);
        const unsigned cursor=iceCursor_;iceCursor_=(iceCursor_+1)%IceCapacity;++iceBirths_;
        free=slots_.begin()+Capacity+cursor;if(*free)return true;
        delay=int(iceBirths_>=1024?cursor%32+16:iceBirths_>=512?cursor%16+8:iceBirths_>=256?cursor%8+4:cursor%4);
    }else free=std::find_if(slots_.begin(),slots_.begin()+Capacity,[](const auto& slot){return !slot;});
    if((kind!=9&&free==slots_.begin()+Capacity)||free==slots_.end())return true;
    const auto motion=playershot::computeShotMotion(angle,speed);
    if(!motion||!nextId_)return fail("unsupported item polar motion or identity budget",error);
    Guard guard(executing_);auto candidate=std::make_unique<Item>();
    candidate->id=nextId_++;candidate->kind=kind==1?2:kind;candidate->position=position;
    if(kind!=9)candidate->position.x=std::clamp(position.x,-192.f,192.f);
    candidate->velocity={motion->velocity.x,motion->velocity.y,0};
    if(kind==9){candidate->state=5;candidate->delay=delay;*free=std::move(candidate);return true;}
    auto context=runtime(*candidate);
    if(!candidate->primary.bind(registry_,0,candidate->kind+232,context)||candidate->primary.faulted()||
       !candidate->secondary.bind(registry_,0,candidate->kind+252,context)||candidate->secondary.faulted()||!candidate->fault.empty())
        return fail(candidate->fault.empty()?"required item animation binding failed":candidate->fault,error);
    *free=std::move(candidate);return true;
}
bool ItemHost::tick(Position player,int playerState,bool focused,std::string& error){
    error.clear();if(executing_||!loaded_||!failure_.empty())return fail(failure_.empty()?"item tick unavailable or reentered":failure_,error);
    if(!valid(player)||playerState<0||playerState>4)return fail("unsupported item player context",error);
    Guard guard(executing_);
    for(auto& owned:slots_){
        if(!owned)continue;auto& item=*owned;
        if(item.state==5){
            if(--item.delay>=0)continue;
            item.state=2;auto context=runtime(item);
            if(!item.primary.bind(registry_,0,241,context)||item.primary.faulted()||!item.fault.empty())return fail("required ice item ANM241 bind failed",error);
            continue;
        }
        if(item.state==2){
            item.position={float(double(item.position.x)+item.velocity.x),float(double(item.position.y)+item.velocity.y),float(double(item.position.z)+item.velocity.z)};
            item.velocity.y=float(double(item.velocity.y)+0.029999999329447746);
            if(item.velocity.y>=0){item.state=3;item.attractionSpeed=nativeAttractionSpeed_;}
            else if(item.position.y>472){
                if(!miss_||!miss_(error)||!error.empty())return fail(error.empty()?"required ice item miss update":error,error);
                owned.reset();continue;
            }
        }
        if(item.state==1){
            const auto motion=ordinaryMotion({item.position,item.velocity,playerState,player.y});
            if(!motion)return fail("unsupported item movement domain",error);
            if(motion->continuation==MotionContinuation::AutoAttraction){item.state=3;item.attractionSpeed=nativeAttractionSpeed_;}
            else{
                item.position=motion->position;item.velocity=motion->velocity;
                if(motion->continuation==MotionContinuation::Removed){
                    if(!miss_||!miss_(error)||!error.empty())return fail(error.empty()?"required item miss-state update":error,error);
                    owned.reset();continue;
                }
            }
        }
        if(item.state==3||item.state==4){
            const float dx=float(double(player.x)-item.position.x),dy=float(double(player.y)-item.position.y);
            const float angle=dx==0&&dy==0?1.5707963705062866f:float(std::atan2(double(dy),double(dx)));
            const auto velocity=playershot::computeShotMotion(angle,item.attractionSpeed);
            if(!velocity)return fail("unsupported native item attraction motion",error);
            item.velocity={velocity->velocity.x,velocity->velocity.y,0};
            item.position.x=float(double(item.position.x)+item.velocity.x);item.position.y=float(double(item.position.y)+item.velocity.y);
            if(item.attractionSpeed<=12)item.attractionSpeed=float(double(item.attractionSpeed)+0.20000000298023224);
            if(playerState==4){item.state=1;item.velocity={};}
        }
        if(playerState!=2){
            const auto contact=contactDecision(item.position,player,30,50,focused);
            if(!contact)return fail("unsupported item contact domain",error);
            if(*contact==Contact::Collect){
                if(!collect_||!collect_(item.kind,item.position,item.state,error)||!error.empty())return fail(error.empty()?"required item reward at 0x428b52":error,error);
                owned.reset();continue;
            }
            if(*contact==Contact::Attract&&item.state==1){item.state=4;item.attractionSpeed=float(double(nativeAttractionSpeed_)/3.);}
        }
        auto context=runtime(item);item.primary.tick(context);if(item.kind!=9)item.secondary.tick(context);
        if(item.primary.faulted()||item.secondary.faulted()||!item.fault.empty())return fail(item.fault.empty()?"item animation update failed":item.fault,error);
        if(item.age>=1000000)return fail("item timer budget",error);++item.age;
    }
    return true;
}
std::vector<Snapshot> ItemHost::snapshots() const{
    std::vector<Snapshot> result;const auto bank=registry_.bank(0);if(!bank)return result;
    for(std::size_t index=0;index<slots_.size();++index){
        if(!slots_[index])continue;const auto& item=*slots_[index];const auto& state=item.primary.state();
        if(state.sprite<0||std::size_t(state.sprite)>=bank->sprites.size())continue;
        result.push_back({item.id,std::uint32_t(index),item.kind,item.age,item.kind+232,item.position,item.velocity,state,bank->sprites[std::size_t(state.sprite)]});
    }
    return result;
}
}
