import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import crypto from 'node:crypto';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, 'artifacts/dsh-postboss-review');
const fixturePath = path.join(root, 'local/research/postboss-event-fixtures.json');
const transitionPath = path.join(root, 'local/research/postboss-transition-fixtures.json');
const PINNED = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');

function build(output, cache, temporary) {
  const env = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: cache.replaceAll('\\', '/'), EMSDK_PYTHON: python, PYTHONUTF8: '1',
    EMCC_CORES: '2', BINARYEN_CORES: '2', TEMP: temporary, TMP: temporary, TMPDIR: temporary};
  return spawnSync(python, [compiler, 'tests/native/postboss-event-rules.cpp',
    'src/game/PostBossEventRules.cpp', '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
    '-fexceptions', '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined',
    '-fno-sanitize-recover=all', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1',
    '-sALLOW_MEMORY_GROWTH=1', '-o', output], {cwd: root, env, encoding: 'utf8', timeout: 180000});
}

function parse(text) {
  const blocks = [], lines = text.trim().split('\n');
  for (let index = 0; index < lines.length;) {
    const first = lines[index++].split(' ');
    assert.equal(first[0], 'PLAN', `unexpected plan line: ${lines[index - 1]}`);
    const block = {status: first[1], reason: '', bonus: 0, ops: [], writes: [], effects: []};
    for (; lines[index] !== 'END'; ++index) {
      const parts = lines[index].split(' ');
      if (parts[0] === 'REASON') block.reason = parts.slice(1).join(' ');
      else if (parts[0] === 'BONUS') block.bonus = Number(parts[1]);
      else if (parts[0] === 'OP') block.ops.push({kind: parts[1], address: parts[2],
        a: Number(parts[3]), b: Number(parts[4]), c: Number(parts[5]), receiverBase: parts[6],
        receiverOffset: Number(parts[7]), outputSlotOffset: Number(parts[8]), sequence: Number(parts[9])});
      else if (parts[0] === 'WRITE') block.writes.push({target: parts[1], offset: Number(parts[2]),
        mode: parts[3], value: Number(parts[4]), cap: Number(parts[5]), sequence: Number(parts[6])});
      else if (parts[0] === 'EFFECT') block.effects.push({kind: parts[1], index: Number(parts[2]),
        sequence: Number(parts[3]), afterOperation: Number(parts[4])});
      else assert.fail(`unknown driver line: ${lines[index]}`);
    }
    ++index;
    blocks.push(block);
  }
  return blocks;
}

function execute(cases) {
  const input = cases.map(case_ => `CASE ${case_.opcode} ${case_.stage} ${case_.stats} ${case_.difficulty} `
    + `${case_.rateA} ${case_.rateB} ${case_.messageTimer} ${case_.messageField78} ${case_.playCount} `
    + `${case_.suspend} ${case_.sessionPaused} ${case_.flowGate} ${case_.flowModeOne}`).join('\n') + '\n';
  const run = spawnSync(process.execPath, [output], {input, cwd: root, encoding: 'utf8', timeout: 60000});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  return {blocks: parse(run.stdout), stdout: run.stdout};
}

const kinds = block => block.ops.map(operation => operation.kind).sort();
const write = (block, target, offset) => block.writes.find(item => item.target === target && item.offset === offset);

function checkEffects(block) {
  assert.equal(block.effects.length, block.ops.length + block.writes.length, 'every effect is listed once');
  const operations = new Set(), writes = new Set();
  block.effects.forEach((effect, position) => {
    assert.equal(effect.sequence, position, 'effects carry a dense global sequence');
    if (effect.kind === 'operation') {
      const operation = block.ops[effect.index];
      assert.ok(operation, `operation effect ${effect.index} resolves`);
      assert.equal(operation.sequence, position, 'operation sequence matches its effect');
      assert.equal(operations.has(effect.index), false, 'operation effect listed once');
      operations.add(effect.index);
    } else {
      assert.equal(effect.kind, 'write', `unknown effect kind ${effect.kind}`);
      const item = block.writes[effect.index];
      assert.ok(item, `write effect ${effect.index} resolves`);
      assert.equal(item.sequence, position, 'write sequence matches its effect');
      assert.equal(writes.has(effect.index), false, 'write effect listed once');
      writes.add(effect.index);
    }
    if (effect.afterOperation !== 0xffffffff) {
      const prerequisite = block.ops[effect.afterOperation];
      assert.ok(prerequisite, `required operation ${effect.afterOperation} resolves`);
      assert.ok(prerequisite.sequence < position, 'effect follows its required operation');
    }
  });
  assert.equal(operations.size, block.ops.length);
  assert.equal(writes.size, block.writes.length);
}

function operationEffect(block, kind) {
  return block.effects.find(effect => effect.kind === 'operation' && block.ops[effect.index].kind === kind);
}

function writeEffect(block, target, offset, occurrence = 0) {
  const matches = block.effects.filter(effect => effect.kind === 'write'
    && block.writes[effect.index].target === target && block.writes[effect.index].offset === offset);
  return matches[occurrence];
}

function applied(block) {
  const state = {flags5a04: 0, bonus5a38: 0, field5a3c: 0, stage: undefined, stageMirror: undefined, nextStageRecord: undefined};
  for (const item of block.writes) {
    if (item.target === 'play-count') {
      assert.equal(item.mode, 'increment-capped');
      continue;
    }
    const slot = item.target === 'game-manager'
      ? (item.offset === 0x5a04 ? 'flags5a04' : item.offset === 0x5a38 ? 'bonus5a38' : item.offset === 0x5a3c ? 'field5a3c' : null)
      : item.target === 'stage' ? 'stage' : item.target === 'stage-mirror' ? 'stageMirror'
      : item.target === 'next-stage-record' ? 'nextStageRecord' : null;
    assert.ok(slot, `unexpected rule write target ${item.target}+0x${item.offset.toString(16)}`);
    if (item.mode === 'or-mask') state[slot] |= item.value;
    else if (item.mode === 'assign') state[slot] = item.value;
    else if (item.mode === 'add') state[slot] = (state[slot] ?? 0) + item.value;
    else assert.fail(`unexpected rule write mode ${item.mode}`);
  }
  return state;
}

const floatBits = value => new Uint32Array(new Float32Array([value]).buffer)[0];

const fixture = JSON.parse(fs.readFileSync(fixturePath, 'utf8'));
const transition = JSON.parse(fs.readFileSync(transitionPath, 'utf8'));
const output = path.join(work, 'postboss-event-rules.cjs');

test('original post-Boss MSG21/22/30 scalars match the bounded rule plan', () => {
  assert.ok(fs.existsSync(compiler), `emscripten compiler missing: ${compiler}`);
  assert.ok(fs.existsSync(python), `emscripten python missing: ${python}`);
  fs.mkdirSync(work, {recursive: true});
  const cache = path.join(work, 'emcache');
  if (!fs.existsSync(cache)) {
    const shared = path.resolve(root, '../../work/emscripten-cache-4.0.22');
    assert.ok(fs.existsSync(shared), `shared emscripten cache missing: ${shared}`);
    fs.cpSync(shared, cache, {recursive: true,
      filter: source => !path.basename(source).endsWith('.lock')});
  }
  const temporary = path.join(work, 'temp');
  fs.mkdirSync(temporary, {recursive: true});
  const buildRun = build(output, cache, temporary);
  assert.ifError(buildRun.error);
  assert.equal(buildRun.status, 0, buildRun.stdout + buildRun.stderr);

  assert.equal(fixture.schema, 'th128-postboss-event-fixtures/1');
  assert.equal(fixture.sourceExeSha256, PINNED, 'fixtures must come from the pinned original executable');
  assert.equal(fixture.apiReturnsAssigned, false);
  assert.equal(fixture.launchesOriginalExecutable, false);
  assert.match(fixture.msg.sha256, /^[0-9a-f]{64}$/);
  const probeSource = fs.readFileSync(path.join(root, 'tools/research/postboss_event_probe.py'));
  assert.equal(crypto.createHash('sha256').update(probeSource).digest('hex'), fixture.generatorSha256,
    'fixtures must be regenerated after a probe change');
  const messageSource = fs.readFileSync(path.join(root, fixture.msg.path));
  assert.equal(crypto.createHash('sha256').update(messageSource).digest('hex'), fixture.msg.sha256,
    'fixtures must describe the current original stage MSG');
  assert.equal(messageSource.length, fixture.msg.bytes);
  assert.deepEqual(fixture.msg.entry1TerminalOrder, [30, 21, 22], 'post-Boss terminal order');
  assert.equal(fixture.msg.entry0HasTerminalEvent, false);
  assert.equal(fixture.dispatch.handlers['21'], '0x0042454e');
  assert.equal(fixture.dispatch.handlers['22'], '0x004247f6');
  assert.equal(fixture.dispatch.handlers['30'], '0x0042486a');
  for (const run of fixture.dispatch.emulatedDispatch)
    assert.equal(run.stop.address, fixture.dispatch.handlers[String(run.opcode)],
      `dispatch of opcode ${run.opcode} must jump to its table handler`);

  assert.equal(transition.schema, 'th128-postboss-transition-probe/1');
  assert.equal(transition.sourceExeSha256, fixture.sourceExeSha256);
  assert.equal(transition.checks.staticOnly, true);
  assert.equal(transition.checks.apiReturnsAssigned, false);
  assert.equal(transition.checks.dispatchBytesSha256, fixture.slices.dispatch.sha256);
  for (const opcode of ['21', '22']) {
    assert.equal(transition.checks.handlers[opcode], fixture.dispatch.handlers[opcode]);
    assert.equal(transition.handlers[opcode].sha256, fixture.slices[`handler${opcode}`].sha256,
      `handler ${opcode} must match the independent static probe`);
  }
  assert.equal(transition.checks.handlers['30'], fixture.dispatch.handlers['30']);
  assert.equal(transition.handlers['30'].start, fixture.slices.handler30.start);
  assert.equal(transition.handlers['30'].endExclusive, '0x004248be');
  assert.equal(transition.constants.msg22_fast.float, fixture.constants.floatAt4a674c);
  assert.equal(transition.constants.msg22_slow.float, fixture.constants.floatAt4a682c);
  const transitionSource = fs.readFileSync(path.join(root, 'tools/research/postboss_transition_probe.py'));
  assert.equal(crypto.createHash('sha256').update(transitionSource).digest('hex'),
    transition.generatorSha256, 'transition fixtures must be regenerated after a probe change');
  const timeScaleCases = fixture.timeScaleCases.map(item => ({opcode: 22, stage: item.stage, stats: item.stage,
    difficulty: 0, rateA: 0, rateB: 0, messageTimer: 0, messageField78: 0, playCount: 0,
    suspend: 0, sessionPaused: 0, flowGate: 0, flowModeOne: 0}));
  const timeScale = execute(timeScaleCases).blocks;
  for (const [index, block] of timeScale.entries()) {
    checkEffects(block);
    const item = fixture.timeScaleCases[index];
    const expected = item.stageClamped ? fixture.constants.floatAt4a682c : fixture.constants.floatAt4a674c;
    assert.equal(item.pushedFloat, expected, `probe stage ${item.stage} constant`);
    assert.equal(block.status, 'requires-owner');
    assert.deepEqual(kinds(block), ['time-scale']);
    assert.equal(block.ops[0].address, '0x00434a20');
    assert.equal(block.ops[0].a, floatBits(expected));
    assert.equal(block.writes.length, 0, 'MSG22 must not write owner fields');
    assert.equal(block.effects.length, 1);
    assert.equal(block.effects[0].kind, 'operation');
    assert.equal(block.effects[0].afterOperation, 0xffffffff);
  }
  const cutInCases = fixture.cutInCases.map(item => ({opcode: 30, stage: 1, stats: 1, difficulty: 1,
    rateA: 0, rateB: 0, messageTimer: item.input.messageTimer, messageField78: 0, playCount: 0,
    suspend: 0, sessionPaused: item.input.sessionPaused, flowGate: 0, flowModeOne: 0}));
  const cutIn = execute(cutInCases).blocks;
  for (const [index, block] of cutIn.entries()) {
    checkEffects(block);
    const item = fixture.cutInCases[index];
    if (!item.spawnsAnm && !('postCallPrecondition' in item)) {
      assert.equal(block.status, 'no-operation', 'closed session gate must not spawn');
      assert.equal(block.ops.length, 0);
      assert.equal(block.writes.length, 0);
      assert.equal(block.effects.length, 0);
      continue;
    }
    assert.equal(block.status, 'requires-owner');
    assert.deepEqual(kinds(block), ['spawn-anm']);
    const spawn = block.ops[0];
    assert.equal(spawn.address, '0x004670b0');
    assert.equal(spawn.a, fixture.constants.cutInId);
    assert.equal(spawn.b, fixture.constants.cutInLayer);
    assert.equal(spawn.c, 0);
    assert.equal(spawn.receiverBase, '0x004b8950');
    assert.equal(spawn.receiverOffset, 0x5a34, 'the ANM manager/resource bank comes from [0x4b8950]+0x5a34');
    assert.equal(spawn.outputSlotOffset, -0x64);
    if (item.spawnsAnm) {
      assert.deepEqual([item.stackArguments[1], item.stackArguments[2], item.stackArguments[3]],
        [fixture.constants.cutInId, fixture.constants.cutInLayer, 0], 'original spawn arguments');
    }
    const handle = write(block, 'message', 0x6c);
    assert.equal(handle?.mode, 'assign-handle', 'the ANM handle is owner-supplied, never synthesised');
    assert.equal(handle?.value, 0, 'the probe handle precondition is not a plan value');
    const handleEffect = writeEffect(block, 'message', 0x6c);
    assert.equal(handleEffect.afterOperation, operationEffect(block, 'spawn-anm').index,
      'the handle write belongs to the spawn call result');
    assert.equal(write(block, 'message', 0x70)?.value, item.messageAfter.field70);
    assert.equal(write(block, 'message', 0x74)?.value, item.messageAfter.field74);
    assert.equal(write(block, 'message', 0x78)?.value, item.messageAfter.field78);
    const spawnEffect = operationEffect(block, 'spawn-anm');
    const handleSeq = writeEffect(block, 'message', 0x6c).sequence;
    const field70 = writeEffect(block, 'message', 0x70).sequence;
    const field78 = writeEffect(block, 'message', 0x78).sequence;
    const field74 = writeEffect(block, 'message', 0x74).sequence;
    assert.ok(spawnEffect.sequence < handleSeq && handleSeq < field70 && field70 < field78
      && field78 < field74, 'MSG30 writes follow the original +0x6c/+0x70/+0x78/+0x74 order');
  }

  const cutInExtras = execute([
    {opcode: 30, stage: 99, stats: 99, difficulty: 99, rateA: 0, rateB: 0,
      messageTimer: -2147483648, messageField78: 0, playCount: 100000, suspend: 0,
      sessionPaused: 0, flowGate: 0, flowModeOne: 0},
    {opcode: 30, stage: -5, stats: -5, difficulty: -5, rateA: 0, rateB: 0,
      messageTimer: 5, messageField78: 0, playCount: -1, suspend: 0,
      sessionPaused: 0, flowGate: 0, flowModeOne: 0},
  ]).blocks;
  for (const block of cutInExtras) checkEffects(block);
  assert.equal(cutInExtras[0].status, 'requires-owner',
    'MSG30 reads no stage/stats/difficulty/play-count field');
  assert.equal(cutInExtras[1].status, 'requires-owner');
  assert.equal(write(cutInExtras[0], 'message', 0x78)?.value, 2147483647,
    'the original dec eax wraps INT_MIN - 1 to INT_MAX');
  assert.equal(write(cutInExtras[1], 'message', 0x78)?.value, 0);
  const stageClearCases = fixture.stageClearCases.map(item => ({opcode: 21, stage: item.input.stage,
    stats: item.input.stats, difficulty: item.input.difficulty, rateA: item.input.rateA,
    rateB: item.input.rateB, messageTimer: 0, messageField78: item.input.messageField78,
    playCount: item.input.playCount, suspend: item.input.suspend ? 0x10 : 0,
    sessionPaused: item.input.sessionPaused, flowGate: item.input.flowGate,
    flowModeOne: item.input.flowModeOne ?? 0}));
  const stageClear = execute(stageClearCases).blocks;
  const arm = {'0x00424561': ['save-replay'], '0x00424586': ['reset-path'],
    '0x00424682': ['route-advance', 'set-object-state'], '0x00424776': ['add-score'],
    '0x00424601': ['add-score']};
  for (const [index, block] of stageClear.entries()) {
    checkEffects(block);
    const item = fixture.stageClearCases[index];
    const playable = item.input.stage >= 1 && item.input.stage <= 16;
    if (!playable) {
      assert.equal(block.status, 'rejected');
      assert.ok(block.reason.length > 0);
      assert.equal(block.ops.length + block.writes.length, 0);
      assert.equal(block.effects.length, 0);
      continue;
    }
    assert.equal(block.status, 'requires-owner');
    const branch = arm[item.stop];
    assert.ok(branch, `unrecorded stage-clear stop ${item.stop}`);
    assert.deepEqual(kinds(block), ['clear-slots', 'stage-clear-entry', ...branch].sort(),
      `branch ops for stop ${item.stop}`);
    const prologue = operationEffect(block, 'stage-clear-entry');
    const slots = operationEffect(block, 'clear-slots');
    assert.ok(prologue.sequence < slots.sequence, 'the prologue keeps the original call order');
    for (const kind of branch) assert.ok(slots.sequence < operationEffect(block, kind).sequence);
    const score = block.ops.find(operation => operation.kind === 'add-score');
    if (item.stop === '0x00424682') {
      const route = block.ops.find(operation => operation.kind === 'route-advance');
      assert.equal(route.a, item.input.messageField78);
      assert.equal(write(block, 'game-manager', 0x5a04), undefined, 'ordinary stage must not settle manager flags');
      assert.equal(score, undefined);
      assert.equal(block.writes.length, 0);
      assert.ok(operationEffect(block, 'set-object-state').sequence
        < operationEffect(block, 'route-advance').sequence);
    } else if (score) {
      const flags = writeEffect(block, 'game-manager', 0x5a04, 0);
      assert.equal(flags && block.writes[flags.index].value & 0x10, 0x10);
      assert.equal(flags && block.writes[flags.index].mode, 'or-mask');
      assert.ok(flags.sequence < operationEffect(block, 'add-score').sequence,
        'manager +0x5a04 |= 0x10 happens before the score call');
    }
  }

  const scoreInputs = fixture.scoreCases.map(item => ({opcode: 21, stage: item.input.stage,
    stats: item.input.stats, difficulty: item.input.difficulty, rateA: item.input.rateA,
    rateB: item.input.rateB, messageTimer: 0, messageField78: item.input.messageField78 ?? 0,
    playCount: item.input.playCount, suspend: 0, sessionPaused: 0, flowGate: 0,
    flowModeOne: item.input.flowModeOne ?? 0}));
  const score = execute(scoreInputs).blocks;
  for (const [index, block] of score.entries()) {
    checkEffects(block);
    const item = fixture.scoreCases[index];
    assert.equal(block.status, 'requires-owner');
    assert.equal(block.bonus, item.bonusRaw, `stage ${item.input.stage} difficulty ${item.input.difficulty} bonus`);
    const state = applied(block);
    assert.equal(state.flags5a04, item.managerFlagsAfter, 'manager +0x5a04 after settlement');
    assert.equal(state.bonus5a38, item.managerBonusAfter, 'manager +0x5a38 after settlement');
    assert.equal(block.ops.some(operation => operation.kind === 'add-score'), true,
      'the original score accumulation call stays an explicit operation');
    const reset = block.ops.some(operation => operation.kind === 'reset-path');
    assert.equal(reset, item.continueStop === '0x00424586', 'original 0x4b8a84+0x10 branch');
    const flags = writeEffect(block, 'game-manager', 0x5a04, 0);
    const scoreEffect = operationEffect(block, 'add-score');
    const bonus = writeEffect(block, 'game-manager', 0x5a38);
    assert.ok(flags.sequence < scoreEffect.sequence && scoreEffect.sequence < bonus.sequence,
      'original order is flags, score call, then +0x5a38 bonus add');
    if (item.continueStop === '0x00424586') {
      const resetEffect = operationEffect(block, 'reset-path');
      assert.ok(bonus.sequence < resetEffect.sequence);
      assert.equal(state.stage, undefined, 'gated settlement must not publish a stage');
      assert.equal(state.stageMirror, undefined);
      assert.equal(state.nextStageRecord, undefined);
      assert.equal(block.writes.some(entry => entry.target === 'play-count'), false);
    } else {
      assert.equal(state.stage, item.stageAfter);
      assert.equal(state.stageMirror, item.stageMirrorAfter);
      assert.equal(state.nextStageRecord, item.nextStageRecordAfter);
      const flags8 = writeEffect(block, 'game-manager', 0x5a04, 1);
      const playIndex = item.input.stats * 0x19b + item.input.difficulty;
      const play = writeEffect(block, 'play-count', playIndex);
      if (item.input.stage === 16) {
        assert.ok(play, 'extra-stage play count is written before the flags');
        assert.ok(play.sequence < flags8.sequence, 'extra branch increments the play count first');
      } else if (play) {
        assert.ok(flags8.sequence < play.sequence, 'route-final branch sets flags before the play count');
      }
      const record = writeEffect(block, 'next-stage-record', 0);
      const stage = writeEffect(block, 'stage', 0);
      const mirror = writeEffect(block, 'stage-mirror', 0);
      assert.ok(flags8.sequence < record.sequence && record.sequence < stage.sequence
        && stage.sequence < mirror.sequence, 'settlement tail keeps record/stage/mirror order');
      const delta = item.playCountAfter - item.input.playCount;
      if (delta === 0) assert.equal(block.writes.some(entry => entry.target === 'play-count'), false);
      else {
        const increment = write(block, 'play-count', playIndex);
        assert.equal(increment?.mode, 'increment-capped');
        assert.ok(increment.value > 0);
        assert.equal(increment.cap, 0x1869f);
      }
    }
  }
  const gated = execute([
    {...scoreInputs[0], difficulty: 5, flowModeOne: 1},
    {...scoreInputs[10], difficulty: 5, flowModeOne: 1},
  ]).blocks;
  for (const block of gated) checkEffects(block);
  for (const block of gated) {
    assert.equal(block.status, 'requires-owner');
    assert.equal(block.writes.some(entry => entry.target === 'play-count'), false);
    assert.equal(block.writes.some(entry => entry.target === 'stage'), false);
    assert.equal(block.ops.some(operation => operation.kind === 'reset-path'), true);
    const flags = writeEffect(block, 'game-manager', 0x5a04, 0);
    const scoreEffect = operationEffect(block, 'add-score');
    const bonus = writeEffect(block, 'game-manager', 0x5a38);
    const resetEffect = operationEffect(block, 'reset-path');
    assert.ok(flags.sequence < scoreEffect.sequence && scoreEffect.sequence < bonus.sequence
      && bonus.sequence < resetEffect.sequence);
  }
  assert.equal(gated[0].bonus, 98000, 'the out-of-range difficulty skips only its multiplier');
  const unread = [
    {...timeScaleCases[0], stage: 20, stats: 99, difficulty: 99, playCount: 100000},
    {...timeScaleCases[0], stage: 32},
    {...cutInCases[0], stage: 99, stats: 99, difficulty: 99, playCount: 100000, messageTimer: 5},
    {...scoreInputs[0], stage: 2, stats: 99, difficulty: 99, playCount: 100000},
    {...scoreInputs[0], stage: 2, stats: 99, difficulty: 99, playCount: 100000, suspend: 0x10},
    {...scoreInputs[0], stage: 2, stats: 99, difficulty: 99, playCount: 100000, sessionPaused: 1, flowGate: 1},
    {...scoreInputs[0], stats: 99, difficulty: 99, playCount: 100000, flowModeOne: 1},
    {...scoreInputs[10], stats: 99, difficulty: 99, playCount: 100000, flowModeOne: 1},
  ];
  const unreadBlocks = execute(unread).blocks;
  for (const block of unreadBlocks) checkEffects(block);
  assert.equal(unreadBlocks[0].status, 'requires-owner');
  assert.equal(unreadBlocks[0].ops[0].a, floatBits(8));
  assert.equal(unreadBlocks[1].ops[0].a, floatBits(8));
  assert.equal(unreadBlocks[2].status, 'requires-owner');
  assert.deepEqual(kinds(unreadBlocks[2]), ['spawn-anm']);
  assert.equal(unreadBlocks[3].status, 'requires-owner');
  assert.deepEqual(kinds(unreadBlocks[3]), ['clear-slots', 'route-advance', 'set-object-state', 'stage-clear-entry']);
  assert.equal(unreadBlocks[3].writes.length, 0);
  assert.deepEqual(kinds(unreadBlocks[4]), ['clear-slots', 'save-replay', 'stage-clear-entry']);
  assert.deepEqual(kinds(unreadBlocks[5]), ['clear-slots', 'reset-path', 'stage-clear-entry']);
  for (const block of unreadBlocks.slice(6)) {
    assert.equal(block.status, 'requires-owner');
    assert.deepEqual(kinds(block), ['add-score', 'clear-slots', 'reset-path', 'stage-clear-entry']);
    assert.equal(block.writes.some(entry => entry.target === 'play-count'), false);
    assert.equal(block.writes.some(entry => entry.target === 'stage'), false);
  }
  const rejected = [
    {...timeScaleCases[0], opcode: 20}, {...timeScaleCases[0], opcode: 31},
    {...timeScaleCases[0], opcode: 23}, {...timeScaleCases[0], stage: -1},
    {...timeScaleCases[0], stage: 33}, {...scoreInputs[0], stage: -1},
    {...scoreInputs[0], stage: 0}, {...scoreInputs[0], stage: 17},
    {...scoreInputs[0], stage: 20}, {...scoreInputs[0], stage: 24},
    {...scoreInputs[0], stage: 33}, {...scoreInputs[0], stats: 17},
    {...scoreInputs[0], stats: 16}, {...scoreInputs[0], difficulty: 5},
    {...scoreInputs[0], playCount: 100000},
    {...scoreInputs[0], playCount: -1}, {...scoreInputs[10], difficulty: 5},
    {...scoreInputs[11], stats: 17},
  ];
  const failed = execute(rejected).blocks;
  for (const block of failed) {
    checkEffects(block);
    assert.equal(block.status, 'rejected');
    assert.ok(block.reason.length > 0);
    assert.equal(block.ops.length + block.writes.length, 0, 'rejected input must not publish a plan');
    assert.equal(block.effects.length, 0);
  }
  const all = [...stageClearCases, ...scoreInputs, ...timeScaleCases, ...cutInCases, ...unread];
  const replay = execute(all);
  const first = execute(all);
  assert.equal(replay.stdout, first.stdout, 'plan() must be a pure function of its input');
});
