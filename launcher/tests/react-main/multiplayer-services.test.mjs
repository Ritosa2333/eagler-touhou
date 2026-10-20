import {readUiHistoryText} from '../support/ui-history.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';
/** Main-derived transport/model tests with synthetic sockets, clocks, storage,
 * preparation and Runtime ports. No live relay, game, networking or persistence
 * acceptance. Authorities: lobby.mts and app440–780/6796–6849/7552–7940. */
import test, {before, after} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync, spawn} from 'node:child_process';
import {WebSocket} from 'ws';
import {runInNewContext} from 'node:vm';
import {mkdir, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
const project = fileURLToPath(new URL('../../', import.meta.url));
let work, owner;
before(async () => {
  await mkdir(resolve(project, '.cache'), {recursive: true}); work = await mkdtemp(resolve(project, '.cache/room-owners-'));
  const file = resolve(work, 'owners.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export {createLobbyDirectory,directoryRoomCanEnter} from './app/services/lobby-directory.ts';
    export {createMultiplayerRoom} from './app/services/multiplayer-room.ts';
    export {createRoomPreparation} from './app/services/room-preparation.ts';
    export {createNetworkActivityTracker} from './src/launcher/network-activity.mts';
    export {createDecisionStore} from './app/models/decisions.ts';
    export {createMultiplayerIdentityStore,multiplayerDisplayNameStorageKey} from './src/launcher/multiplayer-identity.mts';
    export {createMultiplayerRoomSessionStore,multiplayerRoomSessionStorageKey} from './src/launcher/multiplayer-room-session.mts';
    export {createMultiplayerPreferenceStore} from './src/launcher/multiplayer-preferences.mts';
    export {encodeRoomInvite,decodeRoomInvite} from './src/launcher/room-invite.mts';
  `}, outfile: file, bundle: true, platform: 'node', format: 'esm', packages: 'external', logLevel: 'silent',
  plugins: [authoredSourcesPlugin(project)]});
  owner = await import(pathToFileURL(file).href);
});
after(async () => {if (work) await rm(work, {recursive: true, force: true});});
const flush = async () => {for (let n = 0; n < 20; n++) await Promise.resolve();};
class Storage {
  values = new Map();
  getItem(key) {return this.values.get(key) ?? null;}
  setItem(key, value) {this.values.set(key, String(value));}
  removeItem(key) {this.values.delete(key);}
}
class Clock {
  now = 0; sequence = 0; tasks = new Map();
  setTimeout = (fn, delay) => {const id = ++this.sequence; this.tasks.set(id, {fn, at: this.now + delay}); return id;};
  clearTimeout = id => {this.tasks.delete(id);};
  tick(ms) {
    const end = this.now + ms;
    for (;;) {
      const next = [...this.tasks].filter(([, value]) => value.at <= end).sort((a, b) => a[1].at - b[1].at)[0];
      if (!next) break;
      this.tasks.delete(next[0]); this.now = next[1].at; next[1].fn();
    }
    this.now = end;
  }
}
class Socket {
  readyState = 0; sent = []; listeners = new Map(); closes = [];
  constructor(url) {this.url = url;}
  addEventListener(type, fn) {this.listeners.set(type, [...(this.listeners.get(type) || []), fn]);}
  emit(type, value = {}) {for (const fn of this.listeners.get(type) || []) fn(value);}
  open() {this.readyState = 1; this.emit('open');}
  message(value) {this.emit('message', {data: JSON.stringify(value)});}
  send(value) {assert.equal(this.readyState, 1); this.sent.push(JSON.parse(value));}
  close(code = 1000, reason = '') {this.readyState = 3; this.closes.push({code, reason}); this.emit('close', {code});}
}
function stores() {
  const persistent = new Storage(), session = new Storage();
  persistent.setItem(owner.multiplayerDisplayNameStorageKey, 'Alice Wonderland');
  return {persistent, session, identity: owner.createMultiplayerIdentityStore({persistentStorage: persistent, sessionStorage: session, randomWords: () => [1234567, 7654321]}),
    sessions: owner.createMultiplayerRoomSessionStore({storage: session}), preferences: owner.createMultiplayerPreferenceStore({storage: persistent})};
}
const hash = 'a'.repeat(64);
const manifest = () => ({schema: 'eagler-touhou/site-manifest/1', protocol: 'eagler-touhou/1', profile: 'synthetic-test',
  shared: {resourceMode: 'hosted', vanillaFont: '../shared/msgothic.ttc', unicodeFont: '../shared/unifont.otf', netplayRelay: 'wss://relay.invalid/socket'},
  games: {th06: {runtime: 'runtime/th06/game.html', gameData: {version: `sha256-${hash}`, layout: `sha256-${hash}`, path: 'th06.data', bytes: 1, sha256: hash}, music: {midi: {files: []}}}}});
function directory(extra = {}) {
  const storage = stores(), clock = new Clock(), sockets = [], navigation = [], normalized = [];
  const service = owner.createLobbyDirectory({loadSiteManifest: async () => manifest(), createSocket: url => {const socket = new Socket(url); sockets.push(socket); return socket;},
    memberId: () => 'member123456', ...storage, sessionStorage: storage.session, translate: key => key,
    launcherUrl: () => 'https://launcher.invalid/en.html', navigateToRoom: url => navigation.push(url),
    selectedProduct: 'th06mp', onSelectedProductNormalized: product => normalized.push(product), randomWord: () => 123,
    setTimeout: clock.setTimeout, clearTimeout: clock.clearTimeout, ...extra});
  return {service, clock, sockets, navigation, normalized, ...storage};
}
const directoryState = (overrides = {}) => ({type: 'directory', version: 1, rooms: [], product: 'th06mp', total: 0, ...overrides});
async function liveDirectory(extra) {
  const fixture = directory(extra); await fixture.service.initialize();
  fixture.sockets[0].open(); fixture.sockets[0].message(directoryState()); return fixture;
}

test('directory preserves main host-existence product subset and 3s retry without declaring socket offline', async () => {
  const fixture = await liveDirectory(); const {service, sockets, clock} = fixture, socket = sockets[0];
  assert.deepEqual(service.getSnapshot().products, ['th06mp'], 'directory does not require general metadata multiplayerRuntime field');
  assert.equal(new URL(socket.url).searchParams.get('member'), 'member123456');
  socket.message(directoryState({product: ''}));
  service.refreshRooms(); const sends = socket.sent.length;
  clock.tick(3000);
  assert.ok(socket.sent.length > sends); assert.equal(service.getSnapshot().connection, 'live'); assert.equal(socket.closes.length, 0);
  assert.ok(socket.sent.every(message => !Object.hasOwn(message, 'name')), 'directory never sends display name');
  clock.tick(17000);
  assert.equal(service.getSnapshot().connection, 'offline'); assert.equal(socket.closes.length, 1);
  clock.tick(1500); assert.equal(sockets.length, 2); service.dispose();
});

for (const mode of ['normal', 'manifest-test', 'query-test']) test(`directory applies test-product visibility to filters and creation: ${mode}`, async () => {
  const published = manifest();
  published.games.th075 = {...published.games.th06, runtime: 'runtime/th075/game.html', multiplayerRuntime: 'runtime/th075/mp.html',
    gameData: {...published.games.th06.gameData, path: 'th075.data'}};
  if (mode === 'manifest-test') published.shared.testBuild = true;
  const {service, sockets} = directory({loadSiteManifest: async () => published, testBuild: mode === 'query-test', selectedProduct: 'th075mp'});
  try {
    await service.initialize();
    sockets[0].open(); sockets[0].message(directoryState({product: service.getSnapshot().selectedProduct}));
    assert.deepEqual(service.getSnapshot().products, mode === 'normal' ? ['th06mp'] : ['th06mp', 'th075mp']);
    assert.equal(service.getSnapshot().selectedProduct, mode === 'normal' ? 'th06mp' : 'th075mp');
    assert.equal(service.prepareForm('create'), true);
    service.updateForm({product: 'th075mp'});
    assert.equal(service.getSnapshot().form.product, mode === 'normal' ? 'th06mp' : 'th075mp');
  } finally {service.dispose();}
});

test('directory 10s manifest timeout and StrictMode-style suspend/resume do not strand leaving', async () => {
  const blocked = directory({loadSiteManifest: () => new Promise(() => {})});
  const initializing = blocked.service.initialize(); blocked.clock.tick(10000); await initializing;
  assert.equal(blocked.service.getSnapshot().connection, 'offline'); blocked.service.dispose();
  const fixture = await liveDirectory(); fixture.service.pageHide(); assert.equal(fixture.service.getSnapshot().leaving, true);
  fixture.service.resumeView(); assert.equal(fixture.service.getSnapshot().leaving, false);
  assert.equal(fixture.sockets.length, 2); fixture.service.dispose();
});

test('directory retains form values across cancel and enters only after committed form close', async () => {
  const fixture = await liveDirectory(), {service, navigation} = fixture;
  assert.equal(service.prepareForm('create'), true);
  service.updateForm({code: '4567', visibility: 'private', disableCheatMovement: true, difficulty: 2});
  service.formDidClose(); assert.equal(navigation.length, 0);
  assert.equal(service.prepareForm('join'), true);
  const form = service.getSnapshot().form;
  assert.equal(form.code, '4567'); assert.equal(form.visibility, 'private'); assert.equal(form.disableCheatMovement, true); assert.equal(form.difficulty, 1);
  assert.equal(service.submitForm(), 'close'); assert.equal(navigation.length, 0);
  service.formDidClose(); assert.equal(navigation.length, 1);
  const invite = owner.decodeRoomInvite(new URL(navigation[0]).searchParams.get('j'));
  assert.deepEqual(invite, {g: 'th06mp', r: '4567', f: true, a: 'join', p: undefined, d: undefined, v: undefined, c: false});
  service.formDidClose(); assert.equal(navigation.length, 1); service.dispose();
});

test('directory release retains membership/session until authoritative removal and snapshots are bounded', async () => {
  const fixture = await liveDirectory(), {service, sockets, sessions, session} = fixture, socket = sockets[0];
  sessions.save('th06mp', {room: {code: '1234', playerCount: 2, difficulty: 1, created: true}, seat: 0, ready: false, spectatorRequested: false, roomSettingsOpen: false});
  const mine = {product: 'th06mp', code: '1234', recoveryToken: 'recovery-token'};
  socket.message(directoryState({mine, membershipRecovery: true,
    rooms: Array.from({length: 205}, (_, n) => ({product: 'th06mp', code: String(1000 + n), capacity: 2, phase: 'lobby', joinable: true, seats: ['Alice', null]}))}));
  assert.equal(service.getSnapshot().rooms.length, 200); assert.equal(service.getSnapshot().rooms[0].seats[0].initial, 'A');
  service.releaseMembership(); assert.ok(service.getSnapshot().mine); assert.ok(service.getSnapshot().recovering);
  assert.ok(session.getItem(owner.multiplayerRoomSessionStorageKey('th06mp')));
  socket.message(directoryState({mine, membershipRecovery: true})); assert.ok(service.getSnapshot().recovering);
  socket.message(directoryState({mine: null, membershipRecovery: true}));
  assert.equal(service.getSnapshot().mine, null); assert.equal(service.getSnapshot().recovering, null);
  assert.equal(session.getItem(owner.multiplayerRoomSessionStorageKey('th06mp')), null);
  assert.equal(service.getSnapshot().notice, 'lobby.released'); service.dispose();
});

function roomFixture(extra = {}) {
  const storage = stores(), clock = new Clock(), sockets = [], events = [], decisions = owner.createDecisionStore();
  const settings = {touchMovementMode: 'joystick', touchEnabled: false, mobileDevice: false, iceServers: []};
  const ports = {...storage, decisions, memberId: () => 'member123456', createSocket: url => {const socket = new Socket(url); sockets.push(socket); return socket;},
    relayUrl: () => 'wss://relay.invalid/socket', settings: () => settings, setMovementMode: mode => {settings.touchMovementMode = mode;},
    translate: key => key, notify: message => events.push(['notify', message]),
    prepareResources: async context => {events.push(['prepare', context]);}, preparationFailed: error => events.push(['prepare-error', error]),
    beginManualImport: context => events.push(['import', context]), launch: async context => {events.push(['launch', context]);},
    checkGame: async context => {events.push(['check', context]);}, operationFailed: (error, kind) => events.push(['operation-error', kind, error]),
    isLaunched: () => false, routes: {restore: value => events.push(['restore', value]), enter: value => events.push(['enter', value]),
      settleInvite: () => events.push(['settle']), leave: value => events.push(['leave', value])},
    network: {reset() {}, receive: message => events.push(['network', message]), minimumRtt: () => null},
    quickChat: {receive: message => events.push(['quick-chat', message])}, online: () => true, visible: () => true,
    now: () => clock.now, random: () => 0, setTimeout: clock.setTimeout, clearTimeout: clock.clearTimeout, ...extra};
  const service = owner.createMultiplayerRoom(ports);
  return {service, ports, clock, sockets, events, decisions, settings, ...storage};
}
test('rollback products default to D0 for both room creation and restored invites',async()=>{
  for(const product of ['th06mp','th07mp','th08mp','th09mp','th10mp','th11mp']){
    const f=roomFixture();f.service.enterRoom(product,'1234',true);await flush();
    const rollback=true;assert.equal(f.service.getSnapshot().rollbackEnabled,rollback,product);
    const socket=f.sockets[0],id=f.identity.lobbyClientId(product);socket.open();
    socket.message({type:'state',room:roomState(id,{predictionLimit:product==='th11mp'?12:8,seats:[{clientId:id,name:'A',loadout:0,ready:true},{clientId:'guest12345',name:'B',loadout:1,ready:true}]})});
    f.service.setTimingChoice(7);f.service.start();const started=socket.sent.findLast(m=>m.type==='start');
    assert.ok(started,product);if(['th08mp','th09mp','th10mp'].includes(product)){assert.equal(started.adonisMode,0);assert.equal(started.inputDelay,product==='th09mp'?7:0);assert.equal(started.inputDelayAuto,false);
      f.service.setRollbackEnabled(false);f.service.start();const delay=socket.sent.findLast(m=>m.type==='start');assert.equal(delay.adonisMode,1);assert.equal(delay.inputDelay,7);
    }
    if(product==='th11mp'){assert.equal(started.adonisMode,0);assert.equal(started.inputDelay,0);assert.equal(started.predictionLimit,12);}
    f.service.dispose();
    const restored=roomFixture();assert.equal(restored.service.restoreInvite('https://launcher.invalid/?j='+fOwnerInvite(product),product),true);assert.equal(restored.service.getSnapshot().rollbackEnabled,rollback,product+' restore');restored.service.dispose();
  }
});
function fOwnerInvite(product){return owner.encodeRoomInvite({g:product,r:'1234',f:true,a:'create'});}

function roomState(clientId, overrides = {}) {
  return {code: '1234', playerCount: 2, difficulty: 1, phase: 'lobby', inputDelay: 0, predictionLimit: 8, startSerial: 0,
    seats: [{clientId, name: 'A', loadout: 0, ready: false}, null], spectators: [], ...overrides};
}
async function joinedRoom(extra = {}) {
  const fixture = roomFixture(extra); fixture.service.enterRoom('th06mp', '1234', true); await flush();
  const socket = fixture.sockets[0]; socket.open();
  socket.message({type: 'state', roomDirectory: {version: 1, controlModes: true}, room: roomState(fixture.identity.lobbyClientId('th06mp'))});
  return fixture;
}

test('room program versions refresh before readiness and pin the agreed generation into transport', async () => {
  let preparations=0;
  const f=await joinedRoom({prepareResources:async()=>{preparations++;return {runtimeGeneration:hash};}}),socket=f.sockets[0],id=f.identity.lobbyClientId('th06mp');
  socket.message({type:'state',roomDirectory:{runtimeVersions:true},room:roomState(id)});
  const beforeReady = socket.sent.length;
  await f.service.toggleReady();
  assert.equal(socket.sent.slice(beforeReady).some(m=>m.type==='resource-progress'&&m.status!=='ready'),false,
    'rechecking an already prepared program must not invalidate other players readiness');
  assert.equal(preparations,2,'ready rechecks the current publication after lobby precache');
  assert.equal(socket.sent.findLast(m=>m.type==='set-ready').runtimeGeneration,hash);
  const seats=[{clientId:id,loadout:0,ready:true,runtimeGeneration:hash},{clientId:'guest12345',loadout:1,ready:true,runtimeGeneration:'b'.repeat(64)}];
  socket.message({type:'state',room:roomState(id,{seats})});assert.equal(f.service.getSnapshot().canStart,false);
  seats[1].runtimeGeneration=hash;
  socket.message({type:'state',room:roomState(id,{seats})});assert.equal(f.service.getSnapshot().canStart,true);
  socket.message({type:'start',serial:1,room:roomState(id,{seats,phase:'starting',runtimeGeneration:hash})});await flush();
  const launch=f.events.find(([kind])=>kind==='launch');assert.ok(launch);
  assert.equal(new URL(launch[1].runtimeOptions.netplayUrl).searchParams.get('runtimeGeneration'),hash);
  socket.message({type:'state',room:roomState(id,{seats})});assert.equal(f.service.getSnapshot().room.runtimeGeneration,undefined,'next lobby cannot retain the last run identity');
  f.service.dispose();
});

for (const matches of [false, true]) test(`late spectator admission requires the running match program: matches=${matches}`, async () => {
  const localGeneration = matches ? hash : 'b'.repeat(64);
  const f = roomFixture({prepareResources: async () => ({runtimeGeneration: localGeneration})});
  const invite = owner.encodeRoomInvite({g:'th06mp',r:'1234',a:'join',s:true});
  f.service.restoreInvite('https://launcher.invalid/?j='+invite,'th06mp');
  await flush(); const socket = f.sockets[0]; socket.open();
  const room = roomState('other-host',{phase:'playing',startSerial:4,spectatorTimeline:true,runtimeGeneration:hash});
  socket.message({type:'state',roomDirectory:{runtimeVersions:true},room});
  await flush(); socket.message({type:'spectator-start',serial:4,room}); await flush();
  assert.equal(f.events.some(([kind])=>kind==='launch'),matches);
  if (matches) assert.equal(new URL(f.events.find(([kind])=>kind==='launch')[1].runtimeOptions.netplayUrl).searchParams.get('runtimeGeneration'),hash);
  else {
    assert.equal(f.events.find(([kind])=>kind==='operation-error')[2].message,'multiplayer.runtimeVersionMismatch');
    assert.equal(f.service.getSnapshot().room.code,'1234');
    assert.equal(f.service.getSnapshot().spectatorRequested,true);
  }
  f.service.dispose();
});

test('room program versions block old relay readiness and failed preparation launch', async () => {
  const old=await joinedRoom({prepareResources:async()=>({runtimeGeneration:hash})});
  await old.service.toggleReady();assert.equal(old.service.getSnapshot().ready,false);
  assert.equal(old.events.some(([kind,value])=>kind==='notify'&&value==='multiplayer.runtimeRelayUpgrade'),true);
  assert.equal(old.sockets[0].sent.some(m=>m.type==='set-ready'&&m.ready),false);old.service.dispose();
  const failed=await joinedRoom({prepareResources:async()=>{throw new Error('new program incomplete');}});
  await failed.service.toggleReady();await failed.service.launch();
  assert.equal(failed.service.getSnapshot().ready,false);assert.equal(failed.events.some(([kind])=>kind==='launch'),false);failed.service.dispose();
});

test('host spectator moves bind the target, wait for relay state and preserve membership', async () => {
  const f=await joinedRoom(), socket=f.sockets[0], id=f.identity.lobbyClientId('th06mp');
  const seats=[{clientId:id,name:'A',loadout:0,ready:true},{clientId:'guest12345',name:'B',loadout:1,ready:true}];
  const receive=patch=>socket.message({type:'state',room:roomState(id,{seats,...patch})});
  receive({});
  f.service.movePlayerToSpectator(1,'old_client');
  f.service.movePlayerToSpectator(0,id);
  assert.ok(!socket.sent.some(m=>m.type==='move-player-to-spectator'));
  f.service.movePlayerToSpectator(1,'guest12345');
  assert.deepEqual(socket.sent.at(-1),{type:'move-player-to-spectator',seat:1,clientId:'guest12345'});
  assert.equal(f.service.getSnapshot().room.seats[1].clientId,'guest12345','only relay state changes the seat');
  const count=socket.sent.length;
  receive({phase:'running'});f.service.movePlayerToSpectator(1,'guest12345');
  receive({seats:[seats[0],{...seats[1],offline:true}]});f.service.movePlayerToSpectator(1,'guest12345');
  receive({seats:[{...seats[0],clientId:'other_host'},seats[1]]});f.service.movePlayerToSpectator(1,'guest12345');
  assert.equal(socket.sent.length,count,'running, offline and non-host moves are rejected locally');
  receive({seats:[seats[0],{...seats[1],clientId:id}],spectators:[]});
  receive({seats:[{...seats[0],clientId:'other_host'},null],spectators:[{clientId:id,name:'A'}]});
  assert.equal(f.service.getSnapshot().seat,null);
  assert.equal(f.service.getSnapshot().spectatorRequested,true);
  assert.equal(f.service.getSnapshot().ready,false);
  assert.equal(f.service.getSnapshot().connected,true);
  f.service.dispose();
});

test('legacy room wire minimizes names until the server advertises moderation and normalizes challenge/prank', async () => {
  const fixture = await joinedRoom(), {service, sockets, identity} = fixture, socket = sockets[0];
  assert.equal(socket.sent.find(message => message.type === 'take-seat').name, 'A');
  service.setDisplayName('Beatrice'); assert.equal(service.getSnapshot().displayName, 'Beatrice');
  assert.equal(socket.sent.findLast(message => message.type === 'set-name').name, 'B');
  service.standUp(); service.spectate(); assert.equal(socket.sent.findLast(message => message.type === 'spectate').name, 'B');
  service.sendControl({type: 'set-name', name: 'Charlie'}); assert.equal(socket.sent.at(-1).name, 'C');
  socket.message({type: 'state', room: roomState(identity.lobbyClientId('th06mp'), {challengeMode: true, prankMode: true})});
  assert.equal(service.getSnapshot().room.challengeMode, true); assert.equal(service.getSnapshot().room.prankMode, false);
  const runtime = service.runtimeOptions(); assert.equal(runtime.netplayChallengeMode, true);
  assert.equal(Object.hasOwn(runtime, 'netplayAdonisMode'), false, 'TH06 must not receive measured-title-only arguments');
  assert.equal(Object.hasOwn(runtime, 'netplayPredictionReserve'), false);
  assert.equal(new URL(runtime.netplayUrl).searchParams.get('member'), 'member123456'); service.dispose();
});

test('room take-seat commits only relay snapshot, and stale movement confirmation cannot seat in successor', async () => {
  const fixture = roomFixture(), {service, sockets, identity, decisions, settings} = fixture;
  service.enterRoom('th06mp', '1234', false); await flush(); const socket = sockets[0]; socket.open();
  socket.message({type: 'state', room: roomState('otherhost123')});
  await service.takeSeat(1); assert.equal(service.getSnapshot().seat, null);
  assert.equal(socket.sent.at(-1).type, 'take-seat');
  socket.message({type: 'state', room: roomState('otherhost123', {seats: [{clientId: 'otherhost123', name: 'H', loadout: 0},
    {clientId: identity.lobbyClientId('th06mp'), name: 'A', loadout: 1}]})});
  assert.equal(service.getSnapshot().seat, 1);
  service.standUp(); settings.touchMovementMode = 'touch-unlimited';
  socket.message({type: 'state', room: roomState('otherhost123', {disableCheatMovement: true})});
  const before = socket.sent.length, pending = service.takeSeat(1); assert.ok(decisions.getSnapshot());
  service.leave(); decisions.resolve('confirm'); await pending;
  assert.equal(socket.sent.length, before); assert.equal(settings.touchMovementMode, 'touch-unlimited'); service.dispose();
});

test('room restores created directory session and dispose preserves reload storage', async () => {
  const fixture = roomFixture(), {service, sessions, session} = fixture;
  sessions.save('th06mp', {room: {code: '4321', playerCount: 2, difficulty: 2, created: true, visibility: 'private', disableCheatMovement: true},
    seat: 0, ready: true, spectatorRequested: false, roomSettingsOpen: true});
  const token = owner.encodeRoomInvite({g: 'th06mp', r: '4321', f: true, a: 'create', p: 2, d: 1});
  assert.equal(service.restoreInvite(`https://launcher.invalid/?j=${token}`), true);
  assert.equal(service.getSnapshot().room.difficulty, 2); assert.equal(service.getSnapshot().seat, 0);
  assert.equal(service.getSnapshot().ready, true); assert.equal(service.getSnapshot().roomSettingsOpen, true);
  service.dispose(); assert.ok(session.getItem(owner.multiplayerRoomSessionStorageKey('th06mp')));
});

test('room serial/reconnect/terminal handling distinguishes observed rounds from applied starts and rejects old sockets', async () => {
  const fixture = await joinedRoom(), {service, sockets, clock, identity, events} = fixture;
  const snapshot = roomState(identity.lobbyClientId('th06mp'), {startSerial: 7});
  sockets[0].message({type: 'state', room: snapshot});
  sockets[0].message({type: 'start', serial: 7, room: snapshot}); await flush();
  assert.equal(events.filter(([kind]) => kind === 'launch').length, 1);
  sockets[0].message({type: 'start', serial: 8, room: snapshot}); await flush();
  assert.equal(events.filter(([kind]) => kind === 'launch').length, 2);
  const previous = sockets[0]; previous.close(1006); clock.tick(650);
  assert.equal(sockets.length, 2); sockets[1].open();
  const before = service.getSnapshot().startSerial; previous.message({type: 'start', serial: 1000, room: snapshot});
  assert.equal(service.getSnapshot().startSerial, before);
  sockets[1].close(4009); assert.equal(service.getSnapshot().room, null);
  assert.equal(events.findLast(([kind]) => kind === 'leave')[1].message, 'lobby.conflict');
  clock.tick(10000); assert.equal(sockets.length, 2); service.dispose();
});

test('a reconnect snapshot recovers a missed player start exactly once', async () => {
  const f = await joinedRoom(), id = f.identity.lobbyClientId('th06mp');
  f.sockets[0].close(1006); f.clock.tick(650);
  const socket = f.sockets[1]; socket.open();
  const room = roomState(id, {phase:'starting',startSerial:1});
  socket.message({type:'state',room}); await flush();
  assert.equal(f.events.filter(([kind])=>kind==='launch').length,1);
  socket.message({type:'state',room}); socket.message({type:'start',serial:1,room}); await flush();
  assert.equal(f.events.filter(([kind])=>kind==='launch').length,1);
  socket.close(1006); f.clock.tick(650); f.sockets[2].open();
  f.sockets[2].message({type:'state',room}); await flush();
  assert.equal(f.events.filter(([kind])=>kind==='launch').length,1);
  f.service.dispose();
});

test('room snapshots do not start a lobby, an already running round or an unadmitted spectator', async () => {
  for (const phase of ['lobby','running','playing']) {
    const f=await joinedRoom(), id=f.identity.lobbyClientId('th06mp');
    f.sockets[0].message({type:'state',room:roomState(id,{phase,startSerial:1})}); await flush();
    assert.equal(f.events.some(([kind])=>kind==='launch'),false,phase); f.service.dispose();
  }
  const f=await joinedRoom();
  f.sockets[0].message({type:'state',room:roomState('other-host',{phase:'starting',startSerial:1,seats:[{clientId:'other-host'},null]})});await flush();
  assert.equal(f.events.some(([kind])=>kind==='launch'),false);f.service.dispose();
});

test('live relay reconnect recovers a lost start without a duplicate Runtime launch', {timeout:15000}, async () => {
  const relay=spawn(process.execPath,['src/netplay-relay.mjs'],{cwd:resolve(project,'../services/relay'),windowsHide:true,
    stdio:['ignore','pipe','pipe'],env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''}});
  let log='', f; const sockets=[];
  const wait=async(predicate,label)=>{const deadline=Date.now()+5000;while(Date.now()<deadline){if(predicate())return;await new Promise(resolve=>setTimeout(resolve,20));}throw Error(label+': '+log);};
  relay.stdout.on('data',data=>{log+=data;});relay.stderr.on('data',data=>{log+=data;});
  try {
    await wait(()=>/listening ws:\/\/127\.0\.0\.1:(\d+)/.test(log),'relay startup');
    const port=/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log)[1], url=`ws://127.0.0.1:${port}/`;
    const host=new WebSocket(url+'?'+new URLSearchParams({room:'th06mp-6432',lobby:'host-public-id',member:'host-private-member'}));
    sockets.push(host);const hostMessages=[];host.on('message',data=>hostMessages.push(JSON.parse(String(data))));
    await wait(()=>hostMessages.some(message=>message.type==='state'),'host join');
    host.send(JSON.stringify({type:'take-seat',seat:0,loadout:0,name:'host',movementMode:'keyboard'}));
    let dropped=false, connections=0;
    f=roomFixture({relayUrl:()=>url,prepareResources:async()=>({runtimeGeneration:hash}),setTimeout,clearTimeout,
      createSocket:address=>{
        const socket=new WebSocket(address), attempt=++connections;sockets.push(socket);
        return {get readyState(){return socket.readyState;},send:value=>socket.send(value),close:(...args)=>socket.close(...args),
          addEventListener:(type,listener)=>socket.addEventListener(type,event=>{
            if(type==='message'&&attempt===1){
              const message=JSON.parse(String(event.data));
              if(message.type==='state'&&message.room?.phase==='starting')return;
              if(message.type==='start'){dropped=true;socket.terminate();return;}
            }
            listener(event);
          })};
      }});
    f.service.enterRoom('th06mp','6432',false);
    await wait(()=>f.service.getSnapshot().connected,'guest join');
    await f.service.takeSeat(1);await wait(()=>f.service.getSnapshot().seat===1,'guest seat');
    host.send(JSON.stringify({type:'set-ready',ready:true,movementMode:'keyboard',runtimeGeneration:hash}));
    await wait(()=>f.service.getSnapshot().room?.seats[0]?.ready,'host ready before guest');
    const readinessMessages=hostMessages.length;
    await f.service.toggleReady();await wait(()=>{const room=f.service.getSnapshot().room;return room?.seats.slice(0,room.playerCount).every(seat=>seat?.ready);},'ready');
    assert.equal(hostMessages.slice(readinessMessages).some(message=>message.type==='state'&&!message.room.seats[0]?.ready),false,
      'the other player stays ready throughout the guest current-program verification');
    host.send(JSON.stringify({type:'start',inputDelay:0}));
    await wait(()=>dropped&&connections===2&&f.events.some(([kind])=>kind==='launch'),'reconnected launch');
    assert.equal(f.service.getSnapshot().room.phase,'starting');assert.equal(f.service.getSnapshot().startSerial,1);
    assert.equal(f.events.filter(([kind])=>kind==='launch').length,1);
    sockets.at(-1).terminate();await wait(()=>connections===3&&f.service.getSnapshot().connected,'second reconnect');
    await wait(()=>f.service.getSnapshot().room?.synced,'second snapshot');
    assert.equal(f.events.filter(([kind])=>kind==='launch').length,1);
  } finally {
    f?.service.dispose();for(const socket of sockets)socket.terminate();relay.kill();
    if(relay.exitCode===null)await new Promise(resolve=>relay.once('exit',resolve));
  }
});

test('room preflight waits the real preparation and check ports, coalesces clicks, and ignores stale completion', async () => {
  let finishPreparation, finishCheck;
  const preparation = new Promise(resolve => {finishPreparation = resolve;});
  const checking = new Promise(resolve => {finishCheck = resolve;});
  let checkCalls = 0;
  const fixture = await joinedRoom({prepareResources: () => preparation, checkGame: async context => {checkCalls++; assert.equal(context.isCurrent(), true); await checking;}});
  const {service, events} = fixture;
  const first = service.checkGame(), second = service.checkGame(); await flush();
  assert.equal(checkCalls, 0); assert.equal(service.getSnapshot().checkBusy, true);
  assert.equal(events.some(([kind, text]) => kind === 'notify' && text === 'multiplayer.checkGamePassed'), false);
  finishPreparation(); await flush(); assert.equal(checkCalls, 1);
  service.leave(); finishCheck(); await Promise.all([first, second]);
  assert.equal(events.some(([kind, text]) => kind === 'notify' && text === 'multiplayer.checkGamePassed'), false,
    'an old room check cannot announce success in its successor');
  assert.equal(service.getSnapshot().room, null); service.dispose();
});

test('room package cancel aborts only package work, unreaddies, prevents launch, and permits explicit retry', async () => {
  let report, finish, preparationContext, attempts = 0;
  const fixture = await joinedRoom({prepareResources: async (context, progress) => {
    attempts++; preparationContext = context; report = progress;
    if (attempts === 1) await new Promise(resolve => {finish = resolve;});
  }});
  const {service, sockets, events} = fixture, socket = sockets[0];
  const readiness=service.toggleReady();await flush();assert.equal(service.getSnapshot().ready,false,'pending preparation cannot ready');
  service.cancelPreparation();
  assert.equal(preparationContext.signal.aborted, true); assert.equal(service.getSnapshot().preparation.status, 'cancelled');
  assert.equal(service.getSnapshot().ready, false); assert.equal(socket.sent.some(message => message.type === 'set-ready'&&message.ready), false);
  assert.equal(events.filter(([kind]) => kind === 'import').length, 1);
  const launch = service.launch(); finish(); await Promise.all([launch,readiness]);
  assert.equal(events.some(([kind]) => kind === 'launch'), false);
  report({status: 'ready', stage: 'runtime', percent: 100, error: ''});
  assert.equal(service.getSnapshot().preparation.status, 'cancelled', 'late aborted progress cannot overwrite cancellation');
  service.retryPreparation(); await flush(); assert.equal(attempts, 2);
  assert.equal(service.getSnapshot().preparation.status, 'ready');
  await service.launch(); assert.equal(events.filter(([kind]) => kind === 'launch').length, 1); service.dispose();
});

test('room runtime-stage preparation cannot take package cancellation path and progress is socket-deduplicated', async () => {
  let report, finish;
  const fixture = await joinedRoom({prepareResources: async (_context, progress) => {report = progress; await new Promise(resolve => {finish = resolve;});}});
  const {service, sockets, events} = fixture, socket = sockets[0];
  const progress = {status: 'preparing', stage: 'package', percent: null, loaded: 13, total: 100, error: ''};
  report(progress); report({...progress, loaded: 14});
  assert.equal(socket.sent.filter(message => message.type === 'resource-progress' && message.percent === 10).length, 1);
  report({...progress, stage: 'runtime'}); service.cancelPreparation();
  assert.equal(service.getSnapshot().preparation.status, 'preparing'); assert.equal(events.some(([kind]) => kind === 'import'), false);
  finish(); await flush(); assert.equal(service.getSnapshot().preparation.status, 'ready'); service.dispose();
});

test('room resource switch aborts old preparation, requests all-seat unready, and ignores late completion', async () => {
  const tasks = [];
  const f = await joinedRoom({prepareResources(context, report) {return new Promise(resolve => tasks.push({context, report, resolve}));}});
  assert.equal(tasks.length, 1);
  f.service.resourcesChanged(); await flush();
  assert.equal(tasks[0].context.signal.aborted, true); assert.equal(tasks.length, 2);
  assert.ok(f.sockets[0].sent.some(message => message.type === 'resources-changed'));
  tasks[0].resolve(); await flush();
  assert.equal(f.service.getSnapshot().preparation.status, 'preparing');
  const readiness = f.service.toggleReady(); await flush(); assert.equal(f.service.getSnapshot().ready, false);
  tasks[1].resolve(); await readiness;
  assert.equal(f.service.getSnapshot().ready, true); f.service.dispose();
});

test('settings reload pauses resource work and readiness until explicit resume', async () => {
  const tasks = [];let released=0;
  const f = await joinedRoom({releasePreparedResources(){released++;},prepareResources(context, report) {return new Promise(resolve => tasks.push({context, report, resolve}));}});
  const initialReleased=released;
  f.service.suspendForSettingsReload();
  assert.equal(released,initialReleased,'draft settings must not retire the visible native title');
  assert.equal(tasks[0].context.signal.aborted, true);
  assert.ok(f.sockets[0].sent.some(message => message.type === 'resources-changed'));
  tasks[0].resolve(); await flush();
  f.service.retryPreparation(); f.service.resourcesChanged();
  await f.service.toggleReady(); await f.service.launch(); await flush();
  assert.equal(tasks.length, 1, 'neither retry, Ready nor Start can bypass the settings decision');
  assert.equal(f.service.getSnapshot().ready, false);
  assert.equal(f.events.some(([kind]) => kind === 'launch'), false);
  f.service.resumeAfterSettingsReload(); await flush();
  assert.equal(released,initialReleased+1,'only the explicit reload may release the old resources');
  assert.equal(tasks.length, 2);
  tasks[1].resolve(); await flush();
  assert.equal(f.service.getSnapshot().preparation.status, 'ready');
  f.service.dispose();
});

test('discarding draft room settings revalidates without releasing prepared resources',async()=>{
 let released=0;
 const f=await joinedRoom({releasePreparedResources(){released++;}});await flush();
 const before=released;f.service.suspendForSettingsReload();
 f.service.resumeAfterSettingsReload(true);await flush();
 assert.equal(released,before);assert.equal(f.service.getSnapshot().preparation.status,'ready');
 f.service.dispose();
});

test('every playing or full directory room admits spectators even without mid-run playback', () => {
  for (const product of ['th06mp', 'th07mp', 'th08mp', 'th09mp', 'th10mp', 'th11mp', 'th15mp']) {
    assert.equal(owner.directoryRoomCanEnter({product, phase: 'playing', players: 2, capacity: 2, joinable: false, spectatable: false}), true);
    assert.equal(owner.directoryRoomCanEnter({product, phase: 'lobby', players: 2, capacity: 2, joinable: false}), true);
  }
});

function preparationFixture({installed = true, worker = null, installError, noCatalog = false, network, prepareProgram} = {}) {
  const events = [], progress = [], controller = new AbortController(); let onChange;
  let generation = installed ? {game: 'th06'} : null;
  const host = {games: {th06: {multiplayerRuntime: 'runtime/th06/mp.html'}}, shared: {runtimeManifest: 'runtime-manifest.json'}};
  const acquisition = {fetchImpl: async () => {throw new Error('fixture must not fetch');}, catalogUrl: 'https://launcher.invalid/releases.json',
    getGameVersion: () => 'original',
    getPublication: async () => ({catalog: noCatalog ? null : {games: {th06: {}}}, catalogUrl: 'https://launcher.invalid/releases.json'}),
    getMetadata: () => ({siteManifest: host, releaseCatalog: noCatalog ? null : {games: {th06: {}}}}),
    readCurrent: async game => {events.push(['read', game]); return {generation};},
    installPublished: async (game, options) => {events.push(['install', game, options]); if (installError) throw installError; generation = {game};},
    refreshInstalled: async game => {events.push(['publish', game]);},
  };
  const context = {product: 'th06mp', roomCode: '1234', epoch: 1, signal: controller.signal, isCurrent: () => !controller.signal.aborted};
  const service = owner.createRoomPreparation({acquisition, metadataReady: async () => {events.push(['metadata']);},
    baseUrl: 'https://launcher.invalid/', translate: key => key, network,
    activeWorker: async () => {events.push(['worker']); return worker;}, dependencies: {
      createTracker: options => {onChange = options.onChange; return {resourceFetch: async () => {throw new Error('fixture installer does not request bytes');}};},
      prepareRuntime: async (...args) => {events.push(['runtime', ...args]); await prepareProgram?.(args[1].onProgress); return {url: '', generation: hash, cached: true};},
    }});
  return {service, events, progress, controller, context, report: value => progress.push(value), emit: snapshot => onChange(snapshot)};
}

test('room preparation uses canonical package-only install and verifies current Runtime without app-shell worker', async () => {
  const fixture = preparationFixture({installed: false}); const prepared=await fixture.service.prepare(fixture.context, fixture.report);
  const install = fixture.events.find(([kind]) => kind === 'install');
  assert.deepEqual(install[2].addComponents, []); assert.equal(install[2].signal, fixture.context.signal);
  assert.equal(install[2].selectedComponentEntries, undefined, 'room preparation does not acquire optional language/music');
  assert.equal(fixture.events.filter(([kind]) => kind === 'read').length, 2, 'canonical install is re-read before ready');
  const runtime=fixture.events.find(([kind])=>kind==='runtime');
  assert.equal(runtime[2].requireCurrent,true);assert.equal(runtime[2].worker,undefined);
  assert.equal(prepared.runtimeGeneration,hash);
  assert.equal(fixture.progress.at(-1).stage, 'runtime');
  assert.equal(fixture.progress.some(value => value.status === 'ready'), false, 'room owner alone publishes final readiness');
});

test('room preparation verifies the current generation with its real worker and ignores aborted tracker callbacks', async () => {
  const worker = {postMessage() {}}; const fixture = preparationFixture({worker});
  await fixture.service.prepare(fixture.context, fixture.report);
  assert.equal(fixture.events.some(([kind]) => kind === 'install'), false);
  const runtime = fixture.events.find(([kind]) => kind === 'runtime');
  assert.equal(runtime[1], 'runtime/th06/mp.html'); assert.equal(runtime[2].worker, worker);
  assert.equal(runtime[2].baseUrl, 'https://launcher.invalid/');
  assert.equal(runtime[2].requireCurrent,true);
  const count = fixture.progress.length; fixture.controller.abort();
  fixture.emit({active: [{loaded: 50, total: 100, title: 'late'}]});
  assert.equal(fixture.progress.length, count);
});

for (const aborted of [false,true]) test(`room program downloads share visible byte progress and release their own tasks (${aborted?'cancel':'complete'})`,async()=>{
 const visible=[],network=owner.createNetworkActivityTracker({fetchImpl:async()=>new Response(''),onChange:value=>visible.push(value)});
 const unrelated=network.begin({title:'unrelated',loaded:10,total:20});
 let late,fixture;
 fixture=preparationFixture({network,prepareProgram:async report=>{
  late=report;
  report({path:'runtime/th06/game.wasm',phase:'requesting',loaded:0,total:4096});
  report({path:'runtime/th06/game.wasm',phase:'receiving',loaded:2048,total:4096});
  assert.equal(network.activeCount,2);
  assert.ok(visible.some(value=>value.active.some(task=>task.label==='TH06 WebAssembly'&&task.loaded===2048&&task.total===4096)));
  assert.ok(fixture.progress.some(value=>value.stage==='runtime'&&value.loaded===2048&&value.total===4096&&value.percent===50));
  if(aborted)fixture.controller.abort();
 }});
 await fixture.service.prepare(fixture.context,fixture.report);
 assert.equal(network.activeCount,1,'unrelated foreground downloads remain active');
 const count=fixture.progress.length;
 late({path:'runtime/th06/game.wasm',phase:'receiving',loaded:3072,total:4096});
 network.update(unrelated,{loaded:15});
 assert.equal(network.activeCount,1,'late program callbacks cannot create stale transfer tasks');
 assert.equal(fixture.progress.length,count,'completed room preparation unsubscribes from foreground activity');
 network.finish(unrelated);
});

test('room preparation classifies install errors while preserving original cancellation identity', async () => {
  const failure = new Error('checksum failed'); const fixture = preparationFixture({installed: false, installError: failure});
  await assert.rejects(fixture.service.prepare(fixture.context, fixture.report), error => error.gameDataAcquisition === true && error.cause === failure);
  const cancelled = {name: 'AbortError', message: 'cancel'};
  const aborted = preparationFixture({installed: false, installError: cancelled});
  await assert.rejects(aborted.service.prepare(aborted.context, aborted.report), error => error === cancelled);
  const missing = preparationFixture({installed: false, noCatalog: true});
  await assert.rejects(missing.service.prepare(missing.context, missing.report), /package.releaseNotReadyNoLocal/);
  assert.equal(missing.events.some(([kind]) => kind === 'metadata'), true); assert.equal(missing.events.some(([kind]) => kind === 'install'), false);
});

test('room import failure cancels the existing import without opening a second modal or sending ready', async () => {
  const fixture = await joinedRoom(), {service, events, sockets} = fixture;
  service.setImporting(true); const sent = sockets[0].sent.filter(message => message.type === 'set-ready').length;
  service.cancelImport(); assert.equal(service.getSnapshot().preparation.status, 'cancelled');
  assert.equal(events.some(([kind]) => kind === 'import'), false); assert.equal(sockets[0].sent.filter(message => message.type === 'set-ready').length, sent);
  service.cancelImport(); assert.equal(service.getSnapshot().preparation.status, 'cancelled'); service.dispose();
});

test('room original count/difficulty actions retain their distinct phase policy from rule buttons', async () => {
  const fixture = await joinedRoom(), {service, sockets, identity} = fixture;
  sockets[0].message({type: 'state', room: roomState(identity.lobbyClientId('th06mp'), {phase: 'running'})});
  const before = sockets[0].sent.length;
  service.setRoomSettings({difficulty: 2}); assert.equal(service.getSnapshot().room.difficulty, 2);
  assert.equal(sockets[0].sent.length, before + 1);
  service.setRoomSettings({challengeMode: true}); assert.equal(sockets[0].sent.length, before + 1);
  service.dispose();
});

test('room failure awaits actual Player cleanup before clearing busy and shows check failure for4000ms', async () => {
  let finishFailure; const failure = new Promise(resolve => {finishFailure = resolve;}), notices = [];
  const fixture = await joinedRoom({checkGame: async () => {throw new Error('preflight failure');},
    operationFailed: async () => {await failure;}, notify: (message, duration) => notices.push({message, duration})});
  const check = fixture.service.checkGame(); await flush();
  assert.equal(fixture.service.getSnapshot().checkBusy, true); assert.equal(fixture.service.getSnapshot().launchBusy, true);
  assert.equal(notices.some(notice => notice.message === 'multiplayer.checkGameFailed'), false);
  finishFailure(); await check;
  assert.equal(fixture.service.getSnapshot().checkBusy, false); assert.equal(fixture.service.getSnapshot().launchBusy, false);
  assert.deepEqual(notices.at(-1), {message: 'multiplayer.checkGameFailed', duration: 4000}); fixture.service.dispose();
});

test('TH09 title-room reset clears membership without invoking ordinary departure route', async () => {
  const fixture = await joinedRoom(); const before = fixture.events.filter(([kind]) => kind === 'leave').length;
  fixture.service.resetRoomWithoutNavigation();
  assert.equal(fixture.service.getSnapshot().room, null);
  assert.equal(fixture.events.filter(([kind]) => kind === 'leave').length, before);
  assert.equal(fixture.session.getItem(owner.multiplayerRoomSessionStorageKey('th06mp')), null); fixture.service.dispose();
});

test('original initial non-MP invite fallback selects default MP without changing supplied token', async () => {
  const fixture = roomFixture(), token = owner.encodeRoomInvite({g: 'th20', r: '4321', f: true, a: 'join'});
  const source = `https://launcher.invalid/?j=${token}&keep=a%20b#original`;
  assert.equal(fixture.service.restoreInvite(source, 'th07mp'), true);
  assert.equal(fixture.service.getSnapshot().product, 'th07mp');
  assert.equal(new URL(fixture.sockets[0].url).searchParams.get('room'), 'th07mp-4321');
  assert.deepEqual(fixture.events.find(([kind]) => kind === 'restore')[1], {product: 'th07mp', code: '4321', fromDirectory: true});
  assert.equal(owner.decodeRoomInvite(token).g, 'th20'); fixture.service.dispose();
});

const pinnedMainRevision = 'edee9633e5e3ee79cd2e1aa334f84f6caf755090';
function pinnedRoomRetry({status, attempt, present = true}) {
  const source = readUiHistoryText('edee9633e5e3ee79cd2e1aa334f84f6caf755090', 'src/launcher/app.mts');
  const start = source.indexOf('$("#mpRoomResourceRetry").addEventListener("click", () => {');
  assert.ok(start > 0); const end = source.indexOf('\n});', start); assert.ok(end > start);
  const events = [], room = {}, sandbox = {mpUiState: {room: present ? room : null},
    roomPreparation: {room, status}, gameDataAttempt: structuredClone(attempt),
    clearGameDataAttempt() {events.push('clear'); sandbox.gameDataAttempt = null;},
    prepareRoomResources(value) {assert.equal(value, room); events.push('prepare');},
    $: selector => {assert.equal(selector, '#mpRoomResourceRetry'); return {addEventListener(type, listener) {assert.equal(type, 'click'); sandbox.retry = listener;}};},
  };
  runInNewContext(source.slice(start, end + '\n});'.length), sandbox); sandbox.retry();
  return {events, attempt: sandbox.gameDataAttempt};
}

test('room Retry matches pinned handler eligibility, cleanup order and continuation filtering', async t => {
  for (const status of ['cancelled', 'failed', 'preparing', 'ready', 'importing']) {
    for (const attempt of [null, {manual: true, continuation: {kind: 'install-only'}},
      {manual: true, continuation: {kind: 'launch'}}, {manual: false, continuation: {kind: 'install-only'}}]) {
      await t.test(`${status} / ${JSON.stringify(attempt)}`, async () => {
        let report, currentAttempt = structuredClone(attempt), cleanupCalls = 0;
        const events = [], finish = [];
        const fixture = await joinedRoom({
          prepareResources: async (_context, progress) => {events.push('prepare'); report = progress; await new Promise(resolve => {finish.push(resolve);});},
          beforePreparationRetry() {cleanupCalls++; if (currentAttempt?.manual && currentAttempt.continuation?.kind === 'install-only') {events.push('clear'); currentAttempt = null;}},
        });
        report({status, stage: 'package', percent: null, error: ''}); events.length = 0;
        const expected = pinnedRoomRetry({status, attempt});
        fixture.service.retryPreparation(); await flush();
        assert.deepEqual(events, expected.events);
        assert.deepEqual(currentAttempt, expected.attempt);
        assert.equal(cleanupCalls, expected.events.includes('prepare') ? 1 : 0, 'ineligible Retry never reaches the cleanup port');
        fixture.service.dispose(); for (const resolve of finish) resolve(); await flush();
      });
    }
  }
  await t.test('absent room', async () => {
    let cleanupCalls = 0; const fixture = await joinedRoom({beforePreparationRetry() {cleanupCalls++;}});
    fixture.service.leave(); const expected = pinnedRoomRetry({status: 'cancelled', attempt: {manual: true, continuation: {kind: 'install-only'}}, present: false});
    fixture.service.retryPreparation(); assert.equal(cleanupCalls, 0); assert.deepEqual(expected.events, []); fixture.service.dispose();
  });
});

test('TH11 owner switches Direct D0/P12 and pure delay while preserving 2P/3P Extra runtime options', async () => {
  for (const playerCount of [2, 3]) {
    const f = roomFixture();
    f.service.enterRoom('th11mp', '1234', true);
    await flush();
    const socket = f.sockets[0];
    socket.open();
    const id = f.identity.lobbyClientId('th11mp');
    const seats = Array.from({ length: playerCount }, (_, i) => ({
      clientId: i ? `peer-seat${i}` : id, name: 'A', loadout: i ? 4 : 0, ready: true,
    }));
    socket.message({ type: 'state', room: roomState(id, {
      playerCount, difficulty: 4, seats, predictionLimit: 12, adonisMode: 0,
    }) });
    f.service.setRollbackEnabled(true);
    assert.equal(f.service.getSnapshot().rollbackEnabled, true);
    f.service.start();
    let start = socket.sent.findLast((m) => m.type === 'start');
    assert.equal(start.adonisMode, 0);
    assert.equal(start.predictionLimit, 12);
    assert.equal(start.inputDelay, 0);
    assert.equal(start.inputDelayAuto, false);
    f.service.setRollbackEnabled(false);
    f.service.setTimingChoice(9);
    f.service.start();
    start = socket.sent.findLast((m) => m.type === 'start');
    assert.equal(start.adonisMode, 1);
    assert.equal(start.predictionLimit, 12, 'room envelope advertises cap, not active pure-delay prediction');
    assert.equal(start.inputDelay, 9);
    assert.equal(start.inputDelayAuto, false);
    socket.message({ type: 'state', room: roomState(id, {
      playerCount, difficulty: 4, seats, phase: 'running', predictionLimit: 12,
      adonisMode: 1, inputDelay: 9, inputDelayAuto: false,
    }) });
    let options = f.service.runtimeOptions();
    assert.equal(options.netplayPredictionLimit, 0, 'pure delay bridge keeps legacy P0');
    assert.equal(options.netplayAdonisMode, 1);
    assert.equal(options.netplayInputDelay, 9);
    assert.equal(options.netplayDifficulty, 4);
    assert.equal(options.netplayPlayerCount, playerCount);
    assert.equal(options.netplayLoadouts[1].shot, 1);
    socket.message({ type: 'state', room: roomState(id, {
      playerCount, difficulty: 4, seats, phase: 'running', predictionLimit: 12,
      adonisMode: 0, inputDelay: 0, inputDelayAuto: false,
    }) });
    options = f.service.runtimeOptions();
    assert.equal(options.netplayPredictionLimit, 12, 'real relay cap survives both typed boundaries');
    assert.equal(options.netplayAdonisMode, 0);
    assert.equal(options.netplayInputDelay, 0);
    assert.equal(options.netplayInputDelayAuto, false);
    // A relay room that requests a smaller window must not silently become a
    // different native fixed-P12 session.
    socket.message({ type: 'state', room: roomState(id, {
      playerCount, difficulty: 4, seats, phase: 'running', predictionLimit: 8,
      adonisMode: 0, inputDelay: 0, inputDelayAuto: false,
    }) });
    assert.throws(() => f.service.runtimeOptions(), /TH11/);
    f.service.dispose();
  }
});

test('rapid loadout changes ignore stale snapshots and cannot ready/start until latest socket echo',async()=>{
 const f=await joinedRoom();const id=f.identity.lobbyClientId('th06mp'),socket=f.sockets[0];
 for(let i=0;i<4;i++) f.service.changeLoadout(1);
 assert.equal(f.service.getSnapshot().preferredLoadout,0); // TH06 wraps its four choices.
 f.service.changeLoadout(1);
 assert.equal(f.service.getSnapshot().preferredLoadout,1);
 socket.message({type:'state',room:roomState(id,{seats:[{clientId:id,name:'A',loadout:0,ready:true},{clientId:'peer_1234',name:'B',loadout:1,ready:true}]})});
 assert.equal(f.service.getSnapshot().preferredLoadout,1);assert.equal(f.service.getSnapshot().loadoutPending,true);
 assert.equal(f.service.getSnapshot().ready,false);const before=socket.sent.length;
 await f.service.toggleReady();f.service.start();assert.equal(socket.sent.length,before);
 assert.throws(()=>f.service.runtimeOptions(),/roomStateInvalid/);
 socket.message({type:'state',room:roomState(id,{seats:[{clientId:id,name:'A',loadout:1,ready:false},{clientId:'peer_1234',name:'B',loadout:1,ready:true}]})});
 assert.equal(f.service.getSnapshot().loadoutPending,false);await f.service.toggleReady();
 assert.equal(socket.sent.at(-1).type,'set-ready');assert.equal(socket.sent.at(-1).ready,true);f.service.dispose();
});

test('TH09 spectator invite joins without seating and waits for authoritative spectator-start', async () => {
  const f = roomFixture();
  const invite = owner.encodeRoomInvite({g: 'th09mp', r: '1234', f: true, a: 'join', s: true});
  assert.equal(owner.decodeRoomInvite(invite).s, true);
  assert.equal(owner.decodeRoomInvite(owner.encodeRoomInvite({g: 'th08mp', r: '1234', a: 'join', s: true})).s, true);
  f.service.restoreInvite('https://launcher.invalid/?j=' + invite, 'th09mp');
  await flush();
  const socket = f.sockets[0];
  socket.open();
  assert.equal(socket.sent.filter(m => m.type === 'spectate').length, 1);
  const room = roomState('host', {phase: 'playing', spectatorTimeline: true, startSerial: 4});
  socket.message({type: 'state', room});
  await flush();
  assert.equal(socket.sent.some(m => m.type === 'take-seat'), false);
  assert.equal(f.events.some(e => e[0] === 'launch'), false);
  socket.message({type: 'spectator-start', serial: 4, room});
  await flush();
  const launched = f.events.find(e => e[0] === 'launch');
  assert.ok(launched);
  assert.equal(launched[1].runtimeOptions.netplaySpectator, true);
  f.service.dispose();
});

for(const product of ['th06mp','th07mp','th08mp','th09mp','th10mp','th11mp','th15mp'])
test(`${product} resumes an admitted spectator launch after explicit resource recovery`,async()=>{
 let installed=false;
 const f=roomFixture({prepareResources:async()=>{if(!installed)throw Error('DATA missing');}});
 f.service.enterRoom(product,'1234',false);await flush();const socket=f.sockets[0];socket.open();
 const room=roomState('host',{phase:'running',spectatorTimeline:true,startSerial:4,
  predictionLimit:product==='th11mp'?12:product==='th15mp'?0:8,adonisMode:product==='th15mp'?1:0,
  seats:[{clientId:'host',name:'A',loadout:0,ready:true},{clientId:'peer',name:'B',loadout:1,ready:true}]});
 socket.message({type:'state',room});f.service.spectate();
 socket.message({type:'spectator-start',serial:4,room});await flush();
 assert.equal(f.events.filter(e=>e[0]==='launch').length,0);assert.equal(f.service.getSnapshot().preparation.status,'failed');
 installed=true;f.service.retryPreparation();await flush();
 const launched=f.events.filter(e=>e[0]==='launch');assert.equal(launched.length,1,'resource recovery continues the already admitted launch');
 assert.equal(launched[0][1].runtimeOptions.netplaySpectator,true);
 f.service.setImporting(true);f.service.cancelImport();f.service.retryPreparation();await flush();
 assert.equal(f.events.filter(e=>e[0]==='launch').length,1,'a completed/exited same-run viewer is not restarted by another resource retry');
 f.service.dispose();
});

test('spectator resource recovery requires a pending authorized launch and current observer role',async()=>{
 for(const mode of ['not-admitted','left-role','left-room','failed-again']){
  let installed=false;const f=roomFixture({prepareResources:async()=>{if(!installed)throw Error('DATA missing');}});
  f.service.enterRoom('th09mp','1234',false);await flush();const socket=f.sockets[0];socket.open();
  const room=roomState('host',{phase:'running',spectatorTimeline:true,startSerial:4});
  socket.message({type:'state',room});f.service.spectate();
  if(mode!=='not-admitted'){socket.message({type:'spectator-start',serial:4,room});await flush();}
  if(mode==='left-role')f.service.leaveSpectator();
  if(mode==='left-room')f.service.leave();
  installed=mode!=='failed-again';f.service.retryPreparation();await flush();
  assert.equal(f.events.filter(e=>e[0]==='launch').length,0,mode);f.service.dispose();
 }
});

test('an in-flight spectator resource retry cannot launch after role departure or owner disposal',async()=>{
 for(const action of ['leave-role','dispose']){
  let recovering=false,finish;
  const f=roomFixture({prepareResources:async()=>{if(!recovering)throw Error('DATA missing');await new Promise(resolve=>finish=resolve);}});
  f.service.enterRoom('th09mp','1234',false);await flush();const socket=f.sockets[0];socket.open();
  const room=roomState('host',{phase:'running',spectatorTimeline:true,startSerial:4});
  socket.message({type:'state',room});f.service.spectate();socket.message({type:'spectator-start',serial:4,room});await flush();
  recovering=true;f.service.retryPreparation();await flush();assert.equal(typeof finish,'function');
  if(action==='leave-role')f.service.leaveSpectator();else f.service.dispose();
  finish();await flush();assert.equal(f.events.filter(e=>e[0]==='launch').length,0,action);f.service.dispose();
 }
});

test('TH09 stream startup race retries the same match without a next-match warning', async () => {
  const f=roomFixture();f.service.enterRoom('th09mp','1234',false);await flush();
  const socket=f.sockets[0];socket.open();
  const room=roomState('host',{phase:'running',spectatorTimeline:true,startSerial:4});
  socket.message({type:'state',room});f.service.spectate();
  const count=()=>socket.sent.filter(m=>m.type==='spectate').length;
  assert.equal(count(),1);
  const rejected={type:'error',error:'本局已经开始；已保留旁观席，将在下一局生效'};
  socket.message(rejected);socket.message(rejected);
  assert.equal(f.events.some(e=>e[0]==='notify'),false);
  f.clock.tick(999);assert.equal(count(),1);f.clock.tick(1);assert.equal(count(),2);
  socket.message(rejected);socket.message({type:'spectator-start',serial:4,room});await flush();
  f.clock.tick(2000);assert.equal(count(),2);assert.ok(f.events.some(e=>e[0]==='launch'));
  f.service.dispose();
});

test('leaving a pending TH09 spectator seat cancels admission retry', async () => {
  const f=roomFixture();f.service.enterRoom('th09mp','1234',false);await flush();
  const socket=f.sockets[0];socket.open();socket.message({type:'state',room:roomState('host',{phase:'running',spectatorTimeline:true,startSerial:4})});
  f.service.spectate();socket.message({type:'error',error:'本局已经开始；已保留旁观席，将在下一局生效'});
  f.service.leaveSpectator();f.clock.tick(2000);
  assert.equal(socket.sent.filter(m=>m.type==='spectate').length,1);f.service.dispose();
});

test('TH09 ordinary room entry during play neither seats nor starts the visitor', async () => {
  const f = roomFixture();
  const invite = owner.encodeRoomInvite({g: 'th09mp', r: '1234', f: true, a: 'join'});
  f.service.restoreInvite('https://launcher.invalid/?j=' + invite, 'th09mp');
  await flush();
  const socket = f.sockets[0];socket.open();
  const room = roomState('host', {phase:'running', spectatorTimeline:true, startSerial:4});
  socket.message({type:'state',room});await flush();
  socket.message({type:'spectator-start',serial:4,room});await flush();
  assert.equal(f.service.getSnapshot().spectatorRequested,false);
  assert.equal(socket.sent.some(m=>m.type==='take-seat'||m.type==='spectate'),false);
  assert.equal(f.events.some(e=>e[0]==='launch'),false);
  f.service.dispose();
});

test('TH09 directory admits full and starting rooms before spectator history is ready',()=>{
  for(const phase of ['lobby','starting','running']){
    assert.equal(owner.directoryRoomCanEnter({product:'th09mp',code:'1234',phase,
      capacity:2,players:2,ready:0,difficulty:1,joinable:false,spectatable:false,
      seats:[{initial:'A',online:true},{initial:'B',online:true}]}),true,phase);
  }
});

for(const product of ['th06mp','th07mp','th08mp','th10mp','th11mp','th15mp'])test(`Directory observer intent never auto-reserves a player seat: ${product}`,async()=>{
  const f=roomFixture();
  assert.equal(f.service.restoreInvite('https://launcher.invalid/?j='+owner.encodeRoomInvite({g:product,r:'1234',f:true,a:'join',s:true}),product),true);
  await flush();const socket=f.sockets.at(-1);socket.open();
  socket.message({type:'state',room:roomState('other_host',{seats:[{clientId:'other_host',name:'A',loadout:0,ready:true},{clientId:'other_guest',name:'B',loadout:1,ready:true}]})});
  await flush();assert.equal(socket.sent.some(m=>m.type==='take-seat'),false);
  assert.equal(socket.sent.some(m=>m.type==='spectate'),true);
  assert.equal(f.service.getSnapshot().spectatorRequested,true);f.service.dispose();
});

for (const phase of ['lobby', 'playing']) test(`directory join falls back to a spectator when a room fills or starts: ${phase}`, async () => {
  const f = roomFixture();
  assert.equal(f.service.restoreInvite('https://launcher.invalid/?j=' + owner.encodeRoomInvite({g:'th06mp',r:'1234',f:true,a:'join'}), 'th06mp'), true);
  await flush(); const socket = f.sockets.at(-1); socket.open();
  socket.message({type:'state', room:roomState('other_host', {phase, seats:[
    {clientId:'other_host',name:'A',loadout:0,ready:true}, {clientId:'other_guest',name:'B',loadout:1,ready:true},
  ]})});
  await flush();
  assert.equal(socket.sent.some(message => message.type === 'take-seat'), false);
  assert.equal(socket.sent.some(message => message.type === 'spectate'), true);
  assert.equal(f.service.getSnapshot().spectatorRequested, true);
  f.service.dispose();
});

test('public nickname commits only an authoritative approval and ignores stale connection replies',async()=>{
 const f=await joinedRoom(),socket=f.sockets.at(-1),before=f.service.getSnapshot().displayName;
 socket.message({type:'nickname-status',name:'',status:'rejected',nextChangeAt:0,retryAt:0});
 assert.equal(f.service.setDisplayName(' Ａlice '),'Alice');
 assert.deepEqual(socket.sent.at(-1),{type:'set-name',name:'Alice'});
 assert.equal(f.service.getSnapshot().displayName,before);
 assert.equal(f.service.getSnapshot().nickname.status,'reviewing');
 socket.message({type:'nickname-status',name:before,status:'queued',pendingName:'Alice',nextChangeAt:0,retryAt:0});
 assert.equal(f.service.getSnapshot().displayName,before);assert.equal(f.service.getSnapshot().nickname.pendingName,'Alice');
 const queuedCount=socket.sent.length;assert.equal(f.service.setDisplayName('Replacement'),'Alice');assert.equal(socket.sent.length,queuedCount,'a saved pending draft cannot be overwritten');
 socket.message({type:'nickname-status',name:'Alice',status:'approved',nextChangeAt:Date.now()+86400000,retryAt:0});
 assert.equal(f.service.getSnapshot().displayName,'Alice');
 socket.message({type:'nickname-status',name:'Alice',status:'cooldown',nextChangeAt:Date.now()+86400000,retryAt:0});
 assert.equal(f.service.getSnapshot().displayName,'Alice');
 f.service.dispose();socket.message({type:'nickname-status',name:'Stale',status:'approved',nextChangeAt:0,retryAt:0});
 assert.notEqual(f.service.getSnapshot().displayName,'Stale');
});

test('existing local nickname migrates only after the moderation handshake without publishing the legacy initial',async()=>{
 const f=await joinedRoom(),socket=f.sockets.at(-1);
 socket.message({type:'nickname-status',name:'',status:'idle',nextChangeAt:0,retryAt:0});
 assert.deepEqual(socket.sent.at(-1),{type:'set-name',name:'Alice Wonder'});
 assert.equal(f.service.getSnapshot().nickname.status,'reviewing');
 f.service.dispose();
});
