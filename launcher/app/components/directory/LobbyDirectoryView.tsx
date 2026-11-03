import {BookButton} from '../launcher/BookButton';
import {useEffect, useState, useSyncExternalStore} from 'react';
import {BOOK_PREFERENCES_EVENT, readBookPreference, stringArray} from '../launcher/book-preferences';
import {useLocale} from '../../i18n';
import {LobbySurface, type LobbySurfaceProps} from '../launcher/LobbySurface';
import {DirectoryRoomRows} from './DirectoryRoomRows';
import {LobbyRoomTools} from './LobbyRoomTools';
import {filterDirectoryRooms, type DirectoryRoomFilter} from './directory-room-filter';
import {ListFilter} from 'lucide-react';
import {ActionMenu,ActionMenuRadioGroup,ActionMenuRadioItem} from '../../ui/action-menu';
import type {DirectoryFormMode, DirectoryRoom, LobbyDirectoryService} from '../../services/lobby-directory';

type Derived = 'roomFilters' | 'selectedProduct' | 'connectionWarning' | 'membership' | 'notice' | 'roomTools' | 'roomList' | 'hasRooms' | 'loading' | 'emptyState' | 'roomCount' | 'roomUsersArtwork';
export interface LobbyDirectoryViewProps extends Omit<LobbySurfaceProps, Derived> {
  service: LobbyDirectoryService;
  assetUrl(path: string): string;
  /** The owner prepares the model then opens the original same-URL form entry. */
  onOpenForm(mode: DirectoryFormMode): void;
  onReplay?(): void;
  initialRows?: boolean;
}
/** Original lobby.mts349–385 render conditions, composed once in LobbySurface. */
export function LobbyDirectoryView({service, assetUrl, onOpenForm, onReplay, initialRows = false, ...surface}: LobbyDirectoryViewProps) {
  const {t} = useLocale(), snapshot = useSyncExternalStore(service.subscribe, service.getSnapshot, service.getSnapshot);
  const [roomFilter, setRoomFilter] = useState<DirectoryRoomFilter>('all');
  const [favorites, setFavorites] = useState<string[]>([]);
  useEffect(() => {
    const refresh = () => setFavorites(readBookPreference('multiplayer-favorites', [], stringArray));
    refresh(); window.addEventListener(BOOK_PREFERENCES_EVENT, refresh); window.addEventListener('storage', refresh);
    return () => {window.removeEventListener(BOOK_PREFERENCES_EVENT, refresh); window.removeEventListener('storage', refresh);};
  }, []);
  // Keep matching last-known rows while refreshing; the loading layer does not move them.
  const visibleRooms = snapshot.loading
    ? snapshot.rooms.filter(room => !snapshot.selectedProduct || room.product === snapshot.selectedProduct)
    : snapshot.visibleRooms;
  const filteredRooms = surface.productFilters?.length ? visibleRooms.filter(room => surface.productFilters!.includes(room.product))
    : surface.viewingProduct ? visibleRooms.filter(room => room.product === surface.viewingProduct) : visibleRooms;
  const state = {...snapshot, loading: snapshot.loading && !snapshot.connectionInterrupted,
    visibleRooms: filterDirectoryRooms(filteredRooms, roomFilter).sort((a, b) => Number(favorites.includes(b.product)) - Number(favorites.includes(a.product)))};
  const disabled = state.connection !== 'live' || !!state.mine || state.leaving || !state.products.length;
  const warning = <aside className="lobby-warning lobby-connection-warning" id="connectionWarning" role="alert" hidden={!state.connectionInterrupted}>
    <div><strong><svg viewBox="0 0 24 24" aria-hidden="true"><path d="M10.3 4.7 2.8 18a2 2 0 0 0 1.7 3h15a2 2 0 0 0 1.7-3L13.7 4.7a2 2 0 0 0-3.4 0Z"/><path d="M12 9v4m0 4h.01"/></svg><span id="connectionWarningTitle">{t(state.connection === 'unsupported' ? 'lobby.unsupported' : state.rooms.length ? 'lobby.disconnected' : 'lobby.offline')}</span></strong><p id="connectionWarningHint">{t(state.connection === 'unsupported' ? 'lobby.unsupportedHint' : state.connection === 'missing' ? 'lobby.noService' : 'lobby.refreshHint')}</p></div>
    <BookButton variant="action" className="lobby-button" id="connectionRefresh" type="button" onClick={service.retry}>{t('lobby.refresh')}</BookButton>
  </aside>;
  const membership = <aside className="lobby-membership" id="membership" hidden={!state.mine}><strong id="membershipTitle">{state.mine ? t('lobby.mine', {code: state.mine.code}) : ''}</strong><p id="membershipHint">{t(state.supportsRecovery ? 'lobby.mineHint' : 'lobby.releaseUnsupported')}</p><BookButton variant="action" className="lobby-button" id="releaseMembership" type="button" hidden={!state.mine || !state.supportsRecovery} disabled={!!state.recovering || state.connection !== 'live' || state.leaving || !state.mine?.recoveryToken} onClick={service.releaseMembership}>{t(state.recovering ? 'lobby.releasing' : 'lobby.release')}</BookButton></aside>;
  const roomTools = <LobbyRoomTools disabled={disabled} onCreate={() => onOpenForm('create')} onReplay={onReplay} onJoin={() => onOpenForm('join')} onRefresh={service.refreshRooms}/>;
  const roomFilters=<DirectoryRoomFilters rooms={filteredRooms} value={roomFilter} onChange={setRoomFilter}/>;
  const filtering = roomFilter !== 'all' || !!surface.productFilters?.length || !!surface.viewingProduct;
  const empty = state.loading || state.visibleRooms.length || state.connectionInterrupted ? null : {
    title: state.connection === 'live' && filtering ? (t('directory.noMatches')) : t(state.connection === 'live' ? 'lobby.empty' : state.connection === 'loading' ? 'lobby.connecting' : state.connection === 'unsupported' ? 'lobby.unsupported' : 'lobby.offline'),
    hint: state.connection === 'live' && filtering ? (t('directory.filterHint')) : t(state.connection === 'live' ? 'lobby.emptyHint' : state.connection === 'loading' ? 'lobby.loadingHint' : state.connection === 'unsupported' ? 'lobby.unsupportedHint' : state.connection === 'missing' ? 'lobby.noService' : 'lobby.offlineHint'),
    action: state.connection === 'loading' || (state.connection === 'live' && !!state.mine) ? undefined : {label: state.connection === 'live' && filtering ? (t('directory.clearFilters')) : t(state.connection === 'live' ? 'lobby.create' : 'lobby.retry'), onClick: () => {if (state.connection !== 'live') service.retry(); else if (filtering) {setRoomFilter('all'); if (surface.onProductFiltersChange) surface.onProductFiltersChange([]); else surface.onSelect('');} else onOpenForm('create');}},
  };
  return <LobbySurface {...surface} selectedProduct={surface.viewingProduct || state.selectedProduct} connectionWarning={warning} membership={membership} notice={state.notice} roomTools={roomTools} roomFilters={roomFilters}
    roomList={<DirectoryRoomRows snapshot={state} service={service} assetUrl={assetUrl} initialRows={initialRows}/>}
    hasRooms={!!state.visibleRooms.length} loading={state.loading} emptyState={empty} roomCount={state.loading && !state.visibleRooms.length ? '' : t('lobby.roomCount', {count: state.visibleRooms.length})}
    viewingProduct={surface.viewingProduct} roomUsersArtwork={assetUrl('assets/room-users.svg')}/>;
}

export function DirectoryRoomFilters({rooms,value,onChange}:{rooms:readonly DirectoryRoom[];value:DirectoryRoomFilter;onChange(value:DirectoryRoomFilter):void}) {
  const {t}=useLocale();
  return <ActionMenu variant="selection" label={t('directory.filters')} className="book-room-filter-popup" trigger={<BookButton variant="action" type="button" className="book-room-filter-trigger" data-active={value !== 'all'}><ListFilter aria-hidden="true"/>{t('directory.filters')}</BookButton>}>
    <ActionMenuRadioGroup value={value} onValueChange={next=>onChange(next as DirectoryRoomFilter)}>{(['all','open','waiting','spectate'] as const).map(id=><ActionMenuRadioItem key={id} value={id} data-room-filter={id}>{t(`directory.filter.${id}`)}<span className="ml-auto text-xs text-muted-foreground">{filterDirectoryRooms(rooms,id).length}</span></ActionMenuRadioItem>)}</ActionMenuRadioGroup>
  </ActionMenu>;
}
