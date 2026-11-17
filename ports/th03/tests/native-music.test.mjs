import test from 'node:test';import assert from 'node:assert/strict';import {createNativeMusic,musicTrack} from '../web/native-music-player.mjs';
test('native cue mapping keeps stage and alternate track identities',()=>{assert.equal(musicTrack(1,0),'op');assert.equal(musicTrack(2,0),'select');assert.equal(musicTrack(3,7),'dec');assert.equal(musicTrack(4,2),'00mm');assert.equal(musicTrack(5,0),'win');});
test('alternate music responds only to song commands and stops cleanly',async()=>{
 const prev={Audio:globalThis.Audio,fetch:globalThis.fetch},instances=[];globalThis.Audio=class{constructor(){this.paused=true;instances.push(this);}play(){this.paused=false;return Promise.resolve();}pause(){this.paused=true;}removeAttribute(){}load(){}};globalThis.fetch=async()=>({ok:true,json:async()=>({tracks:{'select-m26':{url:'akyu/02.opus',fullLoop:true}}})});
 try{const m=createNativeMusic({source:'akyu'});m.commands(Uint8Array.from([0,2,0,0,1,2,0,0,8,0,0,0]));await new Promise(r=>setTimeout(r,0));assert.equal(instances.length,1);assert.equal(instances[0].src,'./bgm/akyu/02.opus');assert.equal(instances[0].paused,false);m.commands(Uint8Array.from([2,0,0,0]));assert.equal(instances[0].paused,true);m.stop();}finally{globalThis.Audio=prev.Audio;globalThis.fetch=prev.fetch;}
});
test('original music uses its independent PMD source and resumes after pause',async()=>{
 const calls=[],audio={currentTime:0,destination:{}},source={connect(){calls.push('connect');},start(){calls.push('start');},pause(){calls.push('pause');},stop(){calls.push('stop');}};
 const player={async createSource(bytes){calls.push([...bytes]);return source;},dispose(){calls.push('dispose');}};
 const music=createNativeMusic({readTrack:id=>{assert.equal(id,'select');return Uint8Array.of(1,2);},preparePmd:async()=>player});
 music.commands(Uint8Array.from([0,2,0,0,1,2,0,0]));music.unlock(audio);await new Promise(r=>setTimeout(r,0));
 assert.deepEqual(calls,[[1,2],'connect','start']);assert.equal(music.playing,true);
 music.commands(Uint8Array.from([3,0,0,0,4,0,0,0]));assert.deepEqual(calls.slice(-2),['pause','start']);
 music.stop();assert.deepEqual(calls.slice(-2),['stop','dispose']);
});
