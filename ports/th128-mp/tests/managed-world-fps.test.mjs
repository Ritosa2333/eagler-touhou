import assert from 'node:assert/strict';
import test from 'node:test';
import * as view from '../runtime/managed-view.mjs';
import {originalFPSDraws} from '../../th128/site/runtime/draw.mjs';

test('managed matches measure their own visible draws and keep the original FPS glyphs while paused',()=>{
  assert.equal(typeof view.createManagedFps,'function','a fresh match needs its own local presentation meter');
  const fps=view.createManagedFps();
  assert.deepEqual(fps.present(0),[]);
  for(let n=1;n<40;n++)assert.deepEqual(fps.present(n*25),[]);
  assert.deepEqual(fps.present(1000),originalFPSDraws(40));
  assert.deepEqual(fps.present(4000,true),originalFPSDraws(40),'pause is not a frame-rate sample');
  assert.deepEqual(fps.present(6000),originalFPSDraws(40));
  for(let n=1;n<20;n++)fps.present(6000+n*50);
  assert.deepEqual(fps.present(7000),originalFPSDraws(20));
  assert.deepEqual(view.createManagedFps().present(0),[]);
});
