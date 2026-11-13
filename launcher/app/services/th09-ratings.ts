import {createContext, useEffect, useState} from 'react';
import {z} from 'zod';
import {accountsEnabled} from '../release-features';
import {accountRequest} from './account-client';
import {useAccountSession} from './account-session';

const rating = z.object({rating: z.number().int(), interval: z.number().nonnegative(), games: z.number().int().nonnegative(), group: z.number().int().positive()});
const ratingCard = z.object({
  rating: z.number().int().optional(),
  characters: z.array(z.tuple([z.number().int().min(0).max(15), z.number().int()])).max(3),
});
const snapshot = z.object({sequence: z.number().int(), players: z.record(z.string(), rating),
  cards: z.record(z.string(), ratingCard).optional()});
export type Th09Rating = z.infer<typeof rating>;
export type Th09RatingCard = z.infer<typeof ratingCard>;
export type Th09RatingCards = Readonly<Record<string, Th09RatingCard>>;
export const th09RatingSettled = 'th09-rating-settled';
const empty: Readonly<Record<string, Th09Rating>> = Object.freeze(Object.create(null));
const emptyCards: Th09RatingCards = Object.freeze(Object.create(null));
export const Th09RatingCardsContext = createContext<Th09RatingCards | undefined>(undefined);

export function useTh09Ratings(enabled: boolean, usernames: (string | undefined)[]) {
  const interactive = accountsEnabled && enabled;
  const {state} = useAccountSession(interactive);
  const ready = interactive && state.status !== 'loading';
  const username = state.status === 'member' ? state.user.username : undefined;
  const roster = [...new Set(usernames.flatMap(name => name ? [name.toLowerCase()] : []))].sort().slice(0, 400).join(',');
  const [data, setData] = useState<{username?: string; players: Readonly<Record<string, Th09Rating>>; cards: Th09RatingCards}>({
    players: empty, cards: emptyCards,
  });
  useEffect(() => {
    if (!ready) return;
    let pending: AbortController | undefined;
    let disposed = false;
    async function refresh() {
      if (pending || document.hidden) return;
      const controller = new AbortController();
      pending = controller;
      try {
        const query = roster ? new URLSearchParams({cards: roster}) : undefined;
        const raw = await accountRequest('v1/th09/ratings', undefined, {signal: controller.signal, query});
        const result = snapshot.safeParse(raw);
        if (result.success && !disposed) setData({
          username,
          players: Object.assign(Object.create(null), result.data.players),
          cards: Object.assign(Object.create(null), result.data.cards),
        });
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
  }, [ready, roster, username]);
  const current = ready && data.username === username ? data : undefined;
  const players = current?.players ?? empty;
  return {players, mine: username ? players[username] : undefined, cards: current?.cards ?? emptyCards};
}
