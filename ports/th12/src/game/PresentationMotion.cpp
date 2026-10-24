#include "PresentationMotion.hpp"
#include "PresentationTracking.hpp"
#include "GameState.hpp"
#include "LaserWorld.hpp"
#include "StageBackgroundEffect.hpp"
#include "UfoEffects.hpp"
#include <iomanip>
#include <sstream>
#include <unordered_set>

namespace th12 {
std::string presentationMotionBuffer;
namespace {
uint32_t continuousFields(const anm_logic::Node& node) {
  return node.vm.continuousFields()|((node.owner>>28)==1?2u:0u);
}
void lineage(std::ostream& out,const anm_logic::Node& node,int depth=0){
  out<<node.id<<','<<node.vm.presentationRevision();
  if(node.parent&&depth<16)if(const auto at=animationScene.nodes().find(node.parent);at!=animationScene.nodes().end()){
    out<<',';lineage(out,*at->second,depth+1);
  }
}
} // namespace
const std::string& capturePresentationMotion(){
  std::ostringstream out;out<<std::setprecision(9);
  out<<"{\"version\":1,\"epoch\":"<<presentation::epoch<<",\"frame\":"<<frame
     <<",\"playerState\":"<<playerState<<",\"focusId\":";
  const auto* focus=animationScene.find(playerFocusAnimation);
  out<<(focus?focus->id:0)<<",\"backgroundRevision\":"<<stageState.presentationRevision
     <<",\"owners\":[";
  bool comma=false;
  std::unordered_set<uint32_t> exportedOwners;
  const auto owner=[&](uint32_t id,float x,float y,float z,uint32_t revision,const char* kind){
    if(!exportedOwners.insert(id).second)return;
    if(comma)out<<',';comma=true;
    out<<"{\"id\":"<<id<<",\"revision\":"<<revision<<",\"position\":["<<x<<','<<y<<','<<z<<"],\"kind\":\""<<kind<<"\"}";
  };
  owner(0xffff0000u,px/128.f+224,py/128.f+16,0,presentation::revision(0xffff0000u)*8u+uint32_t(playerState),"player");
  for(const auto& pointer:enemies){const auto& e=*pointer;if(e.active)
    owner(e.id,e.x+224,e.y+16,e.z,presentation::revision(e.id)+
      (e.isUfo?presentation::revision(0xfffd0001u)+presentation::revision(0xfffd0003u):0),"enemy");}
  for(const auto& b:bullets)if(b.active)
    owner(0x10000000u+b.entityId,b.x+224,b.y+16,0,
      presentation::revision(0x10000000u+b.entityId)*16u+
        (b.friendly?uint32_t(b.weapon.phase):uint32_t(b.enemy.phase)),"projectile");
  for(const auto& item:itemManager.entities)if(item.active())
    owner(0x20000000u+item.id,item.position.x+224,item.position.y+16,0,
      (presentation::revision(0x20000000u+item.id)+presentation::revision(0x30000000u+item.id)+presentation::revision(0x40000000u+item.id))*16u+uint32_t(item.state),"item");
  for(const auto& visual:playerBomb.visuals)if(visual.active){
    const bool sweep=visual.bank==1&&visual.script==24;
    owner(0x50000000u+visual.id,sweep?0:visual.x+224,sweep?0:visual.y+16,0,uint32_t(visual.interrupt),"bomb");
  }
  // Native bomb orbs/custom effects have explicit moving engine origins;
  // screen-coordinate sweep children keep their zero root origin above.
  for(const auto& [id,pointer]:animationScene.nodes()){
    const auto& node=*pointer;const auto& s=node.vm.state();
    if(node.alive&&!node.parent&&node.key&&((node.owner>>28)==5||(node.owner>>28)==10))
      owner(node.owner,s.engineX+node.viewportX,s.engineY+node.viewportY,s.engineZ,node.vm.presentationRevision(),"bomb");
  }
  std::unordered_map<uint64_t,uint32_t> attachedEnemies;
  for(const auto& enemy:enemies)if(enemy->active&&enemy->extraBombShieldAnimation)
    attachedEnemies.emplace(enemy->extraBombShieldAnimation,enemy->id);
  out<<"],\"nodes\":[";comma=false;
  for(const auto membership:{anm_logic::Membership::Primary,anm_logic::Membership::Secondary,anm_logic::Membership::Embedded})
    for(const auto* pointer:animationScene.chain(membership)){
      const auto& node=*pointer;const auto& s=node.vm.state();
      if(!node.alive)continue;if(comma)out<<',';comma=true;
      uint32_t motionOwner=node.owner;
      const char* meshKind="";
      if(node.owner==0xffff0020u)motionOwner=focus&&focus->id==node.id?0xffff0000u:node.owner;
      // Attached focus descendants resolve through their real ancestor IDs.
      if(node.owner==0xffff0020u&&focus){
        const auto* ancestor=&node;for(int depth=0;ancestor&&depth<16;++depth){
          if(ancestor->id==focus->id){motionOwner=0xffff0000u;break;}
          const auto at=animationScene.nodes().find(ancestor->parent);ancestor=at==animationScene.nodes().end()?nullptr:at->second.get();
        }
      }
      if(node.owner==0xfffd0001u||node.owner==0xfffd0003u)motionOwner=ufoManager.enemyId;
      if(node.owner==0xfffe0200u)if(const auto* boss=primaryBoss())motionOwner=boss->id;
      if(const auto attached=attachedEnemies.find(node.key);attached!=attachedEnemies.end())motionOwner=attached->second;
      if(stageBackgroundEffect.owns(node.key))meshKind="stage-grid";
      else if(const auto* effect=ufoEffects.state(node.id))meshKind=effect->kind==UfoEffectKind::Trail?"ufo-trail":"ufo-summon";
      else if((node.owner>>28)==0xbu)meshKind="laser";
      else if(s.mode==9)meshKind="spell-ring";
      else if(s.mode==13)meshKind="ufo-fill";
      else for(const auto& leaf:playerBomb.leaves)if(node.key==0x600000000ull+leaf.id){meshKind="leaf";break;}
      out<<"{\"id\":"<<node.id<<",\"owner\":"<<node.owner<<",\"motionOwner\":"<<motionOwner
         <<",\"parent\":"<<node.parent<<",\"lineage\":[";lineage(out,node);
      out<<"],\"local\":["<<s.x+s.offsetX<<','<<s.y+s.offsetY<<','<<s.z+s.offsetZ
         <<"],\"engine\":["<<s.engineX<<','<<s.engineY<<','<<s.engineZ
         <<"],\"mask\":"<<continuousFields(node)<<",\"flags\":"<<s.flags
         <<",\"stageSlot\":"<<((node.owner&0xff000000u)==0xc1000000u?int(node.owner-0xc1000000u):-1)
         <<",\"meshKind\":\""<<meshKind<<"\"}";
    }
  out<<"],\"lasers\":[";comma=false;
  for(const auto* object:laserManager.ordered()){
    const auto& s=object->state;if(comma)out<<',';comma=true;
    uint32_t revision=presentation::revision(0xb0000000u+s.serialId);
    uint32_t followOwner=0,followRevision=0;
    if(s.kind==laser::Kind::Timed&&(s.parameters.flags&1))if(const auto* boss=primaryBoss()){
      followOwner=boss->id;followRevision=presentation::revision(boss->id);
    }
    out<<"{\"owner\":"<<0xb0000000u+s.serialId<<",\"revision\":"<<revision
       <<",\"topology\":"<<s.presentationTopology<<",\"kind\":"<<uint32_t(s.kind)
       <<",\"phase\":"<<uint32_t(s.phase)<<",\"cursor\":"<<s.cursor
       <<",\"nodes\":"<<s.nodes.size()<<",\"followOwner\":"<<followOwner<<",\"followRevision\":"<<followRevision
       <<",\"age\":"<<s.age.value<<"}";
  }
  out<<"]}";presentationMotionBuffer=out.str();return presentationMotionBuffer;
}
// The JSON ABI above remains the compatibility/verification oracle. Ordinary
// rendering reads these packed records without diagnostic copies or text I/O.
const std::array<uint32_t,17>& capturePresentationMotionBinary(){
  struct Owner {uint32_t id,revision,kind;float position[3];};
  struct Node {uint32_t id,owner,motionOwner,parent;float local[3],engine[3];uint32_t mask,flags;int32_t stageSlot;uint32_t meshKind,lineageOffset,lineageCount;};
  struct Laser {uint32_t owner,revision,topology,kind,phase,cursor,nodes,followOwner,followRevision;float age;};
  static_assert(sizeof(Owner)==24&&sizeof(Node)==64&&sizeof(Laser)==40);
  static std::vector<Owner> owners;
  static std::vector<Node> nodes;
  static std::vector<Laser> lasers;
  static std::vector<uint32_t> lineages;
  static std::array<uint32_t,17> header;
  owners.clear();nodes.clear();lasers.clear();lineages.clear();
  const auto* focus=animationScene.find(playerFocusAnimation);
  std::unordered_set<uint32_t> seen;
  const auto owner=[&](uint32_t id,float x,float y,float z,uint32_t revision,uint32_t kind){
    if(seen.insert(id).second)owners.push_back({id,revision,kind,{x,y,z}});
  };
  owner(0xffff0000u,px/128.f+224,py/128.f+16,0,presentation::revision(0xffff0000u)*8u+uint32_t(playerState),0);
  for(const auto& pointer:enemies){const auto& e=*pointer;if(e.active)
    owner(e.id,e.x+224,e.y+16,e.z,presentation::revision(e.id)+(e.isUfo?presentation::revision(0xfffd0001u)+presentation::revision(0xfffd0003u):0),1);}
  for(const auto& b:bullets)if(b.active)
    owner(0x10000000u+b.entityId,b.x+224,b.y+16,0,presentation::revision(0x10000000u+b.entityId)*16u+(b.friendly?uint32_t(b.weapon.phase):uint32_t(b.enemy.phase)),2);
  for(const auto& item:itemManager.entities)if(item.active())
    owner(0x20000000u+item.id,item.position.x+224,item.position.y+16,0,(presentation::revision(0x20000000u+item.id)+presentation::revision(0x30000000u+item.id)+presentation::revision(0x40000000u+item.id))*16u+uint32_t(item.state),3);
  for(const auto& visual:playerBomb.visuals)if(visual.active){
    const bool sweep=visual.bank==1&&visual.script==24;
    owner(0x50000000u+visual.id,sweep?0:visual.x+224,sweep?0:visual.y+16,0,uint32_t(visual.interrupt),4);
  }
  for(const auto& [id,pointer]:animationScene.nodes()){
    const auto& n=*pointer;const auto& s=n.vm.state();
    if(n.alive&&!n.parent&&n.key&&((n.owner>>28)==5||(n.owner>>28)==10))owner(n.owner,s.engineX+n.viewportX,s.engineY+n.viewportY,s.engineZ,n.vm.presentationRevision(),4);
  }
  std::unordered_map<uint64_t,uint32_t> attached;
  for(const auto& e:enemies)if(e->active&&e->extraBombShieldAnimation)attached.emplace(e->extraBombShieldAnimation,e->id);
  for(const auto membership:{anm_logic::Membership::Primary,anm_logic::Membership::Secondary,anm_logic::Membership::Embedded})
    for(const auto* pointer:animationScene.chain(membership)){
      const auto& n=*pointer;const auto& s=n.vm.state();if(!n.alive)continue;
      uint32_t motionOwner=n.owner,meshKind=0;
      if(n.owner==0xffff0020u)motionOwner=focus&&focus->id==n.id?0xffff0000u:n.owner;
      if(n.owner==0xffff0020u&&focus){const auto* a=&n;for(int depth=0;a&&depth<16;++depth){
        if(a->id==focus->id){motionOwner=0xffff0000u;break;}const auto at=animationScene.nodes().find(a->parent);a=at==animationScene.nodes().end()?nullptr:at->second.get();}}
      if(n.owner==0xfffd0001u||n.owner==0xfffd0003u)motionOwner=ufoManager.enemyId;
      if(n.owner==0xfffe0200u)if(const auto* boss=primaryBoss())motionOwner=boss->id;
      if(const auto at=attached.find(n.key);at!=attached.end())motionOwner=at->second;
      if(stageBackgroundEffect.owns(n.key))meshKind=1;
      else if(const auto* effect=ufoEffects.state(n.id))meshKind=effect->kind==UfoEffectKind::Trail?2:3;
      else if((n.owner>>28)==0xbu)meshKind=4;
      else if(s.mode==9)meshKind=5;else if(s.mode==13)meshKind=6;
      else for(const auto& leaf:playerBomb.leaves)if(n.key==0x600000000ull+leaf.id){meshKind=7;break;}
      const uint32_t offset=lineages.size();const auto* a=&n;
      for(int depth=0;a&&depth<=16;++depth){lineages.push_back(a->id);lineages.push_back(a->vm.presentationRevision());const auto at=animationScene.nodes().find(a->parent);a=at==animationScene.nodes().end()?nullptr:at->second.get();}
      nodes.push_back({n.id,n.owner,motionOwner,n.parent,{s.x+s.offsetX,s.y+s.offsetY,s.z+s.offsetZ},{s.engineX,s.engineY,s.engineZ},continuousFields(n),s.flags,
        (n.owner&0xff000000u)==0xc1000000u?int(n.owner-0xc1000000u):-1,meshKind,offset,uint32_t(lineages.size())-offset});
    }
  for(const auto* object:laserManager.ordered()){
    const auto& s=object->state;uint32_t followOwner=0,followRevision=0;
    if(s.kind==laser::Kind::Timed&&(s.parameters.flags&1))if(const auto* boss=primaryBoss()){followOwner=boss->id;followRevision=presentation::revision(boss->id);}
    lasers.push_back({0xb0000000u+s.serialId,presentation::revision(0xb0000000u+s.serialId),s.presentationTopology,uint32_t(s.kind),uint32_t(s.phase),uint32_t(s.cursor),uint32_t(s.nodes.size()),followOwner,followRevision,s.age.value});
  }
  header={2,presentation::epoch,uint32_t(frame),uint32_t(playerState),focus?focus->id:0,stageState.presentationRevision,
    uint32_t(reinterpret_cast<uintptr_t>(owners.data())),uint32_t(owners.size()),sizeof(Owner),uint32_t(reinterpret_cast<uintptr_t>(nodes.data())),uint32_t(nodes.size()),sizeof(Node),
    uint32_t(reinterpret_cast<uintptr_t>(lasers.data())),uint32_t(lasers.size()),sizeof(Laser),uint32_t(reinterpret_cast<uintptr_t>(lineages.data())),uint32_t(lineages.size())};
  return header;
}
} // namespace th12
