import {clearRoomUrlParams} from './route-state.mjs';
import {ROOM_INVITE_KEY} from './room-invite.mjs';

/** /beta/ publishes program files for the shared Launcher document. */
export const BETA_STORAGE_PREFIX = 'beta:';
export const BETA_OPT_IN_KEY = 'eagler-touhou-beta-opt-in-v1';

// The stable launcher may predate games introduced by the beta publication.
export function isBetaPublication(value: unknown): boolean {
  const site = value as {schema?: unknown; protocol?: unknown; shared?: {testBuild?: unknown; saveChannel?: unknown}} | null;
  return site?.schema === 'eagler-touhou/site-manifest/1' &&
    site.protocol === 'eagler-touhou/1' &&
    site.shared?.testBuild === true && site.shared.saveChannel === 'beta';
}

export function betaModeEnabled(owner: Pick<Window, 'localStorage'>): boolean {
  try { return owner.localStorage.getItem(BETA_OPT_IN_KEY) === '1'; }
  catch { return false; }
}

export function betaProgramBase(launcherBase: string): string {
  return new URL('beta/', launcherBase).href;
}

export function reloadReleaseMode(owner: Pick<Window, 'localStorage' | 'location' | 'history'>, beta: boolean): void {
  if (beta) owner.localStorage.setItem(BETA_OPT_IN_KEY, '1');
  else owner.localStorage.removeItem(BETA_OPT_IN_KEY);
  const address = new URL(owner.location.href);
  const originalQuery = address.searchParams.toString();
  clearRoomUrlParams(address);
  address.searchParams.delete(ROOM_INVITE_KEY);
  // A room link belongs to its original lobby. Ordinary launcher URLs stay intact.
  if (address.searchParams.toString() !== originalQuery) owner.history.replaceState(null, '', address);
  owner.location.reload();
}
export function channelSessionStorage(storage: Storage | null, beta: boolean): Storage | null {
  if (!storage || !beta) return storage;
  const owner = storage;
  const prefix = BETA_STORAGE_PREFIX;
  return {
    get length() {
      return keys().length;
    },
    key(index) {
      return keys()[index] ?? null;
    },
    getItem(key) {
      return storage.getItem(prefix + key);
    },
    setItem(key, value) {
      storage.setItem(prefix + key, value);
    },
    removeItem(key) {
      storage.removeItem(prefix + key);
    },
    clear() {
      for (const key of keys()) storage.removeItem(prefix + key);
    },
  };
  function keys() {
    return Array.from({ length: owner.length }, (_, i) => owner.key(i))
      .filter((key): key is string => key?.startsWith(prefix) === true)
      .map((key) => key.slice(prefix.length));
  }
}
export function betaModeForBase(baseUrl: string): boolean {
  return new URL(baseUrl, 'https://launcher.invalid/').pathname.replace(/\/$/, '').endsWith('/beta');
}
