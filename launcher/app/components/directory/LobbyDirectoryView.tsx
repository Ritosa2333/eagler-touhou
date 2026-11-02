import {BookButton} from '../launcher/BookButton';
import {useEffect, useState, useSyncExternalStore} from 'react';
import {BOOK_PREFERENCES_EVENT, readBookPreference, stringArray} from '../launcher/book-preferences';
import {useLocale} from '../../i18n';
import {LobbySurface, type LobbySurfaceProps} from '../launcher/LobbySurface';
import {DirectoryRoomRows} from './DirectoryRoomRows';
import {LobbyRoomTools} from './LobbyRoomTools';
import {filterDirectoryRooms, type DirectoryRoomFilter} from './directory-room-filter';
import type {DirectoryFormMode, LobbyDirectoryService} from '../../services/lobby-directory';

type Derived = 'roomFilters' | 'selectedProduct' | 'connectionWarning' | 'membership' | 'notice' | 'roomTools' | 'roomList' | 'hasRooms' | 'loading' | 'emptyState' | 'roomCount' | 'connectionNote' | 'roomUsersArtwork';
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
  // Background reconnects keep the warning and last known rows on screen.
  // Only the first connection presents a loading layer.
  const visibleRooms = snapshot.connectionInterrupted && snapshot.loading
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
  const roomFilters = <fieldset className="book-room-filters" aria-label={t('directory.filters')}>
    {([{id: 'all', key: 'directory.filter.all'}, {id: 'open', key: 'directory.filter.open'}, {id: 'waiting', key: 'directory.filter.waiting'}, {id: 'spectate', key: 'directory.filter.spectate'}] as const).map(filter =>
      <BookButton variant="choice" type="button" key={filter.id} data-room-filter={filter.id} aria-pressed={roomFilter === filter.id} onClick={() => setRoomFilter(filter.id)}>{t(filter.key)}<span>{filterDirectoryRooms(filteredRooms, filter.id).length}</span></BookButton>)}
    <span className="book-room-filter-count" role="status">{t('directory.shown', {count: state.visibleRooms.length})}</span>
  </fieldset>;
  const filtering = roomFilter !== 'all' || !!surface.productFilters?.length || !!surface.viewingProduct;
  const empty = state.loading || state.visibleRooms.length || state.connectionInterrupted ? null : {
    title: state.connection === 'live' && filtering ? (t('directory.noMatches')) : t(state.connection === 'live' ? 'lobby.empty' : state.connection === 'loading' ? 'lobby.connecting' : state.connection === 'unsupported' ? 'lobby.unsupported' : 'lobby.offline'),
    hint: state.connection === 'live' && filtering ? (t('directory.filterHint')) : t(state.connection === 'live' ? 'lobby.emptyHint' : state.connection === 'loading' ? 'lobby.loadingHint' : state.connection === 'unsupported' ? 'lobby.unsupportedHint' : state.connection === 'missing' ? 'lobby.noService' : 'lobby.offlineHint'),
    action: state.connection === 'loading' || (state.connection === 'live' && !!state.mine) ? undefined : {label: state.connection === 'live' && filtering ? (t('directory.clearFilters')) : t(state.connection === 'live' ? 'lobby.create' : 'lobby.retry'), onClick: () => {if (state.connection !== 'live') service.retry(); else if (filtering) {setRoomFilter('all'); if (surface.onProductFiltersChange) surface.onProductFiltersChange([]); else surface.onSelect('');} else onOpenForm('create');}},
  };
  return <LobbySurface {...surface} selectedProduct={surface.viewingProduct || state.selectedProduct} connectionWarning={warning} membership={membership} notice={state.notice} roomTools={roomTools} roomFilters={roomFilters}
    roomList={<DirectoryRoomRows snapshot={state} service={service} assetUrl={assetUrl} initialRows={initialRows}/>}
    hasRooms={!!state.visibleRooms.length} loading={state.loading} emptyState={empty} roomCount={state.loading ? '' : t('lobby.roomCount', {count: state.visibleRooms.length})}
    viewingProduct={surface.viewingProduct} roomUsersArtwork={assetUrl('assets/room-users.svg')}/>;
}
