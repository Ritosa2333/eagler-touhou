import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {webcrypto} from 'node:crypto';
import {parse} from 'acorn';

// Exercise each title's actual admission functions without starting its DOM,
// audio or Wasm loader. Native calls record the same configuration words.
const root = new URL('../../', import.meta.url);
const builds = [0x11223344, 0x55667788, 0x99aabbcc, 0xddeeff00];
const adapters = {
  th08: ['ports/th08-mp/web/sdl-runtime/multiplayer-host.mjs', ['validateMultiplayerOptions', 'string', 'configureMultiplayer']],
  th10: ['ports/th10-mp/web/sdl-runtime/shell.mjs', ['configureNetplay']],
  th11: ['ports/th11-mp/web/sdl-runtime/multiplayer.mjs', ['validateMultiplayerOptions', 'multiplayerSetupWords']],
  th12: ['ports/th12-mp/src/multiplayer.mjs', ['validateMultiplayerOptions', 'multiplayerSetupWords']],
  th15: ['ports/th15-mp/web/sdl-runtime/multiplayer.mjs', ['validateMultiplayerOptions', 'multiplayerSetupWords']],
};
for (const [game, [file, names]] of Object.entries(adapters)) {
  test(`${game} room identity survives Relay aliases and separates rooms and runs`, async () => {
    const source = await readFile(new URL(file, root), 'utf8');
    const declarations = parse(source, {ecmaVersion: 'latest', sourceType: 'module'}).body
      .map(node => node.declaration ?? node).filter(node => node.type === 'FunctionDeclaration');
    const functions = names.map(name => {
      const node = declarations.find(node => node.id.name === name);
      assert.ok(node, `Missing admission function ${file}: ${name}`);
      return source.slice(node.start, node.end);
    }).join('\n');
    async function configure(url, seat = 0, spectator = false) {
      const options = {netplayMode: 'lan', netplayUrl: url, netplayPlayerCount: 2, netplayPlayer: seat,
        netplaySeed: 1234, netplayDifficulty: 1, netplayLoadouts: [{character: 0, shot: 0}, {character: 1, shot: 0}],
        netplayAdonisMode: 1, netplayInputDelay: 2, netplayInputDelayAuto: false,
        netplayPredictionLimit: game === 'th08' ? 8 : 0, netplayPredictionReserve: 2,
        netplaySpectator: spectator, netplaySpectatorId: 'viewer_1234'};
      let written, connected;
      const memory = new WebAssembly.Memory({initial: 1});
      const connect = (_app, address) => {connected = address; return 1;};
      const core = {memory, allocate: () => 128, deallocate() {}, files_allocate: () => 128, files_free() {},
        multiplayer_configure(_app, address, length) {written = Array.from(new Uint32Array(memory.buffer, address, length)); return 1;},
        multiplayer_connect: connect, multiplayer_spectator_connect: connect};
      const context = {URL, TextEncoder, crypto: webcrypto, options, core, app: 42, runtimeBuildWords: builds,
        multiplayerRuntime: true, window: {}, calibration: null, updateNetplayDiagnostics() {}, cstring: (value, use) => use(value)};
      const entry = names.at(-1);
      const invoke = new Function(...Object.keys(context), functions + `\nreturn ${entry};`)(...Object.values(context));
      if (game === 'th08') await invoke(core, 42, options, {crypto: webcrypto, runtimeBuildWords: builds});
      else if (game === 'th10') {await invoke(); assert.equal(connected, url, 'Transport still uses the selected entry');}
      else written = (await invoke(options, builds, webcrypto)).words;
      return written.slice(5, 7);
    }
    const primary = `wss://primary.test/relay?room=${game}mp-1234&run=17&member=one&player=0`;
    const backup = primary.replace('primary.test/relay', 'backup.test:8443/alternate/relay')
      .replace('member=one&player=0', 'member=two&player=1');
    const expected = await configure(primary);
    assert.deepEqual(await configure(backup, 1), expected, 'Both players must enter the same native session');
    assert.deepEqual(await configure(backup, 0, true), expected, 'Spectators bind to the same room session');
    assert.notDeepEqual(await configure(primary.replace('run=17', 'run=18')), expected);
    assert.notDeepEqual(await configure(primary.replace('1234', '5678')), expected);
  });
}
