import {BookButton} from './BookButton';
import type {ReactNode} from 'react';
import {Activity, BookOpen, Clapperboard, Ellipsis, Trophy} from 'lucide-react';
import { useLocale } from '../../i18n';
import {ActionMenu, ActionMenuItem} from '../../ui/action-menu';
import {LoadingActivity} from '../../ui/loading-activity';
import type { LibraryCardsProps } from './LibraryCards';
import {BookDirectoryGames} from './BookDirectoryGames';
import {accountsEnabled} from '../../release-features';
import {withUiLanguage} from '../../../src/launcher/i18n.mts';

export interface LobbySurfaceProps extends Omit<
  LibraryCardsProps,
  'variant' | 'lobbyLink' | 'inert'
> {
  masthead: ReactNode;
  guideHref: string | null;
  rankingsHref?: string;
  onNetwork: (() => void) | null;
  networkRunning?: boolean;
  connectionWarning?: ReactNode;
  membership?: ReactNode;
  notice?: string;
  roomTools: ReactNode;
  onReplay?(): void;
  roomFilters?: ReactNode;
  roomList: ReactNode;
  hasRooms: boolean;
  loading: boolean;
  emptyState: {
    title: string;
    hint: string;
    action?: { label: string; onClick: () => void };
  } | null;
  roomCount: string;
  roomUsersArtwork: string;
  assetUrl?(path: string): string;
  children?: ReactNode;
  productFilters?: readonly string[];
  viewingProduct?: string;
  onProductFiltersChange?(products: string[]): void;
}
/** Dedicated lobby skeleton from main lobby.html43–105, without home brand/footer. */
export function LobbySurface({
  masthead,
  guideHref,
  rankingsHref,
  onNetwork,
  networkRunning = false,
  connectionWarning,
  membership,
  notice,
  roomTools,
  onReplay,
  roomFilters,
  roomList,
  hasRooms,
  loading,
  emptyState,
  roomCount,
  roomUsersArtwork,
  assetUrl = path => path,
  children,
  ...rail
}: LobbySurfaceProps) {
  const {t, locale} = useLocale();
  const ratings = accountsEnabled && rankingsHref ? withUiLanguage(rankingsHref, locale) : undefined;
  const more = t('nav.more');
  const showLoading = loading && !hasRooms && rail.interactive !== false;
  return (
    <>
      <main className="lobby-main book-directory" inert={rail.interactive === false}>
        {masthead}
        <div className="lobby-directory-body">
        <BookDirectoryGames {...rail} renderContent={gameBar => <div className="lobby-rooms-column">
        <h1 className="visually-hidden">{t('lobby.title')}</h1>
        <p className="lobby-notice" id="notice" role="status" hidden={!notice}>
          {notice}
        </p>
        {membership}
        {gameBar}
        <header className="lobby-room-tools">
          <div className="lobby-filter-tools">
            {roomFilters}
            <nav className="lobby-support-links" aria-label={t('multiplayerGuide.title')}>
            <BookButton variant="toolbar" size="icon" asChild={!!guideHref}
              id="lobbyGuideOpen"
              aria-label={t('multiplayerGuide.action')}
              title={t('multiplayerGuide.action')}
              disabled={guideHref ? undefined : true}
            >
              {guideHref ? <a href={guideHref}><BookOpen aria-hidden="true"/></a> : <BookOpen aria-hidden="true"/>}
            </BookButton>
            <BookButton variant="toolbar" size="icon" className="aria-disabled:cursor-progress aria-disabled:opacity-60"
              id="lobbyNetworkCheck"
              aria-label={t('networkCheck.action')}
              title={t('networkCheck.action')}
              aria-disabled={networkRunning ? 'true' : undefined}
              type="button"
              disabled={!onNetwork}
              onClick={onNetwork ?? undefined}
            >
              <Activity aria-hidden="true"/>
            </BookButton>
            {ratings && <BookButton variant="toolbar" size="icon" asChild>
              <a href={ratings} aria-label={t('th09.ratingBoard')} title={t('th09.ratingBoard')}>
                <Trophy aria-hidden="true"/>
              </a>
            </BookButton>}
            </nav>
          </div>
          <div className="lobby-actions">
            {roomTools}
            <div className="lobby-tools-overflow">
            <ActionMenu label={more} className="lobby-tools-menu" trigger={
              <BookButton variant="action" size="icon" className="lobby-button lobby-tools-more"
                type="button" aria-label={more} title={more}><Ellipsis aria-hidden="true"/></BookButton>
            }>
              {guideHref ? <ActionMenuItem asChild><a href={guideHref}><BookOpen aria-hidden="true"/>{t('multiplayerGuide.action')}</a></ActionMenuItem>
                : <ActionMenuItem disabled><BookOpen aria-hidden="true"/>{t('multiplayerGuide.action')}</ActionMenuItem>}
              <ActionMenuItem disabled={!onNetwork || networkRunning} onAction={onNetwork ?? undefined}>
                <Activity aria-hidden="true"/>{t('networkCheck.action')}
              </ActionMenuItem>
              {ratings && <ActionMenuItem asChild><a href={ratings}><Trophy aria-hidden="true"/>{t('th09.ratingBoard')}</a></ActionMenuItem>}
              {onReplay && <ActionMenuItem onAction={onReplay}><Clapperboard aria-hidden="true"/>{t('lobby.watchReplay')}</ActionMenuItem>}
            </ActionMenu>
            </div>
          </div>
        </header>
        <section
          className="lobby-list-surface"
          aria-label={t('lobby.rooms')}
          data-has-rooms={hasRooms}
        >
          {connectionWarning}
          <ul className="lobby-room-list" id="roomList" aria-busy={loading} hidden={!hasRooms}>
            {roomList}
          </ul>
          <div className={showLoading ? 'lobby-empty' : 'visually-hidden'} id="listLoading" role="status" hidden={!loading}>
            {showLoading ? <LoadingActivity assetUrl={assetUrl} status={t('lobby.loadingHint')} context="multiplayer"/> : t('lobby.loadingHint')}
          </div>
          {emptyState && (
            <div className="lobby-empty" id="emptyState">
              <img src={roomUsersArtwork} alt="" width="36" height="36" />
              <h2 id="emptyTitle">{emptyState.title}</h2>
              <p id="emptyHint">{emptyState.hint}</p>
              {emptyState.action && (
                <BookButton variant="action"
                  className="lobby-button"
                  id="emptyAction"
                  type="button"
                  onClick={emptyState.action.onClick}
                >
                  {emptyState.action.label}
                </BookButton>
              )}
            </div>
          )}
          <p className="lobby-directory-count" id="roomCount" aria-live="polite" hidden={!hasRooms}>{roomCount}</p>
        </section>
        </div>}/>
        </div>
      </main>
      {children}
    </>
  );
}
