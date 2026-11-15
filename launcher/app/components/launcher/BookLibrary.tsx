import { useEffect, useRef, useState } from 'react';
import { useSearchParams } from 'react-router';
import {
  Search,
  Star,
  LayoutGrid,
  List,
  Monitor,
  Disc3,
  Gamepad2,
  Shapes,
  History,
  X,
} from 'lucide-react';
import { Button } from '../../ui/button';
import { Skeleton } from '../../ui/skeleton';
import { ChoiceGroup, ChoiceItem } from '../../ui/choice-group';
import { InputGroup, InputGroupInput } from '../../ui/input-group';
import { Select, SelectTrigger, SelectValue, SelectContent, SelectItem } from '../../ui/select';
import { useLocale } from '../../i18n';
import { useOptionsTarget } from './options-ownership';
import type { LibraryCardsProps } from './LibraryCards';
import type { LibraryProduct } from './products';
import {
  BOOK_PREFERENCES_EVENT,
  bookCategory,
  filterBookProducts,
  readBookPreference,
  writeBookPreference,
  stringArray,
  type BookCategory,
} from './book-preferences';
import { bookArtwork } from './book-artwork';
import {impressFavorite} from './ink-feedback';
import { BookWorkNumber } from './BookWorkNumber';
import {useLibraryMotion} from './use-library-motion';

type Props = Omit<LibraryCardsProps, 'variant' | 'lobbyLink' | 'inert'>;
const categories: { id: BookCategory; Icon: typeof LayoutGrid }[] = [
  { id: 'all', Icon: LayoutGrid },
  { id: 'favorites', Icon: Star },
  { id: 'windows', Icon: Monitor },
  { id: 'pc98', Icon: Disc3 },
  { id: 'spin-off', Icon: Gamepad2 },
  { id: 'doujin', Icon: Shapes },
];

/** Product membership, settings activation and persistence stay with their existing owners. */
export function BookLibrary({ products, openedProduct, onSelect, onActivate, interactive = true }: Props) {
  const { t } = useLocale(),
    [search] = useSearchParams();
  const [category, setCategory] = useState<BookCategory>('all');
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
    'spin-off': t('library.category.spinOff'),
    doujin: t('library.category.doujin'),
  };
  const sortOptions = [
    { value: 'number', label: t('library.sort.number') },
    { value: 'reverse', label: t('library.sort.reverse') },
    { value: 'name', label: t('library.sort.name') },
  ];
  const available = products.filter((product) => !product.multiplayer && !product.hidden);
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
  const list = filterBookProducts(products, category, favorites, query, sort);
  const toggleFavorite = (id: string) => {
    reserveResultsSpace();
    const next = favorites.includes(id) ? favorites.filter((item) => item !== id) : [...favorites, id];
    setFavorites(next);
    writeBookPreference('favorites', next);
  };
  const activate = (product: LibraryProduct) => {
    onSelect(product.id);
    onActivate(product.id);
  };
  const gameLink = (product: LibraryProduct, recentLink = false, priority = false) => (
    <a
      ref={
        openedProduct === product.id && (!recentLink || !list.some((item) => item.id === product.id))
          ? selectedCard
          : undefined
      }
      className={recentLink ? 'book-recent-link' : 'book-game-link'}
      href={product.href}
      data-book-product={product.id}
      data-game={product.gameId}
      aria-current={openedProduct === product.id ? 'page' : 'false'}
      onClick={
        interactive
          ? (event) => {
              if (event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) return;
              event.preventDefault();
              activate(product);
            }
          : undefined
      }
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
      <aside className="book-sidebar" aria-label={t('library.categories')}>
        <InputGroup className="book-search">
          <Search aria-hidden="true" />
          <InputGroupInput
            ref={input}
            id="librarySearch"
            type="search"
            value={query}
            onChange={(event) => {
              reserveResultsSpace();
              setQuery(event.target.value);
            }}
            disabled={!interactive}
            placeholder={t('library.searchPlaceholder')}
            aria-label={t('library.search')}
            aria-controls="bookGameGrid"
          />
          {query && (
            <Button
              variant="ghost"
              size="icon-sm"
              className="max-[780px]:size-11"
              aria-label={t('library.clearSearch')}
              onClick={() => {
                reserveResultsSpace();
                setQuery('');
                input.current?.focus();
              }}
            >
              <X />
            </Button>
          )}
        </InputGroup>
        <ChoiceGroup
          asChild
          type="single"
          value={category}
          onValueChange={(value) => {
            if (value) {
              reserveResultsSpace();
              const next = value as BookCategory;
              const nextIds = new Set(filterBookProducts(products, next, favorites, query, sort).map(product => product.id));
              animateLibraryChange(() => setCategory(next), view === 'covers' ? nextIds : undefined);
            }
          }}
          disabled={!interactive}
          aria-label={t('library.category')}
        >
          <nav>
            {categories.map(({ id, Icon }) => (
              <ChoiceItem
                key={id}
                value={id}
                data-book-category={id}
                onClick={() => {if (id === category) cancelLibraryMotion();}}
                className="grid min-h-10 w-full shrink-0 pointer-coarse:min-h-11 grid-cols-[18px_minmax(0,1fr)_auto] justify-start gap-2.5 px-2.5 text-left text-[13px] max-[780px]:inline-flex max-[780px]:min-h-11 max-[780px]:w-auto max-[780px]:min-w-max max-[780px]:text-xs"
              >
                <Icon aria-hidden="true" />
                <span>{labels[id]}</span>
                <small>
                  {
                    available.filter(
                      (product) =>
                        id === 'all' ||
                        (id === 'favorites' ? favorites.includes(product.id) : bookCategory(product) === id),
                    ).length
                  }
                </small>
              </ChoiceItem>
            ))}
          </nav>
        </ChoiceGroup>
      </aside>
      <div className="book-content">
        <div className="book-content-main">
          {history.length > 0 && (
            <section className="book-recent" aria-labelledby="bookRecentTitle">
              <div className="book-section-heading">
                <History aria-hidden="true" />
                <h2 id="bookRecentTitle">{t('library.recent')}</h2>
              </div>
              <div className="book-recent-games">
                {history.map((product, index) => (
                  <article key={product.id}>{gameLink(product, true, index === 0)}</article>
                ))}
              </div>
            </section>
          )}
          <div className="book-view-header">
            <div className="book-current">
              <h2>{labels[category]}</h2>
              <span role="status" aria-live="polite">
                {t('library.count', { count: list.length })}
              </span>
            </div>
            <div className="book-tools">
              <ChoiceGroup
                type="single"
                className="book-view-switch"
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
                  className="min-h-7 px-2.5 pointer-coarse:min-h-11 py-1.5 text-xs data-[state=on]:bg-card max-[780px]:min-h-11"
                >
                  <LayoutGrid aria-hidden="true" />
                  <span>{t('library.covers')}</span>
                </ChoiceItem>
                <ChoiceItem
                  value="list"
                  data-book-view="list"
                  onClick={() => {if (view === 'list') cancelLibraryMotion();}}
                  className="min-h-7 px-2.5 pointer-coarse:min-h-11 py-1.5 text-xs data-[state=on]:bg-card max-[780px]:min-h-11"
                >
                  <List aria-hidden="true" />
                  <span>{t('library.list')}</span>
                </ChoiceItem>
              </ChoiceGroup>
              <Select
                value={sort}
                onValueChange={(value) => {
                  reserveResultsSpace();
                  setSort(value);
                }}
                disabled={!interactive}
              >
                <SelectTrigger className="book-sort text-xs" aria-label={t('library.sort')}>
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
            </div>
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
                <Button
                  variant="ghost"
                  size="icon"
                  className="book-favorite rounded-full"
                  data-book-favorite={product.id}
                  disabled={!interactive}
                  aria-label={t('library.favorite', { title: product.title })}
                  aria-pressed={favorites.includes(product.id)}
                  onClick={(event) => {impressFavorite(event.currentTarget); toggleFavorite(product.id);}}
                >
                  <Star aria-hidden="true" />
                </Button>
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
        </div>
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
