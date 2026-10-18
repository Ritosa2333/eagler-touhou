export interface MultiplayerRuntimeLoadout {
  character: number;
  shot: number;
}

export interface MultiplayerRuntimeOptionConstraints {
  difficultyMax: number;
  characterMax: number;
}

export interface MultiplayerRuntimeOptionInput {
  url: string;
  player: number | null;
  playerCount: number;
  seed: number;
  difficulty: number;
  spectator: boolean;
  spectatorId: string;
  spectatorCount: number;
  iceServers: unknown;
  loadouts: MultiplayerRuntimeLoadout[];
}

export interface MultiplayerRuntimeOptions {
  netplayMode: "lan";
  netplayUrl: string;
  netplayPlayer: number | null;
  netplayPlayerCount: number;
  netplaySeed: number;
  netplayDifficulty: number;
  netplaySpectator: boolean;
  netplaySpectatorId: string;
  netplaySpectatorCount: number;
  netplayIceServers: unknown[];
  netplayLoadouts: MultiplayerRuntimeLoadout[];
}

export function buildMultiplayerRuntimeOptions(
  input: MultiplayerRuntimeOptionInput,
  constraints: MultiplayerRuntimeOptionConstraints,
): MultiplayerRuntimeOptions {
  let url: URL;
  try { url = new URL(input.url); }
  catch { throw new Error("Relay WebSocket URL 无效"); }
  if (url.protocol !== "ws:" && url.protocol !== "wss:") {
    throw new Error("Relay URL 必须使用 ws:// 或 wss://");
  }

  const { player, playerCount, seed } = input;
  const spectator = input.spectator === true;
  if (![2, 3].includes(playerCount) || (!spectator &&
      (typeof player !== "number" || !Number.isInteger(player) || player < 0 || player >= playerCount))) {
    throw new Error("LAN 玩家槽位无效");
  }
  if (spectator && !/^[A-Za-z0-9_-]{8,64}$/.test(String(input.spectatorId || ""))) {
    throw new Error("旁观者资格无效");
  }
  if (!Number.isInteger(seed) || seed < 0 || seed > 65535) {
    throw new Error("LAN 同步种子必须在 0–65535 之间");
  }

  const difficultyMax = constraints.difficultyMax;
  const difficulty = Number(input.difficulty);
  if (!Number.isInteger(difficulty) || difficulty < 0 || difficulty > difficultyMax) {
    throw new Error(`LAN 难度必须在 0–${difficultyMax} 之间`);
  }

  const maxCharacter = constraints.characterMax;
  const loadouts = input.loadouts.slice(0, playerCount).map(({ character, shot }, index) => {
    if (!Number.isInteger(character) || character < 0 || character > maxCharacter ||
        !Number.isInteger(shot) || shot < 0 || shot > 1) {
      throw new Error(`P${index + 1} 机体配置无效`);
    }
    return { character, shot };
  });
  if (loadouts.length !== playerCount) throw new Error("LAN 机体配置数量不足");

  return {
    netplayMode: "lan",
    netplayUrl: url.href,
    netplayPlayer: player,
    netplayPlayerCount: playerCount,
    netplaySeed: seed,
    netplayDifficulty: difficulty,
    netplaySpectator: spectator,
    netplaySpectatorId: spectator ? input.spectatorId : "",
    netplaySpectatorCount: Math.max(0, Number(input.spectatorCount) || 0),
    netplayIceServers: Array.isArray(input.iceServers) ? input.iceServers : [],
    netplayLoadouts: loadouts,
  };
}
