import assert from 'node:assert/strict';
import test from 'node:test';
import {runInNewContext} from 'node:vm';
import {controlTouchMotionClock,touchClockRequest} from './adapters/touch-clock.mjs';

const clockIn=context=>{
  const browser=runInNewContext(`(${controlTouchMotionClock.toString()})`,context);
  return request=>browser(touchClockRequest(request));
};

test('scene clock uses existing production tick only on explicit steps and restores RAF',()=>{
  const calls=[],core={sdl_loop_stop(){calls.push('stop');},sdl_loop_tick(...args){calls.push(args);return 0;},
    sdl_loop_pause(value){calls.push(['pause',value]);},sdl_loop_start(){calls.push('start');}},context={__th08Runtime:{app:42,core}};
  const run=clockIn(context);
  assert.equal(run({game:'th08',action:'begin'}).mode,'native-manual');
  assert.deepEqual(calls,['stop']);
  assert.equal(run({game:'th08',action:'tick'}).ticks,1);
  assert.deepEqual(calls[1],[42,1/60,0]);
  assert.throws(()=>run({game:'th08',action:'begin'}),/already owns/);
  assert.throws(()=>run({game:'th07',action:'tick'}),/another Runtime/);
  assert.equal(run({game:'th08',action:'end'}).resumed,true);
  assert.equal(calls.at(-1),'start');
  assert.deepEqual(calls.at(-2),['pause',0],'stopping the production loop also pauses audio, so ending the scene must resume it');
  assert.equal(context.__eaglerTouchMotionClock,undefined);
});

test('missing adapters retain the running production clock and failures remain visible',()=>{
  const context={},run=clockIn(context);
  assert.equal(run({game:'th06',action:'begin'}).mode,'realtime');
  let resumed=false;
  context.__th08Runtime={app:4,core:{sdl_loop_stop(){},sdl_loop_tick(){return 2;},sdl_loop_start(){resumed=true;}}};
  run({game:'th08',action:'begin'});
  assert.throws(()=>run({game:'th08',action:'tick'}),/ended during touch scene \(2\)/);
  run({game:'th08',action:'end'});assert.equal(resumed,true);
});

test('title clock signatures preserve th10 application ownership and th20 seconds argument',()=>{
  for(const [game,app,tickArgs,startArgs] of [
    ['th10',71,[71,1/60,0],[71]],
    ['th20',undefined,[1/60,0],[]],
  ]){
    const calls=[],core={sdl_loop_stop(){},sdl_loop_tick(...args){calls.push(['tick',args]);return 0;},
      sdl_loop_start(...args){calls.push(['start',args]);}};
    const context={[`__${game}Runtime`]:{app,core}},run=clockIn(context);
    assert.equal(run({game,action:'begin'}).mode,'native-manual');
    run({game,action:'tick'});run({game,action:'end'});
    assert.deepEqual(calls,[['tick',tickArgs],['start',startArgs]]);
  }
});

test('unknown signatures never stop a running game and failed restoration retains ownership',()=>{
  let stops=0;
  const core={sdl_loop_stop(){stops++;},sdl_loop_tick(){return 0;},sdl_loop_start(){throw Error('resume failed');}};
  const context={__th06Runtime:{app:1,core},__th10Runtime:{app:2,core}},run=clockIn(context);
  assert.equal(run({game:'th06',action:'begin'}).mode,'realtime');assert.equal(stops,0);
  assert.throws(()=>run({game:'th10',action:'typo'}),/Unknown touch scene clock action/);
  run({game:'th10',action:'begin'});
  assert.throws(()=>run({game:'th10',action:'end'}),/resume failed/);
  assert.ok(context.__eaglerTouchMotionClock,'restoration may be retried by the current owner');
});
