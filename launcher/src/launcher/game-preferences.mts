import { TOUCH_SENSITIVITY_MAX, TOUCH_SENSITIVITY_MIN } from "../contracts/runtime-protocol.mjs";

export type TouchMovementMode = "touch" | "touch-unlimited" | "joystick" | "joystick-free";
export type TouchFocusMode = "two-finger" | "hold-button" | "toggle-button";
export type MusicMode = "ogg-stream" | "ogg-full" | "ogg-akyu" | "midi" | "none";

export type {Th05MultiplayerDisplay} from "../contracts/runtime-protocol.mts";
import type {Th05MultiplayerDisplay} from "../contracts/runtime-protocol.mts";
export const DEFAULT_TH05_MULTIPLAYER_DISPLAY: Readonly<Th05MultiplayerDisplay> = Object.freeze({
  bossBar: "native", phaseMarkers: false, phaseStars: false, phaseTimeout: false,
  timeoutAsTime: false, numericDream: false, numericPower: false,
});
export function normalizeTh05MultiplayerDisplay(value: unknown): Readonly<Th05MultiplayerDisplay> {
  const saved = value && typeof value === "object" && !Array.isArray(value) ? value as Record<string, unknown> : {};
  const bossBar = saved.bossBar === "eosd" || saved.bossBar === "ring" ? saved.bossBar : "native";
  const flag = (key: string) => saved[key] === true;
  return Object.freeze({bossBar, phaseMarkers: bossBar === "native" && flag("phaseMarkers"),
    phaseStars: bossBar === "eosd" && flag("phaseStars"), phaseTimeout: bossBar !== "native" || flag("phaseTimeout"),
    timeoutAsTime: flag("timeoutAsTime"), numericDream: flag("numericDream"), numericPower: flag("numericPower")});
}

export interface GameOptions {
  th05MultiplayerDisplay: Readonly<Th05MultiplayerDisplay>;
  th02RapidFire: boolean;
  thpracEnabled: boolean;
  thpracTouchControlsEnabled: boolean;
  magnifierEnabled: boolean;
  focusHitboxEnabled: boolean;
  th03FocusEnabled: boolean;
  faithBarEnabled: boolean;
  frameLimit60Enabled: boolean;
  pixelPerfectEnabled: boolean;
  touchEnabled: boolean;
  touchMovementMode: TouchMovementMode;
  touchSensitivity: number;
  touchControlOpacity: number;
  touchFocusMode: TouchFocusMode;
  doubleTapBombEnabled: boolean;
  restartButtonEnabled: boolean;
  alwaysHitbox: boolean;
  multiplayerLocalPlayerVisibility: boolean;
  // Opaque `MIDIPort.id` of the selected external MIDI output. Empty means
  // "first available". The on/off switch itself is deliberately NOT a
  // GameOptions field: external MIDI playback is session-only and must be
  // enabled again on every visit.
  externalMidiDeviceId: string;
}

export const DEFAULT_GAME_OPTIONS: Readonly<GameOptions> = Object.freeze({
  th05MultiplayerDisplay: DEFAULT_TH05_MULTIPLAYER_DISPLAY,
  th02RapidFire: true,
  thpracEnabled: false,
  thpracTouchControlsEnabled: false,
  magnifierEnabled: false,
  focusHitboxEnabled: false,
  th03FocusEnabled: true,
  faithBarEnabled: false,
  frameLimit60Enabled: true,
  pixelPerfectEnabled: false,
  touchEnabled: false,
  touchMovementMode: "touch-unlimited",
  touchSensitivity: 150,
  touchControlOpacity: 100,
  touchFocusMode: "hold-button",
  doubleTapBombEnabled: false,
  restartButtonEnabled: false,
  alwaysHitbox: false,
  multiplayerLocalPlayerVisibility: false,
  externalMidiDeviceId: "",
});

export const TOUCH_MOVEMENT_MODES = new Set<TouchMovementMode>([
  "touch", "touch-unlimited", "joystick", "joystick-free",
]);
export const TOUCH_FOCUS_MODES = new Set<TouchFocusMode>([
  "two-finger", "hold-button", "toggle-button",
]);
export const MUSIC_MODES = new Set<MusicMode>(["ogg-stream", "ogg-full", "ogg-akyu", "midi", "none"]);

export function isMusicMode(value: string): value is MusicMode {
  return value === "ogg-stream" || value === "ogg-full" || value === "ogg-akyu" || value === "midi" || value === "none";
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
export const sharedTouchPreferenceStorageKey = "eagler-touhou-touch-options-v1";

export const SHARED_TOUCH_OPTION_NAMES = Object.freeze([
  "touchMovementMode",
  "touchSensitivity",
  "touchControlOpacity",
  "touchFocusMode",
  "doubleTapBombEnabled",
  "restartButtonEnabled",
  "thpracTouchControlsEnabled",
] as const);

export type SharedTouchOptionName = (typeof SHARED_TOUCH_OPTION_NAMES)[number];
export type SharedTouchOptions = Pick<GameOptions, SharedTouchOptionName>;

export interface GamePreferenceStorage {
  getItem(key: string): string | null;
  setItem(key: string, value: string): void;
}

function sharedTouchOptionsFrom(options: GameOptions): SharedTouchOptions {
  return Object.fromEntries(SHARED_TOUCH_OPTION_NAMES.map(name => [name, options[name]])) as SharedTouchOptions;
}

export function normalizeSharedTouchPreferences(value: unknown, fallback: GameOptions): SharedTouchOptions {
  const saved = record(value);
  const normalized = normalizeStoredGamePreferences({ options: { ...fallback, ...saved } }, {
    thpracAvailable: true,
    webAudioAvailable: true,
  });
  return Object.freeze(sharedTouchOptionsFrom(normalized.options));
}

export function applySharedTouchPreferences(options: GameOptions, shared: SharedTouchOptions): GameOptions {
  // Launcher state intentionally mutates option fields before persisting and
  // rendering them. Keep only the standalone shared snapshot immutable.
  return { ...options, ...shared };
}

export function loadOrInitializeSharedTouchPreferences(
  storage: GamePreferenceStorage | null,
  fallback: GameOptions,
): SharedTouchOptions {
  const saved = readStoredJson(storage, sharedTouchPreferenceStorageKey);
  const normalized = normalizeSharedTouchPreferences(saved, fallback);
  if (saved == null && storage) {
    try { storage.setItem(sharedTouchPreferenceStorageKey, JSON.stringify(normalized)); } catch {}
  }
  return normalized;
}

export function persistSharedTouchPreferences(
  storage: GamePreferenceStorage | null,
  options: GameOptions,
): void {
  if (!storage) return;
  try {
    storage.setItem(sharedTouchPreferenceStorageKey, JSON.stringify(sharedTouchOptionsFrom(options)));
  } catch {}
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

function stringOption(source: StoredRecord | null, name: keyof GameOptions, fallback: string): string {
  const value = source?.[name];
  return typeof value === "string" ? value : fallback;
}

function normalizeMusicMode(value: unknown): MusicMode {
  const legacy = value === "ogg" || value === "wav" ? "ogg-stream" : value;
  return typeof legacy === "string" && MUSIC_MODES.has(legacy as MusicMode)
    ? legacy as MusicMode
    : "ogg-stream";
}

export interface NormalizeGamePreferencesContext {
  uiLocale?: string;
  thpracAvailable: boolean;
  webAudioAvailable: boolean;
  // The remembered external MIDI output is only meaningful where the product
  // owns a MIDI music path and the browsing context exposes Web MIDI; a stored
  // device id must never survive into a place it cannot be used.
  externalMidiAvailable?: boolean;
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
  if (saved && savedOptions) {
    const sanitizedOptions = { ...savedOptions };
    let changed = false;
    if (Object.hasOwn(sanitizedOptions, "limitPresentationTo60")) {
      delete sanitizedOptions.limitPresentationTo60;
      changed = true;
    }
    if (Object.hasOwn(sanitizedOptions, "th06FocusHitbox")) {
      if (!Object.hasOwn(sanitizedOptions, "focusHitboxEnabled")) {
        sanitizedOptions.focusHitboxEnabled = sanitizedOptions.th06FocusHitbox;
      }
      delete sanitizedOptions.th06FocusHitbox;
      changed = true;
    }
    if (Object.hasOwn(sanitizedOptions, "enhanceLocalPlayerVisibility")) {
      if (!Object.hasOwn(sanitizedOptions, "multiplayerLocalPlayerVisibility")) {
        sanitizedOptions.multiplayerLocalPlayerVisibility = sanitizedOptions.enhanceLocalPlayerVisibility;
      }
      delete sanitizedOptions.enhanceLocalPlayerVisibility;
      changed = true;
    }
    if (changed) {
      sanitizedRecord = { ...saved, options: sanitizedOptions };
      storageRewriteRequired = true;
    }
  }

  const rawOptions = record(sanitizedRecord?.options);
  const movementCandidate = rawOptions?.touchMovementMode;
  const migratedMovement: TouchMovementMode = typeof movementCandidate === "string" &&
    TOUCH_MOVEMENT_MODES.has(movementCandidate as TouchMovementMode)
    ? movementCandidate as TouchMovementMode
    : rawOptions?.unlimitedTouch === true ? "touch-unlimited"
    : rawOptions?.unlimitedTouch === false ? "touch" : DEFAULT_GAME_OPTIONS.touchMovementMode;

  const focusCandidate = rawOptions?.touchFocusMode;
  let focusMode: TouchFocusMode = typeof focusCandidate === "string" &&
    TOUCH_FOCUS_MODES.has(focusCandidate as TouchFocusMode)
    ? focusCandidate as TouchFocusMode
    : "hold-button";
  if (touchMovementUsesJoystick(migratedMovement) && focusMode === "two-finger") focusMode = "hold-button";

  const sensitivityCandidate = rawOptions?.touchSensitivity;
  const touchSensitivity = typeof sensitivityCandidate === "number" && Number.isFinite(sensitivityCandidate)
    ? Math.min(TOUCH_SENSITIVITY_MAX, Math.max(TOUCH_SENSITIVITY_MIN, Math.round(sensitivityCandidate)))
    : DEFAULT_GAME_OPTIONS.touchSensitivity;

  const options: GameOptions = {
    th05MultiplayerDisplay: normalizeTh05MultiplayerDisplay(rawOptions?.th05MultiplayerDisplay),
    th02RapidFire: booleanOption(rawOptions, "th02RapidFire", DEFAULT_GAME_OPTIONS.th02RapidFire),
    thpracEnabled: context.thpracAvailable && booleanOption(rawOptions, "thpracEnabled", /^zh(?:-|$)/i.test(context.uiLocale || "") || DEFAULT_GAME_OPTIONS.thpracEnabled),
    thpracTouchControlsEnabled: booleanOption(rawOptions, "thpracTouchControlsEnabled", DEFAULT_GAME_OPTIONS.thpracTouchControlsEnabled),
    magnifierEnabled: booleanOption(rawOptions, "magnifierEnabled", DEFAULT_GAME_OPTIONS.magnifierEnabled),
    focusHitboxEnabled: booleanOption(rawOptions, "focusHitboxEnabled", DEFAULT_GAME_OPTIONS.focusHitboxEnabled),
    th03FocusEnabled: booleanOption(rawOptions, "th03FocusEnabled", DEFAULT_GAME_OPTIONS.th03FocusEnabled),
    faithBarEnabled: booleanOption(rawOptions, "faithBarEnabled", DEFAULT_GAME_OPTIONS.faithBarEnabled),
    frameLimit60Enabled: booleanOption(rawOptions, "frameLimit60Enabled", DEFAULT_GAME_OPTIONS.frameLimit60Enabled),
    pixelPerfectEnabled: booleanOption(rawOptions, "pixelPerfectEnabled", DEFAULT_GAME_OPTIONS.pixelPerfectEnabled),
    touchEnabled: booleanOption(rawOptions, "touchEnabled", DEFAULT_GAME_OPTIONS.touchEnabled),
    touchMovementMode: migratedMovement,
    touchSensitivity,
    touchControlOpacity: typeof rawOptions?.touchControlOpacity === "number" && Number.isFinite(rawOptions.touchControlOpacity)
      ? Math.min(100, Math.max(20, Math.round(rawOptions.touchControlOpacity / 5) * 5)) : 100,
    touchFocusMode: focusMode,
    doubleTapBombEnabled: booleanOption(rawOptions, "doubleTapBombEnabled", DEFAULT_GAME_OPTIONS.doubleTapBombEnabled),
    restartButtonEnabled: booleanOption(rawOptions, "restartButtonEnabled", DEFAULT_GAME_OPTIONS.restartButtonEnabled),
    alwaysHitbox: booleanOption(rawOptions, "alwaysHitbox", DEFAULT_GAME_OPTIONS.alwaysHitbox),
    multiplayerLocalPlayerVisibility: booleanOption(rawOptions, "multiplayerLocalPlayerVisibility", DEFAULT_GAME_OPTIONS.multiplayerLocalPlayerVisibility),
    externalMidiDeviceId: context.externalMidiAvailable !== false
      ? stringOption(rawOptions, "externalMidiDeviceId", DEFAULT_GAME_OPTIONS.externalMidiDeviceId)
      : "",
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
  const previous = record(saved);
  const initializeHighRefresh = (preferenceId === 'th11' || preferenceId === 'th11mp')
    && previous !== null && previous.highRefreshPreferenceVersion !== 1;
  if (initializeHighRefresh) {
    saved = {...previous, highRefreshPreferenceVersion: 1,
      options: {...record(previous.options), frameLimit60Enabled: true}};
  }
  const normalized = normalizeStoredGamePreferences(saved, context);
  if ((normalized.storageRewriteRequired || initializeHighRefresh) && storage) {
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
    const saved = serializeGamePreferences(preferences);
    if (preferenceId === 'th11' || preferenceId === 'th11mp') saved.highRefreshPreferenceVersion = 1;
    storage.setItem(gamePreferenceStorageKey(preferenceId), JSON.stringify(saved));
  } catch {}
  try { storage.setItem(languagePreferenceStorageKey(preferenceId), language); } catch {}
}
