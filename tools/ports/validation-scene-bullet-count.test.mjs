import test from 'node:test';
import {resolve} from 'node:path';
import {workspacePath} from '../../launcher/lib/workspace-layout.mjs';
import {runWasiCpp} from '../testing/wasi-cpp.mjs';
const gameDir=workspacePath('th08','web/cpp/game');
test('controlled-world hazard count excludes the real allocator sentinel',()=>runWasiCpp({
  source:resolve(import.meta.dirname,'fixtures/validation-scene-bullet-count.cpp'),
  includes:[gameDir],sources:[resolve(gameDir,'BulletState.cpp')],
}));
