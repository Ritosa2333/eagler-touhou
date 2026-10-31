/** Purpose of the shared Player. Loading, fullscreen and room membership are
 * separate lifecycle/presentation facts, not additional playback modes. */
export type PlayerMode = 'singleplayer' | 'multiplayer' | 'spectator' | 'replay';

export function classifyPlayerMode({
  replay = false,
  spectator = false,
  multiplayer = false,
}: {
  replay?: boolean;
  spectator?: boolean;
  multiplayer?: boolean;
}): PlayerMode {
  if (replay) return 'replay';
  if (spectator) return 'spectator';
  return multiplayer ? 'multiplayer' : 'singleplayer';
}

export function playerUsesGameInput(mode: PlayerMode): boolean {
  return mode === 'singleplayer' || mode === 'multiplayer';
}
