import assert from 'node:assert/strict';
import {test} from 'node:test';
import * as titleScenes from '../../site/runtime/original-title-scenes.mjs';
const {OriginalTitleDemoClock, originalTitleDemoInputs, originalTitleDemoLimit}=titleScenes;

test('only uninterrupted forward title logic starts the three authored demonstrations in order', () => {
  const clock = new OriginalTitleDemoClock();
  const wait = count => {for (let i = 0; i < count; i++) assert.equal(clock.tick(true, 0), null);};
  wait(1799);
  assert.equal(clock.tick(true, 1), null, 'an input on the trigger tick takes precedence');
  wait(1799);
  assert.equal(clock.tick(false, 0), null, 'Options or a hidden page cannot complete the idle wait');
  wait(1799);
  assert.equal(clock.tick(true, 0).name, 'demo0.rpy');
  for (const name of ['demo1.rpy', 'demo2.rpy', 'demo0.rpy']) {
    wait(1799); assert.equal(clock.tick(true, 0).name, name);
  }
  assert.equal(originalTitleDemoLimit, 3600);
  for (const code of ['KeyZ', 'KeyX', 'Enter', 'Escape']) assert.ok(originalTitleDemoInputs.has(code));
  assert.ok(!originalTitleDemoInputs.has('ArrowLeft'));
});

test('inter-scene transition binds the original Loading ANM without the boot signature and cancels its frame owner', async () => {
  const binds=[],listeners=new Map(),frames=new Map();let serial=0,draws=0;
  const abort=new AbortController();
  const window={requestAnimationFrame(fn){const id=++serial;frames.set(id,fn);return id;},cancelAnimationFrame(id){frames.delete(id);}};
  const document={hidden:false,addEventListener(key,fn){listeners.set(key,fn);},removeEventListener(key){listeners.delete(key);}};
  const module={HEAPU8:new Uint8Array(160),UTF8ToString:()=>'',_th128_error:()=>0,_th128_reset(){},_th128_tick:()=>1,
    _th128_bind(bank,script){binds.push([bank,script]);return 1;},_th128_draw_count:()=>0,_th128_draw_ptr:()=>0,_th128_draw_stride:()=>160};
  const renderer={render(_records,options){assert.ok(!options?.nativeMenu);draws++;}};
  const owner=titleScenes.showOriginalTransition({module,renderer,document,window,signal:abort.signal});
  assert.deepEqual(binds,[[11,22]]);
  const present=timestamp=>{const [id,fn]=frames.entries().next().value;frames.delete(id);fn(timestamp);};
  present(0);present(1000/60);
  let ready=false;owner.ready.then(()=>{ready=true;});
  document.hidden=true;listeners.get('visibilitychange')();present(250);
  await Promise.resolve();assert.equal(ready,false);assert.equal(draws,1);
  document.hidden=false;listeners.get('visibilitychange')();
  present(60000);assert.equal(draws,1,'the time spent hidden must not become loading animation ticks');
  for(let n=1;n<26;n++)present(60000+n*1000/60);
  await owner.ready;assert.ok(draws>=24);assert.equal(frames.size,1);
  owner.close();assert.equal(frames.size,0);assert.equal(listeners.size,0);
});

test('demo attenuates the existing title music bus without touching the effects bus or restarting music', () => {
  const effects={gain:{value:.8}},bgm={gain:{value:.72}};
  const release=titleScenes.duckOriginalDemoMusic(bgm);
  assert.equal(bgm.gain.value,.18);assert.equal(effects.gain.value,.8);
  release();assert.equal(bgm.gain.value,.72);
  bgm.gain.value=.4;release();assert.equal(bgm.gain.value,.4);
});
