#include "PlayerShotGroups.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>
#include <set>
#include <utility>

namespace th128::playershot {
namespace {

constexpr std::array<std::array<int,2>,6> kScripts{{
    {0xb,0xe},{0xc,0xf},{0xb,0xd},{0xc,0xe},{0xf,0x11},{0x10,0x12}}};
constexpr std::uint32_t kLayer=0xc;
constexpr std::uint32_t kOwner=1;

struct Guard {
    bool& state;
    explicit Guard(bool& value):state(value){state=true;}
    ~Guard(){state=false;}
};

std::uint32_t word32(const std::array<std::uint8_t,ShtRecordBytes>& raw,std::size_t offset){
    return std::uint32_t(raw[offset])|std::uint32_t(raw[offset+1])<<8|
           std::uint32_t(raw[offset+2])<<16|std::uint32_t(raw[offset+3])<<24;
}

float floating(const std::array<std::uint8_t,ShtRecordBytes>& raw,std::size_t offset){
    return std::bit_cast<float>(word32(raw,offset));
}

bool bounded(float value){return std::isfinite(value)&&std::abs(value)<=1048576.f;}

std::uint64_t key(std::uint32_t group,std::uint32_t slot){return 1+group*2+slot;}

}

bool PlayerShotGroups::fail(std::string message,std::string& output){
    if(failure_.empty())failure_=std::move(message);
    output=failure_;return false;
}

void PlayerShotGroups::runtime(){
    scene_.runtime={};
    scene_.runtime.instructionBudget=4096;
    scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
    scene_.runtime.gameRandom32=[this]{if(failure_.empty())failure_="unproved player shot-group game RNG";return 0u;};
    scene_.runtime.displayRandom32=[this]{if(failure_.empty())failure_="unproved player shot-group display RNG";return 0u;};
    scene_.runtime.unsupported=[this](int,int script,int opcode,const char* reason){
        if(failure_.empty())failure_="player shot-group script="+std::to_string(script)+" opcode="+
            std::to_string(opcode)+": "+reason;
    };
}

void PlayerShotGroups::abandon(){
    scene_.removeOwner(kOwner);
    scene_.collect();
    for(auto& group:roots_)group.fill(0);
}

void PlayerShotGroups::updatePositions(PlayerShotPose pose){
    pose_=pose;
    for(std::uint32_t group=0;group<8;++group)
        for(std::uint32_t slotIndex=0;slotIndex<2;++slotIndex)
            if(roots_[group][slotIndex]){
                auto* root=scene_.find(keys_[group][slotIndex]);
                if(!root)continue;
                auto& control=root->vm.control();
                control.engineX=pose.x+relative_[group][slotIndex].x;
                control.engineY=pose.y+relative_[group][slotIndex].y;
                control.engineZ=pose.z+relative_[group][slotIndex].z;
            }
}

bool PlayerShotGroups::load(std::span<const std::uint8_t> sht,std::span<const std::uint8_t> animation,std::string& error){
    error.clear();
    if(loaded_||configured_||executing_){error="player shot groups require a fresh host";return false;}
    auto bank=std::make_unique<ShtBank>();
    if(!bank->load(sht,error))return false;
    anm::Registry candidate;
    if(!candidate.loadBank(7,animation.data(),animation.size(),error))return false;
    const auto resource=candidate.bank(7);
    if(!resource||resource->scripts.size()<=0x12){error="actual pl00 animation scripts 0xb..0x12 are required";return false;}
    scene_.registry=std::move(candidate);
    bank_=std::move(bank);
    loaded_=true;
    runtime();
    return true;
}

bool PlayerShotGroups::create(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,std::string& error){
    if(selector!=0){error="only the verified pl00a selector0 shot mode is supported";return false;}
    const auto* source=bank_?bank_->group(group):nullptr;
    if(!source||source->records.empty()){error="unsupported player SHT group";return false;}
    const auto count=std::min<std::uint32_t>(8,static_cast<std::uint32_t>(source->records.size()));
    for(std::uint32_t index=0;index<count;++index){
        const auto& record=source->records[index];
        PlayerShotPose relative{floating(record.raw,4),floating(record.raw,8),0};
        if(!bounded(relative.x)||!bounded(relative.y)){error="unsupported player SHT group position";return false;}
        for(std::uint32_t slotIndex=0;slotIndex<2;++slotIndex){
            const auto script=kScripts[selector][slotIndex];
            const auto slotKey=key(index,slotIndex);
            auto* root=scene_.bind(slotKey,kOwner,7,script,anm::Membership::Primary,
                pose.x+relative.x,pose.y+relative.y,{},false,nullptr,224,16,
                static_cast<int>(kLayer),false,pose.z);
            if(!root){error="actual player shot-group ANM binding failed";return false;}
            roots_[index][slotIndex]=root->id;
            keys_[index][slotIndex]=slotKey;
            relative_[index][slotIndex]=relative;
            state3f8_[root->id]=0;
        }
    }
    group_=group;selector_=selector;pose_=pose;
    return true;
}

bool PlayerShotGroups::configureWith(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,
                                     Lifecycle lifecycle,std::optional<NativeCallback> native,
                                     std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!loaded_||configured_||executing_||(!native&&!lifecycle)){
        error="player shot groups are not freshly loadable/configurable";return false;
    }
    lifecycle_=std::move(lifecycle);
    nativeCallback_=native;
    if(!create(group,selector,pose,error)){abandon();return fail(error.empty()?"player shot-group creation failed":error,error);}
    configured_=true;retired_=false;return true;
}

bool PlayerShotGroups::configure(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,
                                 Lifecycle lifecycle,std::string& error){
    return configureWith(group,selector,pose,std::move(lifecycle),std::nullopt,error);
}

bool PlayerShotGroups::configureNativeDefault(std::uint32_t group,std::uint32_t selector,
                                              PlayerShotPose pose,std::string& error){
    return configureWith(group,selector,pose,{},NativeCallback::Default412b40,error);
}

bool PlayerShotGroups::configureNativeDefaultSelection(float cumulativeArea,int difficulty,
                                                       std::uint32_t selector,PlayerShotPose pose,
                                                       std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!loaded_||!bank_||configured_||executing_){
        error="player shot groups are not freshly loadable";return false;
    }
    const auto plan=planShotGroup(*bank_,cumulativeArea,difficulty);
    if(!plan||plan->continuation!=ShotGroupContinuation::Ready||!plan->group){
        error="unsupported player shot-group selection";return false;
    }
    return configureWith(*plan->group,selector,pose,{},NativeCallback::Default412b40,error);
}

bool PlayerShotGroups::configureSelection(float cumulativeArea,int difficulty,std::uint32_t selector,
                                          PlayerShotPose pose,Lifecycle lifecycle,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!loaded_||!bank_||configured_||executing_){error="player shot groups are not freshly loadable";return false;}
    const auto plan=planShotGroup(*bank_,cumulativeArea,difficulty);
    if(!plan||plan->continuation!=ShotGroupContinuation::Ready||!plan->group){
        error="unsupported player shot-group selection";return false;
    }
    return configure(*plan->group,selector,pose,std::move(lifecycle),error);
}

bool PlayerShotGroups::update(std::uint32_t group,std::uint32_t selector,PlayerShotPose pose,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_||executing_){error="player shot groups are not configured";return false;}
    if(selector!=0){error="only the verified pl00a selector0 shot mode is supported";return false;}
    const auto* source=bank_?bank_->group(group):nullptr;
    if(!source||source->records.empty()){error="unsupported player SHT group";return false;}
    const auto desired=std::min<std::uint32_t>(8,static_cast<std::uint32_t>(source->records.size()));
    std::uint32_t current=0;
    for(std::uint32_t index=0;index<8;++index)
        for(std::uint32_t slotIndex=0;slotIndex<2;++slotIndex)
            if(roots_[index][slotIndex]){
                if(scene_.find(keys_[index][slotIndex]))++current;
                else roots_[index][slotIndex]=0;
            }
    if(group!=group_||selector!=selector_||desired!=current){
        if(!clear(1,error))return false;
        if(!create(group,selector,pose,error)){abandon();return fail(error.empty()?"player shot-group update failed":error,error);}
        retired_=false;return true;
    }
    for(std::uint32_t index=0;index<desired;++index){
        const auto relative=PlayerShotPose{floating(source->records[index].raw,4),
                                           floating(source->records[index].raw,8),0};
        for(std::uint32_t slotIndex=0;slotIndex<2;++slotIndex)relative_[index][slotIndex]=relative;
    }
    updatePositions(pose);
    return true;
}

bool PlayerShotGroups::updateSelection(float cumulativeArea,int difficulty,std::uint32_t selector,
                                       PlayerShotPose pose,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!loaded_||!bank_||!configured_||executing_){error="player shot groups are not configured";return false;}
    const auto plan=planShotGroup(*bank_,cumulativeArea,difficulty);
    if(!plan||plan->continuation!=ShotGroupContinuation::Ready||!plan->group){
        error="unsupported player shot-group selection";return false;
    }
    return update(*plan->group,selector,pose,error);
}

bool PlayerShotGroups::visitTree(std::uint32_t handle,const Visitor& visitor,std::string& error){
    const auto start=scene_.nodes().find(handle);
    if(start==scene_.nodes().end()||!start->second->alive){error="player shot-group handle is unavailable";return false;}
    std::set<std::uint32_t> seen;
    const std::function<bool(std::uint32_t)> visit=[&](std::uint32_t id){
        if(!seen.insert(id).second)return true;
        const auto at=scene_.nodes().find(id);
        if(at==scene_.nodes().end()||!at->second->alive)return true;
        error.clear();
        try {
            const bool accepted=visitor(id,at->second->vm,error);
            if(!accepted||!error.empty()){
                const std::string reason=error.empty()?"required player shot-group visitor rejected":error;
                return fail(reason,error);
            }
        } catch(...) {
            return fail("player shot-group visitor threw",error);
        }
        for(const auto* child:scene_.children(id))
            if(!visit(child->id))return false;
        return true;
    };
    return visit(handle);
}

bool PlayerShotGroups::walk(std::uint32_t handle,const Visitor& visitor,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_||executing_||!visitor){error="player shot-group walk is unavailable";return false;}
    return visitTree(handle,visitor,error);
}

bool PlayerShotGroups::retireRoot(std::uint32_t handle,int code,std::string& error){
    std::uint32_t group=0,slotIndex=0;
    bool found=false;
    for(;group<8&&!found;++group)
        for(slotIndex=0;slotIndex<2;++slotIndex)
            if(roots_[group][slotIndex]==handle){found=true;break;}
    if(!found){error="player shot-group handle is not an active slot";return false;}
    --group;
    std::vector<std::pair<std::uint32_t,anm::VM*>> nodes;
    if(!visitTree(handle,[&](std::uint32_t id,anm::VM& vm,std::string&){
        nodes.emplace_back(id,&vm);return true;
    },error))return false;
    for(const auto& [id,vm]:nodes){
        if(nativeCallback_){
            callbackResult_=0;
        } else {
            error.clear();
            try {
                const bool accepted=lifecycle_(id,*vm,code,error);
                if(!accepted||!error.empty()){
                    const std::string reason=error.empty()?"required player shot-group lifecycle callback rejected":error;
                    return fail(reason,error);
                }
            } catch(...) {
                return fail("player shot-group lifecycle callback threw",error);
            }
        }
        state3f8_[id]=static_cast<std::uint16_t>(code);
    }
    scene_.remove(keys_[group][slotIndex]);
    scene_.collect();
    roots_[group][slotIndex]=0;
    retiredHandles_+=static_cast<std::uint32_t>(nodes.size());
    return true;
}

bool PlayerShotGroups::retire(std::uint32_t handle,int code,std::string& error){
    return retireWithChildren(handle,code,error);
}

bool PlayerShotGroups::retireWithChildren(std::uint32_t handle,int code,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_||executing_){error="player shot-group retirement is unavailable";return false;}
    Guard guard(executing_);
    return retireRoot(handle,code,error);
}

bool PlayerShotGroups::clear(int code,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_||executing_){error="player shot-group clear is unavailable";return false;}
    Guard guard(executing_);
    for(std::uint32_t group=0;group<8;++group)
        for(std::uint32_t slotIndex=0;slotIndex<2;++slotIndex){
            if(!roots_[group][slotIndex])continue;
            if(!scene_.find(keys_[group][slotIndex])){roots_[group][slotIndex]=0;continue;}
            if(!retireRoot(roots_[group][slotIndex],code,error))return false;
        }
    retired_=true;return true;
}

bool PlayerShotGroups::tick(PlayerShotPose pose,std::string& error){
    error.clear();
    if(!failure_.empty())return fail(failure_,error);
    if(!configured_||executing_){error="player shot-group tick is unavailable";return false;}
    Guard guard(executing_);
    updatePositions(pose);
    scene_.tickPrimary();
    for(const auto& [id,node]:scene_.nodes()){
        if(node->vm.faulted()||node->detached||node->customGeometry){
            abandon();return fail("unimplemented player shot-group Scene child ownership",error);
        }
    }
    scene_.collect();
    for(std::uint32_t group=0;group<8;++group)
        for(std::uint32_t slotIndex=0;slotIndex<2;++slotIndex)
            if(roots_[group][slotIndex]&&!scene_.find(keys_[group][slotIndex]))roots_[group][slotIndex]=0;
    return true;
}

std::uint32_t PlayerShotGroups::slot(std::uint32_t group,std::uint32_t index) const{
    return group<8&&index<2?roots_[group][index]:0;
}

anm::VM* PlayerShotGroups::vm(std::uint32_t handle) const{
    const auto at=scene_.nodes().find(handle);
    return at==scene_.nodes().end()||!at->second->alive?nullptr:&at->second->vm;
}

std::uint16_t PlayerShotGroups::nativeState3f8(std::uint32_t handle) const{
    const auto at=state3f8_.find(handle);
    return at==state3f8_.end()?0:at->second;
}

std::vector<PlayerShotGroupSnapshot> PlayerShotGroups::snapshots() const{
    std::vector<PlayerShotGroupSnapshot> result;
    const auto bank=scene_.registry.bank(7);
    if(!bank)return result;
    std::array<std::pair<std::uint32_t,std::uint32_t>,16> labels{};
    std::size_t labelCount=0;
    for(std::uint32_t group=0;group<8;++group)
        for(std::uint32_t slotIndex=0;slotIndex<2;++slotIndex)
            if(roots_[group][slotIndex])labels[labelCount++]={roots_[group][slotIndex],group*2+slotIndex};
    for(const auto& [id,node]:scene_.nodes()){
        if(!node->alive)continue;
        std::uint32_t root=id;
        const auto* current=node.get();
        for(std::size_t depth=0;depth<scene_.nodes().size()&&current->parent;++depth){
            const auto parent=scene_.nodes().find(current->parent);
            if(parent==scene_.nodes().end())break;
            root=parent->second->id;current=parent->second.get();
        }
        std::optional<std::pair<std::uint32_t,std::uint32_t>> label;
        for(std::size_t index=0;index<labelCount;++index)
            if(labels[index].first==root)label=labels[index];
        if(!label)continue;
        const auto& state=node->vm.state();
        std::optional<anm::SpriteInfo> sprite;
        if(state.sprite>=0&&std::size_t(state.sprite)<bank->sprites.size())
            sprite=bank->sprites[std::size_t(state.sprite)];
        float x=state.x+state.engineX+state.offsetX;
        float y=state.y+state.engineY+state.offsetY;
        float z=state.z+state.engineZ+state.offsetZ;
        for(const auto* at=node.get();at->parent;){
            const auto parent=scene_.nodes().find(at->parent);
            if(parent==scene_.nodes().end())break;
            const auto& owner=parent->second->vm.state();
            x+=owner.x+owner.engineX+owner.offsetX;
            y+=owner.y+owner.engineY+owner.offsetY;
            z+=owner.z+owner.engineZ+owner.offsetZ;
            at=parent->second.get();
        }
        const auto stateWord=state3f8_.find(id);
        result.push_back({label->second/2,label->second%2,id,node->parent,state.script,
                          stateWord==state3f8_.end()?std::uint16_t(0):stateWord->second,state,sprite,x,y,z});
    }
    return result;
}

PlayerShotGroupsState PlayerShotGroups::state() const{
    std::uint32_t groups=0,handles=0;
    for(const auto& roots:roots_)for(const auto handle:roots)handles+=handle!=0?1u:0u;
    for(const auto& roots:roots_)groups+=roots[0]!=0||roots[1]!=0?1u:0u;
    return {loaded_,configured_,retired_,!failure_.empty(),group_,selector_,groups,handles,
            retiredHandles_,nativeCallback_.has_value(),callbackResult_};
}

}
