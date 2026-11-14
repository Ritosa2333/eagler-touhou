import {BookButton} from '../launcher/BookButton';
import {useEffect, useLayoutEffect, useRef, useState, useSyncExternalStore} from 'react';
import {BOOK_PREFERENCES_EVENT, readBookPreference, stringArray} from '../launcher/book-preferences';
import {multiplayerGroup} from '../launcher/products';
import {gameIdForProduct} from '../../../src/contracts/product-catalog.mts';
import {beginFilterReorder,useFavoriteReorder} from '../launcher/favorite-reorder';
import {useLocale} from '../../i18n';
import {LobbySurface, type LobbySurfaceProps} from '../launcher/LobbySurface';
import {DirectoryRoomRows} from './DirectoryRoomRows';
import {LobbyRoomTools} from './LobbyRoomTools';
import {filterDirectoryRooms, orderTh09OpenRooms, type DirectoryRoomFilter} from './directory-room-filter';
import {useTh09Ratings} from '../../services/th09-ratings';
import {ListFilter} from 'lucide-react';
import {ActionMenu,ActionMenuRadioGroup,ActionMenuRadioItem} from '../../ui/action-menu';
import type {DirectoryFormMode, DirectoryRoom, LobbyDirectoryService} from '../../services/lobby-directory';

type Derived = 'roomFilters' | 'selectedProduct' | 'connectionWarning' | 'membership' | 'notice' | 'roomTools' | 'roomList' | 'hasRooms' | 'loading' | 'emptyState' | 'roomCount' | 'roomUsersArtwork';
export interface LobbyDirectoryViewProps extends Omit<LobbySurfaceProps, Derived> {
  service: LobbyDirectoryService;
  assetUrl(path: string): string;
  /** The owner prepares the model then opens the original same-URL form entry. */
  onOpenForm(mode: DirectoryFormMode): void;
  initialRows?: boolean;
}
/** Original lobby.mts349–385 render conditions, composed once in LobbySurface. */
export function LobbyDirectoryView({service, assetUrl, onOpenForm, initialRows = false, ...surface}: LobbyDirectoryViewProps) {
  const {t} = useLocale(), snapshot = useSyncExternalStore(service.subscribe, service.getSnapshot, service.getSnapshot);
  const [roomFilter, setRoomFilter] = useState<DirectoryRoomFilter>('all');
  const seenRooms = useRef<Set<string> | null>(null);
  const arrivalTimers = useRef(new Set<number>());
  const [arrivals, setArrivals] = useState<ReadonlySet<string>>(() => new Set());
  useEffect(() => () => {arrivalTimers.current.forEach(timer => {window.clearTimeout(timer);}); arrivalTimers.current.clear();}, []);
  useEffect(() => {
    if (snapshot.connection !== 'live' || snapshot.loading) return;
    const keys = snapshot.rooms.map(room => `${room.product}-${room.code}`);
    if (!seenRooms.current) {seenRooms.current = new Set(keys); return;}
    const added = keys.filter(key => !seenRooms.current!.has(key));
    seenRooms.current = new Set(keys);
    if (!added.length) return;
    setArrivals(current => new Set([...current, ...added]));
    const timer = window.setTimeout(() => {
      arrivalTimers.current.delete(timer);
      setArrivals(current => {const next = new Set(current); added.forEach(key => {next.delete(key);}); return next;});
    }, 850);
    arrivalTimers.current.add(timer);
  }, [snapshot.rooms, snapshot.connection, snapshot.loading]);
  const [favorites, setFavorites] = useState<string[]>([]);
  useFavoriteReorder('.lobby-room-list > .lobby-room-row', favorites, JSON.stringify([roomFilter, surface.productFilters, surface.viewingProduct]));
  useEffect(() => {
    const refresh = () => setFavorites(readBookPreference('multiplayer-favorites', [], stringArray));
    refresh(); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => {window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
  }, []);
  // Keep matching last-known rows while refreshing; the loading layer does not move them.
  const visibleRooms = snapshot.loading
    ? snapshot.rooms.filter(room => !snapshot.selectedProduct || room.product === snapshot.selectedProduct)
    : surface.productFilters !== undefined ? snapshot.rooms : snapshot.visibleRooms;
  const filteredRooms = surface.productFilters?.length ? visibleRooms.filter(room => surface.productFilters!.includes(room.product))
    : surface.viewingProduct ? visibleRooms.filter(room => room.product === surface.viewingProduct) : visibleRooms;
  const allGames = !surface.productFilters?.length && !surface.viewingProduct && !snapshot.selectedProduct;
  const oldGame = (room: DirectoryRoom) => multiplayerGroup({gameId: gameIdForProduct(room.product)}) === 'pc98';
  const filtered = filterDirectoryRooms(filteredRooms, roomFilter).sort((a, b) =>
    (allGames ? Number(oldGame(a)) - Number(oldGame(b)) : 0) ||
    Number(favorites.includes(b.product)) - Number(favorites.includes(a.product)));
  const th09Rooms = filtered.filter(room => room.product === 'th09mp');
  const {players, mine, cards} = useTh09Ratings(surface.interactive !== false && th09Rooms.length > 0,
    th09Rooms.flatMap(room => room.seats.map(seat => seat?.profile?.username)));
  const state = {...snapshot, loading: snapshot.loading && !snapshot.connectionInterrupted,
    visibleRooms: orderTh09OpenRooms(filtered, players, mine)};
  const previousVisible = useRef<readonly DirectoryRoom[]>([]);
  const exitTimers = useRef(new Map<string, number>());
  const [exiting, setExiting] = useState<readonly DirectoryRoom[]>([]);
  useEffect(() => () => {exitTimers.current.forEach(timer => {window.clearTimeout(timer);}); exitTimers.current.clear();}, []);
  useLayoutEffect(() => {
    if (snapshot.connection !== 'live' || snapshot.loading) return;
    const key = (room: DirectoryRoom) => `${room.product}-${room.code}`;
    const live = new Set(snapshot.rooms.map(key));
    const removed = previousVisible.current.filter(room => !live.has(key(room)));
    previousVisible.current = state.visibleRooms;
    const returned = new Set<string>();
    if (live.size) {
      for (const [id, timer] of exitTimers.current) if (live.has(id)) {
        window.clearTimeout(timer); exitTimers.current.delete(id); returned.add(id);
      }
    }
    if (removed.length || returned.size) {
      const newlyRemoved = removed.filter(room => !exitTimers.current.has(key(room)));
      setExiting(current => [...current.filter(room => !live.has(key(room))), ...newlyRemoved]);
      for (const room of newlyRemoved) {
        const id = key(room);
        exitTimers.current.set(id, window.setTimeout(() => {
          exitTimers.current.delete(id); setExiting(current => current.filter(item => key(item) !== id));
        }, 500));
      }
    }
  }, [snapshot.rooms, snapshot.connection, snapshot.loading, state.visibleRooms]);
  const exitingKeys = new Set(exiting.map(room => `${room.product}-${room.code}`));
  const rows = {...state, visibleRooms: [...state.visibleRooms, ...exiting.filter(room => !snapshot.rooms.some(live => live.product === room.product && live.code === room.code))]};
  const disabled = state.connection !== 'live' || !!state.mine || state.leaving || !state.products.length;
  const warning = <aside className="lobby-warning lobby-connection-warning" id="connectionWarning" role="alert" hidden={!state.connectionInterrupted}>
    <div><svg className="lobby-connection-icon" viewBox="0 0 24 24" aria-hidden="true"><path d="M10.3 4.7 2.8 18a2 2 0 0 0 1.7 3h15a2 2 0 0 0 1.7-3L13.7 4.7a2 2 0 0 0-3.4 0Z"/><path d="M12 9v4m0 4h.01"/></svg><strong id="connectionWarningTitle">{t(state.connection === 'unsupported' ? 'lobby.unsupported' : state.rooms.length ? 'lobby.disconnected' : 'lobby.offline')}</strong><p id="connectionWarningHint">{t(state.connection === 'unsupported' ? 'lobby.unsupportedHint' : state.connection === 'missing' ? 'lobby.noService' : 'lobby.refreshHint')}{state.connectionDetail && ` | ${state.connectionDetail}`}</p></div>
    <BookButton variant="action" className="lobby-button" id="connectionRefresh" type="button" onClick={service.retry}>{t('lobby.refresh')}</BookButton>
  </aside>;
  const membership = <aside className="lobby-membership" id="membership" hidden={!state.mine}><strong id="membershipTitle">{state.mine ? t('lobby.mine', {code: state.mine.code}) : ''}</strong><p id="membershipHint">{t(state.supportsRecovery ? 'lobby.mineHint' : 'lobby.releaseUnsupported')}</p><BookButton variant="action" className="lobby-button" id="releaseMembership" type="button" hidden={!state.mine || !state.supportsRecovery} disabled={!!state.recovering || state.connection !== 'live' || state.leaving || !state.mine?.recoveryToken} onClick={service.releaseMembership}>{t(state.recovering ? 'lobby.releasing' : 'lobby.release')}</BookButton></aside>;
  const roomTools = <LobbyRoomTools disabled={disabled} loading={state.loading} onCreate={() => onOpenForm('create')} onJoin={() => onOpenForm('join')} onRefresh={service.refreshRooms}/>;
  const roomFilters=<DirectoryRoomFilters rooms={filteredRooms} value={roomFilter} onChange={next => {beginFilterReorder(); setRoomFilter(next);}}/>;
  const filtering = roomFilter !== 'all' || !!surface.productFilters?.length || !!surface.viewingProduct;
  const empty = state.loading || state.visibleRooms.length || state.connectionInterrupted ? null : {
    title: state.connection === 'live' && filtering ? (t('directory.noMatches')) : t(state.connection === 'live' ? 'lobby.empty' : state.connection === 'loading' ? 'lobby.connecting' : state.connection === 'unsupported' ? 'lobby.unsupported' : 'lobby.offline'),
    hint: state.connection === 'live' && filtering ? (t('directory.filterHint')) : t(state.connection === 'live' ? 'lobby.emptyHint' : state.connection === 'loading' ? 'lobby.loadingHint' : state.connection === 'unsupported' ? 'lobby.unsupportedHint' : state.connection === 'missing' ? 'lobby.noService' : 'lobby.offlineHint'),
    action: state.connection === 'loading' || (state.connection === 'live' && !!state.mine) ? undefined : {label: state.connection === 'live' && filtering ? (t('directory.clearFilters')) : t(state.connection === 'live' ? 'lobby.create' : 'lobby.retry'), onClick: () => {if (state.connection !== 'live') service.retry(); else if (filtering) {beginFilterReorder(); setRoomFilter('all'); if (surface.onProductFiltersChange) surface.onProductFiltersChange([]); else surface.onSelect('');} else onOpenForm('create');}},
  };
  return <LobbySurface {...surface} selectedProduct={surface.viewingProduct || state.selectedProduct} connectionWarning={warning} membership={membership} notice={state.notice} roomTools={roomTools} roomFilters={roomFilters}
    roomList={<DirectoryRoomRows snapshot={rows} service={service} assetUrl={assetUrl} ratings={players} myRating={mine} cards={cards} initialRows={initialRows} arrivals={arrivals} exiting={exitingKeys}/>}
    hasRooms={!!rows.visibleRooms.length} loading={state.loading} emptyState={rows.visibleRooms.length ? null : empty} roomCount={state.loading && !state.visibleRooms.length ? '' : t('lobby.roomCount', {count: state.visibleRooms.length})}
    viewingProduct={surface.viewingProduct} roomUsersArtwork={assetUrl('assets/room-users.svg')}/>;
}

export function DirectoryRoomFilters({rooms,value,onChange}:{rooms:readonly DirectoryRoom[];value:DirectoryRoomFilter;onChange(value:DirectoryRoomFilter):void}) {
  const {t}=useLocale();
  return <ActionMenu variant="selection" align="start" sideOffset={10} label={t('directory.filters')} className="book-room-filter-popup" trigger={<BookButton variant="action" type="button" className="book-room-filter-trigger" aria-label={t('directory.filters')} data-active={value !== 'all'}><ListFilter aria-hidden="true"/><span>{t('directory.filters')}</span></BookButton>}>
    <ActionMenuRadioGroup value={value} onValueChange={next=>onChange(next as DirectoryRoomFilter)}>{(['all','open','waiting','spectate'] as const).map(id=><ActionMenuRadioItem key={id} value={id} data-room-filter={id}>{t(`directory.filter.${id}`)}<span className="ml-auto text-xs text-muted-foreground">{filterDirectoryRooms(rooms,id).length}</span></ActionMenuRadioItem>)}</ActionMenuRadioGroup>
  </ActionMenu>;
}
