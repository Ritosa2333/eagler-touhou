import {
  touchLayoutOrientations,
  type TouchLayoutOrientation,
} from "./touch-layout-model.mjs";

export const touchLayoutWindowPositionsStorageKey = "eagler-touhou-touch-layout-window-positions-v1";
export const touchLayoutWindowPositionsVersion = 1;
export const touchLayoutWindowKinds = Object.freeze(["editor", "settings"] as const);

export type TouchLayoutWindowKind = (typeof touchLayoutWindowKinds)[number];

export interface TouchLayoutWindowPoint {
  x: number;
  y: number;
}

export interface TouchLayoutWindowPositions {
  version: typeof touchLayoutWindowPositionsVersion;
  profiles: Record<TouchLayoutOrientation, Record<TouchLayoutWindowKind, TouchLayoutWindowPoint | null>>;
}

export interface TouchLayoutWindowStorage {
  getItem(key: string): string | null;
  setItem(key: string, value: string): void;
}

function isRecord(value: unknown): value is Record<string, unknown> {
  return !!value && typeof value === "object";
}

function normalizePoint(value: unknown): TouchLayoutWindowPoint | null {
  if (!isRecord(value) || typeof value.x !== "number" || !Number.isFinite(value.x) ||
      typeof value.y !== "number" || !Number.isFinite(value.y)) return null;
  return {
    x: Math.max(0, Math.min(1, value.x)),
    y: Math.max(0, Math.min(1, value.y)),
  };
}

export function emptyTouchLayoutWindowPositions(): TouchLayoutWindowPositions {
  return {
    version: touchLayoutWindowPositionsVersion,
    profiles: {
      landscape: { editor: null, settings: null },
      portrait: { editor: null, settings: null },
    },
  };
}

export function normalizeTouchLayoutWindowPositions(value: unknown): TouchLayoutWindowPositions {
  if (!isRecord(value) || value.version !== touchLayoutWindowPositionsVersion || !isRecord(value.profiles)) {
    return emptyTouchLayoutWindowPositions();
  }
  const normalized = emptyTouchLayoutWindowPositions();
  for (const orientation of touchLayoutOrientations) {
    const source = value.profiles[orientation];
    if (!isRecord(source)) continue;
    for (const kind of touchLayoutWindowKinds) {
      normalized.profiles[orientation][kind] = normalizePoint(source[kind]);
    }
  }
  return normalized;
}

function clonePositions(value: TouchLayoutWindowPositions): TouchLayoutWindowPositions {
  return {
    version: touchLayoutWindowPositionsVersion,
    profiles: {
      landscape: {
        editor: value.profiles.landscape.editor ? { ...value.profiles.landscape.editor } : null,
        settings: value.profiles.landscape.settings ? { ...value.profiles.landscape.settings } : null,
      },
      portrait: {
        editor: value.profiles.portrait.editor ? { ...value.profiles.portrait.editor } : null,
        settings: value.profiles.portrait.settings ? { ...value.profiles.portrait.settings } : null,
      },
    },
  };
}

function loadPositions(storage: TouchLayoutWindowStorage | null): TouchLayoutWindowPositions {
  if (!storage) return emptyTouchLayoutWindowPositions();
  try {
    return normalizeTouchLayoutWindowPositions(JSON.parse(storage.getItem(touchLayoutWindowPositionsStorageKey) || "null"));
  } catch {
    return emptyTouchLayoutWindowPositions();
  }
}

export function createTouchLayoutWindowPositionStore({
  storage,
}: {
  storage: TouchLayoutWindowStorage | null;
}) {
  let positions = loadPositions(storage);

  function persist(): void {
    if (!storage) return;
    try { storage.setItem(touchLayoutWindowPositionsStorageKey, JSON.stringify(positions)); } catch {}
  }

  function reload(): TouchLayoutWindowPositions {
    positions = loadPositions(storage);
    return clonePositions(positions);
  }

  function get(orientation: TouchLayoutOrientation, kind: TouchLayoutWindowKind): TouchLayoutWindowPoint | null {
    const point = positions.profiles[orientation][kind];
    return point ? { ...point } : null;
  }

  function set(
    orientation: TouchLayoutOrientation,
    kind: TouchLayoutWindowKind,
    point: TouchLayoutWindowPoint,
  ): boolean {
    const normalized = normalizePoint(point);
    if (!normalized) return false;
    positions.profiles[orientation][kind] = normalized;
    persist();
    return true;
  }

  function snapshot(): TouchLayoutWindowPositions {
    return clonePositions(positions);
  }

  return Object.freeze({ reload, get, set, snapshot });
}
