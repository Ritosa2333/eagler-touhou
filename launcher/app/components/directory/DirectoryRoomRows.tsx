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
import { RoomElapsed } from '../room/RoomElapsed';
import {bookGameArtwork} from '../launcher/book-artwork';

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
  const artwork = bookGameArtwork(gameIdForProduct(room.product)) ?? ('cardArtwork' in meta && meta.cardArtwork ? assetUrl(`assets/${meta.cardArtwork}`) : undefined);
  const title = locale === 'en' ? meta.subtitle : meta.title;
  const policy = multiplayerConfigForProduct(room.product)!;
  const duel = room.product === 'th09mp';
  const minutes = Math.max(...room.seats.map(seat => seat?.stats?.minutes ?? 0), 0);
  const difficulty = policy.inGameSelection ? '' : policy.difficulties[room.difficulty];
  const state = directoryRoomState(room);
  const spectating = directoryRoomSpectatorEntry(room);
  const count = t('lobby.count', { players: room.players, capacity: room.capacity });
  const disabled =
    snapshot.connection !== 'live' ||
    !!snapshot.mine ||
    snapshot.leaving ||
    !directoryRoomCanEnter(room);
  return (
    <article
      className={`lobby-room-row${initiallyHiddenMotion ? ' lobby-room-initial' : ''}`}
      role="listitem"
      data-key={`${room.product}-${room.code}`}
    >
      <div className="lobby-room-identity">
        <span className="lobby-cover" aria-hidden="true">
          <img
            src={artwork}
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
          <span className="lobby-mobile-difficulty" data-difficulty={difficulty.toLowerCase()} hidden={!difficulty}>{difficulty}</span>
          <span className="lobby-movement-rule" hidden={!room.disableCheatMovement}>
            {t('lobby.noCheat')}
          </span>
          <span className="lobby-movement-rule lobby-challengeMode" hidden={!room.challengeMode}>
            {t('room.challengeMode')}
          </span>
          <div className="lobby-room-facts">
            <span className="lobby-room-watchers" aria-label={t('th09.spectatorCount', {count: room.spectators})}
              title={t('th09.spectatorCount', {count: room.spectators})}>
              <img src={assetUrl('assets/room-users.svg')} alt=""/>{t('th09.spectatorCount', {count: room.spectators})}
            </span>
            {room.phase !== 'lobby' && !!room.startedAt && <>
              <span className="lobby-room-fact-divider" aria-hidden="true">|</span>
              <RoomElapsed phase={room.phase} startedAt={room.startedAt}/>
            </>}
            {duel && room.phase !== 'lobby' && !room.startedAt && <span className="room-elapsed">{t('th09.elapsed', {minutes})}</span>}
          </div>
        </div>
      </div>
      <span className="lobby-difficulty" data-difficulty={difficulty.toLowerCase()} hidden={!difficulty && !duel}>{difficulty || (locale === 'en' ? 'In game' : '局内选择')}</span>
      <div className="lobby-room-rules" aria-label={locale === 'en' ? 'Rules' : '玩法'}>
        {room.disableCheatMovement && <span>{t('lobby.noCheat')}</span>}
        {room.challengeMode && <span>{t('room.challengeMode')}</span>}
        {!room.disableCheatMovement && !room.challengeMode && <span className="lobby-rule-default">{locale === 'en' ? 'Standard' : '标准'}</span>}
      </div>
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
              ) + (control ? ` | ${control}` : '');
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
            const name = seat?.name ?? (duel ? seat?.machineSign : undefined);
            return <Fragment key={index}>
              {duel && index === 1 && <strong className="book-room-score" aria-label={t('th09.score')}>{room.seats[0] && room.seats[1] ? `${room.seats[0].stats?.wins ?? 0} : ${room.seats[1].stats?.wins ?? 0}` : 'VS'}</strong>}
              {name !== undefined ? <span className="lobby-public-player">
                {avatar}{controlCaption}<strong className="lobby-seat-name" title={name}>{/^P[12]$/.test(name) ? '' : name}</strong>
              </span> : avatar}
            </Fragment>;
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
