import {directoryRoomState, directoryRoomCanEnter, directoryRoomSpectatorEntry, type DirectorySnapshot, type LobbyDirectoryService} from '../../services/lobby-directory';
import {multiplayerConfigForProduct} from '../../../src/contracts/product-catalog.mts';
import {Th09RoomCard} from './Th09RoomCard';
import '../room/room-presentation.css';

export interface DirectoryRoomRowsProps {
  snapshot: DirectorySnapshot; service: LobbyDirectoryService; assetUrl(path: string): string; initialRows?: boolean;
}
export function DirectoryRoomRows({snapshot, service, assetUrl, initialRows = false}: DirectoryRoomRowsProps) {
  return <>{snapshot.visibleRooms.map(room => {
    const policy = multiplayerConfigForProduct(room.product)!;
    return <Th09RoomCard key={room.product + '-' + room.code} room={room}
      difficulty={policy.inGameSelection ? '' : policy.difficulties[room.difficulty] || 'Normal'} state={directoryRoomState(room)}
      spectating={directoryRoomSpectatorEntry(room)} initial={initialRows} assetUrl={assetUrl}
      disabled={snapshot.connection !== 'live' || !!snapshot.mine || snapshot.leaving || !directoryRoomCanEnter(room)}
      conflict={!!snapshot.mine} onJoin={() => service.joinRoom(room.product, room.code)}/>;
  })}</>;
}
