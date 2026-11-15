'use strict';
const assert=require('node:assert/strict'),path=require('node:path'),{pathToFileURL}=require('node:url');
const {Env,makeContext,makeQueue}=require('./menu-audio-gate-scenarios.cjs');
const source=path.resolve(process.argv[2]||path.join(__dirname,'../../site/runtime/sound.mjs'));
(async()=>{
 const {MenuAudioGate,OriginalPauseController}=await import(pathToFileURL(source));let cases=0;
 const timer=setTimeout(()=>{throw Error('pause/audio composition stalled');},10000);
 try{
  for(const boundary of ['enter','leave']){
   const env=new Env([boundary==='enter'?'game1.suspend':'menu1.close']),game=makeContext(env,'game1'),queue=makeQueue(env,game);
   const audio=new MenuAudioGate(queue,{createContext:()=>makeContext(env,'menu'+(++env.created))});
   let opens=0,commits=0,resumes=0;
   const module={UTF8ToString:p=>p,_th128_world_error:()=>'',_th128_world_pause_open(){++opens;return 1;},
    _th128_world_pause_tick:()=>1,_th128_world_pause_state:()=>JSON.stringify({active:true,action:1}),_th128_world_pause_commit(){++commits;return 1;}};
   const owner=new OriginalPauseController({module,audio,releaseInput(){},consumeSound:async()=>{},
    onResume(){++resumes;},onTitle(){throw Error('unexpected Title');},onRestart(){throw Error('unexpected restart');}});
   let operation;
   if(boundary==='enter'){operation=owner.open();await env.whenReached('game1.suspend');}
   else{await owner.open();operation=owner.step();await env.whenReached('menu1.close');assert.equal(commits,1);}
   const disposing=owner.dispose();env.release(boundary==='enter'?'game1.suspend':'menu1.close');
   await Promise.all([operation,disposing]);assert.equal(owner.closed,true);assert.equal(owner.active,false);assert.equal(owner.busy,false);
   assert.equal(resumes,0);assert.equal(opens,boundary==='enter'?0:1);assert.equal(commits,boundary==='enter'?0:1);
   assert.equal(queue.context,game);assert.equal(game.state,'running');assert.deepEqual(env.closes,['menu1']);++cases;
  }
  console.log(JSON.stringify({passed:true,cases,actualPauseController:true,actualMenuAudioGate:true,nativeAndAudioContextSynthetic:true,browser:0,WorldStarts:0}));
 }finally{clearTimeout(timer);}
})().catch(error=>{console.error(error.stack);process.exitCode=1;});
