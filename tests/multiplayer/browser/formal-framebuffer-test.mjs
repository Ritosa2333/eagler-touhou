import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp, readFile} from 'node:fs/promises';
import {join} from 'node:path';
import {tmpdir} from 'node:os';
import {installFramebufferCapture} from './formal-framebuffer.mjs';
import {persistFramebufferCapture, finishFramebufferAck} from './formal-framebuffer-persist.mjs';

function setup() {
  const keys=['READ_FRAMEBUFFER_BINDING','DRAW_FRAMEBUFFER_BINDING','PIXEL_PACK_BUFFER_BINDING',
    'PACK_ALIGNMENT','PACK_ROW_LENGTH','PACK_SKIP_ROWS','PACK_SKIP_PIXELS'];
  const gl=Object.fromEntries(keys.map(k=>[k,k]));
  Object.assign(gl,{READ_FRAMEBUFFER:'READ',PIXEL_PACK_BUFFER:'PBO',RGBA:'RGBA',UNSIGNED_BYTE:'BYTE',drawingBufferWidth:2,drawingBufferHeight:1});
  const values={READ_FRAMEBUFFER_BINDING:{read:true},DRAW_FRAMEBUFFER_BINDING:null,
    PIXEL_PACK_BUFFER_BINDING:{pbo:true}, PACK_ALIGNMENT:8,PACK_ROW_LENGTH:5,PACK_SKIP_ROWS:2,PACK_SKIP_PIXELS:3};
  const before={...values};
  gl.getParameter=k=>values[k];
  gl.bindFramebuffer=(_,v)=>{values.READ_FRAMEBUFFER_BINDING=v;};
  gl.bindBuffer=(_,v)=>{values.PIXEL_PACK_BUFFER_BINDING=v;};
  gl.pixelStorei=(k,v)=>{values[k]=v;};
  gl.readPixels=(_x,_y,_w,_h,_fmt,_type,pixels)=>{assert.equal(values.READ_FRAMEBUFFER_BINDING,null); assert.equal(values.PIXEL_PACK_BUFFER_BINDING,null); pixels.fill(5);};
  globalThis.document={querySelector:()=>({getContext:()=>gl})};
  let ghost=[0,0,0], lasers=[], mutate=false, snapshots=0;
  const module={HEAP32:new Int32Array(16),_th13_probe_state:()=>0,UTF8ToString:p=>p,
    _th13_debug_mp_seat_state:i=>JSON.stringify({seat:i,ghost:ghost[i]}),
    _th13_debug_replay_entities:()=>JSON.stringify({lasers}),
    _th13_debug_mp_host_state:()=>JSON.stringify({host:mutate ? snapshots++ : 0}),
    _th13_debug_replay_core:()=>'{"core":1}',_th13_debug_mp_net_state:()=>'{"sim_frame":1}'};
  module.HEAP32[0]=1; module.HEAP32[2]=1;
  installFramebufferCapture(); globalThis.__installFramebufferCapture(module,3);
  const state=globalThis.__framebufferCapture;
  let sequence=0;
  const work=(presentations=1)=>state.onWork({sequence:++sequence,begin:0,end:performance.now(),
    ok:true,phase:'active',presentations,presentationAt:presentations===0?null:1});
  return {gl,values,before,module,state,work,setGhost:v=>ghost=v,setLasers:v=>lasers=v,setMutate:v=>mutate=v};
}

test('next presentation captures and restores GL state; absent presentation retries',()=>{
  const t=setup(); t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.pendingReasons,['stage:1']);
  t.module.onGameFrame(true); assert.deepEqual(t.values,t.before); t.work(0);
  assert.equal(t.state.captures.length,0); assert.deepEqual(t.state.pendingReasons,['stage:1']);
  t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.captures[0].reasons,['stage:1']);
  assert.throws(()=>t.state.ack(2,2,1)); t.state.ack(1,1,1); assert.equal(t.state.captures.length,0);
});

test('non-default draw rejected; readPixels exception restores all state',()=>{
  let t=setup(); t.module.onGameFrame(true); t.work(); t.values.DRAW_FRAMEBUFFER_BINDING={draw:true};
  t.module.onGameFrame(true); t.work(); assert.match(t.state.failures[0],/not default/); assert.equal(t.state.candidate,null);
  t=setup(); t.module.onGameFrame(true); t.work(); t.gl.readPixels=()=>{throw Error('injected readPixels')};
  t.module.onGameFrame(true); assert.deepEqual(t.values,t.before); assert.match(t.state.failures[0],/injected readPixels/);
});
test('unsubmitted correction target is recorded, then retried at a real presentation',()=>{
  const t=setup();t.module.onGameFrame(true);t.work();
  t.values.DRAW_FRAMEBUFFER_BINDING={offscreen:true};t.module.onGameFrame(true);t.work(0);
  assert.deepEqual(t.state.failures,[]);assert.equal(t.state.attempts.length,1);
  assert.equal(t.state.attempts[0].work.presentations,0);
  assert.deepEqual(t.state.pendingReasons,['stage:1']);
  t.values.DRAW_FRAMEBUFFER_BINDING=null;t.module.onGameFrame(true);t.work();
  assert.equal(t.state.captures.length,1);assert.deepEqual(t.state.pendingReasons,[]);
});
test('candidate and work must describe the same callback interval',()=>{
  const t=setup();t.module.onGameFrame(true);t.work();t.module.onGameFrame(true);
  t.state.onWork({sequence:2,begin:performance.now()+100,end:performance.now()+200,
    ok:true,presentations:0,presentationAt:null});
  assert.match(t.state.failures[0],/association invalid/);
});

test('candidate requires a prior in-interval presentation and ordered unique work',()=>{
  for (const bad of ['future','before','zero-timed','duplicate','missing','multiple-callbacks']) {
    const t=setup();t.module.onGameFrame(true);t.work();t.module.onGameFrame(true);
    const now=performance.now();
    if(bad==='multiple-callbacks')t.module.onGameFrame(true);
    t.state.onWork({sequence:bad==='duplicate'?1:bad==='missing'?3:2,
      begin:bad==='before'?2:0,end:now+10,ok:true,
      presentations:bad==='zero-timed'?0:1,
      presentationAt:bad==='future'?now+5:1});
    assert(t.state.failures.length,`must reject ${bad}`);
    assert.equal(t.state.captures.length,0);
  }
});

test('changes to read-only state reject capture',()=>{
  const t=setup(); t.module.onGameFrame(true); t.work(); t.setMutate(true); t.module.onGameFrame(true);
  assert.match(t.state.failures[0],/state changed/); assert.equal(t.state.candidate,null);
});

test('stage, ghost, rescue and laser edges generate requests once',()=>{
  const t=setup(); t.module.onGameFrame(true); t.work();
  t.setGhost([0,1,0]); t.setLasers([{active:true}]); t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.captures[0].reasons,['stage:1']);
  assert.deepEqual(t.state.pendingReasons,['ghost:1','laser-start']);
  t.setGhost([0,0,0]); t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.captures[1].reasons,['ghost:1','laser-start']);
  assert.deepEqual(t.state.pendingReasons,['rescue:1']);
  t.module.HEAP32[2]=2; t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.captures[2].reasons,['rescue:1']); assert.deepEqual(t.state.pendingReasons,['stage:2']);
});
test('stage entry and settled presentation have distinct state-qualified requests',()=>{
  const t=setup(); t.module.onGameFrame(true); t.work();
  t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.captures[0].reasons,['stage:1']);
  t.module.HEAP32[5]=179; t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.pendingReasons,[]);
  t.module.HEAP32[5]=180; t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.pendingReasons,['stage-settled:1']);
  t.module.onGameFrame(true); t.work();
  assert.deepEqual(t.state.captures[1].reasons,['stage-settled:1']);
  assert.deepEqual(t.state.pendingReasons,[]);
});

test('raw pixels persist before validation; invalid or failed batches never retry',async()=>{
  const dir=await mkdtemp(join(tmpdir(),'framebuffer-test-'));
  const cursor={next:1,rawPending:0,failure:null}, evidence=[];
  const capture={failures:[],captures:[{sequence:1,width:2,height:1,rgbaBase64:Buffer.alloc(8).toString('base64')}]};
  await assert.rejects(persistFramebufferCapture(dir,0,capture,cursor,evidence),/entirely zero/);
  assert.deepEqual(await readFile(join(dir,'framebuffer-0-1.rgba')),Buffer.alloc(8));
  await assert.rejects(persistFramebufferCapture(dir,0,capture,cursor,evidence),/persistence\/validation failed/);
  assert.equal(evidence.length,0);
  const good={next:1,rawPending:0,failure:null}, pixels=Buffer.alloc(8,7);
  const accepted=await persistFramebufferCapture(dir,1,{failures:[],captures:[{sequence:1,width:2,height:1,rgbaBase64:pixels.toString('base64')}]},good,[]);
  finishFramebufferAck(good,evidence,accepted); assert.equal(good.next,2); assert.equal(evidence.length,1);
});
test('unmatched final candidate raw pixels are retained for diagnosis',async()=>{
  const dir=await mkdtemp(join(tmpdir(),'framebuffer-unmatched-'));
  const pixels=Buffer.alloc(8,9),cursor={next:1,rawPending:0,failure:null};
  const capture={failures:[],captures:[],candidate:{reasons:['stage:1'],rgbaBase64:pixels.toString('base64')}};
  await persistFramebufferCapture(dir,0,capture,cursor,[]);
  assert.deepEqual(await readFile(join(dir,'framebuffer-unmatched-0.rgba')),pixels);
  assert.deepEqual(JSON.parse(await readFile(join(dir,'framebuffer-unmatched-0.json'),'utf8')).reasons,['stage:1']);
});
