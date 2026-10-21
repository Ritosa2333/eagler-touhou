import { Fragment, useRef, useState } from 'react';
import {
  PRODUCT_GAMES,
  gameIdForProduct,
  multiplayerConfigForProduct,
} from '../../../src/contracts/product-catalog.mts';
import {
  directoryRoomState,
  directoryRoomCanEnter,
  directoryRoomSpectatorEntry,
  type DirectoryRoom,
  type DirectorySnapshot,
  type LobbyDirectoryService,
} from '../../services/lobby-directory';
import { useLocale } from '../../i18n';
import { Th09RoomCard } from './Th09RoomCard';

export interface DirectoryRoomRowsProps {
  snapshot: DirectorySnapshot;
  service: LobbyDirectoryService;
  assetUrl(path: string): string;
  /** Rows created during main's first visual gate permanently skip arrival motion. */
  initialRows?: boolean;
}
/** Original public/lobby.html165–177 row template + lobby.mts260–347 renderer. */
export function DirectoryRoomRows({
  snapshot,
  service,
  assetUrl,
  initialRows = false,
}: DirectoryRoomRowsProps) {
  return (
    <>
      {snapshot.visibleRooms.map((room) => (
        <RoomRow
          key={`${room.product}-${room.code}`}
          room={room}
          snapshot={snapshot}
          service={service}
          assetUrl={assetUrl}
          initial={initialRows}
        />
      ))}
    </>
  );
}
function RoomRow({
  room,
  snapshot,
  service,
  assetUrl,
  initial,
}: {
  room: DirectoryRoom;
  snapshot: DirectorySnapshot;
  service: LobbyDirectoryService;
  assetUrl(path: string): string;
  initial: boolean;
}) {
  const { locale, t } = useLocale(),
    initiallyHiddenMotion = useRef(initial).current;
  const [coverFailed, setCoverFailed] = useState(false);
  const meta = PRODUCT_GAMES[gameIdForProduct(room.product)];
  const artwork = 'cardArtwork' in meta ? meta.cardArtwork : null;
  const title = locale === 'en' ? meta.subtitle : meta.title;
  const policy = multiplayerConfigForProduct(room.product)!;
  const difficulty = policy.inGameSelection ? '' : policy.difficulties[room.difficulty];
  const state = directoryRoomState(room);
  const spectating = directoryRoomSpectatorEntry(room);
  const count = t('lobby.count', { players: room.players, capacity: room.capacity });
  const disabled =
    snapshot.connection !== 'live' ||
    !!snapshot.mine ||
    snapshot.leaving ||
    !directoryRoomCanEnter(room);
  if (room.product === 'th09mp')
    return (
      <Th09RoomCard
        room={room}
        difficulty={difficulty}
        state={state}
        spectating={spectating}
        disabled={disabled}
        initial={initiallyHiddenMotion}
        assetUrl={assetUrl}
        conflict={!!snapshot.mine}
        onJoin={() => service.joinRoom(room.product, room.code)}
      />
    );
  return (
    <article
      className={`lobby-room-row${initiallyHiddenMotion ? ' lobby-room-initial' : ''}`}
      role="listitem"
      data-key={`${room.product}-${room.code}`}
    >
      <div className="lobby-room-identity">
        <span className="lobby-cover" aria-hidden="true">
          <img
            src={artwork ? assetUrl(`assets/${artwork}`) : undefined}
            alt=""
            loading="lazy"
            decoding="async"
            hidden={!artwork || coverFailed}
            onError={() => setCoverFailed(true)}
          />
          <span className="lobby-cover-number">{meta.number}</span>
        </span>
        <div className="lobby-room-title">
          <span className="lobby-room-code">#{room.code}</span>
          <h2>{title}</h2>
          <span className="lobby-room-subtitle">{meta.subtitle}</span>
          <span className="lobby-mobile-difficulty" hidden={!difficulty}>{difficulty}</span>
          <span className="lobby-movement-rule" hidden={!room.disableCheatMovement}>
            {t('lobby.noCheat')}
          </span>
          <span className="lobby-movement-rule lobby-challengeMode" hidden={!room.challengeMode}>
            {t('room.challengeMode')}
          </span>
        </div>
      </div>
      <span className="lobby-difficulty" hidden={!difficulty}>{difficulty}</span>
      <div className="lobby-party" aria-label={count}>
        <span className="lobby-occupancy">{count}</span>
        <div
          className={`lobby-seats${room.seats.some((seat) => !!seat?.controlMode) ? ' has-controls' : ''}`}
        >
          {room.seats.map((seat, index) => {
            const control = seat?.controlMode ? t(seat.name !== undefined && seat.controlMode === 'normal' ? 'multiplayer.control.keysOrStick' : `multiplayer.control.${seat.controlMode}`) : '';
            const label =
              t(
                !seat
                  ? 'lobby.seatEmpty'
                  : !seat.online
                    ? 'lobby.seatOffline'
                    : seat.ready
                      ? 'lobby.seatReady'
                      : 'lobby.seatWaiting',
                { seat: index + 1, name: seat?.name || seat?.initial || '?' },
              ) + (control ? ` · ${control}` : '');
            const controlCaption = seat?.controlMode && (
              <span className="lobby-seat-control" data-mode={seat.controlMode} title={control}>
                {control}
              </span>
            );
            const avatar = (
              <span
                className="lobby-seat"
                data-empty={String(!seat)}
                data-ready={String(!!seat?.ready)}
                data-offline={String(!!seat && !seat.online)}
                title={label}
                aria-label={label}
                role="img"
              >
                {seat?.initial || ''}
                {seat?.name === undefined && controlCaption}
              </span>
            );
            return seat?.name !== undefined ? <span key={index} className="lobby-public-player">
              {avatar}{controlCaption}<strong className="lobby-seat-name" title={seat.name || `P${index+1}`}>{seat.name || `P${index+1}`}</strong>
            </span> : <Fragment key={index}>{avatar}</Fragment>;
          })}
        </div>
      </div>
      <span className="lobby-player-count" aria-label={count}>
        {room.players} / {room.capacity}
      </span>
      <span className="lobby-room-state" data-state={state}>
        {t(`lobby.${state}`)}
      </span>
      <button
        className="lobby-button lobby-join"
        type="button"
        disabled={disabled}
        aria-label={`${t(spectating ? 'multiplayer.joinSpectator' : 'lobby.joinRoom')} ${title} #${room.code}`}
        title={snapshot.mine ? t('lobby.conflict') : ''}
        onClick={() => service.joinRoom(room.product, room.code)}
      >
        {t(
          spectating
            ? 'multiplayer.joinSpectator'
            : state === 'recruiting'
              ? 'lobby.join'
              : `lobby.${state}`,
        )}
      </button>
    </article>
  );
}
