import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {Renderer} from '../src/renderer.mjs';
import {createFrameRateMeter,fpsTextDescriptors} from '../src/fps-presentation.mjs';
import {rollbackDiagnostics} from '../src/multiplayer.mjs';
import {SpellWallClock} from '../src/spell-wall-clock.mjs';
import {spellResultDescriptors,decodeSpellRealTime} from '../src/spell-presentation.mjs';
import {interpolatePresentation} from '../src/presentation-interpolation.mjs';
import {managedCommandHandler} from '../src/managed-shell.mjs';

test('room binding preserves the local high refresh choice and accepts an explicit 60 Hz limit',async()=>{
  const settings=[],handler=managedCommandHandler({running:()=>false,resources:async()=>{},configure:async value=>settings.push(value)});
  await handler({command:'configure',music:'none',options:{limitPresentationTo60:false}});
  await handler({command:'configure',music:'none',options:{thpracEnabled:false}});
  assert.equal(settings.at(-1).options.limitPresentationTo60,false);
  await handler({command:'configure',music:'none',options:{limitPresentationTo60:true}});
  await handler({command:'configure',music:'none',options:{}});
  assert.equal(settings.at(-1).options.limitPresentationTo60,true);
  await assert.rejects(handler({command:'configure',music:'none',options:null}),/配置无效/);
});

test('FPS rendering uses measured display time even when the same native snapshot is reused',()=>{
  const calls=[],gl=new Proxy({}, {get:()=>()=>{}}),endpoint={frame:120,hud:[],stage:1,
    background:{camera:{fog:[0,0,0,0,0]},triangles:[]},screenShake:[0,0],poses:[
      {clip:0,drawPriority:69,nativeText:fpsTextDescriptors({currentFps:60})[0]}]};
  const renderer={gl,stageBackgrounds:new Map(),begin(){},flush(){},primitive(){},nativeText:t=>calls.push(t)};
  const meter=createFrameRateMeter();meter.reset(0);
  for(let i=1;i<=60;i++)meter.frame(i*1000/60);
  let now=1000;
  for(const [frames,interval]of [[10,100],[144,1000/144],[240,1000/240]]){
    meter.reset(now);
    for(let i=0;i<frames+2;i++){now+=interval;meter.frame(now);}
    Renderer.prototype.renderPresentation.call(renderer,null,endpoint,1,{currentFps:meter.value});
    assert(Math.abs(parseFloat(calls.at(-1).text)-frames)<=.11);
  }
  assert.equal(calls.length,3,'cached 60fps never draws under the live FPS');
  assert.match(endpoint.poses[0].nativeText.text,/60\./,'render keeps the native snapshot immutable');
});

test('multiplayer high refresh interpolates all seats and stops at pause/cap boundaries',()=>{
  const source=readFileSync(new URL('../site/game.mjs',import.meta.url),'utf8');
  const start=source.indexOf('function interpolateGamePresentation(){'),end=source.indexOf('\nfunction gamePresentationOptions',start);
  const context={multiplayer:{active:true,state:()=>({paused:false})},multiplayerResult:false,runtimeOptions:{limitPresentationTo60:false},
    presentationCadence:{fastFrames:4},presentationInterval:1000/144,logicalStep:1000/60,needsPresentationInterpolation:()=>true};
  runInNewContext(source.slice(start,end)+';globalThis.enabled=interpolateGamePresentation;',context);
  assert.equal(context.enabled(),true);context.runtimeOptions.limitPresentationTo60=true;assert.equal(context.enabled(),false);
  context.runtimeOptions.limitPresentationTo60=false;context.multiplayer.state=()=>({paused:true});assert.equal(context.enabled(),false);
  for(let seat=0;seat<3;seat++){
    const owner=0xffff0000+seat*256;
    const point=(frame,x)=>{const owners=[{id:owner,revision:1,kind:'player',position:[x,400,0]}];return {
      epoch:1,stage:1,frame,poses:[{owner,x,y:400,presentationKey:'seat-'+seat,drawPriority:24,clip:2,a:{mode:1}}],
      nativeMotion:{epoch:1,frame,owners,nodes:[],lasers:[],ownerMap:new Map(owners.map(o=>[o.id,o])),nodeMap:new Map(),laserMap:new Map()}};};
    const a=point(10,220),b=point(11,224);
    assert.deepEqual(interpolatePresentation(a,b,.5).poses[0].presentationOffset,[-2,0]);
  }
});

test('native rollback and resimulation counts reach diagnostics unchanged',()=>{
  const core={HEAPU8:new Uint8Array(128),_th12_mp_rollback_status:()=>16};
  new Uint32Array(core.HEAPU8.buffer,16,8).set([1,7,39,120,1,118,125,1]);
  const status=Array(24).fill(0);status[3]=125;status[4]=120;
  const result=rollbackDiagnostics(core,status);
  assert.deepEqual(result,{restores:7,resimulated:39,committed:120,correcting:true,request:118,frontier:125,missingInputs:4});
});

test('multiplayer actual spell time counts wall time, including pause, without mutating shared spell state',()=>{
  const clock=new SpellWallClock(),spell={flags:1,cardId:2,age:1,lastResult:{kind:0}};
  clock.observe(spell,1000);clock.observe({...spell,age:120},3000);
  clock.observe({...spell,age:120},7000); // paused native world, real time continues
  const result={flags:0,cardId:2,resultAnimationId:12,lastResult:{kind:1,cardId:2,displayElapsedTicks:120,realTimeEncoded:-1}};
  const before=structuredClone(result),local=clock.observe(result,9000);
  const [seconds,hundredths]=decodeSpellRealTime(local.lastResult.realTimeEncoded);
  assert.equal(seconds,7);assert(hundredths>=98);
  const poses=[{id:12,a:{name:'front',sprite:58,alpha:1}}];
  assert.equal(spellResultDescriptors(local,poses).length,4,'both defeat time and actual time have numbers');
  assert.deepEqual(clock.observe(result,30000),local,'result time remains frozen');assert.deepEqual(result,before);
  assert.equal(clock.observe(result,null),result,'Replay never invents a recorded wall time');
  clock.reset();assert.equal(clock.observe(result,50000),result);
});
