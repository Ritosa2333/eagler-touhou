// Independent WASI gate for the prospective TH07-style shared SFX PCM cache.
// Reads real WAV fixtures; does not start a browser or invoke an original executable.
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { readFileSync, writeFileSync, mkdirSync, existsSync, openSync, closeSync } from 'node:fs';
import { resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { createHash } from 'node:crypto';
import { WASI } from 'node:wasi';

const root = fileURLToPath(new URL('../../', import.meta.url));
const fixtureRoot = resolve(process.env.TH09_FIXTURE_ROOT ?? resolve(root, '..'));
const fixtures = resolve(fixtureRoot, 'web/reference/assets');
const sdk = process.env.WASI_SDK_BIN;
assert(sdk, 'Set WASI_SDK_BIN to the workspace WASI SDK bin');
const compiler = resolve(sdk, process.platform === 'win32' ? 'clang++.exe' : 'clang++');
assert(existsSync(compiler), 'WASI compiler is unavailable');
const label = process.env.RUN_LABEL ?? 'audio-sfx-pcm-prospective-01';
assert(/^[a-zA-Z0-9_-]+$/.test(label), 'Safe immutable artifact label');
const directory = resolve(root, 'artifacts/multiplayer-tests');
mkdirSync(directory, { recursive: true });
const wasm = resolve(directory, label + '.wasm'), stdout = resolve(directory, label + '-stdout.json'),
  stderr = resolve(directory, label + '-stderr.txt'), output = resolve(directory, label + '-report.json');
for (const filename of [wasm, stdout, stderr, output]) assert(!existsSync(filename), 'Retained artifact already exists: ' + filename);
const source = resolve(root, 'tests/multiplayer/audio-sfx-pcm.cpp');
const definitions = resolve(root, 'cpp/game/SoundEffects.cpp');
const sourceText = readFileSync(definitions, 'utf8');
const names = [...sourceText.matchAll(/"(se_[^"]+\.wav)"/g)].map(match => match[1]);
assert.equal(names.length, 39);
assert.equal(new Set(names).size, 39);
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const assets = names.map(name => {
  const file = resolve(fixtures, name), bytes = readFileSync(file);
  return { name, bytes: bytes.length, sha256: sha(bytes) };
});
const flags = ['-std=c++17', '-O2', '-g', '-ffp-contract=off', '-fno-exceptions', '-fno-rtti',
  '-Wl,-z,stack-size=1048576'];
const build = spawnSync(compiler, [...flags, source, definitions, '-o', wasm], { encoding: 'utf8', windowsHide: true });
if (build.error) throw build.error;
assert.equal(build.status, 0, build.stdout + build.stderr);
const bytes = readFileSync(wasm), outFd = openSync(stdout, 'wx'), errFd = openSync(stderr, 'wx');
let exit, result;
try {
  const wasi = new WASI({ version: 'preview1', args: ['audio-sfx-pcm'], env: {},
    preopens: { '/fixtures': fixtures }, stdout: outFd, stderr: errFd, returnOnExit: true });
  const { instance } = await WebAssembly.instantiate(bytes, { wasi_snapshot_preview1: wasi.wasiImport });
  exit = wasi.start(instance);
} finally {
  closeSync(outFd); closeSync(errFd);
}
try { result = JSON.parse(readFileSync(stdout, 'utf8')); }
catch (error) { result = { passed: false, parseError: String(error), rawOutput: readFileSync(stdout, 'utf8') }; }
const compilerVersion = spawnSync(compiler, ['--version'], { encoding: 'utf8', windowsHide: true });
const evidence = {
  passed: exit === 0 && result.passed === true, exit,
  scope: 'Prospective f32 stereo 44100 Hz full-decode PCM buffer versus the current per-voice decoder. Actual 39 WAV assets and all 54 logical voices. A component correctness comparison, not a browser/release performance result or physical audio continuity measurement. No production source is changed by this script.',
  limitations: [
    'All comparisons are exact float-bit and frame/cursor/status checks, including natural EOF and seek/retrigger. Differences are reported, not converted into a tolerance.',
    'Nonzero seek cases deliberately test the resampler-reset boundary of a plain PCM buffer; they may reject this prospective strategy.',
    'SFX mixing uses actual SoundEffects definitions and command batching with deterministic overlapping voices. BGM and the SDL device queue are not exercised here.',
    'Decoded PCM bytes count payload only. ma_decode_memory can retain allocation slack; its component elapsed time is not a live game startup measurement.',
    'This executable models the prospective data-source replacement directly. Any eventual production adapter also needs its own contract gate and live correctness checks.'
  ],
  compiler: compilerVersion.stdout.trim(), flags, fixtures, assets,
  identities: {
    runner: sha(readFileSync(fileURLToPath(import.meta.url))), test: sha(readFileSync(source)),
    soundDefinitions: sha(readFileSync(definitions)),
    soundHeader: sha(readFileSync(resolve(root, 'cpp/game/SoundEffects.hpp'))),
    audioBaseline: sha(readFileSync(resolve(root, 'cpp/sdl/AudioDevice.cpp'))),
    miniaudio: sha(readFileSync(resolve(root, '../portable/sdl/third_party/miniaudio.h'))),
    wasm: sha(bytes)
  },
  result, stderr: readFileSync(stderr, 'utf8')
};
writeFileSync(output, JSON.stringify(evidence, null, 2) + '\n', { flag: 'wx' });
console.log(JSON.stringify({ output, passed: evidence.passed, exit,
  decodedPcmBytes: result.decodedPcmBytes, componentFullDecodeElapsedMs: result.componentFullDecodeElapsedMs,
  comparisons: result.comparisons }, null, 2));
if (!evidence.passed) process.exitCode = 1;
