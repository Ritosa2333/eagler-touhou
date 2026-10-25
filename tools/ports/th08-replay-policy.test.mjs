import test from 'node:test';
import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {runWasiCpp} from '../testing/wasi-cpp.mjs';
import {TH08_MAX_REPLAY_BATCH} from '../../ports/th08/portable/presentation-lab/replay-capture.mjs';

test('compiled native replay policy bounds overlapping dialogue and fast playback at fifteen ticks',async()=>{
  assert.equal(TH08_MAX_REPLAY_BATCH,15);
  const game=resolve(import.meta.dirname,'../../ports/th08/web/cpp/game');
  await runWasiCpp({source:resolve(import.meta.dirname,'fixtures/th08-replay-policy.cpp'),includes:[game,resolve(import.meta.dirname,'../../common/include')],sources:[resolve(game,'ReplayPlayback.cpp')]});
});
