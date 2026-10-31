#include "PracticeDamageSources.hpp"
#include "Practice.hpp"
#include "GameState.hpp"
#include <limits>
#include <unordered_set>

namespace th12::practice::damage {
namespace {
struct SavedStage {bool present=false;pw::Manager state;std::vector<int32_t> words;};
struct Owner {
  bool bound=false,replay=false,practice=false,reimuA=false;
  int currentStage=0,carryFrom=0,carryTo=0;
  uint32_t generation=0;
  pw::Manager carry;
  std::array<SavedStage,5> stages;
  EntryResult result=EntryResult::NotApplicable;
} owner;
std::string failure,stateBuffer;
const std::vector<int32_t> empty;
bool applicable(){return th12::practice::enabled()&&owner.bound&&owner.reimuA&&!owner.practice;}
bool hasActive(const pw::Manager& manager){for(const auto& source:manager.sources)if(source.flags&1u)return true;return false;}
int32_t bits(float value){int32_t result;std::memcpy(&result,&value,4);return result;}
float floating(int32_t value){float result;std::memcpy(&result,&value,4);return result;}
bool fail(const char* message){failure=message;return false;}

bool validSource(const pw::DamageSource& s){
  // These are the semantic pool's defined flags. Imported native 0x74-byte
  // arrays, pointers and unknown future layouts are deliberately unsupported.
  if(!s.id||!(s.flags&1u)||(s.flags&~3u)||s.damage<0||s.accumulated<0||s.limit<0||s.cadence<0||
    s.damage>std::numeric_limits<int>::max()-s.accumulated)return false;
  for(float value:{s.x,s.y,s.radius,s.growth,s.width,s.height,s.widthGrowth,s.heightGrowth,s.angle,s.remainingTime})
    if(!std::isfinite(value))return false;
  // Manager::tick converts its remaining-time float back to int. Keep this
  // conversion representable for imported replay data without changing values.
  if(double(s.remainingTime)<double(std::numeric_limits<int>::min())+16384.||
    double(s.remainingTime)>double(std::numeric_limits<int>::max())-16384.)return false;
  return true;
}
void append(std::vector<int32_t>& words,const pw::DamageSource& s){
  words.insert(words.end(),{int32_t(s.id),bits(s.x),bits(s.y),bits(s.radius),bits(s.growth),bits(s.width),bits(s.height),
    bits(s.widthGrowth),bits(s.heightGrowth),bits(s.angle),s.ticks,s.previousTicks,s.damage,s.accumulated,s.limit,s.cadence,
    int32_t(s.flags),bits(s.remainingTime)});
}
pw::DamageSource sourceAt(const int32_t* w){
  pw::DamageSource s;
  s.id=uint32_t(w[0]);s.x=floating(w[1]);s.y=floating(w[2]);s.radius=floating(w[3]);s.growth=floating(w[4]);
  s.width=floating(w[5]);s.height=floating(w[6]);s.widthGrowth=floating(w[7]);s.heightGrowth=floating(w[8]);s.angle=floating(w[9]);
  s.ticks=w[10];s.previousTicks=w[11];s.damage=w[12];s.accumulated=w[13];s.limit=w[14];s.cadence=w[15];
  s.flags=uint32_t(w[16]);s.remainingTime=floating(w[17]);return s;
}
bool capture(int stage,SavedStage& saved){
  SavedStage next;next.present=true;next.state.nextId=pilot().playerDamageSources.nextId;
  if(!next.state.nextId)return fail("Damage-source ID generation wrapped; snapshot unavailable");
  next.words={int32_t(Magic),Version,stage,int32_t(next.state.nextId),0,SourceWords};
  std::unordered_set<uint32_t> ids;int count=0;
  for(const auto& s:pilot().playerDamageSources.sources)if(s.flags&1u){
    if(!validSource(s)||s.id>=next.state.nextId||!ids.insert(s.id).second)return fail("Live ReimuA damage-source fields cannot be serialized safely");
    next.state.sources[size_t(count++)]=s;append(next.words,s);
  }
  next.words[4]=count;saved=std::move(next);return true;
}
}

void reset(){const auto generation=owner.generation+1;owner={};owner.generation=generation;failure.clear();}
bool begin(bool isReplay,bool isPractice,int c,int s){
  reset();
  if(c<0||c>2||s<0||s>1)return fail("Invalid damage-source run identity");
  owner.bound=true;owner.replay=isReplay;owner.practice=isPractice;owner.reimuA=c==0&&s==0;return true;
}
bool armTransition(int from,int to){
  owner.carryFrom=owner.carryTo=0;
  if(!applicable())return true;
  if(from<1||from>5||to!=from+1||owner.currentStage!=from)return fail("Damage-source carry is not an adjacent live stage transition");
  owner.carry=pilot().playerDamageSources;owner.carryFrom=from;owner.carryTo=to;return true;
}
EntryResult onBirth(int stage){
  failure.clear();owner.result=EntryResult::NotApplicable;
  if(!applicable())return owner.result;
  if(stage<1||stage>7){fail("Invalid damage-source birth stage");return owner.result=EntryResult::Invalid;}
  const bool carry=owner.carryTo==stage&&owner.carryFrom+1==stage&&owner.currentStage==owner.carryFrom;
  // A real stage transition retains actual source slots. It never substitutes
  // a saved replay snapshot for already active live sources (native source's
  // early-return guard). Dead parent Bomb/beam VMs are not resurrected.
  if(carry&&!hasActive(pilot().playerDamageSources))pilot().playerDamageSources=owner.carry;
  owner.carryFrom=owner.carryTo=0;owner.carry={};owner.currentStage=stage;
  if(stage==1||stage==7)return owner.result;
  auto& saved=owner.stages[size_t(stage-2)];
  if(owner.replay){
    if(hasActive(pilot().playerDamageSources))return owner.result=EntryResult::PreservedTransition;
    if(!saved.present){
      fail("This replay has no versioned portable ReimuA damage-source snapshot for the selected stage");
      return owner.result=EntryResult::Unavailable;
    }
    // Source TH12 copies only recorded active entries, compacted in order,
    // after establishing that no existing active sources own a transition.
    pilot().playerDamageSources=saved.state;return owner.result=EntryResult::Restored;
  }
  if(!capture(stage,saved))return owner.result=EntryResult::Invalid;
  return owner.result=EntryResult::Recorded;
}
bool load(const int32_t* w,int count){
  failure.clear();
  if(!applicable()||!owner.replay)return fail("Damage-source import requires the matching ReimuA replay owner");
  if(!w||count<HeaderWords||uint32_t(w[0])!=Magic||w[1]!=Version||w[2]<2||w[2]>6||
    w[4]<0||w[4]>Capacity||w[5]!=SourceWords||count!=HeaderWords+w[4]*SourceWords||!uint32_t(w[3]))
    return fail("Unknown or malformed portable ReimuA damage-source snapshot");
  SavedStage next;next.present=true;next.state.nextId=uint32_t(w[3]);std::unordered_set<uint32_t> ids;
  for(int index=0;index<w[4];++index){
    const auto s=sourceAt(w+HeaderWords+index*SourceWords);
    if(!validSource(s)||s.id>=next.state.nextId||!ids.insert(s.id).second)return fail("Invalid portable damage-source fields or duplicate identity");
    next.state.sources[size_t(index)]=s;
  }
  next.words.assign(w,w+count);owner.stages[size_t(w[2]-2)]=std::move(next);return true;
}
const std::vector<int32_t>& stageWords(int stage){return stage>=2&&stage<=6&&owner.stages[size_t(stage-2)].present?owner.stages[size_t(stage-2)].words:empty;}
const std::string& lastError(){return failure;}
int snapshotSize(){return int(stateBuffer.size());}
const std::string& snapshot(){
  std::ostringstream out;out<<"{\"schema\":\"th12-portable-damage-source/1\",\"applicable\":"<<(applicable()?"true":"false")
    <<",\"replay\":"<<(owner.replay?"true":"false")<<",\"stage\":"<<owner.currentStage<<",\"generation\":"<<owner.generation
    <<",\"entryResult\":"<<int(owner.result)<<",\"recordedStages\":[";
  bool comma=false;for(int stage=2;stage<=6;++stage)if(owner.stages[size_t(stage-2)].present){if(comma)out<<',';comma=true;out<<stage;}
  out<<"],\"nativeBlobImport\":false,\"error\":\"";
  for(char c:failure){if(c=='\\'||c=='\"')out<<'\\';if(c=='\n')out<<"\\n";else out<<c;}
  out<<"\"}";stateBuffer=out.str();return stateBuffer;
}
}
