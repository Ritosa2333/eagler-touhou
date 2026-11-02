import {BookButton} from './BookButton';
import {useEffect, useState, type ReactNode} from 'react';
import {readBookPreference, writeBookPreference} from './book-preferences';
import { useLocale } from '../../i18n';
import type { LibraryCardsProps } from './LibraryCards';
import {BookDirectoryGames} from './BookDirectoryGames';

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
  connectionNote?: string;
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
  connectionNote,
  roomUsersArtwork,
  children,
  ...rail
}: LobbySurfaceProps) {
  const {t} = useLocale();
  const [roomView, setRoomView] = useState<'cards' | 'list'>('cards');
  useEffect(() => setRoomView(readBookPreference('lobby-room-view', 'cards', (value): value is 'cards' | 'list' => value === 'cards' || value === 'list')), []);
  return (
    <>
      <main className={`lobby-main book-directory book-rooms-${roomView}`} inert={rail.interactive === false}>
        {masthead}
        <header className="lobby-intro">
          <h1>{t('lobby.title')}</h1>
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
        <section className="lobby-library" aria-label={t('lobby.game')}>
          <BookDirectoryGames {...rail}/>
        </section>
        <div className="lobby-rooms-column">
        <header className="lobby-room-tools">
          <div className="lobby-actions">{roomTools}</div>
          <fieldset className="book-room-view-toggle" aria-label={t('directory.display')}>
            {(['cards', 'list'] as const).map(view => <BookButton variant="choice" key={view} type="button" aria-pressed={roomView === view} onClick={() => {setRoomView(view); writeBookPreference('lobby-room-view', view);}}>{view === 'cards' ? (t('directory.displayNormal')) : (t('directory.displayList'))}</BookButton>)}
          </fieldset>
        </header>
        {roomFilters}
        <section
          className="lobby-list-surface"
          aria-label={t('lobby.rooms')}
          data-has-rooms={hasRooms}
        >
          <div
            className="lobby-list-head"
            id="listHead"
            aria-hidden="true"
            hidden={!hasRooms}
          >
            {roomView === 'list' ? <>
            <span>{t('directory.columnGame')}</span>
            <span>{t('directory.columnRoom')}</span>
            <span>{t('lobby.difficulty')}</span>
            <span>{t('directory.columnRules')}</span>
            <span>{t('directory.columnPlayers')}</span>
            <span>{t('lobby.state')}</span>
            <span />
            </> : <>
            <span>{t('lobby.gameAndRoom')}</span>
            <span>{t('lobby.difficulty')}</span>
            <span>{t('directory.columnRules')}</span>
            <span />
            <span>{t('lobby.capacity')}</span>
            <span>{t('lobby.state')}</span>
            <span />
            </>}
          </div>
          <ul className="lobby-room-list" id="roomList" aria-busy={loading}>
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
        </section>
        <p className="lobby-notice" id="connectionNote" role="status" hidden={!connectionNote}>
          {connectionNote}
        </p>
        <p className="lobby-sr-only" id="roomCount" aria-live="polite">
          {roomCount}
        </p>
        </div>
      </main>
      {children}
    </>
  );
}
