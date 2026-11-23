import assert from 'node:assert/strict';
import {test} from 'node:test';
import {setImmediate as yieldTurn} from 'node:timers/promises';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {launchManagedRun} from '../runtime/managed-run.mjs';
import {createTh128SpectatorDecoder} from '../runtime/spectator-wire.mjs';

const build={coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64)},matchId='th128mp-34772-1';
const rules=createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId,epoch:1,mode:'parallel-stage',route:'A1-3',
  seed:34772,inputDelay:0,rollbackWindow:2,...build});

for(const natural of [false,true])test(`two managed players keep a ${natural?'completed':'restart'} tail identical to the spectator stream and Replay`,async t=>{
  const targets=[new AbortController(),new AbortController()],errors=[],timers=new Map(),frames=new Map(),datagrams=[[],[]],
    worlds=[[],[]],rendered=[[],[]],first=[[],[]],archives=[[],[]],uploaded=[],closed=[0,0],sent=[];
  let serial=0,elapsed=0,connected=false,held=null,delayRemote=true;
  const doc=Object.assign(new EventTarget(),{hidden:false,hasFocus:()=>false});
  const globals={window:new EventTarget(),document:doc,setTimeout:fn=>{timers.set(++serial,fn);return serial;},clearTimeout:id=>timers.delete(id),
    requestAnimationFrame:fn=>{frames.set(++serial,fn);return serial;},cancelAnimationFrame:id=>frames.delete(id)};
  const original=Object.fromEntries(Object.keys(globals).map(key=>[key,Object.getOwnPropertyDescriptor(globalThis,key)]));
  for(const [key,value] of Object.entries(globals))Object.defineProperty(globalThis,key,{value,writable:true,configurable:true});
  t.after(()=>{
    for(const owner of targets)owner.abort();
    for(const [key,value] of Object.entries(original)){
      if(value)Object.defineProperty(globalThis,key,value);else delete globalThis[key];
    }
  });

  const canvases=[{},{}];
  const controls=await Promise.all([0,1].map(seat=>launchManagedRun({room:{role:'player',seat,rules},canvas:canvases[seat],build,signal:targets[seat].signal,
    async openPeer(_room,receive){
      return {send(packet){
        const row=structuredClone(packet);sent.push({seat,row});
        if(delayRemote&&seat===1&&row.epoch===1&&row.frame===1){assert.equal(held,null);held=row;}
        else datagrams[1-seat].push(row);
      },sendSpectator(row){assert.equal(seat,0);uploaded.push(structuredClone(row));},async pump(){
        if(connected)for(const row of datagrams[seat].splice(0))await receive(row,1-seat);
        return {route:connected?'rtc':'connecting'};
      },close(){closed[seat]++;}};
    },view:{openWorld(selection){
      const stub={epoch:selection.epoch,rows:[],restores:[],live:true,tokens:new Set()};worlds[seat].push(stub);
      return {checkpoint(){const token={frame:stub.rows.length};stub.tokens.add(token);return token;},restore(token){
        assert.ok(stub.tokens.has(token));stub.restores.push(token.frame);stub.rows.length=token.frame;
      },release(token){assert.ok(stub.tokens.delete(token));},step(row){
        assert.equal(row.frame,stub.rows.length);stub.rows.push(row.inputs.map(input=>input.held));
        if(natural&&stub.epoch===1&&row.frame>=4)return {bothGameOver:true};
      },display(current,confirmed){
        assert.equal(current,seat);assert.ok(confirmed<=stub.rows.length);rendered[seat].push({epoch:stub.epoch,confirmed});return true;
      },close(){assert.equal(stub.live,true);stub.live=false;}};
    }},present(actual,path){first[seat].push({actual,path});return true;},
    onError(error){errors.push(error);},onReplay(bytes){archives[seat].push(bytes);}})));

  const assertAlive=()=>assert.deepEqual(errors,[]);
  async function network(){
    const batch=[...timers.values()];timers.clear();
    for(const run of batch)await run();
    await yieldTurn();assertAlive();
  }
  function raf(increment=1000/60){
    elapsed+=increment;const batch=[...frames.values()];frames.clear();
    assert.equal(batch.length,2);for(const draw of batch)draw(elapsed);assertAlive();
  }
  async function until(predicate,label){
    for(let i=0;i<60;i++){if(predicate())return;await network();}
    assert.fail(label);
  }
  function key(seat,code,down,repeat=false){return controls[seat].key({code,down,repeat});}
  function saw(seat,epoch,confirmed){return rendered[seat].some(row=>row.epoch===epoch&&row.confirmed===confirmed);}

  await network();raf();assert.deepEqual(first,[[],[]]);
  connected=true;
  await until(()=>sent.filter(({row})=>row.schema==='th128-mp-ready/1'&&row.kind==='ack').length===2,'players never exchanged both ready acknowledgements');
  await network();raf();
  assert.deepEqual(first,[[{actual:true,path:'rtc'}],[{actual:true,path:'rtc'}]]);
  function localKey(code,down,ctrlKey=true){
    const event=new Event(down?'keydown':'keyup',{cancelable:true});
    for(const [name,value] of Object.entries({target:canvases[0],code,ctrlKey,isTrusted:true,repeat:false}))
      Object.defineProperty(event,name,{value});
    window.dispatchEvent(event);return event.defaultPrevented;
  }
  assert.equal(localKey('ControlRight',true),false);
  assert.equal(localKey('ControlRight',false,false),false);
  assert.equal(key(1,'ControlRight',true),false);
  assert.equal(key(1,'ControlRight',false),false);
  assert.equal(localKey('ControlLeft',true),true);
  assert.equal(localKey('ArrowLeft',true),true);
  assert.equal(localKey('KeyR',true),false,'Ctrl+R remains a browser shortcut');
  raf();await network();await network();raf(0);
  assert.deepEqual(worlds.map(seat=>seat[0].rows),[[[576,0]],[[576,0]]]);
  assert.equal(localKey('ArrowLeft',false),true);
  assert.equal(localKey('ControlLeft',false,false),true);
  key(1,'ArrowRight',true);
  raf();await network();await network();assert.equal(held?.held,128);
  raf();await network();await network();
  assert.ok(worlds[0][0].rows.length>=2);assert.equal(worlds[0][0].rows[1][1],0,'the missing packet was actually predicted');
  // Keep the oldest missing input ahead of queued later packets on delivery.
  delayRemote=false;datagrams[0].unshift(held);held=null;
  await network();await network();raf(0);
  assert.deepEqual(worlds[0][0].restores,[1]);assert.equal(worlds[0][0].rows[1][1],128);
  key(1,'ArrowRight',false);key(1,'KeyZ',true);
  const oldConfirmed=rendered[0].at(-1).confirmed;
  if(natural){
    for(let i=0;i<12&&!archives.every(rows=>rows.length===1);i++){raf();await network();await network();}
    assert.ok(archives.every(rows=>rows.length===1),'round did not emit both live Replay archives');
    assert.equal(uploaded.filter(packet=>ArrayBuffer.isView(packet)&&packet[3]===0x53&&packet[6]===3).length,1,'host emitted exactly one End');
    const count=sent.length;raf();await network();await network();assert.equal(sent.length,count);
  }
  key(0,'KeyR',true);key(0,'KeyR',true,true);
  await network();assert.deepEqual(archives.map(items=>items.length),natural?[1,1]:[0,0]);assert.equal(worlds[0][0].live,true);
  key(1,'KeyR',true);
  await until(()=>archives.every(rows=>rows.length===1)&&worlds.every(seat=>seat.length===2&&!seat[0].live),
    'mutual restart never activated both new Worlds with both sealed Replay copies');
  assert.deepEqual(archives[0],archives[1]);
  const replay=await readMultiplayerReplay(archives[0][0],build),end=replay.status();
  assert.equal(end.end,natural?'completed':'restart');assert.ok(end.nextFrame>=oldConfirmed&&end.nextFrame>=2);
  if(natural)assert.equal(end.nextFrame,5);
  assert.deepEqual(replay.read(1).inputs.map(input=>input.held),[0,128]);

  await until(()=>uploaded.filter(row=>!ArrayBuffer.isView(row)).length===2,'spectator did not receive the new checkpoint');
  const decoder=createTh128SpectatorDecoder({build,matchId});let old=null,ends=0;
  for(const row of uploaded){
    if(!ArrayBuffer.isView(row)){decoder.announce(row);continue;}
    const kind=await decoder.accept(row.subarray(1));
    if(kind==='end'){old=decoder.retain();ends++;}
  }
  assert.ok(old);assert.equal(ends,1);assert.deepEqual(old.status(),end);
  for(let frame=0;frame<end.nextFrame;frame++)assert.deepEqual(old.read(frame),replay.read(frame));
  assert.equal(decoder.status().epoch,2);
  // Fresh direction events are received after the confirmed epoch switch,
  // but before this tab gets its next animation callback. The old held shot
  // must be dropped while these new inputs survive.
  key(0,'ArrowRight',true);key(1,'ArrowLeft',true);
  raf();raf();await network();await network();raf(0);
  assert.ok(saw(0,2,1)&&saw(1,2,1));
  assert.deepEqual(worlds.map(rows=>rows[1].rows[0]),[[128,64],[128,64]]);
  assert.deepEqual(archives.map(rows=>rows.length),[1,1]);
  for(const control of controls)control.close();
  assert.deepEqual(closed,[1,1]);assert.equal(frames.size,0);assert.equal(timers.size,0);
  assert.ok(worlds.flat().every(stub=>!stub.live&&stub.tokens.size===0));
});
