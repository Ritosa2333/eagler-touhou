import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import test from 'node:test';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const environment = {...process.env,
  EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const reportPath = path.join(root, 'local/research/freeze-fixtures.json');
const reportSha256 = '7e439c868fdf0169ee1665e839af0a91bac08b91546e3e1cb189af77144969ff';
const exeSha256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';
let compiled;

function buildFixture() {
  if (compiled) return compiled;
  const directory = path.join(root, 'artifacts/tests');
  fs.mkdirSync(directory, {recursive: true});
  const output = path.join(directory, 'freeze-rules.cjs');
  const build = spawnSync(python, [compiler, 'tests/native/freeze-rules.cpp',
    'src/game/FreezeRules.cpp', '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
    '-fno-fast-math', '-ffp-contract=off', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
    '-sASSERTIONS=1', '-o', output],
  {cwd: root, env: environment, encoding: 'utf8', timeout: 180000, maxBuffer: 8 * 1024 * 1024});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);
  compiled = output;
  return output;
}

function runFixture(input = '') {
  const run = spawnSync(process.execPath, [buildFixture()],
    {cwd: root, input, encoding: 'utf8', timeout: 30000, maxBuffer: 8 * 1024 * 1024});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /^freeze scalar fixed vectors passed: [0-9]+\r?\n/);
  return run.stdout;
}

function readReport() {
  const binary = fs.readFileSync(reportPath);
  assert.equal(createHash('sha256').update(binary).digest('hex'), reportSha256);
  const report = JSON.parse(binary.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-freeze-fixtures/1');
  assert.equal(report.executable.sha256, exeSha256);
  assert.equal(report.executable.sha256After, exeSha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.caseCount, 225);
  assert.equal(report.cases.length, 225);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(createHash('sha256').update(bytes).digest('hex'), chunk.sha256);
  }
  return report;
}

function scalarVectors(report) {
  const vectors = [];
  const selectedSources = new Map();
  const argumentsI32 = result => Buffer.from(result.arguments, 'hex').readInt32LE();
  const stackI32 = result => Buffer.from(result.stackHead, 'hex').readInt32LE();
  const before = (result, name) => result.memory[name].beforeI32;
  const after = (result, name) => result.memory[name].afterI32;
  function add(record, operation, inputs, expected) {
    const reason = record.result.stop.reason;
    assert.ok(['returned', 'boundary', 'unsupported_call'].includes(reason), record.id);
    const category = reason === 'returned' ? 'completed-original-return'
      : reason === 'boundary' ? 'proved-slice-boundary' : 'pre-call-scalar-only';
    const identifier = `${record.id}:${operation}`;
    vectors.push({identifier, operation, inputs, expected: expected === null ? 'none' : String(expected), category});
    selectedSources.set(record.id, category);
  }
  for (const record of report.cases) {
    const result = record.result;
    const stop = result.stop;
    const family = record.code[0];
    if (family === 'power_add') {
      if (stop.reason === 'unsupported_call') assert.equal(stop.target, '0x00458410');
      add(record, 'power', [before(result, 'power'), argumentsI32(result)], after(result, 'power'));
    } else if (family === 'perfect_add') {
      if (stop.reason === 'unsupported_call') assert.equal(stop.target, '0x00458410');
      add(record, 'perfect', [before(result, 'perfect'), argumentsI32(result)], after(result, 'perfect'));
      if (argumentsI32(result) === -10000 && before(result, 'perfect') >= 10000) {
        assert.equal(stop.reason, 'returned', record.id);
        add(record, 'spend', [before(result, 'perfect')], after(result, 'perfect'));
      }
    } else if (family === 'baseline_replenish') {
      if (stop.reason === 'unsupported_call') assert.equal(stop.target, '0x00458410');
      else assert.equal(stop.address, '0x0043f469');
      add(record, 'baseline', [before(result, 'power')], after(result, 'power'));
    } else if (family === 'held_rate') {
      let expected = null;
      if (stop.reason === 'unsupported_call') {
        assert.equal(stop.address, '0x0043f39a');
        assert.equal(stop.target, '0x0041f410');
        expected = stackI32(result);
      } else {
        assert.equal(stop.reason, 'boundary');
        assert.equal(stop.address, '0x0043f39f');
      }
      add(record, 'held', [before(result, 'holdTimer')], expected);
    } else if (family === 'release_gate') {
      let expected = 0;
      if (stop.reason === 'unsupported_call') {
        assert.equal(stop.address, '0x0043f42e');
        assert.equal(stop.target, '0x00403280');
        expected = 1;
      } else {
        assert.equal(stop.reason, 'boundary');
        assert.ok(['0x0043f442', '0x0043f462'].includes(stop.address));
      }
      add(record, 'release', [before(result, 'power'), result.inputRegisters.eax,
        before(result, 'busy')], expected);
    } else if (family === 'duration_gate') {
      assert.equal(stop.reason, 'boundary');
      assert.ok(['0x00410409', '0x00410454'].includes(stop.address));
      add(record, 'duration', [before(result, 'savedPower')], result.registers.ecx | 0);
      add(record, 'pending', [before(result, 'savedPower'), before(result, 'slotTimer')],
        Number(stop.address === '0x00410454'));
    } else if (family === 'activation_zero') {
      assert.equal(stop.reason, 'boundary');
      assert.equal(stop.address, '0x004102dc');
      add(record, 'activation-store', [], after(result, 'power'));
    } else if (family === 'area_perfect_gain') {
      let expected = null;
      if (stop.reason === 'unsupported_call') {
        assert.equal(stop.address, '0x0040f856');
        assert.equal(stop.target, '0x00427300');
        expected = stackI32(result);
      } else {
        assert.equal(stop.reason, 'boundary');
        assert.equal(stop.address, '0x0040f861');
      }
      const areaBits = result.memory.lastArea
        ? Buffer.from(result.memory.lastArea.before, 'hex').readUInt32LE() : 0;
      add(record, 'area', [areaBits, before(result, 'savedPower')], expected);
    }
  }
  assert.equal(new Set(vectors.map(vector => vector.identifier)).size, vectors.length);
  return {vectors, selectedSources};
}

test('research commands preserve the actual original Unicode path', () => {
  const expected = 'E:\\gee\\\u5996\u7cbe\u5927\u6218\u4e89\\th128.exe';
  const document = fs.readFileSync(path.join(root, 'docs/research/freeze-rules.md'), 'utf8');
  const commandLines = document.split(/\r?\n/).filter(line => line.includes('th128.exe'));
  assert.equal(commandLines.length, 3);
  for (const line of commandLines) assert.ok(line.includes(expected), 'copyable path: ' + line);
});

test('golden fixture provenance separates returned results from stopped prefixes',
  {skip: !fs.existsSync(reportPath)}, context => {
    const {vectors, selectedSources} = scalarVectors(readReport());
    const totals = {};
    for (const category of selectedSources.values()) totals[category] = (totals[category] ?? 0) + 1;
    assert.equal(totals['completed-original-return'], 65);
    assert.ok(totals['proved-slice-boundary'] > 0);
    assert.ok(totals['pre-call-scalar-only'] < 97);
    assert.ok(vectors.length > selectedSources.size);
    context.diagnostic(JSON.stringify({goldenQueries: vectors.length, selectedSourceCases: selectedSources.size, totals}));
  });

test('portable C++ freeze scalar fixture builds and passes fixed vectors in WASM',
  {skip: !fs.existsSync(compiler)}, () => {
    const output = runFixture();
    assert.match(output, /freeze evidence query count: 0/);
  });

test('WASM matches original returned, boundary and pre-call scalar evidence without invoking callouts',
  {skip: !fs.existsSync(compiler) || !fs.existsSync(reportPath)}, context => {
    const {vectors} = scalarVectors(readReport());
    const input = vectors.map(vector => [vector.identifier, vector.operation, ...vector.inputs].join(' ')).join('\n') + '\n';
    const output = runFixture(input);
    const lines = output.trim().split(/\r?\n/);
    assert.equal(lines.length, vectors.length + 2);
    assert.equal(lines.at(-1), `freeze evidence query count: ${vectors.length}`);
    const results = new Map(lines.slice(1, -1).map(line => line.split(' ')));
    assert.equal(results.size, vectors.length);
    for (const vector of vectors) {
      assert.equal(results.get(vector.identifier), vector.expected,
        `${vector.identifier} (${vector.category}; not a lifecycle verification)`);
    }
    context.diagnostic(`${vectors.length} scalar queries match; pre-call values are not full native-rule verification`);
  });
