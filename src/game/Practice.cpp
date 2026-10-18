#include "Practice.hpp"
#include "GameSession.hpp"
#include "GameState.hpp"
#include "StageFlow.hpp"
#include "PracticeSections.hpp"
#include <limits>
#include <iterator>
#include <sstream>
#include <type_traits>

#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
#include "PracticeSites.hpp"
#include <eagler/thprac/PracticeSpeed.hpp>
#endif

namespace th12::practice {
namespace {
bool profileEnabled=false, live=false, playback=false, modified=false;
uint32_t toolFlags=0, generation=0;
Parameters config{};
std::string failure, stateBuffer;
std::vector<std::vector<u8>> pristineFiles;
struct Tracker {int misses=0,bombs=0;std::array<int,3> collected{};std::array<int,4>dropped{},summoned{};};
Tracker tracker;
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
eagler::thprac::PracticeSpeed speed;
uint32_t headerCrc32(const uint8_t* bytes,size_t count) {
  uint32_t value=0xffffffffu;
  for(size_t i=0;i<count;++i){value^=bytes[i];for(int bit=0;bit<8;++bit)value=(value>>1)^((value&1u)?0xedb88320u:0u);}
  return value^0xffffffffu;
}

// An owned, bounded replacement for THPrac's Win32 pointer-table writer. It
// validates every affected original instruction before touching its copy.
class ECLHelper {
  std::vector<std::vector<u8>>& candidate;
  const std::vector<std::vector<u8>>& original;
  int stage, ordinal=0;
  size_t position=0;
  size_t index() const { return ordinal==0 ? 1 : size_t(ordinal); }
  bool guard(size_t offset, size_t bytes) {
    if (!valid || index()>=original.size()) return valid=false;
    const auto& source=original[index()];
    if (offset>source.size() || bytes>source.size()-offset) return valid=false;
    const size_t end=offset+bytes;
    for (size_t cursor=offset;cursor<end;) {
      const Site* covering=nullptr;
      for (const auto& site:sites) {
        if (site.stage==stage && site.ordinal==ordinal && cursor>=site.offset &&
            cursor-size_t(site.offset)<site.length) {covering=&site;break;}
      }
      if (!covering || covering->length<16 || covering->offset>source.size() ||
          covering->length>source.size()-covering->offset ||
          headerCrc32(source.data()+covering->offset,16)!=covering->headerCrc32)
        return valid=false;
      cursor=std::min(end,size_t(covering->offset)+covering->length);
    }
    return true;
  }
public:
  bool valid=true;
  ECLHelper(std::vector<std::vector<u8>>& files,const std::vector<std::vector<u8>>& source,int number)
    :candidate(files),original(source),stage(number){}
  void SetFile(unsigned int file) {
    if (file!=0 && !(stage==7 && file==2)) { valid=false; return; }
    ordinal=int(file);position=0;
    if (index()>=candidate.size()) valid=false;
  }
  void SetPos(size_t value) {position=value;}
  void CheckInstruction(size_t offset) {
    if (!guard(offset,16)) return;
    bool exact=false;
    for (const auto& site:sites)
      if (site.stage==stage && site.ordinal==ordinal && site.offset==offset) {exact=true;break;}
    if (!exact) valid=false;
  }
  template<class T>ECLHelper& operator<<(T value) {
    static_assert(std::is_arithmetic_v<T>);
    if (!guard(position,sizeof(value))) return *this;
    auto& file=candidate[index()];
    std::memcpy(file.data()+position,&value,sizeof(value));position+=sizeof(value);
    return *this;
  }
  template<class A,class B>ECLHelper& operator<<(std::pair<A,B> value) {
    SetPos(size_t(value.first));return *this<<value.second;
  }
};
using std::pair;
Parameters thPracParam;
#include "PracticePatches.inc"

bool patchProgram(const Parameters& params) {
  const auto& original=pristineFiles.empty()?program.files:pristineFiles;
  if (original.size()<(stageNumber==7?3u:2u)) {failure="Practice ECL files are incomplete";return false;}
  auto changed=original;
  if (params.mode && params.section) {
    ECLHelper writer(changed,original,stageNumber);
    thPracParam=params;
    if (params.section>=10000) THStageWarp(writer,stageNumber,params.section%100);
    else THPatch(writer,params.section);
    if (!writer.valid) {failure="TH12 retail ECL instruction-site contract mismatch; practice patch was not committed";return false;}
  }
  Program next;
  for (const auto& bytes:original)
    if (!next.add(bytes.data(),uint32_t(bytes.size()))) {failure="Original practice ECL structure is invalid; program retained";return false;}
  // Native THPrac rewrites executable PCs, not the ECL file's linear layout.
  // For example, a 24-byte jump over a 32-byte instruction leaves eight dead
  // bytes. Re-parsing those as another instruction rejects a valid native
  // patch. Bind routines from the strictly validated pristine tables, then
  // publish only the same-size, site-checked body writes into their owned data.
  for (size_t i=0;i<changed.size();++i) {
    if (changed[i].size()!=next.files[i].size()) {failure="Practice patch changed an ECL file's size; original program retained";return false;}
    std::copy(changed[i].begin(),changed[i].end(),next.files[i].begin());
  }
  if (pristineFiles.empty()) pristineFiles=original;
  program=std::move(next);
  return true;
}
#endif

bool fail(const char* text) {failure=text;return false;}
bool validParameters(const Parameters& p,int diff=-1) {
  if (p.mode<0||p.mode>1||p.stage<0||p.stage>6||p.score<0||p.score>9999999990LL||p.score%10||
      p.life<0||p.life>8||p.life_fragment<0||p.life_fragment>3||p.bomb<0||p.bomb>8||
      p.bomb_fragment<0||p.bomb_fragment>2||p.power<0||p.power>400||p.value<0||p.value>999990||
      p.value%10||p.graze<0||p.graze>999999||p.ufo_side<0||p.ufo_side>1||
      p.ventra_1<0||p.ventra_1>3||p.ventra_2<0||p.ventra_2>3||p.ventra_3<0||p.ventra_3>3)
    return fail("Practice parameters are outside original TH12 bounds");
  if ((!p.ventra_1&&(p.ventra_2||p.ventra_3))||(!p.ventra_2&&p.ventra_3))
    return fail("Ventra slots must be filled from left to right");
  if (!p.mode) return true;
  if (p.section==0) return p.phase==0 ? true : fail("Full stage has no spell phase");
  if (p.section>=10000&&p.section<20000) {
    const int chapter=(p.section-10000)%100,number=(p.section-10000)/100;
    if (number!=p.stage+1||chapter<1||chapter>chapterCounts[p.stage][0]+chapterCounts[p.stage][1]||p.phase)
      return fail("Chapter does not belong to the selected TH12 stage");
    return true;
  }
  if (p.section<=0||p.section>=int(std::size(sections))) return fail("Unknown TH12 section");
  const auto& section=sections[p.section];
  if (section.stage!=p.stage+1||p.phase<0||p.phase>=section.phaseCount||
      (diff>=0&&!(section.available&(1<<diff)))) return fail("Section or phase is unavailable for this stage/difficulty");
  if (p.dlg&&!section.dialogue) return fail("This TH12 section has no optional dialogue");
  return true;
}
}

bool enabled() {return profileEnabled;}
void setEnabled(bool value) {
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  profileEnabled=value;
#else
  profileEnabled=false;(void)value;
#endif
  if (!profileEnabled) reset(true);
}
void reset(bool clearTools) {
  config={};live=playback=modified=false;pristineFiles.clear();failure.clear();tracker={};++generation;
  if (clearTools) {
    toolFlags=0;
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
    speed={};
#endif
  }
}
void stageSelected() {live=false;playback=false;pristineFiles.clear();failure.clear();++generation;}
const Parameters& parameters(){return config;}
bool configure(const int32_t* w,int count) {
  failure.clear();
  if (!profileEnabled) return fail("THPrac profile is disabled");
  if (!w||count!=20||w[0]!=1||(w[5]!=0&&w[5]!=1)||(w[19]!=0&&w[19]!=1)||w[7]<0)
    return fail("Invalid TH12 practice ABI version or word count");
  Parameters p;
  p.mode=w[1];p.stage=w[2];p.section=w[3];p.phase=w[4];p.dlg=w[5]!=0;
  const uint64_t rawScore=uint64_t(uint32_t(w[6]))|(uint64_t(uint32_t(w[7]))<<32);
  if (rawScore>9999999990ULL) return fail("Practice score is outside original bounds");
  p.score=int64_t(rawScore);p.life=w[8];p.life_fragment=w[9];p.bomb=w[10];p.bomb_fragment=w[11];
  p.power=w[12];p.value=w[13];p.graze=w[14];p.ufo_side=w[15];
  p.ventra_1=w[16];p.ventra_2=w[17];p.ventra_3=w[18];
  if (!validParameters(p)) return false;
  config=p;playback=w[19]!=0;live=false;modified=false;++generation;
  return true;
}
int start(int c,int s,int d,int seed) {
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  failure.clear();
  if (!profileEnabled||!validParameters(config,d)||config.stage+1!=stageNumber||
      c<0||c>2||s<0||s>1||d<0||d>4||(stageNumber==7)!=(d==4)||seed<0||seed>65535)
    return fail("Practice session selection or resources are invalid"),0;
  if (!patchProgram(config)) return 0;
  live=true;
  resetTracker();
  if (!config.mode) return startFullGame(c,s,d,seed,true);
  SessionCarry initial;
  initial.resources=item_system::ResourceState::initial(d);
  initial.resources.scoreStored=int32_t(config.score/10);
  initial.resources.lives=config.life;
  initial.resources.lifeFragments=config.life_fragment?config.life_fragment+1:0;
  initial.resources.bombs=config.bomb;
  initial.resources.bombFragments=config.bomb_fragment*2;
  initial.resources.power=config.power;
  initial.resources.pointValueStored=config.value*100;
  initial.graze=config.graze;initial.nativeGlobals[3]=config.graze;
  initial.seed=uint16_t(seed);initial.displayRandom={0x1234,0};
  for (int color:{config.ventra_1,config.ventra_2,config.ventra_3}) if (color) initial.inventory.append(color);
  startGame(c,s,d,seed,nullptr,&initial);
  setFullGameMode(true);
  fullGameFlow.replay=playback;
  spellState.replayWallClock=playback;
  fullGameFlow.recordHeader=initial;fullGameFlow.recordHeaderReady=true;
  return 1;
#else
  (void)c;(void)s;(void)d;(void)seed;
  return fail("This ordinary build does not include THPrac"),0;
#endif
}
bool prepareReplay(int difficulty) {
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  failure.clear();
  if(!profileEnabled||!validParameters(config,difficulty)||config.stage+1!=stageNumber)
    return fail("Replay practice selection is invalid");
  if(!patchProgram(config))return false;
  playback=true;live=true;resetTracker();return true;
#else
  (void)difficulty;return fail("This build does not include THPrac");
#endif
}
void applyBirth(){if(profileEnabled&&live&&config.mode) itemManager.tokenSideCounter=config.ufo_side;}
void setReplay(bool value){playback=value;}
bool replay(){return playback;}
bool active(){return profileEnabled&&live;}
void setTools(uint32_t value){if(profileEnabled&&!playback){toolFlags=value&255u;if(toolFlags&191u)modified=true;}}
uint32_t tools(){return toolFlags;}
bool tool(Tool flag){return profileEnabled&&!playback&&(toolFlags&uint32_t(flag));}
bool assisted(){return modified||(profileEnabled&&live&&config.mode);}
void markAssisted(){if(profileEnabled&&!playback)modified=true;}
void resetTracker(){tracker={};}
void recordMiss(){if(profileEnabled)++tracker.misses;}
void recordBomb(){if(profileEnabled)++tracker.bombs;}
void recordUfoCollected(int type){if(profileEnabled&&type>=0&&type<3)++tracker.collected[type?3-type:0];}
void recordUfoDropped(int type){if(!profileEnabled||type<0||type>5)return;if(type>3)type=3;++tracker.dropped[type==1||type==2?3-type:type];}
void recordUfoSummoned(int type){if(profileEnabled&&type>=0&&type<4)++tracker.summoned[type==1||type==2?3-type:type];}
void beforeTick(int& held,int& pressed){
  if (!profileEnabled||playback) return;
  if (toolFlags&191u) modified=true;
  if (tool(AutoBomb)&&playerState==4&&playerStateTicks<8&&bombs>0) {held|=2;pressed|=2;}
}
void afterTick(){}
void setSpeed(int fps,int slow,int fast,bool debug){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  if(!profileEnabled)return;
  speed.fps=std::clamp(fps,60,6000);speed.fps_replay_slow=std::clamp(slow,1,60);
  speed.fps_replay_fast=std::clamp(fast/60,1,21)*60;speed.fps_debug_acc=debug?1:0;
#else
  (void)fps;(void)slow;(void)fast;(void)debug;
#endif
}
double interval(bool isReplay,bool fast,bool slow,bool debug){
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  if(profileEnabled)return speed.interval(isReplay,fast,slow,debug);
#else
  (void)isReplay;(void)fast;(void)slow;(void)debug;
#endif
  return 1./60.;
}
bool bossMusic(){return active()&&config.mode&&config.section>0&&config.section<10000&&!config.dlg&&sections[config.section].bgm;}
bool everlastingBgm(){return tool(EverlastingBgm)&&active()&&config.mode&&config.section;}
const std::string& lastError(){return failure;}
int snapshotSize(){return int(stateBuffer.size());}
const std::string& snapshot(){
  std::ostringstream out;
  out<<"{\"enabled\":"<<(profileEnabled?"true":"false")<<",\"active\":"<<(live?"true":"false")
     <<",\"replay\":"<<(playback?"true":"false")<<",\"assisted\":"<<(assisted()?"true":"false")
     <<",\"generation\":"<<generation<<",\"tools\":"<<toolFlags<<",\"params\":{\"mode\":"<<config.mode
     <<",\"stage\":"<<config.stage<<",\"section\":"<<config.section<<",\"phase\":"<<config.phase
     <<",\"dlg\":"<<(config.dlg?"true":"false")<<",\"score\":"<<config.score<<",\"life\":"<<config.life
     <<",\"life_fragment\":"<<config.life_fragment<<",\"bomb\":"<<config.bomb<<",\"bomb_fragment\":"<<config.bomb_fragment
     <<",\"power\":"<<config.power<<",\"value\":"<<config.value<<",\"graze\":"<<config.graze
     <<",\"ufo_side\":"<<config.ufo_side<<",\"ventra_1\":"<<config.ventra_1<<",\"ventra_2\":"<<config.ventra_2
     <<",\"ventra_3\":"<<config.ventra_3<<"},\"speed\":{";
#if defined(TH12_ENABLE_THPRAC) && TH12_ENABLE_THPRAC
  out<<"\"fps\":"<<speed.fps<<",\"slow\":"<<speed.fps_replay_slow<<",\"fast\":"<<speed.fps_replay_fast
     <<",\"debug\":"<<(speed.fps_debug_acc?"true":"false");
#else
  out<<"\"fps\":60,\"slow\":15,\"fast\":60,\"debug\":false";
#endif
  out<<"},\"tracker\":{\"misses\":"<<tracker.misses<<",\"bombs\":"<<tracker.bombs<<",\"collected\":[";
  for(size_t i=0;i<tracker.collected.size();++i){if(i)out<<',';out<<tracker.collected[i];}out<<"],\"dropped\":[";
  for(size_t i=0;i<tracker.dropped.size();++i){if(i)out<<',';out<<tracker.dropped[i];}out<<"],\"summoned\":[";
  for(size_t i=0;i<tracker.summoned.size();++i){if(i)out<<',';out<<tracker.summoned[i];}
  out<<"]},\"error\":\"";
  for(char value:failure){if(value=='\\'||value=='\"')out<<'\\';if(value=='\n')out<<"\\n";else out<<value;}
  out<<"\"}";stateBuffer=out.str();return stateBuffer;
}
}
