// 房间选项到会话身份的映射契约。线上词表与 session_setup_wire_test.cpp 的
// C++ 向量含义相同：任一侧改动线格式或校验规则，另一侧必须显式跟进。
import test from 'node:test';
import assert from 'node:assert/strict';
import {multiplayerBuildWords, multiplayerSetupWords, validateMultiplayerOptions}
  from '../../web/sdl-runtime/multiplayer.mjs';

const manifest = {game: 'th13', product: 'th13mp', variant: 'multiplayer',
  features: {multiplayer: true}, execution: {sha256: '11'.repeat(32)}};
const build = [0x11111111, 0x11111111, 0x11111111, 0x11111111];
const room = {
  netplayUrl: 'wss://relay.example/th13?room=th13mp-abcd&run=r1', netplayPlayerCount: 2, netplayPlayer: 0,
  netplayDifficulty: 2, netplaySeed: 0x1234, netplayMode: 'lan',
  netplayLoadouts: [{character: 0, shot: 0}, {character: 3}],
};
// 会话号 = SHA-256('th13mp:' + 房间 + ':' + 对局) 的前两词（小端）。
const session = [1657783559, 1038279571];
const identity = (delay, mode, automatic, reserve, flags) =>
  [2, 2, 0, 2, 0x1234, ...session, delay, 0, 3, 0, 0, 0, 0, mode, automatic, reserve, ...build, flags];
const rejection = options => {
  try { validateMultiplayerOptions(options); return 'accepted'; }
  catch (error) { return error.message; }
};

test('session identity words cover rollback, manual delay and challenge rooms', async () => {
  const rollback = await multiplayerSetupWords(room, build);
  assert.deepEqual(rollback.words, identity(0, 2, 0, 2, 0));
  assert.equal(rollback.config.mode, 2);
  assert.equal(rollback.config.delay, 0);

  const delay = await multiplayerSetupWords({...room, netplayAdonisMode: 1, netplayInputDelay: 4}, build);
  assert.deepEqual(delay.words, identity(4, 1, 0, 2, 0));
  assert.equal(delay.config.mode, 1);

  const challenge = await multiplayerSetupWords({...room, netplayChallengeMode: true,
    netplayChallengeKeepBullets: true}, build);
  assert.deepEqual(challenge.words, identity(0, 2, 0, 2, 3));
  assert.equal(challenge.config.challenge, true);
  assert.equal(challenge.config.keepBulletsOnMiss, true);
});

test('timing defaults differ between rollback and manual delay', () => {
  assert.equal(validateMultiplayerOptions(room).delay, 0);
  assert.equal(validateMultiplayerOptions({...room, netplayAdonisMode: 1}).delay, 2);
  // 现状：回滚模式带非零延迟被接受，难度 5 在运行时桥接层合法。
  const rollbackWithDelay = validateMultiplayerOptions({...room, netplayInputDelay: 3});
  assert.equal(rollbackWithDelay.mode, 2);
  assert.equal(rollbackWithDelay.delay, 3);
  assert.equal(validateMultiplayerOptions({...room, netplayDifficulty: 5}).delay, 0);
});

test('out-of-range and conflicting options are rejected with their reasons', () => {
  assert.equal(rejection({...room, thpracEnabled: true}), 'TH13 多人模式不支持 THPrac');
  assert.equal(rejection({...room, netplayMode: 'internet'}), '请从多人房间启动');
  assert.equal(rejection({...room, netplayPlayer: 0, replayViewer: true, netplayMode: 'replay',
    netplaySpectator: false}), 'accepted');
  const invalid = 'TH13 多人连接或输入时序配置无效';
  for (const options of [
    {...room, netplayPlayerCount: 1}, {...room, netplayPlayerCount: 4},
    {...room, netplayPlayer: 2}, {...room, netplayPlayer: 1.5},
    {...room, netplayDifficulty: 6}, {...room, netplayDifficulty: -1},
    {...room, netplaySeed: 65536}, {...room, netplaySeed: -1},
    {...room, netplayAdonisMode: 1, netplayInputDelay: 10},
    {...room, netplayPredictionReserve: 0}, {...room, netplayPredictionReserve: 3},
    {...room, netplayUrl: 'https://relay.example/th13?room=th12mp-abcd&run=r1'},
    {...room, netplayUrl: 'wss://user:pass@relay.example/th13?room=th13mp-abcd&run=r1'},
  ]) assert.equal(rejection(options), invalid);
  assert.equal(rejection({...room, netplayLoadouts: [{character: 0}, {character: 4}]}), 'TH13 多人机体配置无效');
  assert.equal(rejection({...room, netplayLoadouts: [{character: 0}]}), 'TH13 多人机体配置无效');
});

test('spectator and replay entry points keep their own rules', () => {
  const spectator = validateMultiplayerOptions({...room, netplayPlayer: 1, netplaySpectator: true,
    netplaySpectatorId: 'viewer-1'});
  assert.equal(spectator.spectator, true);
  assert.equal(spectator.seat, 0);
  assert.equal(rejection({...room, netplaySpectator: true}), '观战身份缺失');
  assert.deepEqual(validateMultiplayerOptions({...room, replayViewer: true, netplayMode: 'replay'}), {replay: true});
  assert.equal(rejection({...room, replayViewer: true}), '录像与联机不能同时启动');
});

test('challenge options validate their types and dependency', () => {
  assert.equal(rejection({...room, netplayChallengeKeepBullets: true}),
    'Invalid TH13 challenge bullet clearing setting');
  assert.equal(rejection({...room, netplayChallengeMode: 'yes'}), '挑战模式配置无效');
  assert.equal(rejection({...room, netplayChallengeKeepBullets: 1, netplayChallengeMode: true}),
    'Invalid TH13 challenge bullet clearing setting');
});

test('build words come from the runtime execution digest', () => {
  assert.deepEqual(multiplayerBuildWords(manifest), build);
  assert.throws(() => multiplayerBuildWords({...manifest, product: 'th13'}),
    /TH13 多人 Runtime 身份无效/);
  assert.throws(() => multiplayerBuildWords({...manifest, execution: {sha256: 'abc'}}),
    /TH13 多人 Runtime 身份无效/);
});
