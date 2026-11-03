import {BookButton} from '../launcher/BookButton';
import { Fragment, useRef, useState } from 'react';
import {UsersRound} from 'lucide-react';
import {OverflowText} from '../../ui/overflow-text';
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
import './directory-room-list.css';

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
  const { t } = useLocale(),
    initiallyHiddenMotion = useRef(initial).current;
  const [coverFailed, setCoverFailed] = useState(false);
  const meta = PRODUCT_GAMES[gameIdForProduct(room.product)];
  const artwork = bookGameArtwork(gameIdForProduct(room.product)) ?? ('cardArtwork' in meta && meta.cardArtwork ? assetUrl(`assets/${meta.cardArtwork}`) : undefined);
  const title = meta.title;
  const policy = multiplayerConfigForProduct(room.product)!;
  const duel = room.product === 'th09mp';
  const score = room.seats[0] && room.seats[1] ? `${room.seats[0].stats?.wins ?? 0} : ${room.seats[1].stats?.wins ?? 0}` : 'VS';
  const minutes = Math.max(...room.seats.map(seat => seat?.stats?.minutes ?? 0), 0);
  const difficulty = policy.inGameSelection ? '' : policy.difficulties[room.difficulty];
  const state = directoryRoomState(room);
  const spectating = directoryRoomSpectatorEntry(room);
  const count = t('lobby.count', { players: room.players, capacity: room.capacity });
  const disabled =
    snapshot.loading ||
    snapshot.connection !== 'live' ||
    !!snapshot.mine ||
    snapshot.leaving ||
    !directoryRoomCanEnter(room);
  return (
    <li
      className={`lobby-room-row directory-room-grid${initiallyHiddenMotion ? ' lobby-room-initial' : ''}`}
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
          {(!artwork || coverFailed) && <span className="lobby-cover-number">{meta.number}</span>}
        </span>
        <div className="lobby-room-title">
          <h2><span className="lobby-work-name" title={title}>{title}</span>{difficulty && <span className="lobby-difficulty" data-difficulty={difficulty.toLowerCase()}>{difficulty}</span>}</h2>
          <div className="lobby-room-meta"><span className="lobby-room-code">#{room.code}</span></div>
          <div className="lobby-room-facts">
            <span className="lobby-room-watchers" role="img" aria-label={t('th09.spectatorCount', {count: room.spectators})}
              title={t('th09.spectatorCount', {count: room.spectators})}>
              <UsersRound aria-hidden="true"/>{t('th09.spectatorCount', {count: room.spectators})}
            </span>
            {room.phase !== 'lobby' && (duel || !!room.startedAt) && <>
              <span className="lobby-room-fact-divider" aria-hidden="true">|</span>
              <RoomElapsed phase={room.phase} startedAt={duel?undefined:room.startedAt} elapsedMinutes={duel?minutes:undefined}/>
            </>}
          </div>
        </div>
      </div>
      <div className="lobby-room-rules" hidden={!room.disableCheatMovement && !room.challengeMode}>
        {[room.disableCheatMovement ? t('lobby.noCheat') : '', room.challengeMode ? t('room.challengeMode') : ''].filter(Boolean).map(rule => <span key={rule}>{rule}</span>)}
      </div>
      <div className="lobby-room-status">
        <span className="lobby-room-state" data-state={state}>{t(`lobby.${state}`)}</span>
      </div>
      <fieldset className="lobby-party" aria-label={count}>
        <div
          className={`lobby-seats${room.seats.some((seat) => !!seat?.controlMode) ? ' has-controls' : ''}`}
          data-duel={duel ? 'true' : undefined}
          data-capacity={room.capacity}
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
              </span>
            );
            // An approved public name may literally be P1/P2. Legacy duel
            // labels only apply when the relay did not publish a name field.
            const name = !seat ? t('th09.waitingPlayer')
              : (seat.name !== undefined ? seat.name : duel ? seat.machineSign || seat.initial : '') || `P${index + 1}`;
            // biome-ignore lint/suspicious/noArrayIndexKey: A numbered seat is the slot identity even when its occupant changes.
            return <Fragment key={index}>
              {duel && index === 1 && <strong className="book-room-score" role="img" aria-label={`${t('th09.score')} ${score}`}>
                {score!=='VS'&&<span className="book-room-score-caption" aria-hidden="true">{t('th09.score')}</span>}
                <span className="book-room-score-value">{score === 'VS' ? score : <><span>{room.seats[0]?.stats?.wins ?? 0}</span><span className="book-room-score-separator"> : </span><span>{room.seats[1]?.stats?.wins ?? 0}</span></>}</span>
              </strong>}
              <span className="lobby-public-player" data-empty={String(!seat)}>
                {avatar}<OverflowText className="lobby-seat-name" align="start">{name}</OverflowText>{controlCaption}
              </span>
            </Fragment>;
          })}
        </div>
      </fieldset>
      <BookButton variant="primary"
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
      </BookButton>
    </li>
  );
}
