import type {ReactNode} from 'react';
import type {LibraryCardsProps} from './LibraryCards';
import {BookLibrary} from './BookLibrary';
import {useLocale} from '../../i18n';
import {SiteFooter} from './SiteFooter';

export interface LibrarySurfaceProps extends Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'> {
  masthead: ReactNode;
  optionsOpen: boolean;
  detail?: ReactNode;
  onBrowse?: () => void;
  roomOpen?: boolean;
  roomDrawer?: ReactNode;
  children?: ReactNode;
  onDonation?: () => void;
}
/** The book houses inline details; compact and multiplayer overlays keep their own carriers. */
export function LibrarySurface({masthead, optionsOpen, roomOpen = false, roomDrawer, children, onDonation, ...rail}: LibrarySurfaceProps) {
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
    {roomDrawer}
  </div></>;
}
