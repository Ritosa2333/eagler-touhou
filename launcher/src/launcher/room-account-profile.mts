import {parseTh09AccountSummary, type Th09AccountSummary} from './th09-room-stats.mjs';

export type RoomAccountProfile = Readonly<{username: string; avatar?: string; th09?: Th09AccountSummary}>;

export function parseRoomAccountProfile(value: unknown): RoomAccountProfile | undefined {
  if (!value || typeof value !== 'object') return;
  const {username, avatar, th09} = value as Record<string, unknown>;
  if (typeof username !== 'string' || !/^[a-z0-9_.-]{1,32}$/.test(username) || username === '.' || username === '..') return;
  const summary = parseTh09AccountSummary(th09);
  return Object.freeze({
    username,
    ...(summary && {th09: summary}),
    ...(typeof avatar === 'string' && /^[a-f0-9]{16}$/.test(avatar) ? {avatar} : {}),
  });
}
