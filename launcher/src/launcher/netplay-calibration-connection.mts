import { parseMeasuredNetplayTiming } from '../contracts/netplay-timing.mjs';
import { isUiLocale } from './i18n.mts';
import { calibrationConnectionCopy } from './netplay-calibration-copy.mts';

let progress: Record<string, unknown> | null = null;
let resultUntil = 0;
export function resetCalibrationProgress() {
  progress = null;
  resultUntil = 0;
}
export function recordCalibrationProgress(value: unknown) {
  if (!value || typeof value !== 'object') return;
  const v = value as Record<string, unknown>;
  if (v.phase === 'closed') {
    resetCalibrationProgress();
    return;
  }
  if (v.phase === 'ready') {
    if (!parseMeasuredNetplayTiming(v) || v.route === 'spectator') return;
    progress = v;
    resultUntil = Date.now() + 8000;
    return;
  }
  if (
    !['waiting', 'stabilizing', 'measuring', 'negotiating', 'retrying', 'suspended', 'unavailable'].includes(
      String(v.phase),
    )
  )
    return;
  progress = v;
  resultUntil = 0;
}
export function renderCalibrationConnection(windowElement: HTMLElement, onReturn?: () => void): boolean {
  windowElement.removeAttribute('data-calibration');
  windowElement.querySelector('#netplayCalibrationDismiss')?.remove();
  const note = windowElement.querySelector<HTMLElement>('#netplayConnectionNote');
  if (note) note.hidden = true;
  if (!progress || (progress.phase === 'ready' && Date.now() >= resultUntil)) {
    windowElement.querySelector('#netplayConnectionReturn')?.remove();
    return false;
  }
  const language = document.documentElement.dataset.uiLocale;
  const copy = calibrationConnectionCopy(progress, isUiLocale(language) ? language : 'zh-CN');
  const ready = progress.phase === 'ready';
  windowElement.hidden = false;
  windowElement.classList.remove('reconnecting');
  windowElement.dataset.calibration = String(progress.phase);
  const title = windowElement.querySelector<HTMLElement>('#netplayConnectionTitle')!;
  title.setAttribute('role', 'status');
  title.setAttribute('aria-live', 'polite');
  const nextTitle = copy.title;
  if (title.textContent !== nextTitle) title.textContent = nextTitle;
  const summary = windowElement.querySelector<HTMLElement>('#netplayConnectionSummary')!;
  summary.hidden = false;
  summary.replaceChildren();
  if (ready) {
    const delay = document.createElement('span');
    delay.textContent = copy.delay;
    const rollback = document.createElement('span');
    rollback.textContent = copy.rollback;
    summary.append(delay, rollback);
  } else summary.textContent = copy.summary;
  const rows = windowElement.querySelector<HTMLElement>('#netplayConnectionPeers')!;
  rows.replaceChildren();
  if (!ready && progress.phase !== 'unavailable') {
    const meter = document.createElement('progress');
    meter.id = 'netplayCalibrationProgress';
    meter.max = 129;
    if (progress.phase === 'measuring') meter.value = Number(progress.probes ?? 0);
    meter.setAttribute('aria-label', title.textContent);
    rows.append(meter);
  } else if (ready) {
    const table = document.createElement('table');
    table.className = 'netplay-calibration-table';
    const head = table.createTHead().insertRow();
    for (const text of copy.columns) {
      const th = document.createElement('th');
      th.scope = 'col';
      th.textContent = text;
      head.append(th);
    }
    const body = table.createTBody();
    const calibration = progress.calibration as Record<string, unknown> | undefined;
    const players = calibration?.players as Array<Record<string, unknown>> | undefined;
    for (const p of players ?? []) {
      if (!Number(p.samples)) continue;
      const row = body.insertRow();
      const label = document.createElement('th');
      label.scope = 'row';
      label.textContent = `P${Number(p.player) + 1}`;
      row.append(label);
      // The same frozen statistic that determines D, never the mean.
      for (const key of ['p95Us', 'maxUs']) row.insertCell().textContent = `${(Number(p[key]) / 1000).toFixed(2)} ms`;
    }
    rows.append(table);
    if (players?.length === 3 && note) {
      note.hidden = false;
      note.textContent = copy.note;
    }
    const dismiss = document.createElement('button');
    dismiss.id = 'netplayCalibrationDismiss';
    dismiss.type = 'button';
    dismiss.textContent = copy.dismiss;
    dismiss.onclick = () => {
      resultUntil = 0;
      windowElement.hidden = true;
    };
    windowElement.append(dismiss);
  }
  if (!ready && onReturn) {
    let button = windowElement.querySelector<HTMLButtonElement>('#netplayConnectionReturn');
    if (!button) {
      button = document.createElement('button');
      button.id = 'netplayConnectionReturn';
      button.type = 'button';
      windowElement.append(button);
    }
    button.textContent = copy.returnToRoom;
    button.onclick = onReturn;
  } else windowElement.querySelector('#netplayConnectionReturn')?.remove();
  return true;
}
