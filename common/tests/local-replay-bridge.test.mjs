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

function audioFixture(context={state:'running',resume:async()=>{}},pumpName='eagler_replay_audio_pump'){
  const previous={raf:globalThis.requestAnimationFrame,cancel:globalThis.cancelAnimationFrame,document:globalThis.document};
  const callbacks=new Map(),memory=new WebAssembly.Memory({initial:1}),heap=new Uint8Array(memory.buffer);
  let serial=0,time=0,frame=0,outputPaused=false,pumps=0,resumes=0,hidden=false;
  globalThis.document={get hidden(){return hidden;}};
  globalThis.requestAnimationFrame=fn=>{callbacks.set(++serial,fn);return serial;};globalThis.cancelAnimationFrame=id=>callbacks.delete(id);
  const voices=new Map(),stops=[];
  const module={HEAPU8:heap,SDL3:{audioContext:context},FS:{readFile:()=>new Uint8Array([1])},
    eaglerSfxOutput:{voices,stop(voice){voice.playing=false;voice.node=null;stops.push(voice);}}};
  const description=new TextEncoder().encode(JSON.stringify({difficulty:1,stages:[{id:1,score:0}]}));heap.set(description,4096);heap[4096+description.length]=0;
  const core={memory,eagler_replay_allocate:()=>8192,eagler_replay_free(){},eagler_replay_inspect:()=>4096,
    eagler_replay_open(){frame=0;return 1;},eagler_replay_step(){frame++;return 1;},[pumpName](){pumps++;},
    eagler_replay_state(){new Uint32Array(memory.buffer,1024,5).set([frame,5,+(frame>=5),1,1]);return 1024;}};
  const reports=[],waiters=new Set(),player=createNativeLocalReplay({core:()=>core,module:()=>module,physical:path=>path,
    stopLoop(){outputPaused=true;},pauseAudio:paused=>{outputPaused=paused;},resumeAudio(){resumes++;return module.SDL3.audioContext.resume();},onState(value){reports.push(value);for(const waiter of waiters)if(waiter.matches(value)){waiters.delete(waiter);waiter.resolve();}}});
  return {player,module,voices,stops,reports,get frame(){return frame;},get outputPaused(){return outputPaused;},get pumps(){return pumps;},get resumes(){return resumes;},
    hide(value){hidden=value;},waitForReport(matches){
      if(matches(reports.at(-1)))return Promise.resolve();
      return new Promise((resolve,reject)=>{
        const waiter={matches,resolve(){clearTimeout(timeout);resolve();}};
        const timeout=setTimeout(()=>{waiters.delete(waiter);reject(Error('Replay state report timed out'));},2000);
        waiters.add(waiter);
      });
    },draw(){const entry=callbacks.entries().next().value;if(!entry)return;const [id,fn]=entry;callbacks.delete(id);fn(time+=1000/60);},
    close(){player.close();globalThis.requestAnimationFrame=previous.raf;globalThis.cancelAnimationFrame=previous.cancel;globalThis.document=previous.document;}};
}

test('native output resumes after every open, pumps playback, and follows pause, visibility, EOF and close',async()=>{
  const f=audioFixture();
  try{
    await f.player.command({action:'open',path:'a.rpy'});assert.equal(f.outputPaused,false);assert.equal(f.player.audioPaused,false);
    f.draw();f.draw();assert.ok(f.pumps>0);
    await f.player.command({action:'pause'});assert.equal(f.outputPaused,true);const at=f.frame;f.draw();assert.equal(f.frame,at);
    f.hide(true);f.draw();f.hide(false);f.draw();assert.equal(f.outputPaused,true,'foreground keeps the user pause');
    await f.player.command({action:'play'});assert.equal(f.outputPaused,false);
    await f.player.command({action:'open',path:'b.rpy'});assert.equal(f.outputPaused,false,'native stop/reconstruction invalidates the previous output state');
    for(let i=0;i<12;i++)f.draw();assert.equal(f.frame,5);assert.equal(f.outputPaused,true);
    await f.player.command({action:'play'});assert.equal(f.frame,0);assert.equal(f.outputPaused,false);
    f.player.close();assert.equal(f.outputPaused,true);assert.equal(f.player.active,false);
  }finally{f.close();}
});

test('th10 fills the SDL PCM queue separately from its native music task pump',async()=>{
  const f=audioFixture(undefined,'sdl_audio_pump');
  try{
    await f.player.command({action:'open',path:'replay/a.rpy'});f.draw();f.draw();
    assert.ok(f.frame>0);assert.equal(f.pumps,f.frame);
    await f.player.command({action:'pause'});f.draw();assert.equal(f.pumps,f.frame);
  }finally{f.close();}
});

test('seek keeps its pause owner and discards transient voices while retaining loop state',async()=>{
  const f=audioFixture();
  try{
    await f.player.command({action:'open',path:'a.rpy'});f.draw();f.draw();
    await f.player.command({action:'pause'});
    const old={loop:false,playing:true},loop={loop:true,playing:true};f.voices.set(1,old);f.voices.set(2,loop);
    await f.player.command({action:'seek',value:0});assert.equal(old.playing,false);
    const skipped={loop:false,playing:true};f.voices.set(3,skipped);
    await f.waitForReport(value=>value&&!value.seeking);
    assert.equal(f.reports.at(-1).seeking,false);assert.equal(f.reports.at(-1).paused,true);
    assert.equal(f.outputPaused,true);assert.equal(skipped.playing,false);assert.equal(loop.playing,true);
    await f.player.command({action:'play'});assert.equal(f.outputPaused,false);
  }finally{f.close();}
});

test('pending browser recovery leaves RPCs responsive, retains logical frame, and resumes once output runs',async()=>{
  let finish;const context={state:'suspended',resume:()=>new Promise(resolve=>{finish=resolve;})},f=audioFixture(context);
  try{
    await f.player.command({action:'open',path:'a.rpy'});assert.equal(f.resumes,1);
    for(let i=0;i<8;i++)f.draw();assert.equal(f.frame,0);assert.equal(f.resumes,1);assert.equal(f.outputPaused,true);
    await f.player.command({action:'pause'});context.state='running';finish();await Promise.resolve();f.draw();assert.equal(f.outputPaused,true);
    await f.player.command({action:'play'});f.draw();f.draw();assert.equal(f.outputPaused,false);assert.ok(f.frame>0);
  }finally{f.close();}
});
