import assert from 'node:assert/strict';import test from 'node:test';import {openCoopSound} from '../runtime/coop-sound.mjs';
test('a confirmed frame dispatches the original SE operations once, honours suspended contexts without a stale burst, and closes only this room',async()=>{
 let closed=0,disposed=0;const calls=[],gain={connect(){},disconnect(){}},ctx={state:'suspended',createGain:()=>gain,destination:{},resume(){this.state='running';return Promise.resolve();},close(){closed++;return Promise.resolve();}};const events=new EventTarget(),bindings={sounds:{}};
 const q={async load(b,base){assert.deepEqual(b,bindings);assert.equal(base,'https://local.invalid/assets/');},consumeNativeRequest(id,x,op,{present}){calls.push([id,x,op,present]);},flush({present}){calls.push(present);},dispose(){disposed++;}};
 const menu={...ctx},quiet={async load(){},dispose(){}};let contexts=0,queues=0;
 const scope=new AbortController(),audio=await openCoopSound({resources:{base:'https://local.invalid/assets/',async read(k){assert.equal(k,'soundBindings');return new TextEncoder().encode(JSON.stringify(bindings));}},signal:scope.signal,events,createContext:()=>contexts++?menu:ctx,makeQueue:()=>queues++?quiet:q});
 const frames=[{id:16,x:4,op:0},{id:31,x:3,op:1}];audio.commit(frames);assert.deepEqual(calls,[[16,4,0,false],[31,3,1,false],false]);events.dispatchEvent(new Event('pointerdown'));audio.commit([{id:33,x:0,op:0}]);assert.deepEqual(calls.slice(3),[[33,0,0,true],true]);scope.abort();audio.close();audio.commit(frames);assert.equal(disposed,1);assert.equal(closed,2);assert.equal(calls.length,5);
});

test('original OGG stage/boss switches and native fade are committed on timeline events, not on simulation',async()=>{
 const events=new EventTarget(),ctx={state:'running',createGain:()=>({connect(){},disconnect(){}}),destination:{},close(){return Promise.resolve();}},scope=new AbortController(),commands=[];
 const resources={base:'https://room.invalid/assets/',musicTracks:['a','b'],async read(k){return new TextEncoder().encode(JSON.stringify(k==='atlas'?{stages:[{id:1,music:['a','b']}]}:{sounds:{}}));}};
 const music={async load(_tracks,_base,_resolve,opts){commands.push(['load',...opts.tracks]);},switchTo(n){commands.push(n);},fadeOutFrames(n){commands.push(['fade',n]);},dispose(){commands.push('dispose');}},queue={async load(){},dispose(){}};
 const sound=await openCoopSound({resources,signal:scope.signal,events,createContext:()=>ctx,makeQueue:()=>queue,musicEnabled:true,makeMusic:()=>music});sound.commit(null,{musicTrack:[1,0]});sound.commit(null,{musicTrack:[1,1]});sound.commit(null,{musicFade:120});sound.close();assert.deepEqual(commands,[['load','a','b'],'a','b',['fade',120],'dispose']);
});
test('shared pause freezes the stage clock but original menu clips remain audible; terminal stops that source',async()=>{
 const calls=[],lanes=[],owner=new AbortController(),events=new EventTarget();
 const createContext=()=>{
  const id=lanes.length,ctx={state:'running',destination:{},currentTime:8,createGain:()=>({connect(){},disconnect(){},gain:{value:1}}),
   async suspend(){this.state='suspended';calls.push(['suspend',id]);},async resume(){this.state='running';calls.push(['resume',id]);},async close(){this.state='closed';}};
  lanes.push(ctx);return ctx;
 };
 const nativeIDs=Object.fromEntries([7,9,10,14,42].map(id=>[id,{name:`se_${id}.wav`}])),$=x=>new TextEncoder().encode(JSON.stringify(x));
 const resources={musicTracks:[{name:'a'}],async read(key){return $(key==='atlas'?{stages:[{id:1,music:['a']}]}:{sounds:nativeIDs});}};
 const sound=await openCoopSound({resources,signal:owner.signal,events,musicEnabled:true,createContext,
   makeMusic(ctx){return {async load(){},switchTo(name){calls.push(['track',lanes.indexOf(ctx),name]);},stopSource(){calls.push(['stop',lanes.indexOf(ctx)]);},dispose(){}};},
   makeQueue(ctx){const i=lanes.indexOf(ctx);return {async load(b){calls.push(['load',i,Object.keys(b.sounds)]);},consumeNativeRequest(id,x,op,{present}){calls.push(['se',i,id,present]);},flush(){},dispose(){}};}
 });
 try{
  sound.commit(null,{musicTrack:[1,0]});sound.setBattlePhase('paused');assert.deepEqual(lanes.map(x=>x.state),['suspended','running']);
  assert.deepEqual(calls.filter(c=>c[0]==='load').map(c=>c[2]),[['7','9','10','14','42'],['7','9','10','14']]);
  sound.menuCue(14);events.dispatchEvent(new Event('keydown'));sound.menuCue(10);sound.setBattlePhase('paused');
  assert.equal(lanes[0].state,'suspended','menu key must never unlock the gameplay audio while paused');
  assert.deepEqual(calls.filter(c=>c[0]==='se'),[['se',1,14,true],['se',1,10,true]]);
  sound.menuCue(7);sound.setBattlePhase('playing');assert.equal(lanes[0].state,'running');assert.equal(lanes[0].currentTime,8);
  assert.deepEqual(calls.filter(c=>c[0]==='track'),[['track',0,'a']],'resume keeps the original source rather than restarting it');
  sound.setBattlePhase('stopped');assert.equal(lanes[0].state,'suspended');assert.equal(calls.filter(c=>c[0]==='stop').length,1);
  sound.menuCue(9);assert.deepEqual(calls.filter(c=>c[0]==='se').at(-1),['se',1,9,true]);
  sound.setBattlePhase('playing');sound.commit(null,{musicTrack:[1,0]});assert.equal(calls.filter(c=>c[0]==='track').length,2,'fresh round starts a fresh source');
 }finally{owner.abort();}
 assert.ok(lanes.every(ctx=>ctx.state==='closed'));
});
