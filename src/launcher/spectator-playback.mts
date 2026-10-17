export interface SpectatorNode {
  id: number;
  kind: 'stage' | 'boss';
  frame: number;
  stage: number;
  bossSlots: number;
  /** Title-owned route/encounter name, e.g. Final A or Phantasm. */
  label?: string;
}
export interface SpectatorPlaybackState {
  version: 1;
  run: string;
  revision: number;
  min: number;
  position: number;
  received: number;
  edge: number;
  target: number;
  paused: boolean;
  seeking: boolean;
  following: boolean;
  ended: boolean;
  nodes: readonly SpectatorNode[];
}
export type SpectatorPlaybackAction = 'pause' | 'play' | 'seek' | 'live';
export interface SpectatorPlaybackCommand {
  run: string;
  action: SpectatorPlaybackAction;
  frame?: number;
}
const uint = (value: unknown, max = 0xfffffffe): value is number =>
  typeof value === 'number' && Number.isInteger(value) && value >= 0 && value <= max;
const object = (value: unknown): value is Record<string, unknown> => !!value && typeof value === 'object' && !Array.isArray(value);
/** Native reports only restorable history. A room flag cannot manufacture a
 * playback capability, and a stale report cannot rewind the receive frontier. */
export function parseSpectatorPlayback(value: unknown): Readonly<SpectatorPlaybackState> | null {
  if (!object(value) || value.version !== 1 || typeof value.run !== 'string' ||
      !/^[A-Za-z0-9_-]{1,64}$/.test(value.run) ||
      !['revision', 'min', 'position', 'received', 'edge', 'target'].every(key => uint(value[key])) ||
      !['paused', 'seeking', 'following', 'ended'].every(key => typeof value[key] === 'boolean') ||
      !Array.isArray(value.nodes) || value.nodes.length > 512) return null;
  const state = value as unknown as SpectatorPlaybackState;
  if (state.min > state.position || state.position > state.received || state.min > state.edge ||
      state.edge > state.received || state.target < state.min || state.target > state.received) return null;
  const ids = new Set<number>(); let lastFrame = state.min;
  const nodes: SpectatorNode[] = [];
  for (const raw of value.nodes) {
    if (!object(raw) || !uint(raw.id) || !raw.id || ids.has(raw.id) || !uint(raw.frame) ||
        raw.frame < lastFrame || raw.frame > state.received || !uint(raw.stage, 65535) ||
        !uint(raw.bossSlots, 255) || !(raw.kind === 'stage' && raw.bossSlots === 0 || raw.kind === 'boss' && raw.bossSlots > 0) ||
        raw.label !== undefined && (typeof raw.label !== 'string' || !raw.label.length || raw.label.length > 48 || /[\x00-\x1f\x7f]/.test(raw.label))) return null;
    ids.add(raw.id); lastFrame = raw.frame;
    nodes.push(Object.freeze({id: raw.id, kind: raw.kind, frame: raw.frame, stage: raw.stage, bossSlots: raw.bossSlots,
      ...(raw.label === undefined ? {} : {label: raw.label})}) as SpectatorNode);
  }
  return Object.freeze({version: 1, run: state.run, revision: state.revision, min: state.min,
    position: state.position, received: state.received, edge: state.edge, target: state.target,
    paused: state.paused, seeking: state.seeking, following: state.following, ended: state.ended, nodes: Object.freeze(nodes)});
}
export function spectatorPlaybackCommand(state: SpectatorPlaybackState | null, action: SpectatorPlaybackAction, frame?: number): SpectatorPlaybackCommand | null {
  if (!state || !['pause', 'play', 'seek', 'live'].includes(action)) return null;
  if (action === 'seek' && (!uint(frame) || frame < state.min || frame > state.edge)) return null;
  return {run: state.run, action, ...(action === 'seek' ? {frame} : {})};
}
export function createSpectatorPlayback() {
  let epoch = 0, state: Readonly<SpectatorPlaybackState> | null = null;
  const listeners = new Set<() => void>();
  const publish = (next: Readonly<SpectatorPlaybackState> | null) => {if (state === next) return; state = next; for (const listener of listeners) listener();};
  return {
    subscribe(listener: () => void) {listeners.add(listener); return () => {listeners.delete(listener);};},
    getSnapshot: () => state,
    update(owner: {epoch: number | null; game: string | null; launched: boolean; spectator: boolean}, value: unknown) {
      if (epoch !== owner.epoch) {epoch = owner.epoch ?? 0; publish(null);}
      // TH09 retains its native match/round panel and command semantics.
      if (!owner.launched || !owner.spectator || owner.game === 'th09') {publish(null); return;}
      if (value === null) {publish(null); return;}
      const next = parseSpectatorPlayback(value);
      if (!next) return;
      if (state?.run === next.run && (next.revision <= state.revision || next.received < state.received)) return;
      publish(next);
    },
    clear() {epoch = 0; publish(null);},
  };
}
