import {createNetworkActivityTracker} from '../../src/launcher/network-activity.mts';
import type {DownloadRun, GameDownload} from './game-downloads';

const TOTAL = 100 * 1024 ** 2;
const RATE = 5 * 1024 ** 2;
const PREPARE_MS = 600;
const TICK_MS = 200;

export function isLocalInstallPreview(url: URL, development: boolean) {
  return development && ['127.0.0.1', 'localhost', '[::1]'].includes(url.hostname) &&
    url.searchParams.get('preview') === 'install';
}

/** Drive the normal queue with counters only: no fetch, files or package-store writes. */
export function createGameInstallPreview(owner: Pick<Window, 'performance' | 'setInterval' | 'clearInterval'>) {
  const completed = new Set<string>();
  const key = ({game, version}: GameDownload) => `${game}:${version}`;
  const now = () => owner.performance.now();

  async function download(job: GameDownload, {signal, onTotals, onNetwork}: DownloadRun) {
    signal.throwIfAborted();
    completed.delete(key(job));
    onTotals({completed: 0, total: 1, completedBytes: 0, totalBytes: TOTAL});
    const tracker = createNetworkActivityTracker({clock: now, onChange: onNetwork});
    const prepareUntil = now() + PREPARE_MS;
    let started: {at: number; id: string} | undefined;
    await new Promise<void>((resolve, reject) => {
      const stop = () => {
        owner.clearInterval(timer);
        signal.removeEventListener('abort', abort);
        if (started) tracker.finish(started.id);
      };
      const abort = () => {stop(); reject(signal.reason);};
      const timer = owner.setInterval(() => {
        const at = now();
        if (at < prepareUntil) return;
        started ??= {at, id: tracker.begin({kind: 'game', phase: 'receiving', total: TOTAL})};
        const loaded = Math.min(TOTAL, Math.floor(RATE * (at - started.at) / 1000));
        tracker.update(started.id, {loaded});
        if (loaded === TOTAL) {stop(); resolve();}
      }, TICK_MS);
      signal.addEventListener('abort', abort, {once: true});
    });
    signal.throwIfAborted();
    onTotals({completed: 1, total: 1, completedBytes: TOTAL, totalBytes: TOTAL});
    completed.add(key(job));
  }
  return {download, installed: (job: GameDownload) => completed.has(key(job))};
}
