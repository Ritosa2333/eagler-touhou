'use strict';
const assert=require('node:assert/strict'),path=require('node:path'),{pathToFileURL}=require('node:url');
(async()=>{
 const {OriginalPauseController}=await import(pathToFileURL(path.resolve(__dirname,'../../site/runtime/sound.mjs')).href);
 let total=0;
 for(const action of [1,2,3,6])for(const retain of [false,true]){
  const calls=[];let current=true,entered,release;const entry=new Promise(r=>entered=r),held=new Promise(r=>release=r);
  const module={UTF8ToString:x=>x,_th128_world_error:()=>'',_th128_world_pause_open:()=>1,
   _th128_world_pause_tick:()=>{calls.push('tick');return return1();},_th128_world_pause_state:()=>JSON.stringify({active:true,action}),
   _th128_world_pause_commit:a=>{calls.push('commit:'+a);return 1;}};
  function return1(){return 1;}
  const controller=new OriginalPauseController({module,isCurrent:()=>current,releaseInput:()=>calls.push('release'),
   consumeSound:async()=>{entered();await held;},audio:{async enter(){},async leave(){calls.push('leave');},async suspend(){}},
   onResume:()=>calls.push('resume'),onTitle:()=>calls.push('title'),onRestart:()=>calls.push('restart')});
  controller.active=true;const step=controller.step();await entry;if(!retain)current=false;release();await step;
  const expected=retain?['tick','commit:'+action,'release','leave',action<=2?'resume':action===3?'title':'restart']:['tick'];
  assert.deepEqual(calls,expected,'current-owner barrier before native action'+action);total++;
 }
 console.log(JSON.stringify({passed:true,total,scope:'actualcontroller current-owner barrier before actions1/2/3/6; explicitnative/audio seams'}));
})().catch(e=>{console.error(e);process.exitCode=1;});
