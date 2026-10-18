const relayRoleParams = Object.freeze([
  "room",
  "run",
  "lobby",
  "player",
  "players",
  "signal",
  "spectator",
]);

export interface MultiplayerLobbyRelayUrl {
  roomId: string;
  url: string;
}

export type MultiplayerGameplayRelayRole =
  | { player: number }
  | { spectator: string };

function relayUrl(value: string): URL {
  let url: URL;
  try { url = new URL(value); }
  catch { throw new TypeError("invalid multiplayer relay URL"); }
  if (url.protocol !== "ws:" && url.protocol !== "wss:") {
    throw new TypeError("multiplayer relay URL must use ws or wss");
  }
  return url;
}

function clearRelayRole(url: URL): void {
  for (const key of relayRoleParams) url.searchParams.delete(key);
}

export function multiplayerTransportRoomId(product: string, roomCode: string): string {
  return `${product}-${roomCode}`;
}

export function buildMultiplayerLobbyRelayUrl(
  baseUrl: string,
  { product, roomCode, clientId }: { product: string; roomCode: string; clientId: string },
): MultiplayerLobbyRelayUrl {
  const url = relayUrl(baseUrl);
  clearRelayRole(url);
  const roomId = multiplayerTransportRoomId(product, roomCode);
  url.searchParams.set("room", roomId);
  url.searchParams.set("lobby", clientId);
  return { roomId, url: url.href };
}

export function buildMultiplayerGameplayRelayUrl(
  baseUrl: string,
  {
    product,
    roomCode,
    runId,
    role,
  }: {
    product: string;
    roomCode: string;
    runId: number;
    role: MultiplayerGameplayRelayRole;
  },
): string {
  const url = relayUrl(baseUrl);
  clearRelayRole(url);
  url.searchParams.set("room", multiplayerTransportRoomId(product, roomCode));
  url.searchParams.set("run", String(Math.max(0, Math.trunc(Number(runId) || 0))));
  if ("spectator" in role) url.searchParams.set("spectator", role.spectator);
  else url.searchParams.set("player", String(role.player));
  return url.href;
}
