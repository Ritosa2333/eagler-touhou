import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
import { readCoreSoundEvents } from '../src/sound-events.mjs';

const native = JSON.parse(fs.readFileSync(new URL('fixtures/message-enemy-clear-original.json', import.meta.url)));
const exeSha = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417';
function state(core) {
  const at = core._th12_state(), size = core._th12_state_size();
  return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(at, at + size)));
}
function load(core, bytes, method, index = null) {
  const at = core._malloc(bytes.length);
  try {
    core.HEAPU8.set(bytes, at);
    assert.equal(index === null ? method(at, bytes.length) : method(index, at, bytes.length), 1);
  } finally { core._free(at); }
}
function deathAnimations(snapshot) {
  const nodes = new Map(snapshot.animations.nodes.map(node => [node.id, node]));
  return snapshot.animations.chains[0].map(id => nodes.get(id))
    .filter(node => [84, 85, 86, 93].includes(node.script));
}
function sameFloats(actual, expected, label) {
  assert.equal(actual.length, expected.length, label + ' size');
  // Same float precision scope as the existing164 death-ANM fixture checks.
  // RNG, selection, list order, scripts and clocks remain exact assertions.
  actual.forEach((value, index) => assert.ok(Math.abs(value - expected[index]) < 1e-6,
    label + '[' + index + '] ' + value + ' != ' + expected[index]));
}

test('actual414c60 repeats marked death visuals, honors protection/force and emits no death sound', () => {
  assert.equal(native.originalExeSha256, exeSha);
  for (const row of native.isolated) {
    const protectedEnemy = !!(row.initialFlags & 0x6004a0) && !(row.initialFlags & 0x100);
    const rootBinds = row.actualBindCalls.filter(call => call.caller === '0x414d6c');
    assert.equal(rootBinds.length, !protectedEnemy && row.deathScript >= 0 ? row.enemyCount * 2 : 0, row.name);
    assert.equal(row.actualBindCalls.some(call => call.entry === '0x453e20'), false, row.name + ' actual sound call');
    if (!protectedEnemy && row.deathScript >= 0) {
      const second = rootBinds.filter(call => call.pass === 2);
      assert.ok(second.every(call => call.enemyFlagsBefore & 0x1000000), row.name + ' marked second pass');
    }
    for (let pass = 1; pass <= 2; pass++) {
      assert.equal(row.frames[pass].gameSeedCounter[1], protectedEnemy || row.deathScript < 0 ? 0 : row.enemyCount * 58 * pass, row.name + ' original RNG');
      assert.deepEqual(row.frames[pass].enemyFlags, Array(row.enemyCount).fill(row.initialFlags | (protectedEnemy ? 0 : 0x1000000)), row.name + ' selection');
    }
  }
});

test('production ECL418 performs both original MSG-constructor and dispatcher enemy clears', async () => {
  for (const row of native.integration) {
    const core = await createCore();
    for (const [name, index] of [['bullet.anm', 0], ['front.anm', 7], ['text.anm', 8]])
      load(core, fs.readFileSync(new URL('../local/retail/' + name, import.meta.url)), core._th12_load_anm, index);
    load(core, Buffer.from(row.syntheticEclHex, 'hex'), core._th12_load_ecl);
    load(core, Buffer.from(row.syntheticMsgHex, 'hex'), core._th12_load_msg, row.character * 2 + row.shot);
    core._th12_start(row.character, row.shot, row.difficulty, row.seed);
    const clearEntries = row.actualEntryCalls.filter(call => call.entry === '0x414c60');
    assert.deepEqual(clearEntries.map(call => call.caller), ['0x41fbd1', '0x418838'], row.name + ' actual calls');
    assert.equal(row.actualEntryCalls.some(call => call.entry === '0x453e20'), false, row.name + ' original silent clear');
    for (const frame of row.frames) {
      if (frame.inputTick >= 0) core._th12_tick(0, 0);
      const snapshot = state(core), label = row.name + ' input' + frame.inputTick;
      assert.deepEqual([snapshot.rng.seed, snapshot.rng.calls], frame.gameSeedCounter, label + ' original game RNG');
      assert.equal(!!snapshot.message.active, frame.messageActive, label + ' actual MSG');
      assert.equal(snapshot.enemies.length, frame.enemies.length, label + ' live linked actors');
      for (let i = 0; i < frame.enemies.length; i++) {
        assert.equal(snapshot.enemies[i].flags, frame.enemies[i].flags, label + ' enemy flags' + i);
        sameFloats(snapshot.enemies[i].position, frame.enemies[i].position.slice(0, 2), label + ' enemy position' + i);
      }
      const death = deathAnimations(snapshot);
      assert.equal(death.length, frame.deathAnimations.length, label + ' complete death child factories');
      death.forEach((node, index) => {
        const expected = frame.deathAnimations[index], item = label + ' death VM' + index;
        assert.equal(node.script, expected.script, item + ' script/order');
        assert.equal(node.clock, expected.clock, item + ' clock');
        assert.equal(node.render[5], expected.layer, item + ' layer');
        assert.equal(!!node.parent, expected.attached, item + ' attached');
        sameFloats(node.pose.slice(0, 3), expected.position, item + ' script XYZ');
        const engine = node.pose.slice(3, 6);
        // Native root+430 includes224/16. Scene stores the same viewport
        // separately; attached children retain local engineXYZ in both forms.
        if (!node.parent) { engine[0] += node.viewport[0]; engine[1] += node.viewport[1]; }
        sameFloats(engine, expected.engine, item + ' engine XYZ');
        sameFloats(node.pose.slice(12, 14), expected.scale, item + ' scale');
      });
      assert.equal(readCoreSoundEvents(core).some(event => event.id === 3 || event.id === 4), false, label + ' no enemy death sound');
      assert.deepEqual(snapshot.runtimeFaults, [], label + ' runtime faults');
      assert.deepEqual(snapshot.referenceFaults, [], label + ' ECL operand faults');
    }
  }
});
