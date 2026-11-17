export const LESS_MOTION_STORAGE_KEY = 'eagler-touhou-less-motion-v1';
const QUICK_CHAT_VOLUME_STORAGE_KEY = 'eagler-touhou-quick-chat-volume-v1';
const normalizeVolume = (value: number) => Number.isFinite(value) ? Math.max(0, Math.min(100, Math.round(value))) : 100;
const readVolume = (value: string | null) => value === null ? 100 : normalizeVolume(Number(value));
export type SitePreferenceStorage = Pick<Storage, 'getItem' | 'setItem'>;

export interface SitePreferencesSnapshot {
  readonly lessMotion: boolean;
  readonly quickChatVolume: number;
}
export interface SitePreferencesModel {
  subscribe(listener: () => void): () => void;
  getSnapshot(): SitePreferencesSnapshot;
  setLessMotion(enabled: boolean): void;
  toggleMotion(): void;
  setQuickChatVolume(value: number): void;
  syncStorageEvent(event: Pick<StorageEvent, 'key' | 'newValue'>): void;
}

/** Document-lived preference authority shared by the launcher and lobby.
 * Keys, exact-value parsing and fallbacks come from main app.mts/lobby.mts.
 * Storage is supplied by the real host; a denied storage API is non-fatal.
 */
export function createSitePreferencesModel({storage}: {
  storage: SitePreferenceStorage | null;
}): SitePreferencesModel {
  const listeners = new Set<() => void>();
  const read = (key: string) => {try {return storage?.getItem(key) ?? null;} catch {return null;}};
  const write = (key: string, value: string) => {try {storage?.setItem(key, value);} catch {}};
  let snapshot: SitePreferencesSnapshot = Object.freeze({
    lessMotion: read(LESS_MOTION_STORAGE_KEY) === '1',
    quickChatVolume: readVolume(read(QUICK_CHAT_VOLUME_STORAGE_KEY)),
  });
  function publish(change: Partial<SitePreferencesSnapshot>) {
    const next = {...snapshot, ...change};
    if (Object.keys(next).every(key => next[key as keyof SitePreferencesSnapshot] === snapshot[key as keyof SitePreferencesSnapshot])) return;
    snapshot = Object.freeze(next);
    for (const listener of listeners) listener();
  }
  function setLessMotion(enabled: boolean) {
    write(LESS_MOTION_STORAGE_KEY, enabled ? '1' : '0');
    publish({lessMotion: enabled});
  }
  return Object.freeze({
    subscribe(listener: () => void) {listeners.add(listener); return () => {listeners.delete(listener);};},
    getSnapshot: () => snapshot,
    setLessMotion,
    toggleMotion: () => setLessMotion(!snapshot.lessMotion),
    setQuickChatVolume(value: number) {
      const quickChatVolume = normalizeVolume(value);
      if (quickChatVolume === snapshot.quickChatVolume) return;
      write(QUICK_CHAT_VOLUME_STORAGE_KEY, String(quickChatVolume));
      publish({quickChatVolume});
    },
    syncStorageEvent(event: Pick<StorageEvent, 'key' | 'newValue'>) {
      // Motion remains document-shared; RuntimeDiagnostics owns its own key.
      if (event.key === LESS_MOTION_STORAGE_KEY) publish({lessMotion: event.newValue === '1'});
      if (event.key === QUICK_CHAT_VOLUME_STORAGE_KEY) publish({quickChatVolume: readVolume(event.newValue)});
    },
  });
}

/** Install once per document in a host effect; return its teardown. No history,
 * menu mutations, or runtime ownership is introduced by this binding.
 */
export function bindSitePreferencesToDocument(model: SitePreferencesModel, {
  documentObj = document, windowObj = window,
}: {documentObj?: Document; windowObj?: Window} = {}): () => void {
  const render = () => {
    const {lessMotion} = model.getSnapshot();
    documentObj.body.classList.toggle('less-motion', lessMotion);
    if (lessMotion) documentObj.querySelectorAll<HTMLElement>('.game').forEach(card => {
      card.style.setProperty('--rx', '0deg');
      card.style.setProperty('--ry', '0deg');
    });
  };
  const storage = (event: StorageEvent) => model.syncStorageEvent(event);
  render();
  const unsubscribe = model.subscribe(render);
  windowObj.addEventListener('storage', storage);
  return () => {unsubscribe(); windowObj.removeEventListener('storage', storage);};
}
