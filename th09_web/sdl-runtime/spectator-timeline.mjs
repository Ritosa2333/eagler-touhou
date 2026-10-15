// Playback controls own only this receive-only viewer's native cursor.
export function attachSpectatorTimeline(core, {onLive=()=>core._th09_spectator_control(2,0),onReturn=()=>{}}={}) {
  if (typeof document.createElement !== 'function') return null;
  const panel = document.createElement('section');
  panel.id = 'spectatorTimeline'; panel.setAttribute('aria-label', '观战播放控制');
  panel.dataset.collapsed = 'true';
  panel.innerHTML = '<div class="spectator-bar"><div id="spectatorMatchStats" aria-label="比分与对决时长"><strong id="spectatorScore"></strong><span id="spectatorMatchDuration"></span></div><button id="spectatorToggle" type="button" aria-expanded="false" aria-controls="spectatorDetails" aria-label="展开观战控制"><img class="spectator-control-icon" data-icon="toggle" alt=""></button></div><div id="spectatorDetails" hidden><div id="spectatorRoundMarks" aria-label="回合起点"></div><input id="spectatorSeek" type="range" min="0" max="0" step="1" value="0" aria-label="向前快进"><div class="spectator-detail-bar"><div class="spectator-playback-meta"><span id="spectatorPlaybackStatus" role="status"></span><span id="spectatorLag"></span></div><div class="spectator-actions"><button id="spectatorPause" type="button"><img class="spectator-control-icon spectator-pause-icon" data-icon="pause" alt=""><img class="spectator-control-icon spectator-play-icon" data-icon="play" alt=""><span id="spectatorPauseLabel">暂停</span></button><button id="spectatorLive" type="button"><img class="spectator-control-icon" data-icon="live" alt=""><span>回到直播</span></button></div></div></div>';
  document.body.append(panel);
  document.body.classList.add('spectator-view');
  const waiting = document.createElement('dialog');
  waiting.id = 'spectatorWaiting';
  waiting.setAttribute('aria-labelledby', 'spectatorWaitingTitle');
  waiting.innerHTML = '<h2 id="spectatorWaitingTitle">本局已结束</h2><p>正在等待玩家开始下一局。你也可以返回房间。</p><div><button type="button" data-wait>继续等待</button><button type="button" data-return>返回房间</button></div>';
  document.body.append(waiting);
  waiting.querySelector('[data-wait]').addEventListener('click',()=>waiting.close());
  waiting.querySelector('[data-return]').addEventListener('click',()=>{waiting.close();onReturn();});
  let notifiedEnd = null;
  const toggle = panel.querySelector('#spectatorToggle'), details = panel.querySelector('#spectatorDetails');
  const bar = panel.querySelector('.spectator-bar');
  let touchActivationAt = -Infinity;
  function toggleDetails() {
    const expanded = toggle.getAttribute('aria-expanded') !== 'true';
    toggle.setAttribute('aria-expanded', String(expanded));
    toggle.setAttribute('aria-label', expanded ? '收起观战控制' : '展开观战控制');
    details.hidden = !expanded; panel.dataset.collapsed = String(!expanded);
  }
  // Touch may lose its compatibility click to an embedded game's handlers.
  // Handle pointer release directly and ignore only its following mouse click.
  bar.addEventListener('pointerup', event => {
    if (!['touch', 'pen'].includes(event.pointerType)) return;
    if (panel.dataset.collapsed !== 'true' && !toggle.contains(event.target)) return;
    event.preventDefault();
    touchActivationAt = performance.now();
    toggleDetails();
  });
  toggle.addEventListener('click', event => {
    if (event.detail && event.pointerType !== 'mouse' && performance.now() - touchActivationAt < 700) return;
    toggleDetails();
  });
  bar.addEventListener('click', event => {
    if (toggle.contains(event.target) || panel.dataset.collapsed !== 'true') return;
    if (event.detail && event.pointerType !== 'mouse' && performance.now() - touchActivationAt < 700) return;
    toggleDetails();
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
    if (ended && played >= received && !pendingMatches && notifiedEnd !== received) {
      notifiedEnd = received;
      if (!waiting.open) waiting.showModal();
    }
    // Only the still-playable interval is selectable. Freeze both bounds while
    // dragging, otherwise incoming frames move the time under the pointer.
    if (!editing) {
      range.min = String(played); range.max = String(Math.max(played, edge));
      range.disabled = edge <= played; range.value = String(played);
    }
    range.setAttribute('aria-valuetext', clock(Math.max(0, Number(range.value) - matchStart)));
    panel.querySelector('#spectatorMatchDuration').textContent = panel.dataset.elapsedLabel || `已进行 ${Math.floor(Math.max(0, received - matchStart) / 3600)} 分钟`;
    panel.querySelector('#spectatorPlaybackStatus').textContent = paused ? '已暂停' : seeking ? `满速快进 → ${clock(Math.max(0, target - start))}` : edge <= played ? ended ? '本局已结束，等待下一局' : core.eaglerOptions?.netplayInGameSelection && !received ? '等待双方在游戏内选角' : '等待对局输入' : following ? ended ? '对局已结束' : '直播' : '播放中';
    if(pendingMatches)panel.querySelector('#spectatorPlaybackStatus').textContent += ` · ${pendingMatches} 局待播放`;
    panel.querySelector('#spectatorLag').textContent = `落后 ${Math.ceil((received - played) / 60)} 秒`;
    panel.querySelector('#spectatorPauseLabel').textContent = paused ? '继续' : '暂停';
    pause.dataset.paused=String(!!paused);
    live.disabled = edge <= played && !pendingMatches;
    const low=Number(range.min), high=Number(range.max);
    const fraction = high > low ? Math.min(100, Math.max(0, 100 * (Number(range.value) - low) / (high - low))) : 0;
    range.style.setProperty('--played', `${fraction}%`);
    marks.hidden = !rounds.some(point => point.frame > played && point.frame <= high);
    const next = rounds.find(point => point.frame > played && point.frame <= edge);
    for(const point of rounds){
      point.button.hidden = point.frame <= played || point.frame > high;
      point.button.disabled = point.frame <= played;
      const at = high > low ? 100 * (point.frame - low) / (high - low) : 0;
      point.button.style.left = `${at}%`;
      point.button.dataset.align = at < 15 ? 'left' : at > 85 ? 'right' : 'center';
      point.button.textContent = String(point.round + 1);
      point.button.dataset.next = String(point === next);
    }
  }
  range.addEventListener('pointerdown', event => { editing = true;range.setPointerCapture?.(event.pointerId); });
  range.addEventListener('input', () => { range.value = String(Math.max(Number(range.min), Number(range.value))); });
  range.addEventListener('change', () => {
    const state = info(); core._th09_spectator_control(1, Math.min(state[3], Math.max(state[1], Number(range.value))));
    editing = false; refresh();
  });
  range.addEventListener('pointercancel', () => { editing = false; refresh(); });
  range.addEventListener('pointerup', () => { setTimeout(()=>{if(editing){editing=false;refresh();}},0); });
  range.addEventListener('blur', () => { editing = false; refresh(); });
  pause.addEventListener('click', () => { core._th09_spectator_control(info()[5] ? 3 : 0, 0); refresh(); });
  live.addEventListener('click', () => { onLive(); refresh(); });
  refresh();
  return {refresh,
    resetMatch(frame) { editing=false;waiting.close();notifiedEnd=null;matchStart=frame;rounds.length=0;marks.replaceChildren();refresh(); },
    addRound(frame,round) {
      const button=document.createElement('button');button.type='button';button.className='spectator-round-mark';
      button.dataset.frame=String(frame);button.dataset.round=String(round);
      button.setAttribute('aria-label',`第 ${round + 1} 回合 · ${clock(frame-matchStart)}`);
      button.title=button.getAttribute('aria-label');
      button.addEventListener('click',()=>{const state=info();if(frame>state[1]&&frame<=state[3])core._th09_spectator_control(1,frame);refresh();});
      rounds.push({frame,round,button});marks.append(button);refresh();
    },
    close() { waiting.close();waiting.remove();panel.remove(); document.body.classList.remove('spectator-view'); }};
}
