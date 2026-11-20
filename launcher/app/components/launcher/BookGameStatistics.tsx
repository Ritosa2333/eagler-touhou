import {useCallback, useEffect, useRef, useState, type ReactNode} from 'react';
import {useLocation} from 'react-router';
import type {GameId} from '../../../src/contracts/product-catalog.mts';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {useLocale} from '../../i18n';
import {accountsEnabled} from '../../release-features';
import {useAccountSession} from '../../services/account-session';
import {formatPlaytime, listCloudSaves, type CloudSaveList, type CloudSaveSnapshot, type SaveProgress} from '../../services/cloud-saves';
import {Button} from '../../ui/button';
import {AppLink} from './AppLink';

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

const baseDifficulties = ['easy', 'normal', 'hard', 'lunatic'] as const;
const withoutExtra: ReadonlySet<GameId> = new Set(['th01', 'th03', 'th075', 'th19']);
const difficultyName = (difficulty: string) => difficulty.charAt(0).toUpperCase() + difficulty.slice(1);
const clearLabels = {unread: 'profile.clear.unread', uncleared: 'profile.clear.uncleared',
  cleared: 'profile.clear.cleared', 'one-credit': 'profile.clear.oneCredit'} as const;

export function BookGameStatistics({game, view}: {game: GameId; view?: StatisticsView}) {
  const {locale, t} = useLocale();
  const {pathname, search, hash} = useLocation();
  const save = view?.status === 'ready' ? view.save : undefined;
  const statistics = save?.statistics;
  const progress = statistics?.progress ?? [];
  const numbers = new Intl.NumberFormat(locale);
  const unknown = t('cloudSave.unknownValue');
  const difficulties = new Set<SaveProgress['difficulty']>(baseDifficulties);
  if (!withoutExtra.has(game)) difficulties.add('extra');
  if (game === 'th07') difficulties.add('phantasm');
  if (game === 'th20') difficulties.add('overdrive');
  for (const row of progress) difficulties.add(row.difficulty);
  const rows = [...difficulties].map(difficulty => {
    const records = progress.filter(row => row.difficulty === difficulty);
    const scores = records.flatMap(row => row.bestScore === null ? [] : [row.bestScore]);
    const state: keyof typeof clearLabels = records.some(row => row.oneCreditClear === true) ? 'one-credit'
      : records.some(row => row.cleared === true) ? 'cleared'
      : records.some(row => row.cleared === false || row.oneCreditClear === false) ? 'uncleared'
      : 'unread';
    return {difficulty, state, score: scores.length ? Math.max(...scores) : null};
  });
  const scores = rows.flatMap(row => row.score === null ? [] : [row.score]);
  const clear = rows.filter(row => row.state === 'one-credit' || row.state === 'cleared').map(row => difficultyName(row.difficulty));

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
        rows.some(row => row.state === 'uncleared') ? t('profile.clear.uncleared') : unknown}</dd></div>
    </dl>
    <table className="book-game-progress">
      <caption className="sr-only">{t('profile.clearChart')}</caption>
      <thead><tr><th scope="col">{t('lobby.difficulty')}</th><th scope="col">{t('profile.clear.cleared')}</th>
        <th scope="col">{t('cloudSave.bestScore')}</th></tr></thead>
      <tbody>{rows.map(({difficulty, state, score}) => <tr key={difficulty} data-difficulty={difficulty}>
        <th scope="row">{difficultyName(difficulty)}</th>
        <td><span className="book-game-clear-cell" data-clear={state} aria-hidden="true" />
          <span className={state === 'unread' ? 'sr-only' : undefined}>{t(clearLabels[state])}</span></td>
        <td>{score === null ? <><span aria-hidden="true">—</span><span className="sr-only">{unknown}</span></> : numbers.format(score)}</td>
      </tr>)}</tbody>
    </table>
    {save && <p className="book-game-stats-source">{t('cloudSave.cloudUploaded')} <time dateTime={save.uploadedAt}>
      {new Intl.DateTimeFormat(locale, {dateStyle: 'medium', timeStyle: 'short'}).format(new Date(save.uploadedAt))}
    </time></p>}
    {status}
  </div>;
}
