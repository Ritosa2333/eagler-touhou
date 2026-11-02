import type {ReactNode} from 'react';
import {OptionsBackdrop} from './OptionsBackdrop';
import {useLocale} from '../../i18n';
import {LibraryCards, type LibraryCardsProps} from './LibraryCards';
import {AppLink} from './AppLink';
import {ClassicsShelf} from './ClassicsShelf';
import {createLibraryProducts} from './products';

export interface LibrarySurfaceProps extends Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'> {
  masthead: ReactNode;
  footer: ReactNode;
  serverStatusNote?: ReactNode;
  lobbyHref: string;
  roomUsersArtwork: string;
  optionsOpen: boolean;
  roomOpen?: boolean;
  roomDrawer?: ReactNode;
  classicsAssetUrl?: (path: string) => string;
  onBack: () => void;
  children?: ReactNode;
}
/** OptionsPanel must be a direct child of .main.library-layout for main's CSS. */
export function LibrarySurface({masthead, footer, serverStatusNote, lobbyHref, roomUsersArtwork, optionsOpen, roomOpen = false, roomDrawer, classicsAssetUrl, onBack, children, ...rail}: LibrarySurfaceProps) {
  const {t} = useLocale();
  const Link = rail.interactive === false ? 'a' : AppLink;
  const th05AssetUrl=classicsAssetUrl??((path:string)=>roomUsersArtwork.replace(/assets\/room-users\.svg(?:\?.*)?$/,path));
  const standalone = classicsAssetUrl || rail.products.some(p=>p.id==='th05') ? [{...createLibraryProducts(['th05'], th05AssetUrl, () => th05AssetUrl('th05'))[0]!, id: 'th05-standalone', title: t('game.th05Legacy'), multiplayer: true, external: true as const}] : [];
  return <><div className="library-background" aria-hidden="true"/><div className="sheet">
    {masthead}
    <main className={`main library-layout${roomOpen ? ' mp-room-open' : ''}`} id="main">
      <h1 className="visually-hidden">东方Project 原作 STG ~ EAGLER TOUHOU</h1>
      <div className="game-library" inert={optionsOpen}>
        <LibraryCards {...rail} products={rail.products.filter(p => !p.multiplayer)} variant="singleplayer"/>
        {classicsAssetUrl && <ClassicsShelf assetUrl={classicsAssetUrl}/>}
        <LibraryCards {...rail} products={rail.products.filter(p => p.multiplayer)} externalProducts={standalone} variant="multiplayer" lobbyLink={<Link className="shelf-lobby-link" href={lobbyHref}><img src={roomUsersArtwork} alt="" width="18" height="18"/><span>{t('lobby.title')}</span></Link>}/>
      </div>
      <OptionsBackdrop onBack={onBack}/>
      {children}
    </main>
    {roomDrawer}{serverStatusNote}
  </div>{footer}</>;
}
