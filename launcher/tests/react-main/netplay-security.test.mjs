import test, {before, after, afterEach} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';

// Mounted components and synthetic DOM only. Network admission is tested with
// real isolated WebSockets in test-relay-membership/room-lookup-security.
const project = resolve(import.meta.dirname, '../..');
const require = createRequire(resolve(project, 'package.json'));
let env, work, React, createRoot, views;
const mounts = [];
before(async () => {
  env = installMountedDom();
  React = await import('react');
  ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(project, '.cache/netplay-security-'));
  const outfile = resolve(work, 'views.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export * from './app/components/directory/DirectoryRoomDialog.tsx';
    export * from './app/components/player/TitleNetworkOverlay.tsx';
    export * from './app/components/room/RoomSeats.tsx';
    export * from './app/i18n.tsx';
  `}, bundle: true, format: 'esm', platform: 'node', jsx: 'automatic', outfile,
  logLevel: 'silent', plugins: [{name: 'shared-react', setup(ctx) {
    ctx.onResolve({filter: /^[^./]/}, args => ({path: pathToFileURL(require.resolve(args.path)).href, external: true}));
  }}]});
  views = await import(pathToFileURL(outfile).href);
});
async function mount(component, props, locale) {
  const container = env.document.createElement('div');
  env.document.body.append(container);
  const root = createRoot(container);
  mounts.push(root);
  await React.act(async () => root.render(React.createElement(views.LocaleProvider, {locale},
    React.createElement(component, props))));
  return container;
}
afterEach(async () => {
  for (const root of mounts.splice(0)) await React.act(async () => root.unmount());
  env.document.body.replaceChildren();
  assert.deepEqual(env.errors.splice(0), []);
});
after(async () => {env?.close(); if (work) await rm(work, {recursive: true, force: true});});
for (const locale of ['zh-CN', 'en']) {
  test(`${locale}: host offers a spectator move before removal and hides moderation during play`, async () => {
    const state={product:'th06mp',connected:true,seat:0,preferredLoadout:0,
      localClientId:'host12345',room:{code:'12345678',synced:true,phase:'lobby',playerCount:2,
        seats:[{clientId:'host12345',loadout:0},{clientId:'guest12345',loadout:1}],spectators:[]}};
    const moves=[],removals=[];
    const props={state,service:{movePlayerToSpectator:(...args)=>moves.push(args),removePlayer:i=>removals.push(i)},
      network:{metric:()=>null},context:{touchEnabled:false,touchMovementMode:'joystick'},openPanel(){}};
    const node=await mount(views.RoomSeats,props,locale);
    const menu=node.querySelector('[data-mp-seat="1"] .mp-seat-moderation');
    assert.equal(menu.hidden,false);
    const trigger=menu.querySelector('[aria-haspopup="menu"]');
    await React.act(async()=>trigger.click());
    const buttons=[...env.document.querySelectorAll('.room-member-menu [role="menuitem"]')];
    assert.equal(buttons[0].textContent,locale==='en'?'Move to spectators':'移到观众席');
    await React.act(async()=>buttons[0].click());
    assert.deepEqual(moves,[[1,'guest12345']]);
    assert.deepEqual(removals,[]);
    assert.equal(trigger.getAttribute('aria-expanded'),'false');
    const playing=await mount(views.RoomSeats,{...props,state:{...state,room:{...state.room,phase:'running'}}},locale);
    assert.equal(playing.querySelector('[data-mp-seat="1"] .mp-seat-moderation').hidden,true);
    const guest=await mount(views.RoomSeats,{...props,state:{...state,seat:1}},locale);
    assert.equal(guest.querySelector('[data-mp-seat="1"] .mp-seat-moderation').hidden,true);
  });
  test(`${locale}: unlisted-room limits appear before submitting entry`, async () => {
    const state = {connection: 'live', mine: null, leaving: false, products: ['th06mp'],
      form: {mode: 'create', product: 'th06mp', capacity: 2, difficulty: 1,
        visibility: 'private', disableCheatMovement: false, code: '', validationMessage: ''}};
    let submitted = 0;
    const service = {getSnapshot: () => state, subscribe: () => () => {},
      formDidClose() {}, updateForm() {}, submitForm() {++submitted; return 'unavailable';}};
    const node = await mount(views.DirectoryRoomDialog, {service, open: true, onCloseRequest() {}}, locale);
    assert.equal(node.querySelector('dialog').open, true);
    assert.equal(node.querySelector('#visibilitySelect option[value="private"]').textContent,
      locale === 'en' ? 'Unlisted' : '隐藏');
    assert.match(node.querySelector('#formNote').textContent,
      locale === 'en' ? /Anyone with the room code can join/ : /任何知道房间号的人都能加入/);
    assert.equal(submitted, 0, 'showing the room rules does not submit entry');
    await React.act(async () => node.querySelector('form').dispatchEvent(new Event('submit', {bubbles: true, cancelable: true})));
    assert.equal(submitted, 1, 'entry remains a deliberate form action');
  });
  test(`${locale}: in-game entry accepts a four-digit room code`, async () => {
    const state = {open: true, room: false, code: '4345', focusRequest: 1};
    let joined = 0;
    const model = {getSnapshot: () => state, subscribe: () => () => {},
      create() {}, join() {++joined;}, close() {}, setCode() {}};
    const node = await mount(views.TitleNetworkOverlay, {model}, locale);
    assert.equal(node.querySelector('#th09NetworkDialog').hidden, false);
    assert.equal(node.querySelector('#th09NetworkCode').maxLength, 4);
    assert.equal(node.querySelector('#th09NetworkCode').value, '4345');
    assert.equal(joined, 0);
    await React.act(async () => node.querySelector('#th09NetworkJoin').click());
    assert.equal(joined, 1);
  });
}
