import test from 'node:test';
import {resolve} from 'node:path';
import {workspacePath} from '../../launcher/lib/workspace-layout.mjs';
import {runWasiCpp} from '../testing/wasi-cpp.mjs';
test('th08 production lookup and glyph raster paths emit bounded lossless observations',()=>runWasiCpp({
  source:resolve(import.meta.dirname,'fixtures/language-observation.cpp'),
  includes:[workspacePath('th08','web/cpp/game'),workspacePath('common','include')],
}));
