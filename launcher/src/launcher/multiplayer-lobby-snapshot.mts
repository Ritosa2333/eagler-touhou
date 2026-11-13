import {validTh19VersusRules} from './th19-versus-rules.mjs';
import { validTh15ChapterThreshold } from "./th15-chapter-reward.mjs";
import {validTh05InitialSpares,validTh05InitialBombs} from './th05-initial-stock.mjs';
import {
  parseMeasuredNetplayTiming,
  resolveAdonisPredictionReserve,
  type MeasuredNetplayTiming,
} from '../contracts/netplay-timing.mjs';
import {
  multiplayerInputTimingPolicy,
  type MultiplayerInputTimingConfig,
} from '../contracts/product-catalog.mjs';
import {
  normalizeMultiplayerDisplayName,
  multiplayerControlMode,
  validMultiplayerClientId,
} from './multiplayer-identity.mjs';
import { parseTh09PlayerStats, type Th09PlayerStats } from './th09-room-stats.mjs';
import {parseRoomAccountProfile, type RoomAccountProfile} from './room-account-profile.mjs';

export interface MultiplayerLobbySeat {
  clientId: string;
  name: string;
  profile?: RoomAccountProfile;
  stats?: Th09PlayerStats;
  loadout: number;
  ready: boolean;
  runtimeGeneration?: string;
  offline: boolean;
  controlMode: ReturnType<typeof multiplayerControlMode>;
  touchEnabled?: boolean;
  mobileDevice: boolean;
  resource: MultiplayerResourceProgress | null;
}

export interface MultiplayerResourceProgress {
  basePercent?: number | null;
  runtimePercent?: number | null;
  status: 'preparing' | 'ready' | 'failed' | 'cancelled' | 'importing';
  stage: 'package' | 'runtime';
  percent: number | null;
}

export interface MultiplayerLobbySpectator {
  clientId: string;
  name: string;
  profile?: RoomAccountProfile;
}

export interface NormalizedMultiplayerLobbySnapshot {
  th19Rules?: number[];
  initialSpares?: number;
  initialBombs?: number;
  timingChoice?: 'auto' | number;
  startedAt?: number;
  visibility: 'public' | 'private';
  disableCheatMovement: boolean;
  unrated?: boolean;
  disableLocalChargePriority?: boolean;
  challengeMode?: boolean;
  chapterThreshold?: number;
  prankMode?: boolean;
  playerCount: 2 | 3;
  difficulty: number;
  inputDelay: number;
  adonisMode?: number;
  inputDelayAuto?: boolean;
  predictionReserve?: number;
  timing?: MeasuredNetplayTiming | null;
  predictionLimit: number;
  settingsVersion: number;
  phase: 'lobby' | 'starting' | 'running';
  gameplayRelayFallback: boolean;
  runtimeGeneration?: string;
  spectators: MultiplayerLobbySpectator[];
  spectatorCount: number;
  spectatorTimeline?: boolean;
  seats: Array<MultiplayerLobbySeat | null>;
  localSeat: number | null;
  localSpectator: boolean;
}

function normalizedNonNegativeLimit(value: number): number {
  return Number.isFinite(value) ? Math.max(0, Math.trunc(value)) : 0;
}

function record(value: unknown): Record<string, unknown> | null {
  return value !== null && typeof value === 'object' && !Array.isArray(value)
    ? (value as Record<string, unknown>)
    : null;
}

export function normalizeMultiplayerLobbySnapshot(
  value: unknown,
  {
    localClientId,
    playerCounts,
    difficulties,
    loadouts,
    inputTiming,
  }: {
    localClientId: string;
    playerCounts: readonly (2 | 3)[];
    difficulties: readonly unknown[];
    loadouts: readonly unknown[];
    inputTiming?: Readonly<MultiplayerInputTimingConfig>;
  },
): NormalizedMultiplayerLobbySnapshot | null {
  const source = record(value);
  if(source?.th19Rules!==undefined&&!validTh19VersusRules(source.th19Rules))return null;
  if(source&&(source.initialSpares!==undefined&&!validTh05InitialSpares(source.initialSpares)||source.initialBombs!==undefined&&!validTh05InitialBombs(source.initialBombs)))return null;
  if (!source ||
    (source.chapterThreshold !== undefined &&
      !validTh15ChapterThreshold(source.chapterThreshold))
  ) return null;
  const policy = multiplayerInputTimingPolicy({ inputTiming });
  const timingChoice = source.timingChoice === 'auto' ? 'auto' as const :
    typeof source.timingChoice === 'number' && Number.isInteger(source.timingChoice) && source.timingChoice >= 0 && source.timingChoice <= policy.manualDelayLimit ? source.timingChoice : undefined;
  const phase = source.phase === 'starting' || source.phase === 'running' ? source.phase : 'lobby';
  const requestedPlayerCount = Number(source.playerCount);
  if (!playerCounts.includes(requestedPlayerCount as 2 | 3)) return null;
  const playerCount = requestedPlayerCount as 2 | 3;
  const difficulty = Math.max(
    0,
    Math.min(Math.max(0, difficulties.length - 1), Number(source.difficulty) || 0),
  );
  const adonisMode = source.adonisMode === undefined ? 0 : Number(source.adonisMode);
  if (source.inputDelayAuto !== undefined && typeof source.inputDelayAuto !== 'boolean')
    return null;
  if (source.inputDelayAuto && !adonisMode) return null;
  const predictionReserve = source.predictionReserve ?? 2;
  if (
    typeof predictionReserve !== 'number' ||
    !Number.isInteger(predictionReserve) ||
    predictionReserve < 1 ||
    predictionReserve > 2
  )
    return null;
  const timing = source.timing == null ? null : parseMeasuredNetplayTiming(source.timing);
  if (source.timing != null && !timing) return null;
  if (!Number.isInteger(adonisMode) || adonisMode < 0 || adonisMode > 2) return null;
  if (!policy.rollback && (adonisMode === 2 || (phase !== 'lobby' && adonisMode !== 1)))
    return null;
  const rawDelay = Number(source.inputDelay);
  const delayLimit = adonisMode ? (inputTiming ? policy.manualDelayLimit : 9) : 8;
  if (adonisMode && (!Number.isInteger(rawDelay) || rawDelay < 0 || rawDelay > delayLimit))
    return null;
  const inputDelay =
    Number.isInteger(rawDelay) && rawDelay >= 0 && rawDelay <= delayLimit ? rawDelay : 0;
  if (
    timing &&
    (timing.adonisMode !== adonisMode ||
      timing.inputDelay !== inputDelay ||
      timing.automatic !== (source.inputDelayAuto ?? false) ||
      timing.predictionReserve !==
        (adonisMode === 2
          ? resolveAdonisPredictionReserve(timing.fullDelay, predictionReserve, timing.automatic)
          : 0))
  )
    return null;
  const rawLimit = Number(source.predictionLimit);
  if (!policy.rollback && source.predictionLimit !== undefined && source.predictionLimit !== 0)
    return null;
  const predictionLimit = !policy.rollback
    ? 0
    : Number.isInteger(rawLimit) && rawLimit >= 1 && rawLimit <= policy.predictionLimit
      ? rawLimit
      : policy.predictionLimit;
  const normalizedLoadoutCount = normalizedNonNegativeLimit(loadouts.length);
  const settingsVersion = Math.max(1, Math.trunc(Number(source.settingsVersion) || 1));

  const spectators: MultiplayerLobbySpectator[] = Array.isArray(source.spectators)
    ? source.spectators.flatMap((entry) => {
        const item = record(entry);
        const clientId = String(item?.clientId || '');
        if (!validMultiplayerClientId(clientId)) return [];
        const profile = parseRoomAccountProfile(item?.profile);
        return [{clientId, name: normalizeMultiplayerDisplayName(item?.name || ''), ...(profile ? {profile} : {})}];
      })
    : [];

  const rawSpectatorCount = Number(source.spectatorCount);
  const spectatorCount =
    Number.isFinite(rawSpectatorCount) && rawSpectatorCount >= 0
      ? Math.max(spectators.length, Math.trunc(rawSpectatorCount))
      : spectators.length;

  const rawSeats = Array.isArray(source.seats) ? source.seats : [];
  const seats = Array.from({ length: 3 }, (_, index): MultiplayerLobbySeat | null => {
    const seat = record(rawSeats[index]);
    if (!seat) return null;
    const clientId = String(seat.clientId || '');
    const loadout = Number(seat.loadout);
    if (
      !validMultiplayerClientId(clientId) ||
      !Number.isInteger(loadout) ||
      loadout < 0 ||
      loadout >= normalizedLoadoutCount
    )
      return null;
    const profile = parseRoomAccountProfile(seat.profile);
    return {
      clientId,
      name: normalizeMultiplayerDisplayName(seat.name || ''),
      ...(profile ? {profile} : {}),
      stats: parseTh09PlayerStats(seat.stats),
      loadout,
      ready: !!seat.ready,
      ...(typeof seat.runtimeGeneration === 'string' && /^[a-f0-9]{64}$/.test(seat.runtimeGeneration)
        ? {runtimeGeneration: seat.runtimeGeneration} : {}),
      offline: !!seat.offline,
      controlMode: multiplayerControlMode(seat.controlMode),
      ...(typeof seat.touchEnabled === 'boolean' ? {touchEnabled: seat.touchEnabled} : {}),
      mobileDevice: seat.mobileDevice === true,
      resource: normalizeResourceProgress(seat.resource),
    };
  });

  const localSeatIndex = seats
    .slice(0, playerCount)
    .findIndex((seat) => seat?.clientId === localClientId);
  const localSeat = localSeatIndex >= 0 ? localSeatIndex : null;
  const localSpectator =
    localSeat == null && spectators.some((entry) => entry.clientId === localClientId);

  return {
    playerCount,
    ...(source.th19Rules!==undefined?{th19Rules:[...source.th19Rules as number[]]}:{}),
    ...(source.initialSpares!==undefined?{initialSpares:source.initialSpares as number}:{}),
    ...(source.initialBombs!==undefined?{initialBombs:source.initialBombs as number}:{}),
    ...(source.chapterThreshold !== undefined
      ? { chapterThreshold: source.chapterThreshold as number }
      : {}),
    visibility: source.visibility === 'private' ? 'private' : 'public',
    disableCheatMovement: source.disableCheatMovement === true,
    ...(typeof source.unrated === 'boolean' ? {unrated: source.unrated} : {}),
    ...(source.disableLocalChargePriority !== undefined ? {disableLocalChargePriority: source.disableLocalChargePriority === true} : {}),
    ...(source.challengeMode !== undefined ? { challengeMode: source.challengeMode === true } : {}),
    ...(source.prankMode !== undefined ? { prankMode: false } : {}),
    difficulty,
    inputDelay,
    ...(source.adonisMode !== undefined ? { adonisMode } : {}),
    ...(source.inputDelayAuto !== undefined
      ? { inputDelayAuto: source.inputDelayAuto as boolean }
      : {}),
    ...(source.predictionReserve !== undefined ? { predictionReserve } : {}),
    ...(source.timing !== undefined ? { timing } : {}),
    predictionLimit,
    settingsVersion,
    phase,
    // Relay-server capability mirror: only a deployment that enables the
    // gameplay relay fallback may select a relay route. Absent (older relay)
    // normalizes to false; runtime options only opt in on server truth.
    gameplayRelayFallback: source.gameplayRelayFallback === true,
    ...(typeof source.runtimeGeneration === 'string' && /^[a-f0-9]{64}$/.test(source.runtimeGeneration)
      ? {runtimeGeneration: source.runtimeGeneration} : {}),
    spectators,
    spectatorCount,
    ...(source.spectatorTimeline === true ? { spectatorTimeline: true } : {}),
    seats,
    localSeat,
    localSpectator,
    ...(timingChoice !== undefined ? {timingChoice} : {}),
    ...(typeof source.startedAt === 'number' && Number.isSafeInteger(source.startedAt) && source.startedAt > 0 ? {startedAt: source.startedAt} : {}),
  };
}

function normalizeResourceProgress(value: unknown): MultiplayerResourceProgress | null {
  const source = record(value);
  if (
    !source ||
    !['preparing', 'ready', 'failed', 'cancelled', 'importing'].includes(String(source.status)) ||
    (source.stage !== 'package' && source.stage !== 'runtime')
  )
    return null;
  const percent = source.percent === null ? null : Number(source.percent);
  return {
    status: source.status as MultiplayerResourceProgress['status'],
    ...Object.fromEntries(['basePercent', 'runtimePercent'].filter(key => key in source).map(key => [key,
      typeof source[key] === 'number' && Number.isFinite(source[key]) && source[key] >= 0 && source[key] <= 100 ? Math.round(source[key]) : null])),
    stage: source.stage,
    percent:
      percent !== null && Number.isFinite(percent) && percent >= 0 && percent <= 100
        ? Math.round(percent)
        : null,
  };
}
