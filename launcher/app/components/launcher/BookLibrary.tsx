import {useEffect, useRef, useState, type ReactNode} from 'react';
import {useSearchParams} from 'react-router';
import {Search, Star, LayoutGrid, List, Monitor, Disc3, Gamepad2, Shapes, History, X, ArrowUpRight} from 'lucide-react';
import {Button} from '../../ui/button';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {InputGroup, InputGroupInput} from '../../ui/input-group';
import {Select, SelectTrigger, SelectValue, SelectContent, SelectItem} from '../../ui/select';
import {useLocale} from '../../i18n';
import {useOptionsTarget} from './options-ownership';
import type {LibraryCardsProps} from './LibraryCards';
import type {LibraryProduct} from './products';
import {BOOK_PREFERENCES_EVENT, bookCategory, filterBookProducts, readBookPreference, writeBookPreference, stringArray, type BookCategory} from './book-preferences';
import {bookArtwork} from './book-artwork';

type Props = Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'> & {footer?: ReactNode};
const categories: {id: BookCategory; Icon: typeof LayoutGrid}[] = [
  {id:'all',Icon:LayoutGrid},{id:'favorites',Icon:Star},{id:'windows',Icon:Monitor},
  {id:'pc98',Icon:Disc3},{id:'spin-off',Icon:Gamepad2},{id:'doujin',Icon:Shapes},
];

/** Product membership, settings activation and persistence stay with their existing owners. */
export function BookLibrary({products, openedProduct, onSelect, onActivate, interactive = true, footer}: Props) {
  const {t} = useLocale(), [search] = useSearchParams();
  const [category, setCategory] = useState<BookCategory>('all');
  const [query, setQuery] = useState(''), [sort, setSort] = useState('number');
  const [view, setView] = useState<'covers' | 'list'>('covers');
  const [favorites, setFavorites] = useState<string[]>([]), [recent, setRecent] = useState<string[]>([]);
  const input = useRef<HTMLInputElement>(null), selectedCard = useOptionsTarget('selectedCard');
  const labels: Record<BookCategory, string> = {all:t('library.category.all'),favorites:t('library.category.favorites'),pc98:t('library.category.pc98'),windows:t('library.category.windows'),'spin-off':t('library.category.spinOff'),doujin:t('library.category.doujin')};
  const sortOptions = [{value:'number',label:t('library.sort.number')},{value:'reverse',label:t('library.sort.reverse')},{value:'name',label:t('library.sort.name')}];
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
    <span className="book-art"><BookCover product={product}/></span>
    <span className="book-identity"><small>th{product.number}</small><strong>{product.title}</strong>{!recentLink && <span>{product.subtitle}</span>}</span>
    {recentLink && <ArrowUpRight className="book-recent-arrow" aria-hidden="true"/>}
  </a>;
  const displayedRecent = search.get('preview') === 'recent' ? available.slice(0, 7).map(product => product.id) : recent;
  const history = displayedRecent.map(id => available.find(product => product.id === id)).filter((product): product is LibraryProduct => !!product).slice(0, 20);
  return <div className={`book-library book-view-${view}`} data-ui-kit="home">
    <aside className="book-sidebar" aria-label={t('library.categories')}>
      <InputGroup className="book-search"><Search aria-hidden="true"/><InputGroupInput ref={input} id="librarySearch" type="search" value={query} onChange={event => setQuery(event.target.value)} disabled={!interactive} placeholder={t('library.searchPlaceholder')} aria-label={t('library.search')} aria-controls="bookGameGrid"/>
        {query && <Button variant="ghost" size="icon-sm" className="max-[780px]:size-11" aria-label={t('library.clearSearch')} onClick={()=>{setQuery('');input.current?.focus();}}><X/></Button>}
      </InputGroup>
      <ChoiceGroup asChild type="single" value={category} onValueChange={value=>{if(value)setCategory(value as BookCategory);}} disabled={!interactive} aria-label={t('library.category')}>
        <nav>{categories.map(({id,Icon})=><ChoiceItem key={id} value={id} data-book-category={id} className="grid min-h-10 w-full shrink-0 pointer-coarse:min-h-11 grid-cols-[18px_minmax(0,1fr)_auto] justify-start gap-2.5 px-2.5 text-[13px] max-[780px]:inline-flex max-[780px]:min-h-11 max-[780px]:w-auto max-[780px]:min-w-max max-[780px]:text-xs">
          <Icon aria-hidden="true"/><span>{labels[id]}</span><small>{available.filter(product=>id==='all'||(id==='favorites'?favorites.includes(product.id):bookCategory(product)===id)).length}</small>
        </ChoiceItem>)}</nav>
      </ChoiceGroup>
    </aside>
    <div className="book-content">
      <div className="book-content-main">
        {history.length>0&&<section className="book-recent" aria-labelledby="bookRecentTitle">
          <div className="book-section-heading"><History aria-hidden="true"/><h2 id="bookRecentTitle">{t('library.recent')}</h2></div>
          <div className="book-recent-games">{history.map(product=><article key={product.id}>{gameLink(product,true)}</article>)}</div>
        </section>}
        <div className="book-view-header">
          <div className="book-current"><h2>{labels[category]}</h2><span role="status" aria-live="polite">{t('library.count',{count:list.length})}</span></div>
          <div className="book-tools">
            <ChoiceGroup type="single" className="book-view-switch" value={view} onValueChange={value=>{if(value==='covers'||value==='list'){setView(value);writeBookPreference('view',value);}}} disabled={!interactive} aria-label={t('library.view')}>
              <ChoiceItem value="covers" data-book-view="covers" className="min-h-7 px-2.5 pointer-coarse:min-h-11 py-1.5 text-xs data-[state=on]:bg-card max-[780px]:min-h-11"><LayoutGrid aria-hidden="true"/><span>{t('library.covers')}</span></ChoiceItem>
              <ChoiceItem value="list" data-book-view="list" className="min-h-7 px-2.5 pointer-coarse:min-h-11 py-1.5 text-xs data-[state=on]:bg-card max-[780px]:min-h-11"><List aria-hidden="true"/><span>{t('library.list')}</span></ChoiceItem>
            </ChoiceGroup>
            <Select value={sort} onValueChange={setSort} disabled={!interactive}>
              <SelectTrigger className="book-sort text-xs" aria-label={t('library.sort')}><SelectValue>{sortOptions.find(option=>option.value===sort)?.label}</SelectValue></SelectTrigger>
              <SelectContent>{sortOptions.map(option=><SelectItem key={option.value} value={option.value}>{option.label}</SelectItem>)}</SelectContent>
            </Select>
          </div>
        </div>
        <div className="book-grid" id="bookGameGrid">{list.map(product=><article key={product.id} className="book-card">
          {gameLink(product)}
          <Button variant="ghost" size="icon" className="book-favorite rounded-full aria-pressed:text-[var(--kit-brand)]" data-book-favorite={product.id} disabled={!interactive} aria-label={t('library.favorite',{title:product.title})} aria-pressed={favorites.includes(product.id)} onClick={()=>toggleFavorite(product.id)}><Star aria-hidden="true"/></Button>
        </article>)}</div>
        {!list.length&&<div className="book-empty"><Search aria-hidden="true"/><h3>{t(category==='favorites'&&!query?'library.emptyFavorites':'library.emptySearch')}</h3><Button variant="outline" className="max-[780px]:h-11" disabled={!interactive} onClick={()=>{setQuery('');setCategory('all');}}>{t('library.viewAll')}</Button></div>}
      </div>
      {footer}
    </div>
  </div>;
}

function BookCover({product}: {product: LibraryProduct}) {
  const [failed, setFailed] = useState(false), source = bookArtwork(product);
  return source && !failed ? <img src={source} alt="" loading="lazy" decoding="async" onError={()=>setFailed(true)}/> : <span className="book-cover-fallback">th{product.number}<strong>{product.title}</strong></span>;
}
