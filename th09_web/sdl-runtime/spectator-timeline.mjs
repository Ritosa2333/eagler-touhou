import {createSpectatorTimelineView} from './spectator-timeline-view.mjs';

const clamp = (value, low, high) => Math.max(low, Math.min(high, value));
const clock = frame => `${Math.floor(frame / 3600)}:${String(Math.floor(frame / 60) % 60).padStart(2, '0')}`;

function readPlaybackInfo(core) {
  const pointer = core._th09_spectator_playback_info() / 4;
  return Array.from(core.HEAPU32.subarray(pointer, pointer + 11));
}

/** Convert TH09's fixed eleven-word native report into the shared view model. */
export function createTh09SpectatorTimelineModel({words, historyView, matchStart = 0, pendingMatches = 0,
  key = 'th09:0', selectionWaiting = false, elapsedLabel = null}) {
  const [, played, received, nativeEdge, nativeTarget, paused, seeking, following, start, , ended] = words;
  const min = historyView?.min ?? played;
  const edge = historyView?.edge ?? nativeEdge;
  const position = historyView && seeking
    ? clamp(historyView.position + nativeTarget - played, min, edge)
    : historyView?.position ?? played;
  const marks = (historyView?.marks ?? []).map((mark, index) => {
    const frame = mark.position;
    return {
      id: `${mark.kind}:${frame}:${index}`,
      frame,
      kind: mark.kind === 'match' ? 'match' : 'round',
      label: mark.label,
      active: mark.active === true,
      current: frame === position,
      disabled: frame === position && (mark.kind !== 'match' || mark.active === true),
    };
  });
  const total = historyView?.total ?? received;
  const statusText = paused ? '已暂停'
    : seeking ? `满速定位 → ${historyView ? clock(position) : clock(Math.max(0, nativeTarget - start))}`
    : edge <= position
      ? ended ? '本局已结束，等待下一局'
        : selectionWaiting && !received ? '等待双方在游戏内选角'
          : following ? '直播' : '等待对局输入'
      : following ? ended ? '对局已结束' : '直播'
        : '播放中';

  return {
    key,
    min,
    position,
    edge,
    received: total,
    target: historyView && seeking ? position : Math.max(0, nativeTarget - start),
    paused: paused === 1,
    seeking: seeking === 1,
    following: following === 1,
    ended: ended === 1,
    marks,
    timeOrigin: historyView ? 0 : matchStart,
    statusText: pendingMatches ? `${statusText} · ${pendingMatches} 局待播放` : statusText,
    lagText: `落后 ${Math.ceil(Math.max(0, total - position) / 60)} 秒`,
    durationText: elapsedLabel || `已进行 ${Math.floor(Math.max(0, total - (historyView ? 0 : matchStart)) / 3600)} 分钟`,
    liveDisabled: edge <= position && !pendingMatches,
  };
}

/** Keep the title's numeric ABI behind the shared semantic action names. */
export function dispatchTh09SpectatorAction(core, action, frame, {onLive, onSeek, history, readInfo}) {
  if (action === 'pause') return core._th09_spectator_control(0, 0);
  if (action === 'play') return core._th09_spectator_control(3, 0);
  if (action === 'live') return onLive();
  if (action !== 'seek') return undefined;

  const state = readInfo();
  const view = history(state[1]);
  if (view && onSeek) return onSeek(clamp(frame, view.min, view.edge));
  return core._th09_spectator_control(1, clamp(frame, state[1], state[3]));
}

// Playback controls own only this receive-only viewer's native cursor.
export function attachSpectatorTimeline(core, {onLive = () => core._th09_spectator_control(2, 0),
  onSeek = null, history = () => null, onReturn = () => {}, document: ownerDocument = globalThis.document} = {}) {
  if (typeof ownerDocument?.createElement !== 'function') return null;

  const view = createSpectatorTimelineView({
    document: ownerDocument,
    mount: ownerDocument.body,
    labels: {summary: '比分与对决时长', nodes: '对局与回合起点'},
    onAction: (action, frame) => {
      const result = dispatchTh09SpectatorAction(core, action, frame, {onLive, onSeek, history, readInfo: () => readPlaybackInfo(core)});
      if (result && typeof result.then === 'function') return result.finally(() => refresh());
      refresh();
      return result;
    },
  });
  const panel = view.panel;
  ownerDocument.body.classList.add('spectator-view');

  const waiting = ownerDocument.createElement('dialog');
  waiting.id = 'spectatorWaiting';
  waiting.setAttribute('aria-labelledby', 'spectatorWaitingTitle');
  waiting.innerHTML = '<h2 id="spectatorWaitingTitle">本局已结束</h2><p>正在等待玩家开始下一局。你也可以返回房间。</p><div><button type="button" data-wait>继续等待</button><button type="button" data-return>返回房间</button></div>';
  ownerDocument.body.append(waiting);
  waiting.querySelector('[data-wait]').addEventListener('click', () => waiting.close());
  waiting.querySelector('[data-return]').addEventListener('click', () => {waiting.close(); onReturn();});

  let latest = null, pendingMatches = 0, matchStart = 0, notifiedEnd = null, generation = 0;
  const rounds = [];

  function refresh(nextMatches = pendingMatches) {
    pendingMatches = nextMatches;
    const words = readPlaybackInfo(core);
    const [, played, received, , , , , , , , ended] = words;
    const historyView = history(played);
    const key = `th09:${generation}`;
    latest = createTh09SpectatorTimelineModel({words, historyView, matchStart, pendingMatches,
      key, selectionWaiting: !!core.eaglerOptions?.netplayInGameSelection, elapsedLabel: panel.dataset.elapsedLabel});
    if (!historyView && rounds.length) {
      latest.marks = rounds.map((point, index) => ({
        id: `round:${point.frame}:${point.round}:${index}`,
        frame: point.frame,
        kind: 'round',
        label: `第 ${point.round + 1} 回合`,
        current: point.frame === latest.position,
        disabled: point.frame === latest.position,
      }));
    }
    if (waiting.open && (pendingMatches || historyView && latest.position < historyView.total)) waiting.close();
    if (ended && played >= received && !pendingMatches && (!historyView || latest.position >= historyView.total) && notifiedEnd !== received) {
      notifiedEnd = received;
      if (!waiting.open && typeof waiting.showModal === 'function') waiting.showModal();
    }
    view.update(latest);
    return latest;
  }

  refresh();
  return {
    refresh,
    resetMatch(frame) {
      waiting.close(); notifiedEnd = null; matchStart = frame; ++generation; rounds.length = 0; refresh();
    },
    addRound(frame, round) {
      if (history(readPlaybackInfo(core)[1])) {refresh(); return;}
      rounds.push({frame, round}); refresh();
    },
    close() {
      waiting.close(); waiting.remove(); view.destroy(); ownerDocument.body.classList.remove('spectator-view');
    },
  };
}
