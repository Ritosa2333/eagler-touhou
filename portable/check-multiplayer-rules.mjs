import { execFileSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { WASI } from 'node:wasi';

// A rule-owner test, independent of SDL, retail resources and room transport.
// Use the same WASI compilation lane as check-frame-cadence.mjs.
const root = resolve(import.meta.dirname, '..');
const sdk = process.env.WASI_SDK_PATH ?? [
  resolve(root, '../toolchains/wasi-sdk-34.0-x86_64-windows'),
  resolve(root, '../../toolchains/wasi-sdk-34.0-x86_64-windows'),
].find(path => existsSync(resolve(path, 'bin/clang++.exe')));
if (!sdk) throw new Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');
const compiler = resolve(sdk, 'bin', process.platform === 'win32' ? 'clang++.exe' : 'clang++');
const out = resolve(root, 'artifacts/multiplayer-tests');
mkdirSync(out, { recursive: true });
const common = resolve(root, 'third_party/eagler-common');
const softfloat = resolve(out, 'softfloat.o');
execFileSync(compiler, [
  '--target=wasm32-wasip1', '-O2', '-x', 'c', '-std=c11',
  '-DSOFTFLOAT_FAST_INT64', '-DINLINE_LEVEL=5', '-c',
  resolve(root, 'th10_web/cpp/rebuild/third_party/softfloat.c'), '-o', softfloat,
], { cwd: root, windowsHide: true, stdio: 'inherit' });
const economySources = [
  'tests/economy-resources-test.cpp', 'th10_web/cpp/game/GameEconomy.cpp',
  'th10_web/cpp/game/Timer.cpp', 'th10_web/cpp/game/Arithmetic.cpp',
].map(path => resolve(root, path));
for (const [name, sources, flags = []] of [
  ['cooperative-rules', [resolve(root, 'tests/cooperative-rules-test.cpp')]],
  ['multiplayer-balance', [
    resolve(root,'tests/multiplayer-balance-test.cpp'),
    resolve(root,'th10_web/cpp/game/Arithmetic.cpp'),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['session-setup', [
    resolve(root,'tests/multiplayer-session-test.cpp'),
    resolve(root,'th10_web/cpp/multiplayer/SessionSetup.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['input-lanes', [
    resolve(common,'src/netplay/NetplayProtocol.cpp'),
    resolve(root,'tests/multiplayer-input-lanes-test.cpp'),
    resolve(root,'th10_web/cpp/multiplayer/InputLanes.cpp'),
    resolve(root,'th10_web/cpp/game/GameInput.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['cooperative-rollback', [
    resolve(root, 'tests/cooperative-rollback-test.cpp'),
    resolve(common, 'src/netplay/NetplayCore.cpp'),
    resolve(common, 'src/netplay/NetplayProtocol.cpp'),
    resolve(common, 'src/netplay/RollbackJournal.cpp'),
  ]],
  ['economy-normal', [...economySources, softfloat]],
  ['economy-multiplayer', [...economySources, softfloat], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['item-ownership', [
    ...['tests/item-ownership-test.cpp','th10_web/cpp/multiplayer/ItemOwnership.cpp',
      'th10_web/cpp/game/ItemFrame.cpp','th10_web/cpp/game/Arithmetic.cpp'].map(path=>resolve(root,path)),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['item-native-resources', [
    ...['tests/item-native-resources-test.cpp','th10_web/cpp/game/ItemFrame.cpp',
      'th10_web/cpp/game/GameEconomy.cpp','th10_web/cpp/game/Timer.cpp',
      'th10_web/cpp/game/GameMath.cpp','th10_web/cpp/game/Arithmetic.cpp'].map(path=>resolve(root,path)),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
]) {
  const wasm = resolve(out, name + '.wasm');
  execFileSync(compiler, [
    '--target=wasm32-wasip1', '-O2', '-std=c++17', '-Wall', '-Wextra', '-Werror',
    '-fno-exceptions', '-fno-rtti',
    ...flags,
    '-Wl,-z,stack-size=1048576',
    '-I' + resolve(common, 'include'),
    '-I' + resolve(common, 'tests/fixtures/include'),
    resolve(root, 'th10_web/cpp/multiplayer/CooperativeRules.cpp'),
    ...sources,
    '-o', wasm,
  ], { cwd: root, windowsHide: true, stdio: 'inherit' });
  const wasi = new WASI({ version: 'preview1', args: [], env: {}, returnOnExit: true });
  const { instance } = await WebAssembly.instantiate(readFileSync(wasm), {
    wasi_snapshot_preview1: wasi.wasiImport,
  });
  const code = wasi.start(instance);
  if (code !== 0) throw new Error(`Cooperative rule tests failed: ${code}`);
  console.log(`${name}: PASS`);
}
