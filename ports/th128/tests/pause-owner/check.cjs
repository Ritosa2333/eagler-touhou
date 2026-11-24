'use strict';
const fs=require('node:fs'),path=require('node:path'),assert=require('node:assert/strict'),vm=require('node:vm');
const {pathToFileURL}=require('node:url');
const {runScenarios}=require('./pause-owner-scenarios.cjs');
const sound=path.resolve(process.argv[2]||path.join(__dirname,'../../site/runtime/sound.mjs'));
const play=path.resolve(process.argv[3]||path.join(__dirname,'../../site/play.mjs'));
const deferred=()=>{let resolve,reject;const promise=new Promise((a,b)=>{resolve=a;reject=b;});return{promise,resolve,reject};};
const results=[];
async function check(name,fn){try{await fn();results.push({name,passed:true});}catch(e){results.push({name,passed:false,error:e.message});}}
function fixture(Controller,{boundary,action=1}={}){
 const entered=deferred(),pending=deferred();let opens=0,ticks=0,commits=0,states=0,consumes=0,leaves=0,callbacks=0;
 let model={active:true,action,ready:true,closing:false,confirm:false,manual:false,replayPhase:0,selection:0,serial:1};
 const delay=async name=>{if(name===boundary){entered.resolve(name);await pending.promise;}};
 const native={UTF8ToString:p=>p,_th128_world_error:()=>'',_th128_world_pause_open(){++opens;return 1;},
  _th128_world_pause_tick(){++ticks;return 1;},_th128_world_pause_state(){++states;return JSON.stringify(model);},
  _th128_world_pause_commit(a){++commits;assert.equal(a,action);model.active=false;return 1;}};
 const owner=new Controller({module:native,releaseInput(){},consumeSound:async()=>{++consumes;await delay(consumes===1?'open-consume':'step-consume');},
  audio:{enter:()=>delay('open-audio'),suspend:async()=>{},leave:async()=>{++leaves;if(leaves===1)await delay('leave');}},
  onPresentation:()=>delay('presentation'),onResume:async()=>{++callbacks;},onTitle:async()=>{++callbacks;},onRestart:async()=>{++callbacks;},
  onReplayOpen:async()=>{++callbacks;}});
 return{owner,entered,pending,counts:()=>({opens,ticks,commits,states,consumes,callbacks}),setAction:v=>{model.action=v;},get callbacks(){return callbacks;},get commits(){return commits;}};
}
async function hostCase({replace=false,reject=false}={}){
 const source=fs.readFileSync(play,'utf8'),start=source.indexOf('async function openOriginalNativePause('),end=source.indexOf('\nfunction handlePauseKey(',start);
 assert(start>=0&&end>start);
 const gate=deferred(),statuses=[],module={},selection={},owner={closed:false,active:false,open:()=>gate.promise};
 const state={module,action:7,stageSelection:selection,worldMode:true,active:true,nativePause:owner,nativePausePending:null};
 const menu={fault:false,pauseOpen:false},noop=()=>{};
 const ctx={state,menu,clock:{reset:noop},resetHighFrameratePresentation:noop,originalMenuClock:{reset:noop},
  find:()=>({hidden:false}),originalPauseConsumer:()=>owner,pendingOriginalEndingRequest:()=>false,status:m=>statuses.push(m)};
 vm.createContext(ctx);vm.runInContext(source.slice(start,end)+'\nthis.run=openOriginalNativePause;',ctx);
 const promise=ctx.run(true),successor=Promise.resolve();
 if(replace){owner.closed=true;state.action++;state.module={};state.nativePause={};state.nativePausePending=successor;}
 if(reject)gate.reject(Error('retired open failure'));else gate.resolve();
 let error;try{await promise;}catch(e){error=e;}
 if(replace){assert.equal(error,undefined,'old open failure must not escape to successor');assert.equal(statuses.length,0,'retired open must not change page status');assert.equal(state.nativePausePending,successor);}
 else{assert.equal(error,undefined);assert.equal(statuses.length,1);assert.equal(state.nativePausePending,null);}
}
(async()=>{
 const deadline=setTimeout(()=>{throw Error('pause owner regression stalled before explicit barrier completed');},10000);
 try{
  const {OriginalPauseController}=await import(pathToFileURL(sound));
  for(const row of await runScenarios(pathToFileURL(sound).href))results.push({name:row.name,passed:row.ok,error:row.ok?undefined:row.detail});
  for(const boundary of ['open-audio','open-consume','step-consume','presentation','leave']){
   await check('disposed rejection '+boundary,async()=>{
    const f=fixture(OriginalPauseController,{boundary});
    let run;if(boundary.startsWith('open-'))run=f.owner.open();else{await f.owner.open();run=f.owner.step();}
    const observed=run.then(()=>({}),error=>({error}));
    await f.entered.promise;await f.owner.dispose();const baseline=f.counts();
    f.pending.reject(Error('obsolete '+boundary));const outcome=await observed;
    assert.equal(outcome.error,undefined,'retired rejection must not escape');assert.deepEqual(f.counts(),baseline);assert.equal(f.owner.active,false);assert.equal(f.owner.busy,false);
   });
  }
  for(const action of [1,2,3,6])await check('live action '+action,async()=>{
   const f=fixture(OriginalPauseController,{action});await f.owner.open();await f.owner.step();assert.equal(f.commits,1);assert.equal(f.callbacks,1);assert.equal(f.owner.active,false);assert.equal(f.owner.busy,false);await f.owner.dispose();
  });
  await check('live errors remain visible',async()=>{
   const f=fixture(OriginalPauseController,{boundary:'open-audio'}),run=f.owner.open();const observed=run.catch(e=>e);
   await f.entered.promise;f.pending.reject(Error('current audio error'));assert.match((await observed).message,/current audio error/);assert.equal(f.owner.busy,false);await f.owner.dispose();
  });
  await check('current opening still reports status',()=>hostCase());
  await check('replaced opening suppresses success status',()=>hostCase({replace:true}));
  await check('replaced opening suppresses stale rejection',()=>hostCase({replace:true,reject:true}));
  for(const row of results)console.log(JSON.stringify(row));
  const failed=results.filter(x=>!x.passed);console.log(JSON.stringify({passed:failed.length===0,cases:results.length,failed:failed.length,wholeProductionController:true,wholeProductionOpenCaller:true,nativeAudioDOMSynthetic:true,WorldStarts:0,browser:0}));
  if(failed.length)process.exitCode=1;
 }finally{clearTimeout(deadline);}
})().catch(e=>{console.error(e.stack);process.exitCode=2;});
