import { z } from 'zod';
import {
  isMultiplayerProductId,
  isProductId,
  PRODUCT_IDS,
  type ProductId,
} from '../../src/contracts/product-catalog.mts';
import { sha256Hex } from '../../src/launcher/sha256.mts';
import type { UiMessageKey } from '../i18n';
import { isAbortError, linkAbortSignals, waitWithAbort } from './abort-signals';
import { AccountRequestError, accountRequest } from './account-client';
import { accountSession } from './account-session';
import { type CloudSaveList, type CloudSaveSnapshot, listCloudSaves, uploadCloudSave } from './cloud-saves';
import type { LocalSaveReader } from './local-save-storage';

const prefix = 'eagler-cloud-sync-v1:';
const lockLeaseMs = 12_000;
const recordSchema = z.object({
  owner: z.string().max(12),
  product: z.custom<ProductId>((value) => isProductId(value) && !isMultiplayerProductId(value)),
  revision: z.number().int().nonnegative(),
  fingerprint: z.string().max(1024),
  runId: z.string().uuid().nullable(),
  activeUntil: z.number().finite().nonnegative(),
  unclaimed: z.boolean().default(false),
  registered: z.boolean().default(true),
});
type RecordState = z.infer<typeof recordSchema>;
type IdentityStatus = 'member' | 'guest' | 'unknown';
type ProductLock = { signal: AbortSignal; assert(): boolean; release(): void };
type Ports = {
  storage: Storage;
  read: LocalSaveReader;
  restore(
    product: ProductId,
    snapshot: CloudSaveSnapshot,
    signal: AbortSignal,
    allowedFiles: readonly string[],
  ): Promise<void>;
  decide(
    kind: 'conflict' | 'otherDevice' | 'offline' | 'otherAccount' | 'claim',
    product: ProductId,
    signal?: AbortSignal,
  ): Promise<'local' | 'cloud' | 'cancel'>;
  notify(key: UiMessageKey): void;
  isRunning(): boolean;
};
export type CloudSyncStatus = { product: ProductId; error: UiMessageKey } | null;
type ManifestFile = { name: string; sha256: string };
const manifestFingerprint = (files: ManifestFile[]) =>
  files
    .map((file) => `${file.name}:${file.sha256}`)
    .sort()
    .join('|');
export async function localSaveFingerprint(files: File[]) {
  return manifestFingerprint(
    await Promise.all(
      files.map(async (file) => ({
        name: file.name,
        sha256: await sha256Hex(new Uint8Array(await file.arrayBuffer())),
      })),
    ),
  );
}
export function createCloudSaveSync(ports: Ports) {
  let deviceId = ports.storage.getItem(prefix + 'device');
  if (!z.string().uuid().safeParse(deviceId).success) {
    deviceId = crypto.randomUUID();
    ports.storage.setItem(prefix + 'device', deviceId);
  }
  const device = deviceId!;
  let owner = '',
    identityStatus: IdentityStatus = 'unknown',
    generation = 0,
    disposed = false,
    launching = false,
    task: Promise<void> | null = null;
  let controller = new AbortController(),
    status: CloudSyncStatus = null;
  const live = new Map<ProductId, string>(),
    runLocks = new Map<ProductId, ProductLock>(),
    listeners = new Set<() => void>();
  const productIds = PRODUCT_IDS.filter((product): product is ProductId => !isMultiplayerProductId(product));
  const records = () =>
    productIds.flatMap((product) => {
      const raw = ports.storage.getItem(prefix + 'save:' + product);
      if (raw === null) return [];
      try {
        const parsed = recordSchema.safeParse(JSON.parse(raw));
        return parsed.success && parsed.data.product === product ? [parsed.data] : [];
      } catch {
        return [];
      }
    });
  const readRecord = (product: ProductId) => {
    const raw = ports.storage.getItem(prefix + 'save:' + product);
    if (raw === null) return undefined;
    try {
      const parsed = recordSchema.safeParse(JSON.parse(raw));
      return parsed.success && parsed.data.product === product ? parsed.data : undefined;
    } catch {
      return undefined;
    }
  };
  const writeRecord = (record: RecordState, lease?: ProductLock) => {
    if (lease && !lease.assert()) throw new DOMException('Product lock was lost', 'AbortError');
    ports.storage.setItem(prefix + 'save:' + record.product, JSON.stringify(record));
    if (lease && !lease.assert()) throw new DOMException('Product lock was lost', 'AbortError');
  };
  const publish = (value: CloudSyncStatus) => {
    status = value;
    for (const listener of listeners) listener();
  };
  const check = (signal: AbortSignal, user: string, lease?: ProductLock) => {
    if (signal.aborted || disposed || identityStatus !== 'member' || owner !== user || (lease && !lease.assert()))
      throw new DOMException('Cancelled', 'AbortError');
  };
  const identity = (record: RecordState) => ({ deviceId: device, runId: record.runId! });
  const complete = (record: RecordState, revision: number, signal: AbortSignal) =>
    accountRequest(
      `v1/me/cloud-saves/${record.product}/sessions/complete`,
      { ...identity(record), baseRevision: revision },
      { signal },
    );
  const register = (record: RecordState, signal: AbortSignal, allowPending = false) =>
    accountRequest(
      `v1/me/cloud-saves/${record.product}/sessions`,
      { ...identity(record), baseRevision: record.revision, allowPending },
      { signal },
    );
  const clean = (record: RecordState, revision: number, fingerprint: string, lease: ProductLock) => {
    const current = readRecord(record.product);
    if (current?.owner === record.owner && current.runId === record.runId)
      writeRecord(
        { ...record, revision, fingerprint, runId: null, activeUntil: 0, unclaimed: false, registered: true },
        lease,
      );
  };
  const errorKey = (error: unknown): UiMessageKey =>
    error instanceof AccountRequestError && error.status === 409
      ? 'cloudSave.syncConflict'
      : error instanceof AccountRequestError && error.code === 'SAVE_QUOTA_EXCEEDED'
        ? 'cloudSave.quota'
        : 'cloudSave.syncPending';

  async function acquireProductLock(product: ProductId): Promise<ProductLock | null> {
    const key = prefix + 'product:' + product;
    const manager = typeof navigator !== 'undefined' ? navigator.locks : undefined;
    if (manager) {
      let resolveLock!: (lease: ProductLock | null) => void;
      const acquired = new Promise<ProductLock | null>((resolve) => {
        resolveLock = resolve;
      });
      const request = manager
        .request(key, { mode: 'exclusive', ifAvailable: true }, async (lock) => {
          if (!lock) {
            resolveLock(null);
            return;
          }
          let releaseHeld!: () => void,
            done = false;
          const controller = new AbortController();
          const held = new Promise<void>((resolve) => {
            releaseHeld = resolve;
          });
          const lease: ProductLock = {
            signal: controller.signal,
            assert: () => !done && !controller.signal.aborted,
            release() {
              if (!done) {
                done = true;
                controller.abort();
                releaseHeld();
              }
            },
          };
          resolveLock(lease);
          await held;
        })
        .catch(() => resolveLock(null));
      const lease = await acquired;
      if (!lease) return null;
      return {
        ...lease,
        release() {
          lease.release();
          void request;
        },
      };
    }
    const keyName = prefix + 'lock:' + product,
      token = crypto.randomUUID();
    try {
      const current = JSON.parse(ports.storage.getItem(keyName) || 'null') as {
        token?: string;
        expiresAt?: number;
      } | null;
      if (current && typeof current.expiresAt === 'number' && current.expiresAt > Date.now()) return null;
      const write = () =>
        ports.storage.setItem(keyName, JSON.stringify({ token, expiresAt: Date.now() + lockLeaseMs }));
      write();
      const verify = () => {
        try {
          return (JSON.parse(ports.storage.getItem(keyName) || 'null') as { token?: string } | null)?.token === token;
        } catch {
          return false;
        }
      };
      if (!verify()) return null;
      const controller = new AbortController();
      let active = true;
      const assert = () => {
        if (!active) return false;
        if (verify()) return true;
        active = false;
        controller.abort();
        return false;
      };
      const timer = setInterval(() => {
        if (!assert()) return;
        try {
          write();
        } catch {
          active = false;
          controller.abort();
        }
      }, lockLeaseMs / 3);
      return {
        signal: controller.signal,
        assert,
        release() {
          if (active) {
            active = false;
            controller.abort();
            clearInterval(timer);
            if (verify())
              try {
                ports.storage.removeItem(keyName);
              } catch {}
          } else clearInterval(timer);
        },
      };
    } catch {
      throw new Error('Cloud sync storage is unavailable');
    }
  }

  async function synchronize(record: RecordState, data: CloudSaveList, signal: AbortSignal, lease: ProductLock) {
    if (!record.runId || record.owner !== owner || record.unclaimed || live.has(record.product)) return;
    const user = owner,
      product = data.products.find((item) => item.id === record.product && !item.multiplayer);
    if (!product) return;
    const files = await ports.read(record.product, product.files);
    check(signal, user, lease);
    const fingerprint = await localSaveFingerprint(files);
    check(signal, user, lease);
    const latest = data.saves.find((item) => item.product === record.product && item.revision === product.revision);
    const uploadedAlready = !!latest && fingerprint === manifestFingerprint(latest.files);
    // Upload atomically retires its marker. A lost response is recognized by the manifest.
    if (uploadedAlready) {
      await complete(record, product.revision, signal);
      check(signal, user, lease);
      clean(record, product.revision, fingerprint, lease);
      return;
    }
    if (fingerprint === record.fingerprint) {
      // Complete is idempotent when no marker exists, and also repairs the
      // register-success/localStorage-failure window.
      await complete(record, product.revision, signal);
      check(signal, user, lease);
      clean(record, record.revision, fingerprint, lease);
      return;
    }
    if (product.revision !== record.revision) throw new AccountRequestError('SAVE_CONFLICT', 409);
    if (!files.length || !product.requiredFiles.some((name) => files.some((file) => file.name === name)))
      throw new AccountRequestError('MAIN_SAVE_REQUIRED');
    let current = record;
    if (!current.registered) {
      await register(current, signal, true);
      check(signal, user, lease);
      current = { ...current, registered: true };
      writeRecord(current, lease);
    }
    const saved = await uploadCloudSave(current.product, current.revision, files, signal, identity(current));
    check(signal, user, lease);
    clean(current, saved.revision, fingerprint, lease);
    window.dispatchEvent(new Event('cloud-save-change'));
  }

  async function flush() {
    if (task || !owner || identityStatus !== 'member' || disposed || launching || ports.isRunning()) return task;
    const signal = controller.signal,
      user = owner;
    task = (async () => {
      let pending: RecordState[];
      try {
        pending = records().filter(
          (record) => record.owner === user && record.runId && !record.unclaimed && !live.has(record.product),
        );
      } catch {
        publish({ product: productIds[0]!, error: 'cloudSave.syncPending' });
        return;
      }
      for (const record of pending) {
        const lease = await acquireProductLock(record.product);
        if (!lease) continue;
        const linked = linkAbortSignals(signal, lease.signal),
          operationSignal = linked.signal!;
        try {
          check(operationSignal, user, lease);
          const current = readRecord(record.product);
          if (
            !current ||
            current.runId !== record.runId ||
            current.owner !== user ||
            current.unclaimed ||
            live.has(record.product)
          )
            continue;
          const data = await listCloudSaves(operationSignal);
          check(operationSignal, user, lease);
          await synchronize(current, data, operationSignal, lease);
          check(operationSignal, user, lease);
          publish(null);
        } catch (error) {
          if (!operationSignal.aborted) publish({ product: record.product, error: errorKey(error) });
        } finally {
          linked.dispose();
          lease.release();
        }
      }
    })()
      .catch(() => {
        publish({ product: productIds[0]!, error: 'cloudSave.syncPending' });
      })
      .finally(() => {
        task = null;
      });
    return task;
  }

  const currentAccount = () => {
    const state = accountSession.getSnapshot();
    if (state.status === 'member') return { status: 'member' as const, owner: state.user.username.toLowerCase() };
    if (state.status === 'guest') return { status: 'guest' as const, owner: '' };
    return { status: 'unknown' as const, owner: '' };
  };
  const changeOwner = () => {
    const next = currentAccount();
    if (next.status === identityStatus && next.owner === owner) return;
    identityStatus = next.status;
    owner = next.owner;
    generation++;
    controller.abort();
    controller = new AbortController();
    publish(null);
    if (identityStatus === 'member') void Promise.resolve(task).then(() => flush());
  };
  const unsubscribe = accountSession.subscribe(changeOwner);
  changeOwner();
  const timer = setInterval(() => {
    for (const [product, runId] of live)
      try {
        const record = readRecord(product),
          lease = runLocks.get(product);
        if (record?.runId === runId) writeRecord({ ...record, activeUntil: Date.now() + 45_000 }, lease);
      } catch {
        publish({ product, error: 'cloudSave.syncPending' });
      }
    void flush();
  }, 15_000);
  const online = () => {
    void flush();
  };
  window.addEventListener('online', online);

  async function saveOfflineRun(
    product: ProductId,
    signal: AbortSignal,
    user: string,
    knownMember: boolean,
    lease: ProductLock,
  ) {
    const answer = await ports.decide('offline', product, signal);
    if (
      answer === 'cancel' ||
      signal.aborted ||
      disposed ||
      (knownMember && (identityStatus !== 'member' || owner !== user))
    )
      return false;
    let record = readRecord(product);
    if (knownMember && record?.owner && record.owner !== user && record.runId) {
      await ports.decide('otherAccount', product, signal);
      return false;
    }
    const hasUnclaimedData = !!record && (record.unclaimed || record.owner !== user);
    if (knownMember && hasUnclaimedData) {
      const claim = await ports.decide('claim', product, signal);
      if (claim !== 'local' || signal.aborted || identityStatus !== 'member' || owner !== user) return false;
    }
    if (!knownMember && record?.owner) {
      record = {
        ...record,
        runId: record.runId ?? crypto.randomUUID(),
        activeUntil: Date.now() + 45_000,
        registered: record.runId ? record.registered : false,
      };
    } else if (record?.owner === user) {
      record = {
        ...record,
        runId: record.runId ?? crypto.randomUUID(),
        activeUntil: Date.now() + 45_000,
        unclaimed: !knownMember,
        registered: record.runId ? record.registered : false,
      };
    } else {
      record = {
        owner: knownMember ? user : '',
        product,
        revision: 0,
        fingerprint: '',
        runId: crypto.randomUUID(),
        activeUntil: Date.now() + 45_000,
        unclaimed: !knownMember,
        registered: false,
      };
    }
    try {
      writeRecord(record, lease);
    } catch {
      publish({ product, error: 'cloudSave.syncPending' });
      ports.notify('cloudSave.syncPending');
      return false;
    }
    live.set(product, record.runId!);
    publish(null);
    return true;
  }

  async function beforeLaunchCore(
    product: ProductId,
    lease: ProductLock,
    externalSignal?: AbortSignal,
  ): Promise<boolean> {
    if (isMultiplayerProductId(product)) return true;
    if (externalSignal) await waitWithAbort(accountSession.whenReady(), externalSignal);
    else await accountSession.whenReady();
    if (task) {
      if (externalSignal) await waitWithAbort(task, externalSignal);
      else await task;
    }
    if (disposed || externalSignal?.aborted) return false;
    const user = owner,
      knownMember = identityStatus === 'member',
      statusAtStart = identityStatus,
      revision = generation;
    const linked = linkAbortSignals(controller.signal, externalSignal, lease.signal),
      signal = linked.signal!;
    const recordAtStart = readRecord(product);
    try {
      if (!knownMember) {
        if (statusAtStart === 'unknown') return await saveOfflineRun(product, signal, user, false, lease);
        if (recordAtStart?.owner) {
          await ports.decide('otherAccount', product, signal);
          return false;
        }
        let record = recordAtStart;
        const runId = record?.runId ?? crypto.randomUUID();
        record = record
          ? { ...record, runId, activeUntil: Date.now() + 45_000, registered: record.runId ? record.registered : false }
          : {
              owner: '',
              product,
              revision: 0,
              fingerprint: '',
              runId,
              activeUntil: Date.now() + 45_000,
              unclaimed: true,
              registered: false,
            };
        try {
          writeRecord(record, lease);
        } catch {
          publish({ product, error: 'cloudSave.syncPending' });
          ports.notify('cloudSave.syncPending');
          return false;
        }
        live.set(product, runId!);
        return true;
      }
      if (recordAtStart?.owner && recordAtStart.owner !== user && recordAtStart.runId) {
        await ports.decide('otherAccount', product, signal);
        return false;
      }
      let data: CloudSaveList;
      try {
        data = await listCloudSaves(signal);
        check(signal, user, lease);
      } catch (error) {
        if (signal.aborted) return false;
        return saveOfflineRun(product, signal, user, true, lease);
      }
      const definition = data.products.find((item) => item.id === product && !item.multiplayer);
      if (!definition) return true;
      let record = readRecord(product);
      if (record?.owner === user && record.runId && !record.unclaimed) {
        try {
          await synchronize({ ...record, activeUntil: 0 }, data, signal, lease);
          check(signal, user, lease);
          data = await listCloudSaves(signal);
          check(signal, user, lease);
          record = readRecord(product);
        } catch (error) {
          if (signal.aborted) return false;
          if (!(error instanceof AccountRequestError && error.status === 409)) {
            ports.notify(errorKey(error));
            return false;
          }
        }
      }
      const current = data.products.find((item) => item.id === product)!;
      const latest = data.saves.find((item) => item.product === product && item.revision === current.revision);
      let files = await ports.read(product, current.files);
      check(signal, user, lease);
      let fingerprint = await localSaveFingerprint(files);
      check(signal, user, lease);
      let cloudFingerprint = latest ? manifestFingerprint(latest.files) : '';
      const hasForeignOwner = !!record && record.owner !== user;
      const localChanged =
        files.length > 0 && (!record || hasForeignOwner || record.unclaimed || record.fingerprint !== fingerprint);
      const cloudChanged = !record || hasForeignOwner || record.revision !== current.revision;
      let choice: 'cloud' | 'local' | 'cancel' = !localChanged && cloudChanged && latest ? 'cloud' : 'local';
      if (files.length && !latest && (hasForeignOwner || record?.unclaimed)) {
        choice = await ports.decide('claim', product, signal);
      } else if (
        (localChanged && cloudChanged && latest && fingerprint !== cloudFingerprint) ||
        (record?.unclaimed && !!latest && files.length > 0) ||
        (hasForeignOwner && !!latest && files.length > 0)
      ) {
        choice = await ports.decide('conflict', product, signal);
      }
      check(signal, user, lease);
      if (choice === 'cancel') return false;
      if (choice === 'cloud' && latest) {
        await ports.restore(product, latest, signal, current.files);
        check(signal, user, lease);
        files = await ports.read(product, current.files);
        fingerprint = await localSaveFingerprint(files);
        check(signal, user, lease);
        cloudFingerprint = latest ? manifestFingerprint(latest.files) : '';
      }
      const other = data.pending.some((item) => item.product === product && item.deviceId !== device);
      if (other && (await ports.decide('otherDevice', product, signal)) === 'cancel') return false;
      check(signal, user, lease);
      if (record?.owner === user && record.runId) {
        if (!record.unclaimed) {
          await complete(record, current.revision, signal);
          check(signal, user, lease);
        } else if (record.registered) {
          await complete(record, current.revision, signal);
          check(signal, user, lease);
        }
      }
      const run: RecordState = {
        owner: user,
        product,
        revision: current.revision,
        fingerprint: cloudFingerprint,
        runId: crypto.randomUUID(),
        activeUntil: Date.now() + 45_000,
        unclaimed: false,
        registered: false,
      };
      try {
        writeRecord(run, lease);
      } catch {
        publish({ product, error: 'cloudSave.syncPending' });
        ports.notify('cloudSave.syncPending');
        return false;
      }
      try {
        await register(run, signal, other);
        check(signal, user, lease);
        const registered = { ...run, registered: true };
        try {
          writeRecord(registered, lease);
        } catch {
          publish({ product, error: 'cloudSave.syncPending' });
          ports.notify('cloudSave.syncPending');
          return false;
        }
      } catch (error) {
        if (signal.aborted) return false;
        if (error instanceof AccountRequestError && error.status === 409) {
          publish({ product, error: 'cloudSave.syncConflict' });
          ports.notify('cloudSave.syncConflict');
          return false;
        }
        const answer = await ports.decide('offline', product, signal);
        if (
          answer === 'cancel' ||
          signal.aborted ||
          generation !== revision ||
          identityStatus !== 'member' ||
          owner !== user
        )
          return false;
      }
      live.set(product, run.runId!);
      publish(null);
      return true;
    } finally {
      linked.dispose();
    }
  }

  return {
    async beforeLaunch(product: ProductId, signal?: AbortSignal) {
      if (launching || disposed || signal?.aborted) return false;
      launching = true;
      let started = false;
      let lease: ProductLock | null = null;
      try {
        if (isMultiplayerProductId(product)) return true;
        try {
          lease = await acquireProductLock(product);
        } catch {
          publish({ product, error: 'cloudSave.syncPending' });
          ports.notify('cloudSave.syncPending');
          return false;
        }
        if (!lease) {
          ports.notify('cloudSave.sameBrowserActive');
          return false;
        }
        if (disposed || signal?.aborted) return false;
        try {
          started = await beforeLaunchCore(product, lease, signal);
        } catch (error) {
          if (!isAbortError(error)) {
            publish({ product, error: errorKey(error) });
            ports.notify(errorKey(error));
          }
          return false;
        }
        if (started) {
          runLocks.set(product, lease);
          return true;
        }
        return false;
      } finally {
        if (!started) lease?.release();
        launching = false;
        if (!started) void flush();
      }
    },
    flush,
    finished() {
      try {
        for (const [product, runId] of live)
          try {
            const record = readRecord(product),
              lease = runLocks.get(product);
            if (record?.runId === runId) writeRecord({ ...record, activeUntil: 0 }, lease);
          } catch {
            publish({ product, error: 'cloudSave.syncPending' });
          }
      } finally {
        live.clear();
        for (const lease of runLocks.values())
          try {
            lease.release();
          } catch {}
        runLocks.clear();
        void flush();
      }
    },
    subscribe(listener: () => void) {
      listeners.add(listener);
      return () => {
        listeners.delete(listener);
      };
    },
    getSnapshot: () => status,
    dispose() {
      disposed = true;
      controller.abort();
      unsubscribe();
      clearInterval(timer);
      window.removeEventListener('online', online);
      for (const lease of runLocks.values())
        try {
          lease.release();
        } catch {}
      runLocks.clear();
      listeners.clear();
    },
  };
}
export type CloudSaveSync = ReturnType<typeof createCloudSaveSync>;
