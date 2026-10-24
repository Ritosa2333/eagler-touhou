import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {neutralPlan,committedHorizon,assertProofState,assertBeforeDeadline,awaitBeforeDeadline,proveNeutral,fireHorizon,proveFire} from './performance-input-proof.mjs';

function snapshot(seat,capture,delay=0){const net=Array(24).fill(0),rollback=Array(20).fill(0);
  Object.assign(net,{1:1,2:1,5:2,6:seat,7:delay,12:4});Object.assign(rollback,{0:1,2:capture,3:capture,4:capture+delay-1});
  return {net,rollback,nativePhase:1,error:''};}
function replay(frames,buttons=()=>0){const bytes=new Uint8Array(180+frames*24),view=new DataView(bytes.buffer);
  bytes.set(new TextEncoder().encode('EAGLRPY1'));view.setUint32(8,1,true);view.setUint32(12,15,true);bytes[20]=2;
  view.setUint32(24,132,true);view.setUint32(28,1,true);view.setUint32(32,frames,true);
  for(let frame=0;frame<frames;++frame)for(let seat=0;seat<2;++seat)view.setUint16(180+(frame*2+seat)*12,buttons(frame,seat),true);
  return bytes;
}
function fixture({delay=0,direction=false}={}){
  const plan=neutralPlan([snapshot(0,10,delay),snapshot(1,12,delay)]),first=plan.end;
  const hosts=[0,1].map(seat=>({before:snapshot(seat,first+seat-delay,delay),afterInput:snapshot(seat,first+seat+2-delay,delay),
    inputAckBeforeSync:true,events:[{kind:'input-ack',at:20},{kind:'sync-ack',at:80}],errors:[]}));
  const end=fireHorizon(hosts,plan),bytes=replay(end+2,(frame,seat)=>frame>=first+seat?1|(direction?128:0):0);
  return {plan,hosts,end,bytes,buffers:[bytes,bytes.slice()]};
}

test('two confirmed all-seat neutral frames precede newly held fire even when movement is gated',()=>{
  const f=fixture();assert.equal(proveNeutral(f.buffers,f.plan).framesPerSeat,2);
  const proof=proveFire(f.buffers,f.plan,f.hosts,f.end);assert.equal(proof.passed,true);
  assert.deepEqual(proof.seats.map(seat=>seat.transitionFrame),[14,15]);
  assert.ok(proof.seats.every(seat=>seat.fireFrames>=2&&seat.rightFramesObserved===0));
});
test('stale always-fire input cannot satisfy the neutral precondition',()=>{
  const f=fixture(),bytes=replay(f.end,()=>1);assert.throws(()=>proveNeutral([bytes,bytes],f.plan),/neutral baseline contains input/);
});
test('ACKed but dropped fire fails instead of relying on old input or ACK alone',()=>{
  const f=fixture(),bytes=replay(f.end);assert.throws(()=>proveFire([bytes,bytes],f.plan,f.hosts,f.end),/No new native-confirmed/);
});
test('direction alone and a one-frame fire pulse both fail the sustained new-fire proof',()=>{
  const f=fixture();for(const buttons of [(frame,seat)=>frame>=14+seat?128:0,(frame,seat)=>frame===14+seat?1:0]){
    const bytes=replay(f.end,buttons);assert.throws(()=>proveFire([bytes,bytes],f.plan,f.hosts,f.end),/No new native-confirmed/);
  }
});
test('sampling before ACK delivery remains eligible while horizon includes two post-ACK captures',()=>{
  const f=fixture({direction:true}),proof=proveFire(f.buffers,f.plan,f.hosts,f.end);
  assert.equal(proof.seats[0].transitionFrame,14);assert.equal(f.hosts[0].afterInput.rollback[3],16);
  assert.equal(f.end,19);assert.ok(proof.seats[0].rightFramesObserved>0);
});
test('actual D=2 and uneven capture frontiers map neutral and fire to the same confirmed frames',()=>{
  const f=fixture({delay:2});assert.deepEqual([f.plan.first,f.plan.end],[14,16]);
  assert.deepEqual(proveFire(f.buffers,f.plan,f.hosts,f.end).seats.map(seat=>seat.transitionFrame),[16,17]);
});
test('missing exported proof horizon is rejected, never clipped to available Replay',()=>{
  const f=fixture(),short=replay(f.end-1,(frame,seat)=>frame>=14+seat?1:0);
  assert.throws(()=>proveFire([short,short],f.plan,f.hosts,f.end),/does not cover/);
});
test('proof requires every peer, all-seat prefix equality, and true committed frontiers',()=>{
  const f=fixture(),changed=f.bytes.slice();changed[180+(15*2+1)*12]^=2;
  assert.throws(()=>proveFire([f.bytes,changed],f.plan,f.hosts,f.end),/prefixes disagree/);
  assert.throws(()=>proveNeutral([f.bytes],f.plan),/Missing input-proof peer/);
  const states=[snapshot(0,f.end),snapshot(1,f.end)];assert.equal(committedHorizon(states,f.plan,f.end),true);
  states[1].rollback[4]=0xffffffff;assert.equal(committedHorizon(states,f.plan,f.end),false);
  states[1].rollback[4]=f.end-2;assert.equal(committedHorizon(states,f.plan,f.end),false);
});
test('generation, delay, seat ownership, capture regression and lifecycle changes are failures',()=>{
  const f=fixture();for(const mutate of [s=>s.net[12]++,s=>s.net[7]++,s=>s.net[6]=1,s=>s.rollback[3]=1,s=>s.nativePhase=2,s=>s.net[13]=1]){
    const state=snapshot(0,14);mutate(state);assert.throws(()=>assertProofState(state,f.plan.references[0]));
  }
});
test('ACK-before-sync remains mandatory for the concurrent command, including exact timestamps',()=>{
  const f=fixture();f.hosts[0].inputAckBeforeSync=false;assert.throws(()=>proveFire(f.buffers,f.plan,f.hosts,f.end),/ACK did not precede/);
  f.hosts[0].inputAckBeforeSync=true;f.hosts[0].events[0].at=81;assert.throws(()=>proveFire(f.buffers,f.plan,f.hosts,f.end),/ACK timestamps/);
});
test('phase deadlines bound unresolved asynchronous reads and do not reset on progress',async()=>{
  assertBeforeDeadline(9,10,'neutral');assert.throws(()=>assertBeforeDeadline(10,10,'neutral'),/deadline expired/);
  await assert.rejects(awaitBeforeDeadline(new Promise(()=>{}),performance.now()+10,'fire'),/fire deadline expired/);
  assert.equal(await awaitBeforeDeadline(Promise.resolve(7),performance.now()+100,'completed'),7);
});
test('a late successful promise cannot beat an expired timer after event-loop blockage',async()=>{
  const deadline=performance.now()+10,work=Promise.resolve().then(()=>{const until=performance.now()+20;while(performance.now()<until){}return 7;});
  await assert.rejects(awaitBeforeDeadline(work,deadline,'blocked-read'),/blocked-read deadline expired/);
});

// Execute the actual serialized Host methods and copy observer. These tests
// verify harness command ordering/cancellation, not a substitute game model.
const source=readFileSync(new URL('./performance-browser.mjs',import.meta.url),'utf8');
const hostSource=source.slice(source.indexOf('function host('),source.indexOf('\n// Read the actual IDBFS'));
function hostFixture({syncMs=10,inputMs=0,never=false}={}){
  const listeners=new Set(),commands=[],snap=snapshot(0,20),child={__th15Performance:{snapshot:()=>structuredClone(snap)}};let serial=0;
  const iframe={contentWindow:child},context={performance,TextDecoder,TextEncoder,setTimeout,clearTimeout,
    location:{origin:'http://proof.test'},crypto:{randomUUID:()=>String(++serial)},
    addEventListener:(name,callback)=>listeners.add(callback),removeEventListener:(name,callback)=>listeners.delete(callback),
    document:{querySelector:()=>iframe,createElement:()=>iframe,body:{append:()=>{}}}};
  child.postMessage=message=>{commands.push(message);if(never)return;
    setTimeout(()=>{const event={source:child,origin:context.location.origin,data:{protocol:'eagler-touhou/1',request:message.request,ok:true}};
      for(const listener of [...listeners])listener(event);},message.command==='sync'?syncMs:inputMs);};
  context.window=context;runInNewContext('('+hostSource+')(0)',context);return {context,commands,listeners};
}
test('actual Host neutralizer stops producers and waits every outstanding ACK before explicit neutral',async()=>{
  const f=hostFixture();let stopped=false;
  f.context.inputEvidence={submitted:2,acknowledged:1,errors:[]};f.context.stopInput=()=>{stopped=true;};
  const result=f.context.neutralize(500);assert.equal(stopped,true);assert.equal(f.commands.length,0);
  f.context.inputEvidence.acknowledged=2;const evidence=await result;
  assert.equal(f.commands.length,1);assert.equal(f.commands[0].controls.fireEnabled,false);
  assert.equal(evidence.settledProducer.submitted,evidence.settledProducer.acknowledged);
});
test('actual concurrent Host proof holds input until coordinator release and separates sync ACK',async()=>{
  const f=hostFixture(),evidence=await f.context.verifySave(500);
  assert.equal(evidence.inputAckBeforeSync,true);assert.equal(evidence.errors.length,0);
  assert.deepEqual(f.commands.map(command=>command.command),['sync','touch-controls']);
  assert.equal(f.commands[1].controls.fireEnabled,true);assert.equal(evidence.commands[1].request,'2');
  await f.context.releaseProofInput(500);assert.equal(f.commands[2].controls.fireEnabled,false);
});
test('actual Host proof records late ACK and RPC deadline failures',async()=>{
  const late=hostFixture({syncMs:0,inputMs:10}),evidence=await late.context.verifySave(500);
  assert.equal(evidence.inputAckBeforeSync,false);
  const blocked=hostFixture({never:true}),failed=await blocked.context.verifySave(15);
  assert.equal(failed.errors.length,2);assert.ok(failed.errors.every(error=>error.error.includes('timeout')));
});
test('completed Host Replay observer writes only its independent temporary copy',()=>{
  const bytes=replay(20),files=new Map([['/save/replay/th15mp_01.rpy',bytes.slice()]]),writes=[],heap=new Uint8Array(4096);
  const context={performance,Module:{HEAPU8:heap,_malloc:()=>128,_free:()=>{},
    _th15_mp_replay_save:()=>{throw Error('Probe must not invoke native export');},
    _th15_mp_replay_validate:(at,size)=>{assert.deepEqual(heap.slice(at,at+size),bytes);return 1;},
    FS:{mkdirTree:()=>{},readFile:path=>files.get(path).slice(),writeFile:(path,data)=>{writes.push(path);files.set(path,data.slice());}}},
    __th15Performance:{snapshot:()=>snapshot(0,20)}};
  const copySource=source.slice(source.indexOf('function copySavedReplay('),source.indexOf('\nasync function waitProofCommit('));
  runInNewContext(copySource,context);const result=context.copySavedReplay({label:'evidence-sync',seat:0});
  assert.deepEqual(writes,['/tmp/th15-input-proof/evidence-sync-0.rpy']);assert.equal(result.nativeValid,true);
  assert.deepEqual(files.get('/save/replay/th15mp_01.rpy'),bytes);assert.deepEqual(Array.from(result.bytes),Array.from(bytes));
});
