import {useEffect, useState} from 'react';
import {z} from 'zod';
import {accountsEnabled} from '../release-features';
import {accountRequest} from './account-client';
import {useAccountSession} from './account-session';

const rating = z.object({rating: z.number().int(), interval: z.number().nonnegative(), games: z.number().int().nonnegative(), group: z.number().int().positive()});
const snapshot = z.object({sequence: z.number().int(), players: z.record(z.string(), rating)});
export type Th09Rating = z.infer<typeof rating>;
export const th09RatingSettled = 'th09-rating-settled';
const empty: Readonly<Record<string, Th09Rating>> = Object.freeze(Object.create(null));

export function useTh09Ratings(enabled: boolean) {
  const interactive = accountsEnabled && enabled;
  const {state} = useAccountSession(interactive);
  const username = state.status === 'member' ? state.user.username : undefined;
  const [data, setData] = useState<{username?: string; players: Readonly<Record<string, Th09Rating>>}>({players: empty});
  useEffect(() => {
    if (!interactive) return;
    let pending: AbortController | undefined;
    let disposed = false;
    async function refresh() {
      if (pending || document.hidden) return;
      const controller = new AbortController();
      pending = controller;
      try {
        const raw = await accountRequest('v1/th09/ratings', undefined, {signal: controller.signal});
        const result = snapshot.safeParse(raw);
        if (result.success && !disposed) setData({username, players: Object.assign(Object.create(null), result.data.players)});
      } catch { /* Account API and ratings can be deployed independently. */ }
      finally {pending = undefined;}
    }
    void refresh();
    const interval = window.setInterval(refresh, 12_000);
    window.addEventListener(th09RatingSettled, refresh);
    document.addEventListener('visibilitychange', refresh);
    return () => {
      disposed = true;
      pending?.abort();
      window.clearInterval(interval);
      window.removeEventListener(th09RatingSettled, refresh);
      document.removeEventListener('visibilitychange', refresh);
    };
  }, [interactive, username]);
  const players = interactive && data.username === username ? data.players : empty;
  return {players, mine: username ? players[username] : undefined};
}
