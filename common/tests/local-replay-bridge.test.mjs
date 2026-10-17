import test from 'node:test';
import assert from 'node:assert/strict';
import {createLocalReplay} from '../browser/local-replay.mjs';
import {createNativeLocalReplay} from '../browser/local-replay-native.mjs';

function clock({bootstrap=0,prepare=async()=>{},openError=false,prepareTicks=0}={}) {
  let frame=0,ended=false,callback=null,serial=0,time=0,opens=0,stops=0,preparing=0;
  const reports=[],errors=[],description={character:0,difficulty:1,stages:[{id:3,score:123456}]};
  const player=createLocalReplay({inspect:()=>description,prepare,stopLoop(){stops++;},
    open(){opens++;if(openError)throw Error('native rejected');frame=0;ended=false;preparing=prepareTicks;},
    step(){if(preparing)preparing--;else frame++;ended=frame>=600;return true;},state:()=>({frame,total:600,ended,stage:3,startStage:3,bootstrapTarget:bootstrap,preparing:preparing>0}),
    onState:value=>reports.push(value),onError:value=>errors.push(value),
    raf:fn=>{callback=fn;return ++serial;},cancel:()=>{callback=null;},defer:fn=>{callback=fn;return ++serial;},cancelDeferred:()=>{callback=null;},now:()=>time,hidden:()=>false});
  const pump=()=>{const fn=callback;callback=null;time+=1000/60;fn?.(time);};
  return {player,pump,reports,errors,get frame(){return frame;},get opens(){return opens;},get stops(){return stops;}};
}
test('metadata inspection never starts native playback; stage must exist in the native descriptor',async()=>{
  const f=clock();await f.player.command({action:'inspect',path:'replay/a.rpy'});assert.equal(f.opens,0);assert.equal(f.stops,0);
  await assert.rejects(f.player.command({action:'open',path:'replay/a.rpy',stage:4}));assert.equal(f.opens,0);
});
test('startup preparation keeps the original native loop alive and closing cancels a pending open',async()=>{
  let ready;const gate=new Promise(resolve=>ready=resolve),f=clock({prepare:()=>gate});
  const opening=f.player.command({action:'open',path:'replay/a.rpy',stage:3});await Promise.resolve();assert.equal(f.stops,0);
  f.player.close();ready();await assert.rejects(opening,/关闭/);assert.equal(f.opens,0);
});
test('pause freezes native consumption; backward seek reopens the selected stage and stops exactly',async()=>{
  const f=clock();await f.player.command({action:'open',path:'replay/a.rpy',stage:3});for(let i=0;i<100;i++)f.pump();
  await f.player.command({action:'pause'});const before=f.frame;for(let i=0;i<20;i++)f.pump();assert.equal(f.frame,before);
  await f.player.command({action:'seek',value:20});f.pump();assert.equal(f.frame,20);assert.equal(f.opens,2);
  for(let i=0;i<20;i++)f.pump();assert.equal(f.frame,20);assert.equal(f.reports.at(-1).paused,true);
});
test('MP stage bootstrap retains its native start offset across restart and rejects earlier positions',async()=>{
  const f=clock({bootstrap:300});await f.player.command({action:'open',path:'replay/a.rpyx',stage:3});f.pump();
  assert.equal(f.frame,300);assert.equal(f.reports.at(-1).startFrame,300);
  await f.player.command({action:'pause'});await assert.rejects(f.player.command({action:'seek',value:20}));
  await f.player.command({action:'seek',value:300});f.pump();assert.equal(f.frame,300);
});

test('paused seek to zero finishes native initialization before retiring the seek target',async()=>{
  const f=clock({prepareTicks:3});await f.player.command({action:'open',path:'replay/a.rpy',stage:3});for(let i=0;i<20;i++)f.pump();
  await f.player.command({action:'pause'});await f.player.command({action:'seek',value:0});for(let i=0;i<7;i++)f.pump();
  assert.equal(f.frame,0);assert.equal(f.reports.at(-1).preparing,false);assert.equal(f.reports.at(-1).seeking,false);assert.deepEqual(f.errors,[]);
});
test('native rejection clears the previous playback presentation',async()=>{
  const f=clock({openError:true});await assert.rejects(f.player.command({action:'open',path:'replay/a.rpy',stage:3}));
  assert.equal(f.player.active,false);assert.equal(f.reports.at(-1),null);
});

test('seek yields bounded tasks without waiting for RAF, presents its exact paused target and cancels queued work',async()=>{
  let frame=0,time=0,serial=0,shown=0,presents=0,maxSteps=0;
  const animations=new Map(),tasks=new Map(),reports=[];
  const player=createLocalReplay({inspect:()=>({difficulty:1,stages:[{id:1,score:0}]}),open(){frame=0;},stopLoop(){},
    step(render){frame++;time++;if(render)shown=frame;return true;},present(){presents++;shown=frame;},deferPresentation:()=>true,
    state:()=>({frame,total:600,ended:false,stage:1,startStage:1}),onState:value=>reports.push(value),
    raf:fn=>{const id=++serial;animations.set(id,fn);return id;},cancel:id=>animations.delete(id),
    defer:fn=>{const id=++serial;tasks.set(id,fn);return id;},cancelDeferred:id=>tasks.delete(id),now:()=>time,hidden:()=>false});
  await player.command({action:'open',path:'a.rpy',stage:1});await player.command({action:'pause'});
  await player.command({action:'seek',value:100});assert.equal(animations.size,0);
  while(tasks.size){const [id,fn]=tasks.entries().next().value;tasks.delete(id);const before=frame;fn(time);maxSteps=Math.max(maxSteps,frame-before);}
  assert.equal(frame,100);assert.equal(shown,100);assert.ok(presents>0);assert.ok(maxSteps<=8);
  assert.equal(reports.at(-1).seeking,false);assert.equal(reports.at(-1).paused,true);assert.equal(reports.at(-1).rate,1);
  await player.command({action:'seek',value:200});assert.equal(tasks.size,1);player.close();assert.equal(tasks.size,0);assert.equal(animations.size,0);assert.equal(frame,100);
});
test('native C strings use zero terminators and malloc buffers are always freed',async()=>{
  const memory=new WebAssembly.Memory({initial:1});let next=64,allocated=0,freed=0;const heap=()=>new Uint8Array(memory.buffer);
  const core={memory,eagler_replay_allocate(n){allocated++;const p=next;next+=n;return p;},eagler_replay_free(){freed++;},
    eagler_replay_inspect(_data,_size,path){assert.equal(new TextDecoder().decode(heap().subarray(path,heap().indexOf(0,path))),'/save/replay/中文.rpy');
      const value=new TextEncoder().encode(JSON.stringify({character:0,difficulty:1,stages:[{id:1,score:0}]}));heap().set(value,4096);heap()[4096+value.length]=0;return 4096;}};
  const player=createNativeLocalReplay({core:()=>core,module:()=>({FS:{readFile:()=>new Uint8Array([1,2])}}),physical:path=>'/save/'+path,stopLoop(){}});
  await player.command({action:'inspect',path:'replay/中文.rpy'});assert.equal(allocated,2);assert.equal(freed,2);
});
