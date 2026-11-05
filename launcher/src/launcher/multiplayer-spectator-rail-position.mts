export const multiplayerSpectatorRailPositionStorageKey = "eagler-touhou-mp-spectator-rail-position-v1";
export const multiplayerSpectatorRailLegacyPositionStorageKey = "eagler.mpSpectatorRail.mobilePosition.v1";

export interface SpectatorRailPositionStorage {
  getItem(key: string): string | null;
  setItem(key: string, value: string): void;
  removeItem(key: string): void;
}

export interface SpectatorRailPosition {
  x: number;
  y: number;
}

export interface SpectatorRailPositionStore {
  load(): SpectatorRailPosition | null;
  save(position: SpectatorRailPosition): void;
}

function browserLocalStorage(): SpectatorRailPositionStorage | null {
  try { return globalThis.localStorage as SpectatorRailPositionStorage; }
  catch { return null; }
}

function parsePosition(raw: string | null): SpectatorRailPosition | null {
  if (raw == null) return null;
  try {
    const value = JSON.parse(raw);
    if (!value || !Number.isFinite(value.x) || !Number.isFinite(value.y)) return null;
    return { x: value.x, y: value.y };
  } catch {
    return null;
  }
}

export function createMultiplayerSpectatorRailPositionStore({
  storage = browserLocalStorage(),
}: { storage?: SpectatorRailPositionStorage | null } = {}): SpectatorRailPositionStore {
  const load = (): SpectatorRailPosition | null => {
    if (!storage) return null;
    let raw: string | null = null;
    try { raw = storage.getItem(multiplayerSpectatorRailPositionStorageKey); }
    catch { return null; }

    if (raw == null) {
      let legacy: string | null = null;
      try { legacy = storage.getItem(multiplayerSpectatorRailLegacyPositionStorageKey); }
      catch { return null; }
      if (legacy != null) {
        try {
          storage.setItem(multiplayerSpectatorRailPositionStorageKey, legacy);
          storage.removeItem(multiplayerSpectatorRailLegacyPositionStorageKey);
          raw = legacy;
        } catch {
          return parsePosition(legacy);
        }
      }
    }
    return parsePosition(raw);
  };

  const save = ({ x, y }: SpectatorRailPosition): void => {
    if (!Number.isFinite(x) || !Number.isFinite(y)) return;
    try {
      storage?.setItem(multiplayerSpectatorRailPositionStorageKey, JSON.stringify({
        x: Math.round(x),
        y: Math.round(y),
      }));
    } catch {}
  };

  return Object.freeze({ load, save });
}
