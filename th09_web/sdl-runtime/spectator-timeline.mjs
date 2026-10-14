// Playback controls own only this receive-only viewer's native cursor.
export function attachSpectatorTimeline(core, {onLive=()=>core._th09_spectator_control(2,0)}={}) {
  if (typeof document.createElement !== 'function') return null;
  const panel = document.createElement('section');
  panel.id = 'spectatorTimeline'; panel.setAttribute('aria-label', '观战播放控制');
  panel.dataset.collapsed = 'true';
  panel.innerHTML = '<div class="spectator-bar"><div id="spectatorMatchStats" aria-label="比分与对决时长"><strong id="spectatorScore"></strong><span id="spectatorMatchDuration"></span></div><button id="spectatorToggle" type="button" aria-expanded="false" aria-controls="spectatorDetails" aria-label="展开观战控制"><img class="spectator-control-icon" data-icon="toggle" alt=""></button></div><div id="spectatorDetails" hidden><div id="spectatorRoundMarks" aria-label="回合起点"></div><input id="spectatorSeek" type="range" min="0" max="0" step="1" value="0" aria-label="向前快进"><div class="spectator-detail-bar"><div class="spectator-playback-meta"><span id="spectatorPlaybackStatus" role="status"></span><span id="spectatorLag"></span></div><div class="spectator-actions"><button id="spectatorPause" type="button"><img class="spectator-control-icon spectator-pause-icon" data-icon="pause" alt=""><img class="spectator-control-icon spectator-play-icon" data-icon="play" alt=""><span id="spectatorPauseLabel">暂停</span></button><button id="spectatorLive" type="button"><img class="spectator-control-icon" data-icon="live" alt=""><span>回到直播</span></button></div></div></div>';
  document.body.append(panel);
  document.body.classList.add('spectator-view');
  const toggle = panel.querySelector('#spectatorToggle'), details = panel.querySelector('#spectatorDetails');
  toggle.addEventListener('click', () => {
    const expanded = toggle.getAttribute('aria-expanded') !== 'true';
    toggle.setAttribute('aria-expanded', String(expanded));
    toggle.setAttribute('aria-label', expanded ? '收起观战控制' : '展开观战控制');
    details.hidden = !expanded; panel.dataset.collapsed = String(!expanded);
  });
  const range = panel.querySelector('#spectatorSeek'), pause = panel.querySelector('#spectatorPause');
  const live = panel.querySelector('#spectatorLive');
  const marks = panel.querySelector('#spectatorRoundMarks');
  let latest, editing = false, pendingMatches = 0, matchStart = 0;
  const rounds = [];
  const clock = n => `${Math.floor(n / 3600)}:${String(Math.floor(n / 60) % 60).padStart(2, '0')}`;
  const info = () => Array.from(core.HEAPU32.subarray(core._th09_spectator_playback_info() / 4,
    core._th09_spectator_playback_info() / 4 + 11));
  function refresh(nextMatches = pendingMatches) {
    pendingMatches = nextMatches;
    latest = info();
    const [, played, received, edge, target, paused, seeking, following, start, round, ended] = latest;
    range.min = String(matchStart); range.max = String(Math.max(matchStart, edge));
    range.disabled = edge <= played;
    if (!editing) range.value = String(played);
    range.setAttribute('aria-valuetext', clock(Math.max(0, played - matchStart)));
    panel.querySelector('#spectatorMatchDuration').textContent = `已进行 ${Math.floor(Math.max(0, received - matchStart) / 3600)} 分钟`;
    panel.querySelector('#spectatorPlaybackStatus').textContent = paused ? '已暂停' : seeking ? `4 倍速 → ${clock(Math.max(0, target - start))}` : edge <= played ? ended ? '本局已结束，等待下一局' : '等待对局输入' : following ? ended ? '对局已结束' : '直播' : '播放中';
    if(pendingMatches)panel.querySelector('#spectatorPlaybackStatus').textContent += ` · ${pendingMatches} 局待播放`;
    panel.querySelector('#spectatorLag').textContent = `落后 ${Math.ceil((received - played) / 60)} 秒`;
    panel.querySelector('#spectatorPauseLabel').textContent = paused ? '继续' : '暂停';
    pause.dataset.paused=String(!!paused);
    live.disabled = edge <= played && !pendingMatches;
    const fraction = edge > matchStart ? Math.min(100, Math.max(0, 100 * (played - matchStart) / (edge - matchStart))) : 0;
    range.style.setProperty('--played', `${fraction}%`);
    marks.hidden = !rounds.some(point => point.frame <= edge);
    const next = rounds.find(point => point.frame > played && point.frame <= edge);
    for(const point of rounds){
      point.button.hidden = point.frame > edge;
      point.button.disabled = point.frame <= played;
      const at = edge > matchStart ? 100 * (point.frame - matchStart) / (edge - matchStart) : 0;
      point.button.style.left = `${at}%`;
      point.button.dataset.align = at < 15 ? 'left' : at > 85 ? 'right' : 'center';
      point.button.textContent = String(point.round + 1);
      point.button.dataset.next = String(point === next);
    }
  }
  range.addEventListener('pointerdown', () => { editing = true; });
  range.addEventListener('input', () => { const state = info(); range.value = String(Math.max(state[1], Number(range.value))); });
  range.addEventListener('change', () => {
    const state = info(); core._th09_spectator_control(1, Math.min(state[3], Math.max(state[1], Number(range.value))));
    editing = false; refresh();
  });
  range.addEventListener('pointercancel', () => { editing = false; refresh(); });
  pause.addEventListener('click', () => { core._th09_spectator_control(info()[5] ? 3 : 0, 0); refresh(); });
  live.addEventListener('click', () => { onLive(); refresh(); });
  refresh();
  return {refresh,
    resetMatch(frame) { matchStart=frame;rounds.length=0;marks.replaceChildren();refresh(); },
    addRound(frame,round) {
      const button=document.createElement('button');button.type='button';button.className='spectator-round-mark';
      button.dataset.frame=String(frame);button.dataset.round=String(round);
      button.setAttribute('aria-label',`第 ${round + 1} 回合 · ${clock(frame-matchStart)}`);
      button.title=button.getAttribute('aria-label');
      button.addEventListener('click',()=>{const state=info();if(frame>state[1]&&frame<=state[3])core._th09_spectator_control(1,frame);refresh();});
      rounds.push({frame,round,button});marks.append(button);refresh();
    },
    close() { panel.remove(); document.body.classList.remove('spectator-view'); }};
}
