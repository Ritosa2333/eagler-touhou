import { execFileSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { WASI } from 'node:wasi';

// Exercise TH08's MP resource owner independently of SDL, retail assets and
// transport, using the repository's WASI lane for deterministic C++ checks.
const root = resolve(import.meta.dirname, '..');
const sdk = process.env.WASI_SDK_PATH ?? [
  resolve(root, '../toolchains/wasi-sdk-34.0-x86_64-windows'),
  resolve(root, '../../toolchains/wasi-sdk-34.0-x86_64-windows'),
].find(path => existsSync(resolve(path, 'bin/clang++.exe')));
if (!sdk) throw new Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');

const compiler = resolve(sdk, 'bin', process.platform === 'win32' ? 'clang++.exe' : 'clang++');
const cCompiler = resolve(sdk, 'bin', process.platform === 'win32' ? 'clang.exe' : 'clang');
const out = resolve(root, 'artifacts/multiplayer-tests');
const wasm = resolve(out, 'player-resources.wasm');
mkdirSync(out, { recursive: true });
const softfloat = resolve(root, 'th08_web/cpp/third_party/softfloat.c');
const softfloatObject = resolve(out, 'player-resources-softfloat.o');
execFileSync(cCompiler, [
  '--target=wasm32-wasip1', '-O2', '-std=c11', '-DSOFTFLOAT_FAST_INT64',
  '-DINLINE_LEVEL=5', '-c', softfloat, '-o', softfloatObject,
], { cwd: root, windowsHide: true, stdio: 'inherit' });
execFileSync(compiler, [
  '--target=wasm32-wasip1', '-O2', '-std=c++17', '-Wall', '-Wextra', '-Werror',
  '-fno-exceptions', '-fno-rtti', '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1',
  '-Wl,-z,stack-size=1048576',
  '-I' + resolve(root, 'th08_web/cpp/multiplayer'),
  '-I' + resolve(root, 'th08_web/cpp/game'),
  resolve(root, 'th08_web/cpp/game/Arithmetic.cpp'),
  resolve(root, 'th08_web/cpp/game/GameGauge.cpp'),
  resolve(root, 'th08_web/cpp/game/GameValues.cpp'),
  resolve(root, 'th08_web/cpp/game/Rng.cpp'),
  resolve(root, 'th08_web/cpp/multiplayer/PlayerResources.cpp'),
  resolve(root, 'tests/player-resources-test.cpp'),
  softfloatObject,
  '-o', wasm,
], { cwd: root, windowsHide: true, stdio: 'inherit' });

const wasi = new WASI({ version: 'preview1', args: [], env: {}, returnOnExit: true });
const { instance } = await WebAssembly.instantiate(readFileSync(wasm), {
  wasi_snapshot_preview1: wasi.wasiImport,
});
const code = wasi.start(instance);
if (code !== 0) throw new Error(`Player resource tests failed: ${code}`);
console.log('player-resources: PASS');
