/** Third-consumer mounted contracts for shared presentation owners.
 * Synthetic DOM only; no browser layout, animation pixels or native runtime. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';
const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, React, createRoot, owners, root;
const act = async callback => React.act(async () => {callback(); await new Promise(resolve => setTimeout(resolve, 0));});
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(project, '.cache/settings-primitives-'));
  const output = resolve(work, 'owners.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export {SettingsBody} from './app/components/settings/SettingsBody.tsx';
    export {createGameSettingsModel} from './app/models/game-settings.ts';
    export {LocaleProvider} from './app/i18n.tsx';
    export {LaunchActions} from './app/components/launcher/LaunchActions.tsx';
    export {OptionsGroup} from './app/components/settings/OptionsGroup.tsx';
    export {OptionRow} from './app/components/settings/OptionRow.tsx';
    export {OptionSwitch} from './app/components/settings/TouchSettingsFields.tsx';
    export {DEFAULT_TH05_MULTIPLAYER_DISPLAY, normalizeTh05MultiplayerDisplay} from './src/launcher/game-preferences.mts';
  `}, outfile: output, bundle: true, platform: 'node', format: 'esm', packages: 'external', jsx: 'automatic', plugins: [authoredSourcesPlugin(project)], logLevel: 'silent'});
  owners = await import(pathToFileURL(output).href);
});
afterEach(async () => {if (root) await act(() => root.unmount()); root = null; assert.deepEqual(env.errors, []); env.document.body.replaceChildren();});
after(async () => {env?.close(); if (work) await rm(work, {recursive: true, force: true});});
async function render(tree) {if (!root) {const container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container);} await act(() => root.render(tree));}
const n = selector => {const node = env.document.querySelector(selector); assert.ok(node, selector); return node;};
function createSettings(productId, values = new Map()) {
  const storage = {getItem: key => values.get(key) ?? null, setItem: (key, value) => values.set(key, String(value)), removeItem: key => values.delete(key)};
  const model = owners.createGameSettingsModel({storage});
  const context = {productId, uiLocale: 'en', languages: [{id: 'ja', title: 'Japanese', pack: null}],
    musicAvailability: {audio: true, midiAvailable: false, importServer: false, remoteOggAdvertised: false}, webMidiAvailable: false, mobile: false};
  model.hydrate(context);
  return {model, context, storage, values};
}
function settingsActions() {
  const unavailable = () => {throw new Error('Unexpected settings action');};
  const midi = Object.freeze({enabled: false, granted: false, busy: false, supported: false, outputs: [], selectedId: '', hint: 'Unavailable'});
  return {confirm: unavailable, file: unavailable, showAppleNotice: unavailable, feedback() {}, reportError: unavailable,
    externalMidi: {subscribe: () => () => {}, getSnapshot: () => midi, setEnabled: unavailable, selectOutput: unavailable}};
}

test('third launch consumer shares shell/icon order and retains current intent, disabled and label ownership', async () => {
  const calls = []; let submits = 0;
  async function update(label, disabled = false) {
    await render(React.createElement('form', {onSubmit: event => {event.preventDefault(); submits++;}}, React.createElement(owners.LaunchActions, {
      placement: 'compact', primary: {id: 'thirdPlay', textId: 'thirdText', label, disabled, onClick: () => calls.push(label)},
      secondary: {id: 'thirdImport', label: 'Import', onClick: () => calls.push('import')},
    })));
  }
  await update('Preview');
  const play = n('#thirdPlay'), secondary = n('#thirdImport');
  assert.equal(play.parentElement.className, 'launch-wrap launch-actions launch-actions-compact');
  assert.ok(play.classList.contains('launch')); assert.ok(secondary.classList.contains('launch-secondary'));
  assert.equal(play.lastElementChild.id, 'thirdText');
  assert.equal(secondary.lastElementChild.textContent, 'Import');
  assert.ok(play.firstElementChild.matches('svg.lucide-play'));
  assert.ok(secondary.firstElementChild.matches('svg.lucide-folder-input'));
  for (const button of [play, secondary]) {assert.equal(button.type, 'button'); assert.equal(button.firstElementChild.getAttribute('aria-hidden'), 'true');}
  play.click(); secondary.click(); assert.deepEqual(calls, ['Preview', 'import']); assert.equal(submits, 0);
  await update('Busy', true); assert.equal(n('#thirdPlay'), play); play.click(); assert.deepEqual(calls, ['Preview', 'import']);
  await update('Retry'); play.click(); assert.deepEqual(calls, ['Preview', 'import', 'Retry']); assert.equal(submits, 0);
});

test('third native group keeps model-owned disclosure and same child/focus through repeated close/reopen', async () => {
  const changes = [];
  const input = React.createElement('input', {defaultValue: 'draft', id: 'draft'});
  async function update(open, prefix = 'first') {await render(React.createElement(owners.OptionsGroup, {id: 'thirdGroup', title: 'Third group', open,
    onOpenChange: value => changes.push([prefix, value]), bodyClassName: 'file-tools-grid'}, input));}
  await update(true);
  const group = n('#thirdGroup'), draft = n('#draft'), summary = n('.options-group-head');
  assert.equal(group.className, 'options-group'); assert.ok(summary.classList.contains('options-group-head'));
  assert.equal(summary.querySelector('.options-group-chevron').getAttribute('aria-hidden'), 'true');
  assert.equal(draft.parentElement.className, 'file-tools-grid'); draft.value = 'edited'; draft.focus();
  await act(() => summary.click()); assert.equal(changes.at(-1)[1], false, 'native toggle reports actual details state');
  await update(false, 'latest'); await update(true, 'latest');
  assert.equal(n('#thirdGroup'), group); assert.equal(n('#draft'), draft); assert.equal(draft.value, 'edited');
  assert.equal(env.document.activeElement, draft, 'presentation did not invent a focus lifecycle');
  await act(() => summary.click()); assert.deepEqual(changes.at(-1), ['latest', false]);
});

test('field labels activate the linked switch and describe it while hidden/disabled gates stay controlled', async () => {
  const changes = [];
  async function update({hidden = false, disabled = false, checked = false, hint = 'Reason'} = {}) {
    await render(React.createElement(owners.OptionRow, {id: 'thirdRow', className: 'item option-frame-limit', label: 'Third switch', hint,
      hintId: 'thirdHint', hidden, control: React.createElement(owners.OptionSwitch, {id: 'thirdSwitch', label: 'Third switch', title: 'Capability reason',
        disabled, checked, onChange: () => changes.push(!checked)})}));
  }
  await update(); const row = n('#thirdRow'), button = n('#thirdSwitch');
  assert.equal(row.dataset.slot, 'field'); assert.equal(n('#thirdHint').parentElement.firstElementChild.textContent, 'Third switch');
  assert.equal(row.querySelector('label').htmlFor, 'thirdSwitch');
  assert.equal(button.getAttribute('aria-describedby'), 'thirdHint'); assert.equal(button.title, 'Capability reason');
  await act(()=>row.querySelector('label').click()); assert.deepEqual(changes, [true]);
  await update({disabled: true, checked: true, hidden: true}); assert.equal(n('#thirdSwitch'), button); assert.equal(row.hidden, true);
  assert.equal(button.getAttribute('aria-checked'), 'true'); assert.ok(button.classList.contains('ui-switch')); assert.equal(button.dataset.state,'checked'); await act(()=>button.click()); assert.deepEqual(changes, [true]);
  await render(React.createElement(owners.OptionRow, {className: 'item', label: 'Plain', control: React.createElement('button', {type: 'button'}, 'Control')}));
  assert.equal(env.document.querySelector('[data-slot="field-description"]'), null); assert.equal(n('[data-slot="field-label"]').textContent, 'Plain');
});

for (const productId of ['th06', 'th06mp']) test(`${productId}: grouped settings retain one touch section across mobile reorder`, async () => {
  const values = new Map();
  const model = owners.createGameSettingsModel({storage: {getItem: key => values.get(key) ?? null, setItem: (key, value) => values.set(key, String(value))}});
  const context = {productId, uiLocale: 'en', languages: [{id: 'ja', title: 'Japanese'}],
    musicAvailability: {audio: true, midiAvailable: true, importServer: false, remoteOggAdvertised: false}, webMidiAvailable: false, mobile: false};
  model.hydrate(context); model.setDisclosure('touch', true);
  const midi = Object.freeze({enabled: false, granted: false, busy: false, supported: false, outputs: [], selectedId: '', hint: 'Unavailable'});
  const unexpected = () => {throw new Error('Unexpected domain action');};
  const actions = {confirm: unexpected, file: unexpected, showAppleNotice: unexpected, feedback() {}, reportError: unexpected,
    externalMidi: {subscribe: () => () => {}, getSnapshot: () => midi, setEnabled: unexpected, selectOutput: unexpected}};
  await render(React.createElement(owners.LocaleProvider, {locale: 'en'}, React.createElement(owners.SettingsBody, {model, actions, onOpenTouchLayout: unexpected})));
  assert.equal(model.getSnapshot().options.touchMovementMode, 'touch-unlimited');
  const touchSwitch = n(productId.endsWith('mp') ? '#mpTouchToggle' : '#touchToggle');
  await act(() => touchSwitch.click());
  assert.equal(model.getSnapshot().options.touchEnabled, true, 'enabling unlimited touch applies directly without a warning decision');
  await act(() => touchSwitch.click());
  assert.equal(model.getSnapshot().options.touchEnabled, false);
  const pixelSwitch = n(productId.endsWith('mp') ? '#mpPixelPerfectToggle' : '#pixelPerfectToggle');
  assert.equal(pixelSwitch.getAttribute('aria-label'), 'Enable pixel-perfect display');
  assert.equal(model.getSnapshot().options.pixelPerfectEnabled, false);
  await act(() => pixelSwitch.click());
  assert.equal(model.getSnapshot().options.pixelPerfectEnabled, true);
  const prefix = productId.endsWith('mp') ? 'mpMobile' : 'mobile';
  const section = n(`#${prefix}Options`), toggle = n(`#${prefix}OptionsToggle`), body = n(`#${prefix}OptionsBody`);
  const groups = [...env.document.querySelectorAll('.options-group')].filter(node => node !== section);
  toggle.focus();
  for (const mobile of [true, false, true, false]) {
    await act(() => model.refreshContext({...context, mobile}));
    assert.equal(n(`#${prefix}Options`) === section, true); assert.equal(n(`#${prefix}OptionsToggle`) === toggle, true); assert.equal(n(`#${prefix}OptionsBody`) === body, true);
    assert.equal(env.document.querySelectorAll(`#${prefix}Options`).length, 1);
    assert.equal(groups.every(node => env.document.getElementById(node.id) === node),true,'Group nodes survive mobile reorder');
    assert.equal(model.getSnapshot().disclosure.touch, true); assert.equal(body.hasAttribute('inert'), false);
    assert.equal(env.document.activeElement.id, toggle.id);
    const siblings = [...section.parentElement.children];
    assert.equal(mobile ? siblings.indexOf(section) < siblings.indexOf(groups[0]) : siblings.indexOf(section) > siblings.indexOf(groups.at(-1)), true);
  }
  // Replay workspaces do not mount SettingsBody. The actual Replay entry and
  // absence of live input controls are checked by browser-session-mounted.
  assert.equal(pixelSwitch.getAttribute('aria-checked'), 'true');
  await act(() => pixelSwitch.click());
  assert.equal(model.getSnapshot().options.pixelPerfectEnabled, false);
});

test('TH05 MP display normalization and persistence keep only options valid for the selected bar', () => {
  assert.deepEqual(owners.DEFAULT_TH05_MULTIPLAYER_DISPLAY,
    {bossBar: 'native', phaseMarkers: false, phaseStars: false, phaseTimeout: false, timeoutAsTime: false, numericDream: false, numericPower: false});
  const {model, context, values} = createSettings('th05mp');
  assert.deepEqual(model.getSnapshot().options.th05MultiplayerDisplay, owners.DEFAULT_TH05_MULTIPLAYER_DISPLAY);
  model.setOption('th05MultiplayerDisplay', {bossBar: 'eosd', phaseMarkers: true, phaseStars: true, phaseTimeout: false,
    timeoutAsTime: true, numericDream: true, numericPower: true});
  assert.deepEqual(model.getSnapshot().options.th05MultiplayerDisplay,
    {bossBar: 'eosd', phaseMarkers: false, phaseStars: true, phaseTimeout: true, timeoutAsTime: true, numericDream: true, numericPower: true});
  model.setOption('th05MultiplayerDisplay', {bossBar: 'ring', phaseMarkers: true, phaseStars: true, phaseTimeout: false,
    timeoutAsTime: true, numericDream: true, numericPower: false});
  const expected = {bossBar: 'ring', phaseMarkers: false, phaseStars: false, phaseTimeout: true,
    timeoutAsTime: true, numericDream: true, numericPower: false};
  assert.deepEqual(model.getSnapshot().options.th05MultiplayerDisplay, expected);
  const restored = createSettings('th05mp', values);
  assert.deepEqual(restored.model.getSnapshot().options.th05MultiplayerDisplay, expected);
  assert.equal(context.productId, 'th05mp');
});

test('TH17 render resolution uses the shared local preference in single-player and rooms', async () => {
  const {model, values} = createSettings('th17mp');
  const actions = settingsActions();
  const mount = current => render(React.createElement(owners.LocaleProvider, {locale: 'en'},
    React.createElement(owners.SettingsBody, {model: current, actions, onOpenTouchLayout: () => {}})));
  await mount(model);
  assert.equal(n('#mpRenderResolution').value, '2');
  await act(() => n('#mpRenderResolution-trigger').click());
  await act(() => n('[role="option"][data-value="1"]').click());
  assert.equal(model.getSnapshot().options.renderScale, 1);
  await mount(createSettings('th17', values).model);
  assert.equal(n('#renderResolution').value, '1');
  await mount(createSettings('th15mp', values).model);
  assert.equal(env.document.querySelector('#mpRenderResolution'), null);
});

test('TH05 MP display controls track EoSD/ring dependencies and stay hidden for single-player', async () => {
  const {model} = createSettings('th05mp');
  const actions = settingsActions();
  await render(React.createElement(owners.LocaleProvider, {locale: 'en'},
    React.createElement(owners.SettingsBody, {model, actions, onOpenTouchLayout: () => {}})));
  const bossBar = n('#mpTh05BossBarSelect');
  assert.deepEqual([...bossBar.options].map(option => option.value), ['native', 'eosd', 'ring']);
  assert.equal(n('#mpTh05PhaseMarkersToggle').disabled, false);
  assert.equal(n('#mpTh05PhaseStarsToggle').disabled, true);
  assert.equal(n('#mpTh05PhaseTimeoutToggle').disabled, false);
  assert.equal(n('#mpTh05TimeoutAsTimeToggle').disabled, true);

  async function chooseBar(value) {
    await act(() => n('#mpTh05BossBarSelect-trigger').click());
    const option = env.document.querySelector(`[role="option"][data-value="${value}"]`);
    assert.ok(option, `visible boss-bar option ${value}`);
    await act(() => option.click());
  }
  await chooseBar('eosd');
  assert.equal(model.getSnapshot().options.th05MultiplayerDisplay.bossBar, 'eosd');
  assert.equal(model.getSnapshot().options.th05MultiplayerDisplay.phaseTimeout, true);
  assert.equal(n('#mpTh05PhaseMarkersToggle').disabled, true);
  assert.equal(n('#mpTh05PhaseStarsToggle').disabled, false);
  assert.equal(n('#mpTh05PhaseTimeoutToggle').disabled, true);
  assert.equal(n('#mpTh05TimeoutAsTimeToggle').disabled, false);
  await act(() => n('#mpTh05PhaseStarsToggle').click());
  assert.equal(model.getSnapshot().options.th05MultiplayerDisplay.phaseStars, true);

  await chooseBar('ring');
  const display = model.getSnapshot().options.th05MultiplayerDisplay;
  assert.deepEqual(display, {bossBar: 'ring', phaseMarkers: false, phaseStars: false, phaseTimeout: true,
    timeoutAsTime: false, numericDream: false, numericPower: false});
  assert.equal(n('#mpTh05PhaseMarkersToggle').disabled, true);
  assert.equal(n('#mpTh05PhaseStarsToggle').disabled, true);
  assert.equal(n('#mpTh05PhaseTimeoutToggle').disabled, true);
  assert.equal(n('#mpTh05TimeoutAsTimeToggle').disabled, false);

  const single = createSettings('th05');
  await render(React.createElement(owners.LocaleProvider, {locale: 'en'},
    React.createElement(owners.SettingsBody, {model: single.model, actions, onOpenTouchLayout: () => {}})));
  assert.equal(env.document.querySelector('#mpTh05BossBarSelect'), null);
  assert.equal(env.document.querySelector('[id^="mpTh05"][id$="Toggle"]'), null);
});
