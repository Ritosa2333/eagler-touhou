import {
  touchLayoutOrientations,

} from "./touch-layout-model.mjs";

export const touchLayoutWindowPositionsStorageKey = "eagler-touhou-touch-layout-window-positions-v1";
export const touchLayoutWindowPositionsVersion = 1;
export const touchLayoutWindowKinds = Object.freeze(["editor", "settings"]         );

function isRecord(value         )                                   {
  return !!value && typeof value === "object";
}

function normalizePoint(value         )                                {
  if (!isRecord(value) || typeof value.x !== "number" || !Number.isFinite(value.x) ||
      typeof value.y !== "number" || !Number.isFinite(value.y)) return null;
  return {
    x: Math.max(0, Math.min(1, value.x)),
    y: Math.max(0, Math.min(1, value.y)),
  };
}

export function emptyTouchLayoutWindowPositions()                             {
  return {
    version: touchLayoutWindowPositionsVersion,
    profiles: {
      landscape: { editor: null, settings: null },
      portrait: { editor: null, settings: null },
    },
  };
}

export function normalizeTouchLayoutWindowPositions(value         )                             {
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

function clonePositions(value                            )                             {
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

function loadPositions(storage                                 )                             {
  if (!storage) return emptyTouchLayoutWindowPositions();
  try {
    return normalizeTouchLayoutWindowPositions(JSON.parse(storage.getItem(touchLayoutWindowPositionsStorageKey) || "null"));
  } catch {
    return emptyTouchLayoutWindowPositions();
  }
}

export function createTouchLayoutWindowPositionStore({
  storage,
}

 ) {
  let positions = loadPositions(storage);

  function persist()       {
    if (!storage) return;
    try { storage.setItem(touchLayoutWindowPositionsStorageKey, JSON.stringify(positions)); } catch {}
  }

  function reload()                             {
    positions = loadPositions(storage);
    return clonePositions(positions);
  }

  function get(orientation                        , kind                       )                                {
    const point = positions.profiles[orientation][kind];
    return point ? { ...point } : null;
  }

  function set(
    orientation                        ,
    kind                       ,
    point                        ,
  )          {
    const normalized = normalizePoint(point);
    if (!normalized) return false;
    positions.profiles[orientation][kind] = normalized;
    persist();
    return true;
  }

  function snapshot()                             {
    return clonePositions(positions);
  }

  return Object.freeze({ reload, get, set, snapshot });
}
