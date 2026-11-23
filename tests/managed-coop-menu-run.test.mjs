import assert from 'node:assert/strict';
import test from 'node:test';
import {setImmediate as turn} from 'node:timers/promises';
import {makeCoopRules} from '../runtime/coop-rules.mjs';
import {readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {launchManagedRun} from '../runtime/managed-run.mjs';

test('two managed seats paint local selection only at the shared fence, restart, then expose only the newly sealed result',async t=>{
  const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)},rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'menu-run',epoch:1,players:2,route:'A',difficulty:1,challengeMode:false,seed:421,inputDelay:0,rollbackWindow:0,...build});
  const window=new EventTarget(),document=Object.assign(new EventTarget(),{hidden:false,hasFocus:()=>false,body:{}});
  const frames=new Map(),timers=new Map(),life=new AbortController(),queues=[[],[]],packets=[],results=[[],[]],archives=[[],[]],saveMenus=[[],[]],errors=[],displays=[[],[]],inputs=[[],[]];
  let id=0,time=0;
  for(const [name,value] of Object.entries({window,document,requestAnimationFrame:fn=>(frames.set(++id,fn),id),cancelAnimationFrame:id=>frames.delete(id),setTimeout:fn=>(timers.set(++id,fn),id),clearTimeout:id=>timers.delete(id)})){
    const descriptor=Object.getOwnPropertyDescriptor(globalThis,name);Object.defineProperty(globalThis,name,{configurable:true,value});
    t.after(()=>{if(descriptor)Object.defineProperty(globalThis,name,descriptor);else delete globalThis[name];});
  }
  t.after(()=>life.abort());
  const canvases=[{},{}],runs=await Promise.all([0,1].map(seat=>launchManagedRun({room:{role:'player',rules,seat},build,canvas:canvases[seat],signal:life.signal,
    async openReplaySave(bytes,{gameOver,signal}){
      assert.equal(gameOver,false);const tape=await readMultiplayerReplay(bytes,build);
      saveMenus[seat].push(tape);let done=false;
      return {key({code,down}){signal.throwIfAborted();if(code==='Escape'&&down)done=true;return true;},frame(){return {phase:done?5:1,slot:0,revision:1,records:[]};},clear(){}};
    },
    openPauseArt:async()=>({frame:state=>state&&{revision:1,records:[],...state},reset(){},close(){}}),
    view:{openWorld:async({epoch})=>({checkpoint(){return 0;},release(){},restore(){},close(){},step(row){inputs[seat].push(row);},
      display(_s,frame,_rev,menu){displays[seat].push({epoch,frame,menu:menu??null});return true;}})},
    openPeer:async(_room,receive)=>({send(p){packets.push(p);queues[1-seat].push(p);},sendSpectator(){},close(){},async pump(){for(const p of queues[seat].splice(0))await receive(p,1-seat);return {route:'rtc'};}}),
    present:()=>true,onLocalResult:value=>results[seat].push(value),onReplay:bytes=>archives[seat].push(bytes),onError:e=>errors.push(e)})));
  function paint(){time+=1000/60;const batch=[...frames.values()];frames.clear();for(const fn of batch)fn(time);assert.deepEqual(errors,[]);}
  async function network(){const batch=[...timers.values()];timers.clear();for(const fn of batch)await fn();await turn();assert.deepEqual(errors,[]);}
  const last=i=>displays[i].at(-1);
  async function until(predicate,message){for(let n=0;n<100;n++){await network();paint();if(predicate())return;}assert.fail(message);}
  const key=(i,code,down,repeat=false)=>runs[i].key({code,down,repeat});
  const tap=(i,code)=>{key(i,code,true);key(i,code,false);};
  function local(code,down,target){const e=new Event(down?'keydown':'keyup',{cancelable:true});for(const [k,v] of Object.entries({code,target,isTrusted:true,repeat:false}))Object.defineProperty(e,k,{value:v});window.dispatchEvent(e);return e.defaultPrevented;}
  try{
    await until(()=>last(0)?.frame>=2&&last(1)?.frame>=2,'ready');
    assert.ok(local('Escape',true,canvases[0]));key(0,'Escape',true);paint();assert.equal(last(0).menu,null,'no private pause pending peer');
    await until(()=>last(0).menu&&last(1).menu,'confirmed pause did not reach either canvas');
    const frame=last(0).frame;assert.equal(last(1).frame,frame);key(0,'Escape',true,true);key(0,'Escape',false);
    assert.equal(local('ArrowDown',true,{}),false,'DOM control owns its key');tap(1,'ArrowDown');paint();assert.equal(last(1).menu.selection,'end');assert.equal(last(0).menu.selection,'resume');
    key(0,'KeyZ',true);await until(()=>!last(0).menu&&!last(1).menu,'peer resume');key(0,'KeyZ',true,true);
    await until(()=>last(0).frame>frame&&last(1).frame>frame,'resumed');assert.ok(inputs[0].slice(frame).every(row=>row.inputs.every(s=>s.held===0)),'no menu input in subsequent confirmed tape');key(0,'KeyZ',false);
    tap(0,'KeyR');await until(()=>last(0).menu?.confirm,'R must wait for shared pause / Yes');assert.ok(!packets.some(p=>p.action==='restart'));tap(0,'ArrowUp');tap(0,'Enter');
    await until(()=>last(0).epoch===2&&last(1).epoch===2&&last(0).frame>=1,'new epoch');assert.ok(archives.every(a=>a.length===1));assert.ok(results.flat().every(r=>!r.result&&!r.replayBytes),'restart is not a result');
    tap(1,'Escape');await until(()=>last(1).menu?.selection==='resume','fresh menu');tap(1,'ArrowDown');tap(1,'ArrowDown');tap(1,'Enter');tap(1,'ArrowDown');tap(1,'Enter');
    await until(()=>results.every(r=>r.at(-1)?.result==='left'),'sealed local results');
    for(const list of results){const {epoch,replayBytes}=list.at(-1),tape=await readMultiplayerReplay(replayBytes,build);assert.equal(epoch,2);assert.equal(tape.header.rules.epoch,2);assert.equal(tape.status().end,'left');}
    await until(()=>last(1).menu?.replaySave?.phase===1,'in-game save slot menu did not open for the requester');
    assert.equal(saveMenus[0].length,0);assert.equal(saveMenus[1].length,1);
    assert.equal(last(0).menu,null);assert.equal(last(1).frame,last(0).frame,'no new native frames are played in the save menu');
    tap(1,'Escape');await until(()=>!last(1).menu,'cancel returns to the sealed result without saving');
  }finally{for(const run of runs)run.close();}
  assert.equal(timers.size,0);assert.equal(frames.size,0);
});
