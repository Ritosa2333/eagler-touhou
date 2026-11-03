import type {ReactNode} from 'react';
import type {LibraryCardsProps} from './LibraryCards';
import {BookLibrary} from './BookLibrary';
import {useLocale} from '../../i18n';

export interface LibrarySurfaceProps extends Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'> {
  masthead: ReactNode;
  footer: ReactNode;
  serverStatusNote?: ReactNode;
  optionsOpen: boolean;
  roomOpen?: boolean;
  roomDrawer?: ReactNode;
  children?: ReactNode;
}
/** The page holds library content. Settings occupy the application overlay layer. */
export function LibrarySurface({masthead, footer, serverStatusNote, optionsOpen, roomOpen = false, roomDrawer, children, ...rail}: LibrarySurfaceProps) {
  const {t} = useLocale();
  return <><div className="library-background" aria-hidden="true"/><div className="sheet">
    {masthead}
    <main className={`main library-layout${roomOpen ? ' mp-room-open' : ''}`} id="main">
      <h1 className="visually-hidden">{t('site.documentTitle')}</h1>
      <div className="game-library" inert={optionsOpen}>
        <BookLibrary {...rail} footer={footer}/>
      </div>
      {children}
    </main>
    {roomDrawer}{serverStatusNote}
  </div></>;
}
