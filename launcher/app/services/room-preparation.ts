import { gameIdForProduct } from '../../src/contracts/product-catalog.mts';
import type { RuntimeDownloadProgress } from '../../src/contracts/runtime-generations.mts';
import { createNetworkActivityTracker, type NetworkActivitySnapshot } from '../../src/launcher/network-activity.mts';
import { prepareRuntimeLaunch } from '../../src/launcher/runtime-launch.mts';
import type { Translate } from '../i18n';
import { GameDataAcquisitionError } from './game-data-acquisition';
import type { RoomOperationContext, RoomPreparationProgress } from './multiplayer-room';
import { type PackageAcquisition, preparationErrorText } from './package-acquisition';
import { packageNetworkMetadata } from './preparation-network';

type RuntimeWorker = Exclude<
  NonNullable<Parameters<typeof prepareRuntimeLaunch>[1]>['worker'],
  PromiseLike<unknown> | null | undefined
>;
export interface RoomPreparationOptions {
  acquisition: PackageAcquisition;
  metadataReady(): Promise<unknown>;
  baseUrl: string;
  translate: Translate;
  /** Prefer this deployment's worker. The shared helper can also establish the
   * runtime-only authority or verify immutable HTTP when caching is unavailable. */
  activeWorker(): Promise<RuntimeWorker | null>;
  network?: ReturnType<typeof createNetworkActivityTracker>;
  dependencies?: {
    createTracker?: typeof createNetworkActivityTracker;
    prepareRuntime?: typeof prepareRuntimeLaunch;
  };
}

/** Package-only install followed by current Multiplayer program preparation.
 * This adapter never starts a game, changes options, selects optional
 * music/language, or creates another Package Store. Room owner holds the task. */
export function createRoomPreparation(options: RoomPreparationOptions) {
  const { acquisition, translate: t } = options;
  const createTracker = options.dependencies?.createTracker ?? createNetworkActivityTracker;
  const prepareRuntime = options.dependencies?.prepareRuntime ?? prepareRuntimeLaunch;
  async function prepare(
    context: RoomOperationContext,
    report: (value: RoomPreparationProgress) => void,
  ): Promise<{ runtimeGeneration: string } | void> {
    const game = gameIdForProduct(context.product);
    let stage: 'package' | 'runtime' = 'package';
    let finished = false;
    const reportNetwork = (snapshot: NetworkActivitySnapshot) => {
      if (finished || !context.isCurrent()) return;
      // Original UI and wire use the last active request, not the aggregate.
      const active = snapshot.active.at(-1),
        loaded = active?.loaded || 0,
        total = active?.total || 0;
      report({
        status: 'preparing',
        stage,
        percent: total > 0 ? Math.min(100, (loaded / total) * 100) : null,
        error: '',
        loaded,
        total,
        title: active?.title || t(stage === 'package' ? 'room.preparingResources' : 'room.preparingRuntime'),
      });
    };
    const tracker = options.network ?? createTracker({ fetchImpl: acquisition.fetchImpl, onChange: reportNetwork });
    const unsubscribe = options.network?.subscribe(reportNetwork);
    const programTransfers = new Map<string, string>();
    const finish = () => {
      finished = true;
      unsubscribe?.();
      for (const id of programTransfers.values()) tracker.finish(id);
      programTransfers.clear();
    };
    const programProgress = (progress: RuntimeDownloadProgress) => {
      if (finished || !context.isCurrent()) return;
      const current = programTransfers.get(progress.path);
      if (progress.phase === 'complete' || progress.phase === 'failed') {
        if (current) {
          tracker.finish(current);
          programTransfers.delete(progress.path);
        }
        return;
      }
      const url = new URL(progress.path, options.baseUrl);
      if (current) tracker.update(current, progress);
      else
        programTransfers.set(
          progress.path,
          tracker.begin({
            ...packageNetworkMetadata(game, url, options.baseUrl, t),
            url: url.href,
            phase: progress.phase,
            loaded: progress.loaded,
            total: progress.total,
          }),
        );
    };
    context.signal.addEventListener('abort', finish, { once: true });
    try {
      const gameVersion = acquisition.getGameVersion(game);
      let installed = await acquisition.readCurrent(game, { gameVersion });
      if (!context.isCurrent()) return;
      if (!installed.generation) {
        if (!acquisition.getMetadata().releaseCatalog) {
          try {
            await options.metadataReady();
          } catch {}
        }
        if (!context.isCurrent()) return;
        const { catalog, catalogUrl } = await acquisition.getPublication(gameVersion, context.signal);
        if (!catalog?.games[game]) throw new Error(t('package.releaseNotReadyNoLocal', { game: game.toUpperCase() }));
        try {
          await acquisition.installPublished(game, {
            catalog,
            catalogUrl,
            gameVersion,
            addComponents: [],
            fetchImpl: (input, init) =>
              tracker.resourceFetch(input, init, packageNetworkMetadata(game, input, options.baseUrl, t)),
            signal: context.signal,
          });
        } catch (error) {
          if (
            context.signal.aborted ||
            (error !== null && typeof error === 'object' && 'name' in error && error.name === 'AbortError') ||
            /已取消下载/.test(preparationErrorText(error))
          )
            throw error;
          throw new GameDataAcquisitionError(preparationErrorText(error), { cause: error });
        }
        installed = await acquisition.readCurrent(game, { gameVersion });
        if (!installed.generation) throw new Error(t('package.objectNotPersisted'));
      }
      if (!context.isCurrent()) return;
      // Publication is a hint read; canonical committed generation stays authority.
      void acquisition.refreshInstalled(game).catch(() => {});
      stage = 'runtime';
      report({ status: 'preparing', stage, percent: null, error: '', title: t('room.preparingRuntime') });
      if (!acquisition.getMetadata().siteManifest) {
        try {
          await options.metadataReady();
        } catch {}
      }
      if (!context.isCurrent()) return;
      const host = acquisition.getMetadata().siteManifest;
      if (!host) throw new Error(t('multiplayer.runtimeCheckFailed'));
      const runtime = host?.games[game]?.multiplayerRuntime;
      if (!runtime) throw new Error(t('multiplayer.runtimeCheckFailed'));
      if (typeof runtime === 'string' && runtime && host?.shared.runtimeManifest) {
        let worker: RuntimeWorker | null = null;
        try {
          worker = await options.activeWorker();
        } catch {}
        if (!context.isCurrent()) return;
        const prepared = await prepareRuntime(runtime, {
          worker: worker ?? undefined,
          baseUrl: options.baseUrl,
          fetchImpl: acquisition.fetchImpl,
          requireCurrent: true,
          onProgress: programProgress,
        });
        if (context.isCurrent()) return { runtimeGeneration: prepared.generation };
      }
      // The owning room service publishes ready only after this real task resolves
      // and its epoch/abort check still passes.
    } finally {
      context.signal.removeEventListener('abort', finish);
      finish();
    }
  }
  return Object.freeze({ prepare });
}
