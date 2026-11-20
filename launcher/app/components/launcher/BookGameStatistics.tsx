import {useCallback, useEffect, useRef, useState, type ReactNode} from 'react';
import {useLocation} from 'react-router';
import type {GameId} from '../../../src/contracts/product-catalog.mts';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {useLocale} from '../../i18n';
import {accountsEnabled} from '../../release-features';
import {useAccountSession} from '../../services/account-session';
import {formatPlaytime, listCloudSaves, type CloudSaveList, type CloudSaveSnapshot} from '../../services/cloud-saves';
import {Button} from '../../ui/button';
import {AppLink} from './AppLink';
import {ClearMatrix} from './ProfileGameData';

type Result = {owner: string; data: CloudSaveList} | {owner: string; failed: true};
type StatisticsView =
  | {status: 'loading'}
  | {status: 'guest'}
  | {status: 'error'; retry(): void}
  | {status: 'ready'; save?: CloudSaveSnapshot};

/** One read for the open detail, shared across games and both presentations. */
export function useBookGameStatistics(game: GameId | undefined): StatisticsView | undefined {
  const enabled = accountsEnabled && !!game;
  const {state: account, retry: retryAccount} = useAccountSession(enabled);
  const owner = account.status === 'member' ? account.user.username : undefined;
  const [result, setResult] = useState<Result>();
  const refresh = useRef<(() => void) | undefined>(undefined);
  const retry = useCallback(() => refresh.current?.(), []);
  useEffect(() => {
    if (!owner) {
      setResult(undefined);
      return;
    }
    let pending: AbortController | undefined;
    const load = () => {
      pending?.abort();
      const controller = new AbortController();
      pending = controller;
      setResult(previous => previous?.owner === owner && 'data' in previous ? previous : undefined);
      void listCloudSaves(controller.signal).then(data => {
        if (!controller.signal.aborted) setResult({owner, data});
      }, () => {
        if (!controller.signal.aborted) setResult({owner, failed: true});
      });
    };
    refresh.current = load;
    load();
    window.addEventListener('cloud-save-change', load);
    return () => {
      refresh.current = undefined;
      pending?.abort();
      window.removeEventListener('cloud-save-change', load);
    };
  }, [owner]);

  if (!enabled) return;
  if (account.status === 'guest') return {status: 'guest'};
  if (account.status === 'error') return {status: 'error', retry: retryAccount};
  if (!owner || result?.owner !== owner) return {status: 'loading'};
  if ('failed' in result) return {status: 'error', retry};
  const current = result.data.products.find(product => product.id === game && product.game === game && !product.multiplayer);
  const save = current && result.data.saves.find(entry =>
    entry.product === game && entry.statistics.game === game && entry.revision === current.revision);
  return {status: 'ready', save};
}

export function BookGameStatistics({game, view}: {game: GameId; view?: StatisticsView}) {
  const {locale, t} = useLocale();
  const {pathname, search, hash} = useLocation();
  const save = view?.status === 'ready' ? view.save : undefined;
  const statistics = save?.statistics;
  const progress = statistics?.progress ?? [];
  const numbers = new Intl.NumberFormat(locale);
  const unknown = t('cloudSave.unknownValue');
  const scores = progress.flatMap(row => row.bestScore === null ? [] : [row.bestScore]);
  const clear = [...new Set(progress.filter(row => row.cleared || row.oneCreditClear).map(row => row.difficulty))]
    .map(difficulty => difficulty.charAt(0).toUpperCase() + difficulty.slice(1));

  let status: ReactNode;
  if (view?.status === 'loading') status = <p className="book-game-stats-message" role="status">{t('status.loading')}</p>;
  if (view?.status === 'guest') {
    const login = new URL(withUiLanguage('/login.html', locale), 'https://launcher.invalid');
    login.searchParams.set('next', pathname + search + hash);
    status = <div className="book-game-stats-message">
      <Button asChild variant="outline" size="sm"><AppLink href={login.pathname + login.search}>{t('account.login')}</AppLink></Button>
    </div>;
  }
  if (view?.status === 'error') status = <div className="book-game-stats-message">
    <p role="status">{t('cloudSave.statisticsFailed')}</p>
    <Button variant="outline" size="sm" onClick={view.retry}>{t('action.retry')}</Button>
  </div>;
  if (view?.status === 'ready' && !save) status = <p className="book-game-stats-message">{t('cloudSave.noSave')}</p>;
  return <div className="book-game-stats" data-statistics-game={game} data-cloud-record={!!save}>
    <dl className="book-game-stats-grid">
      <div><dt>{t('cloudSave.duration')}</dt><dd data-stat="duration">{statistics?.playtimeSeconds == null ? unknown : formatPlaytime(statistics.playtimeSeconds, locale)}</dd></div>
      <div><dt>{t('cloudSave.playCount')}</dt><dd data-stat="play-count">{statistics?.playCount == null ? unknown : numbers.format(statistics.playCount)}</dd></div>
      <div><dt>{t('cloudSave.bestScore')}</dt><dd data-stat="best-score">{scores.length ? numbers.format(Math.max(...scores)) : unknown}</dd></div>
      <div><dt>{t('cloudSave.clearDifficulties')}</dt><dd data-stat="clears">{clear.length ? clear.join(' · ') :
        progress.some(row => row.cleared === false || row.oneCreditClear === false) ? t('profile.clear.uncleared') : unknown}</dd></div>
    </dl>
    <ClearMatrix key={game} game={game} statistics={statistics}/>
    {save && <p className="book-game-stats-source">{t('cloudSave.cloudUploaded')} <time dateTime={save.uploadedAt}>
      {new Intl.DateTimeFormat(locale, {dateStyle: 'medium', timeStyle: 'short'}).format(new Date(save.uploadedAt))}
    </time></p>}
    {status}
  </div>;
}
