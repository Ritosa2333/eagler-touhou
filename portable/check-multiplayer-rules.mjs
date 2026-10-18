import { execFileSync } from 'node:child_process';
import { existsSync, mkdirSync, readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { WASI } from 'node:wasi';

// A rule-owner test, independent of SDL, retail resources and room transport.
// Use the same WASI compilation lane as check-frame-cadence.mjs.
const root = resolve(import.meta.dirname, '..');
const audioOnly = process.argv.includes('--audio-only');
const selectedSuites = new Set();
for (let at=2;at<process.argv.length;++at) {
  if (process.argv[at] === '--audio-only') continue;
  if (process.argv[at] !== '--suite' || !process.argv[at+1] || process.argv[at+1].startsWith('--'))
    throw new Error('Expected --suite NAME or --audio-only');
  selectedSuites.add(process.argv[++at]);
}
const remainingSuites = new Set(selectedSuites);
const sanitized = process.env.TH10_TEST_UBSAN === '1';
if (sanitized && !audioOnly) throw new Error('This sanitizer lane is limited to --audio-only.');
const sanitizerFlags = sanitized ? ['-fsanitize=undefined', '-fsanitize-trap=undefined'] : [];
const audioSuites = new Set(['audio-events', 'audio-native-equivalence', 'dense-item-audio']);
const sdk = process.env.WASI_SDK_PATH ?? [
  resolve(root, '../toolchains/wasi-sdk-34.0-x86_64-windows'),
  resolve(root, '../../toolchains/wasi-sdk-34.0-x86_64-windows'),
].find(path => existsSync(resolve(path, 'bin/clang++.exe')));
if (!sdk) throw new Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');
const compiler = resolve(sdk, 'bin', process.platform === 'win32' ? 'clang++.exe' : 'clang++');
const out = resolve(root, 'artifacts/multiplayer-tests' + (sanitized ? '-audio-ubsan' : ''));
mkdirSync(out, { recursive: true });
const common = resolve(process.env.EAGLER_COMMON_ROOT ?? resolve(root, '../../common'));
const softfloat = resolve(out, 'softfloat.o');
if (sanitized) console.log('Audio-only UBSan: C++ and unmodified SoftFloat instrumented; not a whole-game sanitizer pass.');
execFileSync(compiler, [
  '--target=wasm32-wasip1', '-O2', '-x', 'c', '-std=c11',
  '-DSOFTFLOAT_FAST_INT64', '-DINLINE_LEVEL=5', ...sanitizerFlags, '-c',
  resolve(root, 'web/cpp/rebuild/third_party/softfloat.c'), '-o', softfloat,
], { cwd: root, windowsHide: true, stdio: 'inherit' });
const economySources = [
  'tests/economy-resources-test.cpp', 'web/cpp/game/GameEconomy.cpp',
  'web/cpp/game/Timer.cpp', 'web/cpp/game/Arithmetic.cpp',
].map(path => resolve(root, path));
for (const [name, sources, flags = []] of [
  ['callback-work-timing', [resolve(root,'tests/callback-work-timing-test.cpp')],
    ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['netplay-pacing', [resolve(root,'tests/netplay-pacing-test.cpp'),
    resolve(common,'src/netplay/NetplayCore.cpp'),resolve(common,'src/netplay/NetplayProtocol.cpp')]],
  ['common-session-pacing', [resolve(common,'tests/session-pacing-test.cpp')]],
  ['cooperative-rules', [resolve(root, 'tests/cooperative-rules-test.cpp')]],
  ['hitbox-ownership', [resolve(root,'tests/hitbox-ownership-test.cpp'),
    resolve(common,'src/netplay/RollbackJournal.cpp')]],
  ['multiplayer-balance', [
    resolve(root,'tests/multiplayer-balance-test.cpp'),
    resolve(root,'web/cpp/game/Arithmetic.cpp'),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['session-setup', [
    resolve(root,'tests/multiplayer-session-test.cpp'),
    resolve(root,'web/cpp/multiplayer/SessionSetup.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['input-lanes', [
    resolve(common,'src/netplay/NetplayProtocol.cpp'),
    resolve(root,'tests/multiplayer-input-lanes-test.cpp'),
    resolve(root,'web/cpp/multiplayer/InputLanes.cpp'),
    resolve(root,'web/cpp/game/GameInput.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['multiplayer-session-records', [
    ...['tests/multiplayer-session-records-test.cpp','web/cpp/multiplayer/SessionRecords.cpp',
      'web/cpp/game/ScoreData.cpp','web/cpp/game/Rng.cpp',
      'web/cpp/game/Arithmetic.cpp'].map(path=>resolve(root,path)),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['netplay-runtime', [
    resolve(root,'tests/netplay-runtime-test.cpp'),
    resolve(root,'web/cpp/multiplayer/NetplayRuntime.cpp'),
    resolve(root,'web/cpp/multiplayer/SessionSetup.cpp'),
    resolve(common,'src/netplay/NetplayProtocol.cpp'),
    resolve(common,'src/netplay/NetplayCore.cpp'),
    resolve(common,'src/netplay/NetplaySession.cpp'),
    resolve(common,'src/netplay/SessionChannel.cpp'),
    resolve(common,'src/netplay/BrowserPeerTransport.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['multiplayer-rules-compatibility', [
    resolve(root,'tests/multiplayer-rules-compatibility-test.cpp'),
    resolve(root,'web/cpp/multiplayer/NetplayRuntime.cpp'),
    resolve(root,'web/cpp/multiplayer/SessionSetup.cpp'),
    ...['NetplayProtocol','NetplayCore','NetplaySession','SessionChannel']
      .map(name=>resolve(common,'src/netplay',name+'.cpp')),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['netplay-generation', [
    resolve(root,'tests/netplay-generation-test.cpp'),
    resolve(root,'web/cpp/multiplayer/NetplayRuntime.cpp'),
    resolve(root,'web/cpp/multiplayer/SessionSetup.cpp'),
    ...['NetplayProtocol','NetplayCore','NetplaySession','SessionChannel','BrowserPeerTransport']
      .map(name=>resolve(common,'src/netplay',name+'.cpp')),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['multiplayer-replay-archive', [
    resolve(root,'tests/multiplayer-replay-archive-test.cpp'),
    resolve(root,'web/cpp/multiplayer/ReplayArchive.cpp'),
    resolve(root,'web/cpp/multiplayer/NetplayRuntime.cpp'),
    resolve(root,'web/cpp/multiplayer/SessionSetup.cpp'),
    resolve(root,'web/cpp/game/ApplicationConfig.cpp'),
    ...['InputReplay','NetplayProtocol','NetplayCore','NetplaySession','SessionChannel','BrowserPeerTransport']
      .map(name=>resolve(common,'src/netplay',name+'.cpp')),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['presentation-cache', [resolve(root,'tests/presentation-cache-test.cpp')],
    ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['background-projection-cache', [
    ...['tests/background-projection-cache-test.cpp','web/cpp/game/GraphicsMath.cpp',
      'web/cpp/game/Arithmetic.cpp','web/cpp/game/GameMath.cpp']
      .map(path=>resolve(root,path)),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-ffp-contract=off','-fno-strict-aliasing']],
  ['player-presentation', [resolve(root,'tests/player-presentation-test.cpp')],
    ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['touch-input-frontier', [resolve(root,'tests/touch-input-frontier-test.cpp')],
    ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['camera-math-cache', [
    ...['tests/camera-math-cache-test.cpp','web/cpp/game/GraphicsMath.cpp',
      'web/cpp/game/Arithmetic.cpp','web/cpp/game/GameMath.cpp']
      .map(path=>resolve(root,path)),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-ffp-contract=off','-fno-strict-aliasing']],
  ['common-sparse-pool', [
    resolve(common,'tests/sparse-pool-test.cpp'),
    resolve(common,'src/netplay/RollbackJournal.cpp'),
  ],
    ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['rollback-pool', [
    resolve(root,'tests/rollback-pool-test.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['common-journal-capture-batch', [
    resolve(root,'tests/rollback-journal-capture-batch-test.cpp'),
    resolve(common,'src/netplay/RollbackJournal.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['hint-owner-lifetime', [
    ...['tests/hint-owner-lifetime-test.cpp','web/cpp/game/StageHints.cpp',
      'web/cpp/game/UpdateChain.cpp','web/cpp/game/AnmRegistry.cpp',
      'web/cpp/game/Arithmetic.cpp'].map(path=>resolve(root,path)),softfloat,
    resolve(common,'src/netplay/RollbackJournal.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-DTH_NATIVE_PLATFORM=1']],
  ['scene-owner-lifetime', [
    ...['tests/scene-owner-lifetime-test.cpp','web/cpp/game/ProjectileSystems.cpp',
      'web/cpp/game/GameObjectResources.cpp','web/cpp/game/UpdateChain.cpp']
      .map(path=>resolve(root,path)),
    resolve(common,'src/netplay/RollbackJournal.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-DTH_NATIVE_PLATFORM=1']],
  ['session-effect-owner-lifetime', [
    ...['tests/session-effect-owner-lifetime-test.cpp','web/cpp/game/GameSessionResources.cpp',
      'web/cpp/game/GameSession.cpp','web/cpp/game/GameObjectResources.cpp',
      'web/cpp/game/UpdateChain.cpp','web/cpp/game/Timer.cpp',
      'web/cpp/game/AnmRegistry.cpp','web/cpp/game/Arithmetic.cpp']
      .map(path=>resolve(root,path)),softfloat,
    resolve(common,'src/netplay/RollbackJournal.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-DTH_NATIVE_PLATFORM=1','-fno-strict-aliasing']],
  ['rollback-pool-capture', [
    resolve(root,'tests/rollback-pool-capture-test.cpp'),
    resolve(common,'src/netplay/RollbackJournal.cpp'),
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['audio-events', [
    resolve(root,'tests/multiplayer-audio-events-test.cpp'),
    resolve(root,'web/cpp/multiplayer/AudioEvents.cpp'),
    resolve(root,'web/cpp/game/AudioManager.cpp'),
    resolve(root,'web/cpp/game/Arithmetic.cpp'),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['audio-native-equivalence', [
    resolve(root,'tests/multiplayer-audio-native-equivalence-test.cpp'),
    resolve(root,'web/cpp/multiplayer/AudioEvents.cpp'),
    resolve(root,'web/cpp/game/AudioManager.cpp'),
    resolve(root,'web/cpp/game/Arithmetic.cpp'),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['dense-item-audio', [
    ...['tests/multiplayer-dense-item-audio-test.cpp','web/cpp/multiplayer/AudioEvents.cpp',
      'web/cpp/game/AudioManager.cpp','web/cpp/game/ItemFrame.cpp',
      'web/cpp/game/GameEconomy.cpp','web/cpp/game/Timer.cpp',
      'web/cpp/game/GameMath.cpp','web/cpp/game/Arithmetic.cpp'].map(path=>resolve(root,path)),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['common-simulation-frontier', [
    resolve(common,'tests/simulation-frontier-test.cpp'),
    resolve(common,'src/netplay/NetplayCore.cpp'),
    resolve(common,'src/netplay/NetplayProtocol.cpp'),
  ]],
  ['common-netplay-headers', [resolve(common,'tests/netplay-headers-test.cpp')]],
  ['common-input-replay', [
    resolve(common,'tests/input-replay-test.cpp'),
    ...['InputReplay','NetplayCore','NetplayProtocol'].map(name=>resolve(common,'src/netplay',name+'.cpp')),
  ]],
  ['common-browser-transport', [
    resolve(common,'tests/browser-peer-transport-test.cpp'),
    resolve(common,'src/netplay/BrowserPeerTransport.cpp'),
  ]],
  ['common-netplay-base', [
    resolve(common,'tests/netplay-base-test.cpp'),
    ...['NetplayCore','NetplayProtocol','NetplaySession','WebSocketTransport']
      .map(name=>resolve(common,'src/netplay',name+'.cpp')),
  ]],
  ['common-packet-transaction', [
    resolve(common,'tests/packet-transaction-test.cpp'),
    ...['NetplayCore','NetplayProtocol'].map(name=>resolve(common,'src/netplay',name+'.cpp')),
  ]],
  ['common-session-channel', [
    resolve(common,'tests/session-channel-test.cpp'),
    ...['NetplayCore','NetplayProtocol','NetplaySession','SessionChannel']
      .map(name=>resolve(common,'src/netplay',name+'.cpp')),
  ]],
  ['common-confirmation-window', [
    resolve(common,'tests/confirmation-window-test.cpp'),
    ...['NetplayCore','NetplayProtocol']
      .map(name=>resolve(common,'src/netplay',name+'.cpp')),
  ]],
  ['cooperative-rollback', [
    resolve(root, 'tests/cooperative-rollback-test.cpp'),
    resolve(common, 'src/netplay/NetplayCore.cpp'),
    resolve(common, 'src/netplay/NetplayProtocol.cpp'),
    resolve(common, 'src/netplay/RollbackJournal.cpp'),
  ]],
  ...[false,true].map(multiplayer=>['enemy-native-drops-'+(multiplayer?'multiplayer':'normal'),[
    ...['tests/enemy-native-drops-test.cpp','web/cpp/game/EnemyDrops.cpp',
      'web/cpp/game/GameEconomy.cpp','web/cpp/game/Timer.cpp',
      'web/cpp/game/Rng.cpp','web/cpp/game/GameMath.cpp',
      'web/cpp/game/Arithmetic.cpp'].map(path=>resolve(root,path)),softfloat,
  ],multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[]]),
  ['economy-normal', [...economySources, softfloat]],
  ['economy-multiplayer', [...economySources, softfloat], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['item-ownership', [
    ...['tests/item-ownership-test.cpp','web/cpp/multiplayer/ItemOwnership.cpp',
      'web/cpp/game/ItemFrame.cpp','web/cpp/game/Arithmetic.cpp'].map(path=>resolve(root,path)),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
  ['item-native-resources', [
    ...['tests/item-native-resources-test.cpp','web/cpp/game/ItemFrame.cpp',
      'web/cpp/game/GameEconomy.cpp','web/cpp/game/Timer.cpp',
      'web/cpp/game/GameMath.cpp','web/cpp/game/Arithmetic.cpp'].map(path=>resolve(root,path)),softfloat,
  ], ['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']],
]) {
  if (audioOnly && !audioSuites.has(name)) continue;
  if (selectedSuites.size && !selectedSuites.has(name)) continue;
  remainingSuites.delete(name);
  console.log(`${name}: compile wasm32`);
  const wasm = resolve(out, name + '.wasm');
  execFileSync(compiler, [
    '--target=wasm32-wasip1', '-O2', '-std=c++17', '-Wall', '-Wextra', '-Werror',
    '-fno-exceptions', '-fno-rtti',
    ...sanitizerFlags, ...flags,
    '-Wl,-z,stack-size=1048576',
    '-I' + resolve(common, 'include'),
    '-I' + resolve(common, 'tests/fixtures/include'),
    resolve(root, 'web/cpp/multiplayer/CooperativeRules.cpp'),
    ...sources,
    '-o', wasm,
  ], { cwd: root, windowsHide: true, stdio: 'inherit' });
  console.log(`${name}: run wasm32`);
  const wasi = new WASI({ version: 'preview1', args: [], env: {}, returnOnExit: true });
  const { instance } = await WebAssembly.instantiate(readFileSync(wasm), {
    wasi_snapshot_preview1: wasi.wasiImport,
  });
  const code = wasi.start(instance);
  if (code !== 0) throw new Error(`Cooperative rule tests failed: ${code}`);
  console.log(`${name}: PASS`);
}
if (remainingSuites.size) throw new Error('Unknown or excluded suites: '+[...remainingSuites].join(', '));
