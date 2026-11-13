import {directoryRoomCanEnter, directoryRoomSpectatorEntry, directoryRoomState, type DirectoryRoom} from '../../services/lobby-directory';
import type {Th09Rating} from '../../services/th09-ratings';

export type DirectoryRoomFilter = 'all' | 'open' | 'waiting' | 'spectate';
export function filterDirectoryRooms(rooms: readonly DirectoryRoom[], filter: DirectoryRoomFilter) {
  return rooms.filter(room => filter === 'all' || (filter === 'open' ? directoryRoomState(room) === 'recruiting' && directoryRoomCanEnter(room)
    : filter === 'waiting' ? room.phase === 'lobby' : directoryRoomSpectatorEntry(room)));
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
