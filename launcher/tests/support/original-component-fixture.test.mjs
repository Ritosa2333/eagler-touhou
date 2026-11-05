import {readUiHistoryText} from './ui-history.mjs';
/** Carrier setup only. No browser, layout, native focus/input or animation gate. */
import {test} from 'vitest';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import {parse} from 'acorn';
import {JSDOM} from 'jsdom';
import {transform} from 'esbuild';
import {originalComponentFixture} from './original-component-fixture.mjs';
const project = fileURLToPath(new URL('../../', import.meta.url));
const baseline = 'edee9633e5e3ee79cd2e1aa334f84f6caf755090';
const path = 'tests/test-multiplayer-quick-chat-browser.mjs';
const pinned = name => readUiHistoryText('edee9633e5e3ee79cd2e1aa334f84f6caf755090', name);
function nodes(source) {
  const result = [];
  function visit(node) {
    if (!node || typeof node !== 'object') return;
    if (node.type) result.push(node);
    for (const value of Object.values(node)) if (Array.isArray(value)) value.forEach(visit); else if (value && typeof value === 'object') visit(value);
  }
  visit(parse(source, {ecmaVersion: 'latest', sourceType: 'module'})); return result;
}
const call = (node, object, property) => node.type === 'CallExpression' && node.callee.type === 'MemberExpression' && node.callee.object.name === object && node.callee.property.name === property;

test('main selection builds nothing; unsupported targets fail closed', async () => {
  const previous = process.env.EAGLER_COMPONENT_TEST_TARGET;
  try {
    delete process.env.EAGLER_COMPONENT_TEST_TARGET;
    assert.equal(await originalComponentFixture('not-a-buildable-carrier'), null);
    assert.equal(await originalComponentFixture('not-a-buildable-carrier', 'main'), null);
    await assert.rejects(originalComponentFixture('multiplayer-quick-chat', 'invalid'), /must be main or react/);
    await assert.rejects(originalComponentFixture('not-a-buildable-carrier', 'react'), /Unknown original component fixture/);
  } finally {
    if (previous === undefined) delete process.env.EAGLER_COMPONENT_TEST_TARGET; else process.env.EAGLER_COMPONENT_TEST_TARGET = previous;
  }
});

test('selected carrier mounts production React under original Player and accepts exact original initialization', async () => {
  const fixture = await originalComponentFixture('multiplayer-quick-chat', 'react');
  for (const owner of ['app/components/player/MultiplayerQuickChat.tsx', 'src/launcher/multiplayer-quick-chat-model.mts', 'src/launcher/quick-chat-voice.mts', 'src/contracts/multiplayer-quick-chat.mts', 'app/i18n.tsx']) assert.ok(fixture.inputs.includes(owner), owner);
  assert.ok(!fixture.inputs.includes('src/launcher/multiplayer-quick-chat.mts'), 'The carrier must mount React, not the legacy imperative view');
  const original = pinned(path), tree = nodes(original);
  const html = tree.find(node => call(node, 'res', 'end') && node.arguments[0]?.value?.startsWith('<!doctype html')).arguments[0].value;
  const initialize = tree.find(node => call(node, 'page', 'evaluate')).arguments[0].body;
  let initSource = original.slice(initialize.start + 1, initialize.end - 1);
  const originalImport = "const {MultiplayerQuickChat}=await import('/assets/launcher/multiplayer-quick-chat.mjs');";
  assert.ok(initSource.includes(originalImport));
  initSource = initSource.replace(originalImport, 'const {MultiplayerQuickChat}=window.Fixture;');
  const dom = new JSDOM(html, {url: 'http://127.0.0.1:18900/', runScripts: 'outside-only'});
  const {window} = dom, errors = [], timers = new Map(); let nextTimer = 0;
  window.matchMedia = query => ({matches: true, media: query});
  window.setTimeout = (callback, delay) => {const id = ++nextTimer; timers.set(id, {callback, delay}); return id;};
  window.clearTimeout = id => timers.delete(id);
  window.addEventListener('error', event => errors.push(event.error || event.message));
  const parent = window.document.getElementById('fixture'), controls = [...parent.children];
  try {
    const script = await transform(fixture.module, {format: 'iife', globalName: 'Fixture'}); window.eval(script.code);
    assert.throws(() => new window.Fixture.MultiplayerQuickChat(parent, () => '', () => {}), /fixture translation mismatch/);
    assert.deepEqual([...parent.children], controls, 'Mismatched fixture translations fail before mounting');
    window.eval(initSource);
    assert.equal(typeof window.chat.update, 'function'); assert.equal(typeof window.chat.receive, 'function');
    assert.deepEqual([...parent.children].slice(0, controls.length), controls, 'Original controls retain identity and ordering');
    const chat = parent.querySelector(':scope > .mp-quick-chat'); assert.ok(chat, 'React view is a direct Player child');
    assert.equal(parent.children.length, controls.length + 1, 'No layout wrapper or replaced Player markup');
    assert.equal(chat.querySelector('.mp-quick-chat-log').children.length, 1, 'Original initialization receives synchronously');
    assert.equal(JSON.stringify(window.playedClips), '["assets/quick-chat/1.wav"]', 'Original Audio stub receives production voice URL');
    const expiryTimers = () => [...timers].filter(([, timer]) => timer.delay === 10000);
    assert.equal(expiryTimers().length, 1, 'Current quick-chat contract keeps accepted messages for 10 seconds');
    const [timerId, timer] = expiryTimers()[0]; timers.delete(timerId); timer.callback();
    window.chat.update(window.ctx); // Flush the production view after model expiry.
    assert.equal(chat.querySelector('.mp-quick-chat-log').children.length, 0);
    window.chat.receive(window.message);
    assert.equal(expiryTimers().length, 1, 'A later message owns a fresh expiry');
    assert.deepEqual(errors, []);
    window.dispatchEvent(new window.Event('pagehide'));
    assert.equal(expiryTimers().length, 0); assert.deepEqual([...parent.children], controls, 'Page teardown releases only the carrier');
  } finally {window.dispatchEvent(new window.Event('pagehide')); window.close();}
});
