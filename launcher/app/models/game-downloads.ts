import type {GameId} from '../../src/contracts/product-catalog.mts';
import type {GameVersion} from '../../src/contracts/package-read-models.mts';
import type {NetworkActivitySnapshot} from '../../src/launcher/network-activity.mts';
import type {PackageInstallTotals} from '../../package/package-installer.mjs';

export interface GameDownload {game: GameId; version: GameVersion}
interface DownloadSnapshot {
  active: GameDownload | null;
  pending: readonly GameDownload[];
  failed: readonly GameDownload[];
  totals: PackageInstallTotals | null;
  task: NetworkActivitySnapshot['active'][number] | null;
  received: number;
}
export interface DownloadRun {
  signal: AbortSignal;
  onTotals(value: PackageInstallTotals): void;
  onNetwork(value: NetworkActivitySnapshot): void;
}
const idle: DownloadSnapshot = {active: null, pending: [], failed: [], totals: null, task: null, received: 0};

/** Percent tracks all declared package bytes; readiness still belongs to the commit. */
export function gameDownloadProgress({task, totals, received}: DownloadSnapshot) {
  const total = totals?.totalBytes ?? null;
  const loaded = total === null ? received : Math.min(total, (totals?.completedBytes ?? 0) + received);
  const elapsed = task ? task.updatedAt - task.startedAt : 0;
  const speed = task?.phase === 'receiving' && elapsed > 0 && task.loaded > 0 ? task.loaded * 1000 / elapsed : null;
  const phase = task ? task.phase === 'receiving' ? 'receiving' : 'waiting'
    : received > 0 || totals && totals.completed === totals.total ? 'saving' : 'preparing';
  return {
    loaded,
    total,
    // Do not call it complete before the package has been saved.
    percent: total && loaded > 0 ? Math.min(99, Math.floor(loaded / total * 100)) : null,
    speed,
    eta: total !== null && speed !== null && loaded < total ? (total - loaded) / speed : null,
    phase,
  } as const;
}

/** The launcher document owns the FIFO independently of its pages. */
export function createGameDownloads({download, onFailure}: {
  download(job: GameDownload, options: DownloadRun): Promise<void>;
  onFailure(job: GameDownload): void;
}) {
  let snapshot = idle, current: AbortController | null = null, disposed = false;
  const listeners = new Set<() => void>();
  function publish(value: DownloadSnapshot) {
    snapshot = value;
    for (const listener of listeners) listener();
  }
  function advance([job, ...pending]: readonly GameDownload[], failed: readonly GameDownload[]) {
    if (!job) {
      current = null;
      publish({...idle, failed});
      return;
    }
    const controller = new AbortController();
    current = controller;
    publish({...idle, active: job, pending, failed});
    void run(job, controller);
  }
  async function run(job: GameDownload, controller: AbortController) {
    let failed = false;
    try {
      await download(job, {
        signal: controller.signal,
        onTotals(totals) {
          if (current === controller && !controller.signal.aborted) publish({...snapshot, totals, received: 0});
        },
        onNetwork(network) {
          if (current !== controller || controller.signal.aborted) return;
          const task = network.active.find(item => item.kind !== 'descriptor') ?? null;
          publish({...snapshot, task, received: task?.loaded ?? snapshot.received});
        },
      });
    } catch {
      failed = !controller.signal.aborted;
    } finally {
      if (!disposed) {
        advance(snapshot.pending, failed ? [...snapshot.failed, job] : snapshot.failed);
        if (failed) onFailure(job);
      }
    }
  }
  function enqueue(job: GameDownload) {
    if (disposed || snapshot.active?.game === job.game || snapshot.pending.some(item => item.game === job.game)) return;
    const failed = snapshot.failed.filter(item => item.game !== job.game);
    if (current) publish({...snapshot, failed, pending: [...snapshot.pending, job]});
    else advance([job], failed);
  }
  return {
    getSnapshot: () => snapshot,
    subscribe(listener: () => void) {listeners.add(listener); return () => {listeners.delete(listener);};},
    enqueue,
    cancel(game: GameId) {
      if (snapshot.active?.game === game) current?.abort();
      else publish({...snapshot, pending: snapshot.pending.filter(item => item.game !== game)});
    },
    retry(game: GameId) {
      const failed = snapshot.failed.find(item => item.game === game);
      if (failed) enqueue(failed);
    },
    dispose() {disposed = true; current?.abort(); current = null; listeners.clear(); snapshot = idle;},
  };
}
export type GameDownloads = ReturnType<typeof createGameDownloads>;
