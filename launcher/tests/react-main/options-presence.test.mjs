/** Actual React hook/panels in synthetic DOM + Memory Router only.
 * The clock and media queries are explicit fixtures. No browser, rendered CSS,
 * mobile device, native runtime, network or deployment validation is claimed.
 */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdir, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {authoredSourcesPlugin} from './authored-sources.mjs';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {installNativeTimeline} from './native-animation-environment.mjs';
const motions=[];

const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, React, createRoot, createMemoryRouter, RouterProvider, useLocation, useNavigate, owners;
let mounted = null;

before(async () => {
  env = installMountedDom();
  installNativeTimeline(env.window,motions);
  React = await import('react');
  ({createRoot} = await import('react-dom/client'));
  ({createMemoryRouter, RouterProvider, useLocation, useNavigate} = await import('react-router'));
  await mkdir(resolve(project, '.cache'), {recursive: true});
  work = await mkdtemp(resolve(project, '.cache/options-presence-'));
  const output = resolve(work, 'actual-owners.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export {OptionsOwnership} from './app/components/launcher/options-ownership.ts';
    export {useLibraryOptionsPresence} from './app/components/launcher/use-library-options-presence.ts';
    export {OptionsPanel} from './app/components/launcher/OptionsPanel.tsx';
    export {OptionsBackdrop} from './app/components/launcher/OptionsBackdrop.tsx';
    export {LibrarySurface} from './app/components/launcher/LibrarySurface.tsx';
    export {LibraryCards} from './app/components/launcher/LibraryCards.tsx';
    export {LobbyOptionsHost} from './app/components/launcher/LobbyOptionsHost.tsx';
    export {MainSelect} from './app/components/launcher/MainSelect.tsx';
    export {LocaleProvider} from './app/i18n.tsx';
    export {createLibraryProducts} from './app/components/launcher/products.ts';
  `}, bundle: true, platform: 'node', format: 'esm', jsx: 'automatic', outfile: output,
  packages: 'external', logLevel: 'silent', plugins: [authoredSourcesPlugin(project)]});
  owners = await import(pathToFileURL(output).href);
});

afterEach(async () => {
  if (mounted) {
    await unmount();
    mounted.clock.restore();
    mounted.router.dispose();
    mounted = null;
  }
  assert.deepEqual(env.errors, [], 'uncaught DOM/React errors fail this synthetic fixture');
  assert.equal(env.document.querySelectorAll('.ui-select-content').length, 0, 'detached menus are disposed');
  assert.equal(env.document.body.classList.contains('library-tools-open'), false);
  assert.equal(env.document.body.classList.contains('library-tools-closing'), false);
  env.document.body.replaceChildren();
  motions.length=0;
});
after(async () => {env?.close(); if (work) await rm(work, {recursive: true, force: true});});

function syntheticClock() {
  const originalSet = env.window.setTimeout, originalClear = env.window.clearTimeout;
  const tasks = new Map();
  let now = 0, nextId = 1;
  env.window.setTimeout = (callback, delay = 0, ...args) => {
    const id = nextId++;
    tasks.set(id, {id, delay, at: now + delay, run: () => callback(...args)});
    return id;
  };
  env.window.clearTimeout = id => {tasks.delete(id);};
  return {
    tasks,
    async advance(milliseconds) {
      const end = now + milliseconds;
      while (true) {
        const next = [...tasks.values()].filter(task => task.at <= end).sort((a, b) => a.at - b.at)[0];
        if (!next) break;
        now = next.at; tasks.delete(next.id);
        await React.act(async () => {next.run();});
      }
      now = end;
    },
    restore() {env.window.setTimeout = originalSet; env.window.clearTimeout = originalClear; tasks.clear();},
  };
}

async function mount({entry = '/?game=th06', mobile = true, coarse = false, noHover = false,
  reduced = false, lessMotion = false, strict = false} = {}) {
  env.errors.length = 0;
  env.window.matchMedia = query => ({media: query,
    matches: query.includes('prefers-reduced-motion') ? reduced :
      (query.includes('max-width: 780px') && mobile) || (query.includes('pointer: coarse') && coarse) || (query.includes('hover: none') && noHover),
    onchange: null, addEventListener() {}, removeEventListener() {}, addListener() {}, removeListener() {}, dispatchEvent() {return true;}});
  globalThis.matchMedia = env.window.matchMedia;
  const clock = syntheticClock();
  const products = owners.createLibraryProducts(['th06', 'th07'], path => `/fixtures/${path}`, id => `/?game=${id}`);
  const observed = [];
  function Fixture() {
    const location = useLocation(), navigate = useNavigate();
    const productId = new URLSearchParams(location.search).get('game');
    const context = location.pathname === '/lobby.html' ? 'lobby' : 'library';
    const presence = owners.useLibraryOptionsPresence({open: productId !== null, context,
      product: products.find(product => product.id === productId), lessMotion, concealed: location.hash === '#playing'});
    observed.push(presence);
    const onBack = () => navigate(context === 'lobby' ? '/lobby.html' : '/');
    const panel = presence.product && React.createElement(owners.OptionsPanel, {product: presence.product,
      open: presence.open, closing:presence.closing, onBack, assetUrl: path => `/fixtures/${path}`},
    React.createElement(owners.MainSelect, {id: 'presenceSelect', 'aria-label': 'Synthetic options select', defaultValue: 'a'},
      React.createElement('option', {value: 'a'}, 'A'), React.createElement('option', {value: 'b'}, 'B')));
    return React.createElement(owners.OptionsOwnership, {value: presence.targets}, React.createElement(owners.LocaleProvider, {locale: 'en'},
      context === 'library' ? React.createElement(React.Fragment,null,React.createElement(owners.LibrarySurface, {products,
        selectedProduct: 'th06', openedProduct: presence.openedProduct, optionsOpen: presence.open,
        onSelect() {}, onActivate: id => navigate(`/?game=${id}`), onBack,
        masthead: null, footer: null, lobbyHref: '/lobby.html', roomUsersArtwork: '/fixtures/room-users.svg'}), React.createElement(owners.OptionsBackdrop,{onBack}),panel)
        : React.createElement('div', {'data-fixture-lobby': true, className: 'main library-layout'},
          React.createElement('div', {className: 'game-library', inert: presence.open},
            React.createElement(owners.LibraryCards, {products, variant: 'lobby', selectedProduct: 'th06', openedProduct: presence.openedProduct,
              onSelect() {}, onActivate: id => navigate(`/lobby.html?game=${id}`)})),
          React.createElement(owners.LobbyOptionsHost, {open: presence.open, onCloseRequest: onBack, foregroundActive: location.hash === '#playing'}, panel))));
  }
  const fixture = React.createElement(Fixture);
  const router = createMemoryRouter([{path: '*', element: strict ? React.createElement(React.StrictMode, null, fixture) : fixture}], {initialEntries: [entry]});
  const container = env.document.createElement('div'); env.document.body.append(container);
  const root = createRoot(container);
  mounted = {clock, router, root, observed, isMounted: true};
  await React.act(async () => {root.render(React.createElement(RouterProvider, {router}));});
  return mounted;
}

async function unmount() {
  if (!mounted?.isMounted) return;
  mounted.isMounted = false;
  await React.act(async () => {mounted.root.unmount();});
}
async function navigate(url) {await React.act(async () => {await mounted.router.navigate(url);});}
function node(selector) {const found = env.document.querySelector(selector); assert.ok(found, `missing ${selector}`); return found;}
function hasBodyClass(name) {return env.document.body.classList.contains(name);}
function watchFocus(element) {
  const actual = element.focus.bind(element), calls = [];
  element.focus = options => {
    calls.push({options, inert: !!element.closest('[inert]'), open: hasBodyClass('library-tools-open'), closing: hasBodyClass('library-tools-closing')});
    actual(options);
  };
  return calls;
}
function assertOpen(product = 'th06',closing=false) {
  assert.equal(hasBodyClass('library-tools-open'), true);
  assert.equal(node('.tools').getAttribute('role'), 'dialog');
  assert.equal(node('.tools').getAttribute('aria-modal'),closing?null:'true');
  assert.equal(node('.tools').getAttribute('data-state'),closing?'closed':'open');
  assert.equal(node('.tools').hasAttribute('inert'),closing);
  assert.equal(node('.tools').classList.contains('is-open'), true);
  assert.ok(node('.game-library').hasAttribute('inert'));
  assert.equal(node(':is(.game.selected,.book-game-link[aria-current="page"])').dataset.game, product);
  assert.equal(node(':is(.game.selected,.book-game-link[aria-current="page"])').getAttribute('aria-current'), 'page');
}
function assertClosed() {
  assert.equal(hasBodyClass('library-tools-open'), false);
  assert.equal(hasBodyClass('library-tools-closing'), false);
  assert.equal(node('.tools').getAttribute('role'), 'dialog');
  assert.equal(node('.tools').hasAttribute('aria-modal'), false, 'a closed inert surface does not claim modal ownership');
  assert.equal(node('.tools').hasAttribute('inert'), true);
  assert.equal(node('.tools').classList.contains('is-open'), false);
  assert.equal(node('.game-library').hasAttribute('inert'), false);
  assert.equal(env.document.querySelector(':is(.game.selected,.book-game-link[aria-current="page"])'), null);
  assert.equal(node(':is(.game-th06,.book-game-link[data-game="th06"])').getAttribute('aria-current'), 'false');
}


function currentMotion() {
  const panel=node('.tools');
  const motion=motions.findLast(item=>item.node===panel&&!item.settled&&!item.cancelled);
  assert.ok(motion,'Options have an active native animation');return motion;
}
async function finishOptions() {
  const panel=node('.tools');currentMotion();
  await React.act(async()=>{for(const motion of motions.filter(item=>(item.node===panel||item.node.id==='libraryBackdrop')&&!item.settled&&!item.cancelled))motion.finish();});
}

for(const context of ['library','lobby'])for(const device of ['desktop','mobile','coarse'])test(context+' '+device+': route commits before completion, preserving controls and selection until focus can return',async()=>{
  const home=context==='lobby'?'/lobby.html':'/';
  const {clock,observed}=await mount({entry:home+'?game=th06',mobile:device==='mobile',coarse:device==='coarse'});
  const panel=node('.tools'),card=node(':is(.game.selected,.book-game-link[aria-current="page"])'),focus=watchFocus(card);
  await React.act(async()=>{panel.querySelector('.ui-select-trigger').click();});
  assert.ok(node('.ui-select-content'));
  await navigate(home);
  assert.equal(mounted.router.state.location.search,'');assertOpen('th06',true);assert.equal(hasBodyClass('library-tools-closing'),true);
  assert.equal(node('.tools'),panel);assert.equal(observed.at(-1).closing,true);
  assert.equal(env.document.querySelector('.ui-select-content'),null);assert.deepEqual(focus,[]);
  if(context==='lobby')assert.equal(node('#lobbyOptionsDialog').open,true);
  await clock.advance(10000);assertOpen('th06',true);assert.deepEqual(focus,[]);
  await finishOptions();assertClosed();assert.equal(node('.tools'),panel);
  assert.equal(observed.at(-1).closing,false);assert.equal(env.document.activeElement,card);
  assert.equal(focus.length,1);assert.equal(focus[0].inert,false);
  if(context==='lobby')assert.equal(node('#lobbyOptionsDialog').open,false);
});

for(const setting of ['system','site','zero','unsupported'])test(setting+': no-motion closing releases selection immediately',async()=>{
  const {clock}=await mount({reduced:setting==='system',lessMotion:setting==='site'});
  const panel=node('.tools');
  if(setting==='zero')panel.style.setProperty('--ui-surface-duration','0ms');
  if(setting==='unsupported')panel.animate=undefined;
  await navigate('/');assertClosed();
  assert.equal([...clock.tasks.values()].some(task=>task.delay===220||task.delay===180),false);
});

test('reopening cancels the prior close and cannot move focus to a retired selection',async()=>{
  const {clock}=await mount();const first=node(':is(.game.selected,.book-game-link[aria-current="page"])'),firstFocus=watchFocus(first);
  await navigate('/');const closing=currentMotion();
  await navigate('/?game=th07');assert.equal(closing.cancelled,true);assertOpen('th07');
  await React.act(async()=>closing.finish());await clock.advance(10000);assertOpen('th07');assert.deepEqual(firstFocus,[]);
  await finishOptions();assertOpen('th07');
  const second=node(':is(.game.selected,.book-game-link[aria-current="page"])'),secondFocus=watchFocus(second);
  await navigate('/');await finishOptions();assertClosed();assert.equal(secondFocus.length,1);assert.deepEqual(firstFocus,[]);
});

test('repeated closed Router updates do not restart a pending animation',async()=>{
  await mount();await navigate('/');const closing=currentMotion();
  await navigate('/?keep=still-home');assert.equal(currentMotion(),closing);
  await finishOptions();assertClosed();assert.equal(mounted.router.state.location.search,'?keep=still-home');
});

test('context changes retire the old exit without stealing focus',async()=>{
  await mount();const focus=watchFocus(node(':is(.game.selected,.book-game-link[aria-current="page"])'));
  await navigate('/');const closing=currentMotion();await navigate('/lobby.html');
  assert.equal(closing.cancelled,true);assert.equal(hasBodyClass('library-tools-open'),false);assert.equal(hasBodyClass('library-tools-closing'),false);
  await React.act(async()=>closing.finish());assert.deepEqual(focus,[]);assert.ok(node('[data-fixture-lobby]'));
});

test('StrictMode and unmount cancel effects and stale callbacks without body state or focus changes',async()=>{
  const {clock}=await mount({strict:true});assertOpen();const focus=watchFocus(node(':is(.game.selected,.book-game-link[aria-current="page"])'));
  await navigate('/');const closing=currentMotion();await unmount();assert.equal(closing.cancelled,true);
  await React.act(async()=>closing.finish());await clock.advance(10000);
  assert.deepEqual(focus,[]);assert.equal(hasBodyClass('library-tools-open'),false);assert.equal(hasBodyClass('library-tools-closing'),false);
});

test('initial closed state never creates an animation, selection or focus return',async()=>{
  const {observed}=await mount({entry:'/',strict:true});assert.equal(observed.at(-1).open,false);
  assert.equal(env.document.querySelector(':is(.game.selected,.book-game-link[aria-current="page"])'),null);assert.equal(motions.length,0);
});

test('launched Player conceals retained options and restores the same nodes when it leaves',async()=>{
  await mount();const panel=node('.tools'),focus=watchFocus(node(':is(.game.selected,.book-game-link[aria-current="page"])'));
  await navigate('/?game=th06#playing');assertClosed();assert.equal(node('.tools'),panel);assert.deepEqual(focus,[]);
  await navigate('/?game=th06');assertOpen();assert.equal(node('.tools'),panel);
});

for(const context of ['library','lobby'])test(context+': a surface timing token controls the real animation, with no mirrored deadline',async()=>{
  const home=context==='lobby'?'/lobby.html':'/';const {clock}=await mount({entry:home+'?game=th06'});
  node('.tools').style.setProperty('--ui-surface-duration','360ms');await navigate(home);
  assert.equal(currentMotion().options.duration,360);await clock.advance(10000);assertOpen('th06',true);await finishOptions();assertClosed();
});

test('enabling less motion during close finishes the active effect and restores focus once',async()=>{
  await mount();const focus=watchFocus(node(':is(.game.selected,.book-game-link[aria-current="page"])'));await navigate('/');
  await React.act(async()=>{env.document.body.classList.add('less-motion');});
  assertClosed();assert.equal(focus.length,1);env.document.body.classList.remove('less-motion');
});

test('inactive retained carriers cannot capture focus or force entrance measurements',async()=>{
  const decoy=env.document.createElement('section');decoy.innerHTML='<a class="game selected" href="#wrong">Inactive</a><div class="main library-layout"><aside class="tools"></aside></div>';
  env.document.body.prepend(decoy);const wrongFocus=watchFocus(decoy.querySelector('a'));let measurements=0;
  decoy.querySelector('.tools').getBoundingClientRect=()=>{measurements++;return new env.window.DOMRect();};
  await mount();const active=env.document.querySelector('.sheet :is(.game.selected,.book-game-link[aria-current="page"])');
  await navigate('/');
  await React.act(async()=>{for(const motion of motions.filter(item=>!item.settled&&!item.cancelled))motion.finish();});
  assert.equal(env.document.activeElement,active);assert.deepEqual(wrongFocus,[]);assert.equal(measurements,0);
});
