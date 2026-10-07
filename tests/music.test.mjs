import test from 'node:test';
import assert from 'node:assert/strict';
import {MusicPlayer} from '../src/music.mjs';

// Pinned original StageInfo+10/+14: first-stage road th12_00, Boss th12_02.
// The +34/+38 values 1/2 are music-room indices, not filename suffixes.
const road={id:'th12_00',file:'th12_00.ogg',loopStart:8.5,loopEnd:103.25};
const boss={id:'th12_02',file:'th12_02.ogg',loopStart:4.25,loopEnd:94.5};
function deferred(){let resolve,reject;const promise=new Promise((a,b)=>{resolve=a;reject=b;});return {promise,resolve,reject};}
function audio(decode=async bytes=>({encoded:[...new Uint8Array(bytes)]})){
  const context={destination:{},sources:[],gains:[],decoded:0};
  context.decodeAudioData=bytes=>{context.decoded++;return decode(bytes);};
  context.createGain=()=>{const gain={gain:{value:1},connect(target){this.target=target;}};context.gains.push(gain);return gain;};
  context.createBufferSource=()=>{
    const source={started:0,stopped:0,connect(target){this.target=target;},start(){this.started++;},stop(){this.stopped++;}};
    context.sources.push(source);return source;
  };
  return context;
}

test('road to Boss stops the previous source and retains original loop points',async()=>{
  const context=audio(),player=new MusicPlayer({context,readBytes:async()=>new Uint8Array([1]),volume:.2});
  assert.equal(await player.play(road),true);
  const first=player.source;
  assert.equal(first.loop,true);assert.equal(first.loopStart,road.loopStart);assert.equal(first.loopEnd,road.loopEnd);
  assert.equal(first.target.gain.value,.2);
  assert.equal(await player.play(boss),true);
  assert.equal(first.stopped,1);assert.notEqual(player.source,first);
  assert.equal(player.source.loopStart,boss.loopStart);assert.equal(player.source.loopEnd,boss.loopEnd);
  // An old source's asynchronous ended event cannot clear the new track.
  first.onended();assert.equal(player.source,context.sources[1]);
  player.stop();assert.equal(context.sources[1].stopped,1);assert.equal(player.source,null);
});

test('a slower old road load cannot replace a newer Boss selection',async()=>{
  const slow=deferred(),context=audio(),player=new MusicPlayer({context,readBytes:file=>file===road.file?slow.promise:Promise.resolve(new Uint8Array([2]))});
  const old=player.play(road);
  assert.equal(await player.play(boss),true);
  const current=player.source;
  slow.resolve(new Uint8Array([1]));
  assert.equal(await old,false);assert.equal(player.source,current);
  assert.equal(context.sources.length,1);assert.equal(current.started,1);
});

test('stopping pending music prevents a late load from starting after exit or mute',async()=>{
  const slow=deferred(),context=audio(),player=new MusicPlayer({context,readBytes:()=>slow.promise});
  const pending=player.play(road);player.stop();slow.resolve(new Uint8Array([1]));
  assert.equal(await pending,false);assert.equal(player.source,null);assert.equal(context.sources.length,0);
});

test('current-session predicate is rechecked after audio decoding',async()=>{
  const entered=deferred(),decoded=deferred(),context=audio(()=>{entered.resolve();return decoded.promise;});
  const player=new MusicPlayer({context,readBytes:async()=>new Uint8Array([1])});let current=true;
  const pending=player.play(road,()=>current);await entered.promise;current=false;decoded.resolve({});
  assert.equal(await pending,false);assert.equal(context.sources.length,0);
  assert.equal(await player.play(boss,()=>false),false);assert.equal(context.decoded,1);
});

test('overlapping preloads and play share one read and decode per file',async()=>{
  const slow=deferred(),context=audio(),reads=[];
  const player=new MusicPlayer({context,readBytes:file=>{reads.push(file);return slow.promise;}});
  const first=player.preload([road,road,boss]),second=player.preload([boss,road]),pending=player.play(boss);
  const backing=new Uint8Array([9,1,2,8]);slow.resolve(backing.subarray(1,3));
  await Promise.all([first,second,pending]);
  assert.deepEqual(reads,[road.file,boss.file]);assert.equal(context.decoded,2);
  assert.deepEqual(player.source.buffer.encoded,[1,2]);assert.deepEqual([...backing],[9,1,2,8]);
  await player.preload([road,boss]);assert.equal(context.decoded,2);assert.equal(reads.length,2);
});

test('failed preloads can be retried and errors from superseded playback stay stale',async()=>{
  const context=audio();let attempts=0;
  const player=new MusicPlayer({context,readBytes:async()=>{if(!attempts++)throw Error('missing');return new Uint8Array([1]);}});
  await assert.rejects(player.preload([road]),/missing/);assert.equal(player.pending.size,0);
  assert.equal(await player.play(road),true);assert.equal(attempts,2);
  const slow=deferred(),obsolete=new MusicPlayer({context:audio(),readBytes:()=>slow.promise});
  const pending=obsolete.play(road);obsolete.stop();slow.reject(Error('old failure'));
  assert.equal(await pending,false);
});
