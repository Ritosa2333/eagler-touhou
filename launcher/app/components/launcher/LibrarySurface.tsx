import { Users } from 'lucide-react';
import type { ReactNode } from 'react';
import { useLocale } from '../../i18n';
import { AppLink } from './AppLink';
import { LibraryCards, type LibraryCardsProps } from './LibraryCards';
import { OptionsBackdrop } from './OptionsBackdrop';

export interface LibrarySurfaceProps extends Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'> {
  masthead: ReactNode;
  footer: ReactNode;
  serverStatusNote?: ReactNode;
  lobbyHref: string;
  roomUsersArtwork: string;
  optionsOpen: boolean;
  roomOpen?: boolean;
  roomDrawer?: ReactNode;
  onBack: () => void;
  children?: ReactNode;
}
/** OptionsPanel must be a direct child of .main.library-layout for main's CSS. */
export function LibrarySurface({
  masthead,
  footer,
  serverStatusNote,
  lobbyHref,
  roomUsersArtwork,
  optionsOpen,
  roomOpen = false,
  roomDrawer,
  onBack,
  children,
  ...rail
}: LibrarySurfaceProps) {
  const { t } = useLocale();
  const Link = rail.interactive === false ? 'a' : AppLink;
  return (
    <>
      {masthead}
      <div className="library-background" aria-hidden="true" />
      <div className="sheet">
        <main className={`main library-layout${roomOpen ? ' mp-room-open' : ''}`} id="main">
          <h1 className="visually-hidden">{t('site.documentTitle')}</h1>
          <div className="game-library" inert={optionsOpen}>
            <LibraryCards {...rail} products={rail.products.filter((p) => !p.multiplayer)} variant="singleplayer" />
            <LibraryCards
              {...rail}
              products={rail.products.filter((p) => p.multiplayer)}
              variant="multiplayer"
              lobbyLink={
                <Link className="shelf-lobby-link" href={lobbyHref}>
                  <Users size={18} aria-hidden="true" focusable="false" />
                  <span>{t('lobby.title')}</span>
                </Link>
              }
            />
          </div>
          <OptionsBackdrop onBack={onBack} />
          {children}
        </main>
        {roomDrawer}
        {serverStatusNote}
      </div>
      {footer}
    </>
  );
}
