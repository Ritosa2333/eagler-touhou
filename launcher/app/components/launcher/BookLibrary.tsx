import { Activity, useEffect, useLayoutEffect, useMemo, useRef, useState, type MouseEvent, type ReactNode } from 'react';
import { useSearchParams } from 'react-router';
import { createPortal } from 'react-dom';
import {
  Search,
  Star,
  LayoutGrid,
  List,
  Monitor,
  Puzzle,
  Disc3,
  Gamepad2,
  Shapes,
  Swords,
} from 'lucide-react';
import { Button } from '../../ui/button';
import { Skeleton } from '../../ui/skeleton';
import { ChoiceGroup, ChoiceItem } from '../../ui/choice-group';
import { Select, SelectTrigger, SelectValue, SelectContent, SelectItem } from '../../ui/select';
import { Toolbar } from '../../ui/toolbar';
import {useNavigationPageMotion} from '../../navigation/navigation-motion';
import { useLocale } from '../../i18n';
import { useOptionsTarget } from './options-ownership';
import type { LibraryCardsProps } from './LibraryCards';
import { bookWorkCategories, type LibraryProduct } from './products';
import {
  BOOK_PREFERENCES_EVENT,
  filterBookProducts,
  readBookPreference,
  writeBookPreference,
  stringArray,
  type BookCategory,
} from './book-preferences';
import { bookArtwork } from './book-artwork';
import {impressFavorite} from './ink-feedback';
import { BookWorkNumber } from './BookWorkNumber';
import {BookGameRail} from './BookGameRail';
import {LibrarySearch} from './LibrarySearch';
import {useContentMotion} from '../../ui/use-surface-motion';
import {useLibraryMotion} from './use-library-motion';
import {useBookDisclosures} from './use-book-disclosures';

type Props = Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'> & {
  compact?: boolean;
  mobileSearchTarget?: HTMLElement | null;
  tools?: ReactNode;
  detail?: ReactNode;
  onBrowse?: () => void;
};
const categories: { id: BookCategory; Icon: typeof LayoutGrid }[] = [
  { id: 'all', Icon: LayoutGrid },
  { id: 'favorites', Icon: Star },
  { id: 'windows', Icon: Monitor },
  { id: 'versus', Icon: Swords },
  { id: 'fighting', Icon: Gamepad2 },
  { id: 'spin-off', Icon: Puzzle },
  { id: 'pc98', Icon: Disc3 },
  { id: 'doujin', Icon: Shapes },
];
const defaultDisclosures = {windows: true, versus: true, fighting: true};

/** Product membership, settings activation and persistence stay with their existing owners. */
export function BookLibrary({ products, openedProduct, onSelect, onActivate, interactive = true, compact = false, mobileSearchTarget, tools, detail, onBrowse }: Props) {
  const { t } = useLocale(),
    [search] = useSearchParams();
  const showDetail = !compact && detail != null;
  const detailId = showDetail ? openedProduct : undefined;
  const previousDetail = useRef(detailId);
  const content = useNavigationPageMotion(compact ? null : detailId ?? 'library', {
    contentSelector: showDetail ? '.book-game-detail' : '.book-content-main',
    direction: showDetail ? 'forward' : 'back',
  });
  const [category, setCategory] = useState<BookCategory>('all');
  const headingMotion = useContentMotion<HTMLHeadingElement>(compact ? category : null);
  // Navigation opens the current game's category and preserves disclosure preferences.
  const [disclosures, setDisclosure] = useBookDisclosures('library-disclosures', defaultDisclosures, interactive);
  const [sidebarOrigin, setSidebarOrigin] = useState<BookCategory | null>(null);
  const [query, setQuery] = useState(''),
    [sort, setSort] = useState('number');
  const [view, setView] = useState<'covers' | 'list'>('covers');
  const [resultsMinHeight, setResultsMinHeight] = useState(0);
  const [favorites, setFavorites] = useState<string[]>([]),
    [recent, setRecent] = useState<string[]>([]);
  const input = useRef<HTMLInputElement>(null),
    results = useRef<HTMLDivElement>(null),
    selectedCard = useOptionsTarget('selectedCard');
  const {run: animateLibraryChange, cancel: cancelLibraryMotion} = useLibraryMotion(results);
  useLayoutEffect(() => {
    const previous = previousDetail.current;
    previousDetail.current = detailId;
    if (compact || previous === detailId) return;
    if (detailId) cancelLibraryMotion();
    else if (document.activeElement === document.body) {
      const target = results.current?.querySelector<HTMLElement>(`[data-book-product="${previous}"]`);
      (target ?? input.current)?.focus({ preventScroll: true });
    }
  }, [detailId, compact, cancelLibraryMotion]);
  function reserveResultsSpace() {
    if (!results.current) return;
    // Keep the exposed canvas in the same viewport when filtering shortens it.
    const viewportHeight = document.documentElement.clientHeight || window.innerHeight;
    const canvas = results.current.getBoundingClientRect();
    setResultsMinHeight(Math.max(0, Math.min(canvas.height, Math.ceil(viewportHeight - canvas.top))));
  }
  const labels: Record<BookCategory, string> = {
    all: t('library.category.all'),
    favorites: t('library.category.favorites'),
    pc98: t('library.category.pc98'),
    windows: t('library.category.windows'),
    versus: t('library.category.versus'),
    fighting: t('library.category.fighting'),
    'spin-off': t('library.category.spinOff'),
    doujin: t('library.category.doujin'),
  };
  const sortOptions = [
    { value: 'number', label: t('library.sort.number') },
    { value: 'reverse', label: t('library.sort.reverse') },
    { value: 'name', label: t('library.sort.name') },
  ];
  const available = useMemo(() => products.filter(product => !product.multiplayer && !product.hidden), [products]);
  const detailProduct = showDetail ? available.find((product) => product.id === openedProduct) : undefined;
  const detailCategory = detailProduct ? sidebarOrigin ?? bookWorkCategories(detailProduct)[0] : undefined;
  useLayoutEffect(() => {
    if (!detailCategory || !openedProduct) return;
    setDisclosure(detailCategory, true);
  }, [detailCategory, openedProduct, setDisclosure]);
  useEffect(() => {
    if (!interactive) return;
    const refresh = () => {
      setFavorites(readBookPreference('favorites', [], stringArray));
      setRecent(readBookPreference('recent', [], stringArray));
      setView(
        readBookPreference(
          'view',
          'covers',
          (value): value is 'covers' | 'list' => value === 'covers' || value === 'list',
        ),
      );
    };
    refresh();
    window.addEventListener(BOOK_PREFERENCES_EVENT, refresh);
    window.addEventListener('storage', refresh);
    return () => {
      window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh);
      window.removeEventListener('storage', refresh);
    };
  }, [interactive]);
  const list = useMemo(() => filterBookProducts(available, category, favorites, query, sort), [available, category, favorites, query, sort]);
  const categoryProducts = useMemo(() => new Map(categories.map(({id}) =>
    [id, filterBookProducts(available, id, favorites, '', sort)] as const)), [available, favorites, sort]);
  const selectedSidebar = sidebarOrigin && disclosures[sidebarOrigin] &&
    categoryProducts.get(sidebarOrigin)?.some(product => product.id === openedProduct) ? sidebarOrigin : null;
  const changeCategory = (value: string) => {
    if (!value) return;
    reserveResultsSpace();
    const next = value as BookCategory;
    const nextIds = new Set(filterBookProducts(available, next, favorites, query, sort).map(product => product.id));
    animateLibraryChange(() => setCategory(next), {nextIds: view === 'covers' ? nextIds : undefined});
    if (showDetail) onBrowse?.();
  };
  const toggleFavorite = (id: string) => {
    reserveResultsSpace();
    const next = favorites.includes(id) ? favorites.filter((item) => item !== id) : [...favorites, id];
    setFavorites(next);
    writeBookPreference('favorites', next);
  };
  const favoriteButton = (product: LibraryProduct) => (
    <Button
      variant="ghost"
      size="icon"
      className="book-favorite rounded-full"
      data-book-favorite={product.id}
      disabled={!interactive}
      aria-label={t('library.favorite', { title: product.title })}
      aria-pressed={favorites.includes(product.id)}
      onClick={event => {impressFavorite(event.currentTarget); toggleFavorite(product.id);}}
    >
      <Star aria-hidden="true" />
    </Button>
  );
  const activate = (event: MouseEvent<HTMLElement>, product: LibraryProduct, origin: BookCategory | null = null) => {
    if (event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) return;
    event.preventDefault();
    setSidebarOrigin(origin);
    onSelect(product.id);
    onActivate(product.id);
  };
  const gameLink = (product: LibraryProduct, recentLink = false, priority = false) => (
    <a
      ref={
        openedProduct === product.id && !selectedSidebar && (!recentLink || !list.some((item) => item.id === product.id))
          ? selectedCard
          : undefined
      }
      className={recentLink ? 'book-recent-link' : 'book-game-link'}
      href={product.href}
      data-book-product={product.id}
      data-game={product.gameId}
      aria-current={openedProduct === product.id ? 'page' : 'false'}
      onClick={interactive ? event => activate(event, product) : undefined}
    >
      <span className="book-art">
        <BookCover key={bookArtwork(product)} product={product} priority={priority} />
      </span>
      <span className="book-identity">
        <span className="book-title-line">
          <strong title={product.title}>{product.title}</strong>
          <BookWorkNumber gameId={product.gameId} number={product.number} />
        </span>
        <span title={product.subtitle}>{product.subtitle}</span>
      </span>
    </a>
  );
  const displayedRecent =
    search.get('preview') === 'recent' ? available.slice(0, 7).map((product) => product.id) : recent;
  const history = displayedRecent
    .map((id) => available.find((product) => product.id === id))
    .filter((product): product is LibraryProduct => !!product)
    .slice(0, 20);
  return (
    <div className={`book-library book-view-${view}`} data-ui-kit="home">
      {!compact && <BookGameRail label={t('library.categories')} interactive={interactive}
        search={{query, inputRef:input, id:'librarySearch', controls:showDetail ? undefined : 'bookGameGrid',
          onChange:next => {
            reserveResultsSpace();
            setQuery(next);
            if (showDetail) onBrowse?.();
          }}}
        groups={categories.map(({id, Icon}) => {
          const entries = categoryProducts.get(id) ?? [];
          return {id, icon:Icon, label:labels[id], count:entries.length, selected:id === category, panelId:`bookCategory-${id}`,
            products:Object.hasOwn(disclosures, id) ? entries : undefined,
            expanded:disclosures[id] === true,
            onExpand:id !== 'all' && entries.length ? (open:boolean) => setDisclosure(id, open, true) : undefined,
            onSelect:() => {
              if (id !== category) changeCategory(id);
              else {
                cancelLibraryMotion();
                if (showDetail) onBrowse?.();
              }
            }};
        })}
        favorites={favorites} onFavorite={toggleFavorite}
        gameAction={(product,id) => ({href:product.href, selected:openedProduct === product.id,
          anchorRef:selectedSidebar === id && openedProduct === product.id ? selectedCard : undefined,
          onClick:event => activate(event, product, id)})}/>}
      <div ref={content} className="book-content" data-work-detail={showDetail || undefined}>
        {showDetail && detail}
        <Activity mode={showDetail ? 'hidden' : 'visible'}><div className="book-content-main" inert={showDetail}>
          {compact && mobileSearchTarget && createPortal(<LibrarySearch query={query} interactive={interactive} onChange={value => {
            reserveResultsSpace();
            setQuery(value);
          }}/>, mobileSearchTarget)}
          {history.length > 0 && (
            <section className="book-recent" aria-labelledby="bookRecentTitle">
              <h2 className="book-section-heading" id="bookRecentTitle">{t('library.recent')}</h2>
              <div className="book-recent-games">
                {history.map((product, index) => (
                  <article key={product.id}>{gameLink(product, true, index === 0)}</article>
                ))}
              </div>
            </section>
          )}
          <div className="book-view-header">
            <div className="book-current">
              <h2 className="book-section-heading" ref={headingMotion}>{labels[category]}</h2>
              <span role="status" aria-live="polite">
                {t('library.count', { count: list.length })}
              </span>
            </div>
            {compact && <div className="book-compact-categories">
              <ChoiceGroup type="single" className="book-categories" value={category}
                onValueChange={changeCategory} disabled={!interactive} aria-label={t('library.category')}>
                {categories.filter(({id}) => id === 'all' || id === 'favorites' || !!categoryProducts.get(id)?.length)
                  .map(({id, Icon}) => <ChoiceItem key={id} value={id} className="book-category-choice" data-book-category={id}>
                    <Icon aria-hidden="true"/>
                    <span>{labels[id]}</span>
                    <small className="book-category-count" aria-hidden={category !== id}>
                      <span>{categoryProducts.get(id)?.length ?? 0}</span>
                    </small>
                  </ChoiceItem>)}
              </ChoiceGroup>
            </div>}
            <Toolbar className="book-tools">
              <ChoiceGroup
                type="single"
                className="book-view-switch"
                data-toolbar-segments=""
                data-view={view}
                value={view}
                onValueChange={(value) => {
                  if (value === 'covers' || value === 'list') {
                    reserveResultsSpace();
                    animateLibraryChange(() => {setView(value); writeBookPreference('view', value);});
                  }
                }}
                disabled={!interactive}
                aria-label={t('library.view')}
              >
                <ChoiceItem
                  value="covers"
                  data-book-view="covers"
                  onClick={() => {if (view === 'covers') cancelLibraryMotion();}}
                  className="px-2.5 py-1.5"
                >
                  <LayoutGrid aria-hidden="true" />
                  <span>{t('library.covers')}</span>
                </ChoiceItem>
                <ChoiceItem
                  value="list"
                  data-book-view="list"
                  onClick={() => {if (view === 'list') cancelLibraryMotion();}}
                  className="px-2.5 py-1.5"
                >
                  <List aria-hidden="true" />
                  <span>{t('library.list')}</span>
                </ChoiceItem>
              </ChoiceGroup>
              <Select
                value={sort}
                onValueChange={(value) => {
                  animateLibraryChange(() => setSort(value), {reorder: true});
                }}
                disabled={!interactive}
              >
                <SelectTrigger className="book-sort" aria-label={t('library.sort')}>
                  <SelectValue>{sortOptions.find((option) => option.value === sort)?.label}</SelectValue>
                </SelectTrigger>
                <SelectContent>
                  {sortOptions.map((option) => (
                    <SelectItem key={option.value} value={option.value}>
                      {option.label}
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>
              {tools}
            </Toolbar>
          </div>
          <div
            ref={results}
            className="book-grid"
            id="bookGameGrid"
            data-empty={!list.length || undefined}
            style={resultsMinHeight ? { minHeight: resultsMinHeight } : undefined}
          >
            {list.map((product, index) => (
              <article key={product.id} className="book-card" data-book-card={product.id}>
                {gameLink(product, false, history.length === 0 && index === 0)}
                {favoriteButton(product)}
              </article>
            ))}
            {!list.length && (
              <div className="book-empty">
                <Search aria-hidden="true" />
                <h3>{t(category === 'favorites' && !query ? 'library.emptyFavorites' : 'library.emptySearch')}</h3>
                <Button
                  variant="outline"
                  className="max-[780px]:h-11"
                  disabled={!interactive}
                  onClick={() => {
                    reserveResultsSpace();
                    setQuery('');
                    setCategory('all');
                  }}
                >
                  {t('library.viewAll')}
                </Button>
              </div>
            )}
          </div>
        </div></Activity>
      </div>
    </div>
  );
}

function BookCover({ product, priority }: { product: LibraryProduct; priority: boolean }) {
  const image = useRef<HTMLImageElement>(null);
  const [status, setStatus] = useState<'loading' | 'ready' | 'error'>('loading'),
    source = bookArtwork(product);
  useEffect(() => {
    // A cached image may finish before hydration attaches its load listener.
    if (source && image.current?.complete) setStatus(image.current.naturalWidth ? 'ready' : 'error');
  }, [source]);
  return source && status !== 'error' ? (
    <>
      {status === 'loading' && <Skeleton className="absolute inset-0 rounded-none"/>}
      <img ref={image} src={source} alt="" loading={priority ? 'eager' : 'lazy'} fetchPriority={priority ? 'high' : undefined}
        decoding="async" onLoad={() => setStatus('ready')} onError={() => setStatus('error')} />
    </>
  ) : (
    <span className="book-cover-fallback">
      th{product.number}
      <strong>{product.title}</strong>
    </span>
  );
}
