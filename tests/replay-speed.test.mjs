import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';
import {PreviewInput,supportedReplay} from '../src/preview-input.mjs';
import {replayInputEnded} from '../src/replay.mjs';

// Exercise the actual preview event handlers and loop without a browser/GPU.
// Only imports/bootstrap and presentation/audio adapters are replaced. The
// original held/pressed consumer and replay frame advance remain unmodified.
function preview(inputs){
  const elements=new Map(),windowEvents=new Map(),documentEvents=new Map(),ticks=[],reads=[];
  let now=0;
  const element=id=>{if(!elements.has(id))elements.set(id,{value:'1',checked:false,disabled:false,hidden:false,textContent:'',dataset:{},addEventListener(){},closest(){return null;},focus(){},querySelector:tag=>element(id+tag),replaceChildren(){},append(){}});return elements.get(id);};
  const heap=new Uint8Array(128),hud=new Float32Array(heap.buffer,0,15);hud[5]=hud[6]=2;
  const core={HEAPU8:heap,_th12_hud:()=>0,_th12_hud_size:()=>15,_th12_music_serial:()=>0,
    _th12_tick(held,pressed){ticks.push([held,pressed]);hud[0]++;},_th12_start(){hud[0]=0;}};
  const context={PreviewInput,supportedReplay,replayInputEnded,readCoreSoundEvents:()=>[],
    startReplayStage(){},console,performance:{now:()=>now},requestAnimationFrame(){},
    navigator:{getGamepads:()=>[]},window:{addEventListener:(type,listener)=>windowEvents.set(type,listener)},
    document:{hidden:false,getElementById:element,querySelectorAll:()=>[],addEventListener:(type,listener)=>documentEvents.set(type,listener)},
    Uint8Array,Uint32Array,Float32Array,Map,Set,Promise,setTimeout,clearTimeout,
    fixtureCore:core,fixtureReplay:{character:2,shot:1,difficulty:1,stages:[{number:1}],
      input(stage,frame){reads.push(frame);return inputs[frame]??null;}}};
  const source=fs.readFileSync(new URL('../site/main.mjs',import.meta.url),'utf8')
    .replace(/^import .*;\r?\n/gm,'').replace('initialize().catch(fail);','');
  vm.runInNewContext(source+`
    core=fixtureCore;renderer={render:()=>hud(),prepareStage:async()=>{}};
    stageLoader={load:async()=>true};store={sync:()=>Promise.resolve()};ready=running=true;
    activeReplay=fixtureReplay;audio.enabled=false;last=0;accumulator=0;
    globalThis.previewTest={key,clearKeyboard,pause,finish,start,loop,
      inspect:()=>({running,paused,replayFrame,generation,rate:replayRate(),accumulator}),
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
test('keyboard clear, stop and manual start remove replay acceleration without changing manual input',async()=>{
  const h=preview(inputSequence());h.api.key('ControlLeft',true,'host');h.frame();assert.equal(h.ticks.length,4);
  h.api.clearKeyboard();h.frame();assert.equal(h.ticks.length,5);h.api.key('ControlRight',true,'host');h.api.finish('game-over');assert.equal(h.api.inspect().rate,1);
  await h.api.start();h.event('keydown','ControlLeft');h.event('keydown','KeyZ');h.frame(0);
  assert.equal(h.api.inspect().rate,1);assert.deepEqual(h.ticks.at(-1),[1,1]);
});
test('Ctrl in form controls does not alter replay speed and normal play remains at60Hz',()=>{
  const h=preview(inputSequence());h.event('keydown','ControlLeft',{target:{closest:()=>({})}});h.frame();assert.equal(h.ticks.length,1);
  h.api.manual();h.event('keydown','ControlRight');h.event('keydown','KeyZ');h.frame();assert.deepEqual(h.ticks.at(-1),[1,1]);assert.equal(h.api.inspect().rate,1);
});
