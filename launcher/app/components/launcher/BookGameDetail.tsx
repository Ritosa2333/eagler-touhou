import { useLayoutEffect, useRef, type CSSProperties, type ReactNode } from 'react';
import { Gamepad2 } from 'lucide-react';
import { useLocale } from '../../i18n';
import { bookArtwork } from './book-artwork';
import type { LibraryProduct } from './products';
import './book-game-detail.css';

export function BookGameDetail({ product, actions, children, statistics, active }: {
  product: LibraryProduct;
  actions?: ReactNode;
  children?: ReactNode;
  statistics?: ReactNode;
  active: boolean;
}) {
  const { t } = useLocale();
  const title = useRef<HTMLHeadingElement>(null);
  const cover = bookArtwork(product);
  const composition = product.presentation;
  const compositionStyle = composition ? {
    '--detail-art-position': `${composition.positionPercent}%`,
    '--detail-art-brightness': composition.artBrightness,
    '--detail-art-saturation': composition.artSaturation,
  } as CSSProperties : undefined;
  useLayoutEffect(() => {
    if (!active) return;
    const node = title.current;
    const article = node?.closest('article[data-game-detail]');
    if (!node || !article || article.getAttribute('data-game-detail') !== product.id) return;
    if (!article.contains(document.activeElement)) node.focus({ preventScroll: true });
    const top = getComputedStyle(article).scrollMarginTop;
    if (article.getBoundingClientRect().top < parseFloat(top)) article.scrollIntoView({ block: 'start' });
  }, [active, product.id]);

  return (
    <article className="book-game-detail" data-game-detail={product.id} aria-labelledby="gameTitle" inert={!active}>
      <div className="book-game-detail-hero" style={compositionStyle}>
        {cover && <div className="book-game-detail-art" aria-hidden="true">
          <img src={cover} alt="" decoding="async" />
        </div>}
        <div className="book-game-detail-cover" aria-hidden="true">
          {cover ? <img src={cover} alt="" decoding="async" /> : <Gamepad2 />}
        </div>
        <div className="book-game-detail-intro">
          <span className="book-game-detail-code" id="gameId" data-game={product.gameId}>{product.gameId}</span>
          <h2 id="gameTitle" ref={title} tabIndex={-1}>{product.title}</h2>
          {product.subtitle && <p>{product.subtitle}</p>}
          {actions}
        </div>
      </div>
      <div className="book-game-detail-content">
        <section className="book-game-detail-statistics" aria-labelledby="bookGameStatistics">
          <h3 id="bookGameStatistics">{t('library.statistics')}</h3>
          {statistics}
        </section>
        <section className="book-game-detail-settings" aria-labelledby="bookGameSettings">
          <h3 id="bookGameSettings">{t('room.gameSettingsGroup')}</h3>
          {children}
        </section>
      </div>
    </article>
  );
}
