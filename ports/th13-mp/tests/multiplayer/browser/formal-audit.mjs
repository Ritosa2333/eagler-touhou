import assert from 'node:assert/strict';
import {appendFile} from 'node:fs/promises';

// Runs in each runtime frame, before the first audit enable. The native rings
// have one consumer; batches remain queued until the host persists and acks.
export function installFormalAudit() {
  globalThis.__installFormalAudit = module => {
    if (!module || module.__formalAuditInstalled) return;
    module.__formalAuditInstalled = true;
    const state = globalThis.__formalAudit = {batches: [], next: 1, errors: [], terminal: null,
      calls: 0, elapsedMs: 0};
    state.drain = () => {
      if (!module._th13_mp_audit_take || !module._th13_mp_timing_take) return;
      const start = performance.now();
      try {
        const audit = JSON.parse(module.UTF8ToString(module._th13_mp_audit_take()));
        const timing = JSON.parse(module.UTF8ToString(module._th13_mp_timing_take()));
        if (!Array.isArray(audit.frames) || !Array.isArray(timing.events)) throw Error('Malformed native audit');
        const terminal = audit.terminal && !state.terminal ? audit.terminal : null;
        if (terminal) state.terminal = JSON.stringify(terminal);
        else if (audit.terminal && state.terminal !== JSON.stringify(audit.terminal))
          throw Error('Terminal audit changed');
        audit.terminal = terminal;
        if (audit.frames.length || timing.events.length || audit.overflow || timing.overflow || terminal)
          state.batches.push({sequence: state.next++, audit, timing});
      } catch (error) { state.errors.push(String(error)); }
      finally { ++state.calls; state.elapsedMs += performance.now() - start; }
    };
    const oldFrame = module.onGameFrame;
    module.onGameFrame = (...args) => { state.drain(); oldFrame?.(...args); };
    state.snapshot = () => ({batches: state.batches.slice(), errors: state.errors.slice(),
      calls: state.calls, elapsedMs: state.elapsedMs});
    state.ack = (first, last, count) => {
      if (!count || state.batches[0]?.sequence !== first || state.batches[count - 1]?.sequence !== last)
        throw Error('Audit batch changed before persistence acknowledgement');
      state.batches.splice(0, count);
    };
  };
}

// Persist raw observations before validating. A subsequent failure flush may
// see unacknowledged batches again; rawPending prevents writing them twice.
export async function persistAuditCapture(path, capture, cursor) {
  const fresh = capture.batches.slice(cursor.rawPending);
  if (fresh.length) {
    await appendFile(path, fresh.map(row => JSON.stringify(row) + '\n').join(''));
    cursor.rawPending += fresh.length;
  }
  if (capture.errors.length && !cursor.errorsWritten) {
    await appendFile(path, JSON.stringify({drainErrors: capture.errors}) + '\n');
    cursor.errorsWritten = true;
  }
  assert.deepEqual(capture.errors, [], 'Audit drain errors');
  for (const batch of capture.batches) {
    assert.equal(batch.sequence, cursor.next++, 'Missing or duplicate audit batch');
    assert.equal(batch.audit.overflow, 0, 'Audit overflow');
    assert.equal(batch.timing.overflow, 0, 'Timing overflow');
  }
}
