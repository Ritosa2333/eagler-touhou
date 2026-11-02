import {Check} from '../../ui/controls';
import {BookButton} from './BookButton';
import {impressFavorite} from './ink-feedback';
import {useEffect, useState} from 'react';
import {useLocale} from '../../i18n';
import type {LibraryProduct} from './products';
import {bookArtwork} from './book-artwork';
import {BOOK_PREFERENCES_EVENT, readBookPreference, writeBookPreference, stringArray} from './book-preferences';

export function BookDirectoryGames({products, selectedProduct, viewingProduct, productFilters, onProductFiltersChange, onSelect, interactive = true}: {
  products: readonly LibraryProduct[]; selectedProduct?: string; productFilters?: readonly string[];
  viewingProduct?: string; onProductFiltersChange?(products: string[]): void; onSelect(id: string): void; interactive?: boolean;
}) {
  const {t} = useLocale();
  const [favorites, setFavorites] = useState<string[]>([]);
  const [localFilters, setLocalFilters] = useState<string[]>([]);
  const [failedArt, setFailedArt] = useState<ReadonlySet<string>>(() => new Set());
  useEffect(() => {
    const refresh = () => setFavorites(readBookPreference('multiplayer-favorites', [], stringArray));
    refresh(); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => {window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
  }, []);
  const filters = productFilters ?? localFilters;
  const viewProduct = viewingProduct ?? selectedProduct ?? '';
  const change = (ids: string[]) => onProductFiltersChange ? onProductFiltersChange(ids) : setLocalFilters(ids);
  const view = (id: string) => {if (!onProductFiltersChange) setLocalFilters([]); onSelect(id);};
  const visible = products.filter(p => !p.hidden).sort((a, b) => Number(favorites.includes(b.id)) - Number(favorites.includes(a.id)) || Number(a.number) - Number(b.number));
  return <nav className="book-lobby-games" aria-label={t('directory.gameFilter')}>
    <div className="book-lobby-filter-heading"><span>{t('directory.games')}</span><small>{t('directory.multiSelect')}</small></div>
    <div className="book-lobby-strip" id="lobbyGameRail">
      <BookButton variant="choice" type="button" className="book-lobby-all" aria-pressed={!filters.length && !viewProduct} disabled={!interactive} onClick={() => view('')}>
        <span aria-hidden="true">▦</span><strong>{t('directory.allGames')}</strong><small>{t('directory.allGameHint')}</small>
      </BookButton>
      {visible.map(product => <div key={product.id} className="book-lobby-item" data-lobby-product={product.id} data-selected={filters.includes(product.id)} data-viewing={viewProduct === product.id}>
        <Check aria-label={t('directory.filterGame', {game: product.title})} checked={filters.includes(product.id)} disabled={!interactive}
          onCheckedChange={() => change(filters.includes(product.id) ? filters.filter(id => id !== product.id) : [...filters, product.id])}/>
        <BookButton variant="choice" type="button" className="book-lobby-game" aria-label={t('directory.viewGame', {game: product.title})} aria-pressed={viewProduct === product.id} disabled={!interactive} onClick={() => view(product.id)}>
          <span className="book-lobby-cover" aria-hidden="true">{bookArtwork(product) && !failedArt.has(product.id)
            ? <img src={bookArtwork(product)!} alt="" loading="lazy" decoding="async" onError={() => setFailedArt(previous => new Set(previous).add(product.id))}/>
            : <span>th{product.number}</span>}</span>
          <span className="book-lobby-number">th{product.number}</span><strong>{product.title}</strong>
        </BookButton>
        <BookButton variant="icon" type="button" className="book-lobby-favorite" aria-label={t('directory.favoriteGame', {game: product.title})} aria-pressed={favorites.includes(product.id)} disabled={!interactive}
          onClick={event => {impressFavorite(event.currentTarget); const latest = readBookPreference('multiplayer-favorites', [], stringArray); writeBookPreference('multiplayer-favorites', latest.includes(product.id) ? latest.filter(id => id !== product.id) : [...latest, product.id]);}}>
          <svg viewBox="0 0 24 24" aria-hidden="true"><path d="m12 3 2.78 5.63L21 9.54l-4.5 4.39 1.06 6.2L12 17.2l-5.56 2.93 1.06-6.2L3 9.54l6.22-.91Z"/></svg>
        </BookButton>
      </div>)}
    </div>
  </nav>;
}
