import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/bullet-tick-test');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const bits = value => { const bytes = Buffer.alloc(4); bytes.writeFloatLE(value); return bytes.readUInt32LE(); };
const categories = ['NeedsAnimation', 'NeedsCollision', 'NeedsExtension', 'Removed'];

function record(state) {
  return [state.type, state.motionState, state.age, state.lifeClock, bits(state.lifeTime),
    state.integer0, state.flags, state.activeExtensions, state.grace, state.cooldown,
    Number(state.hasSprite), bits(state.spriteWidth), bits(state.spriteHeight), bits(state.clock),
    Number(state.extensionListEmpty), ...state.positionBits, ...state.velocityBits];
}

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/bullet-tick-fixtures.json'));
  assert.equal(digest(raw), '7cd96410986961d89c4120dba9a2d6d935757604424ad7592e293e6f82d579fb');
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-bullet-tick-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.caseCount, 84);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
  }
  for (const constant of Object.values(report.constants))
    assert.equal(digest(Buffer.from(constant.bytes, 'hex')), constant.sha256);
  return report;
}

test('reusable pre-animation bullet rules exist independently of the frozen ANM sidecar', () => {
  for (const file of ['src/game/BulletTickRules.hpp', 'src/game/BulletTickRules.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `${file}: pre-animation bullet rules are not implemented`);
});

test('bounded no-effect collision continuation is implemented without restarting the body', () => {
  assert.match(fs.readFileSync(path.join(root, 'src/game/BulletTickRules.cpp'), 'utf8'),
    /resumeAfterCollisionMiss\s*\(/, 'collision miss resume is not implemented');
});

test('original update prefixes classify call boundaries without fake collision or animation returns', () => {
  const report = evidence();
  assert.equal(report.scheduler.bulletPriority, 24);
  assert.equal(report.scheduler.enemyPriority, 21);
  const seen = new Set();
  for (const sample of report.cases) {
    seen.add(sample.continuation);
    assert.equal(sample.completedTick, false);
    assert.ok(['boundary', 'unsupported_call', 'returned'].includes(sample.stop.reason), sample.id);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    assert.ok(!sample.instructionTrace.includes('0x00408d0c'), sample.id);
    assert.ok(!sample.instructionTrace.includes('0x00459d90'), sample.id);
    if (sample.continuation === 'NeedsCollision') assert.ok(['0x0043c410', '0x0043c570'].includes(sample.stop.target));
  }
  assert.deepEqual([...seen].sort(), [...categories].sort());
  const registration = report.code['0x408241:0x408258'];
  const enemy = report.code['0x4153ae:0x4153c5'];
  const callTarget = (chunk, location) => {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    const offset = location - Number.parseInt(chunk.start);
    assert.equal(bytes[offset], 0xe8);
    return location + 5 + bytes.readInt32LE(offset + 1);
  };
  assert.equal(Buffer.from(registration.bytes, 'hex')[0], 0xbb);
  assert.equal(Buffer.from(registration.bytes, 'hex').readUInt32LE(1), 24);
  assert.equal(Buffer.from(enemy.bytes, 'hex').readUInt32LE(1), 21);
  assert.equal(callTarget(registration, 0x408253), 0x4681d0);
  assert.equal(callTarget(enemy, 0x4153c0), 0x4681d0);
  assert.equal(callTarget(report.code['0x4091a0:0x4091c8'], 0x4091c1), 0x408d50);
  const manager = report.code['0x408d50:0x408e69'];
  assert.equal(callTarget(manager, 0x408dbf), 0x408640);
  const managerBytes = Buffer.from(manager.bytes, 'hex');
  assert.equal(managerBytes.readUInt32LE(0x408d91 - 0x408d50), 2000);
  assert.equal(managerBytes.readUInt32LE(0x408e4a - 0x408d50), 0x11b8);
  const rounded = report.cases.find(sample => sample.id === 'transition-two-rounded-moves');
  assert.equal(rounded.next.positionBits[0], rounded.input.positionBits[0]);
  assert.notEqual(rounded.next.positionBits[0], bits(rounded.input.position[0] + rounded.input.velocity[0] * 1.5));
});

test('separate hit-effect continuation is explicit instead of inventing a successful effect return',()=>{
  assert.match(fs.readFileSync(path.join(root,'src/game/BulletTickRules.hpp'),'utf8'),/resumeAfterResolvedHit/,
    'completed-hit continuation is not implemented');
});

test('portable pre-ANM next-state matches every actual-byte prefix in WASM and UBSan',
  {skip: !fs.existsSync(compiler)}, () => {
    assert.ok(fs.existsSync(path.join(root, 'src/game/BulletTickRules.hpp')), 'bullet tick rules are not implemented');
    const report = evidence();
    fs.mkdirSync(work, {recursive: true});
    const resumeRaw = fs.readFileSync(path.join(root, 'local/research/bullet-tick-resume-fixtures.json'));
    assert.equal(digest(resumeRaw), 'b2d5d6a897e2f7748dddd5d254d81c115f4b7fb244e488cf2936bbd63481f575');
    const resumeReport = JSON.parse(resumeRaw.toString('utf8'));
    assert.equal(resumeReport.schema, 'th128-original-byte-bullet-tick-resume-fixtures/1');
    assert.equal(resumeReport.executable.sha256, report.executable.sha256);
    assert.equal(resumeReport.executable.sha256After, report.executable.sha256);
    assert.equal(resumeReport.executable.unchanged, true);
    assert.equal(resumeReport.nativeCpuTrace, false);
    assert.equal(resumeReport.windowsGameProcessLaunched, false);
    const resumeCases = resumeReport.cases;
    assert.equal(resumeCases.length, 17);
    for (const chunk of Object.values(resumeReport.code))
      assert.equal(digest(Buffer.from(chunk.bytes, 'hex')), chunk.sha256);
    for (const constant of Object.values(resumeReport.constants))
      assert.equal(digest(Buffer.from(constant.bytes, 'hex')), constant.sha256);
    for (const sample of resumeCases) {
      assert.equal(sample.helperMeasurement.helperReturn, 0);
      assert.equal(sample.helperMeasurement.registers.eax, 0);
      assert.ok(sample.helperMeasurement.instructionTrace.some(location =>
        location === '0x0043c410' || location === '0x0043c570'));
      assert.ok(!sample.expected.instructionTrace.includes('0x00408d0c'));
    }
    const hitRaw=fs.readFileSync(path.join(root,'local/research/bullet-tick-hit-suffix-fixtures.json'));
    assert.equal(digest(hitRaw),'e9f78265ab3396919c0ca54c9be1d9c54058636d2f6d1de3836fe9cb6a143eb2');
    const hitReport=JSON.parse(hitRaw.toString('utf8'));
    assert.equal(hitReport.schema,'th128-original-byte-bullet-hit-suffix/1');
    assert.equal(hitReport.caseCount,6);
    for(const sample of hitReport.cases){
      assert.equal(sample.expected.independentAfterHitSuffix,true);
      assert.equal(sample.expected.entry,'0x00408c93');
      assert.equal(sample.expected.completedTick,false);
    }
    const input = [...report.cases.map(sample => ['T', ...record(sample.input)].join(' ')),
      ...resumeCases.map(sample => ['R', categories.indexOf(sample.pending.continuation),
        sample.pending.boundary, sample.pending.previousLifeClock, ...record(sample.pending.next)].join(' ')),
      ...hitReport.cases.map(sample=>['H',categories.indexOf(sample.pending.continuation),sample.pending.boundary,
        sample.pending.previousLifeClock,...record(sample.pending.next)].join(' '))].join('\n') + '\n';
    const expected = report.cases.map(sample => [categories.indexOf(sample.continuation),
      sample.boundary, sample.previousLifeClock, ...record(sample.next)].join(' '));
    expected.push(...resumeCases.map(sample => [categories.indexOf(sample.expected.continuation),
      sample.expected.boundary, sample.expected.previousLifeClock, ...record(sample.expected.next)].join(' ')));
    expected.push(...hitReport.cases.map(sample=>[categories.indexOf(sample.expected.continuation),sample.expected.boundary,
      sample.expected.previousLifeClock,...record(sample.expected.next)].join(' ')));
    for (const sanitized of [false, true]) {
      const output = path.join(work, `bullet-tick${sanitized ? '-ubsan' : ''}.cjs`);
      const flags = sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : [];
      const build = spawnSync(python, [compiler, 'tests/native/bullet-tick.cpp', 'src/game/BulletTickRules.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-exceptions',
        '-fno-fast-math', '-ffp-contract=off', ...flags, '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
      {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
      assert.ifError(build.error);
      assert.equal(build.status, 0, build.stdout + build.stderr);
      const run = spawnSync(process.execPath, [output], {cwd: root, input, encoding: 'utf8', timeout: 30000});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stdout + run.stderr);
      const actual = run.stdout.trim().split(/\r?\n/);
      assert.equal(actual.length, expected.length);
      actual.forEach((line, index) => assert.equal(line, expected[index],
        index<report.cases.length?report.cases[index].id:index<report.cases.length+resumeCases.length?
          resumeCases[index-report.cases.length].id:hitReport.cases[index-report.cases.length-resumeCases.length].id));
    }
  });
