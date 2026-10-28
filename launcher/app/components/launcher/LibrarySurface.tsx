import type {ReactNode} from 'react';
import {OptionsBackdrop} from './OptionsBackdrop';
import type {LibraryCardsProps} from './LibraryCards';
import {BookLibrary} from './BookLibrary';

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
export function LibrarySurface({masthead, footer, serverStatusNote, lobbyHref, roomUsersArtwork, optionsOpen, roomOpen = false, roomDrawer, onBack, children, ...rail}: LibrarySurfaceProps) {
  return <><div className="library-background" aria-hidden="true"/><div className="sheet">
    {masthead}
    <main className={`main library-layout${roomOpen ? ' mp-room-open' : ''}`} id="main">
      <h1 className="visually-hidden">东方Project 原作 STG ~ EAGLER TOUHOU</h1>
      <div className="game-library" inert={optionsOpen}>
        <BookLibrary {...rail}/>
      </div>
      <OptionsBackdrop onBack={onBack}/>
      {children}
    </main>
    {roomDrawer}{serverStatusNote}
    {footer}
  </div></>;
}
