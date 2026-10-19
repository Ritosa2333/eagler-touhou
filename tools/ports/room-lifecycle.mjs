import assert from 'node:assert/strict';

export function readRoomWorldFrame(snapshot) {
  const state = snapshot?.observation;
  if (state?.game === 'th08' && state?.variant === 'multiplayer')
    return state.worldProgress?.counters?.enemySceneFrame;
  return state?.frames?.simulation;
}
const worldFrame=readRoomWorldFrame;

// The network clock may continue while the game's world is paused. Check the
// title-owned world clock, and keep both clocks in the evidence.
export function pausedWorldFrames(snapshots) {
  assert.ok(snapshots.length >= 2, 'Every room participant must be observed');
  return snapshots.map((snapshot, seat) => {
    assert.ok(!snapshot?.failed&&!snapshot?.error,`P${seat+1}: native room is failed`);
    const state = snapshot?.observation;
    assert.equal(state?.localSeat, seat, `P${seat + 1}: local observation belongs to another seat`);
    assert.equal(state?.pause?.active, true, `P${seat + 1}: native world has not paused`);
    assert.equal(state?.phase?.name, 'paused', `P${seat + 1}: native state is not the pause screen`);
    const frame = worldFrame(snapshot);
    assert.ok(Number.isSafeInteger(frame) && frame >= 0, `P${seat + 1}: missing native world clock`);
    if (state.game === 'th08' && state.variant === 'multiplayer') {
      const world=state.worldProgress;
      assert.equal(world?.localPlayer?.seat,seat,`P${seat+1}: th08 world belongs to another seat`);
      assert.equal(world?.state?.paused,true,`P${seat+1}: th08 world pause flag is missing`);
      assert.ok(Number.isFinite(world?.localPlayer?.position?.x)&&Number.isFinite(world?.localPlayer?.position?.y),
        `P${seat+1}: th08 paused player coordinates unavailable`);
      return {stage:world.counters.stage,enemySceneFrame:frame,position:world.localPlayer.position};
    }
    return frame;
  });
}

export function assertPausedWorldStable(before, after) {
  assert.equal(after.length, before.length, 'A room participant disappeared during pause');
  assert.deepEqual(pausedWorldFrames(after), pausedWorldFrames(before), 'A paused native world continued to simulate');
}

export function assertSynchronizedPause(snapshots){
  const worlds=pausedWorldFrames(snapshots);
  const clocks=worlds.map(world=>typeof world==='number'?world:{stage:world.stage,enemySceneFrame:world.enemySceneFrame});
  for(const clock of clocks.slice(1))
    assert.deepEqual(clock,clocks[0],'Participants paused in different stages or world frames');
}

export async function exerciseRoomLifecycle({game, pages, snapshot, step, record}) {
  const all = () => Promise.all(pages.map(async (page, seat) => {
    const {events, ...state} = await snapshot(page, game, seat);
    return state;
  }));
  const key = async code => {
    const host = pages[0];
    await host.bringToFront();
    await host.frameLocator('#gameFrame').locator('canvas').first().focus();
    await host.keyboard.press(code, {delay: 100});
  };
  const until = async (label, predicate, timeout = 30000) => {
    const deadline = Date.now() + timeout;
    let states;
    do {
      states = await all();
      record.lifecycle.lastObservation={label,states};
      if (states.some(state => state?.failed || state?.error)) throw Error(`${label}: native failure ${JSON.stringify(states)}`);
      if (predicate(states)) return states;
      await pages[0].waitForTimeout(50);
    } while (Date.now() < deadline);
    throw Error(`${label}: ${JSON.stringify(states)}`);
  };
  record.lifecycle = {checks: [], coverage: {ids: ['MP-10'], complete: false,
    scope: 'Actual host Escape pause/resume and R restart, every live participant, native world clocks and generation'}};
  await step('multiplayer-native-pause-and-resume', async () => {
    const before = await all();
    assert.ok(before.every(state => ['stage', 'match'].includes(state?.observation?.phase?.name)), 'Lifecycle requires an actual game');
    await key('Escape');
    const entry = await until('Every native world must enter pause', states => states.every(state =>
      state?.observation?.pause?.active && state?.observation?.phase?.name === 'paused'));
    // Original pause animations can settle after the first active flag.
    await pages[0].waitForTimeout(600);
    const paused = await all(); assertSynchronizedPause(paused);
    await pages[0].waitForTimeout(500);
    const held = await all(); assertPausedWorldStable(paused, held);
    await key('Escape');
    const resumed = await until('Every native world must resume and advance', states => states.every((state, seat) =>
      state?.observation?.pause?.active === false && ['stage', 'match'].includes(state?.observation?.phase?.name) &&
      worldFrame(state) > worldFrame(held[seat])));
    record.lifecycle.checks.push({id: 'synchronized-pause-resume', passed: true, before, entry, paused, held, resumed});
  });
  await step('multiplayer-native-restart-and-second-game', async () => {
    // Let the world exceed the polling window so a genuine reset is observable.
    const before = await until('World must reach the restart observation window', states => states.every(state =>
      worldFrame(state) >= 240), 30000);
    await key('Escape');
    await until('Every world must pause before R', states => states.every(state =>
      state?.observation?.pause?.active && state?.observation?.phase?.name === 'paused'));
    await pages[0].waitForTimeout(600);
    await key('KeyR');
    const reset = await until('R must reset every native world into a fresh playable game', states => states.every((state, seat) =>
      state?.observation?.pause?.active === false && ['stage', 'match'].includes(state?.observation?.phase?.name) &&
      Number.isSafeInteger(worldFrame(state)) && worldFrame(state) < worldFrame(before[seat]) &&
      (before[seat].generation === null || state.generation === before[seat].generation + 1)), 180000);
    const advanced = await until('The second game must continue on every participant', states => states.every((state, seat) =>
      worldFrame(state) > worldFrame(reset[seat]) + 30), 30000);
    record.lifecycle.checks.push({id: 'synchronized-restart-second-game', passed: true, before, reset, advanced});
    record.lifecycle.coverage.complete = true;
  });
}
