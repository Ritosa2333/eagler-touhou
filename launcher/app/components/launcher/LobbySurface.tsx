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
  const { t, locale } = useLocale();
  const [roomView, setRoomView] = useState<'cards' | 'list'>('cards');
  useEffect(() => setRoomView(readBookPreference('lobby-room-view', 'cards', (value): value is 'cards' | 'list' => value === 'cards' || value === 'list')), []);
  return (
    <>
      <main className={`lobby-main book-directory book-rooms-${roomView}`} inert={rail.interactive === false}>
        {masthead}
        <aside className="lobby-survey-notice" aria-label={t('notice.label')}>
          <strong>{t('lobby.surveyNotice')}</strong>
          <a
            className="lobby-button lobby-primary"
            href="https://v.wjx.cn/vm/QqmTdwh.aspx#"
            target="_blank"
            rel="noopener noreferrer"
          >
            {t('lobby.surveyAction')}
          </a>
        </aside>
        <header className="lobby-intro">
          <h1>{t('lobby.title')}</h1>
          <nav className="lobby-support-links" aria-label={t('multiplayerGuide.title')}>
            <button
              className="lobby-support-link"
              id="lobbyGuideOpen"
              type="button"
              disabled={!onGuide}
              onClick={onGuide ?? undefined}
            >
              <span>{t('multiplayerGuide.action')}</span>
              <span aria-hidden="true">›</span>
            </button>
            <button
              className={`lobby-support-link${networkRunning ? ' running' : ''}`}
              id="lobbyNetworkCheck"
              aria-disabled={networkRunning ? 'true' : undefined}
              type="button"
              disabled={!onNetwork}
              onClick={onNetwork ?? undefined}
            >
              <span>{t('networkCheck.action')}</span>
              <span aria-hidden="true">›</span>
            </button>
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
        <header className="lobby-room-tools">
          <div className="lobby-actions">{roomTools}</div>
          <div className="book-room-view-toggle" role="group" aria-label={locale === 'en' ? 'Room display' : '房间显示模式'}>
            {(['cards', 'list'] as const).map(view => <button key={view} type="button" aria-pressed={roomView === view} onClick={() => {setRoomView(view); writeBookPreference('lobby-room-view', view);}}>{view === 'cards' ? (locale === 'en' ? 'Normal' : '普通模式') : (locale === 'en' ? 'List' : '列表模式')}</button>)}
          </div>
        </header>
        <section
          className="lobby-list-surface"
          aria-label={t('lobby.rooms')}
        >
          <div
            className="lobby-list-head"
            id="listHead"
            aria-hidden="true"
            hidden={!hasRooms}
          >
            {roomView === 'list' ? <>
            <span>{locale === 'en' ? 'Game' : '游戏'}</span>
            <span>{locale === 'en' ? 'Room' : '房间'}</span>
            <span>{t('lobby.difficulty')}</span>
            <span>{locale === 'en' ? 'Rules' : '玩法'}</span>
            <span>{locale === 'en' ? 'Players' : '玩家 / 人数'}</span>
            <span>{t('lobby.state')}</span>
            <span />
            </> : <>
            <span>{t('lobby.gameAndRoom')}</span>
            <span>{t('lobby.difficulty')}</span>
            <span>{locale === 'en' ? 'Rules' : '玩法'}</span>
            <span />
            <span>{t('lobby.capacity')}</span>
            <span>{t('lobby.state')}</span>
            <span />
            </>}
          </div>
          <div className="lobby-room-list" id="roomList" role="list" aria-busy={loading}>
            {roomList}
          </div>
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
                <button
                  className="lobby-button"
                  id="emptyAction"
                  type="button"
                  onClick={emptyState.action.onClick}
                >
                  {emptyState.action.label}
                </button>
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
      </main>
      {children}
    </>
  );
}
