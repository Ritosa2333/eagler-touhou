import {BookButton} from './BookButton';
import type {ReactNode} from 'react';
import { useLocale } from '../../i18n';
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
  rankingsHref: string;
  onNetwork: (() => void) | null;
  networkRunning?: boolean;
  connectionWarning?: ReactNode;
  membership?: ReactNode;
  notice?: string;
  roomTools: ReactNode;
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
  roomFilters,
  roomList,
  hasRooms,
  loading,
  emptyState,
  roomCount,
  roomUsersArtwork,
  children,
  ...rail
}: LobbySurfaceProps) {
  const {t, locale} = useLocale();
  return (
    <>
      <main className="lobby-main book-directory" inert={rail.interactive === false}>
        {masthead}
        <header className="lobby-intro">
          <h1 className="visually-hidden">{t('lobby.title')}</h1>
          <nav className="lobby-support-links" aria-label={t('multiplayerGuide.title')}>
            <BookButton variant="quiet" size="sm" asChild={!!guideHref}
              className="lobby-support-link h-9 px-0 max-[780px]:h-11"
              id="lobbyGuideOpen"
              disabled={guideHref ? undefined : true}
            >
              {guideHref ? <a href={guideHref}><span>{t('multiplayerGuide.action')}</span><span aria-hidden="true">›</span></a>
                : <span>{t('multiplayerGuide.action')}</span>}
            </BookButton>
            <BookButton variant="quiet" size="sm"
              className={`lobby-support-link h-9 px-0 max-[780px]:h-11${networkRunning ? ' running' : ''}`}
              id="lobbyNetworkCheck"
              aria-disabled={networkRunning ? 'true' : undefined}
              type="button"
              disabled={!onNetwork}
              onClick={onNetwork ?? undefined}
            >
              <span>{t('networkCheck.action')}</span>
              <span aria-hidden="true">›</span>
            </BookButton>
            {accountsEnabled && <BookButton variant="quiet" size="sm" asChild
              className="lobby-support-link h-9 px-0 max-[780px]:h-11">
              <a href={withUiLanguage(rankingsHref, locale)}>
                <span>{t('th09.ratingBoard')}</span><span aria-hidden="true">›</span>
              </a>
            </BookButton>}
          </nav>
        </header>
        <p className="lobby-notice" id="notice" role="status" hidden={!notice}>
          {notice}
        </p>
        {membership}
        <div className="lobby-directory-body">
        <section className="lobby-library" aria-label={t('lobby.game')}>
          <BookDirectoryGames {...rail}/>
        </section>
        <div className="lobby-rooms-column">
        <header className="lobby-room-tools">
          {roomFilters}
          <div className="lobby-actions">{roomTools}</div>
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
          <div className="visually-hidden" id="listLoading" role="status" hidden={!loading}>{t('lobby.loadingHint')}</div>
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
        </div>
        </div>
      </main>
      {children}
    </>
  );
}
