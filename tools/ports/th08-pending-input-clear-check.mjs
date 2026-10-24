import test from 'node:test';
import {resolve} from 'node:path';
import {workspacePath} from '../../launcher/lib/workspace-layout.mjs';
import {runWasiCpp} from '../testing/wasi-cpp.mjs';
const game=workspacePath('th08','web/cpp/game');
test('th08 lifecycle clear cancels only the unconsumed recorded sample and preserves playback alignment',()=>runWasiCpp({
  source:resolve(import.meta.dirname,'fixtures/th08-pending-input-clear.cpp'),includes:[game,workspacePath('common','include')],
  sources:['ReplayRecording.cpp','ReplayStream.cpp','Arithmetic.cpp'].map(name=>resolve(game,name)),
}));
