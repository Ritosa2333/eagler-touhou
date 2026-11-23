import assert from 'node:assert/strict';
import test from 'node:test';
import {setImmediate as turn} from 'node:timers/promises';
import {makeCoopRules} from '../runtime/coop-rules.mjs';
import {readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {launchManagedRun} from '../runtime/managed-run.mjs';

for(const stage of [5,16])test(`a sealed clear of stage ${stage} enters slot/name save, never the HTML clear banner`,async t=>{
  const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)},rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'postrun-save',epoch:1,players:2,route:stage===16?'EX':'A',difficulty:stage===16?4:1,challengeMode:false,seed:421,inputDelay:0,rollbackWindow:0,...build});
  const win=new EventTarget(),doc=Object.assign(new EventTarget(),{hidden:false,hasFocus:()=>false}),clock=new Map(),timers=new Map();
  const life=new AbortController(),incoming=[[],[]],errors=[],saved=[[],[]],ending=[],local=[],returned=[],native=[];let id=0,time=0,hostReturned=false;
  for(const [name,value] of Object.entries({window:win,document:doc,requestAnimationFrame:fn=>(clock.set(++id,fn),id),cancelAnimationFrame:key=>clock.delete(key),setTimeout:fn=>(timers.set(++id,fn),id),clearTimeout:key=>timers.delete(key)})){
    const descriptor=Object.getOwnPropertyDescriptor(globalThis,name);Object.defineProperty(globalThis,name,{configurable:true,value});
    t.after(()=>{if(descriptor)Object.defineProperty(globalThis,name,descriptor);else delete globalThis[name];});
  }
  t.after(()=>life.abort());
  const runs=await Promise.all([0,1].map(seat=>launchManagedRun({room:{role:'player',rules,seat},build,canvas:{},signal:life.signal,
    openPauseArt:async()=>({frame(){return null;},postrunFrame(save){return save.records;},close(){}}),
    async openEnding({onComplete}){ending.push([seat,'opened']);let ready=false;return {key(){ready=true;return true;},async advance(){if(ready){ready=false;ending.push([seat,'finished']);onComplete();}},close(){}};},
    async openReplaySave(bytes,{postrun,gameOver,signal}){
      assert.equal(postrun,true);assert.equal(gameOver,false);const tape=await readMultiplayerReplay(bytes,build);
      assert.equal(tape.status().end,'completed');let phase=1;
      return {key({code,down}){signal.throwIfAborted();if(code==='Enter'&&down)phase=2;if(code==='KeyZ'&&down){assert.equal(phase,2);saved[seat].push(tape);phase=5;}return true;},
        frame(){return {phase,slot:0,revision:phase,records:[]};},clear(){}};
    },
    view:{openWorld:async()=>({state:()=>({stageId:stage}),startEnding(){},checkpoint(){return 0;},release(){},restore(){},close(){},step({frame}){if(frame===5)return {cleared:true};},display(){return true;}}),displayReplaySave(menu){native.push([seat,menu.phase]);}},
    openPeer:async(_room,receive)=>({send(x){incoming[1-seat].push(x);},sendSpectator(){},close(){},async pump(){if(seat===1&&hostReturned)throw Error('host left after sealing the complete tape');for(const x of incoming[seat].splice(0))await receive(x,1-seat);return {route:'rtc'};}}),
    present:()=>true,onLocalResult:value=>local.push(value.result),onNativeResult:action=>returned.push([seat,action]),onError:error=>errors.push(error)})));
  async function expect(predicate,message){for(let i=0;i<120;i++){
    const scheduled=[...timers.values()];timers.clear();for(const fn of scheduled)await fn();await turn();
    const render=[...clock.values()];clock.clear();time+=1000/60;for(const fn of render)fn(time);
    assert.deepEqual(errors,[]);if(predicate())return;
  }assert.fail(message);}
  const tap=(seat,code)=>{runs[seat].key({code,down:true});runs[seat].key({code,down:false});};
  try{
    if(stage!==16){await expect(()=>ending.filter(([,k])=>k==='opened').length===2,'original ending did not open on the sealed main clear');
      assert.deepEqual(native,[]);tap(0,'KeyZ');tap(1,'KeyZ');}
    await expect(()=>[0,1].every(seat=>native.some(([i,p])=>i===seat&&p===1)),'replay save slots never opened after clear');
    assert.ok(!local.includes('cleared'),'do not display a redundant co-op clear result');
    tap(0,'Enter');tap(1,'Enter');await expect(()=>[0,1].every(seat=>native.some(([i,p])=>i===seat&&p===2)),'name entry');
    tap(0,'KeyZ');await expect(()=>returned.length===1,'host can save and return first');hostReturned=true;
    await expect(()=>native.some(([seat,phase])=>seat===1&&phase===2),'guest may continue editing their name after the host leaves');
    tap(1,'KeyZ');await expect(()=>returned.length===2,'guest can save the sealed tape and return independently');
    assert.deepEqual(saved.map(array=>array.length),[1,1]);assert.ok(returned.every(([,action])=>action==='end'));
  }finally{runs.forEach(r=>r.close());}
});
