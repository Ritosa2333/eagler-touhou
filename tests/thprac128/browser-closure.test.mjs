import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';
const source=fs.readFileSync(new URL('../../site/play.mjs',import.meta.url),'utf8');
function whole(name,next){const start=source.indexOf(name),end=source.indexOf(next,start);assert(start>=0&&end>start);return source.slice(start,end);}
test('actual page Replay disclosure observes slots after recording owner exists',async()=>{
 const controls={open:false,listeners:{},addEventListener(type,fn){this.listeners[type]=fn;}},status={textContent:''};let observations=0;
 const env={find:s=>s==='#thprac-web-controls'?controls:s==='#thprac-replay-status'?status:{addEventListener(){}},refreshThpracReplaySlots:async()=>{++observations;},saveThpracReplayFromMenu:async()=>{},watchThpracReplayFromMenu:async()=>{}};
 const install=new Function(...Object.keys(env),whole('function refreshOpenPracticeReplay(', '\nfunction installPracticeMenu(')+';return installThpracReplayControls;')(...Object.values(env));install();assert.equal(typeof controls.listeners.toggle,'function');controls.open=true;controls.listeners.toggle();await Promise.resolve();assert.equal(observations,1);controls.open=false;controls.listeners.toggle();assert.equal(observations,1);
});
function startHarness(cancelled=false){
 const state={module:{},stageCatalog:{},active:true,worldMode:false,paused:true},menu={launchId:0,launching:false};let live=true;
 const session={params:{mode:1,stage:0},difficulty:1},selection={stage:{id:1},difficulty:1,thprac:session};
 const env={state,menu,find:()=>({disabled:false}),thprac128RuntimeSupported:()=>true,thpracStageSelection:()=>({selection,session}),inheritThpracTools:()=>{},clock:{reset(){}},resetHighFrameratePresentation(){},syncDebugStageJumpControls(){},readThprac128Native:()=>{throw Error('must not read failedWorld');},DOMException,AbortController,session,selection,moveLive:()=>{live=false;}};
 const run=new Function(...Object.keys(env),"let practiceLaunchOwner=null,lastCheckWorldError='';async function startLaunch(){++menu.launchId;lastCheckWorldError='CONFIGURED_NATIVE_FAILURE';if("+cancelled+")moveLive();}function cancelLaunch(){++menu.launchId;}"+whole('async function startThprac128Session(', '\nasync function thpracReplayResourceIdentity(')+';return startThprac128Session;')(...Object.values(env));
 return{run,session,owner:{signal:new AbortController().signal,isCurrent:()=>live}};
}
test('actual practice start preserves configured failure diagnostic before adoption',async()=>{const h=startHarness();await assert.rejects(h.run(h.session,h.owner),e=>e.name==='Error'&&e.message==='CONFIGURED_NATIVE_FAILURE');});
test('actual cancelled practice owner still rejects late adoption as AbortError',async()=>{const h=startHarness(true);await assert.rejects(h.run(h.session,h.owner),e=>e.name==='AbortError');});
test('production HTML and page omit internal browser fixture control strip',()=>{
 const html=fs.readFileSync(new URL('../../site/play.html',import.meta.url),'utf8');for(const text of ['fixture-controls','fixture-three','集成验证：实际WASM','自然推进音乐ACK','下次原生配置后失败','下次提交前持有资源']){assert(!html.includes(text),text);assert(!source.includes(text),text);}
});
