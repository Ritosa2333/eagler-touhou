// Short lifecycle regression through the production Launcher and keyboard path.
import assert from 'node:assert/strict';
export async function runRoomLifecycle({pages, inspect, rounds = 3, evidence = []}) {
  await Promise.all(pages.map(p => p.evaluate(() => {
    // Launcher may install the Runtime as srcdoc, leaving src empty.
    window.__lifecycleDocument = document.querySelector('#gameFrame').contentDocument;
    window.__lifecycleModule = document.querySelector('#gameFrame').contentWindow.Module;
  })));
  const wait = (page, predicate, timeout = 30000) => page.waitForFunction(predicate, {}, {timeout});
  const pulse = async (page, scan) => {
    for (const down of [1, 0]) {
      const captured = await page.evaluate(({scan, down}) => {
        const m = document.querySelector('#gameFrame').contentWindow.Module;
        const frame = JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state())).capture_frame;
        m._th13_key(scan, down);
        return frame;
      }, {scan, down});
      await page.waitForFunction(captured => {
        const m = document.querySelector('#gameFrame').contentWindow.Module;
        return JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state())).capture_frame > captured;
      }, captured);
    }
  };
  const paused = () => {
    const m = document.querySelector('#gameFrame').contentWindow.Module;
    const p = JSON.parse(m.UTF8ToString(m._th13_debug_pause_state()));
    return p.screen === 1 && p.phase === 6;
  };
  // The room root uses display:contents; its visible header and Ready button
  // prove that the returned room is actually presented and usable.
  const room = () => !!document.querySelector('#mpRoomView .room-workspace-header')?.getClientRects().length &&
    !!document.querySelector('#mpReady:not(:disabled)')?.getClientRects().length;
  for (let round = 0; round < rounds; round++) {
    console.log('lifecycle round', round);
    await Promise.all(pages.map(page => wait(page, () => {
      const m = document.querySelector('#gameFrame').contentWindow.Module;
      const net = JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state()));
      if (net.error) throw Error(net.error);
      return m._th13_debug_stage_frame() >= 120;
    })));
    // P2 pauses both worlds, then resumes both. Network frames keep advancing.
    await pulse(pages[1], 1);
    await Promise.all(pages.map(page => wait(page, paused)));
    const before = await Promise.all(pages.map(inspect));
    await new Promise(r => setTimeout(r, 300));
    const after = await Promise.all(pages.map(inspect));
    for (let seat = 0; seat < 2; seat++) {
      assert.equal(after[seat].runtime.stageFrame, before[seat].runtime.stageFrame, 'Pause freezes battle');
      assert(after[seat].runtime.net.sim_frame > before[seat].runtime.net.sim_frame, 'Pause keeps network input live');
      assert(after[seat].runtime.net.capture_frame - after[seat].runtime.net.sim_frame <= 1,
        'Pause retains one pending local sample without accumulating artificial delay');
      assert.equal(after[seat].nativeError, '');
    }
    await pulse(pages[1], 1);
    await Promise.all(pages.map(page => wait(page, () => {
      const m = document.querySelector('#gameFrame').contentWindow.Module;
      return !JSON.parse(m.UTF8ToString(m._th13_debug_pause_state())).screen;
    })));
    // P2's Q shortcut selects Return to title through the ordinary pause menu.
    await pulse(pages[1], 1);
    await Promise.all(pages.map(page => wait(page, paused)));
    await pulse(pages[1], 16);
    await Promise.all(pages.map(page => wait(page, room)));
    const returned = await Promise.all(pages.map(inspect));
    for (let seat = 0; seat < 2; seat++) {
      const state = returned[seat];
      assert(await pages[seat].evaluate(() =>
        document.querySelector('#gameFrame').contentDocument === window.__lifecycleDocument &&
        document.querySelector('#gameFrame').contentWindow.Module === window.__lifecycleModule),
      'Room retains the Runtime document and engine');
      assert.equal(state.runtime.net.active, 0, 'Old game transport retired');
      assert.equal(state.nativeError, '');
      const saved = await pages[seat].evaluate(() => {
        const m = document.querySelector('#gameFrame').contentWindow.Module;
        return m.FS.readdir('/save/replay').filter(n => /^th13mp_\d{2}\.rpy$/.test(n))
          .map(n => ({name:n, bytes:m.FS.stat('/save/replay/'+n).size}));
      });
      assert(saved.length >= round + 1 && saved.every(file => file.bytes > 128), 'Each run saved to its own replay slot');
      evidence.push({round, seat, returned: state, saved});
    }
    if (round + 1 === rounds) break;
    for (const page of pages) {
      if (await page.locator('#mpReady').getAttribute('aria-pressed') !== 'true')
        await page.locator('#mpReady').click();
      await wait(page, () => JSON.parse(sessionStorage.getItem('eagler-touhou-th13mp-room-v1') || '{}').ready === true);
      await page.evaluate(() => document.querySelector('#gameFrame').contentWindow.Module._th13_mp_audit(1));
    }
    await pages[0].locator('#mpStartGame').click();
    await Promise.all(pages.map(page => wait(page, () => {
      const m = document.querySelector('#gameFrame').contentWindow.Module;
      return JSON.parse(m.UTF8ToString(m._th13_debug_mp_net_state())).sim_frame > 0;
    })));
  }
  return evidence;
}
