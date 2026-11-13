import type {ReactNode} from 'react';
import type {LibraryCardsProps} from './LibraryCards';
import {BookLibrary} from './BookLibrary';
import {useLocale} from '../../i18n';
import {SiteFooter} from './SiteFooter';

export interface LibrarySurfaceProps extends Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'> {
  masthead: ReactNode;
  serverStatusNote?: ReactNode;
  optionsOpen: boolean;
  roomOpen?: boolean;
  roomDrawer?: ReactNode;
  children?: ReactNode;
  onDonation?: () => void;
}
/** The page holds library content. Settings occupy the application overlay layer. */
export function LibrarySurface({masthead, serverStatusNote, optionsOpen, roomOpen = false, roomDrawer, children, onDonation, ...rail}: LibrarySurfaceProps) {
  const {t} = useLocale();
  return <><div className="library-background" aria-hidden="true"/><div className="sheet">
    {masthead}
    <main className={`main library-layout${roomOpen ? ' mp-room-open' : ''}`} id="main">
      <h1 className="visually-hidden">{t('site.documentTitle')}</h1>
      <div className="game-library" inert={optionsOpen}>
        <BookLibrary {...rail}/>
      </div>
      {children}
    </main>
    <SiteFooter onDonation={onDonation}/>
    {roomDrawer}{serverStatusNote}
  </div></>;
}
