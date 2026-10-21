import test from 'node:test';
import assert from 'node:assert/strict';
import { EventEmitter } from 'node:events';
import { createHeadlessLifecycleDiagnostics } from './headless-lifecycle-diagnostics.mjs';

test('unexpected page closure retains the active epoch and exact retirement phase', () => {
  const page = new EventEmitter();
  page.url = () => 'https://fixture.test/runtime/th08-normal/entry.mjs?runtimeEpoch=3';
  const diagnostics = createHeadlessLifecycleDiagnostics({ targetId: 'th08-normal', now: () => '2026-10-21T00:00:00.000Z' });
  diagnostics.attachPage(page);
  diagnostics.setPhase('retireAndLaunch.frame-goto', { activeEpoch: 3, plannedEpoch: 4 });

  page.emit('close');

  const snapshot = diagnostics.snapshot();
  assert.equal(snapshot.events.length, 1);
  assert.deepEqual(snapshot.unexpectedEvents[0], {
    sequence: 1,
    at: '2026-10-21T00:00:00.000Z',
    targetId: 'th08-normal',
    activeEpoch: 3,
    plannedEpoch: 4,
    phase: 'retireAndLaunch.frame-goto',
    ownedTeardownPhase: null,
    event: 'page-close',
    expected: false,
    unexpected: true,
    url: 'https://fixture.test/runtime/th08-normal/entry.mjs?runtimeEpoch=3',
  });
});

test('owned context cleanup records page and context close without classifying them as errors', () => {
  const page = new EventEmitter();
  page.url = () => 'https://fixture.test/host/th08-normal';
  const context = new EventEmitter();
  context.pages = () => [];
  const diagnostics = createHeadlessLifecycleDiagnostics({ targetId: 'th08-normal' });
  diagnostics.attachPage(page);
  diagnostics.attachContext(context);
  diagnostics.setPhase('target-context-close', { activeEpoch: 7, ownedTeardownPhase: 'target-context-close' });

  page.emit('close');
  context.emit('close');

  const snapshot = diagnostics.snapshot();
  assert.deepEqual(snapshot.events.map(event => [event.event, event.expected, event.unexpected]), [
    ['page-close', true, false],
    ['context-close', true, false],
  ]);
  assert.deepEqual(snapshot.unexpectedEvents, []);
});

test('renderer crash is unexpected even while the context is being intentionally closed', () => {
  const page = new EventEmitter();
  page.url = () => 'https://fixture.test/runtime/th08-normal/entry.mjs';
  const diagnostics = createHeadlessLifecycleDiagnostics({ targetId: 'th08-normal' });
  diagnostics.attachPage(page);
  diagnostics.setPhase('target-context-close', { activeEpoch: 2, ownedTeardownPhase: 'target-context-close' });

  page.emit('crash', new Error('renderer exited'));

  const [event] = diagnostics.snapshot().unexpectedEvents;
  assert.equal(event.event, 'page-crash');
  assert.equal(event.phase, 'target-context-close');
  assert.equal(event.message, 'renderer exited');
});

test('browser disconnect is expected only during the runner-owned browser close phase', () => {
  const browser = new EventEmitter();
  browser.isConnected = () => false;
  const diagnostics = createHeadlessLifecycleDiagnostics({ targetId: 'th08-normal' });
  diagnostics.attachBrowser(browser);
  diagnostics.setPhase('runtime-navigation', { activeEpoch: 5, plannedEpoch: 6 });
  browser.emit('disconnected');
  diagnostics.setPhase('run-browser-close', { activeEpoch: 5, ownedTeardownPhase: 'run-browser-close' });
  browser.emit('disconnected');

  const snapshot = diagnostics.snapshot();
  assert.deepEqual(snapshot.events.map(event => [event.phase, event.expected, event.unexpected]), [
    ['runtime-navigation', false, true],
    ['run-browser-close', true, false],
  ]);
  assert.equal(snapshot.events[0].connected, false);
});

test('detaching observers prevents later close notifications from being attributed to the run', () => {
  const page = new EventEmitter();
  page.url = () => 'about:blank';
  const diagnostics = createHeadlessLifecycleDiagnostics({ targetId: 'th08-normal' });
  diagnostics.attachPage(page);
  diagnostics.detachAll();
  page.emit('close');
  assert.deepEqual(diagnostics.snapshot().events, []);
});
