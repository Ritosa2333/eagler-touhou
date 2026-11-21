import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-stage-session-identity');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const table = JSON.parse(fs.readFileSync(path.join(root, 'local/research/stage-table.json'), 'utf8'));

test('original stage session identity and terminal carry capabilities', () => {
  assert.equal(table.schema, 'th128-original-stage-table/1');
  assert.equal(table.stages.length, 16);
  const stages = table.stages.sort((a, b) => a.id - b.id);
  const role = id => id === 1 || id === 6 || id === 11 ? 'start'
    : [2, 4, 7, 9, 12, 14].includes(id) ? 'second'
    : [3, 5, 8, 10, 13, 15].includes(id) ? 'terminal' : 'extra';
  const caps = id => {
    const kind = role(id);
    return kind === 'start' ? 'carry=0 handoff=1 playable=1'
      : kind === 'second' ? 'carry=1 handoff=1 playable=1'
      : kind === 'terminal' ? 'carry=1 handoff=0 playable=1'
      : 'carry=0 handoff=0 playable=0';
  };
  const stageLines = stages.map(s =>
    `STAGE ${s.id} ${s.background} ${s.ecl} ${s.message} ${s.logo} ${s.music[0]} ${s.music[1]}`);
  const input = [...stageLines, 'DEFAULT',
    ...stages.map(s => `CHECK ${s.id} ${s.ecl} 1 0`),
    'CHECK 2 st_a1_1.ecl 1 0',
    'CHECK 17 st_x.ecl 1 0',
    'CHECK 1 st_a1_1.ecl 4 0',
    'CHECK 16 st_ex.ecl 4 0',
    'CHECK 1 ../st_a1_1.ecl 1 0',
    'TRANSITION 1 st_a1_1.ecl 1 0 2 st_a1_2.ecl 1 0 0 0',
    'TRANSITION 1 st_a1_1.ecl 1 0 4 st_a2_2.ecl 1 1 0 1',
    'TRANSITION 1 st_a1_1.ecl 1 0 4 st_a2_2.ecl 1 0 0 1',
    'TRANSITION 1 st_a1_1.ecl 1 0 2 st_a1_2.ecl 1 1 0 0',
    'TRANSITION 2 st_a1_2.ecl 1 0 3 st_a1_3.ecl 1 0 0 0',
    'TRANSITION 4 st_a2_2.ecl 1 1 5 st_a2_3.ecl 1 1 0 0',
    'TRANSITION 7 st_b1_2.ecl 1 0 8 st_b1_3.ecl 1 0 0 0',
    'TRANSITION 9 st_b2_2.ecl 1 1 10 st_b2_3.ecl 1 1 0 0',
    'TRANSITION 12 st_c1_2.ecl 1 0 13 st_c1_3.ecl 1 0 0 0',
    'TRANSITION 14 st_c2_2.ecl 1 1 15 st_c2_3.ecl 1 1 0 0',
    'TRANSITION 6 st_b1_1.ecl 1 0 7 st_b1_2.ecl 1 0 0 0',
    'TRANSITION 6 st_b1_1.ecl 1 0 9 st_b2_2.ecl 1 1 0 1',
    'TRANSITION 11 st_c1_1.ecl 1 0 12 st_c1_2.ecl 1 0 0 0',
    'TRANSITION 11 st_c1_1.ecl 1 0 14 st_c2_2.ecl 1 1 0 1',
    'TRANSITION 1 st_a1_1.ecl 1 0 4 st_a2_2.ecl 2 1 0 1',
    'TRANSITION 3 st_a1_3.ecl 1 0 1 st_a1_1.ecl 1 0 0 0',
    'TRANSITION 16 st_ex.ecl 1 0 1 st_a1_1.ecl 1 0 0 0',
    'TRANSITION 1 st_a1_1.ecl 1 0 16 st_ex.ecl 1 0 0 0',
    'TRANSITION 2 st_a1_2.ecl 1 0 1 st_a1_1.ecl 1 0 0 0',
    'TRANSITION 1 st_a1_1.ecl 1 0 2 st_a1_1.ecl 1 0 0 0',
    'TRANSITION 1 st_a1_1.ecl 1 0 2 st_a1_2.ecl 2 0 0 0',
  ];
  const expected = [
    'default stage=0 ecl= difficulty=0 progress=0 role=none',
    ...stages.map(s => `valid role=${role(s.id)} ${caps(s.id)}`),
    ...['invalid', 'invalid', 'invalid', 'invalid', 'invalid'],
    'accepted', 'accepted', 'rejected', 'rejected', 'accepted', 'accepted',
    'accepted', 'accepted', 'accepted', 'accepted', 'accepted', 'accepted',
    'accepted', 'accepted', 'rejected', 'rejected', 'rejected', 'rejected',
    'rejected', 'rejected', 'rejected',
  ];

  assert.ok(fs.existsSync(compiler), `emscripten compiler missing: ${compiler}`);
  assert.ok(fs.existsSync(python), `emsdk python missing: ${python}`);
  fs.mkdirSync(scratch, {recursive: true});
  const cache = path.join(scratch, 'emcache');
  if (!fs.existsSync(path.join(cache, 'sanity.txt'))) {
    const shared = path.resolve(root, '../../work/emscripten-cache-4.0.22');
    assert.ok(fs.existsSync(shared), `shared emscripten cache missing: ${shared}`);
    fs.cpSync(shared, cache, {recursive: true,
      filter: source => !path.basename(source).endsWith('.lock')});
  }
  const temp = path.join(scratch, 'emtemp');
  fs.mkdirSync(path.join(temp, 'emscripten_temp'), {recursive: true});
  const output = path.join(scratch, 'stage-session-identity.cjs');
  const environment = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: cache.replaceAll('\\', '/'),
    EMCC_TEMP_DIR: temp,
    EMCC_DEBUG: '1',
    EMSDK_PYTHON: python,
    EMCC_CORES: '2',
    BINARYEN_CORES: '2'};
  const build = spawnSync(python, [compiler,
    'tests/native/stage-session-identity.cpp', 'src/game/StageSessionIdentity.cpp',
    'src/game/RouteHandoffRules.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
    '-o', output], {cwd: root, env: environment, encoding: 'utf8', timeout: 240000,
    maxBuffer: 16 * 1024 * 1024});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);

  const run = spawnSync(process.execPath, [output], {cwd: root, input: input.join('\n') + '\n',
    encoding: 'utf8', timeout: 30000, maxBuffer: 4 * 1024 * 1024});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  const actual = run.stdout.trim().split(/\r?\n/);
  assert.equal(actual.length, expected.length, actual.join('\n'));
  expected.forEach((want, index) => {
    if (want === 'accepted' || want === 'default stage=0 ecl= difficulty=0 progress=0 role=none')
      assert.equal(actual[index], want);
    else if (want === 'invalid' || want === 'rejected')
      assert.match(actual[index], new RegExp(`^${want} `));
    else
      assert.equal(actual[index], want);
  });
});
