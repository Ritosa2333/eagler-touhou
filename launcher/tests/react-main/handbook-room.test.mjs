import {test, vi, beforeAll, afterAll, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

let env, React, createRoot, RoomExample, work, root, handbook, moveHistory;
const act = async fn => React.act(async () => {await fn?.(); for (let i = 0; i < 12; i++) await Promise.resolve();});
const n = selector => {const node = env.document.querySelector(selector); assert.ok(node, selector); return node;};
const button = text => [...env.document.querySelectorAll('button')].find(node => node.textContent === text);
beforeAll(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve('.cache/handbook-room-'));
  // Canvas pixels belong to the browser gate; mounted tests inspect ownership and navigation.
  const context=new Proxy({measureText:text=>({width:String(text).length*7})},{get:(target,key)=>target[key]??(()=>{})});
  env.window.HTMLCanvasElement.prototype.getContext=()=>context;
  Object.defineProperty(env.document,'fonts',{configurable:true,value:{ready:Promise.resolve()}});
  await build({absWorkingDir: process.cwd(), stdin:{resolveDir:process.cwd(),loader:'ts',contents:`
    export {RoomExample} from './app/ui/room-example.tsx';
    export {PageInventory} from './app/ui/page-inventory.tsx';
    export {catalogPages,catalogAnchor,catalogTarget} from './app/ui/page-catalog.ts';
    export {LocaleProvider} from './app/i18n.tsx';
    export {MemoryRouter,useNavigate} from 'react-router';
  `}, bundle: true, platform: 'node',
    format: 'esm', jsx: 'automatic', outfile: resolve(work, 'demo.mjs'), packages: 'external', loader: {'.css': 'empty'},
    plugins: [authoredSourcesPlugin(process.cwd())], logLevel: 'silent'});
  handbook=await import(pathToFileURL(resolve(work,'demo.mjs')));
  ({RoomExample}=handbook);
});
afterEach(async () => {if (root) await act(() => root.unmount()); root = null; env.document.body.replaceChildren(); assert.deepEqual(env.errors.splice(0), []);});
afterAll(async () => {env.close(); await rm(work, {recursive: true, force: true});});
async function mount(announce = () => {}) {const container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container); await act(() => root.render(React.createElement(RoomExample, {announce})));}
async function open(selector) {
  await act(() => {const trigger = n(selector); trigger.focus(); trigger.dispatchEvent(new env.window.KeyboardEvent('keydown', {key: 'ArrowDown', bubbles: true, cancelable: true}));});
  await vi.waitFor(() => assert.ok(env.document.querySelector('[role=menu]')));
}
async function choose(selector) {
  await act(() => n(selector).click());
  await act(() => new Promise(resolve => setTimeout(resolve, 0)));
}

test('room handbook uses production seat states and local member transfer actions', async () => {
  const announce = vi.fn();
  await mount(announce);
  assert.deepEqual([...env.document.querySelectorAll('[data-card-state]')].map(node => node.dataset.cardState), ['unready', 'loading', 'ready']);
  const player = n('[data-mp-seat="1"]');
  await act(() => button('已准备').click()); assert.equal(n('[data-mp-seat="1"]'), player); assert.equal(player.dataset.cardState, 'ready');
  await open('[aria-label="管理 P2"]');
  await choose('[data-phrase="move-spectator"]');
  assert.equal(player.dataset.cardState, 'empty');
  assert.ok(n('#page-room-spectators'));
  assert.equal(env.document.querySelectorAll('.mp-spectator-entry').length, 2);
  await act(() => button('重置房间示例').click()); assert.equal(n('[data-mp-seat="1"]').dataset.cardState, 'loading');
  assert.equal(announce.mock.lastCall[0], '');
});

test('quick chat posts to the local log and keeps a real SVG menu trigger', async () => {
  await mount();
  const trigger = n('.room-quick-chat-trigger'); assert.ok(trigger.querySelector('svg.lucide-chevron-up'));
  await open('.room-quick-chat-trigger');
  await choose('[role=menuitem]');
  assert.equal(n('.mp-room-chat-log').children.length, 1);
  await act(() => button('重置房间示例').click());
  assert.equal(n('.mp-room-chat-log').children.length, 0);
});

test('seat actions open the personal inspector and locate the selected network peer', async () => {
  await mount();
  await act(() => n('button[aria-label="我的角色"]').click());
  assert.equal(n('#mpRoomPanel').dataset.panel, 'personal');
  assert.equal(n('#mpDisplayName').value, '演示房主');
  await act(() => n('#mpRoomPanelClose').click());
  await act(() => n('[data-network-peer="handbook-player"]').click());
  assert.equal(n('#mpRoomPanel').open,false);
  assert.equal(n('#page-room-network').querySelectorAll('.mp-network-peer').length,1);
  await act(()=>button('查看所有玩家').click());
  assert.equal(n('#page-room-network').querySelectorAll('.mp-network-peer').length,2);
});

test('room examples share role permissions, phase and readiness across all seven regions',async()=>{
  await mount();
  const card=n('[data-mp-seat="1"]');
  assert.equal(n('[aria-label="管理 P2 样例"]').disabled,false);
  await act(()=>button('普通成员').click());
  assert.equal(n('[aria-label="管理 P2 样例"]').disabled,true);
  assert.equal(n('#mpRoomVisibility').disabled,true);
  await act(()=>button('观战').click());
  assert.equal(n('#page-room-session .mp-ready-button').disabled,true);
  assert.equal(n('.room-quick-chat-trigger').disabled,false);
  await act(()=>button('房主').click());
  await act(()=>button('已准备').click());
  await act(()=>button('准备').click());
  assert.equal(n('.mp-start-button').disabled,false);
  await act(()=>button('开始游戏样例').click());
  assert.equal(n('#mpRoomVisibility').disabled,true);
  assert.equal(n('[aria-label="管理 P2 样例"]').disabled,true);
  await act(()=>button('返回等待样例').click());
  assert.equal(n('#mpRoomVisibility').disabled,false);
  await act(()=>button('2P').click());
  assert.equal(n('[data-mp-seat="2"]').hidden,true);
  await act(()=>button('3P').click());
  assert.equal(n('[data-mp-seat="2"]').hidden,false);
  assert.equal(n('[data-mp-seat="1"]'),card);
  const ids=[...env.document.querySelectorAll('[id]')].map(node=>node.id);
  assert.equal(new Set(ids).size,ids.length,'production specimens must not duplicate IDs');
  assert.equal(env.window.localStorage.length,0);
  assert.equal(env.document.querySelectorAll('iframe').length,0);
});

function HistoryProbe(){moveHistory=handbook.useNavigate();return null;}
async function mountInventory(hash){
  const container=env.document.createElement('div');env.document.body.append(container);root=createRoot(container);
  await act(()=>root.render(React.createElement(handbook.MemoryRouter,{initialEntries:['/ui-review.html'+hash]},
    React.createElement(handbook.LocaleProvider,{locale:'zh-CN'},React.createElement(handbook.PageInventory),React.createElement(HistoryProbe)))));
  await act(()=>new Promise(resolve=>setTimeout(resolve,40)));
}
async function navigateHash(hash){await act(()=>moveHistory(hash));await act(()=>new Promise(resolve=>setTimeout(resolve,40)));}

test('every page exposes unique region, component and state anchors with an appropriate sample or workspace entry',async()=>{
  await mountInventory('#page-library');
  const ids=new Set();
  for(const page of handbook.catalogPages){
    await navigateHash('#'+handbook.catalogAnchor(page.id));
    assert.ok(n('#page-'+page.id).open,page.name);
    for(const region of page.regions){
      for(const id of [handbook.catalogAnchor(page.id,region.id),'example-'+page.id+'-'+region.id]){
        assert.ok(!ids.has(id),id);ids.add(id);assert.ok(env.document.getElementById(id),id);
        assert.equal(handbook.catalogTarget('#'+id)?.page.id,page.id);
      }
      for(const part of region.parts){
        const id=handbook.catalogAnchor(page.id,region.id,part.id);
        assert.ok(!ids.has(id),id);ids.add(id);assert.ok(env.document.getElementById(id),id);
        assert.ok(part.states.length,part.name);
        part.states.forEach((state,index)=>{
          const target=handbook.catalogAnchor(page.id,region.id,part.id,index);
          assert.ok(env.document.getElementById(target),state);
          assert.equal(handbook.catalogTarget('#'+target)?.part.id,part.id);
        });
      }
      assert.ok(n('#example-'+page.id+'-'+region.id).children.length,region.name);
    }
  }
  assert.equal(env.window.localStorage.length,0,'inspection must not write launcher preferences');
  assert.equal(env.document.querySelectorAll('iframe').length,0);
  assert.equal(handbook.catalogTarget('#unknown'),null);
  assert.equal(handbook.catalogTarget('#%ZZ'),null);
});

test('deep links reveal closed pages and Back/Forward restores the corresponding component',async()=>{
  const calls=[];
  const scroll=vi.spyOn(env.window.HTMLElement.prototype,'scrollIntoView').mockImplementation(function(){calls.push(this.id);});
  try{
    await mountInventory('#page-room-management-menu-state-1');
    assert.ok(n('#page-room').open);
    assert.equal(calls.at(-1),'page-room-management-menu-state-1');
    await navigateHash('#page-account-form-credentials');
    assert.ok(n('#page-account').open);
    assert.equal(calls.at(-1),'page-account-form-credentials');
    await act(()=>moveHistory(-1));await act(()=>new Promise(resolve=>setTimeout(resolve,40)));
    assert.ok(n('#page-room').open);
    assert.equal(calls.at(-1),'page-room-management-menu-state-1');
    await act(()=>moveHistory(1));await act(()=>new Promise(resolve=>setTimeout(resolve,40)));
    assert.ok(n('#page-account').open);
    assert.equal(calls.at(-1),'page-account-form-credentials');
    await navigateHash('#room-components');
    assert.ok(n('#page-room').open);
    assert.equal(calls.at(-1),'page-room');
  }finally{scroll.mockRestore();}
});

test('page links reopen a collapsed target and reveal pages hidden by a category filter',async()=>{
  await mountInventory('#page-room');
  await act(()=>n('#page-room > summary').click());
  assert.equal(n('#page-room').open,false);
  await act(()=>n('.catalog-page-index a[href$="#page-room"]').click());
  await act(()=>new Promise(resolve=>setTimeout(resolve,40)));
  assert.equal(n('#page-room').open,true);
  await act(()=>button('支持').click());
  assert.equal(env.document.querySelector('#page-room'),null);
  await navigateHash('#page-room-network-links-state-0');
  assert.equal(n('#page-room').open,true);
  assert.ok(n('#page-room-network-links-state-0'));
});
