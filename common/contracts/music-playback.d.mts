type TrackRef = {platform: 'netease' | 'qq'; id: string};
export type MusicRepeat = 'off' | 'all' | 'one';
interface MusicSeed { positionMs?: number; paused?: boolean; repeat?: MusicRepeat; shuffle?: boolean }
export interface MusicPlayback<T extends TrackRef> {
  revision: number;
  track: TrackRef;
  queue: T[];
  order: number[];
  cursor: number;
  startedAt: number;
  pausedAt: number | null;
  playback: number;
  ended: boolean;
  repeat: MusicRepeat;
  shuffle: boolean;
}
export const MUSIC_QUEUE_LIMIT: number;
export const MUSIC_POSITION_LIMIT: number;
export function sameMusicTrack(a?: TrackRef | null, b?: TrackRef | null): boolean;
export function uniqueMusicQueue<T extends TrackRef>(tracks: T[]): T[];
export function createMusicPlayback<T extends TrackRef>(track: TrackRef, queue: T[], time: number, seed?: MusicSeed, random?: () => number): MusicPlayback<T>;
export function musicPlaybackSnapshot<T extends TrackRef>(state: MusicPlayback<T>): Omit<MusicPlayback<T>, 'order' | 'cursor'> & {queueIndex: number; hasPrevious: boolean; hasNext: boolean};
export function applyMusicControl<T extends TrackRef>(state: MusicPlayback<T>, action: string, fields: Record<string, unknown>, time: number, random?: () => number): boolean | 'queue-full';
