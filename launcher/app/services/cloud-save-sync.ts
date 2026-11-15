import { z } from 'zod';
import {
  isMultiplayerProductId,
  isProductId,
  PRODUCT_IDS,
  type ProductId,
} from '../../src/contracts/product-catalog.mts';
import type { UiMessageKey } from '../i18n';
import { isAbortError, linkAbortSignals, waitWithAbort } from './abort-signals';
import { AccountRequestError, accountRequest } from './account-client';
import { accountSession } from './account-session';
import { type CloudSaveList, type CloudSaveSnapshot, listCloudSaves, uploadCloudSave } from './cloud-saves';
import type { LocalSaveReader } from './local-save-storage';
import { compareSaveVersions, localSaveFingerprint, manifestFingerprint, relateSaveVersions, type SaveVersionComparison } from './cloud-save-comparison';

const prefix = 'eagler-cloud-sync-v1:';
const pausedKey = (owner: string) => prefix + 'paused:' + owner;
const reviewKey = (product: ProductId) => prefix + 'review:' + product;
const lockLeaseMs = 12_000;
const recordSchema = z.object({
  owner: z.string().max(12),
  product: z.custom<ProductId>((value) => typeof value === 'string' && isProductId(value) && !isMultiplayerProductId(value)),
  revision: z.number().int().nonnegative(),
  fingerprint: z.string().max(1024),
  runId: z.string().uuid().nullable(),
  unclaimed: z.boolean().default(false),
  verifiedBase: z.boolean().default(false),
});
type RecordState = z.infer<typeof recordSchema>;
type IdentityStatus = 'member' | 'guest' | 'unknown' | 'expired';
type ProductLock = { signal: AbortSignal; assert(): boolean; release(): void };
type ActiveRun = { product: ProductId; owner: string; lease?: ProductLock };
type LaunchCloudState = { product: ProductId; user: string; revision: number; pending: CloudSaveList['pending'] };
type LaunchReview = {
  beforeDecision(): Promise<void>;
  beforeRestore(): Promise<void>;
};
type Ports = {
  storage: Storage;
  read: LocalSaveReader;
  readForLaunch?: LocalSaveReader;
  restore(
    product: ProductId,
    snapshot: CloudSaveSnapshot,
    signal: AbortSignal,
    allowedFiles: readonly string[],
    preparing?: boolean,
    beforeWrite?: () => Promise<void>,
  ): Promise<void>;
  decide(
    kind: 'conflict' | 'claim',
    product: ProductId,
    signal?: AbortSignal,
    comparison?: SaveVersionComparison,
    context?: 'launch' | 'manual',
  ): Promise<'local' | 'cloud' | 'cancel'>;
  decideStart(kind: 'other-device' | 'tab-unavailable', signal: AbortSignal): Promise<boolean>;
  closeForHandoff(product: ProductId): Promise<boolean>;
  retirePrepared(product: ProductId): Promise<boolean>;
  notify(key: UiMessageKey): void;
  isRunning(): boolean;
  isPlaying?(): boolean;
};
export type CloudSyncStatus = { product: ProductId; error: UiMessageKey } | null;
export function createCloudSaveSync(ports: Ports) {
  const deviceKey = prefix + 'device';
  const savedDevice = ports.storage.getItem(deviceKey);
  const device = z.string().uuid().safeParse(savedDevice).data ?? crypto.randomUUID();
  if (device !== savedDevice) ports.storage.setItem(deviceKey, device);
  let owner = '',
    identityStatus: IdentityStatus = 'unknown',
    enabled = true,
    disposed = false,
    active: ActiveRun | null = null,
    task: Promise<void> | null = null;
  let controller = new AbortController(),
    status: CloudSyncStatus = null;
  let markerTask: Promise<void> | null = null;
  let launchData: LaunchCloudState | null = null;
  const rejectedUploads = new Map<string, {fingerprint: string; error: AccountRequestError}>();
  let retryAfter = 0;
  let launchLease: {product: ProductId; lease: ProductLock} | null = null;
  const handoff = typeof window.BroadcastChannel === 'function' ? new window.BroadcastChannel(prefix + 'handoff') : null;
  let closingForHandoff = false;
  handoff?.addEventListener('message', ({data}: MessageEvent<unknown>) => {
    if (disposed || closingForHandoff || !active?.lease?.assert() || data !== active.product) return;
    closingForHandoff = true;
    void ports.closeForHandoff(active.product).catch(() => false).finally(() => { closingForHandoff = false; });
  });
  const listeners = new Set<() => void>();
  const productIds = PRODUCT_IDS.filter((product): product is ProductId => !isMultiplayerProductId(product));
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
  const records = () => productIds.map(readRecord).filter(record => record !== undefined);
  const writeRecord = (record: RecordState, lease?: ProductLock) => {
    if (lease && !lease.assert()) throw new DOMException('Product lock was lost', 'AbortError');
    ports.storage.setItem(prefix + 'save:' + record.product, JSON.stringify(record));
    if (lease && !lease.assert()) throw new DOMException('Product lock was lost', 'AbortError');
  };
  const publish = (value: CloudSyncStatus) => {
    status = value;
    for (const listener of listeners) listener();
  };
  const check = (signal: AbortSignal, user: string, lease: ProductLock, requireEnabled = true) => {
    if (signal.aborted || disposed || (requireEnabled && !enabled) || owner !== user || !lease.assert() || !validAccount())
      throw new DOMException('Cancelled', 'AbortError');
  };
  const clean = (record: RecordState, revision: number, fingerprint: string, verifiedBase: boolean, lease: ProductLock) => {
    const current = readRecord(record.product);
    if (current?.owner === record.owner && current.runId === record.runId) {
      writeRecord(
        { ...record, revision, fingerprint, verifiedBase, runId: null, unclaimed: false },
        lease,
      );
      window.dispatchEvent(new Event('cloud-save-change'));
    }
  };
  function errorKey(error: unknown): UiMessageKey {
    if (!(error instanceof AccountRequestError)) return 'cloudSave.syncPending';
    if (error.code === 'SAVE_CONFLICT') return 'cloudSave.syncConflict';
    if (error.code === 'SAVE_QUOTA_EXCEEDED') return 'cloudSave.quota';
    if (error.code === 'UNSUPPORTED_SAVE_VERSION') return 'cloudSave.unsupportedVersion';
    if (error.status === 429) return 'cloudSave.busy';
    return 'cloudSave.syncPending';
  }

  function validAccount() {
    const state = accountSession.getSnapshot();
    if (state.status !== 'member') return false;
    if (state.expiresAt === undefined || state.expiresAt > Date.now()) return true;
    accountSession.expire(state.user.username);
    return false;
  }
  function sessionExpired(error: unknown, user: string) {
    if (!(error instanceof AccountRequestError) || error.status !== 401) return false;
    accountSession.expire(user);
    return true;
  }

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

  async function completeMarker(product: ProductId, user: string, data: CloudSaveList, revision: number, signal: AbortSignal) {
    const marker = data.pending.find(item => item.product === product && item.deviceId === device);
    if (!marker) return;
    try {
      await bounded(requestSignal => accountRequest(
        `v1/me/cloud-saves/${product}/sessions/complete`,
        {deviceId: device, runId: marker.runId, baseRevision: revision},
        {signal: requestSignal},
      ), signal);
    } catch (error) {
      if (!signal.aborted) sessionExpired(error, user);
    }
  }

  async function synchronize(record: RecordState, data: CloudSaveList, signal: AbortSignal, lease: ProductLock) {
    const product = data.products.find(item => item.id === record.product && !item.multiplayer);
    if (!product) return;
    const verify = () => {
      check(signal, record.owner, lease);
      if (active || ports.isRunning()) throw new DOMException('Cancelled', 'AbortError');
      if (needsReview(record.product)) throw new DOMException('Cancelled', 'AbortError');
    };
    verify();
    const files = await ports.read(record.product, product.files);
    verify();
    const fingerprint = await localSaveFingerprint(files);
    verify();
    const latest = data.saves.find(item => item.product === record.product && item.revision === product.revision);
    const cloudFingerprint = latest && manifestFingerprint(latest.files);
    let revision = product.revision;
    if (fingerprint === cloudFingerprint) {
      // Another device saved the same bytes, or the previous upload response was lost.
      clean(record, revision, fingerprint, true, lease);
    } else if (fingerprint === record.fingerprint) {
      // Leave the common base intact so the next launch can pull a newer cloud save.
      clean(record, record.revision, fingerprint, record.verifiedBase, lease);
    } else {
      const cloudStillAtBase = !!latest && cloudFingerprint === record.fingerprint;
      if (revision !== record.revision && !cloudStillAtBase) throw new AccountRequestError('SAVE_CONFLICT', 409);
      if (!product.requiredFiles.some(name => files.some(file => file.name === name)))
        throw new AccountRequestError('MAIN_SAVE_REQUIRED');
      // Compare-and-set at upload is the concurrency guard. A marker says nothing about saved bytes.
      const uploadKey = JSON.stringify([record.owner, record.product]);
      const rejected = rejectedUploads.get(uploadKey);
      if (rejected?.fingerprint === fingerprint) throw rejected.error;
      let saved: CloudSaveSnapshot;
      try {
        saved = await uploadCloudSave(record.product, revision, files, signal);
      } catch (error) {
        if (error instanceof AccountRequestError && error.status === 400)
          rejectedUploads.set(uploadKey, {fingerprint, error});
        throw error;
      }
      rejectedUploads.delete(uploadKey);
      verify();
      revision = saved.revision;
      clean(record, revision, fingerprint, true, lease);
    }
    await completeMarker(record.product, record.owner, data, revision, signal);
  }

  async function flush(): Promise<void> {
    if (Date.now() < retryAfter) return;
    if (task || !enabled || !owner || disposed || active || ports.isRunning() || !validAccount()) return task ?? undefined;
    if (markerTask) return markerTask.then(flush);
    const signal = controller.signal,
      user = owner;
    task = (async () => {
      let pending: RecordState[];
      try {
        pending = records().filter(
          (record) => record.owner === user && record.runId && !record.unclaimed,
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
            active || ports.isRunning()
          )
            continue;
          if (needsReview(record.product)) continue;
          const data = await listCloudSaves(operationSignal);
          check(operationSignal, user, lease);
          await synchronize(current, data, operationSignal, lease);
          check(operationSignal, user, lease);
          publish(null);
        } catch (error) {
          if (!operationSignal.aborted && !isAbortError(error) && !sessionExpired(error, user)) {
            const key = errorKey(error);
            if (error instanceof AccountRequestError && error.status === 429) {
              retryAfter = Date.now() + 60_000;
              publish({product: record.product, error: key});
              break;
            }
            if (key === 'cloudSave.syncConflict') {
              flagReview(record.product);
              warn(record.product, key);
            } else publish({product: record.product, error: key});
          }
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
    if (state.status === 'guest') return { status: state.expired ? 'expired' as const : 'guest' as const, owner: '' };
    return { status: 'unknown' as const, owner: '' };
  };

  function readEnabled(user: string) {
    try {
      return !user || ports.storage.getItem(pausedKey(user)) !== '1';
    } catch {
      return false;
    }
  }
  function interruptSync() {
    controller.abort();
    controller = new AbortController();
  }
  function queueFlush() {
    void Promise.resolve(task).then(() => flush());
  }
  function invalidateSync() {
    launchData = null;
    interruptSync();
    publish(null);
    if (identityStatus === 'member' && enabled) queueFlush();
  }
  function refreshEnabled() {
    const next = readEnabled(owner);
    if (next === enabled) return;
    enabled = next;
    invalidateSync();
  }
  const changeOwner = () => {
    const next = currentAccount();
    if (next.status === identityStatus && next.owner === owner) return;
    if (active && (next.status === 'member' || next.status === 'guest') && active.owner !== next.owner) flagReview(active.product);
    identityStatus = next.status;
    owner = next.owner;
    enabled = readEnabled(owner);
    invalidateSync();
  };
  const unsubscribe = accountSession.subscribe(changeOwner);
  changeOwner();
  const timer = setInterval(() => { void flush(); }, 15_000);
  const online = () => {
    void flush();
  };
  const storageChanged = (event: StorageEvent) => {
    if (owner && (event.key === pausedKey(owner) || event.key === null)) refreshEnabled();
    if (event.key?.startsWith(prefix + 'review:') && event.newValue) interruptSync();
  };
  window.addEventListener('online', online);
  window.addEventListener('storage', storageChanged);

  function warn(product: ProductId, error: UiMessageKey) {
    if (status?.product === product && status.error === error) return;
    publish({product, error});
    ports.notify(error);
  }
  const needsReview = (product: ProductId) => ports.storage.getItem(reviewKey(product)) !== null;
  function flagReview(product: ProductId) {
    try { ports.storage.setItem(reviewKey(product), '1'); }
    catch { warn(product, 'cloudSave.syncPending'); }
  }
  const otherRunIds = (data: CloudSaveList, product: ProductId) =>
    data.pending.filter(item => item.product === product && item.deviceId !== device).map(item => item.runId);
  const approvedKey = (product: ProductId, user: string) => prefix + 'approved:' + user + ':' + product;

  async function trackRun(run: ActiveRun, account: ReturnType<typeof currentAccount>, data: LaunchCloudState | null) {
    const {product} = run;
    const lease = run.lease ?? await acquireProductLock(product);
    if (active !== run || disposed) { lease?.release(); return; }
    if (!lease) {
      flagReview(product);
      publish({product, error: 'cloudSave.sameBrowserActive'});
      return;
    }
    run.lease = lease;
    const previous = readRecord(product);
    const user = account.owner;
    if (previous?.owner && previous.owner !== user) {
      if (account.status !== 'expired') {
        flagReview(product);
        warn(product, 'cloudSave.otherAccountPending');
      }
      return;
    }
    if (account.status === 'member' && (owner !== user || !enabled || !validAccount())) return;
    if (needsReview(product) || (previous?.unclaimed && user)) return;
    const runId = previous?.runId ?? crypto.randomUUID();
    const revision = previous?.revision ?? 0;
    // Record locally before play can change files. Flush compares the committed bytes after exit.
    writeRecord({
      owner: user, product, revision, fingerprint: previous?.fingerprint ?? '',
      runId, unclaimed: account.status !== 'member', verifiedBase: previous?.verifiedBase ?? false,
    }, lease);
    if (!data || !user || data.revision !== revision || data.pending.some(item => item.deviceId === device)) return;
    const signal = controller.signal;
    // Another device may confirm starting from this revision. The upload still checks its version.
    const registration = bounded(requestSignal => accountRequest(
      `v1/me/cloud-saves/${product}/sessions`, {deviceId: device, runId, baseRevision: revision, allowPending: true}, {signal: requestSignal},
    ), signal).then(() => {}, error => {
      if (!signal.aborted) sessionExpired(error, user);
    }).finally(() => {
      if (markerTask === registration) markerTask = null;
    });
    markerTask = registration;
  }

  async function bounded<T>(operation:(signal:AbortSignal)=>Promise<T>,signal:AbortSignal,ms=2500):Promise<T> {
    const deadline=new AbortController(),linked=linkAbortSignals(signal,deadline.signal);
    const timer=setTimeout(()=>deadline.abort(new Error('Cloud check timed out')),ms);
    try{return await waitWithAbort(operation(linked.signal),linked.signal);}
    finally{clearTimeout(timer);linked.dispose();}
  }
  function baselineFor(product: ProductId, user: string) {
    const previous = readRecord(product);
    return previous?.owner === user && !previous.unclaimed ? previous : undefined;
  }

  async function review(product: ProductId, externalSignal: AbortSignal, context: 'launch' | 'manual', reserved?: ProductLock, launch?: LaunchReview): Promise<'continue' | 'reload' | 'cancel'> {
    const preparing = context === 'launch';
    if (!owner || disposed || active || (!preparing && ports.isRunning()) || ports.isPlaying?.() ||
        isMultiplayerProductId(product) || !validAccount() || (preparing && !enabled)) return 'continue';
    const user = owner;
    const lease = reserved ?? await acquireProductLock(product).catch(() => {
      warn(product, 'cloudSave.syncPending');
      return null;
    });
    if (!lease) return 'continue';
    const linked = linkAbortSignals(controller.signal, externalSignal, lease.signal);
    const verify = () => {
      check(linked.signal, user, lease, false);
      if (active || (!preparing && ports.isRunning()) || ports.isPlaying?.())
        throw new DOMException('Cancelled', 'AbortError');
    };
    try {
      verify();
      let data = await bounded(listCloudSaves, linked.signal, launch ? 15_000 : undefined);
      verify();
      while (true) {
        const current = data.products.find(item => item.id === product && !item.multiplayer);
        if (!current) return 'continue';
        const rememberMarkers = () => {
          if (preparing) launchData = {product, user, revision: current.revision, pending: data.pending.filter(item => item.product === product)};
        };
        rememberMarkers();
        const key = approvedKey(product, user);
        const pendingRuns = preparing ? otherRunIds(data, product) : [];
        const approved = new Set(ports.storage.getItem(key)?.split('|'));
        const confirmOverwrite = pendingRuns.some(id => !approved.has(id));
        if (confirmOverwrite) {
          await launch?.beforeDecision();
          verify();
          const proceed = await ports.decideStart('other-device', linked.signal);
          verify();
          if (!proceed) {
            if (launch) {flagReview(product); return 'continue';}
            return 'cancel';
          }
          const checked = await bounded(listCloudSaves, linked.signal);
          verify();
          const confirmed = new Set([...approved, ...pendingRuns]);
          const currentRuns = otherRunIds(checked, product);
          if (checked.products.find(item => item.id === product)?.revision !== current.revision ||
              currentRuns.some(id => !confirmed.has(id))) {
            data = checked;
            continue;
          }
          data = checked;
          rememberMarkers();
        }
        const latest = data.saves.find(item => item.product === product && item.revision === current.revision) ?? null;
        const read = launch ? ports.readForLaunch ?? ports.read : ports.read;
        const files = await bounded(() => read(product, current.files), linked.signal);
        verify();
        const fingerprint = await localSaveFingerprint(files);
        verify();
        const previous = readRecord(product);
        const base = previous?.owner === user && !previous.unclaimed ? previous : undefined;
        const relation = relateSaveVersions(fingerprint, latest, current.revision, base);
        const unowned = !!previous && !base;
        const quarantined = needsReview(product);
        const deleted = !latest && current.revision > (base?.revision ?? 0);
        let choice: 'local' | 'cloud' | 'cancel' = 'local';
        if (confirmOverwrite) {
          choice = !files.length && latest ? 'cloud' : 'local';
        } else if (!preparing || (relation !== 'same' && relation !== 'cloud-ahead' &&
          (quarantined || unowned || deleted || (!!latest && relation !== 'browser-ahead')))) {
          const comparison = await compareSaveVersions(product, files, latest, relation, linked.signal);
          verify();
          await launch?.beforeDecision();
          verify();
          choice = await ports.decide(latest ? 'conflict' : 'claim', product, linked.signal, comparison, context);
          verify();
          if (choice === 'cancel') {
            if (preparing) flagReview(product);
            return preparing ? 'continue' : 'cancel';
          }
          const checked = await bounded(listCloudSaves, linked.signal);
          verify();
          if (checked.products.find(item => item.id === product)?.revision !== current.revision) {
            data = checked;
            continue;
          }
          data = checked;
          rememberMarkers();
        } else if (relation === 'cloud-ahead') {
          choice = 'cloud';
        }
        const restoring = choice === 'cloud' && latest !== null && relation !== 'same';
        if (restoring) {
          // Let the archive writer finish rolling back before continuing with local files.
          const deadline = new AbortController();
          const restoreSignal = linkAbortSignals(linked.signal, deadline.signal);
          const timer = setTimeout(() => deadline.abort(new Error('Cloud restore timed out')), launch ? 30_000 : 4000);
          try {
            await ports.restore(product, latest, restoreSignal.signal, current.files, preparing, launch?.beforeRestore);
          } catch (error) {
            if (linked.signal.aborted) throw error;
            if (preparing) {
              launchData = null;
              flagReview(product);
            }
            if (!sessionExpired(error, user)) warn(product, 'cloudSave.restoreFailed');
            return 'continue';
          } finally {
            clearTimeout(timer);
            restoreSignal.dispose();
          }
          verify();
        }
        let revision = current.revision;
        let baseline = latest ? manifestFingerprint(latest.files) : '';
        if (!preparing && choice === 'local' && relation !== 'same') {
          const saved = await uploadCloudSave(product, revision, files, linked.signal);
          verify();
          revision = saved.revision;
          baseline = manifestFingerprint(saved.files);
        }
        writeRecord({
          owner: user, product, revision, fingerprint: baseline, unclaimed: false, verifiedBase: true,
          runId: preparing && !restoring && fingerprint !== baseline ? base?.runId ?? null : null,
        }, lease);
        ports.storage.removeItem(reviewKey(product));
        if (confirmOverwrite) ports.storage.setItem(key, otherRunIds(data, product).join('|'));
        publish(null);
        window.dispatchEvent(new Event('cloud-save-change'));
        if (!preparing) await completeMarker(product, user, data, revision, linked.signal);
        return preparing && restoring ? 'reload' : 'continue';
      }
    } catch (error) {
      if (preparing) {
        launchData = null;
        if (launch && !externalSignal.aborted) flagReview(product);
      }
      if (externalSignal.aborted) throw error;
      if (!linked.signal.aborted && !isAbortError(error) && !sessionExpired(error, user))
        warn(product, errorKey(error));
      return 'continue';
    } finally {
      linked.dispose();
      if (!reserved) lease.release();
    }
  }

  function releaseLaunchLease() {
    launchLease?.lease.release();
    launchLease = null;
  }

  async function beforeStart(product: ProductId, signal: AbortSignal, launch?: LaunchReview): Promise<'continue' | 'reload' | 'cancel'> {
    if (disposed || active || ports.isPlaying?.() || isMultiplayerProductId(product)) return 'continue';
    releaseLaunchLease();
    let lease: ProductLock | null;
    try {
      lease = await acquireProductLock(product);
    } catch {
      warn(product, 'cloudSave.syncPending');
      return 'continue';
    }
    const occupied = lease === null;
    if (occupied && handoff && !signal.aborted && !launch) {
      handoff.postMessage(product);
      const deadline = Date.now() + 5000;
      while (!lease && !disposed && Date.now() < deadline) {
        await waitWithAbort(new Promise(resolve => setTimeout(resolve, 100)), signal);
        lease = await acquireProductLock(product).catch(() => null);
      }
    }
    if (signal.aborted || disposed) {
      lease?.release();
      return 'cancel';
    }
    if (!lease) {
      await launch?.beforeDecision();
      await ports.decideStart('tab-unavailable', signal);
      return 'cancel';
    }
    launchLease = {product, lease};
    let accepted = false;
    try {
      // The retiring tab commits its IDBFS before releasing the product lock.
      // Discard this tab's preloaded view so all reads use those committed files.
      if (occupied && !await ports.retirePrepared(product)) return 'cancel';
      if (signal.aborted) return 'cancel';
      const result = await review(product, signal, 'launch', lease, launch);
      accepted = result !== 'cancel';
      return occupied && result === 'continue' ? 'reload' : result;
    } finally {
      if (!accepted) releaseLaunchLease();
    }
  }

  async function resolve(product:ProductId,signal:AbortSignal){await review(product,signal,'manual');}

  return {
    prepareLocalLaunch() { launchData = null; releaseLaunchLease(); interruptSync(); },
    deferLaunch(product: ProductId) {
      launchData = null;
      flagReview(product);
    },
    beforeStart,
    started(product: ProductId) {
      if (disposed || active || isMultiplayerProductId(product)) return;
      validAccount();
      const account = currentAccount();
      const lease = launchLease?.product === product ? launchLease.lease : undefined;
      if (lease) launchLease = null;
      const run: ActiveRun = {product, owner: account.owner, lease};
      active = run;
      const data = launchData?.product === product && launchData.user === account.owner ? launchData : null;
      launchData = null;
      void trackRun(run, account, data).catch(error => {
        if (active === run && !isAbortError(error)) warn(product, errorKey(error));
      });
    },
    flush,
    resolve,
    finished(handedOff = false) {
      launchData = null;
      releaseLaunchLease();
      if (active) {
        const {lease} = active;
        active = null;
        lease?.release();
      }
      if (!handedOff) queueFlush();
    },
    async versionRelation(product: ProductId, files: File[], latest: CloudSaveSnapshot | null, revision: number) {
      const fingerprint = await localSaveFingerprint(files);
      const base = owner && validAccount() ? baselineFor(product, owner) : undefined;
      return relateSaveVersions(fingerprint, latest, revision, base);
    },
    subscribe(listener: () => void) {
      listeners.add(listener);
      return () => {
        listeners.delete(listener);
      };
    },
    getSnapshot: () => status,
    getEnabled: () => identityStatus === 'member' ? enabled : null,
    setEnabled(value: boolean) {
      if (disposed || identityStatus !== 'member' || value === enabled) return;
      if (value) ports.storage.removeItem(pausedKey(owner));
      else ports.storage.setItem(pausedKey(owner), '1');
      refreshEnabled();
    },
    hasLocalPending(product: ProductId) {
      if (identityStatus !== 'member') return false;
      try {
        const record = readRecord(product);
        return needsReview(product) || !!record?.runId && (record.owner === owner || record.unclaimed);
      } catch {
        return false;
      }
    },
    dispose() {
      disposed = true;
      controller.abort();
      releaseLaunchLease();
      handoff?.close();
      unsubscribe();
      clearInterval(timer);
      window.removeEventListener('online', online);
      window.removeEventListener('storage', storageChanged);
      active?.lease?.release();
      active = null;
      listeners.clear();
    },
  };
}
export type CloudSaveSync = ReturnType<typeof createCloudSaveSync>;
