import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
const source=await readFile(new URL('../../sdl-runtime/replay-player.mjs',import.meta.url),'utf8');
const clock=new URL('../../../../../common/browser/playback-clock.mjs',import.meta.url);
const {createReplayPlayer}=await import('data:text/javascript;base64,'+Buffer.from(source.replace('./playback-clock.mjs',clock.href).replace('./auto-replay.mjs',new URL('../../sdl-runtime/auto-replay.mjs',import.meta.url).href)).toString('base64'));

test('actual player rewinds, switches and seeks paused without rebuilding the native core',()=>{
  const callbacks=new Map();let id=0,frame=0,hash=17,restarts=0,time=0;
  const old={raf:globalThis.requestAnimationFrame,cancel:globalThis.cancelAnimationFrame,document:globalThis.document};
  globalThis.document={hidden:false};globalThis.requestAnimationFrame=fn=>{callbacks.set(++id,fn);return id;};globalThis.cancelAnimationFrame=n=>callbacks.delete(n);
  const heap=new ArrayBuffer(4096),errors=[],states=[];
  const core={HEAPU8:new Uint8Array(heap),HEAPU32:new Uint32Array(heap),
    _th09_replay_view_info(){this.HEAPU32.set([1,frame,600,+(frame>=600),1,0,1,9],0);return 0;},
    _th09_file_buffer:()=>1024,_th09_play_replay(){if(this.HEAPU8[1024]===255)return 0;frame=0;hash=17;restarts++;return 1;},
    _th09_replay_view_step(){hash=(Math.imul(hash,31)+(frame++*13)%31)>>>0;return 1;},
    _th09_loop_stop(){},_th09_loop_pause(){},_th09_replay_view_silent(){},_th09_game_draw(){},_th09_audio_pump(){},_th09_suspend(){}};
  const player=createReplayPlayer({core,onState:s=>states.push(s),onError:e=>errors.push(e)});
  const draw=()=>{const [id,fn]=callbacks.entries().next().value;callbacks.delete(id);fn(time+=17);};
  try{
    player.play('replay/th9_01.rpy',new Uint8Array([1]));player.control('pause');player.control('seek',320);draw();const expected=hash;
    assert.equal(frame,320);assert.equal(player.state().paused,true);assert.equal(player.state().seeking,false);
    player.control('seek',90);draw();player.control('seek',320);draw();assert.equal(hash,expected);assert.equal(restarts,2);
    for(let i=0;i<10;i++)draw();assert.equal(frame,320);
    player.play('replay/th9_02.rpy',new Uint8Array([2]));assert.equal(frame,1);assert.equal(restarts,3);assert.equal(callbacks.size,1);
    assert.throws(()=>player.play('bad',new Uint8Array([255])));assert.equal(player.state().path,'replay/th9_02.rpy');
    player.close();assert.equal(callbacks.size,0);assert.equal(player.active,false);assert.equal(errors.length,0);
    assert.ok(states.some(s=>s.path==='replay/th9_01.rpy'&&s.frame===320));
  }finally{globalThis.requestAnimationFrame=old.raf;globalThis.cancelAnimationFrame=old.cancel;globalThis.document=old.document;}
});

test('JSON markers appear immediately; retail Replay discovers exact boundaries and retains them across rewind',()=>{
  const callbacks=new Map();let id=0,frame=0,time=0;
  const old={raf:globalThis.requestAnimationFrame,cancel:globalThis.cancelAnimationFrame,document:globalThis.document};
  globalThis.document={hidden:false};globalThis.requestAnimationFrame=fn=>{callbacks.set(++id,fn);return id;};globalThis.cancelAnimationFrame=n=>callbacks.delete(n);
  const heap=new ArrayBuffer(4096),core={HEAPU8:new Uint8Array(heap),HEAPU32:new Uint32Array(heap),
    _th09_replay_view_info(){this.HEAPU32.set([2,frame,600,+(frame>=600),1,7,14,9,frame>=400?2:frame>=160?1:0]);return 0;},
    _th09_file_buffer:()=>1024,_th09_play_replay(){if(this.HEAPU8[1024]===255)return 0;frame=0;return 1;},
    _th09_replay_view_step(){frame++;return 1;},
    _th09_loop_stop(){},_th09_loop_pause(){},_th09_replay_view_silent(){},_th09_audio_pump(){},_th09_suspend(){}};
  const player=createReplayPlayer({core}),draw=()=>{const [id,fn]=callbacks.entries().next().value;callbacks.delete(id);fn(time+=17);};
  try{
    const match={schema:'eagler-touhou/th09-match-replay/1',names:['a','b'],startedAt:1,frames:600,winner:0,roundStarts:[0,160,400]};
    player.play('new.rpy',new Uint8Array([1]),match);
    assert.deepEqual(player.state().roundStarts,[0,160,400]);assert.equal(player.state().left,7);assert.equal(player.state().right,14);
    player.play('offset.rpy',new Uint8Array([1]),{...match,roundStarts:[0,161,401]});
    player.control('pause');player.control('seek',500);for(let n=0;n<20&&player.state().seeking;n++)draw();
    assert.deepEqual(player.state().roundStarts,[0,160,400],'observing a known round updates its offset instead of adding a duplicate marker');
    player.play('retail.rpy',new Uint8Array([2]));assert.deepEqual(player.state().roundStarts,[0]);
    player.control('pause');player.control('seek',500);for(let n=0;n<20&&player.state().seeking;n++)draw();
    assert.equal(frame,500);assert.deepEqual(player.state().roundStarts,[0,160,400]);
    player.control('seek',90);draw();assert.deepEqual(player.state().roundStarts,[0,160,400]);
    assert.throws(()=>player.play('bad.rpy',new Uint8Array([255])));
    assert.equal(player.state().path,'retail.rpy');assert.deepEqual(player.state().roundStarts,[0,160,400]);
    player.play('other.rpy',new Uint8Array([3]));assert.deepEqual(player.state().roundStarts,[0]);
  }finally{player.close();globalThis.requestAnimationFrame=old.raf;globalThis.cancelAnimationFrame=old.cancel;globalThis.document=old.document;}
});
