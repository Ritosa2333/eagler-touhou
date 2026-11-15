'use strict';
const assert=require('node:assert/strict'),path=require('node:path'),{pathToFileURL}=require('node:url');
const {Env,makeContext,makeQueue}=require('./menu-audio-gate-scenarios.cjs');
const sound=path.resolve(process.argv[2]||path.join(__dirname,'../../site/runtime/sound.mjs'));
const settled=p=>p.then(()=>({ok:true}),error=>({ok:false,error}));
(async()=>{
 const {MenuAudioGate}=await import(pathToFileURL(sound));
 let cases=0;const timer=setTimeout(()=>{throw Error('audio waiting-owner cancellation stalled');},10000);
 try{
  for(const sameGate of [true,false]){
   const env=new Env(['menu1.close']),game=makeContext(env,'game1'),queue=makeQueue(env,game);
   const createContext=()=>makeContext(env,'menu'+(++env.created)),first=new MenuAudioGate(queue,{createContext});
   await first.enter();const leaving=first.leave();await env.whenReached('menu1.close');
   const next=sameGate?first:new MenuAudioGate(queue,{createContext});
   const opening=settled(next.enter()),cancelled=next.leave();env.release('menu1.close');
   await Promise.all([leaving,cancelled]);const result=await opening;
   assert.equal(result.ok,false,'leave must cancel an entry still waiting for previous cleanup');
   assert.equal(env.created,1,'cancelled waiting entry must not allocate a new menu');
   assert.equal(queue.context,game);assert.equal(game.state,'running');assert.deepEqual(env.closes,['menu1']);++cases;
  }
  {
   const env=new Env(),game=makeContext(env,'game1'),queue=makeQueue(env,game);
   const gate=new MenuAudioGate(queue,{createContext:()=>makeContext(env,'menu'+(++env.created))});
   await gate.enter();await gate.leave();
   const opening=settled(gate.enter());await gate.leave();
   assert.equal((await opening).ok,false,'resolved cleanup still has an async cancellation window');
   assert.equal(env.created,1);assert.equal(queue.context,game);++cases;
  }
  console.log(JSON.stringify({passed:true,cases,actualMenuAudioGate:true,syntheticAudioContext:true,WorldStarts:0,browser:0}));
 }finally{clearTimeout(timer);}
})().catch(error=>{console.error(error.stack);process.exitCode=1;});
