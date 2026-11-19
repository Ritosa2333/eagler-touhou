// Execute production launch functions, audio helpers and optional Asyncify queue.
// Native initialization/loop and Web Audio are controlled ports, not browser proof.
import assert from 'node:assert/strict';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';
const [shellArg,hostArg,asyncArg,outArg]=process.argv.slice(2);
assert(shellArg&&hostArg&&asyncArg&&outArg,'shell host async-runtime output required');
const shellPath=resolve(shellArg),source=await readFile(shellPath,'utf8');
const {resumeRuntimeAudio}=await import(pathToFileURL(resolve(hostArg)));
const {Th14AsyncRuntime}=await import(pathToFileURL(resolve(asyncArg)));
function section(start,end){const a=source.indexOf(start),b=source.indexOf(end,a);assert(a>=0&&b>a,start);return source.slice(a,b);}
const launch=section('async function launch(){','\nfunction replayPlayback()');
const foreground=section('async function resumeForegroundAudio(','\nasync function closeAudio()');
const mp=source.includes('async function initializeGame(){');
const initialize=mp?section('async function initializeGame(){','\nasync function presentRoom('):'';
const rows=[];
for(const queued of [false,true])for(const name of ['visible-running','visible-pending','visible-rejected','hidden-running','hidden-pending','hide-in-initialize','hide-in-open-room','hide-before-resume-settles']){
 const document={hidden:name.startsWith('hidden-')},calls=[];
 let pendingResolve,context={state:name.endsWith('running')?'running':'suspended',resume(){
  if(name==='visible-rejected')return Promise.reject(Error('controlled resume denial'));
  return new Promise(r=>{pendingResolve=()=>{context.state='running';r();};});
 }};
 let paused=false,running=false,initializations=0;
 const raw={
  th11_loop_start(){calls.push('start');running=true;paused=false;},
  sdl_loop_pause(on){calls.push(on?'pause':'resume');paused=!!on;},
  th11_phase:()=>0,
 };
 const Module={SDL3:{audioContext:context},FS:{analyzePath:()=>({exists:false})}};
 if(queued)Module.th14AsyncRuntime=new Th14AsyncRuntime(Module,{onError:e=>{throw e;}});
 let core=raw;
 const callCore=async name=>{assert.equal(name,'th11_initialize');++initializations;
  if(name==='th11_initialize'&&nameCase==='hide-in-initialize')document.hidden=true;return 1;};
 const nameCase=name;
 const make=new Function('Module','core','document','resumeRuntimeAudio','callCore','openRoom',`
  const game='th14',window={},canvas={focus(){}},performance={now:()=>0};
  const options={multiplayerPreflight:true,netplayMode:${mp&&name==='hide-in-open-room'?"'lan'":"undefined"}},roomRuntime=${mp?'true':'false'};
  let launched=false,roomPreview=false,stopping=false,replayViewer=null,first=false,lastPresented=0,lastHealth=0,lastFrame=0,frames=0,maxGap=0;
  const clearKeyboard=()=>{},apply=()=>{},$=()=>null,emit=()=>{},coreError=()=>'',save=async()=>{};
  const protocol='fixture',runtimeBuild={version:'fixture'};
  ${initialize}\n${foreground}\n${launch}
  return {launch};
 `);
 const api=make(Module,core,document,resumeRuntimeAudio,callCore,async()=>{if(name==='hide-in-open-room')document.hidden=true;});
 await api.launch();
 if(Module.th14AsyncRuntime)await Module.th14AsyncRuntime.tail;
 const beforeSettlePaused=paused;
 if(name==='hide-before-resume-settles')document.hidden=true;
 if(pendingResolve){pendingResolve();await Promise.resolve();await Promise.resolve();if(Module.th14AsyncRuntime)await Module.th14AsyncRuntime.tail;}
 await api.launch();
 if(Module.th14AsyncRuntime)await Module.th14AsyncRuntime.tail;
 const shouldPause=document.hidden||name==='visible-rejected';
 const pendingWasPaused=context.state==='running'&&pendingResolve?beforeSettlePaused:true;
 const passed=running&&paused===shouldPause&&pendingWasPaused&&initializations===1&&calls.filter(c=>c==='start').length===1;
 rows.push({name,queued,passed,paused,expectedPaused:shouldPause,beforeSettlePaused,calls});
}
const result={schema:'th14/launch-suspension/1',passed:rows.every(r=>r.passed),variant:mp?'mp':'normal',
 sourceSha256:createHash('sha256').update(source).digest('hex'),actualLaunchFunction:true,actualAudioHelper:true,
 actualAsyncQueue:true,controlledNativeLoopAndAudio:true,actualBrowserHiddenAccepted:false,completeGame:false,rows};
await writeFile(resolve(outArg),JSON.stringify(result,null,2)+'\n');
console.log(JSON.stringify({passed:result.passed,variant:result.variant,cases:rows.length,failed:rows.filter(r=>!r.passed)}));
process.exitCode=result.passed?0:1;
