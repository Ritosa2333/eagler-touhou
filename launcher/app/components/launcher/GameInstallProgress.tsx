import {Download, X} from 'lucide-react';
import {useMemo, useSyncExternalStore, type ReactNode} from 'react';
import {PRODUCT_GAMES, type GameId} from '../../../src/contracts/product-catalog.mts';
import {localeHome} from '../../../src/launcher/i18n.mts';
import {useLocale} from '../../i18n';
import {gameDownloadProgress, type GameDownloads} from '../../models/game-downloads';
import {transferClock} from '../../models/transfer';
import {shallowEqual, useStoreSelector} from '../../shared/react-store';
import {Button} from '../../ui/button';
import {LoadingActivity} from '../../ui/loading-activity';
import {Progress} from '../../ui/progress';
import {AppLink} from './AppLink';
import {bookGameArtwork} from './book-artwork';
import {LauncherBack} from './LauncherBack';
import {launcherSectionHref, useLauncherSection} from './launcher-sections';
import './game-install-progress.css';

type InstallProps = {downloads: GameDownloads; assetUrl(path: string): string};

function formatBytes(bytes: number, numbers: Intl.NumberFormat) {
  const mib = bytes >= 1048576;
  const amount = numbers.format(mib ? bytes / 1048576 : Math.round(bytes / 1024));
  return `${amount} ${mib ? 'MiB' : 'KiB'}`;
}

const shortcutState = (value: ReturnType<GameDownloads['getSnapshot']>) => ({
  active: value.active !== null, pending: value.pending.length, percent: gameDownloadProgress(value).percent,
});

const activeInstallPercent = (value: ReturnType<GameDownloads['getSnapshot']>) =>
  value.active ? gameDownloadProgress(value).percent : null;

export function GameInstallSidebarProgress({downloads}: Pick<InstallProps, 'downloads'>) {
  const percent = useStoreSelector(downloads, activeInstallPercent);
  return percent === null ? null : <span className="book-sidebar-footer-progress">{percent}%</span>;
}

export function GameInstallShortcut({downloads, assetUrl}: InstallProps) {
  const section = useLauncherSection(), {locale, t} = useLocale();
  const {active, pending, percent} = useStoreSelector(downloads, shortcutState, shallowEqual);
  if (!active || section === 'installs') return null;
  return <Button asChild className="game-install-shortcut" variant="outline" size="lg">
    <AppLink href={launcherSectionHref(assetUrl(localeHome(locale)), 'installs')} rememberEntry
      title={t('downloadTasks.title')} aria-label={t('downloadTasks.title')}>
      <Download aria-hidden="true"/>
      <span aria-hidden="true">{percent === null ? t('downloadTasks.downloading') : `${percent}%`}</span>
      {pending > 0 && <span className="game-install-count" aria-hidden="true">+{pending}</span>}
    </AppLink>
  </Button>;
}

function CancelInstall({downloads, game}: {downloads: GameDownloads; game: GameId}) {
  const {t} = useLocale(), label = t('action.cancel');
  return <Button variant="ghost" size="icon" title={label}
    aria-label={`${label} · ${PRODUCT_GAMES[game].title}`} onClick={() => downloads.cancel(game)}>
    <X aria-hidden="true"/>
  </Button>;
}

const statusKeys = {
  preparing: 'transfer.preparingShort',
  waiting: 'transfer.waitingServer',
  receiving: 'downloadTasks.downloading',
  saving: 'downloadTasks.saving',
} as const;

function ActiveInstall({downloads, assetUrl, gameId, progress}: InstallProps & {
  gameId: GameId; progress: ReturnType<typeof gameDownloadProgress>;
}) {
  const {locale, t} = useLocale();
  const numbers = useMemo(() => new Intl.NumberFormat(locale, {maximumFractionDigits: 1}), [locale]);
  const game = PRODUCT_GAMES[gameId];
  const showScene = progress.percent === null && (progress.phase === 'preparing' || progress.phase === 'waiting');
  const status = t(statusKeys[progress.phase]);
  return <section className="game-install-active" data-download-active={gameId} aria-labelledby="game-install-active-title">
    <img className="game-install-cover" src={bookGameArtwork(gameId)} alt=""/>
    <div className="game-install-identity">
      <span className="book-work-number">{gameId}</span>
      <h3 id="game-install-active-title">{game.title}</h3>
      <p>{game.subtitle}</p>
    </div>
    <CancelInstall downloads={downloads} game={gameId}/>
    <div className="game-install-meter">
      {!showScene && <div className="game-install-meter-heading">
        <span data-install-phase={progress.phase}>{status}</span>
        {progress.percent !== null && <strong>{progress.percent}%</strong>}
      </div>}
      <Progress value={progress.percent} smooth aria-label={game.title}/>
      {(progress.total !== null || progress.loaded > 0 || progress.speed !== null) && <div className="game-install-meta">
        {(progress.total !== null || progress.loaded > 0) && <span>{formatBytes(progress.loaded, numbers)}
          {progress.total !== null && ` / ${formatBytes(progress.total, numbers)}`}</span>}
        {progress.speed !== null && <span data-install-speed>{formatBytes(progress.speed, numbers)}/s</span>}
        {progress.eta !== null && <span data-install-eta>{t('downloadTasks.eta', {time: transferClock(progress.eta)})}</span>}
      </div>}
    </div>
    {showScene && <div className="game-install-wait">
      <LoadingActivity assetUrl={assetUrl} context="install" compact status={status}/>
    </div>}
  </section>;
}

function InstallTasks({downloads, assetUrl}: InstallProps) {
  const snapshot = useSyncExternalStore(downloads.subscribe, downloads.getSnapshot, downloads.getSnapshot);
  const {t} = useLocale();
  const {active, pending, failed} = snapshot;
  return <section className="game-install-tasks" aria-labelledby="game-install-tasks-heading">
    <h2 id="game-install-tasks-heading">{t('downloadTasks.title')}</h2>
    {active && <ActiveInstall key={active.game} downloads={downloads} assetUrl={assetUrl}
      gameId={active.game} progress={gameDownloadProgress(snapshot)}/>}
    {pending.length > 0 && <section className="game-install-group" aria-labelledby="game-install-queued-heading">
      <h3 id="game-install-queued-heading">{t('downloadTasks.queued')}</h3>
      <ol className="game-install-list">
        {pending.map(({game}, index) => <li key={game} data-download-queued={game}>
          <span className="game-install-order" aria-hidden="true">{index + 1}</span>
          <img className="game-install-cover" src={bookGameArtwork(game)} alt=""/>
          <strong>{PRODUCT_GAMES[game].title}</strong>
          <CancelInstall downloads={downloads} game={game}/>
        </li>)}
      </ol>
    </section>}
    {failed.length > 0 && <section className="game-install-group" aria-labelledby="game-install-failed-heading">
      <h3 id="game-install-failed-heading">{t('downloadTasks.failed')}</h3>
      <ul className="game-install-list">
        {failed.map(({game}) => <li key={game} data-download-failed={game}>
          <img className="game-install-cover" src={bookGameArtwork(game)} alt=""/>
          <strong>{PRODUCT_GAMES[game].title}</strong>
          <Button variant="outline" size="sm" onClick={() => downloads.retry(game)}>{t('action.retry')}</Button>
        </li>)}
      </ul>
    </section>}
    {!active && failed.length === 0 && <div className="game-install-empty" role="status">
      <Download aria-hidden="true"/><span>{t('downloadTasks.empty')}</span>
    </div>}
  </section>;
}

export function GameInstallPage({downloads, assetUrl, children}: InstallProps & {children: ReactNode}) {
  const {locale, t} = useLocale();
  return <main id="main" className="game-install-page" aria-labelledby="game-install-heading">
    <header className="game-install-header">
      <LauncherBack href={assetUrl(localeHome(locale))} returnToEntry replace/>
      <h1 id="game-install-heading">{t('downloadTasks.pageTitle')}</h1>
    </header>
    <InstallTasks downloads={downloads} assetUrl={assetUrl}/>
    <section className="game-install-group" aria-labelledby="game-install-add-ons-heading">
      <h2 id="game-install-add-ons-heading">{t('downloadTasks.addOns')}</h2>
      {children}
    </section>
  </main>;
}
