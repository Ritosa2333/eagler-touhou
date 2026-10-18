import { HOST_PROTOCOL, isGameId, type GameId } from "./product-catalog.mjs";

export const RUNTIME_PROTOCOL_COMMANDS = Object.freeze([
  "configure",
  "resources",
  "keyboard",
  "keyboard-clear",
  "touch-controls",
  "touch-cancel",
  "direct-touch",
  "launch",
  "list",
  "read",
  "write",
  "remove",
  "sync",
] as const);

// Optional commands are capability-specific and are not required of every
// Runtime shell. They still share the same envelope and request ownership.
export const RUNTIME_PROTOCOL_OPTIONAL_COMMANDS = Object.freeze([
  "retry-music",
  "thprac-mouse",
] as const);

export const RUNTIME_PROTOCOL_EVENTS = Object.freeze([
  "ready",
  "transfer",
  "first-frame",
  "runtime-info",
  "frame-health",
  "exit",
  "error",
] as const);

export const RUNTIME_PROTOCOL_OPTIONAL_EVENTS = Object.freeze([
  "audio-health",
  "midi-fallback",
  "music-complete",
  "music-error",
  "music-incomplete",
  "player-debug",
  "thprac-session",
] as const);

export type RuntimeProtocolCommand =
  | (typeof RUNTIME_PROTOCOL_COMMANDS)[number]
  | (typeof RUNTIME_PROTOCOL_OPTIONAL_COMMANDS)[number];

export type RuntimeProtocolEvent =
  | (typeof RUNTIME_PROTOCOL_EVENTS)[number]
  | (typeof RUNTIME_PROTOCOL_OPTIONAL_EVENTS)[number];

export interface RuntimeCommandPayloads {
  configure: { options?: unknown; [key: string]: unknown };
  resources: { resources: unknown[] };
  keyboard: { down: boolean; code: string };
  "keyboard-clear": Record<string, never>;
  "touch-controls": { controls: unknown };
  "touch-cancel": Record<string, never>;
  "direct-touch": { phase: string; pointer: unknown };
  launch: Record<string, never>;
  list: Record<string, never>;
  read: { path: string };
  write: { path: string; bytes: number[] };
  remove: { path: string };
  sync: Record<string, never>;
  "retry-music": Record<string, never>;
  "thprac-mouse": { phase: string; x?: number; y?: number; button?: number };
}

export interface RuntimeProtocolEnvelope {
  protocol: typeof HOST_PROTOCOL;
  game: GameId;
}

export type RuntimeCommandMessage<C extends RuntimeProtocolCommand = RuntimeProtocolCommand> =
  RuntimeProtocolEnvelope & { command: C; request?: string } & RuntimeCommandPayloads[C];

export interface RuntimeSuccessResponse extends RuntimeProtocolEnvelope {
  request: string;
  ok: true;
  [key: string]: unknown;
}

export interface RuntimeFailureResponse extends RuntimeProtocolEnvelope {
  request: string;
  ok: false;
  error?: unknown;
  errno?: unknown;
  [key: string]: unknown;
}

export type RuntimeResponseMessage = RuntimeSuccessResponse | RuntimeFailureResponse;

export type RuntimeEventMessage<E extends RuntimeProtocolEvent = RuntimeProtocolEvent> =
  RuntimeProtocolEnvelope & { event: E; [key: string]: unknown };

export type RuntimeInboundMessage = RuntimeEventMessage | RuntimeResponseMessage;

export function isRuntimeResponseMessage(value: RuntimeInboundMessage): value is RuntimeResponseMessage {
  return typeof value.request === "string" && typeof value.ok === "boolean";
}

type UnknownRecord = Record<string, unknown>;

const runtimeEvents = new Set<string>([
  ...RUNTIME_PROTOCOL_EVENTS,
  ...RUNTIME_PROTOCOL_OPTIONAL_EVENTS,
]);

function isRecord(value: unknown): value is UnknownRecord {
  return value !== null && typeof value === "object" && !Array.isArray(value);
}

export function parseRuntimeInboundMessage(value: unknown, expectedGame: GameId): RuntimeInboundMessage | null {
  if (!isRecord(value) || value.protocol !== HOST_PROTOCOL || value.game !== expectedGame || !isGameId(expectedGame)) {
    return null;
  }
  if (typeof value.event === "string" && runtimeEvents.has(value.event)) {
    return value as RuntimeEventMessage;
  }
  if (typeof value.request === "string" && typeof value.ok === "boolean") {
    return value as RuntimeResponseMessage;
  }
  return null;
}
