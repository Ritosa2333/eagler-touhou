import test from 'node:test';
import assert from 'node:assert/strict';
import {createSpectatorPlaybackBridge} from '../browser/spectator-playback.mjs';
function fixture() {
  let heap=new Uint8Array(16384),active=true,timer=null,revision=0,forced=null;
  const events=[],calls=[],allocations=[],frees=[];
  let report={version:1,run:'s0000000000000001_00000002',min:600,position:600,received:1600,edge:1510,target:600,
    paused:false,seeking:false,following:true,ended:false,nodes:[{id:1,kind:'stage',frame:600,stage:4,bossSlots:0}]};
  const string=pointer=>new TextDecoder().decode(heap.subarray(pointer,heap.indexOf(0,pointer)));
  const bridge=createSpectatorPlaybackBridge({active:()=>active,memory:()=>heap,
    state(){const bytes=new TextEncoder().encode(JSON.stringify(forced||{...report,revision:++revision})+'\0');heap.set(bytes,64);return 64;},
    allocate(size){allocations.push(size);const next=new Uint8Array(heap.length+4096);next.set(heap);heap=next;return 8192;},
    deallocate(pointer){frees.push(pointer);},
    control(run,action,frame){calls.push({run:string(run),action:string(action),frame});report={...report,paused:string(action)==='pause'};return 1;},
    emit(event,value){events.push({event,...value});},schedule(fn,ms){assert.equal(ms,250);timer=fn;return 7;},cancel(id){assert.equal(id,7);timer=null;},
  });
  return {bridge,events,calls,allocations,frees,get timer(){return timer;},set active(value){active=value;},
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
