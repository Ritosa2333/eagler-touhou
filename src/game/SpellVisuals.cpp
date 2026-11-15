#include "SpellVisuals.hpp"
#include <limits>
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
bool Visuals::start(std::array<float,3> position,std::string& error){
 error.clear();if(executing_||!failure_.empty()){error="unavailable spell visual start";return false;}Guard guard(executing_);
 for(int key=1;key<=7;++key)scene_.remove(key);scene_.collect();position_=position;
 for(const auto request:std::array<std::array<int,3>,7>{{{1,12,0},{2,11,1},{3,6,74},{4,11,2},{5,0,158},{6,0,168},{7,3,11}}}){
  const bool follow=request[0]==5;
  if(!scene_.bind(request[0],request[0],request[1],request[2],anm::Membership::Primary,follow?320+position[0]:0,follow?16+position[1]:0,{},false,nullptr,0,0,23,false,follow?position[2]:0)){error="original spell animation constructor rejected";return false;}
 }return checked(error);
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
}
