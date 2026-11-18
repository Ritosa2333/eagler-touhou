#include "SpellVisuals.hpp"
#include <algorithm>
#include <bit>
#include <limits>
#include <string>
namespace th128::spell {
namespace {struct Guard {bool& active;explicit Guard(bool& value):active(value){active=true;}~Guard(){active=false;}};}
bool Visuals::load(int bank,std::span<const std::uint8_t> bytes,std::string& error){
 error.clear();if(executing_||!scene_.nodes().empty()){error="spell resource replacement requires a fresh host";return false;}
 if(!scene_.registry.loadBank(bank,bytes.data(),bytes.size(),error))return false;
 scene_.runtime.instructionBudget=4096;scene_.runtime.cameraPosition.fill(std::numeric_limits<float>::quiet_NaN());scene_.runtime.cameraEyeOffset.fill(std::numeric_limits<float>::quiet_NaN());
 scene_.runtime.unsupported=[this](int bank,int script,int opcode,const char* reason){if(failure_.empty())failure_="spell ANM bank="+std::to_string(bank)+" script="+std::to_string(script)+" opcode="+std::to_string(opcode)+": "+reason;};return true;
}
bool Visuals::checked(std::string& error){
 if(!failure_.empty()){error=failure_;return false;}
 for(const auto& [identifier,node]:scene_.nodes()){
  if(node->vm.faulted()||node->detached||node->customGeometry){failure_="unsupported spell ownership/geometry callback";error=failure_;return false;}
  const auto& state=node->vm.state();if(state.ended||state.deleteRequested)node->alive=false;
 }scene_.collect();return true;
}
bool Visuals::start(std::array<float,3> position,std::string& error,std::array<bool,3> medals,int stageId,int spellId){
 error.clear();if(executing_||!failure_.empty()){error="unavailable spell visual start";return false;}Guard guard(executing_);
 for(int key=1;key<=7;++key)scene_.remove(key);scene_.collect();position_=position;
 // The original spell start selects the Extra boss's own background and cut-in (40e1cf / 40e2d9).
 const bool extra=stageId==16;
 const bool jointStage=stageId==3||stageId==5||stageId==8||stageId==10||stageId==13||stageId==15;
 if(jointStage&&(spellId<0||spellId>=240)){error="joint stage spell lacks the original cut-in selector";return false;}
 // Original 40e216 stage map and 40e270 card-id dispatch in the shared spell constructor.
 const auto in=[spellId](int first){return spellId>=first&&spellId<first+4;};
 const int cutIn=extra?17:jointStage?(in(20)||in(216)?36:in(100)||in(180)?37:in(56)||in(136)?38:39):11;
 for(const auto request:std::array<std::array<int,3>,7>{{{1,extra?3:12,extra?18:0},{2,11,1},{3,6,74},{4,11,2},{5,0,158},{6,0,168},{7,3,cutIn}}}){
  const bool follow=request[0]==5;
  if(!scene_.bind(request[0],request[0],request[1],request[2],anm::Membership::Primary,follow?320+position[0]:0,follow?16+position[1]:0,{},false,nullptr,0,0,23,false,follow?position[2]:0)){error="original spell animation constructor rejected";return false;}
 }
 // Original 40deaa..40dfcd: CD+132/128/136 select the three stored medals.
 const auto* root=scene_.find(4);
 if(!root){error="original medal root is unavailable";return false;}
 std::array<unsigned,3> slots{};
 for(const auto* child:scene_.children(root->id)){
  const int script=child->vm.state().script;
  if(script<4||script>6)continue;
  const auto index=std::size_t(script-4);++slots[index];
  scene_.nodes().at(child->id)->vm.control().sprite=medals[index]?309+int(index):308;
 }
 if(slots!=std::array<unsigned,3>{1,1,1}){error="original medal children are incomplete or ambiguous";return false;}
 return checked(error);
}
bool Visuals::retireFollowEffect(std::string& error){
 error.clear();if(executing_||!failure_.empty()){error="unavailable original443 follow-effect retirement";return false;}Guard guard(executing_);
 scene_.remove(5);return checked(error);
}
bool Visuals::finish(std::string& error){
 error.clear();if(executing_||!failure_.empty()){error="unavailable spell visual end";return false;}Guard guard(executing_);
 scene_.remove(1);scene_.remove(5);scene_.remove(6);scene_.remove(7);for(int key=2;key<=4;++key)scene_.interrupt(key,1);return checked(error);
}
bool Visuals::levelUp(int level,std::array<float,3> player,std::string& error){
 error.clear();if(executing_||!failure_.empty()||level<0||level>15){error="invalid original shot level-up context";return false;}Guard guard(executing_);
 scene_.remove(100);scene_.collect();
 if(!scene_.bind(100,100,7,26+level*3,anm::Membership::Primary,0,0,{},false,nullptr,0,0,23)){error="native persistent shot level notice rejected";return false;}
 const auto identity=1000u+unsigned(level);scene_.remove(identity);scene_.collect();
 if(!scene_.bind(identity,identity,7,24+level*3,anm::Membership::Primary,320+player[0],16+player[1],{},false,nullptr,0,0,23,false,player[2])){error="native shot level effect rejected";return false;}
 return checked(error);
}
bool Visuals::tick(std::array<float,3> position,bool active,std::string& error){
 error.clear();if(executing_||!failure_.empty()){error=failure_.empty()?"spell visual tick reentered":failure_;return false;}Guard guard(executing_);
 if(active){for(std::size_t index=0;index<3;++index){const float difference=float(double(position[index])-position_[index]),delta=float(double(difference)*.05000000074505806);position_[index]=float(double(position_[index])+delta);}
  if(auto* root=scene_.find(5)){auto& state=root->vm.control();state.engineX=320+position_[0];state.engineY=16+position_[1];state.engineZ=position_[2];}}
 scene_.tickPrimary();scene_.tickSecondary();return checked(error);
}
bool Visuals::pose(const anm::Node& node,Visual& output,unsigned depth) const {
 if(depth>32)return false;const auto& state=node.vm.state();output.id=node.id;output.state=state;
 output.x=state.x+state.engineX+state.offsetX;output.y=state.y+state.engineY+state.offsetY;output.z=state.z+state.engineZ+state.offsetZ;
 if(node.parent){const auto found=scene_.nodes().find(node.parent);Visual parent;if(found==scene_.nodes().end()||!pose(*found->second,parent,depth+1))return false;output.x+=parent.x;output.y+=parent.y;output.z+=parent.z;output.state.sx*=parent.state.sx;output.state.sy*=parent.state.sy;if(state.mode==1)output.state.rotation+=parent.state.rotation;}
 const auto bank=scene_.registry.bank(state.spriteBank);if(bank&&state.sprite>=0&&std::size_t(state.sprite)<bank->sprites.size())output.sprite=bank->sprites[std::size_t(state.sprite)];return true;
}
std::vector<Visual> Visuals::snapshots() const {std::vector<Visual> result;for(const auto* node:scene_.chain(anm::Membership::Primary)){Visual output;if(pose(*node,output))result.push_back(output);}return result;}
std::optional<Visual> Visuals::titlePose() const {for(const auto& node:snapshots())if(node.state.bank==6&&node.state.script==74)return node;return std::nullopt;}
std::vector<Visual> Visuals::bonusGlyphs(std::uint32_t flags,std::uint32_t value) const {
 std::vector<Visual> result;if(!(flags&1u))return result;
 const auto bank=scene_.registry.bank(11);if(!bank||bank->sprites.size()<=307)return result;
 const auto nodes=snapshots();
 const auto label=std::find_if(nodes.begin(),nodes.end(),[](const auto& node){return node.state.bank==11&&node.state.script==3;});
 if(label==nodes.end()||!label->state.visible)return result;
 const auto alpha=label->state.primary&0xff000000u;if(!alpha)return result;
 const auto color=alpha|((flags&2u)&&(flags&0x100u)?0x00ff0000u:0x00ffffffu);
 const auto append=[&](int sprite,float x){
  Visual glyph;glyph.id=unsigned(result.size());glyph.state.bank=glyph.state.spriteBank=11;
  glyph.state.sprite=sprite;glyph.state.script=-1;glyph.state.layer=0;glyph.state.mode=0;
  glyph.state.anchorX=glyph.state.anchorY=1;glyph.state.primary=color;glyph.state.blend=0;glyph.state.flags2|=4u;
  glyph.sprite=bank->sprites[std::size_t(sprite)];glyph.x=x;glyph.y=61;result.push_back(glyph);
 };
 // Original 40dbf0 uses style2 and %8d at (365,61); '$' is sprite307.
 if(!(flags&2u)){append(307,387);return result;}
 auto text=std::to_string(std::bit_cast<std::int32_t>(value));
 if(text.size()<8)text.insert(0,8-text.size(),' ');
 float x=365;
 for(const unsigned char character:text){if(character!=' ')append(character=='-'?239:int(character)+179,x);x+=7;}
 return result;
}
}
