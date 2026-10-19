// Independent codec build; this script does not run the game or any tests.
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';

const root = path.resolve(import.meta.dirname, '..');
const candidates = [process.env.EMSDK, path.resolve(root, '../tools/emsdk'), path.resolve(root, '../../tools/emsdk')].filter(Boolean);
const sdk = candidates.find(value => fs.existsSync(path.join(value, 'upstream/emscripten/emcc.py')));
const compiler = process.env.EMCC_PYTHON_SCRIPT || (sdk && path.join(sdk, 'upstream/emscripten/emcc.py'));
if (!compiler || !fs.existsSync(compiler)) throw Error('Activate Emscripten 4.0.22 and set EMSDK; see docs/BUILD.md.');
const bundledPython = sdk && path.join(sdk, 'python/3.13.3_64bit/python.exe');
const python = process.env.EMSDK_PYTHON || process.env.PYTHON ||
  (bundledPython && fs.existsSync(bundledPython) ? bundledPython : 'python');
const output = path.join(root, 'site/runtime/audio');
const cache = path.join(root, 'artifacts/emscripten-cache');
const temporary = path.join(root, 'artifacts/emscripten-temp/audio');
const linked = path.join(temporary, 'linked');
for (const directory of [output, cache, temporary, linked]) fs.mkdirSync(directory, {recursive:true});
const exported = ['malloc', 'free', 'th12_vorbis_open', 'th12_vorbis_close', 'th12_vorbis_length',
  'th12_vorbis_rate', 'th12_vorbis_channels', 'th12_vorbis_pcm', 'th12_vorbis_max_frames',
  'th12_vorbis_seek', 'th12_vorbis_read', 'th12_vorbis_error'].map(name => '_' + name);
const environment = {...process.env, EM_CACHE:cache, EMSDK_PYTHON:python,
  EMCC_TEMP_DIR:temporary, TMP:temporary, TEMP:temporary, TMPDIR:temporary,
  PATH:path.dirname(python) + path.delimiter + process.env.PATH};
if (!environment.EM_CONFIG && sdk && fs.existsSync(path.join(sdk, '.emscripten'))) environment.EM_CONFIG = path.join(sdk, '.emscripten');
const result = spawnSync(python, [compiler, 'src/audio/vorbis-decoder.cpp', '-std=c++17', '-O2',
  '-o', path.join(linked, 'vorbis.mjs'), '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=web,worker',
  '-sALLOW_MEMORY_GROWTH=1', '-sINITIAL_MEMORY=16777216', '-sMAXIMUM_MEMORY=268435456',
  '-sSTACK_SIZE=1048576', '-sFILESYSTEM=0', '-sABORTING_MALLOC=0', '-sASSERTIONS=1',
  '-sEXPORTED_FUNCTIONS=' + JSON.stringify(exported),
  '-sEXPORTED_RUNTIME_METHODS=' + JSON.stringify(['HEAPU8', 'HEAPF32'])],
{cwd:root, stdio:'inherit', env:environment});
if (result.error) throw result.error;
if (result.status !== 0) process.exit(result.status || 1);
// Keep compiler intermediates in its own writable directory. Promote only a
// completed pair, using the same file owner as the rest of the Node build.
for (const name of ['vorbis.mjs', 'vorbis.wasm']) fs.copyFileSync(path.join(linked, name), path.join(output, name));
