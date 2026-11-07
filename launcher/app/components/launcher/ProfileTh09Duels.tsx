import {useEffect, useState} from 'react';
import {Link} from 'react-router';
import {z} from 'zod';
import {formatTh09WinRate, parseTh09AccountSummary, type Th09AccountSummary} from '../../../src/launcher/th09-room-stats.mjs';
import {useLocale} from '../../i18n';
import {accountRequest} from '../../services/account-client';
import {Button} from '../../ui/button';
import './profile-th09-duels.css';

const count = z.number().int().nonnegative().max(Number.MAX_SAFE_INTEGER);
const duel = z.object({
  duelId: z.string().uuid(),
  wins: count,
  losses: count,
  playedAt: z.number().int().min(0).max(8.64e15),
  opponent: z.object({
    username: z.string().regex(/^[a-z0-9_.-]{1,32}$/).refine(value => value !== '.' && value !== '..'),
    displayName: z.string().min(1).max(128),
  }).nullable(),
}).refine(value => value.wins + value.losses > 0 && Number.isSafeInteger(value.wins + value.losses));
const page = z.object({summary: z.custom<Th09AccountSummary>(value => !!parseTh09AccountSummary(value)),
  duels: z.array(duel).max(30), nextCursor: z.string().max(256).nullable().optional()});

export function ProfileTh09Duels({username, owner}: {username: string; owner: boolean}) {
  const {t, locale} = useLocale();
  const [data, setData] = useState<z.infer<typeof page>>();
  const [request, setRequest] = useState<{cursor: string | null}>({cursor: null});
  const [pending, setPending] = useState(true);
  const [failed, setFailed] = useState(false);
  useEffect(() => {
    const controller = new AbortController();
    setPending(true);
    setFailed(false);
    const path = 'v1/profiles/' + encodeURIComponent(username) + '/th09-duels';
    const query = request.cursor ? '?' + new URLSearchParams({cursor: request.cursor}) : '';
    void accountRequest(path + query, undefined, {signal: controller.signal}).then(value => {
      if (controller.signal.aborted) return;
      const next = page.parse(value);
      setData(current => request.cursor && current ? {
        ...next,
        duels: [...new Map([...current.duels, ...next.duels].map(item => [item.duelId, item])).values()],
      } : next);
    }).catch(() => {
      if (!controller.signal.aborted) setFailed(true);
    }).finally(() => {
      if (!controller.signal.aborted) setPending(false);
    });
    return () => controller.abort();
  }, [username, request]);
  if (!data && (!owner || !failed) || data && !owner && !data.summary.games) return null;
  const numbers = new Intl.NumberFormat(locale);
  const dates = new Intl.DateTimeFormat(locale, {dateStyle:'medium'});
  return <section className="profile-panel profile-public-section profile-th09" aria-labelledby="profile-th09-title" aria-busy={pending}>
    <div className="profile-section-title"><h2 id="profile-th09-title">{t('th09.accountRecord')}</h2></div>
    {data && <>
      <dl className="profile-th09-summary">
        <div><dt>{t('th09.totalGames')}</dt><dd>{numbers.format(data.summary.games)}</dd></div>
        <div><dt>{t('th09.totalWins')}</dt><dd>{numbers.format(data.summary.wins)}</dd></div>
        <div><dt>{t('th09.winRate')}</dt><dd>{formatTh09WinRate(data.summary, locale) ?? '—'}</dd></div>
      </dl>
      <h3>{t('th09.duelHistory')}</h3>
      {data.duels.length ? <ul className="profile-th09-history">
        {data.duels.map(item => <li key={item.duelId}>
          {item.opponent ? <Link to={'/u/' + encodeURIComponent(item.opponent.username)}>
            {item.opponent.displayName}<small>@{item.opponent.username}</small>
          </Link> : <span>{t('th09.guestOpponent')}</span>}
          <strong role="img" aria-label={t('th09.score') + ' ' + item.wins + ' : ' + item.losses}>{numbers.format(item.wins)}:{numbers.format(item.losses)}</strong>
          <time dateTime={new Date(item.playedAt).toISOString()}>{dates.format(item.playedAt)}</time>
        </li>)}
      </ul> : <p className="profile-muted">{t('th09.noDuels')}</p>}
    </>}
    {failed ? <div className="profile-th09-actions" role="status">
      <span>{t('account.requestFailed')}</span>
      <Button variant="outline" size="sm" onClick={() => setRequest({...request})}>{t('action.retry')}</Button>
    </div> : data?.nextCursor && <div className="profile-th09-actions">
      <Button variant="outline" size="sm" disabled={pending} onClick={() => setRequest({cursor: data.nextCursor ?? null})}>{t('th09.moreDuels')}</Button>
    </div>}
  </section>;
}
