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
const click = selector => React.act(async () => node(selector).click());
const ids = () => [...env.document.querySelectorAll('.book-grid [data-book-product]')].map(node => node.dataset.bookProduct);
test('multiplayer filters allow several games, all resets them, and favorites persist and sort first', async () => {
  const products = api.createLibraryProducts(['th06mp', 'th07mp', 'th08mp'], p => p, id => '?game=' + id);
  const viewed = [];
  function Fixture() {
    const [filters, setFilters] = React.useState([]);
    const [viewing, setViewing] = React.useState('');
    return React.createElement(api.BookDirectoryGames, {products, productFilters: filters, viewingProduct: viewing,
    onProductFiltersChange: ids => {setFilters(ids); setViewing('');}, onSelect: id => {if (id) viewed.push(id); setViewing(id); if (!id) setFilters([]);}});
  }
  const container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container);
  const render = () => React.act(async () => root.render(React.createElement(api.LocaleProvider, {locale: 'en'}, React.createElement(Fixture))));
  await render();
  await click('[data-lobby-product="th06mp"] [role="checkbox"]'); await click('[data-lobby-product="th08mp"] [role="checkbox"]');
  assert.deepEqual([...env.document.querySelectorAll('[role=checkbox][data-state=checked]')].map(n => n.closest('[data-lobby-product]').dataset.lobbyProduct), ['th06mp', 'th08mp']);
  await click('[data-lobby-product="th08mp"] .book-lobby-favorite');
  assert.equal(node('[data-lobby-product]').dataset.lobbyProduct, 'th08mp');
  assert.equal(env.document.querySelectorAll('[role=checkbox][data-state=checked]').length, 2);
  await click('.book-lobby-all'); assert.equal(env.document.querySelectorAll('[role=checkbox][data-state=checked]').length, 0);
  await click('[data-lobby-product="th07mp"] .book-lobby-game');
  assert.deepEqual([...env.document.querySelectorAll('[role=checkbox][data-state=checked]')].map(n => n.closest('[data-lobby-product]').dataset.lobbyProduct), []);
  assert.deepEqual(viewed, ['th07mp']);
  assert.equal(node('[data-lobby-product="th07mp"]').dataset.viewing, 'true');
  await click('[data-lobby-product="th07mp"] .book-lobby-game'); assert.deepEqual(viewed, ['th07mp', 'th07mp']);
  assert.deepEqual([...env.document.querySelectorAll('[role=checkbox][data-state=checked]')].map(n => n.closest('[data-lobby-product]').dataset.lobbyProduct), [], 'viewing does not change filters');
  await click('[data-lobby-product="th06mp"] [role="checkbox"]'); await click('[data-lobby-product="th08mp"] [role="checkbox"]');
  assert.deepEqual([...env.document.querySelectorAll('[role=checkbox][data-state=checked]')].map(n => n.closest('[data-lobby-product]').dataset.lobbyProduct), ['th08mp', 'th06mp'], 'only checkboxes control multi-select');
  assert.equal(node('[data-lobby-product="th07mp"] .book-lobby-game').getAttribute('aria-pressed'), 'false', 'checkbox filtering clears the view-only selection');
  assert.equal(env.document.querySelector('[data-lobby-product] .book-lobby-settings'), null, 'the directory has no game settings button');
  await React.act(async () => root.unmount()); root = createRoot(container); await render();
  assert.equal(node('[data-lobby-product]').dataset.lobbyProduct, 'th08mp');
  assert.equal(node('[data-lobby-product="th08mp"] .book-lobby-favorite').getAttribute('aria-pressed'), 'true');
});
async function mount(products, locale = 'en') {
  const activated = [], container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container);
  await React.act(async () => root.render(React.createElement(api.MemoryRouter, null, React.createElement(api.LocaleProvider, {locale},
    React.createElement(api.BookLibrary, {products, onSelect() {}, onActivate(id) {activated.push(id);}})))));
  return activated;
}
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

test('appearance defaults to dark, persists light, and restores it on remount', async () => {
  const container = env.document.createElement('div'); env.document.body.append(container);
  const render = () => React.createElement(api.LocaleProvider, {locale:'en'}, React.createElement(api.BookThemeToggle));
  root = createRoot(container);
  await React.act(async () => root.render(render()));
  assert.equal(env.document.documentElement.dataset.bookTheme, 'dark');
  await click('.book-theme-toggle');
  assert.equal(env.document.documentElement.dataset.bookTheme, 'light');
  await React.act(async () => root.unmount()); root = createRoot(container);
  await React.act(async () => root.render(render()));
  assert.equal(env.document.documentElement.dataset.bookTheme, 'light');
  assert.equal(node('.book-theme-toggle').getAttribute('aria-pressed'), 'false');
  await click('.book-theme-toggle');
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
