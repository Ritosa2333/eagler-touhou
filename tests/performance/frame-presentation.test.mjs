import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
const home=path.dirname(fileURLToPath(import.meta.url));
const root=path.resolve(home,'../..');
const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8').replaceAll('\r\n','\n');
const {FrameStepper,WorldPresentationInterpolator,renderInterpolationPhase}=await import(pathToFileURL(path.join(root,'site/runtime/timing.mjs')).href);
function span(start,end){const first=source.indexOf(start);assert(first>=0,start);const last=source.indexOf(end,first);assert(last>first,end);return source.slice(first,last);}
const audioSource=span('function ordinaryWorldAudioState(){','function closeOrdinaryWorldAudio(){');
function audioHarness({entry=0,configured=true,begun=false,resources=true,owned=false,lightweight=true}={}){
  const calls={world:0,message:0,identity:0};
  const snapshot={normalTerminal:{configured:true,begun}};
  const module={UTF8ToString:v=>v,_th128_world_state(){calls.world++;return JSON.stringify(snapshot);}};
  if(lightweight)module._th128_world_message_state=()=>{calls.message++;return JSON.stringify({configured,entry,active:true});};
  const state={worldMode:true,module,replayPlayback:null,ordinaryWorldAudio:owned?{}:null,ordinaryWorldAudioLoading:null,
    endingResources:resources?{readAudioIdentity(value){calls.identity++;assert.deepEqual(value,snapshot);return {generation:9,frame:42};},createAudio(){}}:null};
  const read=new Function('state',audioSource+';return ordinaryWorldAudioState;')(state);
  return {state,calls,read};
}
test('ordinary battle audio does not serialize the World before the ending message entry',()=>{
  const h=audioHarness();assert.equal(h.read(),null);assert.equal(h.calls.world,0);
});
test('a terminal-stage battle also uses the small live message probe',()=>{
  const h=audioHarness({resources:true});h.state.worldSnapshot={normalTerminal:{begun:true}};
  assert.equal(h.read(),null);assert.equal(h.calls.world,0);assert.equal(h.calls.message,1);
});
test('unconfigured messages cannot begin an ordinary terminal owner',()=>{
  const h=audioHarness({configured:false});assert.equal(h.read(),null);assert.equal(h.calls.world,0);
});
test('entry one still checks the current World when the terminal is not begun',()=>{
  const h=audioHarness({entry:1});assert.equal(h.read(),null);assert.equal(h.calls.world,1);assert.equal(h.calls.identity,0);
});
test('begun terminal retains exact current identity validation',()=>{
  const h=audioHarness({entry:1,begun:true});assert.deepEqual(h.read(),{generation:9,frame:42});assert.equal(h.calls.world,1);assert.equal(h.calls.identity,1);
});
test('an existing terminal owner cannot be bypassed by a changed message entry',()=>{
  const h=audioHarness({entry:0,begun:true,owned:true});assert.deepEqual(h.read(),{generation:9,frame:42});assert.equal(h.calls.world,1);
});
test('missing strict ending resources still fail after native terminal entry',()=>{
  const h=audioHarness({entry:1,begun:true,resources:false});assert.throws(h.read,/strict audio consumer/);assert.equal(h.calls.world,1);
});
test('old runtimes without a small message export retain full validation',()=>{
  const h=audioHarness({lightweight:false,begun:true});assert.deepEqual(h.read(),{generation:9,frame:42});assert.equal(h.calls.world,1);
});
test('non-World and replay audio scopes do not inspect ordinary state',()=>{
  const h=audioHarness();h.state.worldMode=false;assert.equal(h.read(),null);h.state.worldMode=true;h.state.replayPlayback={};assert.equal(h.read(),null);assert.equal(h.calls.world,0);
});
const hfrSource=span('// BEGIN HIGH_FRAMERATE_PRESENTATION','// END HIGH_FRAMERATE_PRESENTATION');
const presentSource=span('async function present(timestamp){','\nrequestAnimationFrame(present);');
function presentationHarness(){
 const calls={states:0,draws:0,captures:0,samples:0,ticks:0,presentations:[],inputs:[]};
 const state={worldMode:true,shotMode:true,active:true,paused:false,handoff:false,endingHost:null,endingLoading:false,normalPostrun:null,
  stageSelection:{stage:{id:1}},background:{tick(){},geometry:()=>[]},frame:0,autoFlow:false,extraPresentation:null};
 state.module={UTF8ToString:v=>v,
  _th128_world_state(){calls.states++;return JSON.stringify({frame:state.frame,flow:{currentId:1}});},
  _th128_world_bullet_draws(){calls.draws++;return JSON.stringify([{id:4000000,drawPass:'player',x:state.frame*2,y:400,bank:1,entry:0,sprite:0}]);},
  _th128_world_bullet_draw_error:()=>''};
 class CountedHistory extends WorldPresentationInterpolator{
  capture(snapshot){calls.captures++;return super.capture(snapshot);}
  sample(alpha){calls.samples++;return super.sample(alpha);}
 }
 const menu={pauseOpen:false,fault:false},clock=new FrameStepper(),fps={reset(){},observe(){}};
 const drawSource='function draw(alpha){const raw=worldPresentationForDraw(),view=highFramerateWorldView(raw.world,raw.records,alpha);calls.presentations.push({frame:raw.world.frame,x:view.records[0].x,interpolated:view.interpolated});return true;}return {present,setHighFramerateMode};';
 const construct=new Function('state','menu','document','WorldPresentationInterpolator','clock','originalMenuClock','originalPresentationFPS',
 'renderInterpolationPhase','stageIntroDrawFailure','advance','advanceNormalPostrun','advanceOriginalEnding','maybeAutoHandoff',
 'syncGameplayFrameUI','requestAnimationFrame','performance','pause','status','error','calls',
 hfrSource+'\n'+presentSource+'\n'+drawSource);
 const tick=async()=>{calls.inputs.push(state.frame);calls.ticks++;state.frame++;};
 const h=construct(state,menu,{hidden:false},CountedHistory,clock,new FrameStepper(),fps,renderInterpolationPhase,()=>'',tick,tick,tick,()=>{},()=>{},()=>{},performance,()=>{},()=>{},e=>{throw e;},calls);
 return {calls,state,...h};
}
test('four catch-up logic ticks export only the final display and keep all four inputs',async()=>{
 const h=presentationHarness();h.setHighFramerateMode(true);await h.present(0);
 for(const k of ['states','draws','captures','samples'])h.calls[k]=0;h.calls.presentations=[];
 await h.present(1000/15);
 assert.equal(h.calls.ticks,4);assert.deepEqual(h.calls.inputs,[0,1,2,3]);
 assert.deepEqual(h.calls.presentations,[{frame:4,x:8,interpolated:false}]);
 assert.equal(h.calls.states,1);assert.equal(h.calls.draws,1);assert.equal(h.calls.captures,0);assert.equal(h.calls.samples,0);
});
test('high-refresh interpolation resumes after catch-up without stale frame history',async()=>{
 const h=presentationHarness();h.setHighFramerateMode(true);await h.present(0);await h.present(1000/15);
 await h.present(1000/15+1000/60);await h.present(1000/15+2000/60);await h.present(1000/15+2500/60);
 assert.equal(h.calls.ticks,6);
 const last=h.calls.presentations.at(-1);assert.equal(last.frame,6);assert.equal(last.interpolated,true);assert(Math.abs(last.x-11)<1e-6);
});
test('60 Hz fixed mode keeps its original tick and presentation counts',async()=>{
 const h=presentationHarness();await h.present(0);
 for(let tick=1;tick<=60;tick++)await h.present(tick*1000/60);
 assert.equal(h.calls.ticks,60);assert.equal(h.calls.presentations.length,60);assert.equal(h.calls.captures,0);assert.equal(h.calls.states,60);
});
