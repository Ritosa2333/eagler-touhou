import {directoryRoomCanEnter, directoryRoomSpectatorEntry, directoryRoomState, type DirectoryRoom} from '../../services/lobby-directory';
import type {Th09Rating} from '../../services/th09-ratings';
import {PRODUCT_IDS, isMultiplayerProductId, multiplayerConfigForProduct} from '../../../src/contracts/product-catalog.mts';

export const directoryRoomStatuses = ['all', 'open', 'waiting', 'spectate'] as const;
export type DirectoryRoomFiltersValue = {status: typeof directoryRoomStatuses[number]; difficulty: string; players: string};
export const defaultDirectoryRoomFilters: DirectoryRoomFiltersValue = {status:'all', difficulty:'', players:''};
export const directoryRoomDifficulties = [...new Set(PRODUCT_IDS.filter(isMultiplayerProductId).flatMap(product =>
  multiplayerConfigForProduct(product)?.difficulties ?? []))];

export function directoryRoomDifficulty(room: DirectoryRoom) {
  const policy = multiplayerConfigForProduct(room.product);
  return policy?.inGameSelection && room.product !== 'th03mp' ? '' : policy?.difficulties[room.difficulty] ?? '';
}

export function hasDirectoryRoomFilters({status, difficulty, players}: DirectoryRoomFiltersValue) {
  return status !== 'all' || difficulty !== '' || players !== '';
}

export function filterDirectoryRooms(rooms: readonly DirectoryRoom[], {status, difficulty, players}: DirectoryRoomFiltersValue) {
  return rooms.filter(room => (!difficulty || directoryRoomDifficulty(room) === difficulty) &&
    (!players || room.players === Number(players)) &&
    (status === 'all' || (status === 'open' ? directoryRoomState(room) === 'recruiting' && directoryRoomCanEnter(room)
      : status === 'waiting' ? room.phase === 'lobby' : directoryRoomSpectatorEntry(room))));
}

export function orderTh09OpenRooms(rooms: DirectoryRoom[], players: Readonly<Record<string, Th09Rating>>, mine?: Th09Rating) {
  if (!mine) return rooms;
  const own = mine;
  const waiting = (room: DirectoryRoom) => room.product === 'th09mp' && room.phase === 'lobby' && room.joinable && room.players === 1;
  function distance(room: DirectoryRoom) {
    const name = room.seats.find(seat => seat?.online)?.profile?.username;
    const rating = name ? players[name] : undefined;
    return rating?.group === own.group ? Math.abs(rating.rating - own.rating) : Infinity;
  }
  const ordered = rooms.filter(waiting).sort((left, right) => distance(left) - distance(right));
  let next = 0;
  return rooms.map(room => waiting(room) ? ordered[next++] : room);
}
