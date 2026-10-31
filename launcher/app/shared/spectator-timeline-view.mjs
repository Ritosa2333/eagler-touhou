/** Shared DOM renderer for receive-only playback timelines.
 *
 * The view owns presentation and browser interaction only. A title adapter owns
 * its native playback state, history, labels, and action mapping.
 * @typedef {'pause'|'play'|'seek'|'live'} SpectatorTimelineAction
 * @typedef {'match'|'round'|'stage'|'boss'} SpectatorTimelineMarkKind
 * @typedef {{id:string|number,frame:number,kind:SpectatorTimelineMarkKind,label?:string,active?:boolean,disabled?:boolean,current?:boolean}} SpectatorTimelineMark
 * @typedef {{key:string,min:number,position:number,edge:number,received:number,target?:number,paused:boolean,seeking:boolean,following:boolean,ended:boolean,marks:readonly SpectatorTimelineMark[],timeOrigin?:number,statusText?:string,lagText?:string,durationText?:string,summaryText?:string,liveDisabled?:boolean}} SpectatorTimelineModel
 * @typedef {{controls?:string,summary?:string,expand?:string,collapse?:string,progress?:string,nodes?:string,pause?:string,play?:string,live?:string,error?:string}} SpectatorTimelineLabels
 * @typedef {{toggle?:string,pause?:string,play?:string,live?:string}} SpectatorTimelineIcons
 * @typedef {{document?:Document,mount?:Element,idPrefix?:string,expanded?:boolean,onAction?:(action:SpectatorTimelineAction,frame?:number,key?:string)=>unknown,labels?:SpectatorTimelineLabels,icons?:SpectatorTimelineIcons}} SpectatorTimelineOptions
 * @typedef {{panel:HTMLElement,slots:{summary:HTMLElement,duration:HTMLElement},update:(model:SpectatorTimelineModel)=>void,updateLabels:(labels?:SpectatorTimelineLabels,icons?:SpectatorTimelineIcons)=>void,destroy:()=>void}} SpectatorTimelineView
 */

const DEFAULT_LABELS = Object.freeze({
  controls: '观战播放控制',
  expand: '展开观战控制',
  collapse: '收起观战控制',
  summary: '比分与对决时长',
  progress: '观战进度',
  nodes: '回合起点',
  pause: '暂停',
  play: '继续',
  live: '回到直播',
  error: '播放操作失败',
});
const STYLE_ATTRIBUTE = 'data-spectator-timeline-view-style';
const STYLE_TEXT = `
.spectator-timeline{position:fixed;left:50%;bottom:max(10px,env(safe-area-inset-bottom));transform:translateX(-50%);width:min(660px,calc(100% - 24px));padding:12px 16px;border-radius:12px;background:rgba(24,24,27,.88);border:1px solid rgba(255,255,255,.07);font:13px/1.4 var(--ui-font,system-ui);color:#e8e6e8;z-index:10}
.spectator-timeline[data-collapsed=true]{left:auto;right:max(8px,env(safe-area-inset-right));bottom:max(8px,env(safe-area-inset-bottom));transform:none;width:max-content;max-width:calc(100% - 16px);min-height:44px;padding:2px 4px 2px 10px;background:rgba(24,24,27,.44)}
.spectator-timeline [hidden]{display:none}
.spectator-timeline .spectator-bar,.spectator-timeline .spectator-detail-bar{display:flex;gap:12px;align-items:center;justify-content:space-between}
.spectator-timeline[data-collapsed=true] .spectator-bar{gap:6px}
.spectator-timeline .spectator-match-stats{flex:1;min-width:0;display:grid;gap:2px}
.spectator-timeline[data-collapsed=true] .spectator-match-stats{display:flex;align-items:center;gap:8px}
.spectator-timeline .spectator-score-slot{display:flex;align-items:baseline;gap:10px;min-width:0;line-height:1.3;font-weight:700}
.spectator-timeline .spectator-score-player{min-width:0;max-width:180px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;font-size:16px}
.spectator-timeline[data-collapsed=true] .spectator-score-player{display:none}
.spectator-timeline .spectator-score-value{flex:none;font-size:24px;font-variant-numeric:tabular-nums}
.spectator-timeline[data-collapsed=true] .spectator-score-value{font-size:18px}
.spectator-timeline .spectator-duration-slot{font-size:13px;color:#b7aabd}
.spectator-timeline[data-collapsed=true] .spectator-duration-slot{font-size:11px;white-space:nowrap}
.spectator-timeline .spectator-playback-meta{flex:1;min-width:0;display:grid;gap:1px;font-size:12px;color:#b7aabd}
.spectator-timeline .spectator-status-slot{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.spectator-timeline .spectator-actions{display:flex;align-items:center;gap:16px;flex:none}
.spectator-timeline .spectator-timeline-times{display:flex;justify-content:space-between;font-size:11px;font-variant-numeric:tabular-nums;color:#b7aabd;margin:0 2px 6px}
.spectator-timeline .spectator-mark-track{position:relative;height:34px;margin:6px 7px 0}
.spectator-timeline .spectator-round-mark{position:absolute;bottom:0;transform:translateX(-50%);min-height:22px;min-width:24px;padding:0 4px;background:transparent;border:0;border-radius:0;font:11px/1.2 var(--ui-font);color:#b7aabd}
.spectator-timeline .spectator-round-mark:after{content:'';position:absolute;bottom:-6px;left:50%;height:7px;border-left:1px solid currentColor;pointer-events:none}
.spectator-timeline .spectator-round-mark[data-next=true],.spectator-timeline .spectator-round-mark[data-current=true]{color:#e7d6ef}
.spectator-timeline .spectator-round-mark[data-kind=match]{bottom:10px;font-size:12px;font-weight:700;color:#e7d6ef}
.spectator-timeline .spectator-round-mark[data-kind=match]:after{height:17px;bottom:-16px;border-left-width:2px}
.spectator-timeline .spectator-round-mark[data-kind=round]{font-size:10px;color:#b7aabd}
.spectator-timeline .spectator-round-mark[data-kind=round]:after{border-left-style:dashed}
.spectator-timeline .spectator-round-mark[data-align=left]{transform:none}
.spectator-timeline .spectator-round-mark[data-align=right]{transform:translateX(-100%)}
.spectator-timeline input{appearance:none;display:block;width:100%;height:24px;padding:0;margin:2px 0 6px;border:0;background:transparent;cursor:pointer;letter-spacing:normal}
.spectator-timeline input::-webkit-slider-runnable-track{height:6px;border:1px solid #e7d6ef55;border-radius:5px;background:linear-gradient(to right,transparent var(--played),#e7d6ef var(--played))}
.spectator-timeline input::-moz-range-track{height:6px;border:1px solid #e7d6ef55;border-radius:5px;background:linear-gradient(to right,transparent var(--played),#e7d6ef var(--played))}
.spectator-timeline input::-webkit-slider-thumb{appearance:none;width:14px;height:14px;border-radius:50%;background:#e7d6ef;margin-top:-5px;border:0}
.spectator-timeline input::-moz-range-thumb{appearance:none;width:14px;height:14px;border-radius:50%;background:#e7d6ef;border:0}
.spectator-timeline button{display:inline-flex;align-items:center;justify-content:center;gap:7px;font:inherit;color:inherit;border:0;border-radius:8px;background:transparent;padding:6px 8px;margin:0;min-height:32px;cursor:pointer;touch-action:manipulation;white-space:nowrap}
.spectator-timeline .spectator-toggle{align-self:start;font-size:18px!important;width:32px;flex:none}
.spectator-timeline[data-collapsed=true] .spectator-toggle{align-self:center;width:36px;min-height:36px;padding:8px}
.spectator-timeline button:hover{background:rgba(255,255,255,.04)}
.spectator-timeline button:focus-visible,.spectator-timeline input:focus-visible{outline:2px solid #e7d6ef;outline-offset:1px}
.spectator-timeline button:disabled{opacity:.4;cursor:default}
.spectator-timeline .spectator-control-icon{width:18px;height:18px;filter:brightness(0) invert(.9);flex:none}
.spectator-timeline .spectator-toggle .spectator-control-icon{width:16px;height:16px;transform:rotate(90deg)}
.spectator-timeline[data-collapsed=true] .spectator-toggle .spectator-control-icon{transform:rotate(-90deg)}
.spectator-timeline .spectator-pause-icon{display:block}
.spectator-timeline .spectator-play-icon{display:none}
.spectator-timeline .spectator-pause-button[data-paused=true] .spectator-pause-icon{display:none}
.spectator-timeline .spectator-pause-button[data-paused=true] .spectator-play-icon{display:block}
.spectator-timeline .spectator-error{margin:6px 0 0;color:#ef777d;overflow-wrap:anywhere}
@media(max-width:560px){
 .spectator-timeline{padding:10px 12px}
 .spectator-timeline .spectator-score-slot{gap:6px}
 .spectator-timeline .spectator-score-player{font-size:14px;flex:0 1 auto}
 .spectator-timeline .spectator-score-value{font-size:20px}
 .spectator-timeline .spectator-duration-slot{font-size:12px}
 .spectator-timeline .spectator-actions{gap:6px}
 .spectator-timeline .spectator-playback-meta{font-size:11px}
}
`;

const styleReferences = new WeakMap();

function retainStyles(ownerDocument) {
  let entry = styleReferences.get(ownerDocument);
  if (!entry) {
    const style = ownerDocument.createElement('style');
    style.setAttribute(STYLE_ATTRIBUTE, '');
    style.textContent = STYLE_TEXT;
    (ownerDocument.head || ownerDocument.documentElement).append(style);
    entry = { style, count: 0 };
    styleReferences.set(ownerDocument, entry);
  }
  ++entry.count;
  let released = false;
  return () => {
    if (released) return;
    released = true;
    if (--entry.count === 0) {
      entry.style.remove();
      styleReferences.delete(ownerDocument);
    }
  };
}

function clock(frame) {
  const value = Math.max(0, Number.isFinite(frame) ? frame : 0);
  return `${Math.floor(value / 3600)}:${String(Math.floor(value / 60) % 60).padStart(2, '0')}`;
}

function finiteFrame(value, fallback = 0) {
  return Number.isSafeInteger(value) && value >= 0 ? value : fallback;
}

function setLabel(element, value) {
  if (typeof value === 'string') element.textContent = value;
}

function setIcon(image, value) {
  if (typeof value === 'string' && value) image.setAttribute('src', value);
  else image.removeAttribute('src');
}

/**
 * Create one shared, title-neutral playback timeline.
 *
 * The caller remains responsible for producing a validated model and handling
 * native state. `mount` defaults to the document body. The default prefix keeps
 * every historical TH09 element ID unchanged; a different prefix permits
 * multiple views in one document without changing their shared class names.
 *
 * @param {SpectatorTimelineOptions} options
 * @returns {SpectatorTimelineView}
 */
export function createSpectatorTimelineView(options = {}) {
  const ownerDocument = options.document || options.mount?.ownerDocument || globalThis.document;
  if (!ownerDocument?.createElement) throw new TypeError('A document is required for the spectator timeline view');
  const prefix = options.idPrefix ?? 'spectator';
  if (typeof prefix !== 'string' || !/^[A-Za-z][A-Za-z0-9_-]{0,47}$/.test(prefix))
    throw new TypeError('idPrefix must be a short HTML identifier prefix');
  const id = (suffix) => `${prefix}${suffix}`;
  const el = (tag, className, suffix, parent) => {
    const node = ownerDocument.createElement(tag);
    if (className) node.className = className;
    if (suffix) node.id = id(suffix);
    if (parent) parent.append(node);
    return node;
  };

  let labels = { ...DEFAULT_LABELS, ...(options.labels || {}) };
  let icons = { ...(options.icons || {}) };
  let model = null;
  let currentKey;
  let requestSerial = 0;
  let errorText = '';
  let editing = false;
  let dragKey;
  let dragBounds = null;
  let dragTimeOrigin = 0;
  let suppressChange = false;
  let destroyed = false;
  let latestMarks = new Map();
  const markElements = new Map();

  const releaseStyles = retainStyles(ownerDocument);
  const panel = el('section', 'spectator-timeline', 'Timeline');
  panel.setAttribute('role', 'region');
  panel.dataset.collapsed = 'true';
  const bar = el('div', 'spectator-bar', null, panel);
  const matchStats = el('div', 'spectator-match-stats', 'MatchStats', bar);
  matchStats.setAttribute('aria-label', labels.summary);
  const summary = el('strong', 'spectator-score-slot', 'Score', matchStats);
  const duration = el('span', 'spectator-duration-slot', 'MatchDuration', matchStats);
  const toggle = el('button', 'spectator-toggle', 'Toggle', bar);
  toggle.type = 'button';
  toggle.setAttribute('aria-expanded', 'false');
  toggle.setAttribute('aria-controls', id('Details'));
  const toggleIcon = el('img', 'spectator-control-icon', null, toggle);
  toggleIcon.dataset.icon = 'toggle';
  toggleIcon.alt = '';

  const details = el('div', 'spectator-details', 'Details', panel);
  details.hidden = true;
  const marks = el('div', 'spectator-mark-track', 'RoundMarks', details);
  const range = el('input', '', 'Seek', details);
  range.type = 'range';
  range.min = '0';
  range.max = '0';
  range.step = '1';
  range.value = '0';
  const times = el('div', 'spectator-timeline-times', null, details);
  const cursorTime = el('span', '', 'CurrentTime', times);
  const edgeTime = el('span', '', 'EdgeTime', times);
  const detailBar = el('div', 'spectator-detail-bar', null, details);
  const metadata = el('div', 'spectator-playback-meta', null, detailBar);
  const status = el('span', 'spectator-status-slot', 'PlaybackStatus', metadata);
  status.setAttribute('role', 'status');
  const lag = el('span', '', 'Lag', metadata);
  const actions = el('div', 'spectator-actions', null, detailBar);
  const pause = el('button', 'spectator-pause-button', 'Pause', actions);
  pause.type = 'button';
  const pauseIcon = el('img', 'spectator-control-icon spectator-pause-icon', null, pause);
  pauseIcon.dataset.icon = 'pause';
  pauseIcon.alt = '';
  const playIcon = el('img', 'spectator-control-icon spectator-play-icon', null, pause);
  playIcon.dataset.icon = 'play';
  playIcon.alt = '';
  const pauseLabel = el('span', '', 'PauseLabel', pause);
  const live = el('button', '', 'Live', actions);
  live.type = 'button';
  const liveIcon = el('img', 'spectator-control-icon', null, live);
  liveIcon.dataset.icon = 'live';
  liveIcon.alt = '';
  const liveLabel = el('span', '', 'LiveLabel', live);
  const error = el('p', 'spectator-error', 'Error', details);
  error.setAttribute('role', 'alert');
  error.hidden = true;
  (options.mount || ownerDocument.body).append(panel);

  function renderLabels() {
    panel.setAttribute('aria-label', labels.controls);
    matchStats.setAttribute('aria-label', labels.summary || '');
    toggle.setAttribute('aria-label', panel.dataset.collapsed === 'true' ? labels.expand : labels.collapse);
    range.setAttribute('aria-label', labels.progress);
    marks.setAttribute('aria-label', labels.nodes);
    pauseLabel.textContent = model?.paused ? labels.play : labels.pause;
    pause.setAttribute('aria-label', model?.paused ? labels.play : labels.pause);
    pause.title = model?.paused ? labels.play : labels.pause;
    liveLabel.textContent = labels.live;
    live.setAttribute('aria-label', labels.live);
    live.title = labels.live;
    range.setAttribute('aria-valuetext', cursorTime.textContent || '');
    error.setAttribute('aria-label', labels.error);
    if (errorText) error.textContent = labels.error ? `${labels.error}: ${errorText}` : errorText;
    else error.textContent = '';
    error.hidden = !errorText;
    // A hosted TH09 view lets the parent own its icon URLs after adoption.
    // An adapter that did not supply icons must leave those external URLs alone.
    if (Object.hasOwn(icons, 'toggle')) setIcon(toggleIcon, icons.toggle);
    if (Object.hasOwn(icons, 'pause')) setIcon(pauseIcon, icons.pause);
    if (Object.hasOwn(icons, 'play')) setIcon(playIcon, icons.play);
    if (Object.hasOwn(icons, 'live')) setIcon(liveIcon, icons.live);
  }

  function timeOrigin() {
    return finiteFrame(model?.timeOrigin, 0);
  }
  function rangeBounds(value = model) {
    if (!value) return { min: 0, max: 0, edge: 0 };
    const min = finiteFrame(value.min);
    const edge = Math.max(min, finiteFrame(value.edge, min));
    const max = Math.max(min, edge, finiteFrame(value.position, min));
    return { min, max, edge };
  }
  function renderError() {
    error.textContent = errorText ? (labels.error ? `${labels.error}: ${errorText}` : errorText) : '';
    error.hidden = !errorText;
  }
  function cancelActiveDragTail() {
    suppressChange = true;
  }
  function renderMarks(displayModel, bounds) {
    if (!displayModel) {
      marks.replaceChildren();
      markElements.clear();
      return;
    }
    const visible = displayModel.marks.filter(
      (mark) => Number.isSafeInteger(mark.frame) && mark.frame >= bounds.min && mark.frame <= bounds.max,
    );
    const next = visible.find((mark) => mark.frame > displayModel.position && mark.frame <= bounds.edge);
    const nearbyIds = new Set();
    const previousByStyle = new Map();
    const markerStyleKind = (value) => (value === 'stage' ? 'match' : value === 'boss' ? 'round' : value);
    const pixelsPerFrame = bounds.max > bounds.min ? (marks.clientWidth || 300) / (bounds.max - bounds.min) : 0;
    for (const mark of visible) {
      const markId = String(mark.id);
      const styleKind = markerStyleKind(mark.kind);
      const previous = previousByStyle.get(styleKind);
      if (previous && Math.abs(mark.frame - previous.frame) * pixelsPerFrame < 48) {
        nearbyIds.add(markId);
        nearbyIds.add(String(previous.id));
      }
      previousByStyle.set(styleKind, mark);
    }
    const desired = new Set(visible.map((mark) => String(mark.id)));
    for (const [markId, element] of markElements)
      if (!desired.has(markId)) {
        element.remove();
        markElements.delete(markId);
      }
    for (let index = 0; index < visible.length; ++index) {
      const mark = visible[index],
        markId = String(mark.id);
      let button = markElements.get(markId);
      if (!button) {
        button = ownerDocument.createElement('button');
        button.type = 'button';
        button.className = 'spectator-round-mark';
        button.addEventListener('click', () => {
          const latest = latestMarks.get(markId),
            current = model;
          if (!latest || !current || button.disabled) return;
          const range = rangeBounds(current);
          invoke('seek', Math.max(range.min, Math.min(range.edge, latest.frame)), current.key);
        });
        markElements.set(markId, button);
        marks.append(button);
      }
      const styleKind = mark.kind === 'stage' ? 'match' : mark.kind === 'boss' ? 'round' : mark.kind;
      const frame = Math.max(bounds.min, Math.min(bounds.max, mark.frame));
      const fraction = bounds.max > bounds.min ? (100 * (frame - bounds.min)) / (bounds.max - bounds.min) : 0;
      const isNext = mark.id === next?.id;
      const shortLabel = mark.label ?? (mark.kind === 'round' ? String(index + 1) : '');
      button.dataset.kind = styleKind;
      button.dataset.nodeKind = mark.kind;
      button.dataset.next = String(isNext);
      button.dataset.current = String(mark.current === true);
      button.dataset.active = String(mark.active === true);
      button.dataset.align = fraction < 15 ? 'left' : fraction > 85 ? 'right' : 'center';
      button.style.left = `${fraction}%`;
      button.hidden = false;
      button.disabled = mark.disabled === true || mark.frame > bounds.edge;
      button.textContent = nearbyIds.has(markId) && !isNext ? '' : shortLabel;
      const text = `${mark.label || shortLabel} · ${clock(mark.frame - timeOrigin())}`.trim();
      button.title = text;
      button.setAttribute('aria-label', text);
      if (mark.current) button.setAttribute('aria-current', 'location');
      else button.removeAttribute('aria-current');
    }
    marks.hidden = visible.length === 0;
    latestMarks = new Map(displayModel.marks.map((mark) => [String(mark.id), mark]));
  }

  function render() {
    if (!model || destroyed) return;
    const currentBounds = rangeBounds();
    const bounds = editing && dragBounds ? dragBounds : currentBounds;
    const positionValue = editing
      ? finiteFrame(Number(range.value), bounds.min)
      : finiteFrame(model.position, bounds.min);
    const displayPosition = Math.max(bounds.min, Math.min(bounds.max, positionValue));
    const displayTimeOrigin = editing ? dragTimeOrigin : timeOrigin();
    const elapsed = clock(displayPosition - displayTimeOrigin);
    const durationValue =
      typeof model.durationText === 'string' ? model.durationText : clock(bounds.edge - displayTimeOrigin);
    if (typeof model.summaryText === 'string') summary.textContent = model.summaryText;
    if (typeof model.durationText === 'string' || !duration.textContent) duration.textContent = durationValue;
    setLabel(status, model.statusText || '');
    setLabel(lag, model.lagText || '');
    cursorTime.textContent = elapsed;
    edgeTime.textContent = clock(bounds.edge - displayTimeOrigin);
    range.setAttribute('aria-valuetext', elapsed);
    pause.dataset.paused = String(model.paused === true);
    live.disabled = typeof model.liveDisabled === 'boolean' ? model.liveDisabled : bounds.edge <= displayPosition;
    if (!editing) {
      range.min = String(bounds.min);
      range.max = String(bounds.max);
      range.value = String(displayPosition);
      range.disabled = bounds.max <= bounds.min;
    }
    const fraction =
      bounds.max > bounds.min
        ? Math.max(0, Math.min(100, (100 * (displayPosition - bounds.min)) / (bounds.max - bounds.min)))
        : 0;
    range.style.setProperty('--played', `${fraction}%`);
    const displayModel = editing ? { ...model, position: displayPosition, min: bounds.min, edge: bounds.edge } : model;
    renderMarks(displayModel, bounds);
    renderLabels();
    renderError();
  }

  function invoke(action, frame, key = model?.key) {
    if (!model || typeof key !== 'string' || typeof options.onAction !== 'function') return;
    const request = ++requestSerial;
    errorText = '';
    renderError();
    let result;
    try {
      result = options.onAction(action, frame, key);
    } catch (reason) {
      result = Promise.reject(reason);
    }
    Promise.resolve(result).catch((reason) => {
      if (destroyed || request !== requestSerial || key !== currentKey) return;
      const message = reason instanceof Error ? reason.message : String(reason);
      errorText = message || labels.error || 'Playback action failed';
      renderError();
    });
  }

  function updateLabels(nextLabels = {}, nextIcons = {}) {
    if (destroyed) return;
    labels = { ...labels, ...nextLabels };
    icons = { ...icons, ...nextIcons };
    renderLabels();
    renderError();
  }

  function setExpanded(expanded) {
    panel.dataset.collapsed = String(!expanded);
    toggle.setAttribute('aria-expanded', String(expanded));
    details.hidden = !expanded;
    renderLabels();
  }
  let touchActivationAt = -Infinity;
  const isCompatibilityClick = (event) =>
    event.detail > 0 && event.pointerType !== 'mouse' && Date.now() - touchActivationAt < 700;
  toggle.addEventListener('click', (event) => {
    if (!isCompatibilityClick(event)) setExpanded(panel.dataset.collapsed === 'true');
  });
  bar.addEventListener('pointerup', (event) => {
    if (event.pointerType !== 'touch' && event.pointerType !== 'pen') return;
    if (panel.dataset.collapsed !== 'true' && !toggle.contains(event.target)) return;
    event.preventDefault();
    touchActivationAt = Date.now();
    setExpanded(panel.dataset.collapsed === 'true');
  });
  bar.addEventListener('click', (event) => {
    if (toggle.contains(event.target) || panel.dataset.collapsed !== 'true' || isCompatibilityClick(event)) return;
    setExpanded(true);
  });

  range.addEventListener('pointerdown', (event) => {
    if (!model) return;
    editing = true;
    dragKey = model.key;
    dragBounds = { min: Number(range.min), max: Number(range.max), edge: rangeBounds().edge };
    dragTimeOrigin = timeOrigin();
    suppressChange = false;
    range.setPointerCapture?.(event.pointerId);
  });
  range.addEventListener('input', () => {
    if (suppressChange) {
      render();
      return;
    }
    const bounds = dragBounds || rangeBounds();
    range.value = String(Math.max(bounds.min, Math.min(bounds.max, finiteFrame(Number(range.value), bounds.min))));
    const value = clock(Number(range.value) - dragTimeOrigin);
    cursorTime.textContent = value;
    range.setAttribute('aria-valuetext', value);
  });
  range.addEventListener('change', () => {
    if (suppressChange) {
      render();
      return;
    }
    if (!model || (dragKey !== undefined && dragKey !== model.key)) {
      editing = false;
      dragKey = undefined;
      dragBounds = null;
      render();
      return;
    }
    const bounds = rangeBounds();
    const target = Math.max(bounds.min, Math.min(bounds.edge, finiteFrame(Number(range.value), bounds.min)));
    editing = false;
    dragKey = undefined;
    dragBounds = null;
    invoke('seek', target, model.key);
    render();
  });
  range.addEventListener('pointerup', () => {
    setTimeout(() => {
      if (editing) {
        editing = false;
        dragKey = undefined;
        dragBounds = null;
        render();
      }
    }, 0);
  });
  range.addEventListener('pointercancel', () => {
    editing = false;
    dragKey = undefined;
    dragBounds = null;
    cancelActiveDragTail();
    render();
  });
  range.addEventListener('keydown', () => {
    suppressChange = false;
  });
  range.addEventListener('blur', () => {
    editing = false;
    dragKey = undefined;
    dragBounds = null;
    render();
  });
  pause.addEventListener('click', () => {
    if (model) invoke(model.paused ? 'play' : 'pause', undefined, model.key);
  });
  live.addEventListener('click', () => {
    if (model && !live.disabled) invoke('live', undefined, model.key);
  });

  const view = {
    panel,
    slots: { summary, duration },
    update(nextModel) {
      if (destroyed) return;
      if (!nextModel || typeof nextModel.key !== 'string')
        throw new TypeError('A spectator timeline model needs a string key');
      if (currentKey !== nextModel.key) {
        const hadActiveDrag = editing;
        currentKey = nextModel.key;
        ++requestSerial;
        errorText = '';
        editing = false;
        dragKey = undefined;
        dragBounds = null;
        if (hadActiveDrag) cancelActiveDragTail();
        else suppressChange = false;
      }
      model = nextModel;
      render();
    },
    updateLabels,
    destroy() {
      if (destroyed) return;
      destroyed = true;
      ++requestSerial;
      model = null;
      latestMarks.clear();
      markElements.clear();
      panel.remove();
      releaseStyles();
    },
  };
  setExpanded(options.expanded === true);
  return view;
}
