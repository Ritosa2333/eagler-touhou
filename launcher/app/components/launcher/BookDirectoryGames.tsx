import {Checkbox} from 'radix-ui';
import {Check,LayoutGrid,List,Star} from 'lucide-react';
import {useEffect,useState} from 'react';
import {ActionMenu,ActionMenuItem,ActionMenuCheckboxItem,ActionMenuGroup,ActionMenuLabel} from '../../ui/action-menu';
import {useLocale} from '../../i18n';
import {BookButton} from './BookButton';
import {impressFavorite} from './ink-feedback';
import {multiplayerGroup,type LibraryProduct,type MultiplayerGroup} from './products';
import {bookArtwork} from './book-artwork';
import {BOOK_PREFERENCES_EVENT,readBookPreference,writeBookPreference,stringArray} from './book-preferences';

export function BookDirectoryGames({products,selectedProduct,viewingProduct,productFilters,onProductFiltersChange,onSelect,interactive=true,favoriteProducts,onFavoriteProductsChange}:{
  products:readonly LibraryProduct[];selectedProduct?:string;productFilters?:readonly string[];
  viewingProduct?:string;onProductFiltersChange?(products:string[]):void;onSelect(id:string):void;interactive?:boolean;
  favoriteProducts?:readonly string[];onFavoriteProductsChange?(products:string[]):void;
}) {
  const {t}=useLocale();
  const [savedFavorites,setFavorites]=useState<string[]>([]),[localFilters,setLocalFilters]=useState<string[]>([]);
  const favorites=favoriteProducts??savedFavorites;
  const [failedArt,setFailedArt]=useState<ReadonlySet<string>>(()=>new Set());
  useEffect(()=>{
    if(favoriteProducts!==undefined)return;
    const refresh=()=>setFavorites(readBookPreference('multiplayer-favorites',[],stringArray));
    refresh();window.addEventListener(BOOK_PREFERENCES_EVENT,refresh);window.addEventListener('storage',refresh);
    return()=>{window.removeEventListener(BOOK_PREFERENCES_EVENT,refresh);window.removeEventListener('storage',refresh);};
  },[favoriteProducts]);
  const filters=productFilters??localFilters,viewProduct=viewingProduct??selectedProduct??'';
  const selected=filters.length?filters:viewProduct?[viewProduct]:[];
  const change=(ids:string[])=>{
    if(onProductFiltersChange)onProductFiltersChange(ids);
    else {setLocalFilters(ids);if(viewProduct)onSelect('');}
  };
  const toggle=(id:string,checked:boolean)=>change(checked?[...new Set([...selected,id])]:selected.filter(value=>value!==id));
  const selectOnly=(id:string)=>change([id]);
  const reset=()=>change([]);
  const favorite=(id:string)=>{
    if(favoriteProducts!==undefined){onFavoriteProductsChange?.(favorites.includes(id)?favorites.filter(value=>value!==id):[...favorites,id]);return;}
    const latest=readBookPreference('multiplayer-favorites',[],stringArray);
    writeBookPreference('multiplayer-favorites',latest.includes(id)?latest.filter(value=>value!==id):[...latest,id]);
  };
  const visible=products.filter(p=>!p.hidden).sort((a,b)=>Number(favorites.includes(b.id))-Number(favorites.includes(a.id))||Number(a.number)-Number(b.number));
  const groups=(['versus','original','fighting'] as const).map(id=>({id,products:visible.filter(product=>multiplayerGroup(product)===id)})).filter(group=>group.products.length);
  const groupLabel=(id:MultiplayerGroup)=>t(id==='versus'?'directory.group.versus':id==='fighting'?'directory.group.fighting':'directory.group.original');
  const cover=(product:LibraryProduct)=>{
    const art=bookArtwork(product);
    return <span className="book-lobby-cover" aria-hidden="true">{art&&!failedArt.has(product.id)?<img src={art} alt="" loading="lazy" decoding="async" onError={()=>setFailedArt(previous=>new Set(previous).add(product.id))}/>:<span>th{product.number}</span>}</span>;
  };
  const copy=(product:LibraryProduct)=><span className="book-lobby-copy"><span className="book-lobby-name"><strong title={product.title}>{product.title}</strong><small>th{product.number}</small></span><span className="book-lobby-subtitle" title={product.subtitle}>{product.subtitle}</span></span>;
  const checkmark=(checked:boolean)=><span className="book-lobby-check" aria-hidden="true">{checked&&<Check/>}</span>;
  const allCopy=<><span className="book-lobby-cover" aria-hidden="true"><LayoutGrid/></span><span className="book-lobby-copy"><span className="book-lobby-name"><strong>{t('directory.allGames')}</strong></span></span></>;
  const choices=<>
    <div className="book-lobby-menu-row" role="presentation" data-selected={!selected.length}>
      <ActionMenuCheckboxItem className="book-lobby-menu-check w-9 min-h-11 justify-center p-0" showIndicator={false} aria-label={t('directory.allGames')} checked={!selected.length} disabled={!interactive} onCheckedChange={reset} onSelect={event=>event.preventDefault()}>{checkmark(!selected.length)}</ActionMenuCheckboxItem>
      <ActionMenuItem className="pr-1 pl-0 book-lobby-menu-choice book-lobby-menu-all" disabled={!interactive} onSelect={reset}>{allCopy}</ActionMenuItem>
      <span aria-hidden="true"/>
    </div>
    {groups.map(group=><ActionMenuGroup key={group.id} aria-label={groupLabel(group.id)} data-lobby-category={group.id}><ActionMenuLabel className="px-2 pt-3 pb-1 text-xs font-medium text-muted-foreground">{groupLabel(group.id)}</ActionMenuLabel>{group.products.map(product=><div key={product.id} className="book-lobby-menu-row" role="presentation" data-lobby-product={product.id} data-selected={selected.includes(product.id)}>
      <ActionMenuCheckboxItem className="book-lobby-menu-check w-9 min-h-11 justify-center p-0" showIndicator={false} aria-label={t('directory.filterGame',{game:product.title})} checked={selected.includes(product.id)} disabled={!interactive} onCheckedChange={checked=>toggle(product.id,checked===true)} onSelect={event=>event.preventDefault()}>{checkmark(selected.includes(product.id))}</ActionMenuCheckboxItem>
      <ActionMenuItem className="pr-1 pl-0 book-lobby-menu-choice" aria-label={t('directory.viewGame',{game:product.title})} disabled={!interactive} onSelect={()=>selectOnly(product.id)}>{cover(product)}{copy(product)}</ActionMenuItem>
      <ActionMenuCheckboxItem className="book-lobby-favorite w-9 min-h-11 justify-center p-0" showIndicator={false} aria-label={t('directory.favoriteGame',{game:product.title})} checked={favorites.includes(product.id)} disabled={!interactive} onCheckedChange={()=>favorite(product.id)} onSelect={event=>event.preventDefault()}><Star aria-hidden="true"/></ActionMenuCheckboxItem>
    </div>)}</ActionMenuGroup>)}
  </>;
  return <nav className="book-lobby-games" aria-label={t('directory.gameFilter')}>
    <div className="book-lobby-desktop"><div className="book-lobby-full-list">
      <div className="book-lobby-item" data-selected={!selected.length}><Checkbox.Root asChild checked={!selected.length} disabled={!interactive} onCheckedChange={reset}><BookButton variant="choice" className="book-lobby-check-control" aria-label={t('directory.allGames')}>{checkmark(!selected.length)}</BookButton></Checkbox.Root><BookButton variant="choice" className="book-lobby-game book-lobby-all" disabled={!interactive} onClick={reset}>{allCopy}</BookButton><span aria-hidden="true"/></div>
      {groups.map(group=><section key={group.id} className="book-lobby-group" aria-label={groupLabel(group.id)} data-lobby-category={group.id}><h2 className="book-lobby-group-label">{groupLabel(group.id)}</h2>{group.products.map(product=><div key={product.id} className="book-lobby-item" data-lobby-product={product.id} data-selected={selected.includes(product.id)}>
      <Checkbox.Root asChild checked={selected.includes(product.id)} disabled={!interactive} onCheckedChange={checked=>toggle(product.id,checked===true)}><BookButton variant="choice" className="book-lobby-check-control" aria-label={t('directory.filterGame',{game:product.title})}>{checkmark(selected.includes(product.id))}</BookButton></Checkbox.Root>
      <BookButton variant="choice" type="button" className="book-lobby-game" aria-label={t('directory.viewGame',{game:product.title})} disabled={!interactive} onClick={()=>selectOnly(product.id)}>{cover(product)}{copy(product)}</BookButton>
      <BookButton variant="icon" type="button" className="book-lobby-favorite" aria-label={t('directory.favoriteGame',{game:product.title})} aria-pressed={favorites.includes(product.id)} disabled={!interactive} onClick={event=>{impressFavorite(event.currentTarget);favorite(product.id);}}><Star aria-hidden="true"/></BookButton>
    </div>)}</section>)}</div></div>
    <div className="book-lobby-mobile"><ActionMenu variant="selection" label={t('directory.games')} align="start" minimumWidth={280} className="book-lobby-picker" trigger={<BookButton variant="action" size="icon" className="book-lobby-picker-trigger size-12" disabled={!interactive} aria-label={t('directory.games')}><List aria-hidden="true"/></BookButton>}>{choices}</ActionMenu>
      <div className="book-lobby-icon-strip" id="lobbyGameRail"><BookButton variant="choice" size="icon" type="button" className="book-lobby-icon book-lobby-all" aria-label={t('directory.allGames')} aria-pressed={!selected.length} disabled={!interactive} onClick={reset}><LayoutGrid aria-hidden="true"/></BookButton>{visible.map(product=><div key={product.id} className="book-lobby-tile" data-selected={selected.includes(product.id)}>
        <BookButton variant="choice" size="icon" type="button" className="book-lobby-icon" title={product.title} aria-label={t('directory.viewGame',{game:product.title})} aria-pressed={selected.includes(product.id)} data-lobby-icon={product.id} disabled={!interactive} onClick={()=>selectOnly(product.id)}>{cover(product)}<span className="book-lobby-short" aria-hidden="true">{product.shortTitle}</span></BookButton>
        <Checkbox.Root asChild checked={selected.includes(product.id)} disabled={!interactive} onCheckedChange={checked=>toggle(product.id,checked===true)}><BookButton variant="choice" className="book-lobby-tile-toggle" data-lobby-toggle={product.id} aria-label={t('directory.filterGame',{game:product.title})}>{checkmark(selected.includes(product.id))}</BookButton></Checkbox.Root>
      </div>)}</div>
    </div>
  </nav>;
}
