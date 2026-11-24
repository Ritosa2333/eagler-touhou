import test from 'node:test';
import assert from 'node:assert/strict';
import {firstDivergence, makeDocument, parseArguments, parseDigestLog, SCHEMA}
  from '../tools/regression-digest.mjs';

const header = 'th13-regression-digest/v1';
const sample = [
  header,
  'stage 0 frame 0 state 0123456789abcdef events fedcba9876543210 stepped 1',
  'stage 0 frame 60 state 1111111111111111 events 2222222222222222 stepped 1',
  'stage 1 frame 0 state 3333333333333333 events 4444444444444444 draws 5555555555555555 stepped 0',
  'total stages 2 frames 61 state aaaaaaaaaaaaaaaa events bbbbbbbbbbbbbbbb draws cccccccccccccccc',
].join('\n') + '\n';

test('解析 digest 日志：头、采样行、draws/stepped 与 total', () => {
  const parsed = parseDigestLog(sample);
  assert.equal(parsed.header, header);
  assert.equal(parsed.rows.length, 3);
  assert.deepEqual(parsed.rows[0],
    {stage: 0, frame: 0, state: '0123456789abcdef', events: 'fedcba9876543210',
     draws: null, stepped: true});
  assert.equal(parsed.rows[2].draws, '5555555555555555');
  assert.equal(parsed.rows[2].stepped, false);
  assert.deepEqual(parsed.total,
    {stages: 2, frames: 61, state: 'aaaaaaaaaaaaaaaa', events: 'bbbbbbbbbbbbbbbb',
     draws: 'cccccccccccccccc'});
});

test('拒绝缺头、坏十六进制、回退帧号与未知行', () => {
  assert.throws(() => parseDigestLog('stage 0 frame 0 state 0123456789abcdef events ' +
    'fedcba9876543210 stepped 1\n'), /missing .*header/);
  assert.throws(() => parseDigestLog(`${header}\nstage 0 frame 0 state xyz events ` +
    'fedcba9876543210\n'), /malformed.*hex/);
  assert.throws(() => parseDigestLog(`${header}\n` +
    'stage 0 frame 60 state 0123456789abcdef events fedcba9876543210\n' +
    'stage 0 frame 0 state 0123456789abcdef events fedcba9876543210\n'), /order regressed/);
  assert.throws(() => parseDigestLog(`${header}\nhello world\n`), /unrecognized/);
  assert.throws(() => parseDigestLog(`${header}\nstage 0 frame 0 state ` +
    '0123456789abcdef events fedcba9876543210 stepped 7\n'), /stepped/);
});

const document = (rows, overrides = {}) => makeDocument({
  mode: 'replay',
  stride: 60,
  replays: [{
    file: 'case.rpy', source: 'replays', sha256: 'a'.repeat(64), stages: 2, frames: 61,
    completed: true, stop: null, rows,
  }],
  build: {wasmSha256: 'wasm-a'},
  ...overrides,
});

const goldenRows = () => [
  {stage: 0, frame: 0, state: '0123456789abcdef', events: 'fedcba9876543210', draws: null,
   stepped: true},
  {stage: 0, frame: 60, state: '1111111111111111', events: '2222222222222222', draws: null,
   stepped: true},
];

test('golden 形状固定：schema/mode/stride/corpus 行与 build provenance', () => {
  const golden = document(goldenRows());
  assert.equal(golden.schema, SCHEMA);
  assert.equal(golden.mode, 'replay');
  assert.equal(golden.stride, 60);
  assert.equal(golden.drawsFrames, undefined);
  assert.equal(golden.corpus[0].file, 'case.rpy');
  assert.equal(golden.corpus[0].rows[0].draws, null);
  assert.equal(golden.corpus[0].stable, true);
  assert.equal(golden.build.wasmSha256, 'wasm-a');
  const draws = document([{stage: 0, frame: 0, state: '0123456789abcdef',
                            events: 'fedcba9876543210', draws: '5555555555555555',
                            stepped: false}],
                          {mode: 'draws', drawsFrames: 600});
  assert.equal(draws.drawsFrames, 600);
  assert.equal(draws.corpus[0].rows[0].draws, '5555555555555555');
});

test('首差报告：相同返回 null，状态流差异给出 replay/stage/frame/stream', () => {
  const golden = document(goldenRows());
  assert.equal(firstDivergence(golden, document(goldenRows())), null);
  const changed = goldenRows();
  changed[1].state = 'ffffffffffffffff';
  assert.deepEqual(firstDivergence(golden, document(changed)),
    {replay: 'case.rpy', stage: 0, frame: 60, stream: 'state',
     expected: '1111111111111111', actual: 'ffffffffffffffff'});
});

test('首差报告：事件流、行缺失、sha 与构建身份', () => {
  const golden = document(goldenRows());
  const events = goldenRows();
  events[0].events = '0000000000000000';
  assert.equal(firstDivergence(golden, document(events)).stream, 'events');

  const missing = goldenRows().slice(0, 1);
  const diff = firstDivergence(golden, document(missing));
  assert.equal(diff.stream, 'rows-missing');
  assert.equal(diff.frame, 60);

  const sha = document(goldenRows());
  sha.corpus[0].sha256 = 'b'.repeat(64);
  assert.equal(firstDivergence(golden, sha).stream, 'replay-sha256');

  const build = document(goldenRows());
  build.build = {wasmSha256: 'wasm-b'};
  assert.equal(firstDivergence(golden, build), null, 'build 身份不参与比较');
});

test('首差报告：draws 与 stepped 各自成流', () => {
  const drawsGolden = document([{stage: 0, frame: 0, state: '0123456789abcdef',
                                 events: 'fedcba9876543210', draws: '5555555555555555',
                                 stepped: false}], {mode: 'draws', drawsFrames: 600});
  const drawsCurrent = document([{stage: 0, frame: 0, state: '0123456789abcdef',
                                  events: 'fedcba9876543210', draws: '6666666666666666',
                                  stepped: false}], {mode: 'draws', drawsFrames: 600});
  assert.equal(firstDivergence(drawsGolden, drawsCurrent).stream, 'draws');
  const stepped = document([{stage: 0, frame: 0, state: '0123456789abcdef',
                             events: 'fedcba9876543210', draws: null, stepped: false}]);
  assert.equal(firstDivergence(document(goldenRows()), stepped).stream, 'stepped');
});

test('稳定性：不稳定条目不比较摘要，稳定条目变不稳定即失败', () => {
  const unstable = overrides => makeDocument({
    mode: 'replay', stride: 60,
    replays: [{file: 'case.rpy', source: 'replays', sha256: 'a'.repeat(64), stages: 1,
               frames: 100, completed: false, stop: {stage: 0, frame: 60},
               rows: [], ...overrides}],
    build: null,
  });
  assert.equal(firstDivergence(unstable({}), unstable({completed: true, stable: true})),
    null, '金标不稳定时允许后续跑通');
  const stable = document(goldenRows());
  const broken = unstable({});
  assert.equal(firstDivergence(stable, broken).stream, 'stability');
});

test('CLI 参数：默认 stride=60、draws 窗 600，非法值报错', () => {
  assert.deepEqual(parseArguments([]),
    {check: false, draws: false, stride: 60, drawsFrames: 600, drawsStages: 'first',
     attempts: 1, confirmations: 2, replays: [], keep: false, help: false});
  const parsed = parseArguments(['--check', '--draws', '--stride', '1', '--replay', 'a.rpy',
                                 '--replay', 'b.rpy', '--draws-frames', '120', '--keep']);
  assert.equal(parsed.check, true);
  assert.equal(parsed.draws, true);
  assert.equal(parsed.stride, 1);
  assert.deepEqual(parsed.replays, ['a.rpy', 'b.rpy']);
  assert.equal(parsed.drawsFrames, 120);
  assert.equal(parseArguments(['--draws', '--draws-stages', 'all']).drawsStages, 'all');
  assert.throws(() => parseArguments(['--draws-stages', 'some']), /first or all/);
  assert.throws(() => parseArguments(['--stride', '0']), /positive integer/);
  assert.throws(() => parseArguments(['--bogus']), /unknown argument/);
});
