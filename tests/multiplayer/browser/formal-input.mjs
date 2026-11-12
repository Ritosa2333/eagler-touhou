// Short taps enter through the hosted production keyboard, with no manual
// world pumping. The audit proves both edges survive real network correction.
import assert from 'node:assert/strict';

export async function runInputEdges({pages, inspect, evidence}) {
  const histories = pages.map(() => []), timings = pages.map(() => []);
  let collection = Promise.resolve();
  const collect = () => collection = collection.then(async () => {
    const states = await Promise.all(pages.map(inspect));
    states.forEach((state, seat) => {
      assert.equal(state.nativeError, '');
      assert.equal(state.runtime.error, '');
      assert.equal(state.runtime.audit.overflow, 0);
      assert.equal(state.runtime.timing.overflow, 0);
      histories[seat].push(...state.runtime.audit.frames);
      timings[seat].push(state.runtime.timing);
    });
    return states;
  });
  await collect();
  await Promise.all(pages.map(page => page.evaluate(() => {
    const w = document.querySelector('#gameFrame').contentWindow, m = w.Module;
    const old = m.onGameFrame;
    let direction = 0;
    m._th13_key(44, 1);
    m.onGameFrame = (ok, ms, ticks) => {
      old?.(ok, ms, ticks);
      if (!ok) return;
      const state = JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state()));
      const correcting = m.HEAPU32[(m._th13_mp_rollback_status() >>> 2) + 7];
      if (correcting && !w.__inputEdgesTap) {
        // Both quick taps occur inside a budget-yielded real correction.
        const clock = performance.now();
        for (const down of [1, 0, 1, 0]) m._th13_key(42, down);
        m._th13_key(200, 1); m._th13_key(200, 0);
        w.__inputEdgesTap = {frame: state.capture_frame, clock, scan: 42,
          downs: [1, 0, 1, 0], correcting: true, state};
      }
      // Keep producing remote changes after this seat's tap so the other
      // seat still has corrections to observe, even when its first replays
      // finish within the normal production work budget.
      const next = state.capture_frame % 12 < 6 ? 203 : 205;
      if (next !== direction) {
        if (direction) m._th13_key(direction, 0);
        m._th13_key(next, 1); direction = next;
      }
    };
    w.__inputEdgesStop = () => {
      m.onGameFrame = old;
      m._th13_key(44, 0);
      if (direction) m._th13_key(direction, 0);
      delete w.__inputEdgesStop;
    };
  })));
  // Both diagnostic streams are bounded. Drain while waiting for the other
  // seat's actual budget yield, preserving all confirmed history in order.
  let drainError;
  const timer = setInterval(() => { collect().catch(error => { drainError = error; }); }, 100);
  try {
    for (let seat = 0; seat < pages.length; ++seat) {
      // Wait for an actual restore. Capture and simulation are observed, never
      // edited; the configured transport disturbance creates the correction.
      await pages[seat].waitForFunction(() =>
        document.querySelector('#gameFrame').contentWindow.__inputEdgesTap,
      {}, {timeout: 30000});
      const tap = await pages[seat].evaluate(() =>
        document.querySelector('#gameFrame').contentWindow.__inputEdgesTap);
      evidence.push({seat, tap});
      await pages[seat].waitForFunction(frame => {
        const m = document.querySelector('#gameFrame').contentWindow.Module;
        const state = JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state()));
        return state.confirmed >= frame + 8 && state.sim_frame > frame + 8;
      }, tap.frame, {timeout: 30000});
      await collect();
    }
    await new Promise(resolve => setTimeout(resolve, 250));
    const states = await collect();
    if (drainError) throw drainError;
    const common = Math.min(...histories.map(rows => rows.length));
    assert(common > 12, 'Confirmed correction coverage');
    assert(common >= Math.max(...evidence.map(row => row.tap.frame + 4)),
      'Every tap frame is covered by the full all-seat comparison');
    for (let frame = 0; frame < common; ++frame)
      for (let seat = 0; seat < pages.length; ++seat) {
        assert.equal(histories[seat][frame].frame, frame);
        assert.deepEqual(histories[seat][frame], histories[0][frame]);
      }
    for (const row of evidence) {
      const {seat, tap} = row, local = histories[seat];
      assert.deepEqual(local.slice(tap.frame, tap.frame + 4).map(frame => +(!!(frame.inputs[seat] & 8))),
        tap.downs, 'Both focus tap edges reach consecutive authoritative inputs');
      assert.equal(local[tap.frame].inputs[seat] & 16, 16);
      assert.equal(local[tap.frame + 1].inputs[seat] & 16, 0);
      row.confirmedInputs = local.slice(tap.frame, tap.frame + 4);
      row.rollback = states[seat].runtime.rollbackMetrics;
      assert(row.rollback.restores > 0, 'A real correction occurred');
    }
    return {histories, timings, states, common};
  } finally {
    clearInterval(timer);
    await collection.catch(() => {});
    await Promise.all(pages.map(page => page.evaluate(() =>
      document.querySelector('#gameFrame').contentWindow.__inputEdgesStop())));
  }
}
