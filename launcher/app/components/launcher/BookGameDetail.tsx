import { useLayoutEffect, useRef, type ReactNode } from 'react';
import { ArrowLeft, Settings2 } from 'lucide-react';
import { useLocale } from '../../i18n';
import { Button } from '../../ui/button';
import { useContentMotion } from '../../ui/use-surface-motion';
import { bookArtwork } from './book-artwork';
import { GameDetailFavorite, GameDetailIdentity, GameDetailMetrics } from './GameDetailChrome';
import type { LibraryProduct } from './products';
import './book-game-detail.css';

export function BookGameDetail({ product, actions, children, statistics, friends, playtime, saved, active, settings, setSettings, onBack }: {
  product: LibraryProduct;
  actions?: ReactNode;
  children?: ReactNode;
  statistics?: ReactNode;
  friends?: ReactNode;
  playtime?: number | null;
  saved: boolean;
  active: boolean;
  settings: boolean;
  setSettings(open: boolean): void;
  onBack(): void;
}) {
  const { t } = useLocale();
  const title = useRef<HTMLHeadingElement>(null), settingsEntry = useRef<HTMLButtonElement>(null);
  const settingsBack = useRef<HTMLButtonElement>(null), previousSettings = useRef(false);
  const article = useContentMotion<HTMLElement>(settings ? 'settings' : 'detail');
  const cover = bookArtwork(product);
  useLayoutEffect(() => {
    if (!active) return;
    const container = article.current;
    if (!container || container.dataset.gameDetail !== product.id) return;
    const changed = previousSettings.current !== settings;
    previousSettings.current = settings;
    if (changed || !container.contains(document.activeElement)) {
      const target = settings ? settingsBack.current : changed ? settingsEntry.current : title.current;
      target?.focus({preventScroll: true});
    }
    const top = parseFloat(getComputedStyle(container).scrollMarginTop);
    if (changed || container.getBoundingClientRect().top < top) container.scrollIntoView({block: 'start'});
  }, [active, article, product.id, settings]);

  return (
    <article ref={article} className="book-game-detail" data-game-detail={product.id}
      aria-labelledby={settings ? 'bookGameSettings' : 'gameTitle'} inert={!active}>
      <div hidden={settings} inert={settings}>
        <header className="game-detail-hero book-game-detail-hero">
          <Button variant="ghost" size="icon" className="book-game-detail-back" onClick={onBack} aria-label={t('library.back')}>
            <ArrowLeft aria-hidden="true"/>
          </Button>
          {cover && <img className="game-detail-backdrop" src={cover} alt="" aria-hidden="true" decoding="async"/>}
          <GameDetailIdentity product={product} cover={cover} titleId="gameTitle" titleRef={title} desktop/>
          <div className="book-game-detail-actions">
            {actions}
            <GameDetailMetrics productId={product.id} playtime={playtime} saved={saved}/>
            <Button variant="outline" id={settings ? undefined : 'gameSettingsOpen'} ref={settingsEntry} onClick={() => setSettings(true)}>
              <Settings2 aria-hidden="true"/>{t('game.settings')}
            </Button>
            <GameDetailFavorite productId={product.id}/>
          </div>
        </header>
        <div className="book-game-detail-content" data-has-friends={!!friends || undefined}>
          {statistics}
          {friends}
        </div>
      </div>
      <section className="book-game-detail-settings game-settings-page" aria-labelledby="bookGameSettings" hidden={!settings} inert={!settings}>
        <header>
          <Button ref={settingsBack} id={settings ? 'libraryBack' : undefined} variant="ghost" size="sm"
            className="book-game-settings-back" onClick={() => setSettings(false)}>
            <ArrowLeft aria-hidden="true"/>{t('game.backToDetail')}
          </Button>
          <GameDetailIdentity product={product} cover={cover} titleId="bookGameSettings" desktop/>
        </header>
        {children}
      </section>
    </article>
  );
}
