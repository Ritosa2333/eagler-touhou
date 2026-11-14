import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {authoredSourcesPlugin} from './authored-sources.mjs';
import {installMountedDom} from './mounted-dom-environment.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let env, React, createRoot, api, work, root;
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(project, '.cache/book-library-'));
  const outfile = resolve(work, 'views.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'tsx', contents: `
    export {MemoryRouter} from 'react-router';
    export {BookLibrary} from './app/components/launcher/BookLibrary.tsx';
    export {BookDirectoryGames} from './app/components/launcher/BookDirectoryGames.tsx';
    export {BookThemeToggle} from './app/components/launcher/BookThemeToggle.tsx';
    export * from './app/components/launcher/book-preferences.ts';
    export {LocaleProvider} from './app/i18n.tsx';
    export {createLibraryProducts} from './app/components/launcher/products.ts';
  `}, outfile, bundle: true, packages: 'external', platform: 'node', format: 'esm', jsx: 'automatic', logLevel: 'silent', plugins: [authoredSourcesPlugin(project)]});
  api = await import(pathToFileURL(outfile).href);
});
afterEach(async () => {
  if (root) await React.act(async () => root.unmount()); root = null;
  env.document.body.replaceChildren(); env.window.localStorage.clear();
  assert.deepEqual(env.errors.splice(0), []);
});
after(async () => {env.close(); await rm(work, {recursive: true, force: true});});
const node = selector => {const value = env.document.querySelector(selector); assert.ok(value, selector); return value;};
const click = selector => React.act(async () => {
  const target=node(selector);
  if(target.getAttribute('aria-haspopup')==='menu'){target.focus();target.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true,cancelable:true}));}
  else target.click();
  await new Promise(resolve=>setTimeout(resolve,0));
});
const ids = () => [...env.document.querySelectorAll('.book-grid [data-book-product]')].map(node => node.dataset.bookProduct);
test('multiplayer icon and desktop checkboxes share selection while favorites persist and sort first', async () => {
  const products=api.createLibraryProducts(['th06mp','th07mp','th08mp'],p=>p,id=>'?game='+id);
  function Fixture(){const[filters,setFilters]=React.useState([]);return React.createElement(api.BookDirectoryGames,{products,productFilters:filters,viewingProduct:'',onProductFiltersChange:setFilters,onSelect(){assert.fail('Filtering does not open game options');}});}
  const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);
  const render=()=>React.act(async()=>root.render(React.createElement(api.LocaleProvider,{locale:'en'},React.createElement(Fixture))));
  const checked=()=>[...env.document.querySelectorAll('.book-lobby-desktop [data-lobby-product] [role="checkbox"][aria-checked="true"]')].map(n=>n.closest('[data-lobby-product]').dataset.lobbyProduct).sort();
  await render();
  await click('.book-lobby-desktop [data-lobby-product="th06mp"] [role="checkbox"]');
  await click('[data-lobby-toggle="th08mp"]');assert.deepEqual(checked(),['th06mp','th08mp']);
  assert.equal(node('[data-lobby-toggle="th06mp"]').getAttribute('aria-checked'),'true');
  await click('.book-lobby-desktop [data-lobby-product="th08mp"] .book-lobby-favorite');
  assert.equal(node('.book-lobby-desktop [data-lobby-product]').dataset.lobbyProduct,'th08mp');
  assert.deepEqual(checked(),['th06mp','th08mp']);
  await click('[data-lobby-icon="th07mp"]');assert.deepEqual(checked(),['th07mp'],'Main cover selects only one work');
  await click('[data-lobby-toggle="th08mp"]');assert.deepEqual(checked(),['th07mp','th08mp'],'Only the checkbox adds a work');
  await click('.book-lobby-desktop [data-lobby-product="th08mp"] .book-lobby-game');assert.deepEqual(checked(),['th08mp']);
  await click('.book-lobby-desktop [data-lobby-product="th08mp"] .book-lobby-game');assert.deepEqual(checked(),['th08mp'],'Repeating the main action stays on that work');
  await click('.book-lobby-desktop .book-lobby-all');assert.deepEqual(checked(),[]);
  assert.equal(env.document.querySelectorAll('#lobbyGameRail [aria-checked="true"]').length,0);
  await React.act(async()=>root.unmount());root=createRoot(container);await render();
  assert.equal(node('.book-lobby-desktop [data-lobby-product]').dataset.lobbyProduct,'th08mp');
  assert.equal(node('.book-lobby-desktop [data-lobby-product="th08mp"] .book-lobby-favorite').getAttribute('aria-pressed'),'true');
});
async function mount(products, locale = 'en') {
  const activated = [], container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container);
  await React.act(async () => root.render(React.createElement(api.MemoryRouter, null, React.createElement(api.LocaleProvider, {locale},
    React.createElement(api.BookLibrary, {products, onSelect() {}, onActivate(id) {activated.push(id);}})))));
  return activated;
}

test('anchored multiplayer menu preserves deep-link selection and shares multi-selection with the icon rail', async () => {
  const products=api.createLibraryProducts(['th01mp','th03mp','th06mp','th075mp','th09mp','th10mp','th15mp'],p=>p,id=>'?game='+id);
  function Fixture(){const[filters,setFilters]=React.useState([]),[viewing,setViewing]=React.useState('th09mp');return React.createElement(api.BookDirectoryGames,{products,productFilters:filters,viewingProduct:viewing,onProductFiltersChange:ids=>{setFilters(ids);setViewing('');},onSelect(){assert.fail('Checkboxes must not replace a multi-selection');}});}
  const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);
  await React.act(async()=>root.render(React.createElement(api.LocaleProvider,{locale:'en'},React.createElement(Fixture))));
  assert.equal(node('[data-lobby-toggle="th09mp"]').getAttribute('aria-checked'),'true');
  for(const[id,title]of [['th06mp','红'],['th075mp','萃'],['th10mp','风'],['th15mp','绀']])assert.equal(node(`[data-lobby-icon="${id}"] .book-lobby-short`).textContent,title);
  await click('[data-lobby-toggle="th06mp"]');
  await click('.book-lobby-picker-trigger');
  assert.equal(node('.book-lobby-picker').getAttribute('role'),'menu');assert.equal(env.document.querySelector('[role="dialog"]'),null);
  const grouping=scope=>[...env.document.querySelectorAll(`${scope} [data-lobby-category]`)].map(group=>[group.dataset.lobbyCategory,[...group.querySelectorAll('[data-lobby-product]')].map(row=>row.dataset.lobbyProduct)]);
  const expectedGroups=[['versus',['th09mp']],['original',['th06mp','th10mp','th15mp']],['fighting',['th075mp']],['pc98',['th01mp','th03mp']]];
  assert.deepEqual(grouping('.book-lobby-desktop'),expectedGroups);
  assert.deepEqual(grouping('.book-lobby-picker'),expectedGroups,'Both presentations consume the same catalog classification');
  assert.equal(node('.book-lobby-picker [data-lobby-product="th10mp"] strong').textContent,'東方風神録');
  assert.equal(node('.book-lobby-picker [data-lobby-product="th10mp"] .book-lobby-subtitle').textContent,'Mountain of Faith');
  for(const id of ['th06mp','th09mp'])assert.equal(node(`.book-lobby-picker [data-lobby-product="${id}"] .book-lobby-menu-check`).getAttribute('aria-checked'),'true');
  await click('.book-lobby-picker [data-lobby-product="th10mp"] .book-lobby-menu-check');
  assert.equal(node('[data-lobby-toggle="th10mp"]').getAttribute('aria-checked'),'true');
  assert.equal(node('.book-lobby-picker-trigger').getAttribute('aria-expanded'),'true');
  await click('.book-lobby-picker [data-lobby-product="th09mp"] .book-lobby-menu-check');
  assert.equal(node('[data-lobby-toggle="th09mp"]').getAttribute('aria-checked'),'false');
  assert.equal(env.document.querySelectorAll('.book-lobby-desktop [data-lobby-product] [role="checkbox"][aria-checked="true"]').length,2);
  await click('.book-lobby-picker [data-lobby-product="th15mp"] .book-lobby-menu-choice');
  assert.equal(node('.book-lobby-picker-trigger').getAttribute('aria-expanded'),'false');
  assert.deepEqual([...env.document.querySelectorAll('#lobbyGameRail [aria-checked="true"]')].map(n=>n.dataset.lobbyToggle),['th15mp']);
  await React.act(async()=>new Promise(resolve=>env.window.requestAnimationFrame(()=>env.window.requestAnimationFrame(resolve))));
  await click('.book-lobby-picker-trigger');
  await click('.book-lobby-picker [data-lobby-product="th06mp"] .book-lobby-menu-check');
  assert.deepEqual([...env.document.querySelectorAll('#lobbyGameRail [aria-checked="true"]')].map(n=>n.dataset.lobbyToggle),['th06mp','th15mp']);
  await click('.book-lobby-picker [data-lobby-product="th15mp"] .book-lobby-menu-check');
  assert.deepEqual([...env.document.querySelectorAll('#lobbyGameRail [aria-checked="true"]')].map(n=>n.dataset.lobbyToggle),['th06mp']);
  await click('.book-lobby-picker .book-lobby-menu-all');
  assert.equal(env.document.querySelectorAll('#lobbyGameRail [aria-checked="true"]').length,0);
  assert.equal(node('#lobbyGameRail .book-lobby-all').getAttribute('aria-pressed'),'true');
  await React.act(async()=>{env.document.activeElement.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'Escape',bubbles:true,cancelable:true}));await new Promise(resolve=>setTimeout(resolve,0));});
  assert.equal(env.document.querySelector('.book-lobby-picker'),null);
});
test('only supplied published products become cards; hidden/MP/demo titles never appear; doujin stays empty', async () => {
  const products = api.createLibraryProducts(['th06', 'th07', 'th075', 'th06mp', 'th20'], p => p, id => '?game=' + id);
  products.find(p => p.id === 'th20').hidden = true;
  const activated = await mount(products);
  assert.deepEqual(ids(), ['th06', 'th07', 'th075']);
  await click('[data-book-product="th06"]'); assert.deepEqual(activated, ['th06']);
  await click('[data-book-category="doujin"]'); assert.deepEqual(ids(), []);
  assert.ok(node('.book-empty').textContent.includes('No matching games'));
  assert.equal(env.document.querySelector('[data-book-product="dj01"]'), null);
  await click('[data-book-category="spin-off"]'); assert.deepEqual(ids(), ['th075']);
});
test('HSoB is listed with doujin games and not with official spin-offs', async () => {
  const products = api.createLibraryProducts(['th075'], p => p, id => '?game=' + id);
  products.push({...products[0], id: 'hsob', gameId: 'hsob', number: 'HSOB', title: 'Hollow Song of Birds'});
  await mount(products);
  await click('[data-book-category="doujin"]');
  assert.deepEqual(ids(), ['hsob']);
  await click('[data-book-category="spin-off"]');
  assert.deepEqual(ids(), ['th075']);
});
test('TH05 is discoverable through the PC98 category in the current library', async () => {
  const products = api.createLibraryProducts(['th05', 'th06'], p => p, id => '?game=' + id);
  await mount(products);
  assert.deepEqual(ids(), ['th05', 'th06']);
  await click('[data-book-category="pc98"]');
  assert.deepEqual(ids(), ['th05']);
  await click('[data-book-category="windows"]');
  assert.deepEqual(ids(), ['th06']);
});
test('favorites and view persist through remount, retain empty state, and recent list records actual launches only', async () => {
  const products = api.createLibraryProducts(['th06', 'th07'], p => p, id => '?game=' + id);
  await mount(products, 'zh-CN');
  assert.equal(env.document.querySelector('.book-recent-link'), null);
  await click('[data-book-favorite="th07"]'); await click('[data-book-category="favorites"]');
  assert.deepEqual(ids(), ['th07']);
  await click('[data-book-view="list"]'); assert.ok(node('.book-view-list'));
  await React.act(async () => {api.recordBookLaunch('th07'); api.recordBookLaunch('th06'); api.recordBookLaunch('th07');});
  assert.deepEqual([...env.document.querySelectorAll('.book-recent-link')].map(n => n.dataset.bookProduct), ['th07', 'th06']);
  await React.act(async () => root.unmount()); root = null;
  env.document.body.replaceChildren(); await mount(products);
  assert.ok(node('.book-view-list')); await click('[data-book-category="favorites"]'); assert.deepEqual(ids(), ['th07']);
  await click('[data-book-favorite="th07"]'); assert.deepEqual(ids(), []);
});
test('unavailable history IDs are ignored, search is case insensitive, numeric sorting handles decimal titles', async () => {
  env.window.localStorage.setItem('eagler.book.recent', JSON.stringify(['dj01', 'th105', 'th07']));
  const products = api.createLibraryProducts(['th06', 'th07', 'th075', 'th10'], p => p, id => '?game=' + id);
  await mount(products);
  assert.deepEqual([...env.document.querySelectorAll('.book-recent-link')].map(n => n.dataset.bookProduct), ['th07']);
  assert.deepEqual(api.filterBookProducts(products, 'all', [], 'CHERRY', 'number').map(p => p.id), ['th07']);
  assert.deepEqual(api.filterBookProducts(products, 'all', [], '', 'reverse').map(p => p.id), ['th10', 'th075', 'th07', 'th06']);
  env.window.localStorage.setItem('eagler.book.favorites', '{"bad":true}');
  assert.deepEqual(api.readBookPreference('favorites', [], api.stringArray), []);
});

test('number sorting places numeric works including decimals before lettered fan works', () => {
  const [base] = api.createLibraryProducts(['th06'], p => p, id => '?game=' + id);
  const products = [
    {...base, id:'thalpha', number:'alpha', title:'A'},
    {...base, id:'th10', number:'10', title:'B'},
    {...base, id:'th075', number:'7.5', title:'C'},
    {...base, id:'th06', number:'6', title:'D'},
    {...base, id:'thbeta', number:'beta', title:'E'},
  ];
  assert.deepEqual(api.filterBookProducts(products, 'all', [], '', 'number').map(p=>p.id), ['th06','th075','th10','thalpha','thbeta']);
  assert.deepEqual(api.filterBookProducts(products, 'all', [], '', 'reverse').map(p=>p.id), ['th10','th075','th06','thalpha','thbeta']);
  assert.deepEqual(api.filterBookProducts(products, 'doujin', [], '', 'number').map(p=>p.id), ['thalpha','thbeta']);
  assert.deepEqual(api.filterBookProducts(products, 'all', [], '', 'name').map(p=>p.id), ['thalpha','th10','th075','th06','thbeta']);
});

test('appearance defaults to dark, persists light, and restores it on remount', async () => {
  const container = env.document.createElement('div'); env.document.body.append(container);
  const render = () => React.createElement(api.LocaleProvider, {locale:'en'}, React.createElement(api.BookThemeToggle));
  root = createRoot(container);
  await React.act(async () => root.render(render()));
  assert.equal(env.document.documentElement.dataset.bookTheme, 'dark');
  await click('.book-theme-toggle');
  await click('.custom-theme-switch [data-theme-value="light"]');
  assert.equal(env.document.documentElement.dataset.bookTheme, 'light');
  await React.act(async () => root.unmount()); root = createRoot(container);
  await React.act(async () => root.render(render()));
  assert.equal(env.document.documentElement.dataset.bookTheme, 'light');
  await click('.book-theme-toggle');
  await click('.custom-theme-switch [data-theme-value="dark"]');
  assert.equal(env.document.documentElement.dataset.bookTheme, 'dark');
});

test('search filters the rendered collection and clear restores input focus', async () => {
  const products = api.createLibraryProducts(['th06', 'th07', 'th10'], p => p, id => '?game=' + id);
  await mount(products);
  const input = node('[aria-label="Search games"]');
  await React.act(async () => {
    Object.getOwnPropertyDescriptor(env.window.HTMLInputElement.prototype, 'value').set.call(input, 'scarlet');
    input.dispatchEvent(new env.window.Event('input', {bubbles:true}));
  });
  assert.deepEqual(ids(), ['th06']);
  await click('[aria-label="Clear search"]');
  assert.deepEqual(ids(), ['th06','th07','th10']);
  assert.equal(env.document.activeElement,input);
});

test('the selected category cannot be cleared and empty favorites recover through the shared action', async () => {
  const products = api.createLibraryProducts(['th06','th07'], p => p, id => '?game=' + id);
  await mount(products);
  await click('[data-book-category="favorites"]');
  assert.deepEqual(ids(),[]);
  assert.ok(node('.book-empty').textContent.includes('No favorites yet'));
  await click('[data-book-category="favorites"]');
  assert.deepEqual(ids(),[],'a selected category keeps its selection when pressed again');
  await click('.book-empty button');
  assert.deepEqual(ids(),['th06','th07']);
});

test('sort is keyboard reachable and selection reorders the rendered cards', async () => {
  const products = api.createLibraryProducts(['th06','th07','th10'], p => p, id => '?game=' + id);
  await mount(products);
  const trigger = node('[aria-label="Sort games"]');
  await React.act(async () => trigger.dispatchEvent(new env.window.KeyboardEvent('keydown',{key:'ArrowDown',bubbles:true,cancelable:true})));
  const option = [...env.document.querySelectorAll('[role="option"]')].find(element=>element.textContent==='Number, descending');
  assert.ok(option,'the menu exposes actual option semantics');
  await React.act(async () => option.click());
  assert.deepEqual(ids(),['th10','th07','th06']);
  assert.ok(trigger.textContent.includes('Number, descending'));
});
