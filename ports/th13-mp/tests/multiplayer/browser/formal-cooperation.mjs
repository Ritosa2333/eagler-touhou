// Directed cooperation through the production Launcher keyboard bridge.
// The caller supplies three started pages before installing the combat pilot.
import assert from 'node:assert/strict';

export function installGiftGesture({ donor = 0 }) {
  const w = document.querySelector('#gameFrame').contentWindow, m = w.Module;
  const read = f => JSON.parse(m.UTF8ToString(f()));
  const state = w.__cooperativeGesture = {
    donor, commands: [], presses: 0, mask: 0, finished: false,
    startedCapture: read(m._th13_debug_mp_net_state).capture_frame,
  };
  const release = () => {
    for (const scan of [44, 45, 42, 200, 208, 203, 205, 29, 46, 28, 1])
      m._th13_key(scan, 0);
  };
  release();
  let changedAt = state.startedCapture;
  const previous = m.onGameFrame;
  const callback = (ok, ms, ticks) => {
    previous?.(ok, ms, ticks);
    if (!ok || state.finished) return;
    const net = read(m._th13_debug_mp_net_state), capture = net.capture_frame;
    if (capture - changedAt < 2) return;
    const mask = state.mask ? 0 : state.presses < 5 ? 8 : 0;
    if (mask) ++state.presses;
    m._th13_key(42, +!!mask);
    state.mask = mask;
    state.commands.push({ capture, sim: net.sim_frame,
      stageFrame: m._th13_debug_stage_frame(), hostFrame: m._th13_frame(), mask });
    changedAt = capture;
    if (!mask && state.presses === 5) {
      state.finished = true;
      state.finishedCapture = capture;
      if (m.onGameFrame === callback) m.onGameFrame = previous;
    }
  };
  m.onGameFrame = callback;
  w.__stopCooperativeGesture = () => {
    state.finished = true;
    release();
    if (m.onGameFrame === callback) m.onGameFrame = previous;
  };
}

export function assertDirectedGift({ before, after, commands, inputRows, donor = 0 }) {
  assert.equal(before.length, 3, 'Directed gift requires three real seats');
  assert.equal(after.length, 3);
  const origin = before[donor];
  assert(origin.extra_lives > 0 && !origin.ghost && !origin.dead && origin.phase === 1,
    'Donor starts active with a spare life');
  const candidates = before.filter(s => s.seat !== donor && !s.dead && !s.ghost &&
    s.phase === 1 && s.extra_lives < 8 && Math.hypot(s.x - origin.x, s.y - origin.y) <= 20)
    .sort((a, b) => a.extra_lives - b.extra_lives || a.seat - b.seat);
  assert.equal(candidates.length, 2, 'Both other seats qualify in the real world');
  const recipient = candidates[0].seat;
  for (let seat = 0; seat < 3; ++seat) {
    assert.equal(after[seat].extra_lives,
      before[seat].extra_lives + (seat === donor ? -1 : seat === recipient ? 1 : 0),
      `Seat ${seat} receives exactly its transaction quantity`);
    assert.equal(after[seat].deaths, before[seat].deaths, 'No death confounds this transaction');
    assert.equal(after[seat].ghost, before[seat].ghost);
  }
  assert.equal(commands.filter(row => row.mask === 8).length, 5, 'Five real key presses');
  assert.equal(commands.at(-1).mask, 0, 'Final key released');
  assert(inputRows.length >= 12, 'Confirmed input history covers the entire gesture');
  let presses = 0, held = false, previousPress = -1;
  for (let index = 0; index < inputRows.length; ++index) {
    const row = inputRows[index];
    if (index) assert.equal(row.frame, inputRows[index - 1].frame + 1, 'Continuous input history');
    for (let seat = 0; seat < 3; ++seat)
      assert.equal(row.buttons[seat] & ~8, 0, 'Only production Shift input in the directed interval');
    const down = !!(row.buttons[donor] & 8);
    if (down && !held) {
      if (previousPress >= 0) assert(row.frame - previousPress <= 24, 'Tap interval <=24 logical frames');
      previousPress = row.frame; ++presses;
    }
    held = down;
    for (let seat = 0; seat < 3; ++seat)
      if (seat !== donor) assert.equal(row.buttons[seat], 0, 'Other devices remain released');
  }
  assert.equal(presses, 5, 'Five confirmed press edges, no duplicated edges');
  assert(!held, 'Confirmed release follows fifth press');
  return { donor, recipient, presses, firstInput: inputRows[0].frame,
    lastInput: inputRows.at(-1).frame };
}

export async function runDirectedGift({ pages, evidence = [] }) {
  assert.equal(pages.length, 3, 'Use TH13_PLAYERS=3');
  const snapshot = page => page.evaluate(() => {
    const w = document.querySelector('#gameFrame').contentWindow, m = w.Module;
    const read = f => JSON.parse(m.UTF8ToString(f()));
    return { seats: Array.from({ length: 3 }, (_, seat) => read(() => m._th13_debug_mp_seat_state(seat))),
      net: read(m._th13_debug_mp_net_state), diagnostic: read(m._th13_mp_diagnostic_report),
      stageFrame: m._th13_debug_stage_frame(), error: m.UTF8ToString(m._th13_error()),
      gesture: w.__cooperativeGesture };
  });
  for (const page of pages)
    await page.waitForFunction(() => {
      const m = document.querySelector('#gameFrame').contentWindow.Module;
      const seats = [0, 1, 2].map(seat => JSON.parse(m.UTF8ToString(m._th13_debug_mp_seat_state(seat))));
      return seats.every(s => s.phase === 1 && !s.dead && !s.ghost);
    });
  await Promise.all(pages.map(page => page.evaluate(() => {
    const m = document.querySelector('#gameFrame').contentWindow.Module;
    for (const scan of [44, 45, 42, 200, 208, 203, 205, 29, 46, 28, 1]) m._th13_key(scan, 0);
  })));
  const before = await Promise.all(pages.map(snapshot));
  try {
  await pages[0].evaluate(installGiftGesture, { donor: 0 });
  await pages[0].waitForFunction(() =>
    document.querySelector('#gameFrame').contentWindow.__cooperativeGesture?.finished,
  {}, { timeout: 15000 });
  const driver = (await snapshot(pages[0])).gesture;
  await Promise.all(pages.map(page => page.waitForFunction(capture => {
    const m = document.querySelector('#gameFrame').contentWindow.Module;
    const net = JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state()));
    return net.confirmed !== 0xffffffff && net.confirmed >= capture + 12;
  }, driver.finishedCapture, { timeout: 15000 })));
  const after = await Promise.all(pages.map(snapshot));
  const first = driver.startedCapture, last = Math.min(...after.map(x =>
    x.diagnostic.confirmedInputs.at(-1)?.frame ?? -1));
  const interval = state => state.diagnostic.confirmedInputs.filter(row => row.frame >= first && row.frame <= last);
  const reference = interval(after[0]);
  assert.equal(reference[0]?.frame, first, 'Gesture start remains in confirmed history');
  assert(last >= driver.finishedCapture + 8, 'Confirmed input reaches beyond final release');
  const results = after.map((state, observer) => {
    assert.equal(state.error, '', 'Native application error');
    assert.equal(state.net.error, '', 'Native network error');
    assert(state.stageFrame < 1000, 'Early natural start avoids later reward/death confounds');
    assert.deepEqual(interval(state), reference, 'All devices confirm identical gesture input');
    return { observer, ...assertDirectedGift({ before: before[observer].seats,
      after: state.seats, commands: driver.commands, inputRows: reference }) };
  });
  const row = { scope: 'Three-player natural-start directed life gift', before, after,
    driver, inputRows: reference, results };
  evidence.push(row);
  return row;
  } finally {
    await pages[0].evaluate(() => {
      document.querySelector('#gameFrame').contentWindow.__stopCooperativeGesture?.();
    });
  }
}
