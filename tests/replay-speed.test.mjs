import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';
import {PreviewInput,supportedReplay} from '../src/preview-input.mjs';
import {replayInputEnded} from '../src/replay.mjs';
import {createReplaySeeker} from '../src/replay-seek.mjs';

// Exercise the actual preview event handlers and loop without a browser/GPU.
// Only imports/bootstrap and presentation/audio adapters are replaced. The
// original held/pressed consumer and replay frame advance remain unmodified.
function preview(inputs,{batchTicks=240,yieldTask=async()=>{},endsAt=Infinity}={}){
  const elements=new Map(),windowEvents=new Map(),documentEvents=new Map(),ticks=[],reads=[];
  let now=0;
  const element=id=>{if(!elements.has(id))elements.set(id,{value:'1',checked:false,disabled:false,hidden:false,textContent:'',dataset:{},addEventListener(){},closest(){return null;},focus(){},querySelector:tag=>element(id+tag),replaceChildren(){},append(){}});return elements.get(id);};
  const heap=new Uint8Array(256),hud=new Float32Array(heap.buffer,0,15);hud[5]=hud[6]=2;
  const core={HEAPU8:heap,_th12_hud:()=>0,_th12_hud_size:()=>15,_th12_music_serial:()=>hud[0]>=3?1:0,_th12_music_track:()=>hud[0]>=3?1:0,
    _malloc:()=>64,_free(){},_th12_start_replay(){hud[0]=hud[13]=0;return 1;},
    _th12_tick(held,pressed){ticks.push([held,pressed]);hud[0]++;if(hud[0]>=endsAt)hud[13]=2;},_th12_start(){hud[0]=hud[13]=0;}};
  const context={PreviewInput,supportedReplay,replayInputEnded,createReplaySeeker,readCoreSoundEvents:()=>[],batchTicks,yieldTask,
    startReplayStage(){},console,performance:{now:()=>now},requestAnimationFrame(){},
    navigator:{getGamepads:()=>[]},window:{addEventListener:(type,listener)=>windowEvents.set(type,listener)},
    document:{hidden:false,getElementById:element,querySelectorAll:()=>[],addEventListener:(type,listener)=>documentEvents.set(type,listener)},
    Uint8Array,Uint32Array,Float32Array,Map,Set,Promise,setTimeout,clearTimeout,
    fixtureCore:core,fixtureReplay:{character:2,shot:1,difficulty:1,stages:[{number:1,seed:1234,frames:inputs.length,
      initial:{x:0,y:38400,power:100,lives:2,bombs:2,score:0,pointValue:0,lifeFragments:0,bombFragments:0,ufoColors:[0,0,0],rank:0,nativeGlobals:[0,0,0,0]}}],
      input(stage,frame){reads.push(frame);return inputs[frame]??null;}}};
  const source=fs.readFileSync(new URL('../site/main.mjs',import.meta.url),'utf8')
    .replace(/^import .*;\r?\n/gm,'').replace('initialize().catch(fail);','');
  vm.runInNewContext(source+`
    core=fixtureCore;renderer={render:()=>hud(),prepareStage:async()=>{}};
    stageLoader={load:async()=>true};store={sync:()=>Promise.resolve()};ready=running=true;
    activeReplay=fixtureReplay;audio.enabled=false;last=0;accumulator=0;
    replaySeeker=createReplaySeeker(core,{batchTicks,yieldTask,onReset:resetReplayMusic,onTick:(_,cursor)=>observeReplayMusic(cursor)});resetReplayMusic();
    globalThis.previewTest={key,clearKeyboard,pause,finish,start,loop,seekReplay,
      inspect:()=>({running,paused,replayFrame,generation,rate:replayRate(),accumulator,busy,seeking,logicalFrame:hud()[0],trackStartFrame:audio.trackStartFrame}),
      audio(plays){audio.enabled=true;audio.index=[{id:'road'},{id:'boss'}];audio.player={stop(){},async play(track,isCurrent,options){if(isCurrent())plays.push({track:track.id,...options});}};},
      manual:()=>{clear();activeReplay=null;running=true;paused=false;last=performance.now();accumulator=0;}};
  `,context,{filename:'preview-loop-unit.mjs'});
  const api=context.previewTest;
  return {api,ticks,reads,context,element,windowEvents,documentEvents,
    frame(ms=1000/60+.000001){now+=ms;api.loop(now,api.inspect().generation);},
    event(type,code,extra={}){const event={code,repeat:false,target:element('screen'),preventDefault(){},...extra};windowEvents.get(type)?.(event);}};
}
function inputSequence(length=128){
  let prior=0;return Array.from({length},(_,frame)=>{const held=(frame%7<4?1:0)|(frame%3?64:0)|(frame%5?8:0),input={held,pressed:held&~prior,released:prior&~held};prior=held;return input;});
}
test('Ctrl plays four times as many ticks and both rates consume the exact complete replay sequence',()=>{
  const inputs=inputSequence(),normal=preview(inputs),fast=preview(inputs);
  fast.event('keydown','ControlLeft');normal.frame();fast.frame();assert.equal(normal.ticks.length,1);assert.equal(fast.ticks.length,4);
  for(let frame=0;normal.api.inspect().running&&frame<200;frame++)normal.frame();
  for(let frame=0;fast.api.inspect().running&&frame<200;frame++)fast.frame();
  assert.deepEqual(normal.ticks,inputs.map(input=>[input.held,input.pressed]));assert.deepEqual(fast.ticks,normal.ticks);
  assert.deepEqual(fast.reads,Array.from({length:inputs.length+1},(_,frame)=>frame));
  assert.equal(fast.api.inspect().rate,1);assert.equal(fast.api.inspect().running,false);
});
test('two Ctrl keys release independently and releasing the last key removes accelerated backlog',()=>{
  const h=preview(inputSequence(1000));h.event('keydown','ControlLeft');h.event('keydown','ControlRight');h.frame(500);
  assert.equal(h.ticks.length,32);assert.ok(h.api.inspect().accumulator>100);
  h.event('keyup','ControlLeft');assert.equal(h.api.inspect().rate,4);
  h.event('keyup','ControlRight');assert.equal(h.api.inspect().rate,1);assert.equal(h.api.inspect().accumulator,0);
  h.frame();assert.equal(h.ticks.length,33);assert.deepEqual(h.reads,Array.from({length:33},(_,frame)=>frame));
});
test('pause and focus loss cancel acceleration; pressing Ctrl while paused never resumes playback',()=>{
  for(const cancel of [h=>h.api.pause(true),h=>h.windowEvents.get('blur')(),h=>{h.context.document.hidden=true;h.documentEvents.get('visibilitychange')();},h=>h.windowEvents.get('pagehide')()]){
    const h=preview(inputSequence());h.event('keydown','ControlLeft');h.frame();cancel(h);
    h.event('keydown','ControlRight');h.frame();assert.equal(h.ticks.length,4);assert.equal(h.api.inspect().paused,true);assert.equal(h.api.inspect().rate,1);
    h.api.pause(false);h.frame();assert.equal(h.ticks.length,5);assert.equal(h.api.inspect().rate,1);
  }
});
test('keyboard clear, stop and manual start remove replay acceleration while retaining native dialogue Ctrl',async()=>{
  const h=preview(inputSequence());h.api.key('ControlLeft',true,'host');h.frame();assert.equal(h.ticks.length,4);
  h.api.clearKeyboard();h.frame();assert.equal(h.ticks.length,5);h.api.key('ControlRight',true,'host');h.api.finish('game-over');assert.equal(h.api.inspect().rate,1);
  await h.api.start();h.event('keydown','ControlLeft');h.event('keydown','KeyZ');h.frame(0);
  assert.equal(h.api.inspect().rate,1);assert.deepEqual(h.ticks.at(-1),[513,513]);
});
test('Ctrl in form controls does not alter replay speed and normal play remains at60Hz',()=>{
  const h=preview(inputSequence());h.event('keydown','ControlLeft',{target:{closest:()=>({})}});h.frame();assert.equal(h.ticks.length,1);
  h.api.manual();h.event('keydown','ControlRight');h.event('keydown','KeyZ');h.frame();assert.deepEqual(h.ticks.at(-1),[513,513]);assert.equal(h.api.inspect().rate,1);
});

test('physical Ctrl changes replay rate without injecting dialogue bits into recorded inputs',()=>{
  const inputs=[{held:0,pressed:0,released:0},{held:512,pressed:512,released:0},
    {held:545,pressed:33,released:0},{held:1,pressed:0,released:544}];
  const h=preview(inputs);h.event('keydown','ControlLeft');h.frame();
  assert.deepEqual(h.ticks,inputs.map(row=>[row.held,row.pressed]));
  h.frame();assert.equal(h.api.inspect().running,false);
  assert.equal(h.ticks.length,inputs.length);
});

test('actual preview slider rewinds, retains the paused state and resumes the next original input',async()=>{
  const inputs=inputSequence(30),h=preview(inputs);
  for(let i=0;i<12;i++)h.frame();
  h.api.pause(true);const range=h.element('replay-position');range.onpointerdown();range.value='4';range.oninput();
  assert.equal(h.element('replay-time').textContent,'0:00.06 / 0:00.50');
  await range.onchange();
  const after=h.api.inspect();assert.equal(after.replayFrame,4);assert.equal(after.logicalFrame,4);assert.equal(after.paused,true);
  assert.equal(range.value,4);assert.equal(after.accumulator,0);assert.equal(after.busy,false);
  h.api.pause(false);h.frame();assert.deepEqual(h.ticks.at(-1),[inputs[4].held,inputs[4].pressed]);
});

test('single-frame button advances exactly one input and music seeks from the actual Boss change',async()=>{
  const h=preview(inputSequence(40)),plays=[];h.api.audio(plays);
  await h.api.seekReplay(8,false);const before=h.ticks.length;
  assert.equal(h.api.inspect().trackStartFrame,3);
  assert.equal(plays.at(-1).track,'boss');assert.equal(plays.at(-1).offsetSeconds,5/60);
  await h.element('replay-step').onclick();
  assert.equal(h.ticks.length,before+1);assert.equal(h.api.inspect().replayFrame,9);assert.equal(h.api.inspect().paused,true);
  await h.api.seekReplay(1,false);assert.equal(plays.at(-1).track,'road');assert.equal(plays.at(-1).offsetSeconds,1/60);
});

test('a later drag cancels the older seek and normal playback cannot tick while positioning',async()=>{
  let release;const waiting=new Promise(resolve=>release=resolve),h=preview(inputSequence(50),{batchTicks:2,yieldTask:()=>waiting});
  const old=h.api.seekReplay(20,true);assert.equal(h.api.inspect().seeking,true);
  const before=h.ticks.length;h.frame(500);assert.equal(h.ticks.length,before,'old RAF cannot consume inputs during reconstruction');
  await h.api.seekReplay(1,false);release();await old;
  assert.equal(h.api.inspect().replayFrame,1);assert.equal(h.api.inspect().logicalFrame,1);
  assert.equal(h.api.inspect().paused,true);assert.equal(h.api.inspect().seeking,false);
});

test('seeking stops at Game Over and the still-enabled slider can rewind and play again',async()=>{
  const h=preview(inputSequence(40),{endsAt:6});
  await h.api.seekReplay(20,true);assert.equal(h.api.inspect().replayFrame,6);assert.equal(h.api.inspect().running,false);
  assert.equal(h.element('replay-position').disabled,false);
  await h.api.seekReplay(2,true);assert.equal(h.api.inspect().replayFrame,2);assert.equal(h.api.inspect().running,true);
  assert.equal(h.api.inspect().paused,false);
});

test('releasing an unchanged slider resumes and focus loss during a seek keeps playback paused',async()=>{
  const h=preview(inputSequence(30));h.frame();const range=h.element('replay-position');
  range.onpointerdown();assert.equal(h.api.inspect().paused,true);range.onpointerup();assert.equal(h.api.inspect().paused,false);
  let release;const waiting=new Promise(resolve=>release=resolve),lost=preview(inputSequence(30),{batchTicks:2,yieldTask:()=>waiting});
  const pending=lost.api.seekReplay(8,true);lost.windowEvents.get('blur')();release();await pending;
  assert.equal(lost.api.inspect().replayFrame,8);assert.equal(lost.api.inspect().paused,true);
});
