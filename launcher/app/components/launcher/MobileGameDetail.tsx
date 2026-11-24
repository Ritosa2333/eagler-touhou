import {ArrowLeft, Settings2} from 'lucide-react';
import {useLayoutEffect, useRef, type ReactNode} from 'react';
import {useLocale} from '../../i18n';
import {Button} from '../../ui/button';
import {useContentMotion} from '../../ui/use-surface-motion';
import {GameDetailFavorite, GameDetailIdentity, GameDetailMetrics} from './GameDetailChrome';
import type {LibraryProduct} from './products';
import './mobile-game-detail.css';

export function MobileGameDetail({product, cover, actions, statistics, friends, playtime, saved, settings, setSettings, onBack, children}: {
  product: LibraryProduct; cover?: string | null; actions?: ReactNode; statistics?: ReactNode;
  friends?: ReactNode; playtime?: number | null; saved: boolean; settings: boolean;
  setSettings(open: boolean): void; onBack(): void; children?: ReactNode;
}) {
  const {t} = useLocale();
  const detailMotion = useContentMotion<HTMLDivElement>(settings ? 'settings' : 'detail');
  const settingsButton = useRef<HTMLButtonElement>(null), backButton = useRef<HTMLButtonElement>(null);
  const previous = useRef(settings);
  useLayoutEffect(() => {
    if (previous.current === settings) return;
    previous.current = settings;
    (settings ? backButton.current : settingsButton.current)?.focus({preventScroll:true});
  }, [settings]);
  return <div ref={detailMotion} className="mobile-game-pages">
    <div className="options-scroll mobile-game-view" hidden={settings} inert={settings}>
      <header className="game-detail-hero mobile-game-hero">
        {cover && <img className="game-detail-backdrop" src={cover} alt="" aria-hidden="true" decoding="async"/>}
        <div className="mobile-game-toolbar">
          <Button variant="ghost" size="icon" className="mobile-game-back" id={settings ? undefined : 'libraryBack'}
            onClick={onBack} aria-label={t('library.back')}><ArrowLeft aria-hidden="true"/></Button>
          <GameDetailFavorite productId={product.id}/>
        </div>
        <GameDetailIdentity product={product} cover={cover} titleId={settings ? undefined : 'gameTitle'}>
          <GameDetailMetrics productId={product.id} playtime={playtime} saved={saved}/>
        </GameDetailIdentity>
        <div className="mobile-game-actions">
          {actions}
          <Button variant="outline" className="mobile-game-settings-button" id={settings ? undefined : 'gameSettingsOpen'}
            ref={settingsButton} onClick={() => setSettings(true)}>
            <Settings2 aria-hidden="true"/><span className="mobile-game-settings-label">{t('game.settings')}</span>
          </Button>
        </div>
      </header>
      <div className="mobile-game-progress">
        {statistics}
        {friends}
      </div>
    </div>
    {settings && <div className="options-scroll mobile-game-view mobile-game-settings game-settings-page">
      <header>
        <Button ref={backButton} className="mobile-game-back" variant="ghost" size="icon" id="libraryBack" onClick={() => setSettings(false)}
          aria-label={t('action.back')}><ArrowLeft aria-hidden="true"/></Button>
        <GameDetailIdentity product={product} cover={cover} titleId="gameTitle"/>
      </header>
      {children}
    </div>}
  </div>;
}
