import type {ComponentProps, MouseEvent, ReactNode, Ref} from 'react';
import {ChevronRight, Star, type LucideIcon} from 'lucide-react';
import {Button} from '../../ui/button';
import {Disclosure, DisclosurePanel} from '../../ui/disclosure';
import {useLocale} from '../../i18n';
import {BookRailGameIdentity} from './BookRailGameIdentity';
import {LibrarySearch} from './LibrarySearch';
import {impressFavorite} from './ink-feedback';
import type {LibraryProduct} from './products';

export interface BookRailGroup<Id extends string> {
  id: Id;
  label: string;
  icon: LucideIcon;
  count: number;
  panelId?: string;
  selected?: boolean;
  products?: readonly LibraryProduct[];
  expanded?: boolean;
  onExpand?(expanded: boolean): void;
  onSelect?(): void;
}

type GameAction = {
  href?: string;
  label?: string;
  selected: boolean;
  anchorRef?: Ref<HTMLAnchorElement>;
  marker?: ReactNode;
  onClick(event: MouseEvent<HTMLElement>): void;
};

export function BookGameRail<Id extends string>({label, className, groups, search, interactive, favorites, onFavorite, gameAction}: {
  label: string;
  className?: string;
  groups: readonly BookRailGroup<Id>[];
  search: Omit<ComponentProps<typeof LibrarySearch>, 'interactive' | 'desktop'>;
  interactive: boolean;
  favorites: readonly string[];
  onFavorite(id: string): void;
  gameAction(product: LibraryProduct, group: Id): GameAction;
}) {
  const {t} = useLocale();
  return <aside className={className ? `book-sidebar ${className}` : 'book-sidebar'} aria-label={label}>
    <div className="book-search-slot"><LibrarySearch {...search} interactive={interactive} desktop/></div>
    <nav>
      {groups.map(group => <Disclosure key={group.id} id={group.panelId} open={group.expanded === true} onOpenChange={next => group.onExpand?.(next)}>
        {({open, trigger, panel}) => {
          const Icon = group.icon;
          const separateDisclosure = !!group.onSelect && !!group.onExpand;
          const mark = <ChevronRight aria-hidden="true" data-expanded={open}/>;
          return <div className="book-category" data-book-category-group={group.id} data-selected={group.selected === true}>
            <div className="book-category-row" data-single-action={!separateDisclosure || undefined}>
              <Button variant="ghost" type="button" className="book-category-choice"
                data-book-category={group.id}
                aria-pressed={group.onSelect ? group.selected === true : undefined}
                disabled={!interactive}
                {...(group.onSelect ? {onClick:group.onSelect} : trigger)}>
                <Icon aria-hidden="true"/><span>{group.label}</span><small className="book-category-count">{group.count}</small>
              </Button>
              {separateDisclosure
                ? <Button variant="ghost" size="icon-sm" className="book-category-disclosure" disabled={!interactive}
                    data-book-category-disclosure={group.id} aria-label={t('library.categoryContents', {category:group.label})}
                    {...trigger}>{mark}</Button>
                : <span className="book-category-disclosure" aria-hidden="true">{mark}</span>}
            </div>
            {!!group.onExpand && <DisclosurePanel {...panel} className="book-category-fold">
              <ul className="book-category-games">
                {group.products?.map(product => {
                  const action = gameAction(product, group.id);
                  return <li key={product.id} data-lobby-rail-product={action.marker ? product.id : undefined}>
                    {action.marker}
                    {action.href
                      ? <Button asChild variant="ghost" className="book-rail-game"><a ref={action.anchorRef} href={action.href}
                          data-book-sidebar-product={product.id} aria-current={action.selected ? 'page' : undefined}
                          title={product.title} onClick={interactive ? action.onClick : undefined}>
                          <BookRailGameIdentity product={product}/>
                        </a></Button>
                      : <Button variant="ghost" className="book-rail-game" aria-label={action.label}
                          aria-pressed={action.selected} disabled={!interactive} onClick={action.onClick}>
                          <BookRailGameIdentity product={product}/>
                        </Button>}
                    <Button variant="ghost" size="icon-sm" className="book-favorite" disabled={!interactive}
                      data-book-sidebar-favorite={product.id} aria-label={t('library.favorite', {title:product.title})}
                      aria-pressed={favorites.includes(product.id)}
                      onClick={event => {impressFavorite(event.currentTarget); onFavorite(product.id);}}>
                      <Star aria-hidden="true"/>
                    </Button>
                  </li>;
                })}
              </ul>
            </DisclosurePanel>}
          </div>;
        }}
      </Disclosure>)}
    </nav>
  </aside>;
}
