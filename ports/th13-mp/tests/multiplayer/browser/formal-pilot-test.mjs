import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {webcrypto} from 'node:crypto';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {installPilot} from './formal-pilot.mjs';
// Fault mutation for the old unconditional firing gap. The independent
// artifact review also compared against the retained prior module bytes.
const pilotSource=String(installPilot), priorGate='if (dialogue && state.lastFrame % 60 < 10)';
assert.equal(pilotSource.split(priorGate).length,2);
const baselinePilot=(0,eval)(`(${pilotSource.replace(priorGate,'if (state.lastFrame % 60 < 10)')})`);

const workspace = fileURLToPath(new URL('../../../../../../', import.meta.url));
const assets = process.env.TH13_NATIVE_ASSET_ROOT || resolve(workspace, 'artifacts/reference/raw');
const marisa = await readFile(resolve(assets, 'pl01a.sht'));
const reimu = await readFile(resolve(assets, 'pl00a.sht'));

async function decide({bytes = marisa, trance = false, serialized = false, installer = installPilot, dialogue = false, until = 3, empty = false, runMode = 1, pause = {screen:0}, title = {}, saved = false} = {}) {
  const previousDocument = globalThis.document, jobs = [], keys = [], frames = [];
  let clock = 1;
  const probe = new Int32Array(32);
  probe[0] = runMode; probe[2] = 1; probe[5] = 1;
  const seat = {seat: 0, character: 1, x: 0, y: 400, phase: 1, phase_frame: 40,
    invulnerable: 0, dead: 0, ghost: 0, seat_count: 1, extra_lives: 1, bombs: 1, power: 400};
  const hazards = {dialogue, boss_main_serial: 1,
    enemies: [{serial: 1, dead: false, flags: 0x22, body_invulnerable: 0,
      x: 0, y: 100, dx: 0, dy: 0, width: 1, height: 1, angle: 0, life: 1}],
    bullets: [{alive: true, collision_ready: true, flags: 18, scale: 1,
      width: 8, height: 8, x: -5, y: 400, vx: 0, vy: 0}], items: [], spirits: []};
  if (empty) hazards.bullets = [];
  const module = {
    FS: {readFile: () => bytes, readdir: () => saved ? ["th13mp_01.rpy"] : []},
    HEAP32: probe, HEAPU32: new Uint32Array(32), UTF8ToString: value => value,
    onGameFrame() {},
    _th13_frame: () => clock, _th13_probe_state: () => 0,
    _th13_mp_rollback_status: () => 0,
    _th13_debug_mp_net_state: () => JSON.stringify({simulating: false, capture_frame: clock}),
    _th13_debug_pause_state: () => JSON.stringify(pause),
    _th13_debug_pending: () => JSON.stringify(title),
    _th13_debug_mp_seat_state: () => JSON.stringify(seat),
    _th13_debug_mp_hazards: () => JSON.stringify(hazards),
    _th13_debug_mp_trance_state: () => JSON.stringify({active: +trance, slot: 0, owner: -1}),
    _th13_debug_mp_laser_contact: () => 0,
    _th13_key: (...args) => keys.push(args),
  };
  const window = {Module: module, crypto: {subtle: {digest(...args) {
    const promise = webcrypto.subtle.digest(...args);
    jobs.push(promise);
    return promise;
  }}}};
  globalThis.document = {querySelector: () => ({contentWindow: window})};
  try {
    // Playwright serializes this function; exercise that boundary as well.
    const install = serialized ? (0, eval)(`(${String(installer)})`) : installer;
    install({seat: 0});
    const initial={mask:window.__pilot.mask,keys:structuredClone(keys)};
    ++clock; module.onGameFrame(true, 0, 1); frames.push({clock, mask:window.__pilot.mask});
    await Promise.all(jobs);
    ++clock; module.onGameFrame(true, 0, 1); frames.push({clock, mask:window.__pilot.mask});
    while(clock < until) { ++clock; module.onGameFrame(true, 0, 1); frames.push({clock, mask:window.__pilot.mask}); }
    return {state: window.__pilot, keys, frames, initial};
  } finally {
    globalThis.document = previousDocument;
  }
}

test('role SHT estimate reaches the pilot decision through serialized installation', async () => {
  const {state, keys} = await decide({serialized: true});
  assert.equal(state.movementPrediction.mode, 'normal-sht-estimate');
  assert.equal(state.movementPrediction.sha,
    '083c815505f09bfc5cd26daa3caaa35a568d3d2f67f182aa076c08c48597fed3');
  assert.equal(state.mask, 129);
  assert.equal(state.direction, 7);
  const selected = state.movementPrediction;
  assert.equal(state.direction, (selected.dx + 1) * 3 + selected.dy + 1);
  assert.equal(Boolean(state.mask & 8), selected.shift);
  assert.ok(selected.unknown.includes('active-bomb'));
  assert.ok(keys.some(([scan, down]) => scan === 205 && down === 1));
});

test('wrong role asset uses the prior movement estimate', async () => {
  const {state} = await decide({bytes: reimu});
  assert.equal(state.movementPrediction.mode, 'fallback');
  assert.equal(state.movementPrediction.reason, 'sht-role-sha-mismatch');
  // The prior movement choice is retained; ordinary fire remains held outside dialogue.
  assert.equal(state.mask, 25);
  assert.equal(state.direction, 3);
});

test('known Trance retains fallback and exposes its limit', async () => {
  const {state} = await decide({trance: true});
  assert.equal(state.movementPrediction.mode, 'fallback');
  assert.equal(state.movementPrediction.reason, 'known-special-or-invalid-observation');
});

test('malformed SHT never enables the new estimate', async () => {
  const bytes = Buffer.from(marisa);
  bytes.writeFloatLE(NaN, 16);
  const {state} = await decide({bytes});
  assert.equal(state.movementPrediction.mode, 'fallback');
  assert.equal(state.movementPrediction.reason, 'sht-read:sht-speed-invalid');
});


test('outside dialogue the serialized pilot holds ordinary fire through all 60 frames', async () => {
  const candidate = await decide({serialized:true, empty:true, until:61});
  assert.equal(candidate.frames.length,60);
  assert.ok(candidate.frames.every(x => (x.mask & 1) === 1));
  assert.equal(candidate.keys.filter(([scan,down]) => scan===44 && down===0).length,0);
  const baseline = await decide({installer:baselinePilot, serialized:true, empty:true, until:61});
  assert.equal(baseline.frames.filter(x => !(x.mask & 1)).length,10, 'negative control must expose the prior periodic firing gap');
});

test('real dialogue observations retain the existing Z and Ctrl release/press cadence', async () => {
  const options={serialized:true,empty:true,dialogue:true,until:61};
  const candidate=await decide(options), baseline=await decide({...options,installer:baselinePilot});
  assert.deepEqual(candidate.frames,baseline.frames);
  assert.ok(candidate.frames.some(x => (x.mask & (1|512))===0));
  assert.ok(candidate.frames.some(x => (x.mask & (1|512))===(1|512)));
  for (const scan of [44,29]) {
    assert.ok(candidate.keys.some(([key,down]) => key===scan && down===0));
    assert.ok(candidate.keys.some(([key,down]) => key===scan && down===1));
  }
});


test('serialized installation preserves baseline key state', async () => {
  const candidate=await decide({serialized:true}),baseline=await decide({serialized:true,installer:baselinePilot});
  assert.deepEqual(candidate.initial,baseline.initial);
  assert.deepEqual(candidate.initial,{mask:0,keys:[]});
});

for (const [label,options] of [
 ['title',{runMode:0}],['non-game destination',{runMode:2}],
 ['pause',{pause:{screen:1,phase:0,cursor:0}}],
 ['end menu',{pause:{screen:4,phase:6,cursor:0}}],
 ['replay save menu',{pause:{screen:5,phase:17,cursor:0}}],
]) test('serialized '+label+' preserves every prior hosted key edge', async () => {
 const candidate=await decide({...options,serialized:true,until:61}),baseline=await decide({...options,serialized:true,until:61,installer:baselinePilot});
 assert.deepEqual(candidate.initial,baseline.initial);
 assert.deepEqual(candidate.frames,baseline.frames);
 assert.deepEqual(candidate.keys,baseline.keys);
});

const terminalSource = String(installPilot);
const exitSlotCondition = 'title.title_substate === 2 && saved';
assert.equal(terminalSource.split(exitSlotCondition).length, 2);
const oldTerminalPilot = (0, eval)(`(${terminalSource.replace(exitSlotCondition, 'false')})`);

for (const [label, title, saved, expected] of [
  ['ResultName with empty name', {has_title:true,title_screen:14,title_substate:2}, false, 1],
  ['ResultName with negative rank', {has_title:true,title_screen:14,title_substate:2,rank:-1}, false, 1],
  ['replay slot before save', {has_title:true,title_screen:15,title_substate:2}, false, 1],
  ['replay name before save', {has_title:true,title_screen:15,title_substate:3}, false, 1],
  ['replay name after save', {has_title:true,title_screen:15,title_substate:3}, true, 1],
  ['replay slot after save', {has_title:true,title_screen:15,title_substate:2}, true, 2],
  ['Ending even if an old save exists', {has_title:false}, true, 1],
]) test(`terminal input: ${label}`, async () => {
  const result=await decide({serialized:true,runMode:2,title,saved,until:65});
  const press = expected === 2 ? 45 : 44;
  const other = expected === 2 ? 44 : 45;
  assert.ok(result.keys.some(([scan,down]) => scan===press && down===1));
  assert.ok(result.keys.some(([scan,down]) => scan===press && down===0));
  assert.ok(!result.keys.some(([scan,down]) => scan===other && down===1));
});

test('terminal input: old pilot repeats Z on a saved replay slot', async () => {
  const options={serialized:true,runMode:2,title:{has_title:true,title_screen:15,title_substate:2},saved:true,until:65};
  const old=await decide({...options,installer:oldTerminalPilot});
  const fixed=await decide(options);
  assert.ok(old.keys.some(([scan,down]) => scan===44 && down===1));
  assert.ok(!old.keys.some(([scan,down]) => scan===45 && down===1));
  assert.ok(fixed.keys.some(([scan,down]) => scan===45 && down===1));
  assert.ok(!fixed.keys.some(([scan,down]) => scan===44 && down===1));
});
