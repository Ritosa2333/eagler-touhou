import test from 'node:test';import assert from 'node:assert/strict';import {createNativeMusic,musicTrack} from '../web/native-music-player.mjs';
test('native cue mapping keeps stage and alternate track identities',()=>{assert.equal(musicTrack(1,0),'op');assert.equal(musicTrack(2,0),'select');assert.equal(musicTrack(3,7),'dec');assert.equal(musicTrack(4,2),'00mm');assert.equal(musicTrack(5,0),'win');});
test('alternate music responds only to song commands and stops cleanly',async()=>{
 const prev={Audio:globalThis.Audio,fetch:globalThis.fetch},instances=[];globalThis.Audio=class{constructor(){this.paused=true;instances.push(this);}play(){this.paused=false;return Promise.resolve();}pause(){this.paused=true;}removeAttribute(){}load(){}};globalThis.fetch=async()=>({ok:true,json:async()=>({tracks:{'select-m26':{url:'akyu/02.opus',fullLoop:true}}})});
 try{const m=createNativeMusic({source:'akyu'});m.commands(Uint8Array.from([0,2,0,0,1,2,0,0,8,0,0,0]));await new Promise(r=>setTimeout(r,0));assert.equal(instances.length,1);assert.equal(instances[0].src,'./bgm/akyu/02.opus');assert.equal(instances[0].paused,false);m.commands(Uint8Array.from([2,0,0,0]));assert.equal(instances[0].paused,true);m.stop();}finally{globalThis.Audio=prev.Audio;globalThis.fetch=prev.fetch;}
});
test('original music uses its independent PMD source and resumes after pause',async()=>{
 const calls=[],audio={currentTime:0,destination:{},createGain:()=>({gain:{value:1},connect(){}})},source={connect(){calls.push('connect');},start(){calls.push('start');},pause(){calls.push('pause');},stop(){calls.push('stop');}};
 const player={async createSource(bytes){calls.push([...bytes]);return source;},dispose(){calls.push('dispose');}};
 const music=createNativeMusic({readTrack:id=>{assert.equal(id,'select');return Uint8Array.of(1,2);},preparePmd:async()=>player});
 music.commands(Uint8Array.from([0,2,0,0,1,2,0,0]));music.unlock(audio);await new Promise(r=>setTimeout(r,0));
 assert.deepEqual(calls,[[1,2],'connect','start']);assert.equal(music.playing,true);
 music.commands(Uint8Array.from([3,0,0,0,4,0,0,0]));assert.deepEqual(calls.slice(-2),['pause','start']);
 music.stop();assert.deepEqual(calls.slice(-2),['stop','dispose']);
});

test('music and master volume apply independently to the active track',async()=>{
 const previous={Audio:globalThis.Audio,fetch:globalThis.fetch},instances=[];
 globalThis.Audio=class{constructor(){instances.push(this);this.paused=true;}play(){this.paused=false;return Promise.resolve();}pause(){this.paused=true;}removeAttribute(){}load(){}};
 globalThis.fetch=async()=>({ok:true,json:async()=>({tracks:{'select-m26':{url:'akyu/02.opus'}}})});
 try{const m=createNativeMusic({source:'akyu'});m.setMasterVolume(50);m.setMusicVolume(40);m.commands(Uint8Array.from([1,2,0,0]));await new Promise(r=>setTimeout(r,0));assert.ok(Math.abs(instances[0].volume-.14)<1e-9);m.commands(Uint8Array.from([12,0,20,0]));assert.ok(Math.abs(instances[0].volume-.07)<1e-9);m.stop();}finally{globalThis.Audio=previous.Audio;globalThis.fetch=previous.fetch;}
});

test('silent spectator catch-up preserves the current track without playing skipped tracks',async()=>{
 const calls=[],audio={currentTime:0,destination:{},createGain:()=>({gain:{value:1},connect(){}})};
 const player={async createSource(){return {connect(){},start(){calls.push('start');},pause(){calls.push('pause');},stop(){}};},dispose(){}};
 const music=createNativeMusic({outputEnabled:false,readTrack:id=>{calls.push(id);return Uint8Array.of(1);},preparePmd:async()=>player});
 music.unlock(audio);
 music.commands(Uint8Array.of(1,2,0,0,2,0,0,0,1,3,0,0));
 assert.equal(music.track,'00mm');
 assert.equal(music.playing,false);
 assert.deepEqual(calls,[]);
 music.setOutputEnabled(true);
 await new Promise(r=>setTimeout(r,0));
 assert.deepEqual(calls,['00mm','start']);
 music.setOutputEnabled(false);
 assert.equal(music.playing,false);
 music.setOutputEnabled(true);
 assert.deepEqual(calls,['00mm','start','pause','start']);
 music.stop();
});
