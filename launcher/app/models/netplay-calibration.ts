import type {Translate, UiMessageKey} from '../i18n';
import {parseMeasuredNetplayTiming} from '../../src/contracts/netplay-timing.mts';

export interface CalibrationSnapshot {
  epoch: number;
  connection: Readonly<Record<string, unknown>> | null;
  report: Readonly<Record<string, unknown>> | null;
  reportText: string | null;
}
export interface CalibrationOptions {
  epoch?: number;
  userAgent: string;
  now?: () => number;
  timers: Pick<Window, 'setTimeout' | 'clearTimeout'>;
}
/** Instance-scoped extraction of original netplay-calibration-{connection,report}.
 * The host feeds only its authenticated runtime-info subscription and epoch. */
export function createNetplayCalibration(options: CalibrationOptions) {
  const now = options.now ?? Date.now, listeners = new Set<() => void>();
  let epoch = options.epoch ?? 0, disposed = false, timer: number | null = null;
  let connection: Readonly<Record<string, unknown>> | null = null;
  let report: Readonly<Record<string, unknown>> | null = null, reportText: string | null = null;
  let snapshot: Readonly<CalibrationSnapshot> = Object.freeze({epoch, connection, report, reportText});
  function publish() {
    if (disposed) return;
    snapshot = Object.freeze({epoch, connection, report, reportText});
    for (const listener of listeners) listener();
  }
  function clearTimer() {if (timer !== null) options.timers.clearTimeout(timer); timer = null;}
  function dismiss() {clearTimer(); connection = null; publish();}
  function record(value: unknown, sourceEpoch: number) {
    if (disposed || sourceEpoch !== epoch || !value || typeof value !== 'object') return;
    const raw = value as Record<string, unknown>;
    if (raw.phase === 'closed') {dismiss(); return;}
    const timing = parseMeasuredNetplayTiming(raw);
    if (raw.phase === 'ready') {
      if (!timing || raw.route === 'spectator') return;
      clearTimer(); connection = Object.freeze({...raw});
      const ownedEpoch = epoch;
      timer = options.timers.setTimeout(() => {timer = null; if (!disposed && epoch === ownedEpoch) {connection = null; publish();}}, 8000);
    } else if (['waiting', 'stabilizing', 'measuring', 'negotiating', 'retrying', 'suspended', 'unavailable'].includes(String(raw.phase))) {
      clearTimer(); connection = Object.freeze({...raw});
    } else return;
    const calibration = raw.calibration && typeof raw.calibration === 'object' ? raw.calibration as Record<string, unknown> : null;
    const players = Array.isArray(calibration?.players) ? calibration.players as Record<string, unknown>[] : null;
    if (timing && timing.route !== 'spectator' && calibration && players && [2, 3].includes(players.length) &&
      players.every((p, index) => p && p.player === index && Number.isInteger(p.p95Us) && Number(p.p95Us) >= 1 && Number(p.p95Us) <= 1_000_000 &&
        Number.isInteger(p.samples) && Number(p.samples) >= 96 && Number(p.samples) <= 120 && p.lost === 120 - Number(p.samples))) {
      const game = ['th08mp', 'th09mp', 'th10mp'].includes(String(calibration.game)) ? String(calibration.game) : 'th09mp';
      report = Object.freeze({schema: `eagler-touhou/${game.slice(0, 4)}-calibration-report/1`, game, ...timing,
        completedAt: calibration.completedAt, runtimeBuild: calibration.build, localPlayer: calibration.localPlayer,
        players: players.map(p => ({player: p.player, p95Us: p.p95Us, samples: p.samples, lost: p.lost, minUs: p.minUs, maxUs: p.maxUs, meanUs: p.meanUs})),
        formula: 'B=max(1,ceil(floor(max(players.p95Us)/2)*60/1000000)); automatic hybrid P=min(configuredReserve,B-1), configuredReserve<=2; automatic D=B-P>=1',
        measurement: 'Startup input-channel RTT; arrival-triggered echo and high-resolution clock; frozen for this match',
        method: calibration.method, intervalMs: calibration.intervalMs, tailWaitMs: calibration.tailWaitMs, scope: calibration.scope,
        probes: calibration.probes, windowStart: calibration.windowStart, windowEnd: calibration.windowEnd, userAgent: options.userAgent});
    }
    publish();
  }
  return {
    getSnapshot: () => snapshot,
    subscribe(listener: () => void) {listeners.add(listener); return () => {listeners.delete(listener);};},
    record, dismiss,
    reset(nextEpoch: number) {if (disposed) return; clearTimer(); epoch = nextEpoch; connection = null; report = null; reportText = null; publish();},
    openReport(currentNetwork: string) {
      if (disposed || !report) return;
      reportText = JSON.stringify({...report, copiedAt: new Date(now()).toISOString(), liveNetworkDisplay: currentNetwork}, null, 2); publish();
    },
    closeReport() {if (reportText === null || disposed) return; reportText = null; publish();},
    dispose() {disposed = true; clearTimer(); listeners.clear();},
  };
}
export type NetplayCalibration = ReturnType<typeof createNetplayCalibration>;

/** Connection copy is rendered from the current locale, outside protocol state. */
export function calibrationConnectionCopy(progress: Readonly<Record<string, unknown>>, t: Translate) {
  const phase = String(progress.phase);
  const titles: Record<string, UiMessageKey> = {ready: 'calibration.ready', unavailable: 'calibration.unavailable', suspended: 'calibration.suspended', retrying: 'calibration.retrying', waiting: 'calibration.waiting', stabilizing: 'calibration.stabilizing', negotiating: 'calibration.negotiating'};
  const summary = phase === 'unavailable' ? t(Number(progress.reason) === 8 ? 'calibration.delayHigh' : 'calibration.failed')
    : phase === 'suspended' ? t('calibration.keepOpen')
    : phase === 'retrying' ? t('calibration.attempt', {attempt: progress.attempt ?? 1, maximum: progress.maxAttempts ?? 4})
    : phase === 'stabilizing' ? t('calibration.waitSecond')
    : phase === 'waiting' ? t('calibration.waitChannels')
    : phase === 'negotiating' ? t('calibration.waitConfirm')
    : t('calibration.probes', {probes: progress.probes ?? 0, replies: progress.replies ?? 0});
  return {title: t(titles[phase] ?? 'calibration.measuring'), summary,
    delay: t('calibration.delay', {frames: progress.inputDelay}),
    rollback: t('calibration.rollback', {state: t(Number(progress.adonisMode) === 2 ? 'calibration.on' : 'calibration.off')}),
    columns: [t('calibration.player'), t('calibration.latency'), t('calibration.maximum')],
    note: t('calibration.note'), dismiss: t('calibration.dismiss'), returnToRoom: t('netplay.return')};
}
