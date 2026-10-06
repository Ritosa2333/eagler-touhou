// Native VM ownership and saved-successor traversal. Simulation-only.
#pragma once
#include "anm_logic.hpp"
#include <list>
namespace th12::anm_logic {
enum class Membership {Embedded,Primary,Secondary};
struct Node {
  uint32_t id=0,owner=0,parent=0;uint64_t key=0;Membership membership=Membership::Primary;
  VM vm;bool alive=true,detached=false,customGeometry=false;float viewportX=224,viewportY=16;
  std::function<int(int)> spriteRemap;
  std::function<bool()> beforeTick;
};
class Scene {
  std::list<std::shared_ptr<Node>> primary_,secondary_,embedded_;
  std::unordered_map<uint64_t,std::shared_ptr<Node>> roots_;
  std::unordered_map<uint32_t,std::shared_ptr<Node>> nodes_;
  Node* executing_=nullptr;uint32_t nextId_=1;
  Runtime runtimeFor(Node& node){
    Runtime result=runtime;result.remapSprite=node.spriteRemap;
    result.spawnChild=[this,&node](const ChildRequest& request){
      auto child=std::make_shared<Node>();child->id=nextId_++;child->owner=request.detached?0:node.owner;child->parent=request.detached?0:node.id;child->detached=request.detached;
      child->viewportX=node.viewportX;child->viewportY=node.viewportY;
      child->membership=(request.registrationFlags&4)?Membership::Secondary:Membership::Primary;
      auto& state=child->vm.control();state.engineX=request.x;state.engineY=request.y;state.engineZ=request.z;state.layer=node.vm.state().layer;
      nodes_[child->id]=child;auto childRuntime=runtimeFor(*child);
      if(!child->vm.bind(registry,request.bank,request.script,childRuntime)){child->alive=false;return;}
      if(request.hasOffset){child->vm.control().offsetX=request.offsetX;child->vm.control().offsetY=request.offsetY;}
      auto& list=child->membership==Membership::Secondary?secondary_:primary_;
      if(request.registrationFlags&2)list.push_front(child);else list.push_back(child);
    };return result;
  }
  void tickNode(Node& node){if(!node.alive)return;if(node.beforeTick&&node.beforeTick()){node.alive=false;return;}auto local=runtimeFor(node);const bool ended=node.vm.tick(local);if(node.vm.state().deleteRequested||(ended&&node.membership!=Membership::Embedded))node.alive=false;}
  void traverse(std::list<std::shared_ptr<Node>>& list){
    for(auto at=list.begin();at!=list.end();){const auto current=*at;const auto next=std::next(at);tickNode(*current);at=next;}
  }
public:
  Registry registry;Runtime runtime;
  void reset(){primary_.clear();secondary_.clear();embedded_.clear();roots_.clear();nodes_.clear();nextId_=1;}
  Node* find(uint64_t key){auto at=roots_.find(key);return at==roots_.end()||!at->second->alive?nullptr:at->second.get();}
  const auto& nodes()const{return nodes_;}
  std::vector<const Node*> chain(Membership membership)const{
    const auto& list=membership==Membership::Primary?primary_:membership==Membership::Secondary?secondary_:embedded_;
    std::vector<const Node*> result;for(const auto& node:list)if(node->alive)result.push_back(node.get());return result;
  }
  Node* bind(uint64_t key,uint32_t owner,int bank,int script,Membership membership=Membership::Primary,float x=0,float y=0,std::function<int(int)> remap={},bool head=false,const std::array<float,3>* scriptPosition=nullptr,float viewportX=224,float viewportY=16,int layer=0,bool withoutReset=false){
    if(!registry.bank(bank))return nullptr;
    if(auto old=roots_.find(key);old!=roots_.end())old->second->alive=false;
    auto node=std::make_shared<Node>();node->id=nextId_++;node->key=key;node->owner=owner;node->membership=membership;node->spriteRemap=std::move(remap);
    node->viewportX=viewportX;node->viewportY=viewportY;node->vm.control().engineX=x;node->vm.control().engineY=y;node->vm.control().layer=layer;
    nodes_[node->id]=node;roots_[key]=node;auto local=runtimeFor(*node);
    const bool bound=withoutReset?node->vm.rebindWithoutReset(registry,bank,script,local):node->vm.bind(registry,bank,script,local,scriptPosition);
    if(!bound){node->alive=false;return nullptr;}
    auto& list=membership==Membership::Primary?primary_:membership==Membership::Secondary?secondary_:embedded_;
    if(head)list.push_front(node);else list.push_back(node);return node.get();
  }
  void interrupt(uint64_t key,int code,bool immediate=false){if(auto* node=find(key)){node->vm.interrupt(code);if(immediate)tickNode(*node);}}
  void position(uint64_t key,float x,float y){if(auto* node=find(key)){auto& state=node->vm.control();state.engineX=x;state.engineY=y;}}
  void tickEmbedded(uint64_t key){if(auto* node=find(key))tickNode(*node);}
  void custom(uint64_t key,std::function<bool()> update){if(auto* node=find(key))node->beforeTick=std::move(update);}
  bool rebind(uint64_t key,int bank,int script,bool withoutReset=true){if(auto* node=find(key)){auto local=runtimeFor(*node);return withoutReset?node->vm.rebindWithoutReset(registry,bank,script,local):node->vm.bind(registry,bank,script,local);}return false;}
  void tickPrimary(){traverse(primary_);}
  void tickSecondary(){traverse(secondary_);}
  void remove(uint64_t key){if(auto* node=find(key))node->alive=false;}
  void removeOwner(uint32_t owner){for(auto& [id,node]:nodes_)if(node->owner==owner&&!node->detached)node->alive=false;}
  void collect(){
    // Parent retirement also retires attached descendants before the next pass.
    // Detached children have no parent and retain independent ownership.
    bool changed=true;while(changed){changed=false;for(auto& [id,node]:nodes_)if(node->alive&&node->parent){auto parent=nodes_.find(node->parent);if(parent==nodes_.end()||!parent->second->alive){node->alive=false;changed=true;}}}
    auto inactive=[](const auto& node){return !node->alive;};primary_.remove_if(inactive);secondary_.remove_if(inactive);embedded_.remove_if(inactive);
    for(auto at=roots_.begin();at!=roots_.end();)if(!at->second->alive)at=roots_.erase(at);else ++at;
    for(auto at=nodes_.begin();at!=nodes_.end();)if(!at->second->alive)at=nodes_.erase(at);else ++at;
  }
};
}
