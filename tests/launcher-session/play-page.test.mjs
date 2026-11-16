import test from 'node:test';
import assert from 'node:assert/strict';
import {createPageLaunchContext} from '../../site/runtime/page-launch-context.mjs';
import {bootPlay,functionSource} from '../web-input/play-harness.mjs';
function bind(e,config={}){
 const notices=[],c=createPageLaunchContext({config,onPresented:value=>notices.push(value)}),counts={audio:0,ending:0,renderer:0,high:[]};
 const find=()=>({hidden:false}),releaseGameInput=()=>{e.state.pressed.clear();},clock={reset(){}},resetHighFrameratePresentation=()=>{};
 const ctor=new Function('state','menu','getPageLaunchContext','releaseGameInput','clock','resetHighFrameratePresentation','stopAudio','cancelOriginalEnding','setHighFramerateMode','find',
  ['installLauncherRuntime','finishLauncherStartup','noteLauncherPresentation'].map(functionSource).join('\n')+'\nreturn {installLauncherRuntime,finishLauncherStartup,noteLauncherPresentation};');
 const api=ctor(e.state,e.menu,()=>c,releaseGameInput,clock,resetHighFrameratePresentation,()=>counts.audio++,()=>counts.ending++,value=>counts.high.push(value),find);
 e.state.loading=4;e.state.renderer={dispose(){counts.renderer++;}};e.state.controller=new AbortController();
 api.installLauncherRuntime();return{c,api,counts,notices};
}
test('the real bound page input hook uses source ownership while physical Z stays held',async()=>{
 const e=bootPlay(),b=bind(e);b.api.finishLauncherStartup();const runtime=await b.c.waitRuntime();
 e.key('keydown','KeyZ');runtime.keyboard({down:true,code:'KeyZ'});runtime.clearInput();assert(e.state.pressed.has('KeyZ'));
 e.key('keyup','KeyZ');assert.deepEqual(e.receipts,[[1,1,0],[0,0,1]]);e.input.dispose();
});
test('runtime readiness fails when loadVariant did not adopt an active native module',async()=>{
 const e=bootPlay(),b=bind(e);e.state.active=false;b.api.finishLauncherStartup();await assert.rejects(b.c.waitRuntime(),/ready|startup|title/i);e.input.dispose();
});
test('actual page cleanup cancels loading and input and disposes renderer and audio',()=>{
 const e=bootPlay(),b=bind(e);b.api.finishLauncherStartup();b.c.abort();
 assert.equal(e.state.loading,5);assert.equal(e.state.active,false);assert.equal(e.state.launcherExited,true);assert.equal(e.state.controller.signal.aborted,true);
 assert.equal(b.counts.renderer,1);assert.equal(b.counts.audio,1);assert.equal(b.counts.ending,1);b.c.abort();assert.equal(b.counts.renderer,1);
});
test('configuration reaches the real presentation cadence and touch visibility hooks',()=>{
 const e=bootPlay(),b=bind(e,{options:{limitPresentationTo60:true,touchEnabled:false}});
 assert.deepEqual(b.counts.high,[false]);assert.equal(e.input.layer.hidden,true);e.input.dispose();
});
test('first-frame follows a changed actual render serial, never startup completion alone',()=>{
 const e=bootPlay(),b=bind(e);b.api.finishLauncherStartup();assert.deepEqual(b.notices,[]);
 e.state.presentationSerial=4;b.api.noteLauncherPresentation(4);assert.deepEqual(b.notices,[]);
 e.state.presentationSerial=5;b.api.noteLauncherPresentation(4);assert.deepEqual(b.notices,[true]);e.input.dispose();
});
test('a completed exit prevents later physical input from reaching the old runtime',()=>{
 const e=bootPlay(),b=bind(e);b.api.finishLauncherStartup();b.c.abort();const calls=e.receipts.length;e.key('keydown','KeyZ');e.key('keyup','KeyZ');
 assert.equal(e.state.pressed.size,0);assert.equal(e.receipts.length,calls);
});

test('real title draw issues a render serial for nonempty records while preserving the original FPS return value',()=>{
 const e=bootPlay({worldMode:false}),b=bind(e);b.api.finishLauncherStartup();let count=0;const draws=[];
 Object.assign(e.state.module,{HEAPU8:new Uint8Array(16),_th128_draw_count:()=>count,_th128_draw_ptr:()=>0,_th128_draw_stride:()=>16,_th128_frame:()=>1});
 e.state.renderer.render=records=>draws.push(records.length);
 const canvas={dataset:{}},make=new Function('state','syncOriginalFPSPresentation','coreError','decodeDrawBuffer','nativeMenuSnapshot','find',functionSource('draw')+';return draw;');
 const draw=make(e.state,()=>{},()=>'',()=>count?[{id:1}]:[],()=>({page:0,selection:0,ready:1,transitioning:0}),()=>canvas);
 assert.equal(draw(),false);b.api.noteLauncherPresentation(0);assert.deepEqual(b.notices,[]);
 count=1;assert.equal(draw(),false);b.api.noteLauncherPresentation(0);assert.deepEqual(b.notices,[true]);assert.deepEqual(draws,[0,1]);e.input.dispose();
});
