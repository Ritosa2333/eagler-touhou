import { descriptorGameVersion, loadGameVersion } from '../../package/game-version.mjs';
import { componentFileIds } from '../../package/package-generation.mjs';
import { readCurrentPackageGeneration } from '../../package/package-store.mjs';
import type { InstalledPackageGeneration } from '../../src/contracts/package-read-models.mts';
import {
  createLocalProductManifest,
  type GameId,
  gameIdForProduct,
  isMultiplayerProductId,
  languagePriority,
  PRODUCT_GAMES,
  PRODUCT_IDS,
  type ProductId,
  productEnabledForBuild,
} from '../../src/contracts/product-catalog.mts';
import type { ReleaseCatalog } from '../../src/contracts/release-catalog.mts';
import type { SiteManifest } from '../../src/contracts/site-manifest.mts';
import { buildLanguageCatalog } from '../../src/launcher/language-catalog.mts';
import { loadOfflineLanguageIndex } from '../../src/launcher/offline-language-index.mts';
import { loadRemoteMetadata } from '../../src/launcher/remote-metadata.mts';
import type { SettingsContext } from '../models/game-settings';

export interface MetadataSnapshot {
  phase: 'idle' | 'loading' | 'ready';
  siteManifest: SiteManifest | null;
  releaseCatalog: ReleaseCatalog | null;
  installed: ReadonlyMap<GameId, InstalledPackageGeneration>;
  beautyInstalled: ReadonlySet<GameId>;
  products: readonly ProductId[];
  metadataErrors: readonly unknown[];
  siteManifestError: unknown | null;
  releaseCatalogError: unknown | null;
}
export interface MetadataServiceOptions {
  /** Deployment document/root URL, not a selected client route. */
  baseUrl: string;
  fetchImpl?: typeof fetch;
  storage: Storage | null;
  getGameVersion?: (game: GameId) => 'original' | 'beauty';
  indexedDBFactory?: IDBFactory;
  translate: (locale: string, key: string, params?: Record<string, string | number>) => string;
  webAudioAvailable: boolean;
  webMidiAvailable: boolean;
  mobile: boolean;
  testBuild?: boolean;
  locale?: () => string;
}

/** Read-only metadata owner. Installation, migration and garbage collection remain
 * with the package owner. Construction performs no network or storage work. */
export function createMetadataService(options: MetadataServiceOptions) {
  const local = createLocalProductManifest();
  const base = new URL(options.baseUrl);
  const testBuild = options.testBuild ?? base.searchParams.has('test');
  const listeners = new Set<() => void>();
  const controllers = new Set<AbortController>();
  const reads = new Map<GameId, number>();
  let disposed = false;
  let initialization: Promise<void> | null = null;
  let snapshot: MetadataSnapshot = Object.freeze({
    phase: 'idle',
    siteManifest: null,
    releaseCatalog: null,
    installed: new Map(),
    beautyInstalled: new Set<GameId>(),
    products: availableProducts(null),
    metadataErrors: [],
    siteManifestError: null,
    releaseCatalogError: null,
  });

  function availableProducts(host: SiteManifest | null): readonly ProductId[] {
    return Object.freeze(
      PRODUCT_IDS.filter((product) => {
        if (!productEnabledForBuild(product, testBuild || host?.shared.testBuild === true)) return false;
        if (!host) return true;
        const game = host.games[gameIdForProduct(product)];
        if (!game) return false;
        return (
          !isMultiplayerProductId(product) ||
          (typeof game.multiplayerRuntime === 'string' && game.multiplayerRuntime.length > 0)
        );
      }),
    );
  }
  function publish(patch: Partial<MetadataSnapshot>) {
    if (disposed) return;
    snapshot = Object.freeze({ ...snapshot, ...patch });
    for (const listener of listeners) listener();
  }
  async function refreshInstalled(game?: GameId): Promise<void> {
    if (disposed) return;
    const games = game ? [game] : (Object.keys(PRODUCT_GAMES) as GameId[]);
    await Promise.all(
      games.map(async (id) => {
        const version = (reads.get(id) ?? 0) + 1;
        reads.set(id, version);
        const gameVersion = options.getGameVersion?.(id) ?? loadGameVersion(options.storage, id);
        try {
          const result = await readCurrentPackageGeneration(id, {
            indexedDBFactory: options.indexedDBFactory,
            gameVersion,
          });
          if (
            disposed ||
            reads.get(id) !== version ||
            gameVersion !== (options.getGameVersion?.(id) ?? loadGameVersion(options.storage, id))
          )
            return;
          const installed = new Map(snapshot.installed);
          if (result.generation) installed.set(id, result.generation);
          else installed.delete(id);
          publish({ installed });
          // Optional availability must not hold up selected-package hydration.
          const beauty =
            gameVersion === 'beauty'
              ? Promise.resolve(result)
              : readCurrentPackageGeneration(id, { indexedDBFactory: options.indexedDBFactory, gameVersion: 'beauty' });
          void beauty
            .then((value) => {
              if (disposed || reads.get(id) !== version) return;
              if (snapshot.beautyInstalled.has(id) === !!value.generation) return;
              const beautyInstalled = new Set(snapshot.beautyInstalled);
              if (value.generation) beautyInstalled.add(id);
              else beautyInstalled.delete(id);
              publish({ beautyInstalled });
            })
            .catch(() => {});
        } catch {
          // A failed hint read must not erase a usable previous snapshot. Launch
          // still reads the authoritative store again before using package data.
        }
      }),
    );
  }
  async function fetchJson(file: string): Promise<unknown> {
    const controller = new AbortController();
    controllers.add(controller);
    const timer = setTimeout(() => controller.abort(), 12000);
    try {
      const response = await (options.fetchImpl ?? fetch)(new URL(file, base), {
        cache: 'no-store',
        signal: controller.signal,
      });
      if (!response.ok) throw new Error(`${file}: HTTP ${response.status}`);
      return await response.json();
    } catch (error) {
      if (controller.signal.aborted && !disposed) {
        throw new Error(
          options.translate(options.locale?.() ?? 'zh', 'runtime.requestTimeoutDetail', { path: file, seconds: 12 }),
        );
      }
      throw error;
    } finally {
      clearTimeout(timer);
      controllers.delete(controller);
    }
  }
  function initialize(): Promise<void> {
    if (disposed) return Promise.resolve();
    if (initialization) return initialization;
    publish({ phase: 'loading', metadataErrors: [] });
    // Local package availability never waits for either network request, and a
    // slow/unavailable IndexedDB cannot hold remote metadata initialization.
    void refreshInstalled();
    initialization = loadRemoteMetadata(fetchJson)
      .then((result) => {
        if (disposed) return;
        const siteManifest = result.siteManifest.ok ? result.siteManifest.value : snapshot.siteManifest;
        const releaseCatalog = result.releaseCatalog.ok ? result.releaseCatalog.value : snapshot.releaseCatalog;
        const metadataErrors: unknown[] = [];
        if (!result.siteManifest.ok) metadataErrors.push(result.siteManifest.error);
        if (!result.releaseCatalog.ok) metadataErrors.push(result.releaseCatalog.error);
        publish({
          phase: 'ready',
          siteManifest,
          releaseCatalog,
          products: availableProducts(siteManifest),
          metadataErrors,
          siteManifestError: result.siteManifest.ok ? null : result.siteManifest.error,
          releaseCatalogError: result.releaseCatalog.ok ? null : result.releaseCatalog.error,
        });
      })
      .finally(() => {
        initialization = null;
      });
    return initialization;
  }
  function settingsContext(productId: ProductId, locale: string): SettingsContext {
    const gameId = gameIdForProduct(productId);
    const hostGame = snapshot.siteManifest?.games[gameId];
    const game = hostGame ?? local.games[gameId];
    const generation = snapshot.installed.get(gameId) ?? null;
    const music = hostGame?.music;
    return {
      productId,
      uiLocale: locale,
      hostFeatures: hostGame?.features,
      beautyInstalled: snapshot.beautyInstalled.has(gameId),
      languages: buildLanguageCatalog({
        languageOptions: game?.languageOptions,
        legacyLanguages: hostGame?.languages,
        offlineEntries: loadOfflineLanguageIndex(options.storage, gameId),
        generation,
        translate: (key) => options.translate(locale, key),
        priority: languagePriority,
      }),
      musicAvailability: {
        audio: options.webAudioAvailable,
        midiAvailable: PRODUCT_GAMES[gameId].musicCapabilities.midi && music?.midi?.supported !== false,
        importServer: snapshot.siteManifest?.shared.resourceMode === 'import',
        publishedOggCapable: Boolean(music?.ogg || music?.wav),
        remoteOggAdvertised: Boolean(music?.ogg),
        remoteRevision: snapshot.releaseCatalog?.games?.[gameId]?.revision ?? null,
        installed: generation
          ? {
              revision: generation.descriptor.revision,
              oggFileIds: componentFileIds(generation.descriptor, 'ogg'),
              files: generation.files,
            }
          : null,
      },
      webMidiAvailable: options.webMidiAvailable,
      mobile: options.mobile,
    };
  }
  return {
    subscribe(listener: () => void) {
      listeners.add(listener);
      return () => {
        listeners.delete(listener);
      };
    },
    getSnapshot: () => snapshot,
    initialize,
    refreshInstalled,
    settingsContext,
    /** Display hint from the canonical deferred migration/read owner. Launch
     * still rereads Package Store and never treats this map as DATA authority. */
    acceptInstalledHint(game: GameId, generation: InstalledPackageGeneration) {
      if (
        disposed ||
        descriptorGameVersion(generation.descriptor) !==
          (options.getGameVersion?.(game) ?? loadGameVersion(options.storage, game))
      )
        return;
      if (generation.game !== game) throw new Error('Package hint game mismatch');
      const installed = new Map(snapshot.installed);
      installed.set(game, generation);
      const beautyInstalled = new Set(snapshot.beautyInstalled);
      if (descriptorGameVersion(generation.descriptor) === 'beauty') beautyInstalled.add(game);
      publish({ installed, beautyInstalled });
    },
    dispose() {
      disposed = true;
      for (const controller of controllers) controller.abort();
      controllers.clear();
      listeners.clear();
    },
  };
}
export type MetadataService = ReturnType<typeof createMetadataService>;
