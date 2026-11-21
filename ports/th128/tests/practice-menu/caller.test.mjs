// Real production practice route/caller functions. Native World/start/resource operations are explicit seams.
import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';
import{deferred,flush}from'./surface.mjs';
const source=fs.readFileSync(new URL('../../site/play.mjs',import.meta.url),'utf8').replace(/\r\n/g,'\n');
function extract(name,next){const marker='function '+name+'(',a=source.indexOf(marker),start=source.slice(a-6,a)==='async '?a-6:a,b=source.indexOf('function '+next+'(',a+marker.length),end=source.slice(b-6,b)==='async '?b-6:b;assert(a>=0&&end>start,name+' actual practice page consumer missing');return source.slice(start,end);}
function harness({held=null,tainted=true}={}){
 const calls=[],elements={'#variant':{disabled:false},'#debug-stage-status':{textContent:''}},menu={launching:false,launchId:0},state={stageCatalog:{},module:null,worldMode:false,handoff:false,pendingCarryToken:0,active:false,paused:false,pressed:new Set()},controller=new AbortController();
 const ids={3:'st_a1_3.ecl',5:'st_a2_3.ecl',8:'st_b1_3.ecl',10:'st_b2_3.ecl',13:'st_c1_3.ecl',15:'st_c2_3.ecl',16:'st_ex.ecl'};
 state.module={UTF8ToString:x=>x,_th128_world_state:()=>JSON.stringify({flow:{currentId:state.stageSelection?.stage.id,difficulty:state.stageSelection?.difficulty},debug:{runTainted:tainted},frame:0})};
 const env={state,menu,DOMException,debugRespawnSupported:()=>true,find:s=>elements[s],assertStageStartAllowed(){},resolveStage:(cat,id)=>({id,ecl:ids[id]}),get lastCheckWorldError(){return '';},clock:{reset(){}},resetHighFrameratePresentation(){},syncDebugRespawnUI(){},syncDebugStageJumpControls(){},
 async startLaunch(selection){calls.push(selection);menu.launching=true;menu.launchSelection=selection;++menu.launchId;if(held)await held.promise;menu.launching=false;state.stageSelection=selection;state.worldMode=true;state.active=true;},
 cancelLaunch(){calls.push('cancel');menu.launching=false;++menu.launchId;++state.action;}};
 const text=extract('debugThirdStageSelection','syncDebugStageJumpControls')+extract('prepareStage','startStage')+extract('startDebugThirdStage','startPracticeStage')+extract('startPracticeStage','installPracticeMenu');
 const run=new Function(...Object.keys(env),'let practiceLaunchOwner=null;'+text+';return startPracticeStage;')(...Object.values(env));
 return{run,calls,state,menu,controller,control:{signal:controller.signal,isCurrent:()=>!controller.signal.aborted}};
}
for(const[route,stage,rank]of[['A1-3',3,1],['C2-3',15,1],['EX',16,4]])test('actual page practice consumer starts '+route+' using existing fresh tainted route without carry or unlock',async()=>{
 const h=harness(),receipt=await h.run(route,h.control),selection=h.calls[0];assert.equal(selection.stage.id,stage);assert.equal(selection.difficulty,rank);assert.equal(selection.seed,34772);assert.equal(selection.progress,0);assert.equal(selection.carryToken,undefined);assert.equal(selection.debugJump.normalModeClear,false);
 assert.equal(receipt.nativeTainted,true);assert.equal(receipt.normalModeClear,false);assert.equal(receipt.stageId,stage);
});
test('current page abort cancels only the matching pending launch and drops late native success',async()=>{
 const held=deferred(),h=harness({held}),pending=h.run('A2-3',h.control);h.controller.abort();assert.equal(h.calls.filter(x=>x==='cancel').length,1);held.resolve();await assert.rejects(pending,e=>e.name==='AbortError');
});
test('foreign successor launch is never cancelled by an obsolete practice owner',async()=>{
 const held=deferred(),h=harness({held}),pending=h.run('B1-3',h.control);h.menu.launchSelection={foreign:true};++h.menu.launchId;h.controller.abort();assert.equal(h.calls.includes('cancel'),false);held.resolve();await assert.rejects(pending,e=>e.name==='AbortError');
});
test('actual native taint check rejects a normal-record completion as practice',async()=>{
 const h=harness({tainted:false});await assert.rejects(h.run('A1-3',h.control),/taint|调试跳关/);
});
test('retired owner and invalid route do not invoke a World start',async()=>{
 const h=harness();h.controller.abort();await assert.rejects(h.run('A1-3',h.control),e=>e.name==='AbortError');assert.equal(h.calls.length,0);
 const fresh=harness();await assert.rejects(fresh.run('all-unlocked',fresh.control),/路线|关卡|practice/i);assert.equal(fresh.calls.length,0);await flush();
});
