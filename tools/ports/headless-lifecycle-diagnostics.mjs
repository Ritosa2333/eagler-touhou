const CLOSE_KINDS = new Set(['page-close', 'context-close', 'browser-disconnected']);

function teardownExpected(kind, ownedTeardownPhase) {
  if (kind === 'page-crash') return false;
  if (kind === 'browser-disconnected') return ownedTeardownPhase === 'run-browser-close';
  if (kind === 'page-close' || kind === 'context-close') {
    return ownedTeardownPhase === 'target-context-close' || ownedTeardownPhase === 'run-browser-close';
  }
  return false;
}

function eventMessage(value) {
  if (value instanceof Error) return value.message;
  if (typeof value === 'string') return value;
  return value && typeof value.message === 'string' ? value.message : null;
}

export function createHeadlessLifecycleDiagnostics({ targetId, now = () => new Date().toISOString() } = {}) {
  if (typeof targetId !== 'string' || !targetId) throw new TypeError('A targetId is required for browser lifecycle diagnostics');
  if (typeof now !== 'function') throw new TypeError('The lifecycle clock must be a function');

  let state = {
    targetId,
    activeEpoch: null,
    plannedEpoch: null,
    phase: 'created',
    ownedTeardownPhase: null,
  };
  let sequence = 0;
  const events = [];
  const detachments = new Set();

  const setPhase = (phase, { activeEpoch = state.activeEpoch, plannedEpoch = null, ownedTeardownPhase = null } = {}) => {
    if (typeof phase !== 'string' || !phase) throw new TypeError('A lifecycle phase is required');
    state = { targetId, activeEpoch, plannedEpoch, phase, ownedTeardownPhase };
    return { ...state };
  };

  const attach = (emitter, eventName, kind, details = () => ({})) => {
    if (!emitter || typeof emitter.on !== 'function') throw new TypeError(`Cannot observe ${kind}: event source has no on() method`);
    const listener = (...args) => {
      let extra = {};
      try { extra = details(...args) ?? {}; } catch (error) { extra = { detailReadError: eventMessage(error) }; }
      const expected = CLOSE_KINDS.has(kind) && teardownExpected(kind, state.ownedTeardownPhase);
      events.push({
        sequence: ++sequence,
        at: now(),
        ...state,
        event: kind,
        expected,
        unexpected: !expected,
        ...extra,
      });
    };
    emitter.on(eventName, listener);
    const detach = () => {
      if (typeof emitter.off === 'function') emitter.off(eventName, listener);
      else if (typeof emitter.removeListener === 'function') emitter.removeListener(eventName, listener);
      detachments.delete(detach);
    };
    detachments.add(detach);
    return detach;
  };

  const attachBrowser = browser => attach(browser, 'disconnected', 'browser-disconnected', () => ({ connected: browser.isConnected?.() ?? false }));
  const attachContext = context => attach(context, 'close', 'context-close', () => ({ pageCount: context.pages?.().length ?? null }));
  const attachPage = page => {
    const detachClose = attach(page, 'close', 'page-close', () => ({ url: page.url?.() ?? null }));
    const detachCrash = attach(page, 'crash', 'page-crash', error => ({ message: eventMessage(error), url: page.url?.() ?? null }));
    return () => { detachClose(); detachCrash(); };
  };

  return {
    setPhase,
    attachBrowser,
    attachContext,
    attachPage,
    detachAll() { for (const detach of [...detachments]) detach(); },
    snapshot() {
      const captured = events.map(event => ({ ...event }));
      return {
        targetId,
        current: { ...state },
        events: captured,
        unexpectedEvents: captured.filter(event => event.unexpected),
      };
    },
  };
}
