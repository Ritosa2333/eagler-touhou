import {directoryRoomCanEnter, directoryRoomSpectatorEntry, directoryRoomState, type DirectoryRoom} from '../../services/lobby-directory';
import type {Th09Rating} from '../../services/th09-ratings';
import {PRODUCT_IDS, gameIdForProduct, isMultiplayerProductId, multiplayerConfigForProduct} from '../../../src/contracts/product-catalog.mts';
import {bookGameKind} from '../launcher/products';

export const directoryRoomStatuses = ['open', 'waiting', 'spectate'] as const;
export const directoryRoomModes = ['pve', 'pvp'] as const;
export const directoryRoomCapacities = [2, 3] as const satisfies readonly DirectoryRoom['capacity'][];
export type DirectoryRoomFiltersValue = {
  status: readonly typeof directoryRoomStatuses[number][];
  mode: readonly typeof directoryRoomModes[number][];
  difficulty: readonly string[];
  capacity: readonly DirectoryRoom['capacity'][];
};
export const defaultDirectoryRoomFilters: DirectoryRoomFiltersValue = {status:[], mode:[], difficulty:[], capacity:[]};
export const directoryRoomDifficulties = [...new Set(PRODUCT_IDS.filter(isMultiplayerProductId).flatMap(product =>
  multiplayerConfigForProduct(product)?.difficulties ?? []))];

export function directoryRoomDifficulty(room: DirectoryRoom) {
  const policy = multiplayerConfigForProduct(room.product);
  return policy?.inGameSelection && room.product !== 'th03mp' ? '' : policy?.difficulties[room.difficulty] ?? '';
}

export function hasDirectoryRoomFilters({status, mode, difficulty, capacity}: DirectoryRoomFiltersValue) {
  return !!(status.length || mode.length || difficulty.length || capacity.length);
}

function directoryRoomMode(room: DirectoryRoom) {
  const kind = bookGameKind(gameIdForProduct(room.product));
  return kind === 'versus' || kind === 'fighting' ? 'pvp' : 'pve';
}

export function filterDirectoryRooms(rooms: readonly DirectoryRoom[], {status, mode, difficulty, capacity}: DirectoryRoomFiltersValue) {
  return rooms.filter(room => (!mode.length || mode.includes(directoryRoomMode(room))) &&
    (!difficulty.length || difficulty.includes(directoryRoomDifficulty(room))) &&
    (!capacity.length || capacity.includes(room.capacity)) &&
    (!status.length || status.some(item => matchesStatus(room, item))));
}

function matchesStatus(room: DirectoryRoom, status: typeof directoryRoomStatuses[number]) {
  return status === 'open' ? directoryRoomState(room) === 'recruiting' && directoryRoomCanEnter(room)
    : status === 'waiting' ? room.phase === 'lobby' : directoryRoomSpectatorEntry(room);
}

const facetCounts = () => ({total: 0, options: new Map<string | number, number>()});

/** Count each option with the other three groups' selections applied. */
export function countDirectoryRoomFacets(rooms: readonly DirectoryRoom[], {status, mode, difficulty, capacity}: DirectoryRoomFiltersValue) {
  const counts = {status: facetCounts(), mode: facetCounts(), difficulty: facetCounts(), capacity: facetCounts()};
  const increment = ({options}: ReturnType<typeof facetCounts>, option: string | number) =>
    options.set(option, (options.get(option) ?? 0) + 1);
  for (const room of rooms) {
    const roomMode = directoryRoomMode(room);
    const roomDifficulty = directoryRoomDifficulty(room);
    const modeMatches = !mode.length || mode.includes(roomMode);
    const difficultyMatches = !difficulty.length || difficulty.includes(roomDifficulty);
    const capacityMatches = !capacity.length || capacity.includes(room.capacity);
    const statuses = directoryRoomStatuses.filter(item => matchesStatus(room, item));
    const statusMatches = !status.length || statuses.some(item => status.includes(item));
    if (modeMatches && difficultyMatches && capacityMatches) {
      counts.status.total++;
      for (const item of statuses) increment(counts.status, item);
    }
    if (statusMatches && difficultyMatches && capacityMatches) {
      counts.mode.total++;
      increment(counts.mode, roomMode);
    }
    if (statusMatches && modeMatches && capacityMatches) {
      counts.difficulty.total++;
      if (roomDifficulty) increment(counts.difficulty, roomDifficulty);
    }
    if (statusMatches && modeMatches && difficultyMatches) {
      counts.capacity.total++;
      increment(counts.capacity, room.capacity);
    }
  }
  return counts;
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
