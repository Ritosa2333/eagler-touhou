'use strict';
const assert=require('node:assert/strict'),path=require('node:path'),{pathToFileURL}=require('node:url');
const {Env,makeContext,makeQueue}=require('./menu-audio-gate-scenarios.cjs');
const source=path.resolve(process.argv[2]||path.join(__dirname,'../../site/runtime/sound.mjs'));
(async()=>{
 const {MenuAudioGate}=await import(pathToFileURL(source));let cases=0;
 for(const existing of [false,true]){
  const env=new Env(),game=makeContext(env,'game1'),queue=makeQueue(env,game);let menu,block=!existing,starts=0;
  const gate=new MenuAudioGate(queue,{createContext:()=>{
   menu=makeContext(env,'menu1');menu.resume=()=>{env.note('menu1','resume');++starts;return block?new Promise(()=>{}):Promise.resolve();};return menu;
  }});
  if(existing){await gate.enter();await gate.suspend();block=true;}
  const opening=gate.enter().then(()=>({ok:true}),error=>({ok:false,error}));
  while(starts<(existing?2:1))await new Promise(resolve=>setImmediate(resolve));
  let completed=false;const leaving=gate.leave().then(()=>{completed=true;});
  for(let turn=0;turn<20&&!completed;++turn)await new Promise(resolve=>setImmediate(resolve));
  assert.equal(completed,true,'retiring must not wait forever for a blocked menu.resume promise');
  await leaving;assert.equal((await opening).ok,false);assert.equal(menu.closeCount,1);assert.equal(queue.context,game);assert.equal(game.state,'running');++cases;
 }
 console.log(JSON.stringify({passed:true,cases,actualMenuAudioGate:true,syntheticBlockedResume:true,WorldStarts:0,browser:0}));
})().catch(error=>{console.error(error.stack);process.exitCode=1;});
