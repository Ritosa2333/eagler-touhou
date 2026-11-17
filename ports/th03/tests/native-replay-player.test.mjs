import test from 'node:test';
import assert from 'node:assert/strict';
import {createNativeReplayPlayer} from '../web/native-replay-player.mjs';

test('playback rates, pause, rewind and bounded full speed preserve input order',async()=>{
 const callbacks=new Map();let id=0,time=0,boots=0,steps=[],latest;
 const old={document:globalThis.document,raf:globalThis.requestAnimationFrame,cancel:globalThis.cancelAnimationFrame};
 globalThis.document={hidden:false};globalThis.requestAnimationFrame=fn=>{callbacks.set(++id,fn);return id;};globalThis.cancelAnimationFrame=n=>callbacks.delete(n);
 const advance=ms=>{time+=ms;const queue=[...callbacks.values()];callbacks.clear();for(const fn of queue)fn(time);};
 try{
  const replay={rows:Array.from({length:300},(_,i)=>i)};
  const player=createNativeReplayPlayer({replay,path:'replay/test.th03r',async boot(){boots++;steps=[];},step(row,index){assert.equal(row,index);steps.push(row);},onState:s=>latest=s,onFrame(){},onError:e=>{throw e;}});
  await player.start();advance(20);advance(1000);const normal=player.state().frame;assert(normal>10&&normal<20);
  await player.control('pause');advance(100);assert.equal(player.state().frame,normal);
  await player.control('seek',2);advance(20);assert.equal(player.state().frame,2);assert.equal(player.state().paused,true);assert.equal(boots,2);
  await player.control('rate',8);await player.control('play');advance(20);advance(100);assert(player.state().frame>30);
  await player.control('rate',0);for(let i=0;i<20&&!player.state().ended;i++)advance(20);assert.equal(player.state().frame,300);assert.equal(latest.ended,true);
  await assert.rejects(player.control('rate',3));player.stop();assert.equal(callbacks.size,0);
 }finally{globalThis.document=old.document;globalThis.requestAnimationFrame=old.raf;globalThis.cancelAnimationFrame=old.cancel;}
});
