import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/dsh-stage-resources');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const marker = value => value === '' ? '~' : value;

function table() {
  return JSON.parse(fs.readFileSync(path.join(root, 'local/research/stage-table.json'), 'utf8'));
}

function typed(report) {
  return report.stages.map(stage => ({
    id: stage.id,
    background: stage.background,
    ecl: stage.ecl,
    message: stage.message,
    logo: stage.logo,
    music: [stage.music[0], stage.music[1]],
  }));
}

const clone = records => records.map(record => ({...record, music: [...record.music]}));

function recLines(records) {
  return records.map(record => ['REC', record.id, marker(record.background), marker(record.ecl),
    marker(record.message), marker(record.logo), marker(record.music[0]), marker(record.music[1])].join(' '));
}

function findLine(requested, record) {
  return ['find', requested, record.id, marker(record.background), marker(record.ecl),
    marker(record.message), marker(record.logo), marker(record.music[0]), marker(record.music[1])].join(' ');
}

test('stage-table.json is the fixed 16-entry original table and not a route map', () => {
  const report = table();
  assert.equal(report.schema, 'th128-original-stage-table/1');
  assert.equal(report.exeSha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.address, '0x4b1a30');
  assert.equal(report.stride, 40);
  assert.equal(report.stages.length, 16);
  assert.deepEqual(report.stages.map(stage => stage.id), Array.from({length: 16}, (_, index) => index + 1));
  for (const stage of report.stages) {
    const bytes = Buffer.from(stage.raw, 'hex');
    assert.equal(bytes.length, 40, `stage ${stage.id}: raw record size`);
    assert.equal(bytes.readUInt32LE(0), stage.id, `stage ${stage.id}: raw id field`);
    assert.equal(Number.parseInt(stage.address, 16), 0x4b1a58 + (stage.id - 1) * 40, `stage ${stage.id}: address`);
    assert.equal(stage.pointers.length, 6);
    assert.equal(stage.uninterpretedFields.length, 3);
    assert.match(stage.background, /^stage_[a-z0-9_]+\.std$/);
    assert.match(stage.ecl, /^st_[a-z0-9_]+\.ecl$/);
    assert.match(stage.message, /^st_[a-z0-9_]+\.msg$/);
    assert.match(stage.logo, /^logo_[a-z0-9_]+\.anm$/);
    assert.match(stage.music[0], /^th128_[a-z0-9_]+\.wav$/);
    assert.match(stage.music[1], /^th128_[a-z0-9_]+\.wav$/);
  }
  assert.equal(report.stages[1].ecl, 'st_a1_2.ecl');
  assert.equal(report.stages[1].background, 'stage_b2.std');
  assert.notEqual(report.stages[1].background, 'stage_a1.std');
  assert.equal(report.stages[6].ecl, 'st_b1_2.ecl');
  assert.equal(report.stages[6].background, 'stage_a2.std');
  assert.equal(report.stages[9].ecl, 'st_b2_3.ecl');
  assert.equal(report.stages[9].background, 'stage_a3.std');
  const extra = report.stages[15];
  assert.equal(extra.background, 'stage_ex.std');
  assert.equal(extra.ecl, 'st_ex.ecl');
  assert.equal(extra.message, 'st_ex.msg');
  assert.equal(extra.logo, 'logo_ex.anm');
  assert.deepEqual(extra.music, ['th128_10.wav', 'th128_11.wav']);
  const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
  assert.equal(catalog.schema, 'th128-asset-catalog/1');
  assert.equal(catalog.playable, false);
  for (const variant of ['ja', 'zh']) {
    assert.match(catalog.variants[variant].manifest, /^generations\/[0-9a-f]{32}\/(ja|zh)\/manifest\.json$/);
    assert.equal(catalog.variants[variant].totals.routeScripts, 16);
  }
});

test('typed catalog reproduces the original 16 records and rejects unsafe input without publishing it',
  () => {
    assert.ok(fs.existsSync(compiler), `emscripten compiler missing: ${compiler}`);
    const report = table();
    const records = typed(report);
    const input = [], expected = [];
    const push = (lines, wants) => {
      input.push(...lines);
      expected.push(...wants);
    };
    const rejected = size => new RegExp(`^load 0 size ${size} error [1-9][0-9]*$`);

    const duplicate = clone(records);
    duplicate[3].id = 2;
    push(['DROP', ...recLines(duplicate), 'LOAD'], [rejected(0)]);
    push(['SIZE', 'FIND 1'], ['size 0', 'find 1 none']);
    push(['DROP', 'LOAD'], [rejected(0)]);

    push(['DROP', ...recLines(records), 'LOAD'], ['load 1 size 16 error 0']);
    for (const record of records)
      push([`FIND ${record.id}`], [findLine(record.id, record)]);
    push(['SIZE'], ['size 16']);
    for (const id of [-1, 0, 17, 99])
      push([`FIND ${id}`], [`find ${id} none`]);
    push(['CLONE'], [[`clone 16 1 ${marker(records[0].background)} ${marker(records[0].ecl)}`,
      marker(records[0].message), marker(records[0].logo), marker(records[0].music[0]),
      marker(records[0].music[1])].join(' ')]);
    push(['PTR 2'], [/^ptr 2 [0-9a-f]+$/]);
    push(['MUTATE'], []);
    push(['FIND 2', 'DROP', 'FIND 2'], [findLine(2, records[1]), findLine(2, records[1])]);
    push(['PTR 2'], ['ptr 2 stable']);

    const corrupt = mutate => {
      const copy = clone(records);
      mutate(copy);
      return copy;
    };
    const invalid = [
      corrupt(copy => { copy[3].id = 2; }),
      corrupt(copy => { copy.pop(); }),
      corrupt(copy => { copy[0].id = 0; }),
      corrupt(copy => { copy[0].id = 17; }),
      corrupt(copy => { copy[0].background = 'stage_a1.png'; }),
      corrupt(copy => { copy[1].ecl = '../st_a1_2.ecl'; }),
      corrupt(copy => { copy[2].message = '/st_a1_3.msg'; }),
      corrupt(copy => { copy[3].logo = 'C:logo_a2_2.anm'; }),
      corrupt(copy => { copy[4].logo = 'sub\\logo_a2_3.anm'; }),
      corrupt(copy => { copy[5].music[0] = 'th128_05.mp3'; }),
      corrupt(copy => { copy[6].music[1] = 'th128_02.wav\u0000'; }),
      corrupt(copy => { copy[7].background = ''; }),
      corrupt(copy => { copy[8].logo = `${'l'.repeat(120)}.anm`; }),
      corrupt(copy => { copy[9].ecl = 'ST_B2_3.ECL'; }),
      corrupt(copy => { copy[10].background = '.stage_c1.std'; }),
    ];
    for (const records_ of invalid) {
      push(['DROP', ...recLines(records_), 'LOAD'], [rejected(16)]);
      push(['FIND 2'], [findLine(2, records[1])]);
    }

    const replaced = corrupt(copy => { copy[1].ecl = 'st_reselected_2.ecl'; });
    push(['DROP', ...recLines(replaced), 'LOAD'], ['load 1 size 16 error 0']);
    push(['FIND 2'], [findLine(2, replaced[1])]);
    push(['DROP', ...recLines(records), 'LOAD'], ['load 1 size 16 error 0']);
    push(['FIND 2'], [findLine(2, records[1])]);
    push(['FIND 16'], [findLine(16, records[15])]);

    fs.mkdirSync(work, {recursive: true});
    const temporary = path.join(work, 'temp');
    fs.mkdirSync(temporary, {recursive: true});
    const cache = path.join(work, 'emscripten-cache');
    if (!fs.existsSync(cache))
      fs.cpSync(path.resolve(root, '../../work/emscripten-cache-4.0.22'), cache,
        {recursive: true, filter: source => !path.basename(source).endsWith('.lock')});
    const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
      EM_CACHE: cache.replaceAll('\\', '/'), EMSDK_PYTHON: python, PYTHONUTF8: '1',
      EMCC_CORES: '2', BINARYEN_CORES: '2', TEMP: temporary, TMP: temporary, TMPDIR: temporary};
    const output = path.join(work, 'stage-resource-catalog.cjs');
    const build = spawnSync(python, [compiler, 'tests/native/stage-resource-catalog.cpp',
      'src/game/StageResourceCatalog.cpp', '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
      '-fexceptions', '-fno-fast-math', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
    {cwd: root, env: environment, encoding: 'utf8', timeout: 180000});
    assert.ifError(build.error);
    assert.equal(build.status, 0, build.stdout + build.stderr);
    const run = spawnSync(process.execPath, [output],
      {cwd: root, input: `${input.join('\n')}\n`, encoding: 'utf8', timeout: 30000, maxBuffer: 4 * 1024 * 1024});
    assert.ifError(run.error);
    assert.equal(run.status, 0, run.stdout + run.stderr);
    const actual = run.stdout.trim().split(/\r?\n/);
    assert.equal(actual.length, expected.length, actual.join('\n'));
    expected.forEach((want, index) => {
      if (want instanceof RegExp)
        assert.match(actual[index], want, `line ${index + 1}`);
      else
        assert.equal(actual[index], want, `line ${index + 1}`);
    });
  });
