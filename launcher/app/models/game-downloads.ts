import type {GameId} from '../../src/contracts/product-catalog.mts';
import type {GameVersion} from '../../src/contracts/package-read-models.mts';
import type {NetworkActivitySnapshot} from '../../src/launcher/network-activity.mts';
import type {PreparationProgress} from '../services/package-acquisition';

export interface GameDownload {game: GameId; version: GameVersion}
interface DownloadSnapshot {
  active: GameDownload | null;
  pending: readonly GameDownload[];
  failed: readonly GameDownload[];
  progress: PreparationProgress | null;
  network: NetworkActivitySnapshot | null;
}
interface DownloadRun {
  signal: AbortSignal;
  onProgress(value: PreparationProgress): void;
  onNetwork(value: NetworkActivitySnapshot): void;
}
const idle: DownloadSnapshot = {active: null, pending: [], failed: [], progress: null, network: null};

/** The launcher document owns the FIFO, independently of views and dialogs. */
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
      publish(idle);
      return;
    }
    const controller = new AbortController();
    current = controller;
    publish({active: job, pending, failed, progress: null, network: null});
    void run(job, controller);
  }
  async function run(job: GameDownload, controller: AbortController) {
    let failed = false;
    try {
      await download(job, {
        signal: controller.signal,
        onProgress(progress) {if (current === controller && !controller.signal.aborted) publish({...snapshot, progress});},
        onNetwork(network) {if (current === controller && !controller.signal.aborted) publish({...snapshot, network});},
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
