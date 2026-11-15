import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/ice-mask-test');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const bits = value => { const bytes = Buffer.alloc(4); bytes.writeFloatLE(value); return bytes.readUInt32LE(); };

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/ice-mask-fixtures.json'));
  assert.equal(digest(raw), '754e363dff1333172eb99edb612861063554755f2070c70859a60d9690f14438');
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-ice-mask-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.caseCount, 108);
  assert.equal(report.layout.maskBytes, 21504);
  assert.equal(report.layout.bits, 172032);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
  }
  for (const constant of Object.values(report.constants))
    assert.equal(digest(Buffer.from(constant.bytes, 'hex')), constant.sha256);
  assert.equal(digest(fs.readFileSync(path.join(root, 'tools/research/ice_mask_probe.py'))), report.generatorSha256);
  for (const [name, expected] of Object.entries(report.engineSha256))
    assert.equal(digest(fs.readFileSync(path.join(root, 'tools/research', name))), expected);
  return report;
}

test('production mask API exposes actual byte-span primitives, not a radius substitute', () => {
  for (const file of ['src/game/IceMaskRules.hpp', 'src/game/IceMaskRules.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `${file}: bounded ice mask rules are not implemented`);
});

test('complete native counter and bitmap helpers return; effect and removal callers remain incomplete', () => {
  const report = evidence();
  for (const sample of [...report.counterCases, ...report.rasterCases]) {
    assert.equal(sample.stop.reason, 'returned');
    assert.equal(sample.completedFunction, true);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget);
    assert.equal(sample.traceSummary.length, sample.instructionsExecuted);
  }
  for (const sample of report.counterCases) {
    assert.equal(sample.traceSummary.visits['0x00410980'], 0x700);
    assert.equal(sample.declaredData[0].bytes, 21504);
    assert.equal(sample.declaredData[0].beforeSha256, sample.declaredData[0].afterSha256);
  }
  for (const sample of report.attachmentPrefixes) {
    assert.equal(sample.stop.reason, 'unsupported_call');
    assert.equal(sample.stop.target, '0x00407ec0');
    assert.equal(sample.layer, 23);
    assert.equal(sample.script, sample.selector <= 3 ? 1 : 0);
    assert.equal(sample.objectSelector, 0);
    assert.equal(sample.completedCaller, false);
    const profile = report.profiles.find(value => value.selector === sample.selector);
    assert.equal(digest(Buffer.from(sample.bitmap, 'hex')), profile.sha256);
  }
  for (const sample of report.removalPrefixes) {
    assert.deepEqual(sample.stop, {reason: 'boundary', address: '0x0040f1fe'});
    assert.equal(sample.objectSelector, sample.selector);
    assert.equal(sample.completedCaller, false);
  }
  const wrapper = Buffer.from(report.code['0x407f60:0x407fa4'].bytes, 'hex');
  assert.equal(0x407f70 + 5 + wrapper.readInt32LE(0x407f71 - 0x407f60), 0x466d60);
  const registration = Buffer.from(report.code['0x466d60:0x466ddc'].bytes, 'hex');
  assert.equal(registration.readUInt32LE(0x466d78 - 0x466d60), 0x8b9704);
  assert.equal(registration.readUInt32LE(0x466da8 - 0x466d60), 0x8b9708);
  const loader = Buffer.from(report.code['0x40e825:0x40e83f'].bytes, 'hex');
  assert.equal(loader.readUInt32LE(6), 36);
});

test('portable byte-span raster, selector plans and full counter match native goldens in WASM and UBSan',
  {skip: !fs.existsSync(compiler)}, () => {
    assert.ok(fs.existsSync(path.join(root, 'src/game/IceMaskRules.hpp')), 'ice mask API is not implemented');
    const report = evidence();
    const input = [], expected = [];
    for (const sample of report.counterCases) {
      input.push(`C ${sample.mask}`);
      expected.push({meta: String(sample.count)});
    }
    for (const sample of report.rasterCases) {
      const value = sample.input;
      input.push(['R', value.operation === 'or' ? 0 : 1, ...value.positionBits, value.height,
        value.wordsPerRow, sample.maskBefore, sample.bitmap || '-'].join(' '));
      expected.push({meta: [sample.placement.startRow, sample.placement.startWord, sample.placement.signedRemainder,
        sample.writes, sample.changedBits].join(' '), bitmap: sample.maskAfter});
    }
    for (const sample of report.attachmentPrefixes) {
      input.push(['R', 0, bits(0), bits(200), sample.height, sample.wordsPerRow,
        '00'.repeat(21504), sample.bitmap].join(' '));
      expected.push({bitmap: sample.maskAfter});
      input.push(['A', sample.selector, bits(0), bits(200), bits(.1)].join(' '));
      expected.push({meta: [sample.selector, sample.height, sample.wordsPerRow, 0, Number.parseInt(sample.stop.address),
        0x407ec0, sample.layer, sample.script, 36, 0x65ddc, 0x58, 0x54, bits(0), bits(200), bits(.1)].join(' ')});
    }
    for (const sample of report.removalPrefixes) {
      input.push(['D', sample.selector, bits(0), bits(200), 'ff'.repeat(21504), sample.bitmap || '-'].join(' '));
      expected.push({meta: sample.selector === 0 ? '0' : sample.selector <= 5 ? '1' : '2', bitmap: sample.maskAfter});
    }
    fs.mkdirSync(work, {recursive: true});
    const temporary = path.join(work, 'temp');
    fs.mkdirSync(temporary, {recursive: true});
    const cache = path.join(work, 'emscripten-cache');
    if (!fs.existsSync(cache))
      fs.cpSync(path.resolve(root, '../../work/emscripten-cache-4.0.22'), cache,
        {recursive: true, filter: source => !path.basename(source).endsWith('.lock')});
    const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
      EM_CACHE: cache.replaceAll('\\', '/'), EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2',
      TEMP: temporary, TMP: temporary, TMPDIR: temporary};
    for (const sanitized of [false, true]) {
      const output = path.join(work, `ice-mask${sanitized ? '-ubsan' : ''}.cjs`);
      const build = spawnSync(python, [compiler, 'tests/native/ice-mask.cpp', 'src/game/IceMaskRules.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-exceptions',
        '-fno-fast-math', '-ffp-contract=off', ...(sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : []),
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
      {cwd: root, env: environment, encoding: 'utf8', timeout: 180000});
      assert.ifError(build.error);
      assert.equal(build.status, 0, build.stdout + build.stderr);
      const run = spawnSync(process.execPath, [output], {cwd: root, input: input.join('\n') + '\n',
        encoding: 'utf8', timeout: 30000, maxBuffer: 16 * 1024 * 1024});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stderr);
      const actual = run.stdout.trim().split(/\r?\n/);
      assert.equal(actual.length, expected.length);
      actual.forEach((line, index) => {
        const expectedValue = expected[index];
        const parts = line.split(' ');
        if (expectedValue.bitmap) {
          const bitmap = parts.pop();
          assert.equal(bitmap.length, 21504 * 2);
          assert.equal(digest(Buffer.from(bitmap, 'hex')), digest(Buffer.from(expectedValue.bitmap, 'hex')), `bitmap vector ${index}`);
        }
        if (expectedValue.meta !== undefined)
          assert.equal(parts.join(' '), expectedValue.meta, `metadata vector ${index}`);
      });
    }
  });
