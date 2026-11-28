import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp, readFile, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {authoredSourcesPlugin} from './authored-sources.mjs';
import {installMountedDom} from './mounted-dom-environment.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let env, React, createRoot, api, work, root;
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(process.env.EAGLER_TEST_OUTPUT_DIR || project, '.cache/book-library-'));
  const outfile = resolve(work, 'views.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'tsx', contents: `
    export {MemoryRouter} from 'react-router';
    export {Tooltip} from 'radix-ui';
    export {BookLibrary} from './app/components/launcher/BookLibrary.tsx';
    export {LibraryAnnouncements} from './app/components/launcher/LibraryAnnouncements.tsx';
    export {BookDirectoryGames} from './app/components/launcher/BookDirectoryGames.tsx';
    export {BookThemeToggle} from './app/components/launcher/BookThemeToggle.tsx';
    export * from './app/components/launcher/book-preferences.ts';
    export {LocaleProvider} from './app/i18n.tsx';
    export {createLibraryProducts} from './app/components/launcher/products.ts';
    export {microReorderFrames} from './app/ui/micro-motion.ts';
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
test('reordering keeps a short directional nudge even for a large layout change', () => {
  const [start, end] = api.microReorderFrames(120, -160);
  const [x, y] = start.translate.split(' ').map(value => Number.parseFloat(value));
  assert.ok(Math.abs(Math.hypot(x, y) - 3) < 1e-8);
  assert.ok(x > 0 && y < 0);
  assert.equal(start.opacity, .9);
  assert.deepEqual(end, {translate:'0px 0px', opacity:1});
  assert.deepEqual(api.microReorderFrames(1, 2)[0], {translate:'1px 2px'});
});
test('multiplayer game picker and covers share selection while favorites persist and sort first', async () => {
  const products=api.createLibraryProducts(['th06mp','th07mp','th08mp'],p=>p,id=>'?game='+id);
  function Fixture(){const[filters,setFilters]=React.useState([]);return React.createElement(api.BookDirectoryGames,{products,productFilters:filters,viewingProduct:'',onProductFiltersChange:setFilters,onSelect(){assert.fail('Filtering does not open game options');}});}
  const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);
  const render=()=>React.act(async()=>root.render(React.createElement(api.LocaleProvider,{locale:'en'},React.createElement(Fixture))));
  const checked=()=>[...env.document.querySelectorAll('[data-lobby-toggle][aria-checked="true"]')].map(n=>n.dataset.lobbyToggle).sort();
  const favorite='.book-lobby-picker [data-lobby-product="th08mp"] .book-lobby-favorite';
  await render();
  const sidebar = id => `[data-lobby-rail-product="${id}"]`;
  await click(`${sidebar('th06mp')} .book-lobby-sidebar-check`);
  await click(`${sidebar('th07mp')} .book-lobby-sidebar-check`);
  assert.deepEqual(checked(),['th06mp','th07mp']);
  await click(`${sidebar('th06mp')} .book-rail-game`);
  assert.deepEqual(checked(),['th06mp']);
  await click(`${sidebar('th08mp')} .book-favorite`);
  assert.equal(node(`${sidebar('th08mp')} .book-favorite`).getAttribute('aria-pressed'),'true');
  assert.deepEqual(checked(),['th06mp'],'a favorite does not change the active filter');
  await click(`${sidebar('th08mp')} .book-favorite`);
  await click('[data-book-category="all"]');
  assert.deepEqual(checked(),[]);
  await click('[data-lobby-toggle="th06mp"]');
  await click('[data-lobby-toggle="th08mp"]');assert.deepEqual(checked(),['th06mp','th08mp']);
  assert.equal(node('[data-lobby-toggle="th06mp"]').getAttribute('aria-checked'),'true');
  await click('.book-lobby-picker-trigger');
  await click(favorite);
  assert.equal(node('#lobbyGameRail [data-lobby-icon]').dataset.lobbyIcon,'th08mp');
  assert.deepEqual(checked(),['th06mp','th08mp']);
  await click('.book-lobby-picker .book-lobby-menu-all');
  await click('[data-lobby-toggle="th08mp"]');
  await click('[data-lobby-icon="th07mp"]');assert.deepEqual(checked(),['th07mp'],'Main cover selects only one work');
  await click('[data-lobby-toggle="th08mp"]');assert.deepEqual(checked(),['th07mp','th08mp'],'Only the checkbox adds a work');
  await click('[data-lobby-icon="th08mp"]');assert.deepEqual(checked(),['th08mp']);
  await click('[data-lobby-icon="th08mp"]');assert.deepEqual(checked(),['th08mp'],'Repeating the main action stays on that work');
  await click('#lobbyGameRail .book-lobby-all');assert.deepEqual(checked(),[]);
  assert.equal(env.document.querySelectorAll('#lobbyGameRail [aria-checked="true"]').length,0);
  await React.act(async()=>root.unmount());root=createRoot(container);await render();
  assert.equal(node('#lobbyGameRail [data-lobby-icon]').dataset.lobbyIcon,'th08mp');
  await click('.book-lobby-picker-trigger');
  assert.equal(node(favorite).getAttribute('aria-checked'),'true');
});
async function mount(products, locale = 'en') {
  const activated = [], container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container);
  await React.act(async () => root.render(React.createElement(api.MemoryRouter, null, React.createElement(api.LocaleProvider, {locale},
    React.createElement(api.BookLibrary, {products, onSelect() {}, onActivate(id) {activated.push(id);}})))));
  return activated;
}

function mountAnnouncements() {
  const container = env.document.createElement('div');
  env.document.body.append(container);
  root = createRoot(container);
  return (entries, key, persistRead = true) => React.act(async () => root.render(
    React.createElement(api.LocaleProvider, {locale: 'en'},
      React.createElement(api.Tooltip.Provider, null, React.createElement(api.LibraryAnnouncements, {entries, key, persistRead}))),
  ));
}

const announcementTrigger = '.library-announcements-trigger';
const announcementDot = '.library-announcements-unread';

test('announcement icon marks the three latest localized announcements as read', async () => {
  const render = mountAnnouncements();
  const entries = [
    {id: 'old', date: '2026-11-17', 'zh-CN': {title: '旧公告'}},
    {id: 'first', date: '2026-11-20', 'zh-CN': {title: '新公告', summary: '中文正文'}, en: {title: 'New release'}},
    {id: 'third', date: '2026-11-19', 'zh-CN': {title: '次新公告'}},
    {id: 'second', date: '2026-11-20', 'zh-CN': {title: '同日公告', summary: '回退正文'}},
  ];
  await render(entries, 'first-visit');
  assert.equal(node(announcementTrigger).getAttribute('aria-haspopup'), 'dialog');
  assert.equal(node(announcementTrigger).getAttribute('aria-label'), 'New announcements');
  node(announcementDot);
  assert.equal(env.document.querySelector('.library-announcements-dialog'), null);
  await click(announcementTrigger);
  assert.equal(env.document.querySelector(announcementDot), null);
  const dialog = node('.library-announcements-dialog');
  assert.equal(env.document.getElementById(dialog.getAttribute('aria-labelledby')).textContent, 'Announcements');
  const items = [...dialog.querySelectorAll('.library-announcement')];
  assert.deepEqual(items.map(item => item.querySelector('time').dateTime), ['2026-11-20', '2026-11-20', '2026-11-19']);
  assert.deepEqual(items.map(item => item.querySelector('header h3 + time').dateTime), ['2026-11-20', '2026-11-20', '2026-11-19']);
  assert.deepEqual(items.map(item => item.querySelector('h3').textContent), ['New release', '同日公告', '次新公告']);
  assert.deepEqual(items.map(item => item.querySelector('h3').lang), ['en', 'zh-CN', 'zh-CN']);
  assert.equal(items[0].querySelector('p'), null);
  assert.equal(items[1].querySelector('header + p').textContent, '回退正文');
  await click('.library-announcements-dialog button');
  assert.equal(env.document.querySelector('.library-announcements-dialog'), null);
  await React.act(async () => new Promise(resolve => env.window.requestAnimationFrame(resolve)));
  assert.equal(env.document.activeElement, node(announcementTrigger));
  await render(entries, 'next-visit');
  assert.equal(env.document.querySelector(announcementDot), null);
  await render([...entries, {id: 'new', date: '2026-11-21', 'zh-CN': {title: '最新公告'}}], 'next-visit');
  node(announcementDot);
});

test('the announcement icon stays with no entries, and handbook demos do not write preferences', async () => {
  const render = mountAnnouncements();
  await render([], 'empty');
  node(announcementTrigger);
  assert.equal(env.document.querySelector(announcementDot), null);
  await click(announcementTrigger);
  node('.library-announcements-dialog');
  assert.equal(env.document.querySelector('.library-announcement'), null);
  await render([{id: 'demo', date: '2026-11-20', 'zh-CN': {title: '示例'}}], 'demo', false);
  node(announcementDot);
  await click(announcementTrigger);
  assert.equal(env.document.querySelector(announcementDot), null);
  assert.equal(api.readBookPreference('announcementReadIds', 'unset', Array.isArray), 'unset');
});

test('anchored multiplayer menu preserves deep-link selection and shares multi-selection with the icon rail', async () => {
  const products=api.createLibraryProducts(['th01mp','th02mp','th03mp','th04mp','th05mp','th06mp','th075mp','th09mp','th10mp','th15mp','th19mp'],p=>p,id=>'?game='+id);
  function Fixture(){const[filters,setFilters]=React.useState([]),[viewing,setViewing]=React.useState('th09mp');return React.createElement(api.BookDirectoryGames,{products,productFilters:filters,viewingProduct:viewing,onProductFiltersChange:ids=>{setFilters(ids);setViewing('');},onSelect(){assert.fail('Checkboxes must not replace a multi-selection');}});}
  const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);
  await React.act(async()=>root.render(React.createElement(api.LocaleProvider,{locale:'en'},React.createElement(Fixture))));
  assert.equal(node('[data-lobby-toggle="th09mp"]').getAttribute('aria-checked'),'true');
  for(const[id,title]of [['th06mp','红'],['th075mp','萃'],['th10mp','风'],['th15mp','绀']])assert.equal(node(`[data-lobby-icon="${id}"] .book-lobby-short`).textContent,title);
  await click('[data-lobby-toggle="th06mp"]');
  await click('.book-lobby-picker-trigger');
  assert.equal(node('.book-lobby-picker').getAttribute('role'),'menu');assert.equal(env.document.querySelector('[role="dialog"]'),null);
  const grouping=scope=>[...env.document.querySelectorAll(`${scope} [data-lobby-category]`)].map(group=>[group.dataset.lobbyCategory,[...group.querySelectorAll('[data-lobby-product]')].map(row=>row.dataset.lobbyProduct)]);
  const expectedGroups=[['versus',['th03mp','th09mp','th19mp']],['original',['th06mp','th10mp','th15mp']],['fighting',['th075mp']],['pc98',['th01mp','th02mp','th04mp','th05mp']]];
  assert.deepEqual(grouping('.book-lobby-picker'),expectedGroups);
  assert.equal(node('.book-lobby-picker [data-lobby-product="th10mp"] .book-category-game-title').textContent,'東方風神録');
  assert.equal(node('.book-lobby-picker [data-lobby-product="th10mp"] .book-category-game-subtitle').textContent,'Mountain of Faith');
  for(const id of ['th06mp','th09mp'])assert.equal(node(`.book-lobby-picker [data-lobby-product="${id}"] .book-lobby-menu-check`).getAttribute('aria-checked'),'true');
  await click('.book-lobby-picker [data-lobby-product="th10mp"] .book-lobby-menu-check');
  assert.equal(node('[data-lobby-toggle="th10mp"]').getAttribute('aria-checked'),'true');
  assert.equal(node('.book-lobby-picker-trigger').getAttribute('aria-expanded'),'true');
  await click('.book-lobby-picker [data-lobby-product="th09mp"] .book-lobby-menu-check');
  assert.equal(node('[data-lobby-toggle="th09mp"]').getAttribute('aria-checked'),'false');
  assert.equal(env.document.querySelectorAll('[data-lobby-toggle][aria-checked="true"]').length,2);
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
  await click('[data-book-category="fighting"]'); assert.deepEqual(ids(), ['th075']);
});
test('the library filters by series or game genre', async () => {
  const products = api.createLibraryProducts(['th06', 'th075', 'th09', 'hsob'], p => p, id => '?game=' + id);
  await mount(products);
  await click('[data-book-category="doujin"]');
  assert.deepEqual(ids(), ['hsob']);
  assert.match(node('[data-book-product="hsob"] .book-art img').src, /^data:image\/webp;base64,/);
  await click('[data-book-category-disclosure="doujin"]');
  const sidebar = node('#bookCategory-doujin [data-book-sidebar-product="hsob"]');
  assert.equal(sidebar.querySelector('.book-work-number').textContent, 'thHSOB');
  assert.equal(sidebar.querySelector('img').src, node('[data-book-product="hsob"] .book-art img').src);
  await click('[data-book-category="windows"]');
  assert.deepEqual(ids(), ['th06', 'th09']);
  await click('[data-book-category="versus"]');
  assert.deepEqual(ids(), ['th09']);
  await click('[data-book-category="fighting"]');
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
test('different genres can share a series and each game appears once in all games', async () => {
  const products = api.createLibraryProducts(['th03', 'th05', 'th06', 'th075', 'th09', 'th19'], p => p, id => '?game=' + id);
  await mount(products);
  assert.deepEqual(ids(), ['th03', 'th05', 'th06', 'th075', 'th09', 'th19']);
  for (const [category, expected] of [
    ['versus', ['th03', 'th09', 'th19']], ['pc98', ['th03', 'th05']],
    ['fighting', ['th075']], ['spin-off', ['th075']], ['windows', ['th06', 'th09', 'th19']],
  ]) {
    await click(`[data-book-category="${category}"]`);
    assert.deepEqual(ids(), expected);
    const control = `[data-book-category-disclosure="${category}"]`;
    if (node(control).getAttribute('aria-expanded') !== 'true') await click(control);
    assert.deepEqual([...node(`#bookCategory-${category}`).querySelectorAll('[data-book-sidebar-product]')]
      .map(link => link.dataset.bookSidebarProduct), expected);
    assert.equal(node(`[data-book-category="${category}"] .book-category-count`).textContent, String(expected.length));
  }
});
test('sidebar disclosures list playable single-player entries without changing the category filter', async () => {
  const products = api.createLibraryProducts(['th13', 'th06mp', 'th01', 'th06', 'th07'], p => p, id => '?game=' + id);
  products.find(product => product.id === 'th07').hidden = true;
  const activated = await mount(products);
  const disclosure = id => `[data-book-category-disclosure="${id}"]`;
  const members = id => [...node(`#bookCategory-${id}`).querySelectorAll('[data-book-sidebar-product]')]
    .map(link => link.dataset.bookSidebarProduct);
  assert.equal(env.document.querySelector('[data-book-category-group="all"] .book-category-disclosure, #bookCategory-all'), null);
  assert.equal(env.document.querySelector(disclosure('favorites')), null);
  const windows = () => node('#bookCategory-windows');
  assert.equal(node(disclosure('windows')).getAttribute('aria-controls'), windows().id);
  assert.ok(windows().classList.contains('ui-disclosure-panel'));
  assert.equal(node(disclosure('windows')).getAttribute('aria-expanded'), 'true');
  assert.equal(windows().hasAttribute('inert'), false);
  assert.ok(node(`${disclosure('windows')} svg[data-expanded="true"]`));
  assert.deepEqual(members('windows'), ['th06', 'th13']);
  assert.deepEqual(ids(), ['th01', 'th06', 'th13']);
  assert.equal(env.document.querySelector('[data-book-sidebar-favorite]'), null);
  await click('[data-book-favorite="th13"]');
  assert.equal(node('[data-book-favorite="th13"]').getAttribute('aria-pressed'), 'true');
  await click('[data-book-favorite="th13"]');
  assert.equal(node('[data-book-favorite="th13"]').getAttribute('aria-pressed'), 'false');
  await click(disclosure('pc98'));
  assert.deepEqual(members('pc98'), ['th01']);
  assert.equal(node('#bookCategory-pc98 .book-work-number').textContent, 'th01');
  await click('[data-book-category="pc98"]');
  assert.deepEqual(ids(), ['th01']);
  assert.deepEqual(members('windows'), ['th06', 'th13']);
  await click('#bookCategory-windows [data-book-sidebar-product="th13"]');
  assert.deepEqual(activated, ['th13']);
  await click(disclosure('windows'));
  assert.equal(windows().getAttribute('aria-hidden'), 'true');
  assert.equal(windows().hasAttribute('inert'), true);
  assert.deepEqual(members('windows'), ['th06', 'th13'], 'entries stay mounted for the closing animation');
  assert.deepEqual(ids(), ['th01']);
  await click('[data-book-category="all"]');
  assert.deepEqual(ids(), ['th01', 'th06', 'th13']);
  assert.equal(node('[data-book-category="all"]').getAttribute('aria-pressed'), 'true');
  await React.act(async () => root.unmount());
  await mount(products);
  assert.equal(node(disclosure('windows')).getAttribute('aria-expanded'), 'false');
  assert.equal(node(disclosure('pc98')).getAttribute('aria-expanded'), 'true');
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

test('default single-player list starts at TH01 and keeps favorite priority', async () => {
  const ascending = ['th01', 'th02', 'th03', 'th04', 'th05', 'th06', 'th07'];
  const products = api.createLibraryProducts([...ascending].reverse(), p => p, id => '?game=' + id);
  await mount(products);
  assert.deepEqual(ids(), ascending);
  const order = (favorites, sort = 'number') => api.filterBookProducts(products, 'all', favorites, '', sort).map(p => p.id);
  assert.deepEqual(order(['th07', 'th01']), ['th01', 'th07', 'th02', 'th03', 'th04', 'th05', 'th06']);
  assert.deepEqual(order([], 'reverse'), [...ascending].reverse());
});

test('TH13, TH14 and TH17 single-player cards use the bundled disc covers', async () => {
  const games = ['th13', 'th14', 'th17'];
  const coverDir = resolve(project, 'app/components/launcher/book-covers');
  const sources = JSON.parse(await readFile(resolve(coverDir, 'sources.json'), 'utf8'));
  const sha256 = bytes => createHash('sha256').update(bytes).digest('hex');
  await mount(api.createLibraryProducts(games, p => p, id => '?game=' + id));
  for (const game of games) {
    const source = sources.find(entry => entry.game === game);
    const image = node(`[data-book-product="${game}"] .book-art img`);
    assert.match(image.src, /^data:image\/webp;base64,/);
    assert.equal(sha256(Buffer.from(image.src.split(',')[1], 'base64')), source.generated_sha256);
    assert.equal(sha256(await readFile(resolve(project, source.source_file))), source.source_sha256);
  }
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
  const option = [...env.document.querySelectorAll('[role="option"]')].find(element=>element.textContent==='Number (descending)');
  assert.ok(option,'the menu exposes actual option semantics');
  await React.act(async () => option.click());
  assert.deepEqual(ids(),['th10','th07','th06']);
  assert.ok(trigger.textContent.includes('Number (descending)'));
});
