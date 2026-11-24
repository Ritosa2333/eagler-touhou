import {CloudCheck, Gamepad2, Star} from 'lucide-react';
import {useSyncExternalStore, type ReactNode, type Ref} from 'react';
import {useLocale} from '../../i18n';
import {Button} from '../../ui/button';
import {BookWorkNumber} from './BookWorkNumber';
import {BOOK_PREFERENCES_EVENT, bookLastPlayed, readBookPreference, stringArray, writeBookPreference} from './book-preferences';
import {impressFavorite} from './ink-feedback';
import type {LibraryProduct} from './products';
import './game-detail-chrome.css';

function subscribePreferences(update: () => void) {
  window.addEventListener(BOOK_PREFERENCES_EVENT, update);
  window.addEventListener('storage', update);
  return () => {
    window.removeEventListener(BOOK_PREFERENCES_EVENT, update);
    window.removeEventListener('storage', update);
  };
}

export function GameDetailIdentity({product, cover, titleId, titleRef, desktop, children}: {
  product: LibraryProduct; cover?: string | null; titleId?: string; titleRef?: Ref<HTMLHeadingElement>;
  desktop?: boolean; children?: ReactNode;
}) {
  const Heading = desktop ? 'h2' : 'h1';
  return <div className="game-detail-identity">
    <div className="game-detail-cover" aria-hidden="true">
      {cover ? <img src={cover} alt="" decoding="async"/> : <Gamepad2/>}
    </div>
    <div className="game-detail-description">
      <div className="game-detail-title">
        <Heading id={titleId} ref={titleRef} tabIndex={titleRef ? -1 : undefined}>{product.title}</Heading>
        {desktop && titleRef
          ? <small className="book-work-number" id="gameId" data-game={product.gameId}>{product.gameId}</small>
          : <BookWorkNumber gameId={product.gameId} number={product.number}/>}
      </div>
      {product.subtitle && <p>{product.subtitle}</p>}
      {children}
    </div>
  </div>;
}

export function GameDetailMetrics({productId, playtime, saved}: {productId: string; playtime?: number | null; saved: boolean}) {
  const {locale, t} = useLocale();
  const lastPlayed = useSyncExternalStore(subscribePreferences, () => bookLastPlayed(productId), () => undefined);
  const hours = playtime == null ? '—' : new Intl.NumberFormat(locale, {
    style:'unit', unit:'hour', maximumFractionDigits:1,
  }).formatToParts(playtime / 3600).map((part, index, parts) =>
    part.type === 'unit' && index > 0 && parts[index - 1]?.type !== 'literal' ? ` ${part.value}` : part.value
  ).join('');
  return <div className="game-detail-metrics">
    {saved && <CloudCheck role="img" aria-label={t('cloudSave.title')}/>}
    <dl>
      <div><dt>{t('game.lastPlayed')}</dt><dd>{lastPlayed ?
        new Intl.DateTimeFormat(locale,{month:'short',day:'numeric'}).format(lastPlayed) : '—'}</dd></div>
      <div><dt>{t('game.playtime')}</dt><dd>{hours}</dd></div>
    </dl>
  </div>;
}

export function GameDetailFavorite({productId}: {productId: string}) {
  const {t} = useLocale();
  const favorite = useSyncExternalStore(subscribePreferences,
    () => readBookPreference('favorites', [], stringArray).includes(productId), () => false);
  return <Button variant="outline" size="icon" className="game-detail-favorite" aria-pressed={favorite}
    aria-label={t('library.category.favorites')} onClick={event => {
      impressFavorite(event.currentTarget);
      const current = readBookPreference('favorites', [], stringArray);
      writeBookPreference('favorites', current.includes(productId) ? current.filter(id => id !== productId) : [...current, productId]);
    }}><Star aria-hidden="true" fill={favorite ? 'currentColor' : 'none'}/></Button>;
}
