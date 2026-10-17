import assert from 'node:assert/strict';
import test from 'node:test';
import {createTh09SpectatorTimelineModel, dispatchTh09SpectatorAction} from '../../sdl-runtime/spectator-timeline.mjs';

const words = ({played = 600, received = 700, edge = 680, target = 645, paused = 0,
  seeking = 1, following = 0, start = 10, round = 2, ended = 0} = {}) =>
  [1, played, received, edge, target, paused, seeking, following, start, round, ended];

test('history view translates TH09 seeking frames and round/match marks into shared model units', () => {
  const model = createTh09SpectatorTimelineModel({
    words: words(), key: 'th09:3', pendingMatches: 2,
    historyView: {min: 20, position: 500, edge: 650, total: 700, marks: [
      {position: 20, kind: 'match', label: '第 1 局', active: false},
      {position: 545, kind: 'round', label: '第 2 回合'},
    ]},
  });
  assert.equal(model.key, 'th09:3');
  assert.equal(model.position, 545);
  assert.equal(model.min, 20);
  assert.equal(model.edge, 650);
  assert.equal(model.received, 700);
  assert.equal(model.target, 545);
  assert.equal(model.timeOrigin, 0);
  assert.equal(model.statusText, '满速定位 → 0:09 · 2 局待播放');
  assert.deepEqual(model.marks.map(({frame, kind, disabled, current}) => ({frame, kind, disabled, current})), [
    {frame: 20, kind: 'match', disabled: false, current: false},
    {frame: 545, kind: 'round', disabled: true, current: true},
  ]);
});

test('without history the model uses TH09 native frontiers and match-relative time', () => {
  const model = createTh09SpectatorTimelineModel({
    words: words({played: 620, received: 720, edge: 700, target: 645, seeking: 0, following: 1}),
    matchStart: 600,
    historyView: null,
  });
  assert.equal(model.position, 620);
  assert.equal(model.min, 620);
  assert.equal(model.edge, 700);
  assert.equal(model.received, 720);
  assert.equal(model.timeOrigin, 600);
  assert.equal(model.statusText, '直播');
  assert.equal(model.durationText, '已进行 0 分钟');
  assert.equal(model.liveDisabled, false);
});

test('host-provided elapsed label remains ahead of the adapter fallback', () => {
  const model = createTh09SpectatorTimelineModel({words: words(), historyView: null,
    matchStart: 600, elapsedLabel: '房间实际时长'});
  assert.equal(model.durationText, '房间实际时长');
});

test('pause, play, seek and live keep TH09 numeric control mapping behind adapter', () => {
  const calls = [];
  const core = {_th09_spectator_control: (action, frame) => {calls.push([action, frame]); return true;}};
  let liveCalls = 0, seekFrame = null;
  const common = {onLive: () => {++liveCalls;}, onSeek: frame => {seekFrame = frame;},
    readInfo: () => words({played: 30, edge: 80}), history: () => null};
  dispatchTh09SpectatorAction(core, 'pause', undefined, common);
  dispatchTh09SpectatorAction(core, 'play', undefined, common);
  dispatchTh09SpectatorAction(core, 'seek', 100, common);
  dispatchTh09SpectatorAction(core, 'live', undefined, common);
  assert.deepEqual(calls, [[0, 0], [3, 0], [1, 80]]);
  assert.equal(liveCalls, 1);
  assert.equal(seekFrame, null);
});

test('history seeking clamps to retained tape and delegates to the TH09 tape owner', () => {
  const calls = [];
  const core = {_th09_spectator_control: (...args) => calls.push(args)};
  let seekFrame = null;
  dispatchTh09SpectatorAction(core, 'seek', 99, {
    onLive() {}, onSeek: frame => {seekFrame = frame;},
    readInfo: () => words({played: 40}), history: () => ({min: 10, edge: 70}),
  });
  assert.equal(seekFrame, 70);
  assert.deepEqual(calls, []);
});

test('paused, ended and following states remain separate in the displayed status', () => {
  assert.equal(createTh09SpectatorTimelineModel({words: words({paused: 1}), historyView: null}).statusText, '已暂停');
  assert.equal(createTh09SpectatorTimelineModel({words: words({played: 700, received: 700, edge: 700, seeking: 0, ended: 1}), historyView: null}).statusText, '本局已结束，等待下一局');
  assert.equal(createTh09SpectatorTimelineModel({words: words({played: 600, edge: 600, received: 0, seeking: 0}), historyView: null, selectionWaiting: true}).statusText, '等待双方在游戏内选角');
});
