import test from 'node:test';
import assert from 'node:assert/strict';
import {parseGpuVsyncControl,gpuVsyncLaunchArgs,inspectGpuVsyncLaunch,inspectGpuVsyncRun} from './gpu-vsync-control.mjs';

const a=parseGpuVsyncControl('0',true),b=parseGpuVsyncControl('1',true);
const observed=(control,args,extra={})=>inspectGpuVsyncLaunch(control,{args,observationAvailable:true,verified:true,browserPid:1234,...extra});
const launch=(control,args,pid)=>({browserPid:pid,observationAvailable:true,verified:true,gpuVsync:observed(control,args,{browserPid:pid})});

test('unset preserves default arguments and does not impose diagnostic qualification',()=>{
 const control=parseGpuVsyncControl(undefined,false),base=['--enable-gpu','--disable-frame-rate-limit'];
 assert.equal(control.explicit,false);assert.equal(control.disableGpuVsync,null);
 assert.equal(gpuVsyncLaunchArgs(base,control),base);
 assert.deepEqual(inspectGpuVsyncLaunch(control),{required:false,qualified:null});
 assert.deepEqual(inspectGpuVsyncRun(control,[]),{required:false,qualified:null});
});

test('explicit environment accepts only exact strings 0 or 1 with headed control',()=>{
 assert.equal(a.disableGpuVsync,false);assert.equal(b.disableGpuVsync,true);
 for(const value of ['', ' ', ' 0', '1 ', 'true', '2', 0, 1, true, null])assert.throws(()=>parseGpuVsyncControl(value,true),/exact string/);
 for(const value of ['0','1'])for(const headed of [false,undefined,1,'1'])assert.throws(()=>parseGpuVsyncControl(value,headed),/HEADED_CONTROL=1/);
});

test('launch construction changes only one bare flag and does not mutate base',()=>{
 const base=['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required'];
 const original=base.slice();
 assert.equal(gpuVsyncLaunchArgs(base,a),base);
 assert.deepEqual(gpuVsyncLaunchArgs(base,b),[...original,'--disable-gpu-vsync']);
 assert.deepEqual(base,original);assert.deepEqual(a.addedArgs,[]);assert.deepEqual(b.addedArgs,['--disable-gpu-vsync']);
});

test('A requires the actual GPU flag to be absent, including assigned and case variants',()=>{
 const good=observed(a,['chrome.exe','--enable-gpu','--use-angle=d3d11']);
 assert.equal(good.qualified,true);assert.equal(good.exactCount,0);assert.equal(good.totalCount,0);
 for(const args of [['--disable-gpu-vsync'],['--disable-gpu-vsync=0'],['--disable-gpu-vsync='],['--disable-gpu-vsync','--disable-gpu-vsync'],['--DISABLE-GPU-VSYNC']]){
  assert.equal(observed(a,args).qualified,false,JSON.stringify(args));
 }
});

test('B requires exactly one bare actual GPU flag and rejects missing, duplicate or assigned variants',()=>{
 const good=observed(b,['chrome.exe','--disable-gpu-vsync','--use-angle=d3d11']);
 assert.equal(good.qualified,true);assert.equal(good.exactCount,1);assert.equal(good.nonExactCount,0);
 for(const args of [[],['--disable-gpu-vsync','--disable-gpu-vsync'],['--disable-gpu-vsync=1'],['--disable-gpu-vsync','--disable-gpu-vsync=0'],['--DISABLE-GPU-VSYNC']]){
  assert.equal(observed(b,args).qualified,false,JSON.stringify(args));
 }
});

test('both modes reject actual frame-rate-limit flags without disclosing assigned values',()=>{
 for(const control of [a,b])for(const flag of ['--disable-frame-rate-limit','--disable-frame-rate-limit=0','--DISABLE-FRAME-RATE-LIMIT=1']){
  const result=observed(control,[...control.addedArgs,flag]);assert.equal(result.qualified,false);assert.equal(result.frameLimitCount,1);
 }
 const privateValue='C:/private/profile-location';
 const result=observed(b,['C:/private/chrome.exe','--disable-gpu-vsync='+privateValue]);
 assert.equal(result.assignedCount,1);assert.equal(result.nonExactCount,1);assert.equal(result.qualified,false);
 assert.ok(!JSON.stringify(result).includes('private'));
});

test('missing observation, verification, arguments or a usable PID can never qualify',()=>{
 for(const extra of [{observationAvailable:false},{observationAvailable:undefined},{verified:false},{verified:undefined},{args:null},{args:['--disable-gpu-vsync',42]},{browserPid:0},{browserPid:1.5},{browserPid:Number.MAX_SAFE_INTEGER+1}]){
  assert.equal(observed(b,['--disable-gpu-vsync'],extra).qualified,false);
 }
 const missing=inspectGpuVsyncLaunch(b);
 assert.equal(missing.qualified,false);assert.equal(missing.exactCount,null);assert.equal(missing.frameLimitCount,null);
});

test('run qualification requires two verified observations with distinct positive owned PIDs',()=>{
 const rows=[launch(b,['--disable-gpu-vsync'],100),launch(b,['--disable-gpu-vsync'],200)];
 assert.equal(inspectGpuVsyncRun(b,rows).qualified,true);
 assert.equal(inspectGpuVsyncRun(a,[launch(a,[],100),launch(a,[],200)]).qualified,true);
 for(const invalid of [[],[rows[0]],[...rows,rows[0]],[rows[0],{...rows[1],browserPid:100}],[rows[0],{...rows[1],browserPid:0}],[rows[0],{...rows[1],observationAvailable:false}],[rows[0],{...rows[1],verified:false}],[rows[0],launch(a,[],200)],[rows[0],{...rows[1],gpuVsync:{required:true,mode:b.mode,qualified:true}}]]){
  assert.equal(inspectGpuVsyncRun(b,invalid).qualified,false);
 }
});

test('explicit construction rejects pre-existing controlled flags instead of stripping or multiplying them',()=>{
 for(const control of [a,b])for(const arg of ['--disable-gpu-vsync','--disable-gpu-vsync=0','--DISABLE-GPU-VSYNC','--disable-frame-rate-limit','--disable-frame-rate-limit=1']){
  assert.throws(()=>gpuVsyncLaunchArgs(['--enable-gpu',arg],control),/already contains/);
 }
 assert.throws(()=>gpuVsyncLaunchArgs(null,b),/unavailable/);
});
