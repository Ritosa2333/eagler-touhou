import {useEffect, useState} from 'react';
import {useLocale} from '../../i18n';
import type {LibraryProduct} from './products';
import {bookArtwork} from './book-artwork';
import {BOOK_PREFERENCES_EVENT, readBookPreference, writeBookPreference, stringArray} from './book-preferences';

export function BookDirectoryGames({products, selectedProduct, productFilters, onProductFiltersChange, onSelect, onActivate, interactive = true}: {
  products: readonly LibraryProduct[]; selectedProduct?: string; productFilters?: readonly string[];
  onProductFiltersChange?(products: string[]): void; onSelect(id: string): void; onActivate(id: string): void; interactive?: boolean;
}) {
  const {locale} = useLocale(), en = locale === 'en';
  const [favorites, setFavorites] = useState<string[]>([]);
  const [failedArt, setFailedArt] = useState<ReadonlySet<string>>(() => new Set());
  useEffect(() => {
    const refresh = () => setFavorites(readBookPreference('multiplayer-favorites', [], stringArray));
    refresh(); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => {window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
  }, []);
  const filters = productFilters ?? (selectedProduct ? [selectedProduct] : []);
  const change = (ids: string[]) => onProductFiltersChange ? onProductFiltersChange(ids) : onSelect(ids.length === 1 ? ids[0] : '');
  const visible = products.filter(p => !p.hidden).sort((a, b) => Number(favorites.includes(b.id)) - Number(favorites.includes(a.id)) || Number(a.number) - Number(b.number));
  return <nav className="book-lobby-games" aria-label={en ? 'Filter multiplayer games' : '筛选联机游戏'}>
    <div className="book-lobby-filter-heading"><span>{en ? 'Games' : '联机作品'}</span><small>{en ? 'Select several · swipe to browse' : '可多选 · 窄屏左右滑动'}</small></div>
    <div className="book-lobby-strip" id="lobbyGameRail">
      <button type="button" className="book-lobby-all" aria-pressed={!filters.length} disabled={!interactive} onClick={() => change([])}>
        <span aria-hidden="true">▦</span><strong>{en ? 'All multiplayer' : '全部联机'}</strong><small>{en ? 'Rooms from every game' : '查看所有作品的房间'}</small>
      </button>
      {visible.map(product => <div key={product.id} className="book-lobby-item" data-lobby-product={product.id} data-selected={filters.includes(product.id)}>
        <input type="checkbox" aria-label={`${en ? 'Filter' : '筛选'} ${product.title}`} checked={filters.includes(product.id)} disabled={!interactive}
          onChange={() => change(filters.includes(product.id) ? filters.filter(id => id !== product.id) : [...filters, product.id])}/>
        <button type="button" className="book-lobby-game" disabled={!interactive} onClick={() => filters.length === 1 && filters[0] === product.id ? onActivate(product.id) : change([product.id])}>
          <span className="book-lobby-cover" aria-hidden="true">{bookArtwork(product) && !failedArt.has(product.id)
            ? <img src={bookArtwork(product)!} alt="" loading="lazy" decoding="async" onError={() => setFailedArt(previous => new Set(previous).add(product.id))}/>
            : <span>TH{product.number}</span>}</span>
          <span className="book-lobby-number">TH{product.number}</span><strong>{product.title}</strong>
        </button>
        <button type="button" className="book-lobby-favorite" aria-label={`${en ? 'Favorite' : '收藏'} ${product.title}`} aria-pressed={favorites.includes(product.id)} disabled={!interactive}
          onClick={() => {const latest = readBookPreference('multiplayer-favorites', [], stringArray); writeBookPreference('multiplayer-favorites', latest.includes(product.id) ? latest.filter(id => id !== product.id) : [...latest, product.id]);}}>
          <svg viewBox="0 0 24 24" aria-hidden="true"><path d="m12 3 2.78 5.63L21 9.54l-4.5 4.39 1.06 6.2L12 17.2l-5.56 2.93 1.06-6.2L3 9.54l6.22-.91Z"/></svg>
        </button>
      </div>)}
    </div>
  </nav>;
}
