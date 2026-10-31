import { zipSync } from 'fflate';
import { gameIdForProduct, isMultiplayerProductId, type ProductId } from '../../src/contracts/product-catalog.mts';
import type { RuntimeResponseMessage } from '../../src/contracts/runtime-protocol.mts';
import { parseReplayDescription, type ReplayDescription } from '../../src/launcher/replay-description.mts';
import {
  allocateReplayName,
  createReplayMutationQueue,
  isReplayFilePath,
  isReplayTargetAvailable,
  type ReplayMutationQueue,
  replayStoragePathKey,
} from '../../src/launcher/replay-files.mts';
import { replayFilePolicy } from '../../src/launcher/replay-storage-policy.mts';
import {
  isTh09ReplayDirectory,
  parseTh09ReplayMatch,
  type Th09ReplayMatch,
} from '../../src/launcher/th09-replay-match.mts';
import type { SettingsDecision } from '../components/settings/types';
import type { ReplayStorageReader } from '../services/replay-storage';
import type { RuntimeFileSession, RuntimeService } from '../services/runtime';

export interface ReplayRow {
  readonly path: string;
  readonly name: string;
  readonly size: number;
  readonly modifiedAt?: number;
  readonly replayMatch?: Th09ReplayMatch;
}
export interface ReplayFolder {
  readonly key: string;
  readonly name: string;
  readonly rows: ReplayRow[];
}
export function groupReplayFolders(rows: readonly ReplayRow[]): ReplayFolder[] {
  const folders = new Map<string, ReplayFolder>();
  for (const row of rows) {
    const parts = row.path.split('/');
    if (parts.length !== 3) continue;
    const key = parts.slice(0, -1).join('/');
    let folder = folders.get(key);
    if (!folder) {
      folder = { key, name: parts[1]!, rows: [] };
      folders.set(key, folder);
    }
    folder.rows.push(row);
  }
  return [...folders.values()];
}
export interface ReplaySnapshot {
  readonly open: boolean;
  readonly phase: 'idle' | 'loading' | 'ready' | 'error';
  readonly productId: ProductId | null;
  readonly rows: readonly ReplayRow[];
  readonly animateRows: boolean;
  readonly descriptions: Readonly<Record<string, ReplayDescription>>;
  readonly inspecting: string | null;
}
export interface ReplayModelOptions {
  runtime: RuntimeService;
  prepareFiles(product: ProductId): Promise<void>;
  /** A persistent viewer owns its launched Runtime until this manager closes. */
  prepareViewer?(product: ProductId): Promise<void>;
  /** Browsing/downloads read persisted IDBFS without acquiring a Runtime. */
  storage?: ReplayStorageReader;
  closeViewer?(): void;
  /** Host must recheck product/epoch before retiring an owned idle preload. */
  releasePrepared(product: ProductId, epoch: number): Promise<void>;
  translate(key: string, params?: Record<string, string | number>): string;
  prompt(message: string, initial: string): string | null | Promise<string | null>;
  confirm(decision: SettingsDecision): Promise<boolean>;
  download(name: string, bytes: ArrayBuffer, mime: string): void;
  toast(message: string): void;
  afterPaint(): Promise<void>;
  afterClose(): void;
  mutations?: ReplayMutationQueue;
  watch?(product: ProductId, path: string, stage?: number): Promise<void>;
  inspect?(product: ProductId, path: string): Promise<unknown>;
}
function errorMessage(error: unknown): string {
  if (error instanceof Error) return error.message;
  if (
    error &&
    typeof error === 'object' &&
    !Array.isArray(error) &&
    'message' in error &&
    typeof error.message === 'string'
  )
    return error.message;
  return String(error ?? '');
}
export const formatReplayBytes = (bytes: number) =>
  bytes < 1024 * 1024 ? `${(bytes / 1024).toFixed(1)} KiB` : `${(bytes / 1024 / 1024).toFixed(2)} MiB`;

/** Original Replay manager state. The dialog view owns only presentation and
 * calls close after its original exit animation/native-dialog close completes. */
export function createReplayModel(options: ReplayModelOptions) {
  const { runtime, translate: t } = options;
  const mutations = options.mutations ?? createReplayMutationQueue();
  const listeners = new Set<() => void>();
  let snapshot: ReplaySnapshot = Object.freeze({
    open: false,
    phase: 'idle',
    productId: null,
    rows: [],
    animateRows: false,
    descriptions: {},
    inspecting: null,
  });
  let generation = 0,
    readVersion = 0,
    disposed = false;
  let closeWork: Promise<void> = Promise.resolve(),
    watching = false;
  let viewerPreparation: { product: ProductId; promise: Promise<void> } | null = null;
  const inspections = new Map<string, Promise<void>>();
  type TemporaryOwner = { product: ProductId; epoch: number | null };
  let runtimeOwner: TemporaryOwner | null = null;
  let retainedOwner: TemporaryOwner | null = null;
  // Native informational-dialog Forward retains its existing rows and any
  // in-flight listing. Reopening visibility must not start a second file read.
  let retainedRead: { product: ProductId; animateRows: boolean; promise: Promise<ReplayRow[]> } | null = null;
  function publish(patch: Partial<ReplaySnapshot>) {
    if (disposed) return;
    snapshot = Object.freeze({ ...snapshot, ...patch });
    for (const listener of listeners) listener();
  }
  function valid(token: number, product: ProductId) {
    return !disposed && snapshot.open && generation === token && snapshot.productId === product;
  }
  function prepareViewer(product: ProductId): Promise<void> {
    if (viewerPreparation?.product === product) return viewerPreparation.promise;
    const preparation = { product, promise: (async () => options.prepareViewer!(product))() };
    viewerPreparation = preparation;
    const settled = () => {
      if (viewerPreparation === preparation) viewerPreparation = null;
    };
    void preparation.promise.then(settled, settled);
    return preparation.promise;
  }
  function preloadViewer(product: ProductId, token: number): void {
    if (!options.storage || !options.prepareViewer) return;
    // Listing and downloads stay independent of the eagerly prepared player.
    void prepareViewer(product)
      .then(async () => {
        if (!options.inspect || gameIdForProduct(product) === 'th09') return;
        await retainedRead?.promise.catch(() => {});
        for (const row of snapshot.rows) {
          if (!valid(token, product)) break;
          await inspect(row.path, true);
          await options.afterPaint();
        }
      })
      .catch((error) => {
        if (valid(token, product) && !watching)
          options.toast(t('replay.operationFailed', { reason: errorMessage(error) }));
      });
  }
  function responseBytes(response: RuntimeResponseMessage): number[] {
    if (
      !Array.isArray(response.bytes) ||
      !response.bytes.every((value) => Number.isInteger(value) && value >= 0 && value <= 255)
    )
      throw new Error(t('runtime.invalidFileContent'));
    return response.bytes as number[];
  }
  function responseFiles(response: RuntimeResponseMessage, product: ProductId): ReplayRow[] {
    if (!Array.isArray(response.files)) throw new Error(t('runtime.invalidFileList'));
    return response.files.map((value) => {
      if (!value || typeof value !== 'object' || !('path' in value) || typeof value.path !== 'string' || !value.path)
        throw new Error(t('runtime.invalidFileEntry'));
      const match = product === 'th09mp' && 'replayMatch' in value ? parseTh09ReplayMatch(value.replayMatch) : null;
      const modifiedAt = Number('modifiedAt' in value ? value.modifiedAt : 0);
      return {
        path: value.path,
        name: value.path.split('/').pop() || value.path,
        ...(match ? { replayMatch: match } : {}),
        ...(Number.isFinite(modifiedAt) && modifiedAt > 0 && modifiedAt <= 8.64e15 ? { modifiedAt } : {}),
        size: Math.max(0, Number('size' in value ? value.size : 0) || 0),
      };
    });
  }
  async function withFiles<T>(
    product: ProductId,
    operation: (access: RuntimeFileSession) => Promise<T>,
    mutation = false,
  ): Promise<T> {
    const fileOwner = runtimeOwner ?? retainedOwner;
    if (options.prepareViewer && !options.storage) await options.prepareViewer(product);
    else await options.prepareFiles(product);
    return runtime.withFileSession(
      gameIdForProduct(product),
      async (access) => {
        if (!runtime.getSnapshot().launched && options.storage && fileOwner?.product === product)
          fileOwner.epoch = access.epoch;
        return operation(access);
      },
      {
        runtimeVariant: isMultiplayerProductId(product) ? 'multiplayer' : 'normal',
        readOnly: !mutation,
        replayMutation: mutation,
      },
    );
  }
  async function cleanup(owner: TemporaryOwner, token: number): Promise<void> {
    await mutations.idle();
    // A cold read may not have acquired its exact native epoch when closed.
    if (owner.epoch === null && retainedRead?.product === owner.product) await retainedRead.promise.catch(() => {});
    const { product, epoch } = owner;
    if (disposed || snapshot.open || generation !== token || epoch === null) return;
    const current = runtime.getSnapshot();
    if (!current.launched && current.epoch === epoch && current.game === gameIdForProduct(product)) {
      await options.releasePrepared(product, epoch);
      if (retainedOwner === owner) retainedOwner = null;
    }
  }
  async function refresh({
    animateRows = false,
    afterMutation = false,
  }: {
    animateRows?: boolean;
    afterMutation?: boolean;
  } = {}): Promise<void> {
    const product = snapshot.productId,
      token = generation,
      version = ++readVersion;
    if (!product || !snapshot.open || disposed) return;
    const ownerAtStart = runtimeOwner;
    // Import's committed writer still owns the mutation queue while requesting
    // this read. Waiting for that same queue here would wait on ourselves.
    const promise = options.storage
      ? (afterMutation ? Promise.resolve() : mutations.idle()).then(async () =>
          (await options.storage!.read(product)).map(({ bytes: _bytes, ...file }) => ({
            ...file,
            name: file.path.split('/').pop() || file.path,
          })),
        )
      : withFiles(product, async (access) => {
          if (ownerAtStart && ownerAtStart.epoch === null) ownerAtStart.epoch = access.epoch;
          await access.sync();
          return responseFiles(await access.send('list', {}), product).filter((file) => isReplayFilePath(file.path));
        });
    retainedRead = { product, animateRows, promise };
    const rows = (await promise).sort(
      (a, b) => (b.replayMatch?.startedAt ?? 0) - (a.replayMatch?.startedAt ?? 0) || a.path.localeCompare(b.path),
    );
    if (valid(token, product) && readVersion === version) {
      publish({ phase: 'ready', rows: Object.freeze(rows), animateRows, descriptions: {} });
      preloadViewer(product, token);
    }
  }

  async function open(product: ProductId): Promise<void> {
    if (disposed) return;
    const token = ++generation;
    retainedRead = null;
    const current = runtime.getSnapshot();
    if (options.storage || (!runtimeOwner && !current.ready && current.epoch === null))
      runtimeOwner = { product, epoch: null };
    retainedOwner = null;
    publish({
      descriptions: {},
      inspecting: null,
      open: true,
      phase: 'loading',
      productId: product,
      rows: [],
      animateRows: false,
    });
    await options.afterPaint();
    if (!valid(token, product)) return;
    const listing = refresh({ animateRows: true });
    // Preparation starts independently of the file read. Native descriptions
    // are filled after the new list is published, including after mutations.
    void (options.prepareViewer && prepareViewer(product).catch(() => {}));
    try {
      await listing;
    } catch (error) {
      if (valid(token, product)) publish({ phase: 'error', rows: [], animateRows: false });
      throw error;
    }
  }
  function reopenRetained(): void {
    const product = snapshot.productId;
    if (disposed || snapshot.open || !product) return;
    const token = ++generation,
      read = retainedRead;
    const current = runtime.getSnapshot();
    if (retainedOwner?.product === product && !current.launched && current.epoch === retainedOwner.epoch)
      runtimeOwner = retainedOwner;
    publish({ open: true });
    preloadViewer(product, token);
    if (snapshot.phase !== 'loading') return;
    // Closing before the first paint may have prevented the initial read.
    if (!read) {
      void refresh({ animateRows: true }).catch((error) => {
        if (valid(token, product)) {
          publish({ phase: 'error', rows: [], animateRows: false });
          options.toast(errorMessage(error));
        }
      });
      return;
    }
    if (read.product !== product) return;
    void read.promise.then(
      (rows) => {
        if (valid(token, product) && retainedRead === read)
          publish({ phase: 'ready', rows: Object.freeze(rows), animateRows: read.animateRows });
      },
      () => {
        if (valid(token, product) && retainedRead === read) publish({ phase: 'error', rows: [], animateRows: false });
      },
    );
  }
  function close(): void {
    if (!snapshot.open || disposed) return;
    const product = snapshot.productId;
    const token = ++generation;
    const release = runtimeOwner;
    retainedOwner = release ?? retainedOwner;
    runtimeOwner = null;
    publish({ open: false });
    if (product && options.prepareViewer) options.closeViewer?.();
    if (release && product) {
      closeWork = cleanup(release, token);
      void closeWork
        .catch((error) => options.toast(t('replay.operationFailed', { reason: errorMessage(error) })))
        .finally(options.afterClose);
    } else options.afterClose();
  }
  async function inspect(path: string, background = false): Promise<void> {
    const product = snapshot.productId,
      token = generation,
      version = readVersion;
    if (!product || !valid(token, product) || !options.inspect || !snapshot.rows.some((row) => row.path === path))
      return;
    if (snapshot.descriptions[path]) return;
    if (!background) publish({ inspecting: path });
    const key = `${token}:${version}:${path}`;
    let pending = inspections.get(key);
    if (!pending) {
      pending = (async () => {
        if (options.prepareViewer) await prepareViewer(product);
        await mutations.idle();
        if (!valid(token, product) || readVersion !== version) return;
        const description = parseReplayDescription(await options.inspect!(product, path));
        if (valid(token, product) && readVersion === version)
          publish({ descriptions: { ...snapshot.descriptions, [path]: description } });
      })();
      inspections.set(key, pending);
      void pending.finally(() => inspections.delete(key)).catch(() => {});
    }
    try {
      await pending;
    } catch (error) {
      if (!background && valid(token, product))
        options.toast(t('replay.operationFailed', { reason: errorMessage(error) }));
    } finally {
      if (valid(token, product) && snapshot.inspecting === path) publish({ inspecting: null });
    }
  }
  async function watch(path: string, stage?: number): Promise<void> {
    const product = snapshot.productId,
      token = generation;
    if (watching || !product || !valid(token, product)) return;
    watching = true;
    try {
      if (
        !options.watch ||
        !isReplayFilePath(path) ||
        (gameIdForProduct(product) !== 'th09' && !snapshot.descriptions[path]?.stages.some((item) => item.id === stage))
      )
        throw new Error(t('replay.unsupported'));
      await mutations.idle();
      if (!valid(token, product)) return;
      if (runtime.getSnapshot().launched && !options.prepareViewer) throw new Error(t('replay.stopBeforeWatch'));
      if (!snapshot.rows.some((row) => row.path === path)) throw new Error(t('runtime.invalidFileEntry'));
      if (options.prepareViewer) {
        await prepareViewer(product);
        if (valid(token, product)) await options.watch(product, path, stage);
        return;
      }
      // Retire the manager's temporary native owner before playback acquires
      // its own epoch. Router Back may later reopen the retained folder view.
      close();
      const closedGeneration = generation;
      await closeWork;
      if (!disposed && generation === closedGeneration && !snapshot.open) await options.watch(product, path, stage);
    } catch (error) {
      if (!disposed && (!options.prepareViewer || valid(token, product)))
        options.toast(t('replay.operationFailed', { reason: errorMessage(error) }));
    } finally {
      watching = false;
    }
  }
  async function download(path: string): Promise<void> {
    const product = snapshot.productId,
      token = generation;
    if (!product || !valid(token, product)) return;
    try {
      if (options.storage) {
        const files = await mutations.run(() => options.storage!.read(product, { path }));
        if (!valid(token, product)) return;
        const file = files.find((file) => file.path === path);
        if (!file) throw new Error(t('runtime.invalidFileEntry'));
        const copy = new Uint8Array(file.bytes.length);
        copy.set(file.bytes);
        options.download(path.split('/').pop() || 'replay.rpy', copy.buffer, 'application/octet-stream');
        return;
      }
      const content = await withFiles(product, async (access) => responseBytes(await access.send('read', { path })));
      if (!valid(token, product)) return;
      const bytes = new Uint8Array(content);
      options.download(path.split('/').pop() || 'replay.rpy', bytes.buffer, 'application/octet-stream');
    } catch (error) {
      if (valid(token, product)) options.toast(t('replay.downloadFailed', { reason: errorMessage(error) }));
    }
  }
  async function rename(path: string, suppliedName?: string): Promise<void> {
    const product = snapshot.productId,
      token = generation;
    if (!product || !valid(token, product)) return;
    try {
      const name = path.split('/').pop() || path;
      const renamed = suppliedName ?? (await options.prompt(t('replay.renamePrompt'), name));
      if (renamed === null || !valid(token, product)) return;
      if (!renamed.trim()) throw new Error(t('replay.nameEmpty'));
      const { prefix, validName, sharedExtensionSlot } = replayFilePolicy(product);
      if (!prefix) throw new Error(t('replay.unsupported'));
      if (!validName(renamed.trim())) throw new Error(t('replay.nameInvalid', { prefix }));
      const target = `${path.slice(0, path.lastIndexOf('/'))}/${renamed.trim()}`;
      if (replayStoragePathKey(target, sharedExtensionSlot) === replayStoragePathKey(path, sharedExtensionSlot)) return;
      await mutations.run(async () => {
        // Once the user committed this mutation, closing the manager waits for
        // its queue; it must not silently cancel the requested file operation.
        if (disposed) return;
        await withFiles(
          product,
          async (access) => {
            await access.sync();
            const currentFiles = responseFiles(await access.send('list', {}), product);
            const currentPaths = currentFiles.map((file) => file.path);
            if (!isReplayTargetAvailable(currentPaths, target, sharedExtensionSlot))
              throw new Error(t('replay.nameExists'));
            const content = responseBytes(await access.send('read', { path }));
            const replayMatch = currentFiles.find((file) => file.path === path)?.replayMatch;
            await access.send('write', { path: target, bytes: content, ...(replayMatch ? { replayMatch } : {}) });
            await access.send('remove', { path });
          },
          true,
        );
      });
      if (valid(token, product)) await refresh();
    } catch (error) {
      if (valid(token, product)) options.toast(t('replay.operationFailed', { reason: errorMessage(error) }));
    }
  }
  async function move(path: string, directory: string): Promise<boolean> {
    const product = snapshot.productId,
      token = generation;
    if (!product || !valid(token, product)) return false;
    try {
      if (!isTh09ReplayDirectory(directory) || !isReplayFilePath(path)) throw new Error(t('replay.folderInvalid'));
      if (path.slice(0, path.lastIndexOf('/')).toLowerCase() === directory.toLowerCase()) return true;
      await mutations.run(async () => {
        if (disposed) throw new Error(t('replay.operationCancelled'));
        await withFiles(
          product,
          async (access) => {
            await access.sync();
            const files = responseFiles(await access.send('list', {}), product),
              source = files.find((file) => file.path === path);
            if (!source) throw new Error(t('runtime.invalidFileEntry'));
            const { prefix, sharedExtensionSlot } = replayFilePolicy(product);
            const name = allocateReplayName(
              prefix,
              files.map((file) => file.path),
              source.name,
              sharedExtensionSlot,
              directory,
            );
            if (!name) throw new Error(t('file.replaySlotsExhausted'));
            const target = `${directory}/${name}`,
              content = responseBytes(await access.send('read', { path }));
            await access.send('write', {
              path: target,
              bytes: content,
              ...(source.replayMatch ? { replayMatch: source.replayMatch } : {}),
            });
            // The destination must survive a flush before removing the source.
            await access.sync();
            const saved = responseBytes(await access.send('read', { path: target }));
            if (saved.length !== content.length || saved.some((byte, index) => byte !== content[index]))
              throw new Error(t('file.saveVerifyFailed'));
            await access.send('remove', { path });
          },
          true,
        );
      });
      if (valid(token, product)) await refresh();
      return true;
    } catch (error) {
      if (valid(token, product)) options.toast(t('replay.operationFailed', { reason: errorMessage(error) }));
      return false;
    }
  }
  async function downloadFolder(directory: string): Promise<void> {
    const product = snapshot.productId,
      token = generation;
    if (!product || !valid(token, product)) return;
    try {
      if (!isTh09ReplayDirectory(directory) || directory === 'replay') throw new Error(t('replay.folderInvalid'));
      const bytes = await mutations.run(async () => {
        if (options.storage) {
          const files = await options.storage.read(product, { directory });
          if (!files.length) throw new Error(t('file.noReplayToExport'));
          const entries: Record<string, Uint8Array> = {};
          for (const file of files) {
            entries[file.path] = file.bytes;
            if (file.replayMatch)
              entries[file.path.replace(/\.rpyx?$/, '.json')] = new TextEncoder().encode(
                JSON.stringify(file.replayMatch),
              );
          }
          return zipSync(entries, { level: 1 });
        }
        return withFiles(product, async (access) => {
          await access.sync();
          const files = responseFiles(await access.send('list', {}), product).filter(
            (file) => isReplayFilePath(file.path) && file.path.slice(0, file.path.lastIndexOf('/')) === directory,
          );
          if (!files.length) throw new Error(t('file.noReplayToExport'));
          const entries: Record<string, Uint8Array> = {};
          for (const file of files) {
            entries[file.path] = new Uint8Array(responseBytes(await access.send('read', { path: file.path })));
            if (file.replayMatch)
              entries[file.path.replace(/\.rpyx?$/, '.json')] = new TextEncoder().encode(
                JSON.stringify(file.replayMatch),
              );
          }
          return zipSync(entries, { level: 1 });
        });
      });
      if (!valid(token, product)) return;
      const copy = new Uint8Array(bytes.length);
      copy.set(bytes);
      options.download(`${directory.slice(7)}.zip`, copy.buffer, 'application/zip');
    } catch (error) {
      if (valid(token, product)) options.toast(t('replay.downloadFailed', { reason: errorMessage(error) }));
    }
  }
  async function remove(path: string): Promise<void> {
    const product = snapshot.productId,
      token = generation;
    if (!product || !valid(token, product)) return;
    try {
      if (
        !(await options.confirm({
          message: t('replay.deleteConfirm', { name: path.split('/').pop() || path }),
          confirmText: t('action.delete'),
          tone: 'danger',
        }))
      )
        return;
      if (!valid(token, product)) return;
      await mutations.run(async () => {
        // Once the user committed this mutation, closing the manager waits for
        // its queue; it must not silently cancel the requested file operation.
        if (disposed) return;
        await withFiles(
          product,
          async (access) => {
            await access.send('remove', { path });
          },
          true,
        );
      });
      if (valid(token, product)) await refresh();
    } catch (error) {
      if (valid(token, product)) options.toast(t('replay.deleteFailed', { reason: errorMessage(error) }));
    }
  }
  return {
    subscribe(listener: () => void) {
      listeners.add(listener);
      return () => {
        listeners.delete(listener);
      };
    },
    getSnapshot: () => snapshot,
    isOpen: () => snapshot.open,
    open,
    refresh,
    reopenRetained,
    close,
    inspect,
    watch,
    download,
    downloadFolder,
    rename,
    move,
    remove,
    mutations,
    dispose() {
      disposed = true;
      generation++;
      retainedRead = null;
      listeners.clear();
    },
  };
}
export type ReplayModel = ReturnType<typeof createReplayModel>;
