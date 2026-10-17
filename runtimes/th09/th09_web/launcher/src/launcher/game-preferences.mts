export type TouchMovementMode = "touch" | "touch-unlimited" | "joystick" | "joystick-free";
export type TouchFocusMode = "two-finger" | "hold-button" | "toggle-button";
export type MusicMode = "ogg-stream" | "ogg-full" | "midi" | "none";

export interface GameOptions {
  thpracEnabled: boolean;
  thpracTouchControlsEnabled: boolean;
  magnifierEnabled: boolean;
  th06FocusHitbox: boolean;
  frameLimit60Enabled: boolean;
  touchEnabled: boolean;
  touchMovementMode: TouchMovementMode;
  touchSensitivity: number;
  touchFocusMode: TouchFocusMode;
  doubleTapBombEnabled: boolean;
  alwaysHitbox: boolean;
  enhanceLocalPlayerVisibility: boolean;
}

export const DEFAULT_GAME_OPTIONS: Readonly<GameOptions> = Object.freeze({
  thpracEnabled: false,
  thpracTouchControlsEnabled: false,
  magnifierEnabled: false,
  th06FocusHitbox: false,
  frameLimit60Enabled: false,
  touchEnabled: false,
  touchMovementMode: "touch",
  touchSensitivity: 100,
  touchFocusMode: "hold-button",
  doubleTapBombEnabled: false,
  alwaysHitbox: false,
  enhanceLocalPlayerVisibility: false,
});

export const TOUCH_MOVEMENT_MODES = new Set<TouchMovementMode>([
  "touch", "touch-unlimited", "joystick", "joystick-free",
]);
export const TOUCH_FOCUS_MODES = new Set<TouchFocusMode>([
  "two-finger", "hold-button", "toggle-button",
]);
export const MUSIC_MODES = new Set<MusicMode>(["ogg-stream", "ogg-full", "midi", "none"]);

export function isMusicMode(value: string): value is MusicMode {
  return value === "ogg-stream" || value === "ogg-full" || value === "midi" || value === "none";
}

export function isTouchMovementMode(value: string): value is TouchMovementMode {
  return value === "touch" || value === "touch-unlimited" || value === "joystick" || value === "joystick-free";
}

export function isTouchFocusMode(value: string): value is TouchFocusMode {
  return value === "two-finger" || value === "hold-button" || value === "toggle-button";
}

export const gamePreferenceStorageKey = (preferenceId: string): string =>
  `eagler-touhou-game-options-v1-${preferenceId}`;
export const languagePreferenceStorageKey = (preferenceId: string): string =>
  `eagler-touhou-language-v1-${preferenceId}`;

export interface GamePreferenceStorage {
  getItem(key: string): string | null;
  setItem(key: string, value: string): void;
}

export function touchMovementUsesJoystick(mode: unknown): boolean {
  return mode === "joystick" || mode === "joystick-free";
}

type StoredRecord = Record<string, unknown>;

function record(value: unknown): StoredRecord | null {
  return value !== null && typeof value === "object" && !Array.isArray(value)
    ? value as StoredRecord
    : null;
}

function booleanOption(source: StoredRecord | null, name: keyof GameOptions, fallback: boolean): boolean {
  const value = source?.[name];
  return typeof value === "boolean" ? value : fallback;
}

function normalizeMusicMode(value: unknown): MusicMode {
  const legacy = value === "ogg" || value === "wav" ? "ogg-stream" : value;
  return typeof legacy === "string" && MUSIC_MODES.has(legacy as MusicMode)
    ? legacy as MusicMode
    : "ogg-stream";
}

export interface NormalizeGamePreferencesContext {
  thpracAvailable: boolean;
  webAudioAvailable: boolean;
}

export interface NormalizedGamePreferences {
  options: GameOptions;
  musicPreferenceExplicit: boolean;
  musicPreference: MusicMode;
  music: MusicMode;
  sanitizedRecord: StoredRecord | null;
  storageRewriteRequired: boolean;
}

export function normalizeStoredGamePreferences(
  savedValue: unknown,
  context: NormalizeGamePreferencesContext,
): NormalizedGamePreferences {
  const saved = record(savedValue);
  const savedOptions = record(saved?.options);

  let sanitizedRecord = saved;
  let storageRewriteRequired = false;
  if (saved && savedOptions && Object.prototype.hasOwnProperty.call(savedOptions, "limitPresentationTo60")) {
    const sanitizedOptions = { ...savedOptions };
    delete sanitizedOptions.limitPresentationTo60;
    sanitizedRecord = { ...saved, options: sanitizedOptions };
    storageRewriteRequired = true;
  }

  const rawOptions = record(sanitizedRecord?.options);
  const movementCandidate = rawOptions?.touchMovementMode;
  const migratedMovement: TouchMovementMode = typeof movementCandidate === "string" &&
    TOUCH_MOVEMENT_MODES.has(movementCandidate as TouchMovementMode)
    ? movementCandidate as TouchMovementMode
    : rawOptions?.unlimitedTouch === true ? "touch-unlimited" : "touch";

  const focusCandidate = rawOptions?.touchFocusMode;
  let focusMode: TouchFocusMode = typeof focusCandidate === "string" &&
    TOUCH_FOCUS_MODES.has(focusCandidate as TouchFocusMode)
    ? focusCandidate as TouchFocusMode
    : "hold-button";
  if (touchMovementUsesJoystick(migratedMovement) && focusMode === "two-finger") focusMode = "hold-button";

  const sensitivityCandidate = rawOptions?.touchSensitivity;
  const touchSensitivity = typeof sensitivityCandidate === "number" && Number.isFinite(sensitivityCandidate)
    ? Math.min(300, Math.max(50, Math.round(sensitivityCandidate)))
    : DEFAULT_GAME_OPTIONS.touchSensitivity;

  const options: GameOptions = {
    thpracEnabled: context.thpracAvailable && booleanOption(rawOptions, "thpracEnabled", DEFAULT_GAME_OPTIONS.thpracEnabled),
    thpracTouchControlsEnabled: booleanOption(rawOptions, "thpracTouchControlsEnabled", DEFAULT_GAME_OPTIONS.thpracTouchControlsEnabled),
    magnifierEnabled: booleanOption(rawOptions, "magnifierEnabled", DEFAULT_GAME_OPTIONS.magnifierEnabled),
    th06FocusHitbox: booleanOption(rawOptions, "th06FocusHitbox", DEFAULT_GAME_OPTIONS.th06FocusHitbox),
    frameLimit60Enabled: booleanOption(rawOptions, "frameLimit60Enabled", DEFAULT_GAME_OPTIONS.frameLimit60Enabled),
    touchEnabled: booleanOption(rawOptions, "touchEnabled", DEFAULT_GAME_OPTIONS.touchEnabled),
    touchMovementMode: migratedMovement,
    touchSensitivity,
    touchFocusMode: focusMode,
    doubleTapBombEnabled: booleanOption(rawOptions, "doubleTapBombEnabled", DEFAULT_GAME_OPTIONS.doubleTapBombEnabled),
    alwaysHitbox: booleanOption(rawOptions, "alwaysHitbox", DEFAULT_GAME_OPTIONS.alwaysHitbox),
    enhanceLocalPlayerVisibility: booleanOption(rawOptions, "enhanceLocalPlayerVisibility", DEFAULT_GAME_OPTIONS.enhanceLocalPlayerVisibility),
  };

  const musicPreference = normalizeMusicMode(sanitizedRecord?.music);
  const musicPreferenceExplicit = sanitizedRecord?.musicPreferenceExplicit === true;
  return Object.freeze({
    options,
    musicPreferenceExplicit,
    musicPreference,
    music: context.webAudioAvailable ? musicPreference : "none",
    sanitizedRecord,
    storageRewriteRequired,
  });
}

export interface SerializeGamePreferencesInput {
  options: GameOptions;
  music: MusicMode;
  musicPreference: MusicMode;
  musicPreferenceExplicit: boolean;
}

export function serializeGamePreferences(input: SerializeGamePreferencesInput): StoredRecord {
  return {
    music: input.musicPreferenceExplicit ? input.musicPreference : input.music,
    musicPreferenceExplicit: input.musicPreferenceExplicit,
    options: Object.fromEntries(Object.keys(DEFAULT_GAME_OPTIONS).map(name => [
      name,
      input.options[name as keyof GameOptions],
    ])),
  };
}

function readStoredJson(storage: GamePreferenceStorage | null, key: string): unknown {
  if (!storage) return null;
  try { return JSON.parse(storage.getItem(key) || "null"); }
  catch { return null; }
}

function readStoredString(storage: GamePreferenceStorage | null, key: string): string | null {
  if (!storage) return null;
  try { return storage.getItem(key); }
  catch { return null; }
}

export function loadStoredGamePreferences({
  storage,
  preferenceId,
  fallbackPreferenceId = null,
  context,
}: {
  storage: GamePreferenceStorage | null;
  preferenceId: string;
  fallbackPreferenceId?: string | null;
  context: NormalizeGamePreferencesContext;
}): NormalizedGamePreferences {
  let saved = readStoredJson(storage, gamePreferenceStorageKey(preferenceId));
  if (!saved && fallbackPreferenceId) {
    saved = readStoredJson(storage, gamePreferenceStorageKey(fallbackPreferenceId));
  }
  const normalized = normalizeStoredGamePreferences(saved, context);
  if (normalized.storageRewriteRequired && storage) {
    try {
      storage.setItem(gamePreferenceStorageKey(preferenceId), JSON.stringify(normalized.sanitizedRecord));
    } catch {}
  }
  return normalized;
}

export function loadStoredLanguagePreference({
  storage,
  preferenceId,
  fallbackPreferenceId = null,
}: {
  storage: GamePreferenceStorage | null;
  preferenceId: string;
  fallbackPreferenceId?: string | null;
}): string | null {
  let saved = readStoredString(storage, languagePreferenceStorageKey(preferenceId));
  if (!saved && fallbackPreferenceId) {
    saved = readStoredString(storage, languagePreferenceStorageKey(fallbackPreferenceId));
  }
  return saved;
}

export function persistStoredGamePreferences({
  storage,
  preferenceId,
  preferences,
  language,
}: {
  storage: GamePreferenceStorage | null;
  preferenceId: string;
  preferences: SerializeGamePreferencesInput;
  language: string;
}): void {
  if (!storage) return;
  try {
    storage.setItem(gamePreferenceStorageKey(preferenceId), JSON.stringify(serializeGamePreferences(preferences)));
  } catch {}
  try { storage.setItem(languagePreferenceStorageKey(preferenceId), language); } catch {}
}
