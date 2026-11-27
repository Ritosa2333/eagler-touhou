import {directoryRoomCanEnter, directoryRoomSpectatorEntry, directoryRoomState, type DirectoryRoom} from '../../services/lobby-directory';
import type {Th09Rating} from '../../services/th09-ratings';
import {PRODUCT_IDS, gameIdForProduct, isMultiplayerProductId, multiplayerConfigForProduct} from '../../../src/contracts/product-catalog.mts';
import {bookGameKind} from '../launcher/products';

export const directoryRoomStatuses = ['open', 'waiting', 'spectate'] as const;
export const directoryRoomModes = ['pve', 'pvp'] as const;
export type DirectoryRoomFiltersValue = {
  status: readonly typeof directoryRoomStatuses[number][];
  mode: readonly typeof directoryRoomModes[number][];
  difficulty: readonly string[];
  players: readonly number[];
};
export const defaultDirectoryRoomFilters: DirectoryRoomFiltersValue = {status:[], mode:[], difficulty:[], players:[]};
export const directoryRoomDifficulties = [...new Set(PRODUCT_IDS.filter(isMultiplayerProductId).flatMap(product =>
  multiplayerConfigForProduct(product)?.difficulties ?? []))];

export function directoryRoomDifficulty(room: DirectoryRoom) {
  const policy = multiplayerConfigForProduct(room.product);
  return policy?.inGameSelection && room.product !== 'th03mp' ? '' : policy?.difficulties[room.difficulty] ?? '';
}

export function hasDirectoryRoomFilters({status, mode, difficulty, players}: DirectoryRoomFiltersValue) {
  return !!(status.length || mode.length || difficulty.length || players.length);
}

function directoryRoomMode(room: DirectoryRoom) {
  const kind = bookGameKind(gameIdForProduct(room.product));
  return kind === 'versus' || kind === 'fighting' ? 'pvp' : 'pve';
}

export function filterDirectoryRooms(rooms: readonly DirectoryRoom[], {status, mode, difficulty, players}: DirectoryRoomFiltersValue) {
  return rooms.filter(room => (!mode.length || mode.includes(directoryRoomMode(room))) &&
    (!difficulty.length || difficulty.includes(directoryRoomDifficulty(room))) &&
    (!players.length || players.includes(room.players)) &&
    (!status.length || status.some(item => item === 'open' ? directoryRoomState(room) === 'recruiting' && directoryRoomCanEnter(room)
      : item === 'waiting' ? room.phase === 'lobby' : directoryRoomSpectatorEntry(room))));
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
