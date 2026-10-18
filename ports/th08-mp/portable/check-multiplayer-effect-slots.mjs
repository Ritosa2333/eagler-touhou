import { execFileSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { WASI } from 'node:wasi';

const root = resolve(import.meta.dirname, '..');
const cpp = resolve(root, 'th08_web/cpp');
const sdk = process.env.WASI_SDK_PATH ?? [
  resolve(root, '../toolchains/wasi-sdk-34.0-x86_64-windows'),
  resolve(root, '../../toolchains/wasi-sdk-34.0-x86_64-windows'),
].find(path => existsSync(resolve(path, 'bin/clang++.exe')));
if (!sdk) throw new Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');

const compiler = resolve(sdk, 'bin', process.platform === 'win32' ? 'clang++.exe' : 'clang++');
const out = resolve(root, 'artifacts/multiplayer-tests/effect-slots');
mkdirSync(out, { recursive: true });

async function runWasm(wasm) {
  const wasi = new WASI({ version: 'preview1', args: [], env: {}, returnOnExit: true });
  const { instance } = await WebAssembly.instantiate(readFileSync(wasm), {
    wasi_snapshot_preview1: wasi.wasiImport,
  });
  const code = wasi.start(instance);
  if (code !== 0) throw new Error(`effect slot tests failed (${wasm}): ${code}`);
}

for (const [name, define] of [['singleplayer', null], ['multiplayer', 'TH_ENABLE_MULTIPLAYER_GAMEPLAY=1']]) {
  const flags = ['--target=wasm32-wasip1', '-O2', '-std=c++17', '-Wall', '-Wextra', '-Werror', '-Wno-invalid-offsetof',
    '-fno-exceptions', '-fno-rtti',
    '-I' + resolve(cpp, 'game'), '-I' + resolve(cpp, 'multiplayer')];
  if (define) flags.push('-D' + define);
  const testWasm = resolve(out, `${name}.wasm`);
  execFileSync(compiler, [...flags, '-Wl,-z,stack-size=1048576',
    resolve(root, 'tests/player-effect-slots-test.cpp'), '-o', testWasm,
  ], { cwd: root, windowsHide: true, stdio: 'inherit' });

  // Compile the real production owners in each macro lane as well as checking
  // the pool layout and bank routing against its actual EffectPoolState.
  for (const source of ['EffectSystem.cpp', 'EffectBomb.cpp']) {
    execFileSync(compiler, [...flags, '-c', resolve(cpp, 'game', source), '-o', resolve(out, `${name}-${source}.o`)], {
      cwd: root, windowsHide: true, stdio: 'inherit',
    });
  }
  await runWasm(testWasm);
}
console.log('effect slots: PASS (singleplayer and multiplayer)');
