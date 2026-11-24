import test from 'node:test';
import assert from 'node:assert/strict';
import {guestPath, harnessPaths, validateHarnessOutput} from '../tools/harness-paths.mjs';

const workspace = '/work';

test('WASI 路径区分宿主前缀与 guest 根', () => {
  assert.equal(guestPath(workspace, '/work/artifacts/port-draws/new.jsonl'),
    '/artifacts/port-draws/new.jsonl');
  assert.equal(guestPath(workspace, '/artifacts/port-draws/new.jsonl'),
    '/artifacts/port-draws/new.jsonl');
  assert.equal(guestPath(workspace, 'artifacts/reference/raw'), '/artifacts/reference/raw');
});

test('标题输出归一到外层工作区，而不是 work/work', () => {
  const {args, outputs} = harnessPaths(workspace,
    ['title', 'artifacts/reference/raw', '/work/artifacts/port-draws/title-new.jsonl']);
  assert.equal(args[2], '/artifacts/reference/raw');
  assert.equal(args[3], '/artifacts/port-draws/title-new.jsonl');
  assert.deepEqual(outputs, [{guest: args[3], host: '/work/artifacts/port-draws/title-new.jsonl'}]);
});

test('replay 输出参数与帧数、stage 参数保持分离', () => {
  const {args, outputs} = harnessPaths(workspace,
    ['replay', 'games/replays/case.rpy', 'artifacts/reference/raw', '/artifacts/replay.jsonl', '400', '0']);
  assert.equal(args[2], '/games/replays/case.rpy');
  assert.equal(args[4], '/artifacts/replay.jsonl');
  assert.equal(args[5], '400');
  assert.equal(args[6], '0');
  assert.equal(outputs.length, 1);
});

test('无资源 rng 冒烟不强制要求输出文件', () => {
  assert.deepEqual(harnessPaths(workspace, []).outputs, []);
});

test('拒绝缺输入、越界输出以及空输出', () => {
  assert.throws(() => harnessPaths(workspace, ['draws-replay']), /missing input/);
  assert.throws(() => harnessPaths(workspace, ['title', '', '/../escape.jsonl']), /inside the workspace/);
  assert.throws(() => validateHarnessOutput({host: '/work/out'}, null, null), /nonempty/);
  assert.throws(() => validateHarnessOutput({host: '/work/out'}, null, {size: 0n}), /nonempty/);
});

test('replay/draws-replay 把 digest 日志声明为输出（建目录 + 非空校验）', () => {
  const env = {TH13_DIGEST_LOG: '/work/artifacts/digests/case.log'};
  const replay = harnessPaths(workspace,
    ['replay', 'games/replays/case.rpy', 'artifacts/reference/raw', '/artifacts/e.jsonl', '0', '0'],
    env);
  assert.deepEqual(replay.outputs.map(output => output.host),
    ['/work/artifacts/e.jsonl', '/work/artifacts/digests/case.log']);
  const draws = harnessPaths(workspace,
    ['draws-replay', 'games/replays/case.rpy', 'artifacts/reference/raw', '/artifacts/d.jsonl'],
    env);
  assert.equal(draws.outputs.length, 2);
  assert.equal(draws.outputs[1].guest, '/artifacts/digests/case.log');
  const relative = harnessPaths(workspace,
    ['replay', 'a.rpy', 'artifacts/reference/raw', '/artifacts/e.jsonl'],
    {TH13_DIGEST_LOG: 'artifacts/digests/x.log'});
  assert.equal(relative.outputs[1].guest, '/artifacts/digests/x.log');
  assert.equal(harnessPaths(workspace, ['stage1', 'artifacts/reference/raw', '/artifacts/s.jsonl'],
    env).outputs.length, 1);
  assert.throws(() => harnessPaths(workspace,
    ['replay', 'a.rpy', 'artifacts/reference/raw', '/artifacts/e.jsonl'],
    {TH13_DIGEST_LOG: '/../escape.log'}), /inside the workspace/);
});

test('旧文件存在不能冒充本次产物', () => {
  const old = {size: 120n, ino: 3n, mtimeNs: 100n, ctimeNs: 100n};
  assert.throws(() => validateHarnessOutput({host: '/work/out'}, old, {...old}), /not regenerated/);
  assert.doesNotThrow(() => validateHarnessOutput({host: '/work/out'}, old,
    {...old, mtimeNs: 101n, ctimeNs: 101n}));
  assert.doesNotThrow(() => validateHarnessOutput({host: '/work/out'}, null, old));
});
