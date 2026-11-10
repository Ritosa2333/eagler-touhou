import {directoryRoomCanEnter, directoryRoomSpectatorEntry, directoryRoomState, type DirectoryRoom} from '../../services/lobby-directory';

export type DirectoryRoomFilter = 'all' | 'open' | 'waiting' | 'spectate';
export function filterDirectoryRooms(rooms: readonly DirectoryRoom[], filter: DirectoryRoomFilter) {
  return rooms.filter(room => filter === 'all' || (filter === 'open' ? directoryRoomState(room) === 'recruiting' && directoryRoomCanEnter(room)
    : filter === 'waiting' ? room.phase === 'lobby' : directoryRoomSpectatorEntry(room)));
}
