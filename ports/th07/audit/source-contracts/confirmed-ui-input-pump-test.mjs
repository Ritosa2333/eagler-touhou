import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
const root=resolve(import.meta.dirname,'../..'),game=root.endsWith('th07')?'th07':'th06';
const source=readFileSync(resolve(root,`src/netplay/Th${game.slice(2)}LanStageProbe.cpp`),'utf8').replaceAll('\r\n','\n');
const common=resolve(root,'third_party/eagler-common');
const out=resolve(root,'artifacts/confirmed-ui-input');mkdirSync(out,{recursive:true});
const pump=game==='th06'?source.split('// BEGIN CONFIRMED UI INPUT PUMP')[1].split('// END CONFIRMED UI INPUT PUMP')[0]:source.slice(source.indexOf('FrameInput ContinuePhysicalInput('),source.indexOf('bool SendLocalFrame(std::uint32_t frame)\n{'));
if(!pump.includes(game==='th06'?'PumpConfirmedUiInput':'PumpBufferedLockstepInput'))throw Error('Missing production pump');
const test=String.raw`
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/SnapshotPolicy.hpp>
#include <cassert>
#include <deque>
#include <vector>
#include <cstdio>
using namespace Netplay;
constexpr unsigned TH_BUTTON_DIRECTION=240,TH_BUTTON_FOCUS=4,TH_BUTTON_SHOOT=1,TH_BUTTON_SKIP=256;
RollbackCore g_Core;
std::uint32_t g_SimFrame=0,g_DriverTicks=0,g_TestFrames=10000;
std::uint32_t g_UiCaptureFrame=0,g_InputCaptureFrame=0,g_InitialFrameLagRemaining=0,g_InitialFrameLag=0;
std::uint64_t g_NextUiCaptureNs=0,g_NextInputCaptureNs=0,now=1;
FrameInput g_LastUiCapture,g_LastPhysicalCapture,hardware;
bool g_HaveLastUiCapture=false,g_HaveLastPhysicalCapture=false,g_IncrementalReconcile=false,g_ReconcileActive=false;
bool sharedUi=true;
unsigned hardwareSamples=0;
std::vector<unsigned> sampleCounts(2000),sentCounts(2000);
struct Pending{unsigned frame;std::uint64_t due;FrameInput input;};std::deque<Pending> pending;
std::uint64_t delay=0;
std::uint64_t SDL_GetTicksNS(){return now;}
bool SharedUiNeedsConfirmedInputs(){return sharedUi;}
bool UsePhysicalInput(){return true;}
bool ProbeMode(){return false;}
FrameInput CaptureLocalInput(unsigned frame){++hardwareSamples;++sampleCounts[frame];return hardware;}
bool SendScheduledLocalFrame(unsigned frame){bool present=false;const auto input=g_Core.LocalInput(frame,&present);assert(present);if(!sentCounts[frame]++)pending.push_back({frame,now+delay,input});return true;}
bool SendLocalSample(unsigned frame,const FrameInput& input){return g_Core.ScheduleLocalInput(frame,input)&&SendScheduledLocalFrame(frame);}
bool SendLocalFrame(unsigned frame){return SendLocalSample(frame,CaptureLocalInput(frame));}
`+pump+String.raw`
bool pump(){return `+(game==='th06'?'PumpConfirmedUiInput()':'PumpBufferedLockstepInput()')+String.raw`;}
void reset(){CoreConfig config;config.maxRollbackFrames=120;assert(g_Core.Reset(config));g_SimFrame=g_DriverTicks=0;g_UiCaptureFrame=g_InputCaptureFrame=0;g_NextUiCaptureNs=g_NextInputCaptureNs=0;g_HaveLastUiCapture=g_HaveLastPhysicalCapture=false;now=1;hardware={};pending.clear();sampleCounts.assign(2000,0);sentCounts.assign(2000,0);sharedUi=true;hardwareSamples=0;}
void advance(){while(g_Core.ConfirmedThroughAllRemotes()!=INVALID_FRAME&&g_Core.ConfirmedThroughAllRemotes()>=g_SimFrame){auto decision=g_Core.PrepareFrame(g_SimFrame);assert(decision.canAdvance&&!decision.predictedMask);assert(g_Core.MarkSimulated(g_SimFrame,decision));++g_SimFrame;}}
int main(){
  // Real RollbackCore: every menu frame still needs confirmed remote input.
  for(unsigned latency:{2u,6u,12u}){reset();delay=latency*16666667ull;
    for(unsigned tick=0;tick<600;++tick){now=1+tick*16666667ull;++g_DriverTicks;assert(pump());while(!pending.empty()&&pending.front().due<=now){auto packet=pending.front();pending.pop_front();assert(g_Core.SubmitRemoteInput(1,packet.frame,packet.input)==RemoteInputResult::Accepted);}advance();}
    assert(g_SimFrame>=580);for(auto n:sampleCounts)assert(n<=1);
    std::printf("Confirmed UI with %u-frame input transit: %u ticks in 10s\n",latency,g_SimFrame);
  }
  // Late callbacks preserve held input, consume movement and one-shot edges once.
  reset();delay=0;assert(pump());hardware.buttons=8|2|1;hardware.touchBomb=true;hardware.analogMode=AnalogMode::DirectTouchDelta;hardware.x=3;hardware.y=-2;
  now=1+6*16666667ull;assert(pump());const unsigned end=`+(game==='th06'?'g_UiCaptureFrame':'g_InputCaptureFrame')+String.raw`;
  unsigned bombEdges=0,menuEdges=0;float dx=0,dy=0;for(unsigned f=1;f<end;++f){bool present=false;auto input=g_Core.LocalInput(f,&present);assert(present);bombEdges+=input.touchBomb;menuEdges+=bool(input.buttons&8);dx+=input.x;dy+=input.y;assert(g_Core.LocalFrameForCapture(f)==f);}
  assert(hardwareSamples==2&&bombEdges==1&&menuEdges==1&&dx==3&&dy==-2);
  // UI entered after a rollback: never resample the existing gameplay slot.
  reset();assert(SendLocalSample(0,FrameInput(1)));hardware.buttons=8;assert(pump());assert(g_Core.LocalInput(0).buttons==1);assert(sampleCounts[0]==0);
  // A silent peer cannot make the physical pipeline exceed bounded history.
  reset();for(unsigned t=0;t<1000;++t){now=1+t*16666667ull;++g_DriverTicks;assert(pump());}assert(`+(game==='th06'?'g_UiCaptureFrame':'g_InputCaptureFrame')+String.raw`<=65);assert(sentCounts[0]>1);
  std::puts("Confirmed UI cadence / zero scheduling delay / once-only input / bounded history: PASS");
}
`;
const cpp=resolve(out,'check.cpp'),exe=resolve(out,'check.exe');writeFileSync(cpp,test);
const compiler=process.env.TH_TEST_CXX??'C:/msys64/ucrt64/bin/g++.exe';
execFileSync(compiler,['-std=c++17','-O2','-I'+resolve(common,'include'),'-I'+resolve(common,'tests/fixtures/include'),cpp,resolve(common,'src/netplay/NetplayCore.cpp'),resolve(common,'src/netplay/NetplayProtocol.cpp'),'-o',exe],{stdio:'inherit',windowsHide:true});
execFileSync(exe,[],{stdio:'inherit',windowsHide:true,env:{...process.env,PATH:resolve(compiler,'..')+';'+process.env.PATH}});
