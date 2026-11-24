#include "ecl/EclVm.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#if __has_include("practice/Thprac128Chapter.hpp")
#include "practice/Thprac128Chapter.hpp"
#define HAVE_TH128_CHAPTER 1
#else
#define HAVE_TH128_CHAPTER 0
#endif
using namespace th128::ecl;using namespace ecl_fixture;
static unsigned checks=0,chapters=0,branches=0,actualVmBranches=0,entryTrampolines=0;
void check(bool b,const std::string& name){require(b,name);++checks;}
Bytes readBytes(const std::string& file){std::ifstream s(file,std::ios::binary);require(s.good(),"actual ECL input missing");return Bytes(std::istreambuf_iterator<char>(s),{});}
bool same(const Instruction&a,const Instruction&b){return a.offset==b.offset&&a.time==b.time&&a.opcode==b.opcode&&a.size==b.size&&a.parameterMask==b.parameterMask&&a.rankMask==b.rankMask&&a.parameterCount==b.parameterCount&&a.stackOffset==b.stackOffset&&a.rawWords==b.rawWords;}
int main(int argc,char**argv){try{
 check(argc==2,"owned original fixture directory required");
#if !HAVE_TH128_CHAPTER
 check(false,"actual TH128 thprac chapter planner/Program consumer is missing");
#else
 const std::string home=argv[1];std::ifstream plans(home+"/plans.tsv");check(plans.good(),"independent pinned upstream plans");
 unsigned stage,chapter,count;std::string name;unsigned a,b,t,u,a2,b2,t2,u2;
 while(plans>>stage>>chapter>>name>>count>>a>>b>>t>>u>>a2>>b2>>t2>>u2){
  const auto raw=readBytes(home+"/"+name);Program original,prepared;std::string error;
  check(original.load(raw.data(),raw.size(),error),error);const auto old=original;
  th128::practice::ChapterPlan plan;
  check(th128::practice::chapterPlan(stage,chapter,plan,error),error);
  check(plan.stageId==stage&&plan.chapter==chapter&&plan.section==10000+stage*100+chapter&&plan.sourceName==name,"real TH128 stage/chapter/section mapping");
  check(plan.branches.size()==count,"independent upstream patch count");
  const std::vector<Program::PracticeBranch> expected=count==0?std::vector<Program::PracticeBranch>{}:count==1?std::vector<Program::PracticeBranch>{{a,b,int(t),int(u)}}:std::vector<Program::PracticeBranch>{{a,b,int(t),int(u)},{a2,b2,int(t2),int(u2)}};
  for(std::size_t n=0;n<expected.size();++n){const auto &x=expected[n],&y=plan.branches[n];check(x.sourceOffset==y.sourceOffset&&x.targetOffset==y.targetOffset&&x.eclTime==y.eclTime&&x.targetTime==y.targetTime,"independent upstream literal jump operands");}
  check(th128::practice::prepareChapter(original,name,raw,stage,chapter,prepared,error),error);
  check(prepared.resource().bytes==raw&&original.resource().bytes==raw,"original DAT resource bytes stay exact");
  check(prepared.subroutines().size()==original.subroutines().size(),"subroutine topology preserved");
  for(std::size_t s=0;s<original.subroutines().size();++s){
   const auto &before=original.subroutines()[s],&after=prepared.subroutines()[s];check(before.name==after.name&&before.fileOffset==after.fileOffset&&before.codeSize==after.codeSize&&before.instructions.size()==after.instructions.size(),"all original subroutine identities/bounds");
   for(std::size_t i=0;i<before.instructions.size();++i){const auto &oi=before.instructions[i],&pi=after.instructions[i];const auto absolute=before.fileOffset+16+oi.offset;
    const auto found=std::find_if(expected.begin(),expected.end(),[&](const auto& patch){return patch.sourceOffset==absolute;});
    if(found==expected.end())check(same(oi,pi),"every unpatched original instruction unchanged");
    else{check(pi.opcode==12&&pi.size==24&&pi.parameterMask==0&&pi.rankMask==255&&pi.parameterCount==2&&pi.stackOffset==0&&pi.time==found->eclTime&&pi.offset==oi.offset&&pi.rawWords==std::vector<std::uint32_t>{found->targetOffset-found->sourceOffset,unsigned(found->targetTime)},"exact real parsed jump instruction");++branches;}
    check(same(oi,old.subroutines()[s].instructions[i]),"prior snapshot remains original");
   }
  }
  if(!expected.empty()){
   const auto& first=expected.front();const Subroutine* sub=nullptr;
   for(const auto& s:prepared.subroutines())if(first.sourceOffset>=s.fileOffset+16&&first.sourceOffset<s.fileOffset+16+s.codeSize)sub=&s;
   check(sub!=nullptr,"original chapter first site belongs to a real subroutine");
   Program execution=prepared;Limits limit;limit.maxInstructionsPerTick=1;
   if(sub->fileOffset+16!=first.sourceOffset){
    // Component-only entry trampoline bypasses game initialization; not a whole game start proof.
    auto isolated=expected;isolated.push_back({sub->fileOffset+16,first.sourceOffset,0,first.eclTime});
    check(original.withPracticeBranches(raw,isolated,execution,error),error);
    limit.maxInstructionsPerTick=2;++entryTrampolines;
   }
   Vm vm(execution,limit);check(vm.start(sub->name),"actual original ECL Vm start");
   if(limit.maxInstructionsPerTick==1)for(int n=0;n<first.eclTime;n++)check(vm.runTick().status==VmStatus::Running,"original chapter jump waits actual eclTime");
   const auto tick=vm.runTick();const auto state=vm.threads();
   // Stop exactly after one real instruction; intentional budget fault prevents game/host side effects.
   check(tick.instructions==limit.maxInstructionsPerTick&&tick.status==VmStatus::Faulted&&vm.error().code==ErrorCode::InstructionBudget,"intentional one-instruction stop at real target");
   check(state.size()==1&&state[0].subroutine==sub->name&&state[0].offset==first.targetOffset-sub->fileOffset-16&&state[0].time==first.targetTime,"actual original Vm relative PC/time reaches upstream chapter destination");
   ++actualVmBranches;
  }
  Program kept=prepared;check(!th128::practice::prepareChapter(original,"foreign.ecl",raw,stage,chapter,kept,error),"foreign named resource rejected");check(kept.find(original.subroutines()[0].name)->instructions.size()==prepared.subroutines()[0].instructions.size(),"failed adapter preserves previous Program");
  ++chapters;
 }
 check(chapters==126&&branches==143&&actualVmBranches==110,"all126chapters/143ops/110actualfirstbranches covered");
 th128::practice::ChapterPlan sentinel;sentinel.stageId=987;std::string error;
 for(const auto [stageId,chapterId]:std::vector<std::pair<unsigned,unsigned>>{{0,1},{17,1},{1,0},{1,8},{16,11}}){check(!th128::practice::chapterPlan(stageId,chapterId,sentinel,error),"unknown stage/chapter refused");check(sentinel.stageId==987,"invalid planner atomic output");}
#endif
 std::cout<<"{\"passed\":true,\"checks\":"<<checks<<",\"chapterChoices\":"<<chapters<<",\"upstreamBranchOps\":"<<branches<<",\"actualVmFirstBranches\":"<<actualVmBranches<<",\"intentionalBudgetStop\":true,\"Worlds\":0}\n";return 0;
 }catch(const std::exception&e){std::cerr<<"FIRST_FAILURE "<<e.what()<<" checks="<<checks<<"\n";return 1;}}
