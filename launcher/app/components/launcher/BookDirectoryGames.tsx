import {Checkbox} from 'radix-ui';
import {Check,Disc3,Gamepad2,LayoutGrid,List,Monitor,Star,Swords} from 'lucide-react';
import {useEffect,useMemo,useState,type ReactElement,type ReactNode} from 'react';
import {ActionMenu,ActionMenuItem,ActionMenuCheckboxItem,ActionMenuGroup,ActionMenuLabel,ActionMenuRadioGroup,ActionMenuRadioItem} from '../../ui/action-menu';
import {useLocale} from '../../i18n';
import {BookGameRail, type BookRailGroup} from './BookGameRail';
import {BookButton} from './BookButton';
import {BookRailGameIdentity} from './BookRailGameIdentity';
import {useBookDisclosures} from './use-book-disclosures';
import {impressFavorite} from './ink-feedback';
import {beginFavoriteReorder,beginFilterReorder,useFavoriteReorder} from './favorite-reorder';
import {createLibraryProducts,multiplayerGroup,type LibraryProduct,type MultiplayerGroup} from './products';
import type {ProductId} from '../../../src/contracts/product-catalog.mts';
import {bookArtwork} from './book-artwork';
import {BOOK_PREFERENCES_EVENT,readBookPreference,writeBookPreference,stringArray,matchesBookQuery} from './book-preferences';
import './directory-page.css';

function groupGames(products: readonly LibraryProduct[]) {
  return (['versus','original','fighting','pc98'] as const)
    .map(id=>({id,products:products.filter(product=>multiplayerGroup(product)===id)}))
    .filter(group=>group.products.length);
}

const groupLabelKey = (id:MultiplayerGroup) => id==='pc98' ? 'library.category.pc98' : `directory.group.${id}` as const;
const groupIcons = {versus:Swords, original:Monitor, fighting:Gamepad2, pc98:Disc3};
const defaultDisclosures = {versus: true, original: true};

function ProductCover({product}:{product:LibraryProduct}) {
  const [failed,setFailed] = useState(false);
  const art = bookArtwork(product);
  return <span className="book-lobby-cover" aria-hidden="true">{art && !failed
    ? <img src={art} alt="" loading="lazy" decoding="async" onError={()=>setFailed(true)}/>
    : <span>th{product.number}</span>}</span>;
}

function ProductCopy({product}:{product:LibraryProduct}) {
  return <span className="book-lobby-copy">
    <span className="book-lobby-name"><strong title={product.title}>{product.title}</strong><small>th{product.number}</small></span>
    <span className="book-lobby-subtitle" title={product.subtitle}>{product.subtitle}</span>
  </span>;
}

export function BookProductMenu<Product extends ProductId>({products,value,assetUrl,onChange,trigger}:{
  products:readonly Product[];
  value:Product | '';
  assetUrl(path:string):string;
  onChange(product:Product):void;
  trigger:ReactElement;
}) {
  const {t}=useLocale();
  const label=t('lobby.game');
  const groups=useMemo(()=>groupGames(createLibraryProducts(products,assetUrl,id=>`?game=${id}`)
    .sort((a,b)=>Number(a.number)-Number(b.number))),[products,assetUrl]);
  return <ActionMenu variant="selection" label={label} align="start" minimumWidth={280} className="book-lobby-picker" trigger={trigger}>
    <ActionMenuRadioGroup value={value} onValueChange={next=>{const product=products.find(id=>id===next);if(product)onChange(product);}}>
      {groups.map(group=><ActionMenuGroup key={group.id} aria-label={t(groupLabelKey(group.id))} data-lobby-category={group.id}>
        <ActionMenuLabel className="px-2 pt-3 pb-1 text-xs font-medium text-muted-foreground">{t(groupLabelKey(group.id))}</ActionMenuLabel>
        {group.products.map(product=><ActionMenuRadioItem key={product.id} value={product.id} data-product-choice={product.id}
          className="book-lobby-menu-choice h-12">
          <ProductCover product={product}/><ProductCopy product={product}/>
        </ActionMenuRadioItem>)}
      </ActionMenuGroup>)}
    </ActionMenuRadioGroup>
  </ActionMenu>;
}

export function BookDirectoryGames({products,selectedProduct,viewingProduct,productFilters,onProductFiltersChange,onSelect,interactive=true,favoriteProducts,onFavoriteProductsChange,renderContent}:{
  products:readonly LibraryProduct[];selectedProduct?:string;productFilters?:readonly string[];
  viewingProduct?:string;onProductFiltersChange?(products:string[]):void;onSelect(id:string):void;interactive?:boolean;
  favoriteProducts?:readonly string[];onFavoriteProductsChange?(products:string[]):void;
  renderContent?(gameBar:ReactNode):ReactNode;
}) {
  const {t}=useLocale();
  const [savedFavorites,setFavorites]=useState<string[]>([]),[localFilters,setLocalFilters]=useState<string[]>([]);
  const [expanded,setExpanded]=useBookDisclosures('multiplayer-disclosures', defaultDisclosures, interactive);
  const [query,setQuery]=useState('');
  const [menuOrder,setMenuOrder]=useState<readonly string[] | null>(null);
  const favorites=favoriteProducts??savedFavorites;
  useFavoriteReorder('.book-lobby-tile', favorites);
  useEffect(()=>{
    if(favoriteProducts!==undefined)return;
    const refresh=()=>setFavorites(readBookPreference('multiplayer-favorites',[],stringArray));
    refresh();window.addEventListener(BOOK_PREFERENCES_EVENT,refresh);window.addEventListener('storage',refresh);
    return()=>{window.removeEventListener(BOOK_PREFERENCES_EVENT,refresh);window.removeEventListener('storage',refresh);};
  },[favoriteProducts]);
  const filters=productFilters??localFilters,viewProduct=viewingProduct??selectedProduct??'';
  const selected=filters.length?filters:viewProduct?[viewProduct]:[];
  const change=(ids:string[])=>{
    beginFilterReorder();
    if(onProductFiltersChange)onProductFiltersChange(ids);
    else {setLocalFilters(ids);if(viewProduct)onSelect('');}
  };
  const toggle=(id:string,checked:boolean)=>change(checked?[...new Set([...selected,id])]:selected.filter(value=>value!==id));
  const selectOnly=(id:string)=>change([id]);
  const reset=()=>change([]);
  const favorite=(id:string)=>{
    beginFavoriteReorder();
    if(favoriteProducts!==undefined){onFavoriteProductsChange?.(favorites.includes(id)?favorites.filter(value=>value!==id):[...favorites,id]);return;}
    const latest=readBookPreference('multiplayer-favorites',[],stringArray);
    writeBookPreference('multiplayer-favorites',latest.includes(id)?latest.filter(value=>value!==id):[...latest,id]);
  };
  const orderGames=(order:readonly string[])=>products.filter(product=>!product.hidden)
    .sort((a,b)=>Number(order.includes(b.id))-Number(order.includes(a.id))||Number(a.number)-Number(b.number));
  const visible=orderGames(favorites);
  const groups=groupGames(visible);
  const menuGroups=menuOrder ? groupGames(orderGames(menuOrder)) : groups;
  const groupLabel=(id:MultiplayerGroup)=>t(groupLabelKey(id));
  const checkmark=(checked:boolean)=><span className="book-lobby-check" data-checked={checked} aria-hidden="true"><Check/></span>;
  const railProducts = visible.filter(product => matchesBookQuery(product, query));
  const railGroups: BookRailGroup<MultiplayerGroup | 'all'>[] = [
    {id:'all', label:t('directory.allGames'), icon:LayoutGrid, count:railProducts.length,
      selected:!selected.length, onSelect:reset},
    ...groupGames(railProducts).map(({id, products}) => ({
      id, label:groupLabel(id), icon:groupIcons[id], count:products.length, products,
      expanded:expanded[id] ?? !!query.trim(),
      onExpand:(open:boolean) => setExpanded(id,open,true),
    })),
  ];
  const allCopy=<><span className="book-lobby-cover" aria-hidden="true"><LayoutGrid/></span><span className="book-lobby-copy"><span className="book-lobby-name"><strong>{t('directory.allGames')}</strong></span></span></>;
  const choices=<>
    <div className="book-lobby-menu-row" role="presentation" data-selected={!selected.length}>
      <ActionMenuItem className="book-lobby-menu-choice book-lobby-menu-all" disabled={!interactive} onSelect={reset}>{allCopy}</ActionMenuItem>
      <span aria-hidden="true"/>
    </div>
    {menuGroups.map(group=><ActionMenuGroup key={group.id} aria-label={groupLabel(group.id)} data-lobby-category={group.id}><ActionMenuLabel className="px-2 pt-3 pb-1 text-xs font-medium text-muted-foreground">{groupLabel(group.id)}</ActionMenuLabel>{group.products.map(product=><div key={product.id} className="book-lobby-menu-row" role="presentation" data-lobby-product={product.id} data-selected={selected.includes(product.id)}>
      <ActionMenuCheckboxItem className="book-lobby-menu-check" showIndicator={false} aria-label={t('directory.filterGame',{game:product.title})} checked={selected.includes(product.id)} disabled={!interactive} onCheckedChange={checked=>toggle(product.id,checked===true)} onSelect={event=>event.preventDefault()}>{checkmark(selected.includes(product.id))}</ActionMenuCheckboxItem>
      <ActionMenuItem className="book-lobby-menu-choice book-rail-game" aria-label={t('directory.viewGame',{game:product.title})} disabled={!interactive} onSelect={()=>selectOnly(product.id)}><BookRailGameIdentity product={product}/></ActionMenuItem>
      <ActionMenuCheckboxItem className="book-lobby-favorite w-11 min-h-11 justify-center p-0" showIndicator={false} aria-label={t('directory.favoriteGame',{game:product.title})} checked={favorites.includes(product.id)} disabled={!interactive} onCheckedChange={()=>favorite(product.id)} onSelect={event=>{event.preventDefault();if(event.currentTarget instanceof HTMLElement) impressFavorite(event.currentTarget);}}><Star aria-hidden="true"/></ActionMenuCheckboxItem>
    </div>)}</ActionMenuGroup>)}
  </>;
  const gameBar = <nav className="book-lobby-games lobby-library" aria-label={t('directory.gameFilter')}>
    <ActionMenu variant="selection" label={t('directory.games')} align="start" minimumWidth={280} className="book-lobby-picker"
      onOpenChange={open=>{if(open)setMenuOrder(favorites);}}
      trigger={<BookButton variant="action" size="icon" className="book-lobby-picker-trigger size-[var(--lobby-game-size)]" disabled={!interactive} aria-label={t('directory.games')}><List aria-hidden="true"/></BookButton>}>{choices}</ActionMenu>
    <div className="book-lobby-icon-strip" id="lobbyGameRail"><BookButton variant="choice" size="icon" type="button" className="book-lobby-icon book-lobby-all" aria-label={t('directory.allGames')} aria-pressed={!selected.length} disabled={!interactive} onClick={reset}><LayoutGrid aria-hidden="true"/></BookButton>{groups.flatMap(group=>group.products).map(product=><div key={product.id} className="book-lobby-tile" data-selected={selected.includes(product.id)}>
      <BookButton variant="choice" size="icon" type="button" className="book-lobby-icon" title={product.title} aria-label={t('directory.viewGame',{game:product.title})} aria-pressed={selected.includes(product.id)} data-lobby-icon={product.id} disabled={!interactive} onClick={()=>selectOnly(product.id)}><ProductCover product={product}/><span className="book-lobby-short" aria-hidden="true">{product.shortTitle}</span></BookButton>
      <Checkbox.Root asChild checked={selected.includes(product.id)} disabled={!interactive} onCheckedChange={checked=>toggle(product.id,checked===true)}><BookButton variant="choice" className="book-lobby-tile-toggle" data-lobby-toggle={product.id} aria-label={t('directory.filterGame',{game:product.title})}>{checkmark(selected.includes(product.id))}</BookButton></Checkbox.Root>
    </div>)}</div>
  </nav>;
  return <>
    <BookGameRail label={t('directory.gameFilter')} className="lobby-game-sidebar"
      search={{query,onChange:setQuery}} groups={railGroups} interactive={interactive}
      favorites={favorites} onFavorite={favorite}
      gameAction={product => ({
        label:t('directory.viewGame',{game:product.title}), selected:selected.includes(product.id),
        onClick:() => selectOnly(product.id),
        marker:<Checkbox.Root asChild checked={selected.includes(product.id)} disabled={!interactive}
          onCheckedChange={checked=>toggle(product.id,checked===true)}>
          <BookButton variant="choice" className="book-lobby-sidebar-check"
            aria-label={t('directory.filterGame',{game:product.title})}>{checkmark(selected.includes(product.id))}</BookButton>
        </Checkbox.Root>,
      })}/>
    {renderContent ? renderContent(gameBar) : gameBar}
  </>;
}
