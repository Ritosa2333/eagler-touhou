'use strict';
const assert=require('node:assert/strict'),path=require('node:path'),{pathToFileURL}=require('node:url');
const {runAll,Env,makeContext,makeQueue}=require('./menu-audio-gate-scenarios.cjs');
const source=path.resolve(process.argv[2]||path.join(__dirname,'../../site/runtime/sound.mjs'));
const settled=p=>p.then(()=>({ok:true}),error=>({ok:false,error}));
const rows=[];
async function check(name,fn){try{await fn();rows.push({name,ok:true});}catch(e){rows.push({name,ok:false,error:e.message});}}
(async()=>{
 const deadline=setTimeout(()=>{throw Error('audio gate regression did not finish pending transition');},10000);
 try{
  const url=pathToFileURL(source).href,{MenuAudioGate}=await import(url);
  rows.push(...await runAll(url));
  for(const sameGate of [true,false])await check('close blocks successor enter: '+(sameGate?'same':'new')+' gate',async()=>{
   const env=new Env(['menu1.close']),game=makeContext(env,'game1'),queue=makeQueue(env,game);
   const createContext=()=>makeContext(env,'menu'+(++env.created)),first=new MenuAudioGate(queue,{createContext});
   await first.enter();env.markRetired();const leaving=settled(first.leave());await env.whenReached('menu1.close');
   const next=sameGate?first:new MenuAudioGate(queue,{createContext});const entering=settled(next.enter());
   await Promise.resolve();await Promise.resolve();
   assert.equal(env.created,1,'new menu must wait for prior close/resume completion');
   env.release('menu1.close');assert.equal((await leaving).ok,true);assert.equal((await entering).ok,true);
   assert.equal(queue.context.name,'menu2');assert.equal(first.menu?.name,sameGate?'menu2':undefined);
   await next.leave();assert.equal(queue.context,game);assert.deepEqual(env.closes,['menu1','menu2']);
  });
  await check('suspend state settles late before retirement restores game',async()=>{
   const env=new Env(['game1.suspend']),game=makeContext(env,'game1'),queue=makeQueue(env,game);
   game.suspend=async()=>{env.note('game1','suspend');await env.await('game1.suspend');game.state='suspended';};
   const gate=new MenuAudioGate(queue,{createContext:()=>makeContext(env,'menu'+(++env.created))});
   const entering=settled(gate.enter());await env.whenReached('game1.suspend');
   const leaving=settled(gate.leave());env.release('game1.suspend');
   assert.equal((await entering).ok,false);assert.equal((await leaving).ok,true);
   assert.equal(queue.context,game);assert.equal(game.state,'running');assert.deepEqual(env.closes,['menu1']);
  });
  await check('concurrent leaves clean one menu exactly once',async()=>{
   const env=new Env(['menu1.close']),game=makeContext(env,'game1'),queue=makeQueue(env,game);
   const gate=new MenuAudioGate(queue,{createContext:()=>makeContext(env,'menu'+(++env.created))});await gate.enter();
   const a=gate.leave();await env.whenReached('menu1.close');const b=gate.leave();env.release('menu1.close');await Promise.all([a,b]);
   assert.equal(queue.context,game);assert.deepEqual(env.closes,['menu1']);assert.equal(game.state,'running');
  });
  await check('current entry errors stay visible after owned cleanup',async()=>{
   const env=new Env(['menu1.resume']),game=makeContext(env,'game1'),queue=makeQueue(env,game);
   const gate=new MenuAudioGate(queue,{createContext:()=>makeContext(env,'menu'+(++env.created))});
   const entering=settled(gate.enter());await env.whenReached('menu1.resume');env.fail('menu1.resume',Error('real menu resume failure'));
   const result=await entering;assert.equal(result.ok,false);assert.match(result.error.message,/real menu resume failure/);
   assert.equal(queue.context,game);assert.equal(game.state,'running');assert.deepEqual(env.closes,['menu1']);
  });
  await check('resuming existing menu cannot succeed after leave',async()=>{
   const env=new Env(),game=makeContext(env,'game1'),queue=makeQueue(env,game);
   const gate=new MenuAudioGate(queue,{createContext:()=>makeContext(env,'menu'+(++env.created))});await gate.enter();
   await gate.suspend();env.hold('menu1.resume');
   const reenter=settled(gate.enter());await Promise.resolve();const leaving=settled(gate.leave());
   env.release('menu1.resume');assert.equal((await reenter).ok,false);assert.equal((await leaving).ok,true);
   assert.equal(queue.context,game);assert.deepEqual(env.closes,['menu1']);
  });
  for(const row of rows)console.log(JSON.stringify(row));
  const failed=rows.filter(r=>!r.ok).length;
  console.log(JSON.stringify({passed:failed===0,cases:rows.length,failed,actualMenuAudioGate:true,syntheticAudioContext:true,WorldStarts:0,browser:0}));
  if(failed)process.exitCode=1;
 }finally{clearTimeout(deadline);}
})().catch(e=>{console.error(e.stack);process.exitCode=2;});
