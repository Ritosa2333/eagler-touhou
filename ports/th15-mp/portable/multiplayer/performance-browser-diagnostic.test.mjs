import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {installGlDiagnostic,startCpuDiagnostic,stopCpuDiagnostic,captureProcessCpuDiagnostic,compareProcessCpuDiagnostics} from './performance-browser-diagnostic.mjs';

function fixture(options={}){
  const clock={at:0},calls=[],state={phase:'measure',start:0,end:100,current:null};
  const context={state,options,performance:{now:()=>clock.at},WebGL2RenderingContext:class {
    constructor(id='canvas'){this.canvas={id};this.delay=3;}
    texImage2D(...args){calls.push(['texImage2D',...args]);clock.at+=this.delay;return 17;}
    texSubImage2D(...args){calls.push(['texSubImage2D',...args]);clock.at+=this.delay;}
    bufferData(...args){calls.push(['bufferData',...args]);clock.at+=this.delay;}
    bufferSubData(...args){calls.push(['bufferSubData',...args]);clock.at+=this.delay;}
    drawArrays(...args){calls.push(['drawArrays',...args]);clock.at+=this.delay;if(this.failure)throw this.failure;return 31;}
    readPixels(){throw Error('The observer must never issue a readback');}
    getParameter(){throw Error('The observer must never query GL state');}
  }};
  runInNewContext('('+installGlDiagnostic.toString()+')(state,options);globalThis.gl=new WebGL2RenderingContext();',context);
  return {context,state,clock,calls,gl:context.gl};
}

test('diagnostic wraps the real call, keeps its receiver/result, and attributes GL time to the active callback',()=>{
  const f=fixture();f.state.current={row:{id:7,native:true}};
  assert.equal(f.gl.drawArrays(4,0,3),31);
  const row=f.state.current.row.glDiagnostic;assert.equal(row.calls,1);assert.equal(row.wallMs,3);
  assert.equal(row.methods.drawArrays.calls,1);assert.equal(f.state.glDiagnostic.phases.measure.outsideCallback.calls,0);
  f.state.current=null;f.gl.drawArrays(4,0,3);
  assert.equal(f.state.glDiagnostic.phases.measure.all.calls,2);assert.equal(f.state.glDiagnostic.phases.measure.outsideCallback.calls,1);
  assert.deepEqual(f.calls,[['drawArrays',4,0,3],['drawArrays',4,0,3]]);
});

test('texture payload sizes distinguish RGBA uploads, packed pixels, null allocation and unsupported overloads',()=>{
  const f=fixture(),data=new Uint8Array(64);
  assert.equal(f.gl.texImage2D(3553,0,6408,4,2,0,6408,5121,data),17);
  f.gl.texSubImage2D(3553,0,0,0,3,2,6408,32819,data);
  f.gl.texImage2D(3553,0,6408,4,2,0,6408,5121,null);
  f.gl.texImage2D(3553,0,6408,6408,5121,{width:4,height:2});
  const stats=f.state.glDiagnostic.phases.measure.all;
  assert.equal(stats.textureUploadBytes,44);assert.equal(stats.textureAllocationBytes,32);
  assert.equal(stats.unknownTextureUploadCalls,1);assert.equal(stats.calls,4);
  assert.equal(f.calls.length,4); // No getParameter/readPixels calls were introduced.
});

test('buffer upload counts use the supplied view and element offset/length rather than the backing buffer',()=>{
  const f=fixture(),backing=new Uint16Array(32),view=backing.subarray(4,12);
  f.gl.bufferData(34962,view,35040,2,3); // Three 16-bit elements.
  f.gl.bufferSubData(34962,128,view,3); // Five remaining 16-bit elements.
  f.gl.bufferData(34962,4096,35040); // Allocation without an upload.
  const stats=f.state.glDiagnostic.phases.measure.all;
  assert.equal(stats.bufferUploadBytes,16);assert.equal(stats.bufferAllocationBytes,4096);
});

test('long calls remain bounded, preserve thrown exceptions, and retain calls that cross the fixed window',()=>{
  const f=fixture({longCallThresholdMs:25,maxLongCalls:1});f.clock.at=90;f.gl.delay=30;
  f.state.current={row:{id:11,native:true}};f.gl.drawArrays(4,0,3);
  const failure=Error('driver error');f.gl.failure=failure;
  assert.throws(()=>f.gl.drawArrays(4,0,3),error=>error===failure);
  const diagnostic=f.state.glDiagnostic;
  assert.equal(diagnostic.longCalls.length,1);assert.equal(diagnostic.droppedLongCalls,1);
  assert.equal(diagnostic.longCalls[0].callbackId,11);assert.equal(diagnostic.longCalls[0].ms,30);
  assert.equal(diagnostic.phases.measure.all.calls,1);assert.equal(diagnostic.phases['outside-fixed-window'].all.calls,1);
  assert.equal(f.state.current.row.glDiagnostic.calls,2);assert.equal(f.state.current.row.glDiagnostic.thrownCalls,1);
});

test('other canvases are forwarded without diagnostic counting',()=>{
  const f=fixture();f.gl.canvas.id='another-canvas';assert.equal(f.gl.drawArrays(4,0,3),31);
  assert.equal(Object.keys(f.state.glDiagnostic.phases).length,0);assert.equal(f.calls.length,1);
});

function profilerFixture({failure}={}){
  const events=[],profile={startTime:1000,endTime:2000,nodes:[{id:1}],samples:[1,1]},session={
    async send(method,args){events.push({method,args});if(method===failure)throw Error(method+' failed');return method==='Profiler.stop'?{profile}:{};},
    async detach(){events.push({method:'detach'});},
  };
  const page={context:()=>({newCDPSession:async target=>{assert.equal(target,page);events.push({method:'newCDPSession'});return session;}}),
    evaluate:async()=>{events.push({method:'clock'});return {timeOrigin:500,now:5,dateNow:505};}};
  return {page,events,profile};
}

test('CPU sampler sets 4ms interval before start, records clock bounds, stops once and detaches',async()=>{
  const f=profilerFixture(),handle=await startCpuDiagnostic(f.page),result=await stopCpuDiagnostic(handle);
  assert.deepEqual(f.events.map(e=>e.method),['newCDPSession','Profiler.enable','Profiler.setSamplingInterval','clock','Profiler.start','clock',
    'clock','Profiler.stop','clock','Profiler.disable','detach']);
  assert.deepEqual(f.events.find(e=>e.method==='Profiler.setSamplingInterval').args,{interval:4000});
  assert.equal(result.profile,f.profile);assert.equal(result.metadata.samples,2);assert.equal(result.metadata.profileStartTime,1000);
  assert.equal(handle.closed,true);assert.equal(handle.metadata.stopped,true);
  await assert.rejects(stopCpuDiagnostic(handle),/already stopped/);
});

test('CPU profiler start failure disables and detaches instead of leaving a live CDP session',async()=>{
  const f=profilerFixture({failure:'Profiler.start'});
  await assert.rejects(startCpuDiagnostic(f.page),/Profiler.start failed/);
  assert.deepEqual(f.events.slice(-2).map(e=>e.method),['Profiler.disable','detach']);
  assert.equal(f.events.some(e=>e.method==='Profiler.stop'),false);
});

test('CPU profiler stop failure still closes the session and does not manufacture a profile',async()=>{
  const f=profilerFixture({failure:'Profiler.stop'}),handle=await startCpuDiagnostic(f.page);
  await assert.rejects(stopCpuDiagnostic(handle),/Profiler.stop failed/);
  assert.equal(handle.closed,true);assert.equal(handle.metadata.stopped,false);
  assert.deepEqual(f.events.slice(-2).map(e=>e.method),['Profiler.disable','detach']);
});

test('process CPU snapshot brackets one actual SystemInfo read and detaches its browser session',async()=>{
  const events=[],processInfo=[{id:10,type:'GPU',cpuTime:2.5},{id:11,type:'renderer',cpuTime:1.25}];
  const browser={newBrowserCDPSession:async()=>({send:async method=>{events.push(method);return {processInfo};},
    detach:async()=>events.push('detach')})};
  const snapshot=await captureProcessCpuDiagnostic(browser,'before-profiler-start');
  assert.deepEqual(events,['SystemInfo.getProcessInfo','detach']);assert.deepEqual(snapshot.processes,processInfo);
  assert.equal(snapshot.phase,'before-profiler-start');assert.equal(snapshot.cpuTimeUnit,'seconds');
  assert.ok(snapshot.requestedAt.monotonicMs<=snapshot.completedAt.monotonicMs);
  assert.ok(Number.isFinite(Date.parse(snapshot.requestedAt.utc)));assert.ok(Number.isFinite(snapshot.completedAt.unixMs));
});

test('failed process CPU read still detaches and does not fabricate counter data',async()=>{
  const events=[],browser={newBrowserCDPSession:async()=>({send:async()=>{throw Error('SystemInfo unavailable');},
    detach:async()=>events.push('detach')})};
  await assert.rejects(captureProcessCpuDiagnostic(browser,'before'),/SystemInfo unavailable/);
  assert.deepEqual(events,['detach']);
});

function processSnapshots(){
  return {before:{requestedAt:{monotonicMs:100},completedAt:{monotonicMs:110},processes:[
    {type:'GPU',id:10,cpuTime:2.5},{type:'renderer',id:11,cpuTime:1.25},{type:'browser',id:12,cpuTime:.5}]},
  after:{requestedAt:{monotonicMs:1110},completedAt:{monotonicMs:1120},processes:[
    {type:'GPU',id:10,cpuTime:4.5},{type:'renderer',id:11,cpuTime:1.75},{type:'browser',id:12,cpuTime:.6}]}};
}

test('process CPU differences separate GPU/renderer and retain wall-bracket bounds and multicore rates',()=>{
  const {before,after}=processSnapshots(),result=compareProcessCpuDiagnostics(before,after);
  assert.deepEqual(result.elapsedMs,{minimum:1000,maximum:1020,midpoint:1010});
  assert.equal(result.groups.gpu.deltaCpuSeconds,2);assert.equal(result.groups.renderer.deltaCpuSeconds,.5);
  assert.equal(result.groups.gpu.averageCpuCores.maximum,2);assert.equal(result.groups.gpu.averageCpuCores.minimum,2/1.02);
  assert.equal(result.allProcessesMatched,true);assert.equal(result.validMatchedCounters,true);
});

test('new/disappeared processes do not inherit another PID counter or invent zero CPU use',()=>{
  const {before,after}=processSnapshots();after.processes[0]={type:'GPU',id:99,cpuTime:.2};
  const result=compareProcessCpuDiagnostics(before,after);
  assert.equal(result.allProcessesMatched,false);assert.equal(result.groups.gpu.matchedProcesses,0);
  assert.equal(result.groups.gpu.unmatchedProcesses,2);assert.equal(result.groups.gpu.deltaCpuSeconds,null);
  assert.equal(result.groups.gpu.averageCpuCores,null);
  assert.equal(result.processes.find(p=>p.id===10).status,'disappeared');assert.equal(result.processes.find(p=>p.id===99).status,'appeared');
});

test('regressed CPU counters and invalid clock brackets remain explicit failures',()=>{
  const {before,after}=processSnapshots();after.processes[0].cpuTime=1;
  const result=compareProcessCpuDiagnostics(before,after);
  assert.equal(result.validMatchedCounters,false);assert.equal(result.processes.find(p=>p.id===10).status,'counter-regressed');
  assert.equal(result.groups.gpu.deltaCpuSeconds,null);
  after.requestedAt.monotonicMs=50;assert.throws(()=>compareProcessCpuDiagnostics(before,after),/invalid clock bounds/);
});
