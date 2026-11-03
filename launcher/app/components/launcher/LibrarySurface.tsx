import type {ReactNode} from 'react';
import {OptionsBackdrop} from './OptionsBackdrop';
import {useLocale} from '../../i18n';
import {LibraryCards, type LibraryCardsProps} from './LibraryCards';
import {AppLink} from './AppLink';

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
  const {t} = useLocale();
  const Link = rail.interactive === false ? 'a' : AppLink;
  const singleplayer = rail.products.filter(product => !product.multiplayer);
  const visibleSingleplayer = singleplayer.filter(product => !product.hidden);
  const heroProduct = visibleSingleplayer.find(product => product.id === rail.selectedProduct) ?? visibleSingleplayer[0];
  return <><div className="library-background" aria-hidden="true"/><div className="sheet">
    {masthead}
    <main className={`main library-layout${roomOpen ? ' mp-room-open' : ''}`} id="main">
      <h1 className="visually-hidden">东方Project 原作 STG ~ EAGLER TOUHOU</h1>
      {heroProduct && <section className="library-hero" aria-labelledby="libraryHeroTitle">
        <div className="library-hero-copy">
          <span className="library-hero-eyebrow">No.{heroProduct.number} · ORIGINAL STG</span>
          <h2 id="libraryHeroTitle"><span>東方</span><strong>{heroProduct.title.startsWith('東方') ? heroProduct.title.slice(2) : heroProduct.title}</strong></h2>
          <p className="library-hero-subtitle">{heroProduct.subtitle}</p>
          <p className="library-hero-description">{t('library.singleplayerHint')}</p>
          <div className="library-hero-actions">
            <button className="library-hero-primary" type="button" onClick={() => rail.onActivate(heroProduct.id)}>{t('action.start')}<span aria-hidden="true">↗</span></button>
            <button className="library-hero-secondary" type="button" onClick={() => rail.onActivate(heroProduct.id)}>{t('lobby.gameOptions')}<span aria-hidden="true">⌘</span></button>
          </div>
        </div>
        <div className="library-hero-art" aria-hidden="true">
          {heroProduct.artwork && <img src={heroProduct.artwork} alt="" decoding="async"/>}
          <span className="library-hero-art-shade"/>
          <span className="library-hero-art-caption"><b>{heroProduct.number}</b><span>{heroProduct.title}<small>{heroProduct.subtitle}</small></span></span>
        </div>
      </section>}
      <div className="game-library" inert={optionsOpen}>
        <LibraryCards {...rail} products={singleplayer} variant="singleplayer"/>
        <LibraryCards {...rail} products={rail.products.filter(p => p.multiplayer)} variant="multiplayer" lobbyLink={<Link className="shelf-lobby-link" href={lobbyHref}><img src={roomUsersArtwork} alt="" width="18" height="18"/><span>{t('lobby.title')}</span></Link>}/>
      </div>
      <OptionsBackdrop onBack={onBack}/>
      {children}
    </main>
    {roomDrawer}{serverStatusNote}
  </div>{footer}</>;
}
