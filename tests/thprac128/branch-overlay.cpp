#include "ecl/EclVm.hpp"
#include "../ecl-fixture.hpp"
#include <concepts>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
using namespace th128::ecl;
using namespace ecl_fixture;
static unsigned checks=0;
void check(bool value,const std::string& text){require(value,text);++checks;}
Program parsed(const Bytes& b){Program p;std::string e;require(p.load(b.data(),b.size(),e),e);return p;}
std::uint32_t absolute(const Program& p,const char* name,std::uint32_t local){return p.find(name)->fileOffset+16+local;}
bool sameInstruction(const Instruction&a,const Instruction&b){return a.offset==b.offset&&a.time==b.time&&a.opcode==b.opcode&&a.size==b.size&&a.parameterMask==b.parameterMask&&a.rankMask==b.rankMask&&a.parameterCount==b.parameterCount&&a.stackOffset==b.stackOffset&&a.rawWords==b.rawWords;}
template<class P>concept OverlayAvailable=requires(const P& p,const Bytes& bytes,P& out,std::string& error){
 typename P::PracticeBranch;
 {p.withPracticeBranches(bytes,std::vector<typename P::PracticeBranch>{},out,error)}->std::same_as<bool>;
};
template<class P>void featureTests(){
 if constexpr(!OverlayAvailable<P>){check(false,"actual Program thprac branch overlay consumer is missing");}
 else{
  using B=typename P::PracticeBranch;
  for(const unsigned width:{20u,32u,40u}){
   const auto body=join({instruction(65001,std::vector<std::uint32_t>((width-16)/4,73)),instruction(65000,{111}),instruction(65000,{222}),instruction(10)});
   const Bytes raw=program({{"main",body}});P source=parsed(raw),old=source,out;std::string error;
   const auto at=absolute(source,"main",0),dest=absolute(source,"main",width+20);
   const auto original=source.find("main")->instructions;
   check(source.withPracticeBranches(raw,{B{at,dest,2,7}},out,error),error);
   check(error.empty(),"success clears stale diagnostics");
   check(out.resource().bytes==raw&&source.resource().bytes==raw,"raw original bytes preserved");
   check(out.sourceCount()==source.sourceCount()&&out.sourceBytes()==source.sourceBytes(),"source accounting unchanged");
   const auto& patch=out.find("main")->instructions[0];
   check(patch.offset==0&&patch.time==2&&patch.opcode==12&&patch.size==24&&patch.parameterMask==0&&patch.rankMask==255&&patch.parameterCount==2&&patch.stackOffset==0,"exact executed jump shape");
   check(patch.rawWords==std::vector<std::uint32_t>{width+20,7},"hand-derived relative target/time");
   check(out.find("main")->instructions.size()==original.size(),"instruction vector boundaries preserved");
   for(std::size_t i=1;i<original.size();++i)check(sameInstruction(original[i],out.find("main")->instructions[i]),"all other instructions unchanged");
   check(sameInstruction(source.find("main")->instructions[0],original[0])&&sameInstruction(old.find("main")->instructions[0],original[0]),"source and prior copy remain original");
   std::vector<int> visited;Vm vm(out);
   check(vm.registerHostOpcode(65000,[&](HostCall& call){std::int32_t n=0;check(call.readInteger(0,n),"host argument read");visited.push_back(n);check(call.time()==7,"actual Vm target clock assigned");return true;},error),error);
   check(vm.start("main"),"actual Vm start");
   check(vm.runTick().instructions==0&&visited.empty(),"eclTime2 waits tick0");
   check(vm.runTick().instructions==0&&visited.empty(),"eclTime2 waits tick1");
   check(vm.runTick().status==VmStatus::Finished&&visited==std::vector<int>{222},"actual Vm branches over old prefix to target once");
   unsigned oldFirst=0;Vm before(old);
   check(before.registerHostOpcode(65001,[&](HostCall&){++oldFirst;return true;},error),error);
   check(before.registerHostOpcode(65000,[](HostCall&){return true;},error),error);
   check(before.start("main")&&before.runTick().status==VmStatus::Finished&&oldFirst==1,"old Program still executes original prefix");
   auto changed=raw;changed.back()^=1;P kept=out;
   check(!source.withPracticeBranches(changed,{B{at,dest,0,0}},out,error)&&!error.empty(),"mismatched source bytes rejected");
   check(sameInstruction(out.find("main")->instructions[0],kept.find("main")->instructions[0]),"failure preserves existing destination");
   check(!out.withPracticeBranches(raw,{B{at,dest,0,0}},kept,error),"cannot stack another practice overlay");
   for(const auto& invalid:std::vector<std::vector<B>>{
      {{at+1,dest,0,0}},{{at,dest+1,0,0}},{{at,std::numeric_limits<std::uint32_t>::max(),0,0}},{{at,dest,-1,0}},{{at,dest,0,-1}},{{at,dest,0,0},{at,dest,0,0}},
      {{at,dest,0,0},{at+width+20,dest+1,0,0}}
    }){
     P target=old;check(!source.withPracticeBranches(raw,invalid,target,error),"invalid/duplicate/nonboundary/negative plan rejected");
     check(sameInstruction(target.find("main")->instructions[0],old.find("main")->instructions[0]),"invalid plan atomic destination");
    }
   P unloaded;check(!unloaded.withPracticeBranches(raw,{B{at,dest,0,0}},kept,error),"unloaded source rejected");
   check(!source.withPracticeBranches(Bytes{},{{at,dest,0,0}},kept,error),"empty expected source rejected");
   check(!source.withPracticeBranches(raw,std::vector<B>(129,B{at,dest,0,0}),kept,error),"bounded patch count");
   P alias=source;check(alias.withPracticeBranches(raw,{B{at,dest,0,0}},alias,error),"self output alias transaction");
   check(alias.find("main")->instructions[0].opcode==12&&source.find("main")->instructions[0].opcode==65001,"self alias leaves other shared source untouched");
   P noPatch;check(source.withPracticeBranches(raw,{},noPatch,error)&&sameInstruction(noPatch.find("main")->instructions[0],original[0]),"chapter1 identity plan retains original");
  }
  {
   const auto raw=program({{"main",join({instruction(65001,{0}),instruction(10)})},{"foreign",instruction(10)}});
   P source=parsed(raw),out;std::string error;
   check(!source.withPracticeBranches(raw,{B{absolute(source,"main",0),absolute(source,"foreign",0),0,0}},out,error),"cannot branch into another subroutine");
  }
  {
   auto words=stringWords("worker");
   const auto rootBytes=program({{"main",join({instruction(11,words,0,0,255,1),instruction(10)})}}, {},{"worker.ecl"});
   const auto workerBytes=program({{"worker",join({instruction(65001,{0}),instruction(65000,{1}),instruction(65000,{2}),instruction(10)})}});
   P root=parsed(rootBytes),worker=parsed(workerBytes),patchedWorker;std::string error;
   const auto start=absolute(worker,"worker",0),dest=absolute(worker,"worker",40);
   check(worker.withPracticeBranches(workerBytes,{B{start,dest,0,0}},patchedWorker,error),error);
   check(root.linkIncludes({{"worker.ecl",patchedWorker}},error),error);
   P refused;check(!root.withPracticeBranches(rootBytes,{},refused,error),"linked aggregate must not accept ambiguous file offsets");
   Vm vm(root);std::vector<int> visits;
   check(vm.registerHostOpcode(65000,[&](HostCall& call){int n=0;check(call.readInteger(0,n),"linked argument");visits.push_back(n);return true;},error),error);
   check(vm.start("main")&&vm.runTick().status==VmStatus::Finished&&visits==std::vector<int>{2},"actual include-linked Vm adopts patched worker only");
   check(worker.find("worker")->instructions[0].opcode==65001,"original provider remains immutable after link");
  }
 }
}
int main(){
 try{
  const auto b=program({{"main",instruction(10)}});Program ordinary=parsed(b);Vm vm(ordinary);
  check(vm.start("main")&&vm.runTick().status==VmStatus::Finished,"ordinary unchanged parser/Vm control");
  featureTests<Program>();
  std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"actualVm\":true,\"Worlds\":0,\"fullCore\":0}\n";return 0;
 }catch(const std::exception& e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<"\n";return 1;}
}
