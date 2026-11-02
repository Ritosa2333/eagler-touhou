import {test, vi, beforeAll, afterAll, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

let env, React, createRoot, RoomExample, work, root;
const act = async fn => React.act(async () => {await fn?.(); for (let i = 0; i < 12; i++) await Promise.resolve();});
const n = selector => {const node = env.document.querySelector(selector); assert.ok(node, selector); return node;};
const button = text => [...env.document.querySelectorAll('button')].find(node => node.textContent === text);
beforeAll(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve('.cache/handbook-room-'));
  await build({absWorkingDir: process.cwd(), entryPoints: ['app/ui/room-example.tsx'], bundle: true, platform: 'node',
    format: 'esm', jsx: 'automatic', outfile: resolve(work, 'demo.mjs'), packages: 'external', loader: {'.css': 'empty'},
    plugins: [authoredSourcesPlugin(process.cwd())], logLevel: 'silent'});
  ({RoomExample} = await import(pathToFileURL(resolve(work, 'demo.mjs'))));
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
  await act(() => button('观战席').dispatchEvent(new env.window.KeyboardEvent('keydown', {key: 'Enter', bubbles: true})));
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

test('seat panels preserve the requested role and peer', async () => {
  await mount();
  await act(() => n('button[aria-label="我的角色"]').click());
  assert.equal(n('#mpRoomPanel').dataset.panel, 'personal');
  assert.equal(n('#mpDisplayName').value, '演示房主');
  await act(() => n('#mpRoomPanelClose').click());
  await act(() => n('[data-network-peer="handbook-player"]').click());
  assert.equal(n('#mpRoomPanel').dataset.networkPeer, 'handbook-player');
  assert.equal(n('#mpRoomPanel').querySelectorAll('.mp-network-peer').length, 1);
});
