import {synchronizedMainAuthority} from './source-text.mjs';
/** Independent pinned-main component/service regression. SYNTHETIC DOM + MEMORY
 * ROUTER ONLY: not production-host wiring, browser top layer, CSS, relay or runtime QA. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {existsSync, readFileSync} from 'node:fs';
import {mkdtemp, rm} from 'node:fs/promises';
import {execFileSync} from 'node:child_process';
import {createRequire} from 'node:module';
import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {installMountedDom, uiContractAttributes} from './mounted-dom-environment.mjs';
import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';

const spectatorProducts = Object.entries(PRODUCT_GAMES)
  .filter(([, product]) => product.multiplayer?.spectator).map(([game]) => game + 'mp');

const here = dirname(fileURLToPath(import.meta.url)), project = resolve(here, '../..');
const require = createRequire(resolve(project, 'package.json'));
const {build} = require('esbuild');
const pinnedMain = 'edee9633e5e3ee79cd2e1aa334f84f6caf755090';
const pinned = path => synchronizedMainAuthority(path, project, pinnedMain);
let env, work, owners, ownersAccounts, React, createRoot, createMemoryRouter, RouterProvider, originalHTML;
const mounts = new Set(), fixtures = new Set();
const assetUrl = path => path;
const t = (locale, key, params) => owners.translate(locale, key, params);
const n = (selector, scope = env.document) => {const node = scope.querySelector(selector); assert.ok(node, `Missing ${selector}`); return node;};
const all = (selector, scope = env.document) => [...scope.querySelectorAll(selector)];
const flush = () => React.act(async () => {await Promise.resolve(); await Promise.resolve();});

before(async () => {
  env = installMountedDom();
  React = await import('react');
  ({createRoot} = await import('react-dom/client'));
  ({createMemoryRouter, RouterProvider} = await import('react-router'));
  originalHTML = pinned('public/lobby.html');
  work = await mkdtemp(resolve(here, 'directory-views-build-'));
  const buildOptions = {absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export * from './app/components/directory/DirectoryRoomRows.tsx';
    export * from './app/components/directory/LobbyDirectoryView.tsx';
    export * from './app/components/directory/DirectoryRoomDialog.tsx';
    export * from './app/services/lobby-directory.ts';
    export * from './app/i18n.tsx';
    export * from './app/navigation/surface-navigation.tsx';
    export * from './app/components/launcher/products.ts';
    export * from './src/contracts/product-catalog.mts';
    export * from './src/launcher/room-invite.mts';
  `}, bundle: true, platform: 'node', format: 'esm', jsx: 'automatic', loader: {'.webp': 'dataurl'}, outfile: resolve(work, 'actual-bundle.mjs'), logLevel: 'silent',
    plugins: [{name: 'installed-deps-original-mts', setup(ctx) {
      ctx.onResolve({filter: /^[^./]/}, args => ({path: args.path, external: true}));
      ctx.onResolve({filter: /^\.\.?\/.*\.mjs$/}, args => {const path = resolve(dirname(args.importer), args.path).replace(/\.mjs$/, '.mts'); if (existsSync(path)) return {path};});
    }}]};
  await build(buildOptions);
  owners = await import(pathToFileURL(resolve(work, 'actual-bundle.mjs')));
  await build({...buildOptions, outfile: resolve(work, 'accounts-bundle.mjs'), define: {'import.meta.env': JSON.stringify({VITE_ACCOUNTS_ENABLED:'true'})}});
  ownersAccounts = await import(pathToFileURL(resolve(work, 'accounts-bundle.mjs')));
});
afterEach(async () => {
  for (const m of mounts) {await React.act(async () => m.root.unmount()); m.router?.dispose();}
  mounts.clear();
  for (const f of fixtures) f.service.dispose();
  fixtures.clear(); env.document.body.replaceChildren();
  assert.deepEqual(env.errors.splice(0), [], 'No unexpected React/DOM errors');
});
after(async () => {env?.close(); if (work) await rm(work, {recursive: true, force: true});});

function manifest({games = ['th06', 'th07', 'th08', 'th09', 'th10'], relay = true} = {}) {
  const hash = 'a'.repeat(64);
  return {schema: 'eagler-touhou/site-manifest/1', protocol: 'eagler-touhou/1', profile: 'synthetic-directory-views',
    shared: {resourceMode: 'hosted', testBuild: true, vanillaFont: 'font.ttf', unicodeFont: 'font.otf', ...(relay ? {netplayRelay: 'wss://relay.invalid/netplay'} : {})},
    games: Object.fromEntries(games.map(id => [id, {runtime: `runtime/${id}.html`, ...('multiplayerRuntime' in owners.PRODUCT_GAMES[id] ? {multiplayerRuntime: `runtime/${id}-mp.html`} : {}),
      gameData: {path: owners.PRODUCT_GAMES[id].package.dataTarget.slice(1), bytes: 1, sha256: hash, version: `sha256-${hash}`, layout: `sha256-${hash}`}, music: {midi: {files: []}},
      ...(owners.PRODUCT_GAMES[id].importOnly ? {offlineCompatibility: {
        schema: 'eagler-touhou/offline-game-pack/1',
        runtimeCompatibility: {protocol: 'eagler-touhou/1', dataLayout: `sha256-${hash}`, versionSource: 'offline-pack'},
        requiredShared: owners.PRODUCT_GAMES[id].requiredShared,
        languages: {source: 'offline-pack', baseline: ['ja']},
      }} : {}),
    }]))};
}
class Socket {
  readyState = 1; listeners = new Map(); sent = []; closes = [];
  addEventListener(type, listener) {const entries = this.listeners.get(type) || []; entries.push(listener); this.listeners.set(type, entries);}
  send(data) {this.sent.push(JSON.parse(data));}
  close(...args) {this.closes.push(args); this.readyState = 3;}
  emit(type, data) {if (type === 'close') this.readyState = 3; for (const fn of this.listeners.get(type) || []) fn(type === 'message' ? {data: JSON.stringify(data)} : data);}
}
function fixture({locale = 'en', selectedProduct = 'th06mp', host = {}, loadError = false, clientSideFiltering = false} = {}) {
  const sockets = [], timers = new Map(), events = [], sessionWrites = [], navigations = []; let timerId = 0, closeCalls = 0;
  const real = owners.createLobbyDirectory({selectedProduct, clientSideFiltering,
    loadSiteManifest: async () => {if (loadError) throw Error('synthetic manifest failure'); return manifest(host);},
    createSocket: url => {const socket = new Socket(); socket.url = url; sockets.push(socket); return socket;},
    memberId: () => 'fixture-member', identity: {lobbyClientId: product => {events.push(`identity:${product}`); return 'fixture-client';}},
    sessions: {save: (...args) => sessionWrites.push(['save', ...args]), clear: product => sessionWrites.push(['clear', product])},
    sessionStorage: {getItem: () => null, removeItem() {}}, translate: (key, params) => t(locale, key, params),
    launcherUrl: () => 'https://launcher.invalid/index.html?keep=1#retained',
    navigateToRoom: url => {events.push('navigate'); navigations.push({url, dialogOpen: env.document.querySelector('#roomDialog')?.open}); f.onNavigate?.(url);},
    randomWord: () => 23, setTimeout: (fn, delay) => {timers.set(++timerId, {fn, delay}); return timerId;}, clearTimeout: id => timers.delete(id),
  });
  const service = {...real, formDidClose(...args) {closeCalls++; events.push('formDidClose'); real.formDidClose(...args);}};
  const f = {service, sockets, timers, events, sessionWrites, navigations, get closeCalls() {return closeCalls;},
    get socket() {assert.ok(sockets.length); return sockets.at(-1);},
    async start() {await React.act(async () => service.initialize()); return f;},
    async receive(rooms = [], patch = {}) {await React.act(async () => f.socket.emit('message', {type: 'directory', version: 1,
      product: service.getSnapshot().selectedProduct, rooms, total: rooms.length, ...patch}));},
    async live(rooms = [], patch = {}) {await f.start(); await f.receive(rooms, patch); return f;},
    async emit(type, data) {await React.act(async () => f.socket.emit(type, data));},
  };
  fixtures.add(f); return f;
}
function room(patch = {}) {return {product: 'th06mp', code: '1234', capacity: 3, players: 2, ready: 1, difficulty: 1, spectators: 0,
  phase: 'lobby', joinable: true, seats: [{initial: '霊', online: true, ready: true, controlMode: 'normal'}, {initial: 'M', online: false, ready: false, controlMode: 'touch'}, null],
  disableCheatMovement: false, challengeMode: false, ...patch};}
async function mount(Component, initial, {locale = 'en', strict = false, localeOwner = owners} = {}) {
  let props = initial; const container = env.document.createElement('div'); env.document.body.append(container); const root = createRoot(container);
  async function render() {let tree = React.createElement(localeOwner.LocaleProvider, {locale}, React.createElement(Component, props));
    if (strict) tree = React.createElement(React.StrictMode, null, tree); await React.act(async () => root.render(tree));}
  const m = {root, container, async update(patch) {props = {...props, ...patch}; await render();}, async setLocale(next) {locale = next; await render();},
    async unmount() {await React.act(async () => root.unmount()); mounts.delete(m);}};
  mounts.add(m); await render(); return m;
}
async function mountLobby(f, extra = {}, options = {}) {
  const opens = []; const m = await mount(owners.LobbyDirectoryView, {service: f.service, assetUrl,
    masthead: null, products: [], onSelect: () => {}, onActivate: () => {}, guideHref: '/docs/multiplayer', onNetwork: () => {},
    onOpenForm: mode => opens.push(mode), ...extra}, options); return {m, opens};
}
async function mountDialog(f, mode = 'create', extra = {}, options = {}) {
  await React.act(async () => assert.equal(f.service.prepareForm(mode), true)); let requests = 0;
  const m = await mount(owners.DirectoryRoomDialog, {service: f.service, assetUrl, open: true, onCloseRequest: () => requests++, ...extra}, options);
  return {m, get requests() {return requests;}};
}
async function click(selector, scope) {await React.act(async () => {const target=n(selector,scope);if(target.getAttribute('aria-haspopup')==='menu'){target.focus();target.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true,cancelable:true}));}else target.click();await new Promise(resolve=>setTimeout(resolve,0));});}
async function change(selector, value) {const node = n(selector); await React.act(async () => {
  if (node.localName === 'input') Object.getOwnPropertyDescriptor(env.window.HTMLInputElement.prototype, 'value').set.call(node, value); else node.value = value;
  node.dispatchEvent(new env.window.Event(node.localName === 'input' ? 'input' : 'change', {bubbles: true}));
});}
async function submit() {const event = new env.window.Event('submit', {bubbles: true, cancelable: true}); await React.act(async () => n('#roomForm').dispatchEvent(event)); assert.equal(event.defaultPrevented, true);}
function normalize(element) {
  if (element.nodeType === 3) return element.textContent.replace(/\s+/g, ' ').trim() || null;
  if (element.nodeType !== 1) return null;
  if (element.classList.contains('ui-select')) return normalize(element.querySelector('select.custom-select-native'));
  const kitButton = element.localName === 'button' && element.dataset.slot === 'button';
  const attrs = uiContractAttributes(element).filter(a => !a.name.startsWith('data-i18n') && a.name !== 'open' && a.name !== 'data-ui-variant' && !(kitButton && ['data-slot','data-variant','data-size'].includes(a.name)) && !(element.localName === 'select' && ['tabindex','aria-hidden'].includes(a.name)) && !(element.localName === 'button' && a.name === 'type' && a.value === 'submit') && !(a.name === 'value' && element.localName === 'input'))
    .map(a => [a.name, a.name === 'class' ? a.value.split(/\s+/).filter(value=>kitButton ? value.startsWith('lobby-') : !['ui-button','ui-control','custom-select-native'].includes(value)).join(' ') : a.value]).filter(([name,value])=>name!=='class'||value).sort(([a], [b]) => a.localeCompare(b));
  const children = []; let text = '';
  const flushText = () => {const value = text.replace(/\s+/g, ' ').trim(); if (value) children.push(value); text = '';};
  for (const child of element.childNodes) {if (child.nodeType === 3) text += child.textContent; else if (child.nodeType === 1) {flushText(); children.push(normalize(child));}}
  flushText(); return [element.localName, attrs, children];
}
function baselineDoc(locale) {
  const doc = new env.window.DOMParser().parseFromString(originalHTML, 'text/html');
  for (const el of all('[data-i18n]', doc)) el.textContent = t(locale, el.dataset.i18n);
  for (const el of all('[data-i18n-aria-label]', doc)) el.setAttribute('aria-label', t(locale, el.dataset.i18nAriaLabel));
  return doc;
}
function expectedDialog(f, locale) {
  const doc = baselineDoc(locale), form = f.service.getSnapshot().form, create = form.mode === 'create', policy = owners.multiplayerConfigForProduct(form.product);
  const option = (value, text) => {const node = doc.createElement('option'); node.value = String(value); node.textContent = text; return node;};
  n('#gameSelect', doc).replaceChildren(...f.service.getSnapshot().products.map(product => {const game = owners.PRODUCT_GAMES[owners.gameIdForProduct(product)]; return option(product, game.title);}));
  n('#capacitySelect', doc).replaceChildren(...policy.playerCounts.map(count => option(count, t(locale, 'lobby.playersCount', {count}))));
  const rank = n('#difficultySelect', doc); rank.className = 'difficulty-label'; rank.dataset.difficulty = (policy.difficulties[form.difficulty] || '').toLowerCase();
  rank.replaceChildren(...policy.difficulties.map((name, index) => {const node = option(index, name); node.className = 'difficulty-label'; node.dataset.difficulty = name.toLowerCase(); return node;}));
  n('#dialogTitle', doc).textContent = t(locale, create ? 'lobby.create' : 'lobby.byCode');
  n('#roomDialog', doc).dataset.mode = form.mode;
  n('#closeDialog', doc).setAttribute('aria-label', t(locale, 'lobby.cancel'));
  n('#closeDialog', doc).replaceChildren();
  n('#submitRoom', doc).textContent = t(locale, create ? 'lobby.create' : 'lobby.joinRoom');
  n('#createFields', doc).hidden = n('#policyFields', doc).hidden = !create; n('#codeField', doc).hidden = create;
  const rules = doc.createElement('details'), summary = doc.createElement('summary');
  rules.className = 'lobby-create-rules';
  rules.hidden = !create;
  summary.textContent = t(locale, 'room.ruleSettings');
  const fields = n('#policyFields', doc);
  fields.before(rules);
  rules.append(summary, fields);
  n('#roomCodeInput', doc).required = !create;
  n('#codeField span', doc).className = 'sr-only';
  if (create && form.visibility === 'private') n('#formNote', doc).textContent = t(locale, 'room.privateHint');
  else n('#formNote', doc).remove();
  for (const id of ['gameSelect', 'visibilitySelect']) {
    const field = n('#'+id, doc).closest('label');
    const group = doc.createElement('div'), heading = doc.createElement('div'), label = doc.createElement('label');
    group.className = field.className;
    heading.className = 'flex items-center gap-1';
    label.htmlFor = id+'-trigger';
    label.textContent = field.firstElementChild.textContent;
    field.firstElementChild.remove();
    heading.append(label);
    group.append(heading, ...field.childNodes);
    field.replaceWith(group);
  }
  for (const [id, key] of [['gameSelect','lobby.game'], ['capacitySelect','lobby.capacity'], ['difficultySelect','lobby.difficulty'], ['visibilitySelect','room.visibility'], ['cheatSelect','room.cheatMovement']]) n('#'+id, doc).setAttribute('aria-label', t(locale,key));
  return n('#roomDialog', doc);
}

for (const locale of ['zh-CN', 'en']) test(`Original cooperative list shows room identity, difficulty, seats and join action: ${locale}`, async () => {
  const f = await fixture({locale}).live([room({challengeMode:true,publicNames:true,spectators:3,seats:[{initial:'A',name:'Alice',loadout:1,online:true,ready:true},null]})]);
  const snapshot=f.service.getSnapshot();await mount(owners.DirectoryRoomRows,{snapshot,service:f.service,assetUrl,initialRows:true},{locale});
  assert.equal(n('.lobby-room-code').textContent,'#1234');
  assert.equal(n('.lobby-difficulty').textContent,'Normal');
  assert.equal(n('.lobby-room-rules').textContent,t(locale,'room.challengeMode'));
  assert.equal(n('.lobby-seat-name').textContent,'Alice');
  assert.ok(n('.lobby-room-title h2').textContent.length>0);
  assert.equal(all('.lobby-seat').length,snapshot.visibleRooms[0].capacity);
  assert.equal(n('.lobby-party').getAttribute('aria-label'),t(locale,'lobby.count',{players:snapshot.visibleRooms[0].players,capacity:snapshot.visibleRooms[0].capacity}));
  assert.equal(env.document.querySelector('.lobby-player-count'),null);
  assert.equal(n('.lobby-room-state').textContent,t(locale,'lobby.recruiting'));
  assert.equal(n('.lobby-room-watchers').getAttribute('aria-label'),t(locale,'th09.spectatorCount',{count:3}));
  assert.equal(n('.lobby-room-watchers').textContent,t(locale,'th09.spectatorCount',{count:3}));
  assert.equal(env.document.querySelector('.coop-room-row'),null);
  assert.equal(env.document.querySelector('.room-elapsed'),null);
});

test('List rows retain keyed identity and creation-time motion across reorder', async () => {
  const f=await fixture().live([room()]), m=await mount(owners.DirectoryRoomRows,{snapshot:f.service.getSnapshot(),service:f.service,assetUrl,initialRows:true});
  const original=n('.lobby-room-row');
  await f.receive([room({code:'5678'}),room({difficulty:3})]);await m.update({snapshot:f.service.getSnapshot(),initialRows:false});
  assert.equal(all('.lobby-room-row')[1],original);
  assert.equal(original.classList.contains('lobby-room-initial'),true);
  assert.equal(all('.lobby-room-row')[0].classList.contains('lobby-room-initial'),false);
  assert.equal(n('.lobby-difficulty',original).textContent,'Lunatic');
  await f.receive([room({code:'5678'})]);await m.update({snapshot:f.service.getSnapshot()});assert.equal(original.isConnected,false);
});

test('Closed rooms briefly exit, cannot be entered, and recover if they return', async () => {
  const f = await fixture().live([room()]);
  await mountLobby(f);
  n('.lobby-room-row');
  await f.receive([]);
  const departing = n('.lobby-room-exiting');
  assert.equal(departing.getAttribute('aria-hidden'), 'true');
  assert.ok(departing.hasAttribute('inert'));
  assert.equal(n('#roomList').hidden, false);
  await f.receive([room()]);
  assert.equal(n('.lobby-room-row'), departing);
  assert.equal(departing.classList.contains('lobby-room-exiting'), false);
  assert.equal(departing.hasAttribute('inert'), false);
  await f.receive([]);
  await React.act(async () => {await new Promise(resolve => setTimeout(resolve, 550));});
  assert.equal(env.document.querySelector('.lobby-room-row'), null);
  assert.equal(n('#roomList').hidden, true);
});

test('Offline directory keeps Replay out of room tools and preserves its state', async () => {
  const f=fixture({loadError:true});
  await f.start();
  assert.equal(f.service.getSnapshot().connection,'offline');
  await mountLobby(f);
  const before=f.service.getSnapshot();
  assert.equal(env.document.querySelector('.lobby-actions #watchReplayButton'),null);
  assert.equal(f.service.getSnapshot(),before);
  assert.equal(f.navigations.length,0);
  assert.equal(n('#createButton').disabled,true);
});

test('Rows enforce live/recruiting/joinable/membership/leaving gates and use the latest service room', async () => {
  for (const patch of [{joinable: false}]) {
    const f = await fixture().live([room(patch)]); const m = await mount(owners.DirectoryRoomRows, {snapshot: f.service.getSnapshot(), service: f.service, assetUrl});
    assert.equal(n('.lobby-join').disabled, true); await click('.lobby-join'); assert.equal(f.navigations.length, 0); await m.unmount();
  }
  const f = await fixture().live([room()], {mine: {product: 'th07mp', code: '7777', recoveryToken: 'membership-token'}});
  const m = await mount(owners.DirectoryRoomRows, {snapshot: f.service.getSnapshot(), service: f.service, assetUrl});
  assert.equal(n('.lobby-join').title, t('en', 'lobby.conflict')); assert.equal(n('.lobby-join').disabled, true);
  await f.receive([room()]); await m.update({snapshot: f.service.getSnapshot()}); assert.equal(n('.lobby-join').disabled, false);
  await f.receive([room({joinable: false})]); // Deliberately do not update snapshot: service must reject stale enabled UI.
  await click('.lobby-join'); assert.equal(f.navigations.length, 0);
  await f.receive([room()]); await m.update({snapshot: {...f.service.getSnapshot(), connection: 'offline'}}); assert.equal(n('.lobby-join').disabled, true);
  await m.update({snapshot: {...f.service.getSnapshot(), leaving: true}}); assert.equal(n('.lobby-join').disabled, true);
  await m.update({snapshot: f.service.getSnapshot()}); await click('.lobby-join'); assert.equal(f.navigations.length, 1);
  assert.equal(f.closeCalls, 0); assert.deepEqual(f.sessionWrites, [['clear', 'th06mp']]);
});

for (const product of spectatorProducts) test(`Full waiting room enters the spectator role through the directory: ${product}`, async () => {
  const f=await fixture({selectedProduct:product,host:{games:[product.slice(0,-2)]}}).live([
    room({product,capacity:2,players:2,joinable:false,seats:[{initial:'A',online:true},{initial:'B',online:true}]})]);
  const m=await mount(owners.DirectoryRoomRows,{snapshot:f.service.getSnapshot(),service:f.service,assetUrl});
  const entry = '.lobby-join';
  assert.equal(n(entry).disabled,false);

  if(product!=='th09mp')assert.equal(n('.lobby-join').textContent,t('en','multiplayer.joinSpectator'));
  await click(entry);assert.equal(f.navigations.length,1);
  const invite=owners.decodeRoomInvite(new URL(f.navigations[0].url).searchParams.get('j'));
  assert.equal(invite.g,product);assert.equal(invite.s,true);assert.equal(invite.a,'join');
  assert.equal(f.sessionWrites.some(write=>write[0]==='save'),false,'directory must not reserve a player seat');
  await m.unmount();
});

test('Running cooperative room admits an observer even when its engine must wait for the next run',async()=>{
  for(const spectatable of [false,true]){
    const f=await fixture().live([room({phase:'playing',joinable:false,spectatable})]);
    const m=await mount(owners.DirectoryRoomRows,{snapshot:f.service.getSnapshot(),service:f.service,assetUrl});
    assert.equal(n('.lobby-join').disabled,false);await click('.lobby-join');assert.equal(f.navigations.length,1);
    assert.equal(owners.decodeRoomInvite(new URL(f.navigations[0].url).searchParams.get('j')).s,true);
    await m.unmount();
  }
});

for (const locale of ['zh-CN', 'en']) test(`Lobby real loading/live/empty/list conditions and original static sections: ${locale}`, async () => {
  const f = fixture({locale}); const {opens} = await mountLobby(f, {}, {locale});
  assert.equal(n('#roomList').getAttribute('aria-busy'), 'true'); assert.equal(n('#listLoading').hidden, false);
  assert.equal(n('#refreshRooms').getAttribute('aria-busy'), 'true');
  assert.equal(n('#refreshRooms').dataset.loading, 'true');
  assert.equal(env.document.querySelector('#listLoading .lobby-preload-spinner'), null);
  assert.equal(n('#roomList').hidden, true, 'An unloaded list cannot displace the loading indicator');
  assert.equal(n('#roomCount').hidden, true);
  assert.equal(env.document.querySelector('#listHead'),null); assert.equal(env.document.querySelector('#emptyState'), null);
  assert.equal(n('#createButton').disabled, true); assert.equal(n('#codeButton').disabled, true); assert.equal(n('#roomCount').textContent, '');
  await f.live(); assert.equal(n('#roomList').getAttribute('aria-busy'), 'false'); assert.equal(n('#listLoading').hidden, true);
  assert.equal(n('#refreshRooms').getAttribute('aria-busy'), 'false');
  assert.equal(n('#emptyTitle').textContent, t(locale, 'lobby.empty')); assert.equal(n('#emptyHint').textContent, t(locale, 'lobby.emptyHint'));
  assert.equal(n('#roomList').hidden,true,'An empty list cannot consume the empty-state layout space');
  assert.equal(n('#roomCount').textContent, t(locale, 'lobby.roomCount', {count: 0}));
  assert.equal(n('#roomCount').hidden, true, 'An empty directory needs no zero-room footer');
  assert.equal(n('#connectionWarning').hidden, true); assert.equal(n('#membership').hidden, true);
  const doc = baselineDoc(locale);
  for (const selector of ['.lobby-intro']) {
    const intro = n(selector).cloneNode(true);
    intro.querySelector('.lobby-survey-link')?.remove();
    assert.equal(intro.querySelector('h1').className,'visually-hidden');
    intro.querySelector('h1').removeAttribute('class');
    const guide = n('#lobbyGuideOpen', intro);
    assert.equal(guide.localName, 'a');
    assert.equal(guide.getAttribute('href'), '/docs/multiplayer');
    guide.remove();
    n('#lobbyGuideOpen', doc).remove();
    assert.deepEqual(normalize(intro), normalize(n(selector, doc)), `Pinned DOM: ${selector}`);
  }
  assert.ok(n('.lobby-support-links .lobby-survey-link').textContent.includes(t(locale,'lobby.surveyAction')));
  assert.equal(n('.lobby-survey-link').getAttribute('href'),'https://v.wjx.cn/vm/QqmTdwh.aspx#');
  assert.equal(n('.lobby-survey-link').getAttribute('target'),'_blank');
  assert.equal(env.document.querySelector('.book-room-view-toggle'),null);
  await click('#emptyAction'); await click('#createButton'); await click('#codeButton'); assert.deepEqual(opens, ['create', 'create', 'join']);
  assert.equal(n('#refreshRooms').getAttribute('aria-label'), t(locale, 'lobby.refresh'));
  await f.receive([room()], {total: 45}); assert.equal(env.document.querySelector('#listHead'),null); assert.equal(env.document.querySelector('#emptyState'), null);
  assert.equal(n('#roomList').hidden,false,'Live rows use the scrolling list again');
  assert.equal(env.document.querySelector('#connectionNote'),null,'A truncated response adds no description or layout element');
  assert.equal(n('#roomCount').getAttribute('aria-live'), 'polite'); assert.equal(n('#roomCount').textContent, t(locale, 'lobby.roomCount', {count: 1}));
  assert.equal(n('#roomCount').hidden, false);
  await click('#refreshRooms'); assert.equal(n('#roomList').getAttribute('aria-busy'), 'true'); assert.equal(env.document.querySelector('#listHead'),null);
  assert.equal(n('#refreshRooms').dataset.loading, 'true');
  assert.equal(n('#roomCount').hidden, false, 'Refreshing existing rows preserves the footer geometry');
  assert.equal(n('#roomCount').textContent, t(locale, 'lobby.roomCount', {count: 1}));
  assert.equal(n('.lobby-join').disabled,true,'Stale room data cannot be joined during refresh');
  const beforeRefreshJoin=f.navigations.length;await click('.lobby-join');assert.equal(f.navigations.length,beforeRefreshJoin);
  assert.equal(env.document.querySelector('#emptyState'), null); assert.deepEqual(f.socket.sent.at(-1), {type: 'refresh', product: 'th06mp'});
  await f.receive([room()]); assert.equal(env.document.querySelector('#connectionNote'),null);
});
test('empty rows from older relays disappear with consistent totals while spectator, reconnect and own recovery remain', async () => {
  const empty=room({code:'1111',players:0,seats:[null,null,null]}), live=room({code:'2222'});
  const replay=room({code:'3333',players:0,phase:'running',spectators:1,seats:[null,null,null],joinable:false,spectatable:true});
  const reconnect=room({code:'4444',seats:[{initial:'R',online:false},null,null]});
  const emptyRun=room({...empty,code:'5555',phase:'running',spectatable:true});
  const mine={product:'th06mp',code:'1111',recoveryToken:'retained-recovery'};
  const f=await fixture().live([empty,live,replay,reconnect,emptyRun],{total:5,mine,membershipRecovery:true});
  await mountLobby(f);
  assert.deepEqual(f.service.getSnapshot().rooms.map(r=>r.code),['2222','3333','4444']);
  assert.equal(f.service.getSnapshot().total,3);
  assert.equal(all('#roomList .lobby-room-row').length,3);
  assert.equal(env.document.querySelector('#connectionNote'),null);
  assert.equal(f.service.getSnapshot().mine.recoveryToken,'retained-recovery');
  assert.equal(n('#membership').hidden,false);
  await f.receive([empty],{total:1,mine,membershipRecovery:true});
  assert.equal(f.service.getSnapshot().total,0);
  assert.equal(all('#roomList .lobby-room-exiting').length,3);
  assert.ok(all('#roomList .lobby-room-exiting').every(row=>row.getAttribute('aria-hidden')==='true'&&row.hasAttribute('inert')));
  await React.act(async () => {await new Promise(resolve => setTimeout(resolve,550));});
  assert.equal(all('#roomList .lobby-room-row').length,0);
  assert.equal(n('#roomCount').textContent,t('en','lobby.roomCount',{count:0}));
  assert.equal(n('#membership').hidden,false,'private recovery survives public row removal');
  await f.receive([room({...empty,seats:[{initial:'A',online:true},null,null]})],{total:1});
  assert.equal(all('#roomList .lobby-room-row').length,1,'a reseated member restores the row');
});
for (const [kind, title, hint] of [['offline', 'lobby.offline', 'lobby.refreshHint'], ['missing', 'lobby.offline', 'lobby.noService'], ['unsupported', 'lobby.unsupported', 'lobby.unsupportedHint']]) {
  test(`Lobby real ${kind} warning suppresses empty state and retries through service`, async () => {
    const f = fixture({host: {relay: kind !== 'missing'}, loadError: kind === 'offline'}); await mountLobby(f); await f.start();
    if (kind === 'unsupported') await f.emit('close', {code: 1008});
    assert.equal(f.service.getSnapshot().connection, kind); assert.equal(n('#connectionWarning').hidden, false);
    assert.equal(n('#connectionWarningTitle').textContent, t('en', title));
    const detail = kind === 'offline' ? t('en', 'network.manifestUnavailable')
      : kind === 'unsupported' ? t('en', 'network.relayClosed', {code: 1008}) : '';
    assert.equal(n('#connectionWarningHint').textContent, [t('en', hint), detail].filter(Boolean).join(' | '));
    assert.equal(n('#connectionWarning').getAttribute('role'), 'alert'); assert.equal(env.document.querySelector('#emptyState'), null);
    assert.equal(n('#createButton').disabled, true); assert.equal(n('#codeButton').disabled, true);
    const before = f.sockets.length; await click('#connectionRefresh'); await flush();
    if (kind === 'unsupported') assert.equal(f.sockets.length, before + 1);
  });
}
test('favorite multiplayer rooms sort first, update immediately and retain order within each group', async () => {
  const rooms = [room({code:'1111'}), room({product:'th07mp',code:'2222'}), room({code:'3333'}), room({product:'th07mp',code:'4444'})];
  const f = await fixture({selectedProduct:''}).live(rooms);
  const saved = env.window.localStorage.getItem('eagler.book.multiplayer-favorites');
  const order = () => all('#roomList .lobby-room-row').map(el => el.dataset.key);
  const original = rooms.map(room => `${room.product}-${room.code}`);
  try {
    env.window.localStorage.setItem('eagler.book.multiplayer-favorites', JSON.stringify(['th07mp']));
    const {m} = await mountLobby(f);
    const favoriteFirst = ['th07mp-2222','th07mp-4444','th06mp-1111','th06mp-3333'];
    assert.deepEqual(order(), favoriteFirst);
    const retained = n('[data-key="th06mp-1111"]');
    await f.receive(rooms); assert.deepEqual(order(), favoriteFirst);
    await React.act(async () => {
      env.window.localStorage.setItem('eagler.book.multiplayer-favorites', '[]');
      env.window.dispatchEvent(new env.window.CustomEvent('eagler-book-preferences'));
    });
    assert.deepEqual(order(), original); assert.equal(n('[data-key="th06mp-1111"]'),retained);
    await React.act(async () => {
      env.window.localStorage.setItem('eagler.book.multiplayer-favorites', JSON.stringify(['th07mp']));
      env.window.dispatchEvent(new env.window.Event('storage'));
    });
    assert.deepEqual(order(), favoriteFirst);
    await m.update({productFilters:['th06mp']});
    assert.deepEqual(order(), ['th06mp-1111','th06mp-3333']);
    assert.deepEqual(f.service.getSnapshot().visibleRooms.map(room => `${room.product}-${room.code}`),original);
  } finally {
    if (saved === null) env.window.localStorage.removeItem('eagler.book.multiplayer-favorites');
    else env.window.localStorage.setItem('eagler.book.multiplayer-favorites',saved);
  }
});

test('Lobby defaults to live ratings and shows no-rating games without a win-rate fallback', async () => {
  const originalFetch = globalThis.fetch;
  const requests = [];
  globalThis.fetch = async input => {
    const path = new URL(String(input), 'https://launcher.invalid').pathname;
    requests.push(path);
    const responses = {
      '/api/ready': {ready:true}, '/api/auth/get-session': null,
      '/api/v1/th09/ratings': {sequence:1,players:{rated:{rating:1438,interval:120,games:18,group:1}}},
    };
    assert.ok(path in responses, path);
    return new Response(JSON.stringify(responses[path]), {headers:{'Content-Type':'application/json'}});
  };
  try {
    const f = await fixture({selectedProduct:'th09mp'}).live(['rated','unrated'].map((username,index) => room({
      product:'th09mp',code:String(4201+index),capacity:2,players:1,
      seats:[{initial:'R',online:true,profile:{username,th09:{games:26,wins:13}}},null],
    })));
    const props = {service:f.service,assetUrl,masthead:null,products:[],onSelect:()=>{},onActivate:()=>{},
      guideHref:null,onNetwork:null,onOpenForm:()=>{}};
    const m = await mount(ownersAccounts.LobbyDirectoryView,props,{locale:'zh-CN',localeOwner:ownersAccounts});
    await React.act(async () => {await new Promise(resolve=>setTimeout(resolve,0));});
    assert.ok(requests.includes('/api/v1/th09/ratings'));
    assert.deepEqual(all('.book-room-duel-summary[data-side="0"]').map(node=>node.textContent),['1438 (局数 26)','— (局数 26)']);
    await m.unmount();
    requests.length = 0;
    await mount(ownersAccounts.LobbyDirectoryView,{...props,interactive:false},{localeOwner:ownersAccounts});
    await flush();
    assert.deepEqual(requests,[],'inactive initial render does not request accounts or ratings');
  } finally {
    globalThis.fetch = originalFetch;
  }
});

test('Lobby failed live connection retains stale room warning, interrupted reconnect, and recovery', async () => {
  const f = await fixture().live([room()]); await mountLobby(f); await f.emit('error');
  assert.equal(n('#connectionWarningTitle').textContent, t('en', 'lobby.disconnected')); assert.equal(n('#connectionWarning').hidden, false);
  assert.equal(n('.lobby-join').disabled, true); assert.equal(env.document.querySelector('#emptyState'), null);
  await click('#connectionRefresh'); assert.equal(f.service.getSnapshot().connection, 'loading'); assert.equal(n('#connectionWarning').hidden, false);
  assert.equal(n('#listLoading').hidden, true); assert.equal(all('.lobby-room-row').length, 1);
  const retainedRow = n('.lobby-room-row');
  for (let attempt = 0; attempt < 3; attempt++) {
    await f.emit('error'); await click('#connectionRefresh');
    assert.equal(n('#listLoading').hidden, true);
    assert.equal(n('#connectionWarning').hidden, false);
    assert.equal(n('.lobby-room-row'), retainedRow, 'reconnect keeps the existing row without replaying its entrance');
  }
  await f.receive(); assert.equal(n('#connectionWarning').hidden, true);
  await React.act(async () => {await new Promise(resolve => setTimeout(resolve,550));});
  assert.ok(n('#emptyState'));
  await f.receive([room()]);
  await f.emit('close', {code: 1006});
  assert.equal(n('#connectionWarning').hidden, false);
  assert.equal(n('#listLoading').hidden, true);
  assert.equal(all('.lobby-room-row').length, 1);
});
for (const locale of ['zh-CN', 'en']) test(`Lobby membership/recovery uses exact state/copy and release service action: ${locale}`, async () => {
  const mine = {product: 'th06mp', code: '9876', recoveryToken: 'fixture-token'};
  const f = await fixture({locale}).live([], {mine}); await mountLobby(f, {}, {locale});
  assert.equal(n('#membership').hidden, false); assert.equal(n('#membershipTitle').textContent, t(locale, 'lobby.mine', {code: '9876'}));
  assert.equal(n('#membershipHint').textContent, t(locale, 'lobby.releaseUnsupported')); assert.equal(n('#releaseMembership').hidden, true);
  assert.equal(env.document.querySelector('#emptyAction'), null); assert.equal(n('#createButton').disabled, true); assert.equal(n('#codeButton').disabled, true);
  await f.receive([], {mine, membershipRecovery: true}); assert.equal(n('#membershipHint').textContent, t(locale, 'lobby.mineHint'));
  assert.equal(n('#releaseMembership').hidden, false); assert.equal(n('#releaseMembership').disabled, false);
  await click('#releaseMembership'); assert.equal(n('#releaseMembership').disabled, true); assert.equal(n('#releaseMembership').textContent, t(locale, 'lobby.releasing'));
  assert.deepEqual(f.socket.sent.at(-1), {type: 'release-membership', recoveryToken: 'fixture-token'});
  await f.receive([], {mine: null, membershipRecovery: true}); assert.equal(n('#membership').hidden, true); assert.equal(n('#notice').hidden, false);
  assert.equal(n('#notice').textContent, t(locale, 'lobby.released')); assert.equal(n('#notice').getAttribute('role'), 'status'); assert.equal(n('#createButton').disabled, false);
});
test('Lobby membership token/offline/leaving gates are service-driven and the retired test warning stays absent', async () => {
  const mine = {product: 'th08mp', code: '8888', recoveryToken: ''};
  const f = await fixture({selectedProduct: 'th08mp'}).live([], {mine, membershipRecovery: true}); await mountLobby(f);
  assert.equal(env.document.querySelector('#gameTestNotice'), null); assert.equal(n('#releaseMembership').disabled, true);
  await f.receive([], {mine: {...mine, recoveryToken: 'token'}, membershipRecovery: true}); assert.equal(n('#releaseMembership').disabled, false);
  await f.emit('error'); assert.equal(n('#releaseMembership').disabled, true);
  await React.act(async () => f.service.retry()); await f.receive([], {mine: {...mine, recoveryToken: 'token'}, membershipRecovery: true});
  await React.act(async () => f.service.pageHide()); assert.equal(n('#releaseMembership').disabled, true); assert.equal(n('#createButton').disabled, true);
});

for (const locale of ['zh-CN', 'en']) for (const mode of ['create', 'join']) test(`Dialog exact original DOM, labels, native selects, focus: ${locale}/${mode}`, async () => {
  const f = await fixture({locale}).live(); await mountDialog(f, mode, {}, {locale});
  const dialog = n('#roomDialog').cloneNode(true);
  assert.ok(n('#closeDialog svg[aria-hidden="true"]', dialog));
  n('#closeDialog', dialog).replaceChildren();
  assert.equal(n('.lobby-create-rules', dialog).open, false);
  assert.ok(n('.lobby-create-rules summary svg[aria-hidden="true"]', dialog));
  n('.lobby-create-rules summary', dialog).textContent = t(locale, 'room.ruleSettings');
  assert.deepEqual(normalize(dialog), normalize(expectedDialog(f, locale)));
  assert.equal(n('#roomDialog').open, true); assert.equal(n('#roomDialog').getAttribute('aria-labelledby'), 'dialogTitle');
  assert.deepEqual(all('#roomDialog select.custom-select-native').map(s => s.id), ['gameSelect', 'capacitySelect', 'difficultySelect', 'visibilitySelect', 'cheatSelect']);
  assert.equal(all('#roomDialog [role="combobox"]').length, mode === 'create' ? 4 : 5);
  if (mode === 'create') assert.equal(n('#gameSelect-trigger').getAttribute('aria-haspopup'), 'menu');
  for (const select of all('#roomDialog select.custom-select-native')) assert.equal(select.nextElementSibling.getAttribute('aria-label'), select.getAttribute('aria-label'));
  assert.equal(n('#gameSelect').value, 'th06mp'); assert.equal(n('#capacitySelect').value, '2'); assert.equal(n('#difficultySelect').value, '1');
  assert.equal(n('#roomCodeInput').required, mode === 'join'); if (mode === 'join') assert.equal(env.document.activeElement, n('#roomCodeInput'));
  assert.equal(all('#roomDialog input, #roomDialog select.custom-select-native').length, 6, 'No extra room-policy fields');
});
test('Dialog retains code/visibility/cheat fields while each open/game change resets capacity and difficulty', async () => {
  const f = await fixture().live(); const {m} = await mountDialog(f);
  await change('#capacitySelect', '3'); await change('#difficultySelect', '4'); await change('#visibilitySelect', 'private'); await change('#cheatSelect', '1'); await change('#roomCodeInput', '456789');
  assert.equal(n('#formNote').textContent, t('en', 'room.privateHint')); assert.equal(f.service.getSnapshot().form.code, '456789');
  await m.update({open: false}); assert.equal(f.closeCalls, 1);
  await React.act(async () => f.service.prepareForm('join')); await m.update({open: true});
  assert.equal(n('#roomCodeInput').value, '456789'); assert.equal(n('#visibilitySelect').value, 'private'); assert.equal(n('#cheatSelect').value, '1');
  assert.equal(n('#capacitySelect').value, '2'); assert.equal(n('#difficultySelect').value, '1');
  await change('#gameSelect', 'th07mp'); assert.equal(all('#difficultySelect option').at(-1).textContent, 'Phantasm');
  await change('#capacitySelect', '3'); await change('#difficultySelect', '5'); await change('#gameSelect', 'th09mp');
  assert.deepEqual(all('#capacitySelect option').map(x => x.value), ['2']); assert.equal(n('#difficultySelect').value, '1');
  assert.equal(n('#roomCodeInput').value, '456789'); assert.equal(n('#visibilitySelect').value, 'private'); assert.equal(n('#cheatSelect').value, '1');
  assert.equal(env.document.querySelector('#formNote'), null);
});
for (const locale of ['zh-CN', 'en']) test(`Dialog invalid code uses native validity and input clears localized error: ${locale}`, async () => {
  const f = await fixture({locale}).live(); const mounted = await mountDialog(f, 'join', {}, {locale});
  const input = n('#roomCodeInput'); let reports = 0; input.reportValidity = () => {reports++; return false;};
  await change('#roomCodeInput', 'abc'); await submit();
  assert.equal(input.validationMessage, t(locale, 'lobby.invalidCode')); assert.equal(reports, 1); assert.equal(mounted.requests, 0);
  assert.equal(f.closeCalls, 0); assert.equal(f.navigations.length, 0); assert.equal(n('#formError').hidden, true);
  await change('#roomCodeInput', '12345678'); assert.equal(input.validationMessage, ''); await submit();
  assert.equal(mounted.requests, 1); assert.equal(n('#roomDialog').open, true); assert.equal(f.navigations.length, 0);
  await mounted.m.update({open: false}); assert.equal(f.closeCalls, 1); assert.equal(f.navigations.length, 1); assert.equal(f.navigations[0].dialogOpen, false);
});
test('global room-code join hides the title picker and follows the listed title', async () => {
  const f=await fixture({selectedProduct:''}).live([room({product:'th07mp',code:'5678'})],{globalRoomCodes:true});
  const dialog=await mountDialog(f,'join');
  assert.equal(n('#gameSelect').closest('.lobby-field').hidden,true);
  await change('#roomCodeInput','5678');
  await submit();
  await dialog.m.update({open:false});
  assert.equal(f.navigations.length,1);
  assert.ok(f.events.includes('identity:th07mp'));
});

test('Dialog unavailable service states disable submit and reject programmatic submit without closing', async () => {
  const f = await fixture().live(); const mounted = await mountDialog(f);
  await f.receive([], {mine: {product: 'th06mp', code: '1234', recoveryToken: 'token'}});
  assert.equal(n('#submitRoom').disabled, true); await submit(); assert.equal(mounted.requests, 0);
  await f.receive(); await f.emit('error'); assert.equal(n('#submitRoom').disabled, true); await submit(); assert.equal(mounted.requests, 0);
  await React.act(async () => f.service.retry()); await f.receive(); assert.equal(n('#submitRoom').disabled, false);
  await React.act(async () => f.service.pageHide()); assert.equal(n('#submitRoom').disabled, true); await submit();
  assert.equal(mounted.requests, 0); assert.equal(f.navigations.length, 0);
});
test('Dialog cancel is prevented; only target dialog with coordinates strictly outside closes', async () => {
  const f = await fixture().live(); const mounted = await mountDialog(f);
  const dialog = n('#roomDialog'); dialog.getBoundingClientRect = () => new env.window.DOMRect(100, 100, 200, 200);
  async function at(target, x, y) {await React.act(async () => target.dispatchEvent(new env.window.MouseEvent('click', {bubbles: true, clientX: x, clientY: y})));}
  for (const [x, y] of [[100, 100], [300, 300], [200, 200]]) await at(dialog, x, y);
  await at(n('#dialogTitle'), 99, 99); assert.equal(mounted.requests, 0);
  for (const [x, y] of [[99, 200], [301, 200], [200, 99], [200, 301]]) await at(dialog, x, y);
  assert.equal(mounted.requests, 4); assert.equal(dialog.open, true); assert.equal(f.closeCalls, 0);
  const cancel = new env.window.Event('cancel', {cancelable: true}); await React.act(async () => dialog.dispatchEvent(cancel));
  assert.equal(cancel.defaultPrevented, true); assert.equal(mounted.requests, 5); await click('#closeDialog'); assert.equal(mounted.requests, 6);
  await mounted.m.update({open: false}); await mounted.m.update({open: false}); assert.equal(f.closeCalls, 1); assert.equal(f.navigations.length, 0);
});
test('Dialog StrictMode setup/cleanup and full unmount never execute pending entry', async () => {
  const f = await fixture().live(); f.service.prepareForm('create'); assert.equal(f.service.submitForm(), 'close');
  const m = await mount(owners.DirectoryRoomDialog, {service: f.service, assetUrl, open: true, onCloseRequest() {}}, {strict: true});
  assert.equal(n('#roomDialog').open, true); assert.equal(f.closeCalls, 0); assert.equal(f.navigations.length, 0);
  await m.unmount(); assert.equal(f.closeCalls, 0); assert.equal(f.navigations.length, 0); assert.equal(f.sessionWrites.length, 0);
});
test('Dialog controlled close calls formDidClose once, preserves native node, and reopening starts new lifetime', async () => {
  const f = await fixture().live(); const {m} = await mountDialog(f, 'create', {}, {strict: true}); const node = n('#roomDialog');
  await m.update({open: false}); await m.update({open: false}); await m.setLocale('zh-CN'); assert.equal(f.closeCalls, 1); assert.equal(n('#roomDialog'), node);
  await React.act(async () => f.service.prepareForm('join')); await m.update({open: true}); await m.update({open: false}); assert.equal(f.closeCalls, 2);
  assert.equal(f.navigations.length, 0);
});

async function mountRouter(f, {strict = false} = {}) {
  let nav; const locationAtNavigate = [], transitions = [];
  function Carrier() {
    nav = owners.useSurfaceNavigation();
    React.useLayoutEffect(() => {if (nav.directoryFormMode) f.service.restoreFormMode(nav.directoryFormMode);}, [nav.directoryFormMode]);
    return React.createElement(owners.DirectoryRoomDialog, {service: f.service, assetUrl, open: !!nav.directoryFormMode, onCloseRequest: nav.closeDirectoryForm});
  }
  let tree = React.createElement(owners.LocaleProvider, {locale: 'en'}, React.createElement(owners.SurfaceNavigationProvider,
    {dirty: false, isEditing: false, onDiscard() {}}, React.createElement(Carrier)));
  if (strict) tree = React.createElement(React.StrictMode, null, tree);
  const router = createMemoryRouter([{path: '*', element: tree}], {initialEntries: ['/lobby.html?game=th06mp&keep=1#retained']});
  router.subscribe(state => transitions.push({action: state.historyAction, location: state.location}));
  f.onNavigate = () => locationAtNavigate.push(router.state.location);
  const container = env.document.createElement('div'); env.document.body.append(container); const root = createRoot(container);
  const m = {root, router}; mounts.add(m); await React.act(async () => root.render(React.createElement(RouterProvider, {router})));
  return {router, transitions, locationAtNavigate, async open(mode) {await React.act(async () => {assert.equal(f.service.prepareForm(mode), true); nav.openDirectoryForm(mode);});},
    async back() {await React.act(async () => router.navigate(-1));}};
}
for (const mode of ['create', 'join']) test(`Actual navigation owner: ${mode} submit waits for same-URL Router close commit, then enters once`, async () => {
  const f = await fixture().live([room({code: '1023'})]); const h = await mountRouter(f, {strict: true}); const base = h.router.state.location;
  await h.open(mode); const opened = h.router.state.location; assert.notEqual(opened.key, base.key); assert.equal(opened.search, base.search); assert.equal(opened.hash, base.hash);
  assert.equal(n('#roomDialog').open, true); assert.equal(f.closeCalls, 0);
  if (mode === 'join') await change('#roomCodeInput', '5678'); else {await change('#capacitySelect', '3'); await change('#difficultySelect', '4'); await change('#visibilitySelect', 'private'); await change('#cheatSelect', '1');}
  await submit(); await flush();
  assert.equal(h.router.state.location.key, base.key); assert.equal(n('#roomDialog').open, false); assert.equal(f.closeCalls, 1); assert.equal(f.navigations.length, 1);
  assert.equal(h.locationAtNavigate[0].key, base.key); assert.equal(f.navigations[0].dialogOpen, false);
  assert.deepEqual(f.events, ['formDidClose', 'identity:th06mp', 'navigate']);
  const url = new URL(f.navigations[0].url); assert.equal(url.searchParams.get('keep'), '1'); assert.equal(url.hash, '#retained');
  const invite = owners.decodeRoomInvite(url.searchParams.get(owners.ROOM_INVITE_KEY));
  assert.equal(invite.g, 'th06mp'); assert.equal(invite.r, mode === 'create' ? '1024' : '5678'); assert.equal(invite.a, mode); assert.equal(invite.f, true);
  if (mode === 'create') {assert.equal(invite.p, 3); assert.equal(invite.d, 4); assert.equal(invite.v, 'private'); assert.equal(invite.c, true); assert.equal(f.sessionWrites[0][0], 'save');}
  else assert.deepEqual(f.sessionWrites, [['clear', 'th06mp']]);
  await flush(); assert.equal(f.closeCalls, 1); assert.equal(f.navigations.length, 1);
});
test('Actual navigation owner Back closes form once without entering a room; Forward restores mode and retained code', async () => {
  const f = await fixture().live(); const h = await mountRouter(f); await h.open('join'); await change('#roomCodeInput', '6789');
  await h.back(); assert.equal(n('#roomDialog').open, false); assert.equal(f.closeCalls, 1); assert.equal(f.navigations.length, 0);
  await React.act(async () => h.router.navigate(1)); assert.equal(n('#roomDialog').open, true); assert.equal(n('#roomCodeInput').value, '6789');
  await h.back(); assert.equal(f.closeCalls, 2); assert.equal(f.navigations.length, 0);
});
for (const locale of ['zh-CN','en']) for (const product of ['th06mp','th09mp']) test(`Room cards show recruiting/full before start and time only during play: ${locale} ${product}`, async()=>{
  const f=await fixture({locale,selectedProduct:product}).live();
  const m=await mount(owners.DirectoryRoomRows,{snapshot:f.service.getSnapshot(),service:f.service,assetUrl},{locale});
  const seats=[{initial:'A',online:true,ready:false},null];
  for(const [phase,players,key] of [['lobby',1,'lobby.recruiting'],['lobby',2,'lobby.roomFull'],['playing',2,null],['lobby',1,'lobby.recruiting']]){
    await f.receive([room({product,capacity:2,players,phase,startedAt:Date.now()-65000,seats:players===1?seats:[seats[0],{initial:'B',online:true,ready:false}]})]);
    await m.update({snapshot:f.service.getSnapshot()});

      const state=phase==='playing'?'playing':players===2?'full':'recruiting';
      assert.equal(n('.lobby-room-state').textContent,t(locale,'lobby.'+state));
      assert.equal(!!env.document.querySelector('.room-elapsed'),phase==='playing');
      if(phase==='playing') {
        if(product==='th09mp') assert.equal(n('.room-elapsed').textContent,t(locale,'room.elapsedTime',{time:'0:00'}));
        else assert.match(n('.room-elapsed').textContent,/1:0\d/);
      }
      assert.equal(env.document.querySelector('.th09-room-duration'),null);
  }
});

for (const locale of ['zh-CN', 'en']) test(`Every published multiplayer product uses room metadata and native game policy: ${locale}`, async () => {
  const f = await fixture({locale, host: {games: Object.keys(owners.PRODUCT_GAMES)}}).live();
  const rowsMount = await mount(owners.DirectoryRoomRows, {snapshot: f.service.getSnapshot(), service: f.service, assetUrl}, {locale});
  for (const product of f.service.getSnapshot().products) {
    await React.act(async () => f.service.selectProduct(product));
    await f.receive([room({product, capacity: 2, seats: [{initial: 'A', online: false, ready: true}, null]})]);
    await rowsMount.update({snapshot: f.service.getSnapshot()});
    const snapshot = f.service.getSnapshot();
    const policy = owners.multiplayerConfigForProduct(product);
    if (policy.inGameSelection && product !== 'th03mp') {
      assert.equal(env.document.querySelector('.lobby-difficulty'),null);
      assert.equal(all('.difficulty-label').length, 0);
    } else {
      assert.equal(n('.lobby-difficulty').textContent, policy.difficulties[snapshot.visibleRooms[0].difficulty]);
    }
    if (product === 'th09mp') {
      assert.equal(n('.book-room-score').textContent,'VS');

      assert.equal(all('.lobby-seat').length,2);
    } else {
      assert.equal(n('.lobby-room-code').textContent, '#' + snapshot.visibleRooms[0].code);
      assert.equal(all('.lobby-seat').length,2);
    }
  }
  await rowsMount.unmount();
  const {m} = await mountDialog(f, 'create', {}, {locale});
  for (const product of f.service.getSnapshot().products) {
    await change('#gameSelect', product);
    const policy = owners.multiplayerConfigForProduct(product);
    assert.deepEqual(all('#capacitySelect option').map(x => [x.value, x.textContent]), policy.playerCounts.map(count => [String(count), t(locale, 'lobby.playersCount', {count})]));
    assert.deepEqual(all('#difficultySelect option').map(x => [x.value, x.textContent]), policy.difficulties.map((name, index) => [String(index), name]));
    const inGameDifficulty = !!policy.inGameSelection && product !== 'th03mp';
    assert.equal(n('#difficultySelect').disabled,inGameDifficulty);
    assert.equal(n('#difficultySelect').closest('label').hidden,inGameDifficulty);
    if(inGameDifficulty) assert.ok(n('#roomForm').textContent.includes(t(locale,'room.inGameSelectionHint')));
  }
  await m.update({open: false});
});
test('No multiplayer products disables create/code/submit while keeping the original native field semantics', async () => {
  const f = await fixture({host: {games: ['th20']}}).live(); await mountLobby(f);
  assert.deepEqual(f.service.getSnapshot().products, []); assert.equal(n('#createButton').disabled, true); assert.equal(n('#codeButton').disabled, true);
  assert.equal(f.service.prepareForm('create'), false);
  await mount(owners.DirectoryRoomDialog, {service: f.service, assetUrl, open: true, onCloseRequest() {assert.fail('Unavailable form must not close');}});
  assert.equal(n('#submitRoom').disabled, true); assert.equal(all('#gameSelect option').length, 0);
  for (const select of all('#roomDialog select')) assert.equal(select.disabled, false, 'Main disables submit, not native form fields');
  await submit(); assert.equal(f.closeCalls, 0); assert.equal(f.navigations.length, 0);
});
test('Repeated submit in one turn issues one actual Router pop and one room entry', async () => {
  const f = await fixture().live(); const h = await mountRouter(f, {strict: true}); const base = h.router.state.location;
  await h.open('create'); h.transitions.length = 0;
  await React.act(async () => {for (let i = 0; i < 2; i++) n('#roomForm').dispatchEvent(new env.window.Event('submit', {bubbles: true, cancelable: true}));});
  assert.equal(h.router.state.location.key, base.key); assert.equal(h.transitions.filter(t => t.action === 'POP').length, 1);
  assert.equal(f.closeCalls, 1); assert.equal(f.navigations.length, 1); assert.equal(f.navigations[0].dialogOpen, false);
});
test('Directory views/service do not introduce a second history owner', () => {
  for (const file of ['DirectoryRoomRows', 'LobbyDirectoryView', 'DirectoryRoomDialog']) {
    const source = readFileSync(resolve(project, `app/components/directory/${file}.tsx`), 'utf8');
    assert.doesNotMatch(source, /\b(?:pushState|replaceState|popstate|history\s*\.|useNavigate|useBlocker)\b/);
  }
  assert.doesNotMatch(readFileSync(resolve(project, 'app/services/lobby-directory.ts'), 'utf8'), /\b(?:pushState|replaceState|popstate|history\s*\.|useNavigate|useBlocker)\b/);
});

for (const parentKey of ['original-parent', null]) test(`Dialog submission completes only its originating return context (parent=${parentKey})`, async () => {
  const f = await fixture().live();
  const completionContext = {locationKey: 'form-entry', parentKey, href: '/lobby.html?game=th06mp'};
  const {m} = await mountDialog(f, 'join', {completionContext}); await change('#roomCodeInput', '1234'); await submit();
  await m.update({open: false, completionContext: {...completionContext, locationKey: parentKey ?? 'replacement-entry'}});
  assert.equal(f.navigations.length, 1); assert.equal(f.navigations[0].dialogOpen, false);
});
for (const parentKey of ['original-parent', null]) test(`Dialog newer destination discards its submission (parent=${parentKey})`, async () => {
  const f = await fixture().live();
  const completionContext = {locationKey: 'form-entry', parentKey, href: '/lobby.html?game=th06mp'};
  const {m} = await mountDialog(f, 'join', {completionContext}); await change('#roomCodeInput', '1234'); await submit();
  await m.update({open: false, completionContext: {...completionContext, locationKey: 'newer-entry', href: '/lobby.html?game=th07mp'}});
  assert.equal(f.navigations.length, 0); assert.equal(f.sessionWrites.length, 0);
  await React.act(async () => {f.service.prepareForm('join'); f.service.updateForm({code: '5678'});});
  await m.update({open: true, completionContext: {locationKey: 'reopened-entry', parentKey: 'newer-entry', href: '/lobby.html?game=th07mp'}});
  await submit(); await m.update({open: false, completionContext: {locationKey: 'newer-entry', parentKey: null, href: '/lobby.html?game=th07mp'}});
  assert.equal(f.navigations.length, 1); assert.equal(owners.decodeRoomInvite(new URL(f.navigations[0].url).searchParams.get('j')).r, '5678');
});
test('A retired dialog receipt cannot consume or discard a later form submission', async () => {
  const f = await fixture().live(), oldOwner = {}, newOwner = {};
  assert.equal(f.service.prepareForm('join'), true); f.service.updateForm({code: '1234'}); assert.equal(f.service.submitForm(oldOwner), 'close');
  assert.equal(f.service.prepareForm('join'), true); f.service.updateForm({code: '5678'}); assert.equal(f.service.submitForm(newOwner), 'close');
  f.service.formDidClose(oldOwner, false); f.service.formDidClose(oldOwner); f.service.formDidClose();
  assert.equal(f.navigations.length, 0); assert.equal(f.sessionWrites.length, 0);
  f.service.formDidClose(newOwner); f.service.formDidClose(newOwner);
  assert.equal(f.navigations.length, 1); assert.equal(owners.decodeRoomInvite(new URL(f.navigations[0].url).searchParams.get('j')).r, '5678');
});
test('Closing a dialog without submission cannot consume another form receipt', async () => {
  const f = await fixture().live(), {m} = await mountDialog(f, 'join'), newOwner = {};
  await React.act(async () => {f.service.updateForm({code: '5678'}); assert.equal(f.service.submitForm(newOwner), 'close');});
  await m.update({open: false}); await m.unmount(); assert.equal(f.navigations.length, 0);
  f.service.formDidClose(newOwner); assert.equal(f.navigations.length, 1);
});
test('Unmount discards only its submitted receipt before a later dialog can close', async () => {
  const f = await fixture().live(), {m} = await mountDialog(f, 'join'); await change('#roomCodeInput', '1234'); await submit();
  await m.unmount(); assert.equal(f.navigations.length, 0); assert.equal(f.sessionWrites.length, 0);
  f.service.formDidClose(); assert.equal(f.navigations.length, 0);
});

test('Local game filtering returns to all rooms without a loading request', async () => {
  const f = await fixture({selectedProduct: '', clientSideFiltering: true}).live([
    room({product: 'th06mp', code: '1234'}), room({product: 'th07mp', code: '2345'}),
  ]);
  const {m} = await mountLobby(f, {productFilters: []});
  const sent = f.socket.sent.length;
  await m.update({productFilters: ['th06mp']});
  await React.act(async () => f.service.selectProduct('th06mp'));
  assert.deepEqual(all('.lobby-room-row').map(row => row.dataset.key), ['th06mp-1234']);
  assert.equal(n('#listLoading').hidden, true);
  await m.update({productFilters: []});
  assert.deepEqual(all('.lobby-room-row').map(row => row.dataset.key), ['th06mp-1234', 'th07mp-2345']);
  await React.act(async () => f.service.selectProduct(''));
  assert.equal(n('#listLoading').hidden, true);
  assert.equal(f.socket.sent.length, sent);
  await React.act(async () => f.service.selectProduct('th06mp'));
  await React.act(async () => assert.equal(f.service.prepareForm('create'), true));
  assert.equal(f.service.getSnapshot().form.product, 'th06mp');
  await React.act(async () => f.service.refreshRooms());
  assert.equal(f.socket.sent.at(-1).product, '');
});

test('Truncated all-game response falls back to server filtering and restores the selected room', async () => {
  const f = fixture({selectedProduct: 'th06mp', clientSideFiltering: true});
  const {m} = await mountLobby(f);
  await f.start();
  const truncated = Array.from({length: 200}, (_, index) => room({product: 'th07mp', code: String(1000 + index)}));
  await f.receive(truncated, {product: '', total: 201});
  assert.deepEqual(f.socket.sent.at(-1), {type: 'refresh', product: 'th06mp'});
  assert.equal(f.service.getSnapshot().loading, true);

  await f.receive([room({product: 'th06mp', code: '4321'})], {product: 'th06mp', total: 1});
  assert.deepEqual(f.service.getSnapshot().visibleRooms.map(item => `${item.product}-${item.code}`), ['th06mp-4321']);
  assert.deepEqual(all('.lobby-room-row').map(row => row.dataset.key), ['th06mp-4321']);

  const sent = f.socket.sent.length;
  await React.act(async () => f.service.selectProduct('th07mp'));
  assert.equal(f.socket.sent.length, sent, 'server filter keeps the original debounce');
  const [timerId, filterTimer] = [...f.timers].find(([, timer]) => timer.delay === 180) || [];
  assert.ok(timerId);
  await React.act(async () => filterTimer.fn());
  assert.deepEqual(f.socket.sent.at(-1), {type: 'refresh', product: 'th07mp'});
  await f.receive([room({product: 'th07mp', code: '5678'})], {product: 'th07mp', total: 1});
  assert.deepEqual(f.service.getSnapshot().visibleRooms.map(item => `${item.product}-${item.code}`), ['th07mp-5678']);
  assert.deepEqual(all('#roomList .lobby-room-row:not(.lobby-room-exiting)').map(row => row.dataset.key), ['th07mp-5678']);
  assert.equal(all('#roomList .lobby-room-exiting').length, 1);
  await React.act(async () => {await new Promise(resolve => setTimeout(resolve, 550));});
  assert.deepEqual(all('#roomList .lobby-room-row').map(row => row.dataset.key), ['th07mp-5678']);
  await m.unmount();
});

test('All multiplayer requests the unfiltered directory; multi-selection shows only checked games', async () => {
  const f = await fixture({selectedProduct: ''}).live([
    room({product: 'th06mp', code: '1234'}), room({product: 'th07mp', code: '2345'}), room({product: 'th08mp', code: '3456'}),
  ]);
  assert.equal(f.service.getSnapshot().selectedProduct, '');
  assert.equal(f.service.getSnapshot().visibleRooms.length, 3);
  const {m} = await mountLobby(f, {productFilters: ['th06mp', 'th08mp']});
  assert.deepEqual(all('.lobby-room-row').map(n => n.dataset.key), ['th06mp-1234', 'th08mp-3456']);
  await m.update({productFilters: []});
  assert.equal(all('.lobby-room-row').length, 3);
  await React.act(async () => f.service.selectProduct('th07mp'));
  await f.receive([room({product: 'th07mp', code: '2345'})]);
  await React.act(async () => f.service.selectProduct(''));
  assert.equal(f.service.getSnapshot().selectedProduct, '');
  await React.act(async () => f.service.refreshRooms());
  assert.equal(f.socket.sent.at(-1).product, '');
});

for (const product of ['th06mp','th09mp']) test(`Approved public names remain complete in directory cards (${product})`,async()=>{
  const names=['完整公开昵称甲乙丙丁戊己','完整公开昵称'];
  const f=await fixture({selectedProduct:product}).live([room({product,publicNames:true,capacity:2,seats:names.map(name=>({initial:name[0],name,online:true}))})]);
  const snapshot=f.service.getSnapshot();
  assert.deepEqual(snapshot.visibleRooms[0].seats.map(seat=>seat.name),names);
  await mount(owners.DirectoryRoomRows,{snapshot,service:f.service,assetUrl,initialRows:true});
  assert.deepEqual(all('.lobby-seat-name').map(node=>node.textContent),names);
});

for (const product of ['th06mp','th09mp']) test(`Approved P1/P2 names are public names, not placeholders (${product})`,async()=>{
  const names=['P1','P2'];
  const f=await fixture({selectedProduct:product}).live([room({product,publicNames:true,capacity:2,seats:names.map(name=>({initial:name[0],name,online:true,controlMode:'normal'}))})]);
  await mount(owners.DirectoryRoomRows,{snapshot:f.service.getSnapshot(),service:f.service,assetUrl});
  assert.deepEqual(all('.lobby-seat-name').map(node=>node.textContent),names);
  assert.equal(all('.lobby-seat-control').length,2);
});

test('Legacy duel names use the machine label, then initial, and describe empty seats',async()=>{
  const f=await fixture({selectedProduct:'th09mp'}).live([room({product:'th09mp',capacity:2,seats:[{initial:'花',machineSign:'',online:true},null]})]);
  const m=await mount(owners.DirectoryRoomRows,{snapshot:f.service.getSnapshot(),service:f.service,assetUrl});
  assert.deepEqual(all('.lobby-seat-name').map(node=>node.textContent),['花',t('en','th09.waitingPlayer')]);
  await f.receive([room({product:'th09mp',capacity:2,seats:[{initial:'花',machineSign:'Reimu',online:true},null]})]);
  await m.update({snapshot:f.service.getSnapshot()});
  assert.equal(n('.lobby-seat-name').textContent,'Reimu');
});

test('TH19 directory preserves and updates whole-match scores received from relay', async () => {
  const row = wins => room({product:'th19mp',capacity:2,players:2,seats:wins.map((win,index)=>({initial:String(index),online:true,stats:{wins:win,losses:0,minutes:8}}))});
  const f = await fixture({selectedProduct:'th19mp',host:{games:['th19']}}).live([row([0,0])]);
  const m = await mount(owners.DirectoryRoomRows,{snapshot:f.service.getSnapshot(),service:f.service,assetUrl});
  assert.equal(n('.book-room-score-value').textContent,'0 : 0');
  for (const wins of [[1,0],[1,1],[1,2]]) {
    await f.receive([row(wins)]);
    await m.update({snapshot:f.service.getSnapshot()});
    assert.equal(n('.book-room-score-value').textContent,wins.join(' : '));
  }
  await m.unmount();
});

test('Flower View keeps Japanese identity, score, shared facts and one contextual entry', async () => {
  const f = await fixture({selectedProduct:'th09mp',locale:'zh-CN'}).live([room({product:'th09mp',capacity:2,phase:'playing',startedAt:Date.now()-65000,players:2,joinable:false,seats:[{initial:'A',online:true,stats:{wins:3,losses:2,minutes:7}},{initial:'B',online:true,stats:{wins:5,losses:1,minutes:8}}]})]);
  const m=await mount(owners.DirectoryRoomRows,{snapshot:f.service.getSnapshot(),service:f.service,assetUrl},{locale:'zh-CN'});
  assert.equal(n('.lobby-room-title h2').textContent,'東方花映塚');
  assert.equal(env.document.querySelector('.lobby-room-subtitle'),null);
  assert.equal(n('.book-room-score-value').textContent,'3 : 5');
  assert.equal(n('.book-room-score').getAttribute('aria-label'),`${t('zh-CN','th09.score')} 3 : 5`);
  assert.equal(n('.room-elapsed').textContent,t('zh-CN','room.elapsedTime',{time:'8:00'}));
  assert.equal(env.document.querySelector('.lobby-difficulty'),null);
  assert.equal(all('.lobby-room-row button').length,1);
  assert.equal(n('.lobby-join').textContent,t('zh-CN','multiplayer.joinSpectator'));
  await click('.lobby-join');
  assert.equal(owners.decodeRoomInvite(new URL(f.navigations[0].url).searchParams.get('j')).s,true);
  await m.unmount();
});

test('Directory gameplay flags use their own column and regular rooms name their rules', async () => {
  const f=await fixture({locale:'zh-CN'}).live([room({disableCheatMovement:true,challengeMode:true})]);
  const {m}=await mountLobby(f,{}, {locale:'zh-CN'});
  assert.equal(n('.lobby-room-rules').parentElement,n('.lobby-room-row'));
  assert.equal(n('.lobby-room-meta').nextElementSibling,n('.lobby-room-facts'));
  assert.equal(n('.lobby-difficulty').dataset.difficulty,'normal');
  assert.ok(n('.lobby-room-rules').textContent.includes(t('zh-CN','lobby.noCheat')));
  assert.ok(n('.lobby-room-rules').textContent.includes(t('zh-CN','room.challengeMode')));
  await f.receive([room()]);
  assert.deepEqual([...n('.lobby-room-rules').children].map(node=>node.textContent),[t('zh-CN','directory.regularRules')]);
  assert.equal(n('.lobby-room-rules').firstElementChild.classList.contains('lobby-room-rules-standard'),true);
});


test('room quick filters distinguish playable seats, waiting rooms and spectators and update with the live directory', async () => {
  const rooms = [room({code: '1111'}), room({code: '2222', players: 3, seats: [...room().seats.slice(0, 2), {initial: 'S', online: true, ready: false, controlMode: 'normal'}]}), room({code: '3333', phase: 'playing', players: 3}), room({code: '4444', joinable: false})];
  const f = await fixture({selectedProduct: ''}).live(rooms);
  await mountLobby(f);
  const keys = () => all('.lobby-room-row').map(n => n.dataset.key);
  const filter = async id => {
    await click('.book-room-filter-trigger');
    assert.equal(n('.book-room-filter-trigger').getAttribute('aria-expanded'),'true');
    await click(`[data-room-filter="${id}"]`);
    assert.equal(n('.book-room-filter-trigger').getAttribute('aria-expanded'),'false','Single-choice room filter closes after selection');
    await React.act(async()=>new Promise(resolve=>env.window.requestAnimationFrame(()=>env.window.requestAnimationFrame(resolve))));
  };
  assert.equal(keys().length, 4);
  await filter('open');
  assert.deepEqual(keys(), ['th06mp-1111']);
  await filter('waiting');
  assert.deepEqual(keys(), ['th06mp-1111', 'th06mp-2222', 'th06mp-4444']);
  await filter('spectate');
  assert.deepEqual(keys(), ['th06mp-2222', 'th06mp-3333']);
  await filter('open');
  await f.receive(rooms.slice(1));
  assert.equal(all('.lobby-room-exiting').length,1);
  assert.equal(n('.lobby-room-exiting').getAttribute('aria-hidden'),'true');
  await React.act(async()=>{await new Promise(resolve=>setTimeout(resolve,550));});
  assert.equal(keys().length, 0);
  assert.equal(n('#emptyTitle').textContent, 'No matching rooms');
  await click('#emptyAction');
  await click('.book-room-filter-trigger');
  assert.equal(n('[data-room-filter="all"]').getAttribute('aria-checked'), 'true');
  assert.equal(keys().length, 3);
  assert.equal(f.navigations.length, 0, 'filtering never enters a room');
});
