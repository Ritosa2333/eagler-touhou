/** DOM contracts for the shared shadcn primitives. Browser appearance is checked separately. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let env, React, createRoot, ui, work, root;
const el = (...args) => React.createElement(...args);
const act = fn => React.act(async () => {await fn();});
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(project, '.cache/shadcn-controls-'));
  const output = resolve(work, 'ui.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export {Button} from './app/components/ui/button.tsx';
    export {Input} from './app/components/ui/input.tsx';
    export {Switch} from './app/components/ui/switch.tsx';
    export {Progress} from './app/components/ui/progress.tsx';
  `}, outfile: output, bundle: true, platform: 'node', format: 'esm', packages: 'external', jsx: 'automatic', logLevel: 'silent'});
  ui = await import(pathToFileURL(output));
});
afterEach(async () => {if (root) await act(() => root.unmount()); root = null; env.document.body.replaceChildren(); assert.deepEqual(env.errors.splice(0), []);});
after(async () => {env?.close(); if (work) await rm(work, {recursive: true, force: true});});
async function render(children) {
  if (!root) {const host = env.document.createElement('div'); env.document.body.append(host); root = createRoot(host);}
  await act(() => root.render(children));
}

test('decision buttons retain refs, native form submission, disabled gates and hidden state', async () => {
  const ref = React.createRef(), input = React.createRef(); let submits = 0, clicks = 0;
  await render(el('form', {onSubmit: event => {event.preventDefault(); submits++;}},
    el(ui.Input, {ref: input, id: 'roomCode', required: true, pattern: '[0-9]{4}', defaultValue: '1234'}),
    el(ui.Button, {ref, id: 'submit', type: 'submit', name: 'choice', value: 'confirm'}, 'Confirm'),
    el(ui.Button, {id: 'disabled', type: 'button', disabled: true, onClick: () => clicks++}, 'Disabled'),
    el(ui.Button, {id: 'hidden', hidden: true}, 'Hidden')));
  assert.equal(ref.current, env.document.querySelector('#submit')); assert.equal(input.current.validity.valid, true);
  input.current.setCustomValidity('Invalid code'); assert.equal(input.current.validity.valid, false); input.current.setCustomValidity('');
  ref.current.focus(); assert.equal(env.document.activeElement, ref.current);
  await act(() => ref.current.click()); assert.equal(submits, 1); assert.equal(ref.current.value, 'confirm');
  env.document.querySelector('#disabled').click(); assert.equal(clicks, 0); assert.equal(env.document.querySelector('#hidden').hidden, true);
});

test('asChild composes an accessible link without a nested button', async () => {
  let clicks = 0;
  await render(el(ui.Button, {asChild: true, variant: 'outline', onClick: event => {event.preventDefault(); clicks++;}},
    el('a', {href: '/lobby.html', id: 'lobby'}, 'Multiplayer')));
  const link = env.document.querySelector('#lobby');
  assert.equal(link.tagName, 'A'); assert.equal(link.dataset.slot, 'button'); assert.equal(env.document.querySelector('button'), null);
  await act(() => link.click()); assert.equal(clicks, 1);
});

test('Radix switch commits once and keeps the authored thumb, state and disable policy', async () => {
  let calls = 0;
  function Owner({disabled = false}) {
    const [checked, setChecked] = React.useState(false);
    return el(ui.Switch, {id: 'touch', variant: 'legacy', className: 'option-switch', checked, disabled,
      'aria-label': 'Touch controls', onCheckedChange: value => {calls++; setChecked(value);}});
  }
  await render(el(Owner)); const control = env.document.querySelector('#touch');
  assert.equal(control.type, 'button'); assert.equal(control.getAttribute('aria-checked'), 'false');
  await act(() => control.click()); assert.equal(calls, 1); assert.equal(control.getAttribute('aria-checked'), 'true');
  assert.equal(control.firstElementChild.tagName, 'I'); assert.equal(control.firstElementChild.dataset.state, 'checked');
  assert.equal(control.firstElementChild.style.translate, '');
  await render(el(Owner, {disabled: true})); await act(() => control.click()); assert.equal(calls, 1);
});

test('resource progress distinguishes determinate and indeterminate values while preserving public fill IDs', async () => {
  const progress = value => el(ui.Progress, {id: 'track', 'aria-label': 'Resources', value,
    indicatorProps: {id: 'fill', style: {width: '42%', transform: 'none'}}});
  await render(progress(42)); const track = env.document.querySelector('#track');
  assert.equal(track.getAttribute('role'), 'progressbar'); assert.equal(track.getAttribute('aria-valuenow'), '42');
  assert.equal(track.firstElementChild.id, 'fill'); assert.equal(track.firstElementChild.style.width, '42%');
  await render(progress(null)); assert.equal(track.hasAttribute('aria-valuenow'), false); assert.equal(track.dataset.state, 'indeterminate');
});
