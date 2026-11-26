import {useCallback, useEffect, useRef, useState, type ReactNode} from 'react';
import {useLocation} from 'react-router';
import type {GameId} from '../../../src/contracts/product-catalog.mts';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';
import {useLocale} from '../../i18n';
import {accountsEnabled} from '../../release-features';
import {useAccountSession} from '../../services/account-session';
import {listCloudSaves, type CloudSaveList, type CloudSaveSnapshot} from '../../services/cloud-saves';
import {Button} from '../../ui/button';
import {AppLink} from './AppLink';
import {ClearMatrix} from './ClearMatrix';
import {SpellCardArchive} from './SpellCardArchive';
import {useThkkdProgress, type ThkkdProgressView} from './use-thkkd-progress';

type Result = {owner: string; data: CloudSaveList} | {owner: string; failed: true};
type StatisticsView = {owner?: string} & (
  | {status: 'loading'}
  | {status: 'guest'}
  | {status: 'error'; retry(): void}
  | {status: 'ready'; save?: CloudSaveSnapshot});

/** One read for the open detail, shared across games and both presentations. */
export function useBookGameStatistics(game: GameId | undefined): StatisticsView | ThkkdProgressView | undefined {
  const local = useThkkdProgress(game === 'thkkd');
  const enabled = accountsEnabled && !!game && game !== 'thkkd';
  const {state: account, retry: retryAccount} = useAccountSession(accountsEnabled && !!game);
  const owner = account.status === 'member' ? account.user.username : undefined;
  const [result, setResult] = useState<Result>();
  const refresh = useRef<(() => void) | undefined>(undefined);
  const retry = useCallback(() => refresh.current?.(), []);
  useEffect(() => {
    if (!enabled || !owner) {
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
  }, [owner, enabled]);

  if (game === 'thkkd') return {...local, owner};
  if (!enabled) return;
  if (account.status === 'guest') return {status: 'guest'};
  if (account.status === 'error') return {status: 'error', retry: retryAccount};
  if (!owner || result?.owner !== owner) return {status: 'loading', owner};
  if ('failed' in result) return {status: 'error', retry, owner};
  const current = result.data.products.find(product => product.id === game && product.game === game && !product.multiplayer);
  const save = current && result.data.saves.find(entry =>
    entry.product === game && entry.statistics.game === game && entry.revision === current.revision);
  return {status: 'ready', save, owner};
}

export function BookGameStatistics({game, view}: {game: GameId; view?: StatisticsView | ThkkdProgressView}) {
  const {locale, t} = useLocale();
  const {pathname, search, hash} = useLocation();
  const save = view?.status === 'ready' ? view.save : undefined;
  const statistics = save?.statistics;
  const local = view?.status === 'local' ? view : undefined;
  const progress = local?.progress ?? statistics?.progress ?? [];
  let status: ReactNode;
  if (view?.status === 'loading') status = <p className="book-game-stats-message" role="status">{t('status.loading')}</p>;
  if (view?.status === 'guest') {
    const login = new URL(withUiLanguage('/login.html', locale), 'https://launcher.invalid');
    login.searchParams.set('next', pathname + search + hash);
    status = <div className="book-game-stats-message">
      <Button asChild variant="outline" size="sm"><AppLink href={login.pathname + login.search} rememberEntry>{t('account.login')}</AppLink></Button>
    </div>;
  }
  if (view?.status === 'error') status = <div className="book-game-stats-message">
    <p role="status">{t('cloudSave.statisticsFailed')}</p>
    <Button variant="outline" size="sm" onClick={view.retry}>{t('action.retry')}</Button>
  </div>;
  if (view?.status === 'ready' && !save) status = <p className="book-game-stats-message">{t('cloudSave.noSave')}</p>;
  if (local) status = <div className="book-game-stats-message">
    {local.loading ? <p role="status">{t('status.loading')}</p> : local.failed ? <p role="status">{t('cloudSave.statisticsFailed')}</p>
      : !local.progress ? <p>{t('thkkd.progress.noSave')}</p> : null}
    <Button variant="outline" size="sm" onClick={local.retry}>{t(local.failed ? 'action.retry' : 'cloudSave.refresh')}</Button>
  </div>;
  return <section className="book-game-detail-statistics book-game-stats" data-statistics-game={game} data-cloud-record={!!save} data-statistics-source={local ? 'local' : 'cloud'} aria-labelledby="bookGameStatistics">
    <div className="game-detail-section-heading">
      <h2 id="bookGameStatistics">{t('game.clearProgress')}</h2>
      {local && <span className="book-game-stats-source">{t('cloudSave.browserVersion')}</span>}
      {save && <span className="book-game-stats-source" title={new Intl.DateTimeFormat(locale, {dateStyle: 'medium', timeStyle: 'short'}).format(new Date(save.uploadedAt))}>
        {t('cloudSave.cloudUploaded')}
      </span>}
    </div>
    <ClearMatrix key={game} work={{game, progress}} oneCreditLabel={local ? t('thkkd.progress.oneCredit') : undefined} renderDetails={game === 'th06' || game === 'th07' ? (selection, label) =>
      <SpellCardArchive game={game} cards={statistics?.spellCards} archive={statistics?.spellArchive}
        selection={selection} label={label}/> : undefined}/>
    {status}
  </section>;
}
