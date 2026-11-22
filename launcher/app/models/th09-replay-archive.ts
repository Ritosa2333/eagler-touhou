import {z} from 'zod';

const count = z.number().int().nonnegative().max(Number.MAX_SAFE_INTEGER);
export const th09PublicUsername = z.string().regex(/^[a-z0-9_.-]{1,32}$/i).refine(value => value !== '.' && value !== '..');

export const th09Duel = z.object({
  duelId: z.string().uuid(),
  wins: count,
  losses: count,
  durationFrames: count.positive().nullish(),
  replayCount: count.optional(),
  playedAt: count.max(8.64e15),
  opponent: z.object({
    username: th09PublicUsername,
    displayName: z.string().min(1).max(128),
    avatar: z.string().regex(/^[a-f0-9]{16}$/).optional(),
  }).nullable(),
}).refine(value => value.wins + value.losses > 0 && Number.isSafeInteger(value.wins + value.losses));

export type Th09ReplayEntry = {username: string; duelId: string; roundId: string; name?: string};
export type WatchTh09Replay = (entry: Th09ReplayEntry, action: 'play' | 'download') => Promise<void>;
