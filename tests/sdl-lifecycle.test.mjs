import test from 'node:test';
import assert from 'node:assert/strict';
import {sdlStopError} from '../src/sdl-lifecycle.mjs';

test('a normal room exit before a launched first frame is not a crash',()=>{
  assert.equal(sdlStopError(0,'',{source:'menu-quit'},{started:true,logs:['audio: using per-track OGG files']}),null);
});
test('native failures retain their actual reason and pre-teardown state',()=>{
  const details={source:'native-failure',state:{scene:'loading',stage:4}};
  const error=sdlStopError(1,'missing stage04.ecl',details,{started:true,logs:['graphics: WebGL 2']});
  assert.equal(error.message,'missing stage04.ecl');assert.deepEqual(error.cause.nativeStop,details);
  assert.equal(error.cause.exitCode,1);
});
test('unknown exits describe lifecycle status and keep logs as diagnostic data',()=>{
  const error=sdlStopError(1,'',{source:'initialization'},{logs:['audio: using per-track OGG files']});
  assert.match(error.message,/退出码 1/);assert.match(error.message,/initialization/);
  assert(!error.message.includes('audio:'));assert.deepEqual(error.cause.recentLog,['audio: using per-track OGG files']);
  assert(sdlStopError(0,'',{source:'initialization'}));
});
