import assert from 'node:assert/strict';
import {mkdtempSync, readFileSync, rmSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
import test from 'node:test';
import {createSpectatorTimelineView} from './spectator-timeline-view.mjs';

class FakeNode {
  constructor(ownerDocument, tagName) {
    this.ownerDocument = ownerDocument;
    this.tagName = tagName.toUpperCase();
    this.children = [];
    this.parentNode = null;
    this.attributes = new Map();
    this.listeners = new Map();
    this.dataset = {};
    this.style = {values: new Map(), setProperty(name, value) { this.values.set(name, value); }};
    this.textContent = '';
    this.hidden = false;
    this.disabled = false;
    this.value = '';
    this.min = '';
    this.max = '';
    this.clientWidth = 300;
  }
  append(...nodes) {
    for (const node of nodes) {
      node.remove();
      node.parentNode = this;
      this.children.push(node);
    }
  }
  replaceChildren(...nodes) {
    for (const child of this.children) child.parentNode = null;
    this.children = [];
    this.textContent = '';
    this.append(...nodes);
  }
  remove() {
    if (!this.parentNode) return;
    const at = this.parentNode.children.indexOf(this);
    if (at >= 0) this.parentNode.children.splice(at, 1);
    this.parentNode = null;
  }
  setAttribute(name, value) { this.attributes.set(name, String(value)); }
  getAttribute(name) { return this.attributes.get(name) ?? null; }
  removeAttribute(name) { this.attributes.delete(name); }
  addEventListener(name, callback) {
    const list = this.listeners.get(name) || [];
    list.push(callback); this.listeners.set(name, list);
  }
  dispatch(name, values = {}) {
    const event = {type: name, target: this, currentTarget: this, detail: 0, pointerType: 'mouse', preventDefault() { this.defaultPrevented = true; }, ...values};
    for (const callback of this.listeners.get(name) || []) callback(event);
    return event;
  }
  contains(node) { return node === this || this.children.some(child => child.contains(node)); }
  setPointerCapture() {}
  get childNodes() { return this.children; }
  get firstChild() { return this.children[0] || null; }
}

class FakeDocument {
  constructor() {
    this.head = new FakeNode(this, 'head');
    this.documentElement = new FakeNode(this, 'html');
    this.body = new FakeNode(this, 'body');
  }
  createElement(tag) { return new FakeNode(this, tag); }
}

function find(root, id) {
  if (root.id === id) return root;
  for (const child of root.children) {
    const found = find(child, id);
    if (found) return found;
  }
  return null;
}

function makeModel(overrides = {}) {
  return {
    key: 'run-a', min: 0, position: 30, edge: 90, received: 100,
    paused: false, seeking: false, following: true, ended: false,
    marks: [], statusText: 'Following', lagText: '0', durationText: 'duration',
    ...overrides,
  };
}

test('keeps TH09 IDs, scopes CSS to classes, and reference-counts injected styles', () => {
  const document = new FakeDocument();
  const first = createSpectatorTimelineView({document});
  const second = createSpectatorTimelineView({document, idPrefix: 'cooperativeSpectator'});
  assert.equal(first.panel.id, 'spectatorTimeline');
  assert.equal(first.slots.summary.id, 'spectatorScore');
  assert.equal(first.slots.duration.id, 'spectatorMatchDuration');
  assert.equal(find(second.panel, 'cooperativeSpectatorScore'), second.slots.summary);
  assert.equal(document.head.children.length, 1);
  const css = document.head.children[0].textContent;
  assert.match(css, /\.spectator-timeline \.spectator-score-slot/);
  const selectors = [...css.matchAll(/([^{}]+)\{/g)].map(match => match[1]).filter(value => !value.trimStart().startsWith('@'));
  assert.ok(selectors.every(selector => selector.includes('spectator')));
  assert.ok(selectors.every(selector => !/(^|,)\s*#[A-Za-z][\w-]*/.test(selector)));
  assert.match(css, /\.spectator-pause-button\[data-paused=true\] \.spectator-play-icon\{display:block\}/);
  first.destroy();
  assert.equal(document.head.children.length, 1);
  second.destroy();
  assert.equal(document.head.children.length, 0);
});

test('preserves host-owned icons on update/toggle and applies explicitly supplied icons', () => {
  const document = new FakeDocument();
  const view = createSpectatorTimelineView({document});
  view.update(makeModel());
  const toggle = find(view.panel, 'spectatorToggle');
  const pause = find(view.panel, 'spectatorPause');
  const live = find(view.panel, 'spectatorLive');
  const icons = [toggle.children[0], pause.children[0], pause.children[1], live.children[0]];
  const hostSources = ['/host/toggle.svg', '/host/pause.svg', '/host/play.svg', '/host/live.svg'];
  icons.forEach((icon, index) => icon.setAttribute('src', hostSources[index]));

  view.update(makeModel({position: 31}));
  toggle.dispatch('click');
  icons.forEach((icon, index) => assert.equal(icon.getAttribute('src'), hostSources[index]));

  view.updateLabels({}, {toggle: '/shared/toggle.svg'});
  assert.equal(icons[0].getAttribute('src'), '/shared/toggle.svg');
  assert.equal(icons[1].getAttribute('src'), hostSources[1]);
  assert.equal(icons[2].getAttribute('src'), hostSources[2]);
  assert.equal(icons[3].getAttribute('src'), hostSources[3]);
  view.destroy();
});

test('maps stage and boss marks, allows adapter-owned same-frame policy, and preserves summary slot', () => {
  const document = new FakeDocument();
  const actions = [];
  const view = createSpectatorTimelineView({document, onAction: (...args) => actions.push(args), labels: {summary: 'Playback'}});
  view.slots.summary.textContent = 'React portal content';
  view.update(makeModel({position: 30, marks: [
    {id: 'stage-2', frame: 30, kind: 'stage', label: 'Stage 2'},
    {id: 'boss-1', frame: 60, kind: 'boss', label: 'Boss'},
  ]}));
  const stage = find(view.panel, 'spectatorRoundMarks').children[0];
  const boss = find(view.panel, 'spectatorRoundMarks').children[1];
  assert.equal(stage.dataset.kind, 'match');
  assert.equal(stage.disabled, false);
  assert.equal(boss.dataset.kind, 'round');
  assert.equal(find(view.panel, 'spectatorMatchStats').getAttribute('aria-label'), 'Playback');
  assert.equal(view.slots.summary.textContent, 'React portal content');
  stage.dispatch('click');
  assert.deepEqual(actions, [['seek', 30, 'run-a']]);
  view.destroy();
});

test('detects same-kind marker collisions across interleaved kinds and keeps next label', () => {
  const document = new FakeDocument();
  const view = createSpectatorTimelineView({document, onAction() {}});
  view.update(makeModel({position: 0, marks: [
    {id: 'match-a', frame: 10, kind: 'match', label: 'Match A'},
    {id: 'round-x', frame: 11, kind: 'round', label: 'Round X'},
    {id: 'match-b', frame: 12, kind: 'match', label: 'Match B'},
  ]}));
  const [matchA, roundX, matchB] = find(view.panel, 'spectatorRoundMarks').children;
  assert.equal(matchA.textContent, 'Match A');
  assert.equal(matchA.dataset.next, 'true');
  assert.equal(roundX.textContent, 'Round X');
  assert.equal(matchB.textContent, '');
  assert.match(matchB.getAttribute('aria-label'), /^Match B/);
  view.destroy();
});

test('freezes drag display, uses the latest edge on commit, and pointercancel never seeks', () => {
  const document = new FakeDocument();
  const actions = [];
  const view = createSpectatorTimelineView({document, onAction: (...args) => actions.push(args)});
  const range = find(view.panel, 'spectatorSeek');
  view.update(makeModel({position: 1000, edge: 9000, durationText: undefined}));
  range.dispatch('pointerdown', {pointerId: 1});
  range.value = '3600';
  range.dispatch('input');
  view.update(makeModel({position: 7000, edge: 11000, timeOrigin: 100, durationText: undefined, marks: [{id: 'later', frame: 8000, kind: 'stage'}]}));
  assert.equal(range.min, '0');
  assert.equal(range.max, '9000');
  assert.equal(range.value, '3600');
  assert.equal(find(view.panel, 'spectatorCurrentTime').textContent, '1:00');
  assert.equal(view.slots.duration.textContent, '2:30');
  assert.equal(find(view.panel, 'spectatorRoundMarks').children[0].style.left, '88.88888888888889%');
  range.value = '8000';
  range.dispatch('change');
  assert.deepEqual(actions, [['seek', 8000, 'run-a']]);

  range.dispatch('pointerdown', {pointerId: 2});
  range.value = '5000';
  range.dispatch('input');
  range.dispatch('pointercancel');
  range.dispatch('change');
  assert.deepEqual(actions, [['seek', 8000, 'run-a']]);
  view.destroy();
});

test('does not swallow a normal first change and suppresses old drag input/change after an epoch switch', async () => {
  const document = new FakeDocument();
  const actions = [];
  const view = createSpectatorTimelineView({document, onAction: (...args) => actions.push(args)});
  const range = find(view.panel, 'spectatorSeek');
  view.update(makeModel({key: 'session-1:epoch-3'}));
  range.value = '40';
  range.dispatch('change');
  assert.equal(actions.length, 1);
  range.dispatch('pointerdown', {pointerId: 3});
  range.value = '1400';
  range.dispatch('input');
  view.update(makeModel({key: 'session-1:epoch-4', position: 10, edge: 50}));
  await new Promise(resolve => setTimeout(resolve, 5));
  range.value = '1200';
  range.dispatch('input');
  range.dispatch('change');
  assert.equal(actions.length, 1);
  range.dispatch('keydown', {key: 'ArrowRight'});
  range.value = '20';
  range.dispatch('input');
  range.dispatch('change');
  assert.deepEqual(actions[1], ['seek', 20, 'session-1:epoch-4']);
  view.destroy();
});

test('reports asynchronous errors until a newer request or run supersedes them', async () => {
  const document = new FakeDocument();
  let rejectFirst;
  const view = createSpectatorTimelineView({document, onAction: () => new Promise((_, reject) => { rejectFirst = reject; })});
  const pause = find(view.panel, 'spectatorPause');
  view.update(makeModel());
  pause.dispatch('click');
  rejectFirst(new Error('offline'));
  await Promise.resolve();
  await Promise.resolve();
  assert.match(find(view.panel, 'spectatorError').textContent, /offline/);
  view.update(makeModel({position: 31}));
  assert.match(find(view.panel, 'spectatorError').textContent, /offline/);
  view.update(makeModel({key: 'run-b'}));
  assert.equal(find(view.panel, 'spectatorError').hidden, true);
  view.destroy();
});

test('sync tool copies exact bytes and --check detects drift', () => {
  const temporary = mkdtempSync(join(tmpdir(), 'spectator-timeline-'));
  try {
    const target = join(temporary, 'new', 'nested', 'view.mjs');
    const tool = fileURLToPath(new URL('../tools/sync-spectator-timeline-view.mjs', import.meta.url));
    const run = (...args) => spawnSync(process.execPath, [tool, ...args], {encoding: 'utf8'});
    const copied = run('--target', target);
    assert.equal(copied.status, 0, copied.stderr);
    assert.deepEqual(readFileSync(target), readFileSync(new URL('./spectator-timeline-view.mjs', import.meta.url)));
    assert.equal(run('--check', target).status, 0);
    writeFileSync(target, 'stale');
    assert.notEqual(run('--check', target).status, 0);
  } finally {
    rmSync(temporary, {recursive: true, force: true});
  }
});
