import test, {before, after, afterEach} from 'node:test';
import assert from 'node:assert/strict';
import {existsSync, readFileSync} from 'node:fs';
import {mkdtemp, rm} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {installMountedDom} from './mounted-dom-environment.mjs';
const project = fileURLToPath(new URL('../../', import.meta.url)), require = createRequire(resolve(project, 'package.json'));
const {build} = require('esbuild');
let env, React, createRoot, MemoryRouter, owners, work;
const mounts = new Set(), fixtures = new Set();
const n = (selector, scope = env.document) => {const node = scope.querySelector(selector); assert.ok(node, `Missing ${selector}`); return node;};
const all = (selector, scope = env.document) => [...scope.querySelectorAll(selector)];
const t = (locale, key, params) => owners.translate(locale, key, params);
const act = fn => React.act(async () => {await fn?.(); for (let i = 0; i < 12; i++) await Promise.resolve();});
const defaultContext = {launched: false, th09NetworkOverlayOpen: false, launchStage: null, touchEnabled: false, touchMovementMode: 'joystick'};
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client')); ({MemoryRouter} = await import('react-router'));
  work = await mkdtemp(resolve(project, '.cache/room-views-'));
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export * from './app/components/room/index.ts';
    export * from './app/components/room/RoomPanel.tsx';
    export * from './app/components/room/presentation.ts';
    export * from './app/components/launcher/OptionsPanel.tsx';
    export * from './app/components/launcher/products.ts';
    export * from './app/components/settings/SettingsBody.tsx';
    export * from './app/models/game-settings.ts';
    export * from './app/services/multiplayer-room.ts';
    export * from './app/models/decisions.ts';
    export * from './app/i18n.tsx';
    export * from './src/contracts/product-catalog.mts';
    export * from './src/launcher/multiplayer-identity.mts';
    export * from './src/launcher/multiplayer-room-session.mts';
    export * from './src/launcher/multiplayer-preferences.mts';
    export * from './src/launcher/game-preferences.mts';
  `}, bundle: true, platform: 'node', format: 'esm', jsx: 'automatic', outfile: resolve(work, 'actual.mjs'), logLevel: 'silent',
  plugins: [{name: 'installed-deps-authored-mts', setup(ctx) {
    ctx.onResolve({filter: /^[^./]/}, args => ({path: pathToFileURL(require.resolve(args.path)).href, external: true}));
    ctx.onResolve({filter: /^\.\.?\/.*\.mjs$/}, args => {const path = resolve(dirname(args.importer), args.path).replace(/\.mjs$/, '.mts'); if (existsSync(path)) return {path};});
  }}]}); owners = await import(pathToFileURL(resolve(work, 'actual.mjs')));
});
afterEach(async () => {
  for (const m of mounts) await act(() => m.root.unmount()); mounts.clear();
  for (const f of fixtures) {f.service.dispose(); f.decisions.dispose();} fixtures.clear();
  env.document.body.replaceChildren(); env.document.body.className = '';
  env.window.matchMedia = query => ({matches: query.includes('prefers-reduced-motion'), media: query, addEventListener() {}, removeEventListener() {}}); globalThis.matchMedia = env.window.matchMedia;
  delete env.window.HTMLElement.prototype.animate;
  assert.deepEqual(env.errors.splice(0), [], 'No unexpected React or DOM errors');
});
after(async () => {env?.close(); if (work) await rm(work, {recursive: true, force: true});});
class Storage {values = new Map(); getItem(key) {return this.values.get(key) ?? null;} setItem(key, value) {this.values.set(key, String(value));} removeItem(key) {this.values.delete(key);}}
test('Room connection distinguishes initial connect, synchronization and actual reconnect', async () => {
  const f = await fixture({product: 'th11mp', locale: 'zh-CN'}).enter();
  const message = () => owners.roomConnectionMessage(f.service.getSnapshot(), key => t('zh-CN', key));
  assert.equal(message(), t('zh-CN', 'multiplayer.connectingRoom'));
  await act(() => f.socket.open());
  assert.equal(message(), t('zh-CN', 'multiplayer.syncingMembers'));
  await f.receive({predictionLimit: 12, adonisMode: 3});
  assert.equal(f.service.getSnapshot().room.synced, false);
  assert.equal(message(), t('zh-CN', 'multiplayer.syncingMembers'));
  await f.receive();
  assert.equal(f.service.getSnapshot().room.synced, true);
  await act(() => f.socket.close(1006));
  assert.equal(message(), t('zh-CN', 'multiplayer.reconnecting'));
});
class Socket {
  readyState = 0; sent = []; listeners = new Map(); closes = [];
  addEventListener(type, fn) {this.listeners.set(type, [...(this.listeners.get(type) || []), fn]);}
  emit(type, value = {}) {for (const fn of this.listeners.get(type) || []) fn(value);}
  open() {this.readyState = 1; this.emit('open');}
  message(room) {this.emit('message', {data: JSON.stringify({type: 'state', roomDirectory: {version: 1, controlModes: true}, room})});}
  send(value) {assert.equal(this.readyState, 1); this.sent.push(JSON.parse(value));}
  close(code = 1000) {this.readyState = 3; this.closes.push(code); this.emit('close', {code});}
}
function deferred() {let resolve, reject; const promise = new Promise((a, b) => {resolve = a; reject = b;}); return {promise, resolve, reject};}
function syntheticNetwork() {
  const listeners = new Set(), values = new Map(), retries = []; let revision = 0;
  const capabilities = {supported: true, rtcAvailable: true, turnConfigured: true};
  return {subscribe(fn) {listeners.add(fn); return () => listeners.delete(fn);}, getSnapshot: () => revision,
    metric: (peer, lane) => values.get(`${peer}:${lane}`) || {state: 'checking', rtt: null, jitter: null}, capabilities: () => capabilities,
    retry(peer) {retries.push(peer);}, retries, values, flags: capabilities, get subscriberCount() {return listeners.size;},
    async publish() {await act(() => {revision++; for (const fn of listeners) fn();});}};
}
function fixture({product = 'th06mp', locale = 'en', created = true, checkGame = null} = {}) {
  const persistent = new Storage(), session = new Storage(), sockets = [], events = [], preparations = [], timers = new Map(), decisions = owners.createDecisionStore(); let timer = 0;
  persistent.setItem(owners.multiplayerDisplayNameStorageKey, 'Alice');
  const identity = owners.createMultiplayerIdentityStore({persistentStorage: persistent, sessionStorage: session, randomWords: () => [1234567, 7654321]});
  const settings = {touchMovementMode: 'joystick', touchEnabled: false, mobileDevice: false, iceServers: []}, network = syntheticNetwork();
  const service = owners.createMultiplayerRoom({identity, sessions: owners.createMultiplayerRoomSessionStore({storage: session}), preferences: owners.createMultiplayerPreferenceStore({storage: persistent}), decisions,
    memberId: () => 'synthetic-member', createSocket: () => {const socket = new Socket(); sockets.push(socket); return socket;}, relayUrl: () => 'wss://relay.invalid/socket',
    settings: () => settings, setMovementMode: value => {settings.touchMovementMode = value;}, translate: (key, params) => t(locale, key, params), notify: message => events.push(['notify', message]),
    prepareResources: (context, report) => {const task = deferred(); preparations.push({context, report, ...task}); return task.promise;},
    preparationFailed: error => events.push(['prepare-error', error]), beginManualImport: context => events.push(['import', context]),
    launch: async context => events.push(['launch', context]), checkGame: async context => {events.push(['check', context]); if (checkGame) await checkGame(context);}, operationFailed: (...args) => events.push(['operation-error', ...args]),
    isLaunched: () => false, routes: Object.fromEntries(['restore', 'enter', 'settleInvite', 'leave'].map(kind => [kind, value => events.push([kind, value])])),
    network: {reset: reconnecting => events.push(['network-reset', reconnecting]), receive: value => events.push(['network-receive', value]), minimumRtt: () => 43},
    online: () => true, visible: () => true, random: () => 0, setTimeout: (fn, delay) => {timers.set(++timer, {fn, delay}); return timer;}, clearTimeout: id => timers.delete(id)});
  const f = {service, network, decisions, sockets, settings, events, preparations, product, locale, timers, persistent,
    get id() {return identity.lobbyClientId(product);}, get socket() {return sockets.at(-1);},
    async enter() {await act(() => service.enterRoom(product, '1234', created)); return f;},
    async receive(patch = {}) {await act(() => f.socket.message({code: '1234', playerCount: 2, difficulty: 1, phase: 'lobby', inputDelay: 0, startSerial: 0,
      ...(product === 'th11mp' ? {adonisMode: 0, predictionLimit: 12} : {}),
      ...(product === 'th15mp' ? {adonisMode: 1, predictionLimit: 0} : {}),
      seats: [{clientId: f.id, name: 'A', loadout: 0, ready: false, controlMode: 'normal'}, null], spectators: [], ...patch})); return f;},
    async live(patch = {}) {await f.enter(); await act(() => f.socket.open()); await f.receive(patch); return f;},
    async report(progress) {await act(() => preparations.at(-1).report({status: 'preparing', stage: 'package', percent: null, error: '', ...progress}));},
  }; fixtures.add(f); return f;
}
async function mount(Component, initial, {locale = 'en', strict = false} = {}) {
  let props = initial; const container = env.document.createElement('div'); env.document.body.append(container); const root = createRoot(container);
  const render = () => act(() => {let tree = React.createElement(owners.LocaleProvider, {locale}, React.createElement(Component, props)); if (strict) tree = React.createElement(React.StrictMode, null, tree); root.render(tree);});
  const m = {root, container, async update(patch) {props = {...props, ...patch}; await render();}, async unmount() {await act(() => root.unmount()); mounts.delete(m);}};
  mounts.add(m); await render(); return m;
}
async function mountRoom(f, extra = {}, options = {}) {
  const host=env.document.createElement('section'),stage=env.document.createElement('div'),viewport=env.document.createElement('div');viewport.className='game-viewport';stage.append(viewport);host.append(stage);env.document.body.append(host);
  const intents = []; const m = await mount(owners.RoomView, {service: f.service, network: f.network, context: defaultContext, workspaceHost:host, workspaceStage:stage, assetUrl: path => path,
    onLeave: () => {intents.push(['leave']); f.service.leave();}, onCopyRoomCode: code => intents.push(['copy', code]), onGuide: () => intents.push(['guide']),
    onOpenSettings: () => intents.push(['settings']), settingsOpen: false, panel: null, onOpenPanel: (...args) => intents.push(['panel', ...args]), onClosePanel: () => intents.push(['close']), ...extra}, {locale: f.locale, ...options}); return {m, intents};
}
const click = (selector, scope) => act(() => n(selector, scope).click());
test('TH09 local charge rule defaults on, restores native Replay by room agreement, and locks during play', async () => {
  const f=await fixture({product:'th09mp',locale:'zh-CN'}).live();await mountRoom(f,{panel:{kind:'game'}});
  const toggle=n('#mpDisableLocalChargePriority');
  assert.equal(toggle.getAttribute('aria-checked'),'false');
  assert.equal(toggle.getAttribute('aria-label'),'关闭本地优先的蓄力条进度');
  assert.equal(n('#mpLocalChargeReplayHint').textContent,'启用该选项后可以得到兼容原版的 Replay，但蓄力条的显示可能是不准确的');
  assert.equal(f.service.runtimeOptions().netplayDisableLocalChargePriority,false);
  await click('#mpDisableLocalChargePriority');
  assert.equal(f.socket.sent.findLast(m=>m.type==='settings').disableLocalChargePriority,true);
  await f.receive({disableLocalChargePriority:true});
  assert.equal(toggle.getAttribute('aria-checked'),'true');
  assert.equal(f.service.runtimeOptions().netplayDisableLocalChargePriority,true);
  await f.receive({disableLocalChargePriority:true,phase:'running'});
  assert.equal(toggle.disabled,true);
  const sent=f.socket.sent.length;await act(()=>f.service.setRoomSettings({disableLocalChargePriority:false}));assert.equal(f.socket.sent.length,sent);
  const other=await fixture({product:'th07mp'}).live();const {m}=await mountRoom(other,{panel:{kind:'game'}});
  assert.equal(all('#mpDisableLocalChargePriority',m.container).length,0);
  assert.equal('netplayDisableLocalChargePriority' in other.service.runtimeOptions(),false);
});
test('TH09 auto-save preference reaches only player Runtime options and survives leaving', async () => {
  const f = await fixture({product:'th09mp'}).live();
  // Seat identity is owned by the fixture, not the rendered nickname.
  await f.receive({seats:[{clientId:f.id,name:'xxx',loadout:0,ready:false},{clientId:'peer12345',name:'abdjfal',loadout:1,ready:false}]});
  const preferences=owners.createMultiplayerPreferenceStore({storage:f.persistent});
  assert.equal(f.service.runtimeOptions().netplayReplayAutoSave,true);
  assert.deepEqual(f.service.runtimeOptions().netplayReplayNames,['xxx','abdjfal']);
  preferences.persistReplayAutoSave('th09mp',false);
  assert.equal(f.service.runtimeOptions().netplayReplayAutoSave,false);
  await act(() => f.service.leave()); await f.enter();
  assert.equal(preferences.replayAutoSave('th09mp'),false);
  const other=await fixture({product:'th07mp'}).live();await mountRoom(other,{panel:{kind:'personal'}});
  assert.equal(all('#mpReplayAutoSave').length,0);
  assert.equal('netplayReplayAutoSave' in other.service.runtimeOptions(),false);
});
function compare(f) {
  const state=f.service.getSnapshot(),room=state.room,config=owners.multiplayerConfigForProduct(f.product);
  assert.equal(n('#mpRoomCode').textContent,room.code);
  assert.equal(n('#mpRoomDifficultyBadge').textContent,config.difficulties[room.difficulty] || 'Normal');
  assert.equal(n('#mpRoomView').classList.contains('room-workspace-view'),true);
  const ids=all('[id]').map(node=>node.id);assert.equal(new Set(ids).size,ids.length,'room has one set of live controls');
  for (const [index,seat] of (room.seats || []).slice(0,room.playerCount).entries()) {
    if(seat)assert.equal(n(`[data-mp-seat="${index}"] .mp-seat-name`).textContent,seat.name);
  }
}
for (const locale of ['zh-CN', 'en']) test(`Room workspace follows authoritative state: ${locale}, unsynced/owner/offline/unseated/spectator/ready/running`, async () => {
  const f = await fixture({locale}).enter(), {m} = await mountRoom(f, {panel: {kind: 'game'}});
  compare(f, {panel: {kind: 'game'}});
  await act(() => f.socket.open()); await f.receive(); compare(f, {panel: {kind: 'game'}});
  await f.receive({playerCount: 3, seats: [{clientId: f.id, name: 'A', loadout: 0, ready: true}, {clientId: 'peer-b123', name: '魔', loadout: 3, ready: true, offline: true, controlMode: 'touch', resource: {status: 'failed', stage: 'package', percent: null}}, null], spectators: [{clientId: 'watch-c123', name: '幽'}], spectatorCount: 4});
  compare(f, {panel: {kind: 'game'}});
  await f.receive({seats: [{clientId: 'host12345', name: 'H', loadout: 1, ready: false}, null]}); compare(f, {panel: {kind: 'game'}});
  await f.receive({seats: [{clientId: 'host12345', name: 'H', loadout: 1}, null], spectators: [{clientId: f.id, name: 'A'}, {clientId: 'watch12345', name: 'W'}]});
  await m.update({panel: {kind: 'spectators'}}); compare(f, {panel: {kind: 'spectators'}});
  await f.receive({phase: 'running', seats: [{clientId: f.id, name: 'A', loadout: 0, ready: true}, {clientId: 'peer12345', name: 'B', loadout: 2, ready: true}]}); compare(f, {panel: {kind: 'spectators'}});
  await act(() => f.socket.close(1006)); compare(f, {panel: {kind: 'spectators'}});
});
for (const locale of ['zh-CN', 'en']) test(`All catalog multiplayer products preserve exact ${locale} roster, controls, difficulty and timing`, async () => {
  for (const product of owners.PRODUCT_IDS.filter(owners.isMultiplayerProductId)) {
    const f = await fixture({product, locale}).live(); const {m} = await mountRoom(f, {panel: {kind: 'game'}});
    const config = owners.multiplayerConfigForProduct(product);
    for (const loadout of [0, config.loadouts.length - 1]) {await f.receive({difficulty: config.difficulties.length - 1, seats: [{clientId: f.id, name: 'A', loadout, ready: false}, {clientId: 'peer12345', name: 'B', loadout: -1, ready: false, controlMode: 'cheat'}]}); if (config.inGameSelection) {
      assert.equal(n('#mpRoomDifficultyBadge').textContent, config.difficulties[f.service.getSnapshot().room.difficulty]);
      assert.equal(all('[data-mp-difficulty]').length,0);
      assert.equal(all('[data-mp-difficulty]').length,0);
      assert.equal(all('#mpLoadoutPrevSeat, #mpLoadoutNextSeat, #mpLoadoutPrev, #mpLoadoutNext').length,0);
      assert.equal(f.service.runtimeOptions().netplayInGameSelection,true);
      assert.match(n('#mpRoomSettings').textContent,locale==='zh-CN'?/双方/:/Both players/);
    } else compare(f, {panel: {kind: 'game'}});}
    await m.unmount();
  }
});

for (const locale of ['zh-CN', 'en']) test(`Original ${locale} preparation rendering uses exact local percent, status precedence and real cancellation/retry`, async () => {
  const f = await fixture({locale}).live(), {m} = await mountRoom(f, {panel: {kind: 'personal'}});
  const originalInput = n('#mpDisplayName'), originalReady = n('#mpReady');
  for (const progress of [
    {loaded: 137 * 1048576, total: 1000 * 1048576, title: 'Synthetic package'},
    {loaded: 1048576, total: 0, title: ''}, {loaded: 0, total: 0},
    {loaded: 999, total: 1}, {stage: 'runtime', loaded: 1, total: 2},
    {status: 'failed', error: 'Synthetic preparation error'}, {status: 'ready', stage: 'runtime'},
    {status: 'importing'}, {status: 'cancelled'},
  ]) {await f.report(progress); compare(f, {panel: {kind: 'personal'}}); assert.ok(n('#mpDisplayName') === originalInput); assert.ok(n('#mpReady') === originalReady);}
  await f.report({loaded: 137, total: 1000});
  assert.equal(n('.mp-seat-resource-label').textContent, t(locale, 'room.peerDownloading', {percent: 14}));
  assert.equal(f.socket.sent.findLast(message => message.type === 'resource-progress').percent, 10, 'Wire percent buckets do not replace local precise display');
  // Current-program preparation must finish before a Ready intent is sent.
  await click('#mpReady'); assert.equal(f.service.getSnapshot().ready, false);
  assert.equal(f.socket.sent.some(message => message.type === 'set-ready' && message.ready), false);
  await click('#mpRoomResourceCancel'); assert.equal(f.preparations[0].context.signal.aborted, true); assert.equal(f.events.filter(([kind]) => kind === 'import').length, 1);
  assert.equal(f.service.getSnapshot().ready, false); assert.equal(n('#mpReady').disabled, true); compare(f, {panel: {kind: 'personal'}});
  await act(() => f.preparations[0].resolve());
  assert.equal(f.socket.sent.some(message => message.type === 'set-ready' && message.ready), false, 'cancelled preparation cannot complete a stale Ready intent');
  await click('#mpRoomResourceRetry'); assert.equal(f.preparations.length, 2); assert.equal(n('#mpReady').disabled, true);
  await act(() => f.preparations.at(-1).reject(Error('Synthetic transfer rejected'))); assert.equal(n('.room-preparation-caption p').textContent, 'Synthetic transfer rejected');
  await click('#mpRoomResourceRetry'); assert.equal(f.preparations.length, 3);
  await act(() => f.preparations.at(-1).resolve()); assert.equal(n('.room-preparation-caption strong').textContent, t(locale, 'room.workspaceReady'));
  for (const launchStage of ['preparing', 'path']) {await m.update({context: {...defaultContext, launchStage}}); compare(f, {panel: {kind: 'personal'}, context: {...defaultContext, launchStage}});}
});

test('Room header and panel triggers reach required host edges with original code and peer identity', async () => {
  const f = await fixture().live({seats: null}); await f.receive({seats: [{clientId: f.id, name: 'A', loadout: 0}, {clientId: 'peer12345', name: 'B', loadout: 2}]});
  const {m, intents} = await mountRoom(f);
  await click('#mpCopyRoomCode'); await click('#mpRoomGuideOpen'); await click('#mpSettingsRoomDrawerToggle'); await click('#roomWorkspaceTab-game'); await click('.room-spectator-preview-trigger');
  await click('[data-mp-seat="0"] .mp-seat-edit'); await click('[data-mp-seat="1"] .mp-seat-latency');
  assert.deepEqual(intents, [['copy', '1234'], ['guide'], ['settings'], ['panel', 'spectators', undefined], ['panel', 'personal', undefined], ['panel', 'network', 'peer12345']]);
  await m.update({settingsOpen: true}); assert.equal(n('#mpRoomView').hasAttribute('inert'), true); assert.equal(n('#mpSettingsRoomDrawerToggle').hidden, false);
  await m.update({settingsOpen: false, settingsClosing: true}); assert.equal(n('#mpRoomView').hasAttribute('inert'), false); assert.equal(n('#mpSettingsRoomDrawerToggle').hidden, false);
  await m.update({settingsClosing: false}); assert.equal(n('#mpSettingsRoomDrawerToggle').hidden, false);
  await click('#mpLeaveRoom'); assert.equal(f.service.getSnapshot().room, null); assert.equal(f.events.at(-1)[0], 'leave'); assert.equal(env.document.querySelector('#mpRoomView'), null);
});

test('Real join, stand, spectator, loadout and become-host button ports retain relay authority', async () => {
  const f = await fixture({created: false}).live({seats: [null, null]}), {m} = await mountRoom(f, {panel: {kind: 'personal'}});
  await click('[data-mp-seat="1"] .mp-seat-drop button'); assert.equal(f.socket.sent.at(-1).type, 'take-seat'); assert.equal(f.socket.sent.at(-1).seat, 1);
  assert.equal(f.service.getSnapshot().seat, null, 'Seating waits for relay snapshot');
  await f.receive({seats: [null, {clientId: f.id, name: 'A', loadout: 0}]});
  await click('#mpLoadoutNextSeat'); assert.deepEqual(f.socket.sent.at(-1), {type: 'set-loadout', loadout: 1});
  await click('#mpLoadoutPrevSeat'); assert.deepEqual(f.socket.sent.at(-1), {type: 'set-loadout', loadout: 0});
  await click('#mpStandUp'); assert.deepEqual(f.socket.sent.at(-1), {type: 'stand-up'}); assert.equal(f.service.getSnapshot().seat, null);
  await m.update({panel: {kind: 'spectators'}}); await click('#mpSpectatorJoin'); assert.deepEqual(f.socket.sent.at(-1), {type: 'spectate', name: 'A'});
  assert.equal(n('#mpSpectatorJoin').textContent, t('en', 'multiplayer.leaveSpectator'));
  await click('#mpSpectatorJoin'); assert.deepEqual(f.socket.sent.at(-1), {type: 'leave-spectator'});
  await m.update({panel: {kind: 'game'}}); await click('#mpTakeHostSeat'); assert.equal(f.socket.sent.at(-1).seat, 0); assert.equal(f.socket.sent.at(-1).type, 'take-seat');
  await m.update({panel: {kind: 'personal'}}); await click('#mpLoadoutNext'); assert.equal(f.service.getSnapshot().preferredLoadout, 1);
  await click('#mpLoadoutPrev'); assert.equal(f.service.getSnapshot().preferredLoadout, 0);
});

test('Room settings remain synchronized while owner and phase change', async () => {
 const f=await fixture().live();await mountRoom(f);await click('#roomWorkspaceTab-game');
 const select=async(selector,value)=>act(()=>{const node=n(selector);node.value=value;node.dispatchEvent(new env.window.Event('change',{bubbles:true}));});
 await select('#mpRoomPlayerCount','3');assert.equal(f.service.getSnapshot().room.playerCount,3);assert.equal(n('#mpSeatStage').dataset.playerCount,'3');
 await click('[data-mp-difficulty="3"]');assert.equal(n('#mpRoomDifficultyBadge').textContent,'Lunatic');
 for(const [selector,key,value] of [['#mpRoomVisibility','visibility','private'],['#mpRoomCheatToggle','disableCheatMovement',true],['#mpRoomChallengeToggle','challengeMode',true]]) {
  if(key==='visibility')await select(selector,value);else await click(selector);
  assert.equal(f.socket.sent.at(-1).type,'settings');assert.equal(f.socket.sent.at(-1)[key],value);assert.notEqual(f.service.getSnapshot().room[key],value);
 }
 await f.receive({phase:'running'});assert.equal(n('#mpRoomVisibility').disabled,true);assert.equal(n('#mpRoomCheatToggle').disabled,true);
 await f.receive({seats:[{clientId:'other12345',name:'O',loadout:1},{clientId:f.id,name:'A',loadout:0}]});
 assert.equal(n('#roomWorkspacePanel-game').hidden,false);assert.equal(n('#mpRoomPlayerCount').disabled,true);
 await f.receive();assert.equal(n('#mpRoomPlayerCount').disabled,false);await click('#roomWorkspaceTab-chat');assert.equal(f.service.getSnapshot().roomSettingsOpen,false);
});

test('Ready/start gates use synchronized occupied seats and check-game calls real preparation/runtime port', async () => {
  const checked = deferred();
  const f = await fixture({checkGame: () => checked.promise}).live(), {m} = await mountRoom(f, {panel: {kind: 'personal'}});
  assert.equal(n('#mpReady').disabled, true, 'Ready requires prepared resources');
  assert.equal(f.socket.sent.some(message => message.type === 'set-ready'), false, 'Ready waits for actual preparation completion');
  await f.report({status:'ready',stage:'runtime',percent:100});await act(() => f.preparations[0].resolve());await click('#mpReady');await f.report({status:'ready',stage:'runtime',percent:100});await act(()=>f.preparations.at(-1).resolve());
  assert.equal(f.socket.sent.findLast(message=>message.type==='set-ready')?.ready,true); assert.equal(n('#mpStartGame').hidden, false); assert.equal(n('#mpStartGame').disabled, true);
  assert.equal(n('#mpCheckGame').disabled, true);
  await f.receive({seats: [{clientId: f.id, name: 'A', loadout: 0, ready: true}, {clientId: 'peer12345', name: 'B', loadout: 2, ready: true}]});
  assert.equal(n('#mpStartGame').disabled, false); await click('#mpStartGame'); assert.equal(f.socket.sent.at(-1).type, 'start');
  await f.receive({seats: [{clientId: f.id, name: 'A', loadout: 0, ready: true}, {clientId: 'peer12345', name: 'B', loadout: 2, ready: true, offline: true}]}); assert.equal(n('#mpStartGame').disabled, true);
  await f.receive(); await click('#mpCheckGame'); assert.equal(f.service.getSnapshot().checkBusy, true); assert.equal(n('#mpReady').disabled, true); assert.equal(n('#mpCheckGame').textContent, t('en', 'multiplayer.checkingGame'));
  await act(() => checked.resolve()); assert.equal(f.events.filter(([kind]) => kind === 'check').length, 1); assert.equal(f.service.getSnapshot().checkBusy, false);
  await m.update({context: {...defaultContext, th09NetworkOverlayOpen: true}}); assert.equal(n('#mpCheckGame').hidden, true);
  await act(() => f.socket.close(1006)); assert.equal(n('#mpReady').disabled, true); assert.equal(n('#mpStandUp').disabled, false); assert.equal(n('#mpLoadoutNextSeat').disabled, false);
});

test('Seat/spectator removal calls real danger decisions and rejects a departed stale target', async () => {
  const f = await fixture().live({seats: null});
  const room = {seats: [{clientId: f.id, name: 'A', loadout: 0}, {clientId: 'peer12345', name: 'B', loadout: 1}], spectators: [{clientId: 'watch12345', name: 'W'}]};
  await f.receive(room); await mountRoom(f, {panel: {kind: 'spectators'}});
  await click('[data-mp-seat="1"] .room-member-menu-trigger');await click('.room-member-menu [data-phrase="remove-player"]'); assert.equal(f.decisions.getSnapshot().tone, 'danger');
  assert.equal(f.decisions.getSnapshot().message, t('en', 'room.removePlayerConfirm', {seat: 2}));
  await act(() => f.decisions.resolve('confirm')); assert.deepEqual(f.socket.sent.at(-1), {type: 'remove-player', seat: 1, clientId: 'peer12345'});
  await click('.mp-spectator-remove'); assert.equal(f.decisions.getSnapshot().message, t('en', 'room.removeSpectatorConfirm'));
  await act(() => f.decisions.resolve('confirm')); assert.deepEqual(f.socket.sent.at(-1), {type: 'remove-spectator', clientId: 'watch12345'});
  await click('[data-mp-seat="1"] .room-member-menu-trigger');await click('.room-member-menu [data-phrase="remove-player"]'); await f.receive({seats: [{clientId: f.id, name: 'A', loadout: 0}, {clientId: 'successor', name: 'S', loadout: 2}]});
  const count = f.socket.sent.filter(message => message.type === 'remove-player').length; await act(() => f.decisions.resolve('confirm'));
  assert.equal(f.socket.sent.filter(message => message.type === 'remove-player').length, count);
});

test('Nickname preserves focused draft, commits native change/blur with normalization, stays mounted and editable offline', async () => {
  const f = await fixture().live(); await mountRoom(f, {panel: {kind: 'personal'}}, {strict: true}); const input = n('#mpDisplayName');
  await act(() => {input.focus(); input.value = 'Draft name'; input.dispatchEvent(new env.window.Event('input', {bubbles: true}));});
  assert.equal(f.service.getSnapshot().displayName, 'Alice');
  await act(() => f.service.setDisplayName('External name')); assert.equal(input.value, 'Draft name');
  await f.report({loaded: 5, total: 100}); await f.network.publish(); assert.ok(n('#mpDisplayName') === input); assert.equal(input.value, 'Draft name'); assert.ok(env.document.activeElement === input);
  await act(() => {input.value = '  Beatrice  '; input.dispatchEvent(new env.window.Event('change', {bubbles: true}));});
  assert.equal(f.service.getSnapshot().displayName, 'Beatrice'); assert.equal(input.value, 'Beatrice'); assert.deepEqual(f.socket.sent.at(-1), {type: 'set-name', name: 'B'});
  const before = f.socket.sent.filter(message => message.type === 'set-name').length;
  await act(() => {input.value = 'Charlie'; n('#mpRoomPanelClose').focus();});
  assert.equal(f.service.getSnapshot().displayName, 'Charlie'); assert.equal(f.socket.sent.filter(message => message.type === 'set-name').length, before + 1, 'StrictMode did not duplicate native change listener');
  await act(() => f.socket.close(1006)); assert.equal(input.disabled, false);
  await act(() => {input.focus(); input.value = 'Offline'; input.blur();}); assert.equal(f.service.getSnapshot().displayName, 'Offline');
  await act(() => f.service.setDisplayName('Restored')); assert.equal(input.value, 'Restored');
});

for (const locale of ['zh-CN', 'en']) test(`Synthetic network metrics match executed original ${locale} RTT/jitter/quality, selected peer, empty states and retry ports`, async () => {
  const f = await fixture({locale}).live({seats: null});
  const seats = [{clientId: f.id, name: 'A', loadout: 0}, {clientId: 'peer12345', name: 'B', loadout: 2}, {clientId: 'peer67890', name: 'C', loadout: 3}];
  await f.receive({playerCount: 3, seats});
  for (const [lane, rtt, jitter] of [['direct', 0.4, 2.7], ['turn', 100.4, 5.1], ['relay', 201.8, null]]) f.network.values.set(`peer12345:${lane}`, {state: 'connected', rtt, jitter});
  const panel = {kind: 'network', peerId: 'peer12345'}, {m} = await mountRoom(f, {panel}); compare(f, {panel});
  assert.deepEqual(all('.mp-network-metric').map(node => node.dataset.quality), ['good', 'fair', 'poor']);
  assert.deepEqual(all('.mp-network-metric strong').map(node => node.textContent), ['1 ms', '100 ms', '202 ms']);
  await click('#mpRoomNetworkRetry'); assert.deepEqual(f.network.retries, ['peer12345']);
  const latency = n('[data-mp-seat="1"] .mp-seat-latency'), input = n('#mpDisplayName'); await act(() => latency.focus());
  f.network.values.set('peer12345:direct', {state: 'connected', rtt: 77.6, jitter: 1.2}); await f.network.publish();
  assert.ok(n('[data-mp-seat="1"] .mp-seat-latency') === latency); assert.ok(n('#mpDisplayName') === input); assert.ok(env.document.activeElement === latency); compare(f, {panel});
  await m.update({panel: {kind: 'network'}}); await click('#mpRoomNetworkRetry'); assert.deepEqual(f.network.retries, ['peer12345', undefined]);
  for (const flags of [{supported: false, rtcAvailable: true, turnConfigured: true}, {supported: true, rtcAvailable: false, turnConfigured: true}, {supported: true, rtcAvailable: true, turnConfigured: false}]) {
    Object.assign(f.network.flags, flags); await f.network.publish(); compare(f, {panel: {kind: 'network'}});
  }
  await m.update({panel: {kind: 'network', peerId: 'departed123'}}); compare(f, {panel: {kind: 'network', peerId: 'departed123'}}); assert.equal(n('.mp-network-empty').textContent, t(locale, 'room.peerLeft'));
  await m.update({panel}); await f.receive({playerCount: 3, seats: [seats[0], {...seats[1], offline: true}, seats[2]]}); compare(f, {panel});
  assert.deepEqual(all('.mp-network-metric strong').map(node => node.textContent), Array(3).fill(t(locale, 'room.unavailable')));
  for (const context of [{...defaultContext, launched: true}, {...defaultContext, launched: true, th09NetworkOverlayOpen: true}]) {await m.update({context}); compare(f, {panel, context});}
  await m.update({context: defaultContext}); await f.receive({seats: [null, seats[1]]}); compare(f, {panel}); assert.equal(n('#mpRoomNetworkRetry').disabled, true);
  await f.receive(); compare(f, {panel}); assert.equal(n('.mp-network-empty').textContent, t(locale, 'room.waitPeer'));
  await act(() => f.socket.close(1006)); compare(f, {panel}); assert.equal(n('.mp-network-empty').textContent, t(locale, 'multiplayer.reconnecting'));
});

test('Measured rollback button routes through real room owner without overwriting selected delay; applied running value wins', async () => {
  const product = owners.PRODUCT_IDS.find(id => owners.isMultiplayerProductId(id) && owners.multiplayerConfigForProduct(id).inputTiming?.measuredStartup);
  assert.ok(product, 'Measured product must exist'); const f = await fixture({product}).live(); await mountRoom(f);
  assert.equal(f.service.getSnapshot().rollbackEnabled,true);await click('#mpRollbackToggle');
  await act(() => f.service.setTimingChoice(7)); await click('#mpRollbackToggle'); assert.equal(f.service.getSnapshot().rollbackEnabled, true); assert.equal(f.service.getSnapshot().timingChoice, 7);
  assert.equal(n('#mpRollbackToggle').getAttribute('aria-checked'), 'true'); await click('#mpRollbackToggle'); assert.equal(f.service.getSnapshot().rollbackEnabled, false);
  await f.receive({phase: 'running', adonisMode: 2}); assert.equal(n('#mpRollbackToggle').getAttribute('aria-checked'), 'true'); assert.equal(n('#mpRollbackToggle').disabled, true);
  await f.receive({phase: 'running', adonisMode: 1}); assert.equal(n('#mpRollbackToggle').getAttribute('aria-checked'), 'false');
  await f.receive({seats: [{clientId: 'host12345', name: 'H', loadout: 0}, {clientId: f.id, name: 'A', loadout: 1}]}); assert.equal(n('#mpInputTiming').hidden, false); assert.equal(n('#mpRollbackToggle').disabled, true);
});

test('seated players may enter the spectator lounge before start, never during a live run', async () => {
  const f = await fixture().live(); await mountRoom(f, {panel:{kind:'spectators'}});
  assert.equal(n('#mpSpectatorJoin').hidden, false); assert.equal(n('#mpSpectatorJoin').disabled, false);
  await click('#mpSpectatorJoin');
  assert.equal(f.service.getSnapshot().seat, null); assert.equal(f.service.getSnapshot().spectatorRequested, true);
  assert.equal(f.socket.sent.at(-1).type, 'spectate');
  await f.receive({phase:'running'});
  assert.equal(n('#mpSpectatorJoin').hidden, true);
  const count = f.socket.sent.length; await act(() => f.service.spectate()); assert.equal(f.socket.sent.length, count);
});

test('difficulty labels use the approved plain text without texture images', async () => {
  const f = await fixture({product:'th15mp'}).live(); await mountRoom(f);
  for (const [difficulty, name] of ['Easy','Normal','Hard','Lunatic','Extra'].entries()) {
    await f.receive({difficulty});
    const label=n('#mpRoomDifficultyBadge'), image=label.querySelector('img');
    assert.equal(label.textContent,name);
    assert.equal(image, null);
  }
  assert.equal(n('#mpRoomDifficultyBadge').textContent,'Extra');
  assert.equal(n('#mpRoomDifficultyBadge').classList.contains('difficulty-texture'),false);
});

test('guests see authoritative rollback and delay values but cannot change them', async () => {
  const f = await fixture({product:'th11mp'}).live({adonisMode:1,timingChoice:5});
  // Resolve the generated local identity without relying on a synthetic name.
  await f.receive({adonisMode:1,timingChoice:5,seats:[{clientId:'host12345',name:'Host',loadout:0},{clientId:f.id,name:'Guest',loadout:1}]});
  await mountRoom(f);
  assert.equal(n('#mpRollbackToggle').hidden,false); assert.equal(n('#mpRollbackToggle').disabled,true);
  assert.equal(n('#mpRollbackToggle').getAttribute('aria-checked'),'false');
  assert.equal(n('#mpInputDelaySetting').hidden,false); assert.equal(n('#mpInputDelay').value,'5'); assert.equal(n('#mpInputDelay').disabled,true);
  const count=f.socket.sent.length; await click('#mpRollbackToggle'); await act(()=>f.service.setTimingChoice(7)); assert.equal(f.socket.sent.length,count);
  await f.receive({adonisMode:0,timingChoice:2,seats:[{clientId:'host12345',name:'Host',loadout:0},{clientId:f.id,name:'Guest',loadout:1}]});
  assert.equal(n('#mpRollbackToggle').getAttribute('aria-checked'),'true'); assert.equal(n('#mpInputDelaySetting').hidden,true);
});

test('missing resources never render a default filled or completed resource bar', async () => {
  const f=await fixture().live();await mountRoom(f);
  await f.report({status:'failed',stage:'package',percent:null,loaded:undefined,total:undefined,error:'Missing game package'});
  for(const selector of ['#mpRoomResourceFill','#mpRoomLoadingFill'])assert.equal(n(selector).style.width,'0%');
  for(const selector of ['#mpRoomResourceTrack','#mpRoomLoadingTrack']){
    assert.equal(n(selector).getAttribute('aria-valuenow'),'0');
    assert.equal(n(selector).classList.contains('indeterminate'),false);
  }
  for(const row of all('.mp-seat-resource .mp-resource-row'))assert.equal(row.querySelector('.mp-room-resource-track>span').style.width,'0%');
  await f.report({status:'preparing',stage:'package',percent:null,error:''});
  assert.equal(n('#mpRoomResourceTrack').getAttribute('aria-valuenow'),null);
  assert.equal(n('#mpRoomLoadingTrack').getAttribute('aria-valuenow'),'0');
  assert.equal(n('#mpRoomResourceTrack').parentElement.querySelector('.mp-resource-percent').textContent,'—');
});

test('room shows two real resource stages, names/roles and challenge difficulty beside the code', async () => {
  const f = await fixture().live({challengeMode:true}); await mountRoom(f);
  assert.equal(n('.room-workspace-meta .room-challenge-on').textContent,t('en','room.challengeEnabled'));
  assert.equal(n('[data-mp-seat="0"] .mp-seat-name').textContent,'A');
  assert.ok(n('[data-mp-seat="0"] .mp-seat-loadout').textContent.length > 0);
  await f.report({loaded:25,total:100});
  assert.equal(all('#mpRoomResourceProgress [role="progressbar"]').length,2);
  assert.equal(n('#mpRoomResourceTrack').getAttribute('aria-valuenow'),'25');
  assert.equal(n('#mpRoomLoadingTrack').getAttribute('aria-valuenow'),'0');
  await f.report({stage:'runtime'});
  assert.equal(n('#mpRoomResourceTrack').getAttribute('aria-valuenow'),'100');
  assert.equal(n('#mpRoomLoadingTrack').getAttribute('aria-valuenow'),null);
  await f.report({status:'ready',stage:'runtime',percent:100});
  assert.equal(n('#mpRoomLoadingTrack').getAttribute('aria-valuenow'),'100');
});
function animationFixture() {
  globalThis.matchMedia = env.window.matchMedia = query => ({matches: false, media: query, addEventListener() {}, removeEventListener() {}});
  const animations = []; env.window.HTMLElement.prototype.animate = function (keyframes, options) {const done = deferred(), record = {node: this, keyframes, options, cancelled: false, finished: done.promise, cancel() {record.cancelled = true; done.reject(Error('Synthetic animation cancelled'));}, finish: done.resolve}; done.promise.catch(() => {}); animations.push(record); return record;}; return animations;
}
async function panelFixture({strict = false} = {}) {
  const trigger = env.document.createElement('button'); trigger.textContent = 'Original trigger'; env.document.body.append(trigger); let requests = 0, closures = 0;
  const m = await mount(owners.RoomPanel, {open: true, kind: 'personal', onCloseRequest: () => requests++, onClosed: () => closures++, trigger: {current: trigger}, children: React.createElement('button', {id: 'child-action'}, 'Child')}, {strict});
  return {m, trigger, get requests() {return requests;}, get closures() {return closures;}, node: n('#mpRoomPanel')};
}
test('Native modal Escape/backdrop request close; committed 160ms closure restores the exact original trigger', async () => {
  const animations = animationFixture(), h = await panelFixture(); assert.equal(h.node.open, true);
  const cancel = new env.window.Event('cancel', {cancelable: true}); await act(() => h.node.dispatchEvent(cancel)); assert.equal(cancel.defaultPrevented, true); assert.equal(h.requests, 1); assert.equal(h.node.open, true);
  await click('#child-action'); assert.equal(h.requests, 1);
  for (const [clientX, clientY, expected] of [[0, 0, 1], [1280, 800, 1], [-1, 20, 2], [1281, 20, 3]]) {
    await act(() => h.node.dispatchEvent(new env.window.MouseEvent('click', {bubbles: true, clientX, clientY}))); assert.equal(h.requests, expected, 'Only strictly outside dialog bounds dismisses');
  }
  await h.m.update({open: false}); assert.equal(h.node.open, true); assert.equal(h.closures, 0); assert.equal(animations.length, 1);
  assert.deepEqual(animations[0].options, {duration: 160, easing: 'cubic-bezier(.4,0,1,1)'});
  assert.deepEqual(animations[0].keyframes, [{opacity: 1, transform: 'translateY(0) scale(1)'}, {opacity: 0, transform: 'translateY(14px) scale(.985)'}]);
  await act(() => animations[0].finish()); assert.equal(h.node.open, false); assert.equal(h.closures, 1); assert.ok(env.document.activeElement === h.trigger);
});
test('Reopen during native close cancels stale animation; StrictMode subscriptions and closing promise clean up on unmount', async () => {
  const animations = animationFixture(), h = await panelFixture({strict: true}); assert.equal(h.node.open, true);
  await h.m.update({open: false}); assert.equal(animations.length, 1);
  await h.m.update({open: true, kind: 'network', peerId: 'peer12345'}); assert.equal(animations[0].cancelled, true); assert.equal(h.node.open, true); assert.equal(h.closures, 0);
  await act(() => animations[0].finish()); assert.equal(h.node.open, true); assert.equal(h.closures, 0);
  await h.m.update({open: false}); await h.m.unmount(); assert.equal(animations[1].cancelled, true); await act(() => animations[1].finish()); assert.equal(h.closures, 0);
  const f = await fixture().live(), {m} = await mountRoom(f, {}, {strict: true}); assert.equal(f.network.subscriberCount, 1); await m.unmount(); assert.equal(f.network.subscriberCount, 0);
});
test('Reduced-motion modal closes synchronously; room trigger focus and settings state settle together', async () => {
  const f = await fixture().live(), {m} = await mountRoom(f); const trigger = n('[data-mp-seat="0"] .mp-seat-edit');
  await click('[data-mp-seat="0"] .mp-seat-edit'); await m.update({panel: {kind: 'personal'}}); assert.equal(f.service.getSnapshot().roomSettingsOpen, false);
  await act(() => n('#mpRoomPanelClose').focus()); await m.update({panel: null}); assert.equal(n('#mpRoomPanel').open, false); assert.equal(f.service.getSnapshot().roomSettingsOpen, false);
  assert.ok(env.document.activeElement === trigger); assert.equal(n('.room-spectator-preview-trigger').getAttribute('aria-expanded'), 'false');
});
test('Seat occupant changes animate only existing contents for 180ms and preserve mounted controls', async () => {
  const animations = animationFixture(), f = await fixture().live(), {m} = await mountRoom(f); const seat = n('[data-mp-seat="1"]');
  await f.receive({seats: [{clientId: f.id, name: 'A', loadout: 0}, {clientId: 'peer12345', name: 'B', loadout: 1}]});
  const changes = animations.filter(item => seat.contains(item.node)); assert.equal(changes.length, 3);
  assert.ok(changes.every(item => item.options.duration === 180 && ['mp-seat-face', 'mp-seat-index', 'mp-seat-name', 'mp-seat-identity', 'mp-seat-state'].some(name => item.node.classList.contains(name))));
  assert.ok(changes.every(item => !JSON.stringify(item.keyframes).includes('transform'))); const count = animations.length;
  await f.receive({seats: [{clientId: f.id, name: 'A', loadout: 0}, {clientId: 'peer12345', name: 'B', loadout: 2, ready: true}]}); assert.equal(animations.length, count); assert.ok(n('[data-mp-seat="1"]') === seat);
  await m.unmount(); assert.ok(changes.every(item => item.cancelled));
});
test('Room views and domain owner add no independent History/Router controller', () => {
  for (const file of ['RoomWorkspace.tsx', 'RoomPanel.tsx', 'RoomSeats.tsx', 'RoomPersonal.tsx', 'RoomProgress.tsx', 'RoomNetwork.tsx', 'RoomSpectators.tsx', 'RoomGameSettings.tsx', 'RoomSettingsDrawer.tsx', 'MultiplayerSettingsControls.tsx'])
    assert.doesNotMatch(readFileSync(resolve(project, 'app/components/room', file), 'utf8'), /\b(?:pushState|replaceState|popstate|history\s*\.|useNavigate|useBlocker)\b/, file);
  assert.doesNotMatch(readFileSync(resolve(project, 'app/services/multiplayer-room.ts'), 'utf8'), /\b(?:pushState|replaceState|popstate|history\s*\.|useNavigate|useBlocker)\b/);
});
test('Native dialog close event clears room settings state and restores trigger as in pinned main', async () => {
  const f = await fixture().live(), {m, intents} = await mountRoom(f); const trigger = n('[data-mp-seat="0"] .mp-seat-edit');
  await click('[data-mp-seat="0"] .mp-seat-edit'); await m.update({panel: {kind: 'personal'}}); await act(() => n('#mpRoomPanelClose').focus());
  await act(() => n('#mpRoomPanel').close()); assert.equal(f.service.getSnapshot().roomSettingsOpen, false); assert.ok(env.document.activeElement === trigger); assert.equal(intents.filter(([kind]) => kind === 'close').length, 1);
});
test('Unmount retires an open native modal synchronously', async () => {
  const h = await panelFixture(); await h.m.unmount(); assert.equal(h.node.open, false);
});

for (const locale of ['zh-CN', 'en']) test(`compact room delay stepper retains host, rollback and Replay/import boundaries: ${locale}`, async () => {
  for (const product of owners.PRODUCT_IDS.filter(owners.isMultiplayerProductId)) {
    const f=fixture({product,locale}),events=[],m=await mount(owners.MultiplayerSettingsControls,{product,service:f.service,onReplayViewer:()=>events.push('replay'),onImport:()=>events.push('import')},{locale});
    assert.equal(all('#mpInputDelay').length,0);
    await click('#mpReplayViewer');await click('#mpGamePackageImport');assert.deepEqual(events,['replay','import']);
    await f.live();const h=await mountRoom(f,{panel:{kind:'game'}},{locale});
    const config=owners.multiplayerConfigForProduct(product),policy=owners.multiplayerInputTimingPolicy(config),value=n('#mpInputDelay');
    assert.equal(n('#mpRoomSettings').contains(value),true);assert.equal(n('#mpInputTiming').contains(value),false);
    assert.equal(n('#mpRoomSettings').contains(n('#mpRollbackToggle')),true);
    assert.equal(all('#mpInputDelayHint').length,0);
    assert.equal(n('#mpInputDelaySetting').hidden,!!config.inputTiming?.measuredStartup&&policy.rollback);
    assert.ok(n('#mpRollbackToggle').compareDocumentPosition(n('#mpInputDelaySetting')) & env.window.Node.DOCUMENT_POSITION_FOLLOWING);
    if(config.inputTiming){
      if(policy.rollback){await act(()=>f.service.setRollbackEnabled(false));assert.equal(n('#mpInputDelaySetting').hidden,false);assert.equal(all('#mpInputDelayHint').length,0);}
      await act(()=>f.service.setTimingChoice(0));assert.equal(value.textContent,'0f');assert.equal(n('#mpInputDelayMinus').disabled,true);
      await click('#mpInputDelayPlus');assert.equal(f.service.getSnapshot().timingChoice,1);assert.equal(value.textContent,'1f');
      await click('#mpInputDelayMinus');assert.equal(value.textContent,'0f');
      const maximum=config.inputTiming.manualDelayLimit??8;
      await act(()=>f.service.setTimingChoice(maximum));assert.equal(n('#mpInputDelayPlus').disabled,true);
      if(policy.measuredStartup){
        await click('#mpInputDelayAuto');assert.equal(f.service.getSnapshot().timingChoice,'auto');
        for(const selector of ['#mpInputDelay','#mpInputDelayMinus','#mpInputDelayPlus'])assert.equal(n(selector).disabled,true);
        await click('#mpInputDelayPlus');assert.equal(f.service.getSnapshot().timingChoice,'auto');
        await click('#mpInputDelayAuto');assert.equal(typeof f.service.getSnapshot().timingChoice,'number');
        assert.equal(n('#mpInputDelayPlus').disabled,false);
      }else{await click('#mpInputDelay');assert.equal(f.service.getSnapshot().timingChoice,'auto');}
      await act(()=>f.service.setTimingChoice(5));assert.equal(value.textContent,'5f');
      await f.receive({adonisMode:1,timingChoice:5,seats:[{clientId:'host12345',name:'Host',loadout:0},{clientId:f.id,name:'Guest',loadout:1}]});
      for(const selector of ['#mpInputDelay','#mpInputDelayMinus','#mpInputDelayPlus'])assert.equal(n(selector).disabled,true);
      if(policy.measuredStartup)assert.equal(n('#mpInputDelayAuto').disabled,true);
      assert.equal(value.textContent,'5f');
      await f.receive({phase:'running',adonisMode:1,inputDelay:4,inputDelayAuto:false});assert.equal(value.textContent,'4f');assert.equal(value.disabled,true);
      if(config.inputTiming.measuredStartup){
        const timing={phase:'ready',route:'rtc',inputDelay:3,fullDelay:3,predictionReserve:0,rttP95Us:100000,samples:100,lost:0,adonisMode:1,automatic:true};
        await f.receive({phase:'running',adonisMode:1,inputDelay:3,inputDelayAuto:true,timing});
        assert.equal(value.textContent,'3f');assert.equal(value.title,t(locale,'room.inputDelayMeasured',{frames:3}));
      }
      if(policy.rollback){await f.receive({phase:'running',adonisMode:0});assert.equal(n('#mpInputDelaySetting').hidden,true);}
      await act(()=>f.socket.close(1006));assert.equal(value.disabled,true);
    }else{
      assert.equal(n('#mpRollbackToggle').hidden,true);assert.equal(n('#mpInputDelaySetting').hidden,false);
      assert.equal(value.textContent,'0f');assert.equal(value.disabled,true);
    }
    await h.m.unmount();await m.unmount();
  }
});

function settingsContext(productId, patch = {}) {return {productId, uiLocale: 'en', languages: [{id: 'ja', title: '日本語'}], musicAvailability: {audio: true, midiAvailable: true, importServer: false, remoteOggAdvertised: false}, webMidiAvailable: false, mobile: false, ...patch};}
test('TH075 has no beauty option and rejects beauty selection even with a stale installed hint', async () => {
  const model=owners.createGameSettingsModel({storage:new Storage()}); model.hydrate(settingsContext('th075',{beautyInstalled:true}));
  const midi={enabled:false,granted:false,busy:false,supported:false,outputs:[],selectedId:'',hint:''};
  const actions={confirm:async()=>false,file:async()=>{},showAppleNotice(){},feedback(){},reportError(error){throw error;},
    externalMidi:{subscribe:()=>()=>{},getSnapshot:()=>midi,setEnabled:async()=>{},selectOutput:async()=>{}}};
  await mount(owners.SettingsBody,{model,actions,onOpenTouchLayout(){}});
  assert.equal(env.document.querySelector('#gameVersionSelect option[value="beauty"]'),null);
  await act(()=>model.setGameVersion('beauty')); assert.equal(model.getSnapshot().gameVersion,'original');
});
async function sharedDrawer(f, {strict = false, roomOpen = true, open = true} = {}) {
  const host=env.document.createElement('section'),stage=env.document.createElement('div');host.append(stage);env.document.body.append(host);
  const model = owners.createGameSettingsModel({storage: new Storage()}), events = []; model.hydrate(settingsContext(f.product));
  const midi = {enabled: false, granted: false, busy: false, supported: false, outputs: [], selectedId: '', hint: ''};
  const actions = {confirm: async () => false, file: async (...args) => events.push(['file', ...args]), showAppleNotice: () => events.push(['apple']), feedback: (...args) => events.push(['feedback', ...args]), reportError: error => {throw error;},
    externalMidi: {subscribe: () => () => {}, getSnapshot: () => midi, setEnabled: async () => {}, selectOutput: async () => {}}};
  function Fixture({product = f.product, roomOpen, open, foregroundActive = false}) {
    const [closing, setClosing] = React.useState(false), requestClose = () => events.push(['close']);
    return React.createElement(MemoryRouter, null,
      React.createElement(owners.RoomView, {service: f.service, network: f.network, context: defaultContext, workspaceHost:host, workspaceStage:stage, assetUrl: path => path, onLeave: () => {}, onCopyRoomCode: () => {}, onGuide: () => {}, onOpenSettings: () => {}, settingsOpen: open && !foregroundActive, settingsClosing: closing, panel: null, onOpenPanel: () => {}, onClosePanel: () => {}}),
      React.createElement(owners.RoomSettingsDrawer, {roomOpen, open, foregroundActive, onCloseRequest: requestClose, onClosingChange: setClosing}),
      React.createElement(owners.OptionsPanel, {product: owners.createLibraryProducts([product], path => path, id => `/?game=${id}`)[0], open: !roomOpen, roomDrawer: roomOpen, onBack: requestClose, assetUrl: path => path, lobbyHref: '/lobby.html',
        multiplayerDiagnostics: React.createElement('section', {id: 'mpNetworkDiagnostics'}, 'Synthetic diagnostics slot'), actions: React.createElement('div', {className: 'launch-wrap launch-actions', id: 'ordinary-launch-actions'}, 'Synthetic ordinary launch slot')},
      React.createElement(owners.SettingsBody, {model, actions, onOpenTouchLayout: () => events.push(['editor']), multiplayerControls: React.createElement(owners.MultiplayerSettingsControls, {product, service: f.service, onReplayViewer: () => events.push(['replay-viewer']), onImport: () => events.push(['import'])})})));
  }
  const m = await mount(Fixture, {roomOpen, open}, {locale: f.locale, strict}); return {m, model, events};
}
function fakeTimers() {
  const originalSet = globalThis.setTimeout, originalClear = globalThis.clearTimeout, tasks = new Map(); let time = 0, serial = 0;
  globalThis.setTimeout = (fn, delay = 0, ...args) => {const id = ++serial; tasks.set(id, {at: time + delay, delay, fn: () => fn(...args)}); return id;}; globalThis.clearTimeout = id => tasks.delete(id);
  return {tasks, async tick(ms) {const end = time + ms; for (;;) {const next = [...tasks].filter(([, task]) => task.at <= end).sort((a, b) => a[1].at - b[1].at)[0]; if (!next) break; tasks.delete(next[0]); time = next[1].at; await act(next[1].fn);} time = end;}, restore() {globalThis.setTimeout = originalSet; globalThis.clearTimeout = originalClear;}};
}
function sharedNodes() {return Object.fromEntries(['#mpSettingsFold', '.tools-head', '.mp-settings-body', '#mpShareSettingsToggle', '#mpReplayViewer', '#mpGamePackageImport', '#mpTouchLayoutEdit'].map(selector => [selector, n(selector)]));}
function assertSharedNodes(nodes) {for (const [selector, node] of Object.entries(nodes)) {assert.ok(n(selector) === node, `Preserve original node: ${selector}`); assert.equal(all(selector).length, 1);} const ids = all('[id]').map(node => node.id); assert.equal(new Set(ids).size, ids.length);}

test('Real shared SettingsBody moves one header/fold into drawer and back across StrictMode, rerender, product/context switches', async () => {
  const f = await fixture().live(); const h = await sharedDrawer(f, {strict: true, roomOpen: false, open: false}), nodes = sharedNodes();
  assert.equal(nodes['#mpSettingsFold'].parentElement.id, 'mpShell'); assert.equal(nodes['.tools-head'].parentElement.classList.contains('tools'), true);
  await h.m.update({roomOpen: true, open: true}); assertSharedNodes(nodes);
  const drawer = n('#mpSettingsRoomDrawer'); assert.ok(nodes['.tools-head'].parentElement === drawer); assert.equal(nodes['#mpSettingsFold'].parentElement.id, 'mpSettingsRoomDrawerContent'); assert.equal(nodes['#mpSettingsFold'].classList.contains('mp-room-drawer-mounted'), true);
  assert.ok(env.document.activeElement === n('#libraryBack'), 'Drawer opening focuses original libraryBack'); assert.equal(n('#libraryBack').getAttribute('aria-label'), t('en', 'settings.drawerClose'));
  assert.equal(drawer.querySelector('#mpInputDelaySetting'), null, 'room-wide timing stays out of the local options drawer');
  const actions = n('#mpReplayViewer').closest('.launch-actions');
  assert.ok(actions.previousElementSibling.classList.contains('mp-shared-settings')); assert.equal(actions.classList.contains('mp-replay-launch-wrap'), false, 'compact placement uses the shared semantic variant');
  for (const selector of ['#optionsLobbyLink', '#mpNetworkDiagnostics', '#ordinary-launch-actions']) {assert.ok(n(selector)); assert.equal(drawer.contains(n(selector)), false);}
  await click('#mpReplayViewer'); await click('#mpGamePackageImport'); await click('#mpTouchLayoutEdit'); assert.deepEqual(h.events.slice(-3), [['replay-viewer'], ['import'], ['editor']]);
  const before = h.model.getSnapshot().shareSingleplayerSettings; await click('#mpShareSettingsToggle'); assert.equal(h.model.getSnapshot().shareSingleplayerSettings, !before); assertSharedNodes(nodes);
  await act(() => h.model.refreshContext(settingsContext(f.product, {mobile: true}))); assertSharedNodes(nodes);
  await act(() => h.model.hydrate(settingsContext('th07mp'))); await h.m.update({product: 'th07mp'}); assertSharedNodes(nodes); assert.equal(n('#gameId').dataset.game, 'th07');
  await h.m.update({roomOpen: false, open: false}); assertSharedNodes(nodes); assert.equal(nodes['#mpSettingsFold'].parentElement.id, 'mpShell'); assert.equal(nodes['#mpSettingsFold'].classList.contains('mp-room-drawer-mounted'), false);
  await h.m.unmount(); assert.equal(all('.mizuki-select-menu').length, 0); assert.equal(all('.mizuki-select').length, 0);
});

test('Drawer preserves shared controls through editor foreground suspension and resumes the same focused field', async () => {
  const f = await fixture().live(), h = await sharedDrawer(f), nodes = sharedNodes(), focused = n('#mpTouchLayoutEdit');
  await act(() => focused.focus()); await h.m.update({foregroundActive: true}); assert.equal(n('#mpSettingsRoomDrawer').hidden, true); assert.equal(n('#mpSettingsRoomDrawer').hasAttribute('inert'), true); assert.equal(n('#mpSettingsRoomBackdrop').hidden, true); assertSharedNodes(nodes);
  await act(() => n('#mpCopyRoomCode').focus()); await h.m.update({foregroundActive: false}); assert.equal(n('#mpSettingsRoomDrawer').hidden, false); assert.equal(n('#mpSettingsRoomDrawer').hasAttribute('inert'), false); assert.ok(env.document.activeElement === focused, 'Restores exact focused control'); assertSharedNodes(nodes);
});

test('Drawer close uses 220ms completion, retained fading backdrop and immediate inert release and then visible cue focus; reopen retires stale timer', async () => {
  animationFixture(); const f = await fixture().live(), h = await sharedDrawer(f), clock = fakeTimers();
  try {
    const cue = n('#mpSettingsRoomDrawerToggle'), focusCalls = [], originalFocus = cue.focus.bind(cue); cue.focus = options => {focusCalls.push({hidden: cue.hidden, roomInert: n('#mpRoomView').hasAttribute('inert')}); originalFocus(options);};
    await h.m.update({open: false}); assert.equal(n('#mpSettingsRoomBackdrop').hidden, false); assert.equal(n('#mpSettingsRoomBackdrop').classList.contains('closing'), true); assert.equal(n('#mpRoomView').hasAttribute('inert'), false); assert.equal(n('#mpSettingsRoomDrawer').hidden, false); assert.equal(n('#mpSettingsRoomDrawer').classList.contains('closing'), true); assert.equal(cue.hidden, false);
    assert.equal([...clock.tasks.values()].filter(task => task.delay === 220).length, 1); await clock.tick(219); assert.equal(n('#mpSettingsRoomDrawer').hidden, false); await clock.tick(1);
    assert.equal(n('#mpSettingsRoomDrawer').hidden, true); assert.equal(n('#mpSettingsRoomBackdrop').hidden, true); assert.equal(cue.hidden, false); assert.deepEqual(focusCalls, [{hidden: false, roomInert: false}]);
    await h.m.update({open: true}); await h.m.update({open: false}); const stale = [...clock.tasks.values()].find(task => task.delay === 220); await h.m.update({open: true}); assert.equal([...clock.tasks.values()].some(task => task === stale), false);
    await act(stale.fn); assert.equal(n('#mpSettingsRoomDrawer').hidden, false); assert.equal(n('#mpSettingsRoomDrawer').classList.contains('closing'), false);
    await h.m.update({open: false}); await h.m.update({roomOpen: false}); assert.equal(n('#mpSettingsRoomDrawer').hidden, true); assert.equal(clock.tasks.size, 0);
    await h.m.unmount();
  } finally {clock.restore();}
});
test('Relocated shared settings controls retain Escape/backdrop close and cyclic Tab containment', async () => {
  const f = await fixture().live(), h = await sharedDrawer(f, {strict: true}), drawer = n('#mpSettingsRoomDrawer');
  const handled = new env.window.KeyboardEvent('keydown', {key: 'Escape', bubbles: true, cancelable: true}); handled.preventDefault(); await act(() => n('#libraryBack').dispatchEvent(handled)); assert.equal(h.events.filter(([kind]) => kind === 'close').length, 0);
  const escape = new env.window.KeyboardEvent('keydown', {key: 'Escape', bubbles: true, cancelable: true});
  await act(() => n('#libraryBack').dispatchEvent(escape)); assert.equal(escape.defaultPrevented, true); assert.equal(h.events.filter(([kind]) => kind === 'close').length, 1);
  await click('#mpSettingsRoomBackdrop'); assert.equal(h.events.filter(([kind]) => kind === 'close').length, 2);
  await click('#libraryBack');assert.equal(h.events.filter(([kind])=>kind==='close').length,3,'relocated Back closes exactly once');
  const controls = all('button:not(:disabled), summary, a[href], input:not(:disabled), select:not(:disabled), [tabindex="0"]', drawer).filter(node => node.tabIndex >= 0 && !node.closest('[hidden]'));
  // Explicit synthetic visibility rectangles, not a browser layout assertion.
  for (const node of controls) node.getClientRects = () => [node.getBoundingClientRect()];
  const [first] = controls, last = controls.at(-1); assert.ok(first && last && first !== last);
  const backward = new env.window.KeyboardEvent('keydown', {key: 'Tab', shiftKey: true, bubbles: true, cancelable: true}); await act(() => {first.focus(); first.dispatchEvent(backward);});
  assert.equal(backward.defaultPrevented, true); assert.ok(env.document.activeElement === last);
  const forward = new env.window.KeyboardEvent('keydown', {key: 'Tab', bubbles: true, cancelable: true}); await act(() => last.dispatchEvent(forward));
  assert.equal(forward.defaultPrevented, true); assert.ok(env.document.activeElement === first);
});
test('Drawer departure and unmount cancel retained 220ms timers with no stale focus or detached selects', async () => {
  animationFixture(); const f = await fixture().live(), h = await sharedDrawer(f, {strict: true}), clock = fakeTimers();
  try {
    await h.m.update({open: false}); const task = [...clock.tasks.values()].find(task => task.delay === 220); assert.ok(task);
    await h.m.unmount(); assert.equal(clock.tasks.size, 0); await act(task.fn); assert.equal(env.document.querySelector('#mpSettingsRoomDrawer'), null); assert.equal(all('.mizuki-select-menu').length, 0);
  } finally {clock.restore();}
});

test('TH11 ready host can choose rollback and pure delay', async()=>{
 const f=await fixture({product:'th11mp'}).live(); const {m}=await mountRoom(f);
 assert.equal(n('#mpRollbackToggle').hidden,false); assert.equal(n('#mpRollbackToggle').disabled,false);
 await act(()=>f.service.setRollbackEnabled(true)); assert.equal(f.service.getSnapshot().rollbackEnabled,true);
 await act(()=>f.service.setRollbackEnabled(false)); assert.equal(f.service.getSnapshot().rollbackEnabled,false);
 await m.unmount();
});

test('public nickname editor waits for review and locks changes until the server deadline',async()=>{
 const f=await fixture({product:'th09mp'}).live(),{m}=await mountRoom(f);
 const status=value=>f.socket.emit('message',{data:JSON.stringify({type:'nickname-status',nextChangeAt:0,retryAt:0,...value})});
 await act(()=>status({name:'Alice',status:'approved'}));
 assert.equal(n('#mpNicknameSave').disabled,false);
 assert.match(n('#mpNicknameStatus').textContent,/once more now/);
  await act(()=>status({name:'Alice',status:'rejected',attemptsLeft:1}));
  assert.equal(n('#mpNicknameSave').disabled,false);assert.match(n('#mpNicknameStatus').textContent,/One retry remaining/);
  await act(()=>status({name:'Alice',status:'unavailable',attemptsLeft:1}));
  assert.equal(n('#mpNicknameSave').disabled,false);assert.match(n('#mpNicknameStatus').textContent,/Unable to submit/);
  assert.match(n('#mpNicknameStatus').textContent,/One retry remaining/);assert.doesNotMatch(n('#mpNicknameStatus').textContent,/24-hour cooldown/);
  await act(()=>status({name:'',status:'unavailable'}));
  assert.equal(n('#mpNicknameSave').disabled,false);assert.match(n('#mpNicknameStatus').textContent,/Unable to submit/);
  assert.doesNotMatch(n('#mpNicknameStatus').textContent,/unchanged|24-hour cooldown/);
  await act(()=>status({name:'',status:'reviewing'}));
  assert.equal(n('#mpDisplayName').disabled,true);assert.equal(n('#mpNicknameSave').disabled,true);
  await act(()=>status({name:'Alice',status:'queued',pendingName:'待审昵称'}));
  assert.equal(n('#mpDisplayName').value,'待审昵称');assert.equal(n('#mpNicknameSave').disabled,true);
  assert.match(n('#mpNicknameStatus').textContent,/Pending review.*Saved.*close this window/);
  assert.equal(f.service.getSnapshot().displayName,'Alice');
 await act(()=>status({name:'Alice',status:'approved',nextChangeAt:Date.now()+86400000}));
 assert.equal(n('#mpDisplayName').value,'Alice');assert.equal(n('#mpDisplayName').disabled,true);
 assert.match(n('#mpNicknameStatus').textContent,/Next change|下次可修改/);
 await f.receive({seats:[{clientId:f.id,name:'Alice',loadout:0,ready:false},{clientId:'peer123456',name:'完整公开昵称',loadout:1,ready:false}],spectators:[{clientId:'watch123456',name:'完整观众昵称'}]});
 assert.equal(n('[data-mp-seat-name="1"]').textContent,'完整公开昵称');
 assert.equal(n('[data-mp-seat="1"] .mp-seat-loadout').textContent,'','TH09 native selection keeps room loadout labels empty');
 assert.equal(n('[data-mp-seat="0"] .mp-seat-control').textContent,'Keys / stick');
 await m.update({panel:{kind:'spectators'}});
 assert.match(n('#mpSpectatorList').textContent,/完整观众昵称/);
 await act(()=>status({name:'Alice',status:'cooldown',attemptsLeft:0,nextChangeAt:Date.now()+86400000}));
 assert.equal(n('#mpNicknameSave').disabled,true);assert.match(n('#mpNicknameStatus').textContent,/No retries remaining/);
 await m.unmount();
});
