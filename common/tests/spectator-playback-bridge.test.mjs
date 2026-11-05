import test from 'node:test';
import assert from 'node:assert/strict';
import {createSpectatorPlaybackBridge} from '../browser/spectator-playback.mjs';
function fixture() {
  let heap=new Uint8Array(16384),active=true,timer=null,revision=0,forced=null;
  const events=[],calls=[],allocations=[],frees=[],completions=[];let replay,acceptCompletion=true;
  let report={version:1,run:'s0000000000000001_00000002',min:600,position:600,received:1600,edge:1510,target:600,
    paused:false,seeking:false,following:true,ended:false,nodes:[{id:1,kind:'stage',frame:600,stage:4,bossSlots:0}]};
  const string=pointer=>new TextDecoder().decode(heap.subarray(pointer,heap.indexOf(0,pointer)));
  const bridge=createSpectatorPlaybackBridge({active:()=>active,memory:()=>heap,
    replay:()=>replay,complete(pointer,size){completions.push([...heap.subarray(pointer,pointer+size)]);return acceptCompletion;},
    state(){const bytes=new TextEncoder().encode(JSON.stringify(forced||{...report,revision:++revision})+'\0');heap.set(bytes,64);return 64;},
    allocate(size){allocations.push(size);const next=new Uint8Array(heap.length+4096);next.set(heap);heap=next;return 8192;},
    deallocate(pointer){frees.push(pointer);},
    control(run,action,frame){calls.push({run:string(run),action:string(action),frame});report={...report,paused:string(action)==='pause'};return 1;},
    emit(event,value){events.push({event,...value});},schedule(fn,ms){assert.equal(ms,250);timer=fn;return 7;},cancel(id){assert.equal(id,7);timer=null;},
  });
  return {bridge,events,calls,allocations,frees,completions,set replay(value){replay=value;},set acceptCompletion(value){acceptCompletion=value;},get timer(){return timer;},set active(value){active=value;},
    get report(){return report;},set forced(value){forced=value;}};
}
test('native reports and authenticated controls share current run after heap growth',()=>{
  const f=fixture();f.bridge.start();f.bridge.start();assert.ok(f.timer);
  assert.equal(f.events[0].spectatorPlayback.position,600);
  assert.deepEqual(f.bridge.command({run:f.report.run,action:'pause'}),{});
  assert.deepEqual(f.calls,[{run:f.report.run,action:'pause',frame:0}]);
  assert.equal(f.events.at(-1).spectatorPlayback.paused,true);assert.deepEqual(f.frees,[8192]);
  assert.equal(f.allocations.length,1);
  f.bridge.command({run:f.report.run,action:'seek',frame:1200});assert.equal(f.calls.at(-1).frame,1200);
  f.bridge.stop();assert.equal(f.timer,null);assert.equal(f.events.at(-1).spectatorPlayback,null);
});
test('the original local menu opens once after End, tail playback and the matching complete file',()=>{
  const f=fixture(),file={session:'0000000000000001',bytes:new Uint8Array([1,2,3])};
  f.forced={...f.report,revision:1,ended:true,position:1600};
  assert.ok(f.bridge.poll());assert.equal(f.completions.length,0);
  f.replay=file;f.forced={...f.report,revision:1,ended:true,position:1600,paused:true};
  assert.ok(f.bridge.poll());assert.equal(f.completions.length,0);
  f.forced={...f.report,revision:2,ended:true,position:1599};
  assert.ok(f.bridge.poll());assert.equal(f.completions.length,0);
  f.forced={...f.report,revision:3,ended:true,position:1600,seeking:true};
  assert.ok(f.bridge.poll());assert.equal(f.completions.length,0);
  f.forced={...f.report,revision:3,ended:true,position:1600};
  assert.equal(f.bridge.poll(),null);assert.deepEqual(f.completions,[[1,2,3]]);
  assert.deepEqual(f.frees,[8192]);assert.equal(globalThis.__eaglerSpectatorMenu,true);
  assert.ok(f.events.some(event=>event.spectatorMenu===true));
  // Native handoff may close the read-only transport. Local menus retain input.
  f.replay=undefined;f.bridge.poll();assert.equal(globalThis.__eaglerSpectatorMenu,true);
  assert.equal(f.completions.length,1);
  assert.throws(()=>f.bridge.command({run:f.report.run,action:'play'}));
  f.bridge.stop();assert.equal(globalThis.__eaglerSpectatorMenu,false);
});
test('stale files cannot open results and a later run cannot take input away from local results',()=>{
  const f=fixture();f.forced={...f.report,revision:1,ended:true,position:1600};
  f.replay={session:'0000000000000002',bytes:new Uint8Array([2])};
  f.bridge.poll();assert.equal(f.completions.length,0);
  f.replay={session:'0000000000000001',bytes:new Uint8Array([1])};f.acceptCompletion=false;
  f.bridge.poll();assert.deepEqual(f.completions,[[1]]);assert.equal(f.events.some(event=>event.spectatorMenu===true),false);
  f.acceptCompletion=true;f.bridge.poll();assert.equal(globalThis.__eaglerSpectatorMenu,true);
  f.replay={session:'0000000000000002',bytes:new Uint8Array([2])};
  f.forced={...f.report,run:'s0000000000000002_00000003',revision:1};
  assert.equal(f.bridge.poll(),null);assert.equal(globalThis.__eaglerSpectatorMenu,true);
  assert.equal(f.completions.length,2);
  f.active=false;assert.equal(f.bridge.poll(),null);assert.equal(globalThis.__eaglerSpectatorMenu,false);
  f.active=true;assert.equal(f.bridge.poll().run,'s0000000000000002_00000003');f.bridge.stop();
});
test('invalid, stale and out-of-history controls never reach native state',()=>{
  const f=fixture();
  for(const message of [{run:'previous',action:'play'},{run:f.report.run,action:'seek',frame:599},
    {run:f.report.run,action:'seek',frame:1511},{run:f.report.run,action:'seek',frame:NaN},
    {run:f.report.run,action:'skip'},{run:'../run',action:'pause'}])assert.throws(()=>f.bridge.command(message));
  assert.equal(f.calls.length,0);assert.equal(f.allocations.length,0);
  f.active=false;assert.throws(()=>f.bridge.command({run:f.report.run,action:'play'}));assert.equal(f.bridge.poll(),null);
});
test('native readiness loss clears controls and older reports cannot move receive frontier back',()=>{
  const f=fixture();const first=f.bridge.poll();
  f.forced={...first,received:800,revision:first.revision+1};assert.equal(f.bridge.poll(),first);
  f.forced={...first,version:2};assert.equal(f.bridge.poll(),null);
  assert.equal(f.events.at(-1).spectatorPlayback,null);
});
