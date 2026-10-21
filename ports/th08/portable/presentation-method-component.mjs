import assert from 'node:assert/strict';
import {runNativeCpp} from '../../../launcher/tests/support/native-cpp.mjs';

function extractMethodBody(source, signature) {
  const start=source.indexOf(signature);
  assert.notEqual(start,-1,`Missing production method: ${signature}`);
  const open=source.indexOf('{',start+signature.length);
  assert.notEqual(open,-1,`Missing production method body: ${signature}`);
  let depth=0,state='code',escaped=false;
  for(let index=open;index<source.length;index++){
    const current=source[index],next=source[index+1];
    if(state==='line-comment'){if(current==='\n')state='code';continue;}
    if(state==='block-comment'){if(current==='*'&&next==='/'){state='code';index++;}continue;}
    if(state==='string'||state==='character'){
      if(escaped){escaped=false;continue;}
      if(current==='\\'){escaped=true;continue;}
      if((state==='string'&&current==='"')||(state==='character'&&current==="'"))state='code';
      continue;
    }
    if(current==='/'&&next==='/'){state='line-comment';index++;continue;}
    if(current==='/'&&next==='*'){state='block-comment';index++;continue;}
    if(current==='"'){state='string';continue;}
    if(current==="'"){state='character';continue;}
    if(current==='{')depth++;
    else if(current==='}'&&--depth===0)return source.slice(open,index+1);
  }
  assert.fail(`Unclosed production method body: ${signature}`);
}

export async function verifyBrowserRuntimePresent(source) {
  const body=extractMethodBody(source,'bool BrowserRuntime::present()');
  const mutatedBody=body.replace(/if\s*\(\s*presentation::render_only\s*\)/,'if(false)');
  assert.notEqual(mutatedBody,body,'The negative control must alter the extracted production branch');
  const code=`
#include <cassert>
#include <cstdint>
using u32=std::uint32_t;
namespace th08 {
namespace presentation { inline bool render_only=false; }
inline unsigned event_clock=0;
struct GraphicsDevice { bool swap=true; unsigned calls=0,last_event=0; bool present(u32){++calls;last_event=++event_clock;return swap;} };
inline GraphicsDevice gpu{};
inline GraphicsDevice& graphics_device(){return gpu;}
struct AppState { int active=0; };
struct Supervisor { AppState state; };
struct App { Supervisor supervisor; };
class RuntimeState {
public:
  u32 back=7,language_present_sequence=0;
  bool capture_failed=false,captured=false,fail_capture=false;
  unsigned flush_calls=0,finish_calls=0,finish_event=0;
  App app;
  void flush(){++flush_calls;}
  void finish_capture(){++finish_calls;finish_event=++event_clock;if(fail_capture)capture_failed=true;}
};
class BrowserRuntime:public RuntimeState {public:bool present();};
class MutatedBrowserRuntime:public RuntimeState {public:bool present();};
}
namespace eagler::diagnostics {
class LanguageTrace {
public:
  enum class Phase { Other, Runtime };
  bool active=true;
  unsigned phase_calls=0,scene_calls=0,present_calls=0;
  Phase phase=Phase::Other;
  int scene=-1;
  u32 sequence=0;
  bool enabled()const{return active;}
  void set_phase(Phase value){phase=value;++phase_calls;}
  void set_scene(int value){scene=value;++scene_calls;}
  void present(u32 value,int sceneValue){sequence=value;scene=sceneValue;++present_calls;}
};
inline LanguageTrace trace{};
inline LanguageTrace& language_trace(){return trace;}
}
namespace th08 {
bool BrowserRuntime::present()${body}
bool MutatedBrowserRuntime::present()${mutatedBody}
}
template<class Runtime>
bool renderOnlyObservation(Runtime& runtime){
  const auto finishBefore=runtime.finish_calls;
  const auto capturedBefore=runtime.captured;
  const auto traceBefore=eagler::diagnostics::trace.present_calls;
  const bool result=runtime.present();
  return result&&runtime.captured==capturedBefore&&runtime.finish_calls==finishBefore&&
    eagler::diagnostics::trace.present_calls==traceBefore+1&&
    eagler::diagnostics::trace.phase==eagler::diagnostics::LanguageTrace::Phase::Runtime&&
    eagler::diagnostics::trace.scene==runtime.app.supervisor.state.active&&
    eagler::diagnostics::trace.sequence==runtime.language_present_sequence;
}
void resetHarness(bool renderOnly,bool swap=true){
  th08::presentation::render_only=renderOnly;
  th08::event_clock=0;th08::gpu=th08::GraphicsDevice{};th08::gpu.swap=swap;
  eagler::diagnostics::trace=eagler::diagnostics::LanguageTrace{};
}
int main(){
  resetHarness(true);
  th08::BrowserRuntime renderOnly;renderOnly.captured=true;renderOnly.app.supervisor.state.active=17;
  assert(renderOnlyObservation(renderOnly));
  assert(renderOnly.flush_calls==1&&th08::gpu.calls==1&&renderOnly.finish_calls==0&&renderOnly.captured);
  assert(eagler::diagnostics::trace.phase_calls==1&&eagler::diagnostics::trace.scene_calls==1);

  resetHarness(true);
  th08::MutatedBrowserRuntime brokenGuard;brokenGuard.captured=true;brokenGuard.app.supervisor.state.active=17;
  assert(!renderOnlyObservation(brokenGuard));
  assert(brokenGuard.finish_calls==1&&!brokenGuard.captured);

  resetHarness(true,false);
  th08::BrowserRuntime failedSwap;failedSwap.captured=true;failedSwap.app.supervisor.state.active=19;
  assert(!failedSwap.present());
  assert(failedSwap.finish_calls==0&&failedSwap.captured&&eagler::diagnostics::trace.present_calls==0);

  resetHarness(true);
  th08::BrowserRuntime captureFailed;captureFailed.capture_failed=true;
  assert(!captureFailed.present());
  assert(captureFailed.finish_calls==0&&eagler::diagnostics::trace.present_calls==0);

  resetHarness(false);
  th08::BrowserRuntime normal;normal.captured=true;normal.app.supervisor.state.active=23;
  assert(normal.present());
  assert(!normal.captured&&normal.finish_calls==1&&th08::gpu.calls==1);
  assert(normal.finish_event>th08::gpu.last_event);
  assert(eagler::diagnostics::trace.phase==eagler::diagnostics::LanguageTrace::Phase::Runtime);
  assert(eagler::diagnostics::trace.scene==23&&eagler::diagnostics::trace.sequence==1);

  resetHarness(false,false);
  th08::BrowserRuntime normalSwapFailure;normalSwapFailure.captured=true;
  assert(!normalSwapFailure.present());
  assert(!normalSwapFailure.captured&&normalSwapFailure.finish_calls==1&&eagler::diagnostics::trace.present_calls==0);
  assert(normalSwapFailure.finish_event>th08::gpu.last_event);

  resetHarness(false);
  th08::BrowserRuntime normalCaptureFailure;normalCaptureFailure.fail_capture=true;
  assert(!normalCaptureFailure.present());
  assert(normalCaptureFailure.capture_failed&&normalCaptureFailure.finish_calls==1&&eagler::diagnostics::trace.present_calls==0);
  assert(normalCaptureFailure.finish_event>th08::gpu.last_event);
}
`;
  await runNativeCpp(code);
}