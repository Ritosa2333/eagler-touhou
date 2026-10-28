import {useEffect, useState} from 'react';
import {useLocale} from '../../i18n';
import {useOptionsTarget} from './options-ownership';
import type {LibraryCardsProps} from './LibraryCards';
import type {LibraryProduct} from './products';
import {BOOK_PREFERENCES_EVENT, bookCategory, filterBookProducts, readBookPreference, writeBookPreference, stringArray, type BookCategory} from './book-preferences';
import {bookArtwork} from './book-artwork';

type Props = Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'>;
const categories: BookCategory[] = ['all', 'favorites', 'windows', 'pc98', 'spin-off', 'doujin'];

/** Presentation only: activation still enters the existing Router/settings owner. */
export function BookLibrary({products, openedProduct, onSelect, onActivate, interactive = true}: Props) {
  const {locale} = useLocale(), en = locale === 'en';
  const [category, setCategory] = useState<BookCategory>('all');
  const [query, setQuery] = useState(''), [sort, setSort] = useState('number');
  const [view, setView] = useState<'covers' | 'list'>('covers');
  const [favorites, setFavorites] = useState<string[]>([]), [recent, setRecent] = useState<string[]>([]);
  const selectedCard = useOptionsTarget('selectedCard');
  const labels: Record<BookCategory, string> = {all: en ? 'All games' : '全部', favorites: en ? 'Favorites' : '我的收藏', pc98: en ? 'Classics' : '旧作', windows: en ? 'Main series' : '正作', 'spin-off': en ? 'Spin-offs' : '小数点', doujin: en ? 'Doujin' : '同人'};
  const available = products.filter(product => !product.multiplayer && !product.hidden);
  useEffect(() => {
    if (!interactive) return;
    const refresh = () => {
      setFavorites(readBookPreference('favorites', [], stringArray));
      setRecent(readBookPreference('recent', [], stringArray));
      setView(readBookPreference('view', 'covers', (value): value is 'covers' | 'list' => value === 'covers' || value === 'list'));
    };
    refresh();
    window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => {window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
  }, [interactive]);
  const list = filterBookProducts(products, category, favorites, query, sort);
  const toggleFavorite = (id: string) => {
    const next = favorites.includes(id) ? favorites.filter(item => item !== id) : [...favorites, id];
    setFavorites(next); writeBookPreference('favorites', next);
  };
  const activate = (product: LibraryProduct) => {onSelect(product.id); onActivate(product.id);};
  const gameLink = (product: LibraryProduct, recentLink = false) => <a
    ref={openedProduct === product.id && (!recentLink || !list.some(item => item.id === product.id)) ? selectedCard : undefined}
    className={recentLink ? 'book-recent-link' : 'book-game-link'} href={product.href}
    data-book-product={product.id} data-game={product.gameId} aria-current={openedProduct === product.id ? 'page' : 'false'}
    onClick={interactive ? event => {if (event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) return; event.preventDefault(); activate(product);} : undefined}>
    {view === 'covers' && <span className="book-art"><BookCover product={product}/></span>}
    <span className="book-identity"><small>TH{product.number}</small><strong>{product.title}</strong><span>{product.subtitle}</span></span>
    {view === 'list' && <span className="book-row-action" aria-hidden="true">{en ? 'Play' : '开始游玩'} →</span>}
  </a>;
  const history = recent.map(id => available.find(product => product.id === id)).filter((product): product is LibraryProduct => !!product).slice(0, view === 'list' ? 5 : 20);
  return <div className={`book-library book-view-${view}`}>
    <aside className="book-sidebar" aria-label={en ? 'Game categories' : '游戏分类'}>
      <label className="book-search"><span aria-hidden="true">⌕</span><input type="search" value={query} onChange={event => setQuery(event.target.value)} disabled={!interactive} placeholder={en ? 'Search games…' : '搜索游戏或代号…'} aria-label={en ? 'Search games' : '搜索游戏'}/></label>
      <nav>{categories.map(item => <button key={item} type="button" data-book-category={item} className={category === item ? 'active' : ''} aria-pressed={category === item} disabled={!interactive} onClick={() => setCategory(item)}>
        <span className="book-cat-icon" aria-hidden="true">{item === 'all' ? '▤' : item === 'favorites' ? '★' : '•'}</span><span>{labels[item]}</span>
        <small>{available.filter(product => item === 'all' || (item === 'favorites' ? favorites.includes(product.id) : bookCategory(product) === item)).length}</small>
      </button>)}</nav>
      <p className="book-sidebar-note">EAGLER TOUHOU</p>
    </aside>
    <div className="book-content">
      <section className="book-recent" aria-labelledby="bookRecentTitle"><div className="book-section-heading"><h2 id="bookRecentTitle">{en ? 'Recently played' : '最近游玩'}</h2><span>{en ? 'Your latest adventures' : '按最近启动顺序'}</span></div>
        <div className="book-recent-games">{history.length ? history.map(product => <article key={product.id}>{gameLink(product, true)}</article>) : <p className="book-recent-empty">{en ? 'Choose a game below to begin your collection.' : '还没有游玩记录，从下方选择一部作品开始吧。'}</p>}</div>
      </section>
      <div className="book-view-header"><div className="book-current"><h2>{labels[category]}</h2><span role="status">{list.length} {en ? 'games' : '部作品'}</span></div>
        <div className="book-tools"><div className="book-view-switch" role="group" aria-label={en ? 'Library view' : '书库显示方式'}>{(['covers', 'list'] as const).map(mode => <button key={mode} type="button" data-book-view={mode} disabled={!interactive} aria-pressed={view === mode} onClick={() => {setView(mode); writeBookPreference('view', mode);}}>{mode === 'covers' ? en ? '▦ Covers' : '▦ 封面' : en ? '☷ List' : '☷ 列表'}</button>)}</div>
          <select aria-label={en ? 'Sort games' : '作品排序'} value={sort} disabled={!interactive} onChange={event => setSort(event.target.value)}><option value="number">{en ? 'Number' : '按作品编号'}</option><option value="reverse">{en ? 'Number, descending' : '编号倒序'}</option><option value="name">{en ? 'Title' : '按作品名称'}</option></select>
        </div>
      </div>
      <div className="book-grid">{list.map(product => <article key={product.id} className="book-card">{gameLink(product)}<button className="book-favorite" data-book-favorite={product.id} type="button" disabled={!interactive} aria-label={`${en ? 'Favorite' : '收藏'} ${product.title}`} aria-pressed={favorites.includes(product.id)} onClick={() => toggleFavorite(product.id)}><svg viewBox="0 0 24 24" aria-hidden="true" focusable="false"><path d="m12 3 2.78 5.63L21 9.54l-4.5 4.39 1.06 6.2L12 17.2l-5.56 2.93 1.06-6.2L3 9.54l6.22-.91Z"/></svg></button></article>)}</div>
      {!list.length && <div className="book-empty"><h3>{en ? 'No matching games' : '未找到匹配的游戏'}</h3><p>{en ? 'Try another search or category.' : '请尝试搜索其他关键词或选择其他分类。'}</p><button type="button" onClick={() => {setQuery(''); setCategory('all');}}>{en ? 'Reset filters' : '重置筛选'}</button></div>}
    </div>
  </div>;
}

function BookCover({product}: {product: LibraryProduct}) {
  const [failed, setFailed] = useState(false);
  const source = bookArtwork(product);
  return source && !failed ? <img src={source} alt="" loading="lazy" decoding="async" onError={() => setFailed(true)}/> : <span className="book-cover-fallback">TH{product.number}<strong>{product.title}</strong></span>;
}
