import {BookButton} from './BookButton';
import type {ReactNode} from 'react';
import { useLocale } from '../../i18n';
import type { LibraryCardsProps } from './LibraryCards';
import {BookDirectoryGames} from './BookDirectoryGames';
import './directory-page.css';

export interface LobbySurfaceProps extends Omit<
  LibraryCardsProps,
  'variant' | 'lobbyLink' | 'inert'
> {
  masthead: ReactNode;
  onGuide: (() => void) | null;
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
  onGuide,
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
  const {t} = useLocale();
  return (
    <>
      <main className="lobby-main book-directory" inert={rail.interactive === false}>
        {masthead}
        <header className="lobby-intro">
          <h1 className="visually-hidden">{t('lobby.title')}</h1>
          <nav className="lobby-support-links" aria-label={t('multiplayerGuide.title')}>
            <BookButton variant="quiet" size="sm"
              className="lobby-support-link h-9 px-0 max-[780px]:h-11"
              id="lobbyGuideOpen"
              type="button"
              disabled={!onGuide}
              onClick={onGuide ?? undefined}
            >
              <span>{t('multiplayerGuide.action')}</span>
              <span aria-hidden="true">›</span>
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
            <BookButton variant="quiet" size="sm" className="lobby-support-link lobby-survey-link h-9 px-0 max-[780px]:h-11" asChild><a href="https://v.wjx.cn/vm/QqmTdwh.aspx#" target="_blank" rel="noopener noreferrer" title={t('lobby.surveyNotice')}>
              <span>{t('lobby.surveyAction')}</span><span aria-hidden="true">›</span>
            </a></BookButton>
          </nav>
        </header>
        {connectionWarning}
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
          <ul className="lobby-room-list" id="roomList" aria-busy={loading} hidden={!hasRooms}>
            {roomList}
          </ul>
          <div className="lobby-list-loading" id="listLoading" role="status" hidden={!loading}>
            <i className="lobby-preload-spinner" aria-hidden="true" />
            <span className="lobby-sr-only">{t('lobby.loadingHint')}</span>
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
        </div>
        </div>
      </main>
      {children}
    </>
  );
}
