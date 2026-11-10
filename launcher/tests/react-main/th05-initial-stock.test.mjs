import {test, beforeAll as before, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import {mkdir, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let work, api;

before(async () => {
  await mkdir(resolve(project, '.cache'), {recursive: true});
  work = await mkdtemp(resolve(project, '.cache/react-main-th05-stock-'));
  const outfile = resolve(work, 'actual-owners.mjs');
  await build({
    absWorkingDir: project,
    stdin: {resolveDir: project, loader: 'ts', contents: `
      export {TH05_INITIAL_STOCK, validTh05InitialSpares, validTh05InitialBombs} from './src/launcher/th05-initial-stock.mts';
      export {buildMultiplayerRuntimeOptions} from './src/launcher/multiplayer-runtime-options.mts';
      export {normalizeMultiplayerLobbySnapshot} from './src/launcher/multiplayer-lobby-snapshot.mts';
      export {createMultiplayerRoomSessionStore} from './src/launcher/multiplayer-room-session.mts';
      export {createMultiplayerRoom} from './app/services/multiplayer-room.ts';
      export {multiplayerConfigForProduct} from '../common/contracts/product-catalog.mjs';
    `},
    outfile,
    bundle: true,
    platform: 'node',
    format: 'esm',
    packages: 'external',
    logLevel: 'silent',
    plugins: [authoredSourcesPlugin(project)],
  });
  api = await import(pathToFileURL(outfile).href);
});

after(async () => {
  if (work) await rm(work, {recursive: true, force: true});
});

function runtimeInput(product, patch = {}) {
  const config = api.multiplayerConfigForProduct(product);
  return {
    url: `wss://relay.invalid/socket?room=${product}-1234&run=1`,
    player: 0,
    playerCount: 2,
    seed: 1234,
    difficulty: 1,
    adonisMode: 0,
    predictionLimit: config.inputTiming?.sendPredictionLimit ?? config.inputTiming?.rollbackLimit ?? 8,
    spectator: false,
    spectatorId: '',
    spectatorCount: 0,
    iceServers: [],
    loadouts: config.loadouts,
    ...patch,
  };
}

function snapshot(stock = {}, patch = {}) {
  return {
    code: '1234',
    playerCount: 2,
    difficulty: 1,
    phase: 'lobby',
    visibility: 'public',
    disableCheatMovement: false,
    inputDelay: 0,
    seats: [
      {clientId: 'stock_host_1', name: 'Host', loadout: 0, ready: true},
      {clientId: 'stock_guest_1', name: 'Guest', loadout: 1, ready: true},
    ],
    spectators: [],
    ...stock,
    ...patch,
  };
}

class Socket {
  readyState = 0;
  sent = [];
  listeners = new Map();
  addEventListener(type, listener) {
    this.listeners.set(type, [...(this.listeners.get(type) || []), listener]);
  }
  emit(type, value = {}) {
    for (const listener of this.listeners.get(type) || []) listener(value);
  }
  open() {
    this.readyState = 1;
    this.emit('open');
  }
  message(room) {
    this.emit('message', {data: JSON.stringify({
      type: 'state',
      roomDirectory: {version: 1, controlModes: true, runtimeVersions: true},
      room,
    })});
  }
  send(value) {
    assert.equal(this.readyState, 1);
    this.sent.push(JSON.parse(value));
  }
  close() {this.readyState = 3;}
}

class Storage {
  values = new Map();
  getItem(key) {return this.values.get(key) ?? null;}
  setItem(key, value) {this.values.set(key, String(value));}
  removeItem(key) {this.values.delete(key);}
}

function roomOwner({product = 'th05mp', storage = new Storage(), clientId = 'stock_host_1'} = {}) {
  const sockets = [];
  const sessions = api.createMultiplayerRoomSessionStore({storage});
  const service = api.createMultiplayerRoom({
    identity: {
      lobbyClientId: () => clientId,
      loadDisplayName: () => 'Host',
      updateDisplayName: value => ({updated: true, name: String(value)}),
    },
    sessions,
    preferences: {
      preferTurn: () => false,
      load: () => ({preferredLoadout: 0}),
      persistPreferredLoadout() {},
      persistPreferTurn() {},
      replayAutoSave: () => false,
    },
    memberId: () => `member-${clientId}`,
    createSocket: () => {const socket = new Socket(); sockets.push(socket); return socket;},
    relayUrl: () => 'wss://relay.invalid/socket',
    settings: () => ({touchMovementMode: 'joystick', touchEnabled: false, mobileDevice: false, iceServers: []}),
    setMovementMode() {},
    decisions: {askDecision: async () => 'confirm', askConfirmation: async () => true},
    translate: key => key,
    notify() {},
    prepareResources: async () => {},
    preparationFailed() {},
    beginManualImport() {},
    launch: async () => {},
    checkGame: async () => {},
    operationFailed() {},
    isLaunched: () => false,
    routes: {restore() {}, enter() {}, settleInvite() {}, leave() {}},
    network: {reset() {}, receive() {}, minimumRtt: () => null},
    online: () => true,
    visible: () => true,
  });
  return {service, sessions, sockets, storage, dispose: () => service.dispose(), product};
}

test('all 24 TH05 stock combinations normalize and reach Runtime options', () => {
  const config = api.multiplayerConfigForProduct('th05mp');
  assert.deepEqual(api.TH05_INITIAL_STOCK, {spares: 2, bombs: 3});
  for (let spares = 0; spares <= 5; spares++) {
    for (let bombs = 0; bombs <= 3; bombs++) {
      assert.equal(api.validTh05InitialSpares(spares), true);
      assert.equal(api.validTh05InitialBombs(bombs), true);
      const lobby = api.normalizeMultiplayerLobbySnapshot(snapshot({initialSpares: spares, initialBombs: bombs}), {
        localClientId: 'stock_host_1',
        playerCounts: config.playerCounts,
        difficulties: config.difficulties,
        loadouts: config.loadouts,
      });
      assert.ok(lobby, `stock ${spares}/${bombs} should normalize`);
      assert.equal(lobby.initialSpares, spares);
      assert.equal(lobby.initialBombs, bombs);
      const runtime = api.buildMultiplayerRuntimeOptions(runtimeInput('th05mp', {
        initialSpares: spares,
        initialBombs: bombs,
      }), config);
      assert.equal(runtime.netplayInitialSpares, spares);
      assert.equal(runtime.netplayInitialBombs, bombs);
    }
  }
});

test('stock validation rejects bounds, non-integers, and settings from another title', () => {
  const th05 = api.multiplayerConfigForProduct('th05mp');
  for (const [key, value] of [
    ['initialSpares', -1], ['initialSpares', 6], ['initialSpares', 1.5], ['initialSpares', '2'],
    ['initialBombs', -1], ['initialBombs', 4], ['initialBombs', 1.5], ['initialBombs', '3'],
  ]) {
    assert.throws(() => api.buildMultiplayerRuntimeOptions(runtimeInput('th05mp', {[key]: value}), th05));
    assert.equal(api.normalizeMultiplayerLobbySnapshot(snapshot({[key]: value}), {
      localClientId: 'stock_host_1',
      playerCounts: th05.playerCounts,
      difficulties: th05.difficulties,
      loadouts: th05.loadouts,
    }), null);
  }
  for (const product of ['th06mp', 'th11mp']) {
    const config = api.multiplayerConfigForProduct(product);
    assert.throws(() => api.buildMultiplayerRuntimeOptions(runtimeInput(product, {initialSpares: 2}), config));
    const runtime = api.buildMultiplayerRuntimeOptions(runtimeInput(product), config);
    assert.equal('netplayInitialSpares' in runtime, false);
    assert.equal('netplayInitialBombs' in runtime, false);
  }
  assert.equal(api.validTh05InitialSpares(0), true);
  assert.equal(api.validTh05InitialBombs(0), true);
});

test('zero stock survives room-session save, reload and host defaults', async () => {
  const storage = new Storage();
  const sessions = api.createMultiplayerRoomSessionStore({storage});
  sessions.save('th05mp', {
    room: {code: '1234', playerCount: 2, difficulty: 1, created: true, initialSpares: 0, initialBombs: 0},
    seat: 0,
    ready: false,
    spectatorRequested: false,
    roomSettingsOpen: false,
  });
  const config = api.multiplayerConfigForProduct('th05mp');
  assert.deepEqual(sessions.load({product: 'th05mp', roomCode: '1234', playerCounts: config.playerCounts, difficulties: config.difficulties}).room,
    {code: '1234', visibility: 'public', disableCheatMovement: false, initialSpares: 0, initialBombs: 0, playerCount: 2, difficulty: 1, created: true});
  assert.equal(sessions.load({product: 'th11mp', roomCode: '1234', playerCounts: [2, 3], difficulties: ['Easy', 'Normal']}), null);

  const f = roomOwner({storage});
  try {
    assert.equal(f.service.restoreInvite('https://launcher.invalid/?game=th05mp&mpRoom=1234&lobbyAction=create'), true);
    assert.equal(f.service.getSnapshot().room.initialSpares, 0);
    assert.equal(f.service.getSnapshot().room.initialBombs, 0);
    const socket = f.sockets.at(-1);
    socket.open();
    assert.deepEqual(socket.sent.find(message => message.type === 'settings') && {
      initialSpares: socket.sent.find(message => message.type === 'settings').initialSpares,
      initialBombs: socket.sent.find(message => message.type === 'settings').initialBombs,
    }, {initialSpares: 0, initialBombs: 0});
  } finally {
    f.dispose();
  }
});

test('only a connected host can change stock during lobby; zero reaches the Runtime after relay sync', async () => {
  const host = roomOwner();
  try {
    await host.service.enterRoom('th05mp', '1234', true);
    const socket = host.sockets.at(-1);
    host.service.setRoomSettings({initialSpares: 0, initialBombs: 0});
    assert.equal(socket.sent.some(message => message.type === 'settings'), false, 'stock waits for a connected host');
    socket.open();
    const initial = socket.sent.find(message => message.type === 'settings');
    assert.deepEqual([initial.initialSpares, initial.initialBombs], [2, 3]);
    socket.message(snapshot({initialSpares: 2, initialBombs: 3}));
    socket.sent.length = 0;
    host.service.setRoomSettings({initialSpares: 0, initialBombs: 0});
    const update = socket.sent.find(message => message.type === 'settings');
    assert.ok(update);
    assert.deepEqual([update.initialSpares, update.initialBombs], [0, 0]);
    socket.message(snapshot({initialSpares: 0, initialBombs: 0}, {
      runtimeGeneration: 'a'.repeat(64),
      seats: [
        {clientId: 'stock_host_1', name: 'Host', loadout: 0, ready: false},
        {clientId: 'stock_guest_1', name: 'Guest', loadout: 1, ready: false},
      ],
    }));
    const runtime = host.service.runtimeOptions();
    assert.deepEqual([runtime.netplayInitialSpares, runtime.netplayInitialBombs], [0, 0]);
    socket.sent.length = 0;
    socket.message(snapshot({initialSpares: 0, initialBombs: 0}, {phase: 'running'}));
    host.service.setRoomSettings({initialSpares: 1, initialBombs: 2});
    assert.equal(socket.sent.some(message => message.type === 'settings'), false, 'stock changes close when a run starts');
  } finally {
    host.dispose();
  }

  const guest = roomOwner({clientId: 'stock_guest_1'});
  try {
    await guest.service.enterRoom('th05mp', '1234', false);
    const socket = guest.sockets.at(-1);
    socket.open();
    socket.message(snapshot({initialSpares: 2, initialBombs: 3}));
    socket.sent.length = 0;
    guest.service.setRoomSettings({initialSpares: 5, initialBombs: 0});
    assert.equal(socket.sent.some(message => message.type === 'settings'), false, 'guest cannot change host stock');
  } finally {
    guest.dispose();
  }

  const otherTitle = roomOwner();
  try {
    await otherTitle.service.enterRoom('th11mp', '1234', true);
    const socket = otherTitle.sockets.at(-1);
    socket.open();
    socket.message(snapshot());
    socket.sent.length = 0;
    otherTitle.service.setRoomSettings({initialSpares: 0, initialBombs: 0});
    assert.equal(socket.sent.some(message => message.type === 'settings'), false, 'other titles reject TH05 stock settings');
    assert.equal('initialSpares' in otherTitle.service.getSnapshot().room, false);
  } finally {
    otherTitle.dispose();
  }
});
