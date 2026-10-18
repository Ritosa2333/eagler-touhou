export const multiplayerShareSettingsStorageKey = (product: string): string =>
  `eagler-touhou-${product}-share-singleplayer-settings-v1`;
export const multiplayerLoadoutStorageKey = (product: string): string =>
  `eagler-touhou-${product}-loadout-v1`;

export interface MultiplayerPreferenceStorage {
  getItem(key: string): string | null;
  setItem(key: string, value: string): void;
}

export interface MultiplayerProductPreferences {
  shareSingleplayerSettings: boolean;
  preferredLoadout: number | null;
}

export interface MultiplayerPreferenceStore {
  load(input: { product: string; multiplayer: boolean; maxLoadout: number }): MultiplayerProductPreferences;
  persistShareSingleplayerSettings(product: string, enabled: boolean): void;
  persistPreferredLoadout(product: string, loadout: number): void;
}

function browserLocalStorage(): MultiplayerPreferenceStorage | null {
  try { return globalThis.localStorage as MultiplayerPreferenceStorage; }
  catch { return null; }
}

export function normalizePreferredLoadout(value: unknown, maxLoadout: number): number {
  const parsed = Number(value);
  return Number.isInteger(parsed) && parsed >= 0 && parsed < Math.max(0, Math.trunc(maxLoadout)) ? parsed : 0;
}

export function createMultiplayerPreferenceStore({
  storage = browserLocalStorage(),
}: { storage?: MultiplayerPreferenceStorage | null } = {}): MultiplayerPreferenceStore {
  const load = ({ product, multiplayer, maxLoadout }: {
    product: string;
    multiplayer: boolean;
    maxLoadout: number;
  }): MultiplayerProductPreferences => {
    if (!multiplayer) return { shareSingleplayerSettings: true, preferredLoadout: null };
    let shareSingleplayerSettings = true;
    let savedLoadout: unknown = null;
    try { shareSingleplayerSettings = storage?.getItem(multiplayerShareSettingsStorageKey(product)) !== "0"; }
    catch { shareSingleplayerSettings = true; }
    try { savedLoadout = storage?.getItem(multiplayerLoadoutStorageKey(product)); } catch {}
    return {
      shareSingleplayerSettings,
      preferredLoadout: normalizePreferredLoadout(savedLoadout, maxLoadout),
    };
  };

  const persistShareSingleplayerSettings = (product: string, enabled: boolean): void => {
    try { storage?.setItem(multiplayerShareSettingsStorageKey(product), enabled ? "1" : "0"); } catch {}
  };

  const persistPreferredLoadout = (product: string, loadout: number): void => {
    if (!Number.isInteger(loadout) || loadout < 0) return;
    try { storage?.setItem(multiplayerLoadoutStorageKey(product), String(loadout)); } catch {}
  };

  return Object.freeze({ load, persistShareSingleplayerSettings, persistPreferredLoadout });
}
