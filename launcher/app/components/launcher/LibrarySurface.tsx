import type {ReactNode} from 'react';
import type {LibraryCardsProps} from './LibraryCards';
import {BookLibrary} from './BookLibrary';
import {useLocale} from '../../i18n';

export interface LibrarySurfaceProps extends Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'> {
  masthead: ReactNode;
  active?: boolean;
  optionsOpen: boolean;
  compact?: boolean;
  mobileSearchTarget?: HTMLElement | null;
  tools?: ReactNode;
  detail?: ReactNode;
  onBrowse?: () => void;
  roomOpen?: boolean;
  roomDrawer?: ReactNode;
  children?: ReactNode;
}
/** The book houses inline details; compact and multiplayer overlays keep their own carriers. */
export function LibrarySurface({masthead, active = true, optionsOpen, roomOpen = false, roomDrawer, children, ...rail}: LibrarySurfaceProps) {
  const {t} = useLocale();
  return <><div className="library-background" aria-hidden="true"/><div className="sheet">
    {masthead}
    <main className={`main library-layout${roomOpen ? ' mp-room-open' : ''}`} id={active ? 'main' : undefined}>
      <h1 className="visually-hidden">{t('site.documentTitle')}</h1>
      <div className="game-library" inert={optionsOpen}>
        <BookLibrary {...rail}/>
      </div>
      {children}
    </main>
    {roomDrawer}
  </div></>;
}
