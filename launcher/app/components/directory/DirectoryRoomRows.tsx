import {BookButton} from '../launcher/BookButton';
import { Fragment, useEffect, useRef, useState } from 'react';
import {Music2, UsersRound} from 'lucide-react';
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
import {accountsEnabled} from '../../release-features';
import { RoomElapsed } from '../room/RoomElapsed';
import {ParticipantAvatar} from '../room/ParticipantAvatar';
import {bookGameArtwork} from '../launcher/book-artwork';
import {Th09RatingText} from '../room/Th09RatingText';
import {Th09RatingCardsContext, type Th09Rating, type Th09RatingCards} from '../../services/th09-ratings';
import './directory-room-list.css';

export interface DirectoryRoomRowsProps {
  snapshot: DirectorySnapshot;
  service: LobbyDirectoryService;
  assetUrl(path: string): string;
  /** Rows created during main's first visual gate permanently skip arrival motion. */
  initialRows?: boolean;
  arrivals?: ReadonlySet<string>;
  exiting?: ReadonlySet<string>;
  ratings?: Readonly<Record<string, Th09Rating>>;
  myRating?: Th09Rating;
  cards?: Th09RatingCards;
}
/** Original public/lobby.html165–177 row template + lobby.mts260–347 renderer. */
export function DirectoryRoomRows({
  snapshot,
  service,
  assetUrl,
  initialRows = false,
  arrivals,
  exiting,
  ratings,
  myRating,
  cards,
}: DirectoryRoomRowsProps) {
  return (
    <>
      {snapshot.visibleRooms.map((room) => (
        <Th09RatingCardsContext.Provider key={`${room.product}-${room.code}`} value={room.product === 'th09mp' ? cards : undefined}>
          <RoomRow
            room={room}
            snapshot={snapshot}
            service={service}
            ratings={ratings}
            myRating={myRating}
            assetUrl={assetUrl}
            initial={initialRows}
            arriving={arrivals?.has(`${room.product}-${room.code}`) ?? false}
            exiting={exiting?.has(`${room.product}-${room.code}`) ?? false}
          />
        </Th09RatingCardsContext.Provider>
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
  arriving,
  exiting,
  ratings,
  myRating,
}: {
  room: DirectoryRoom;
  snapshot: DirectorySnapshot;
  service: LobbyDirectoryService;
  assetUrl(path: string): string;
  initial: boolean;
  arriving: boolean;
  exiting: boolean;
  ratings?: Readonly<Record<string, Th09Rating>>;
  myRating?: Th09Rating;
}) {
  const { t, locale } = useLocale(),
    initiallyHiddenMotion = useRef(initial).current;
  const [coverFailed, setCoverFailed] = useState(false);
  const meta = PRODUCT_GAMES[gameIdForProduct(room.product)];
  const artwork = bookGameArtwork(gameIdForProduct(room.product)) ?? ('cardArtwork' in meta && meta.cardArtwork ? assetUrl(`assets/${meta.cardArtwork}`) : undefined);
  const title = meta.title;
  const policy = multiplayerConfigForProduct(room.product)!;
  const duel = room.product === 'th09mp' || room.product === 'th19mp';
  const firstWins = room.seats[0]?.stats?.wins ?? 0;
  const secondWins = room.seats[1]?.stats?.wins ?? 0;
  const hasDuelPair = duel && !!room.seats[0] && !!room.seats[1];
  const score = hasDuelPair ? `${firstWins} : ${secondWins}` : 'VS';
  const largestScore = Math.max(firstWins, secondWins);
  const compactScore = hasDuelPair && largestScore >= 1000;
  let compactNumbers: Intl.NumberFormat | undefined;
  const abbreviate = (value: number) => {
    compactNumbers ??= new Intl.NumberFormat(locale, { notation: 'compact', maximumFractionDigits: 0 });
    return compactNumbers.format(value);
  };
  const scoreNumber = (value: number) => {
    if (!compactScore) return value;
    const compact = abbreviate(value);
    return compact.length <= 4 ? compact : value.toExponential(0).replace('e+', 'e');
  };
  const duelRecords = room.product === 'th09mp' && accountsEnabled ? room.seats.slice(0, 2).map((seat, side) => {
    if (!seat) return <span key={side === 0 ? 'left' : 'right'} className="book-room-duel-summary" data-side={side} aria-hidden="true"><span>&nbsp;</span></span>;
    const profile = seat.profile;
    return <span key={side === 0 ? 'left' : 'right'} className="book-room-duel-summary" data-side={side}>
      <span><Th09RatingText player={profile && ratings?.[profile.username]} mine={myRating} games={profile?.th09?.games}/></span>
    </span>;
  }) : [];
  const minutes = Math.max(...room.seats.map(seat => seat?.stats?.minutes ?? 0), 0);
  const difficulty = policy.inGameSelection && room.product !== 'th03mp' ? '' : policy.difficulties[room.difficulty];
  const state = directoryRoomState(room);
  const waitingSince = policy.gameplay === 'versus' && state === 'recruiting' ? room.createdAt : undefined;
  const [now, setNow] = useState(Date.now);
  useEffect(() => {
    if (waitingSince === undefined) return;
    setNow(Date.now());
    const timer = window.setInterval(() => setNow(Date.now()), 1000);
    return () => window.clearInterval(timer);
  }, [waitingSince]);
  const waitingSeconds = waitingSince === undefined ? 0 : Math.max(0, Math.floor((now - waitingSince) / 1000));
  const longWait = waitingSeconds >= 15 * 60;
  const statusLabel = t(longWait ? 'lobby.longWait' : `lobby.${state}`);
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
      className={`lobby-room-row directory-room-grid${initiallyHiddenMotion ? ' lobby-room-initial' : ''}${arriving && !exiting ? ' lobby-room-arriving' : ''}${exiting ? ' lobby-room-exiting' : ''}`}
      inert={exiting || undefined}
      aria-hidden={exiting || undefined}
      data-key={`${room.product}-${room.code}`}
      data-state={state}
      data-long-wait={longWait || undefined}
      data-listening={room.listeningTrack ? '' : undefined}
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
          <div className="lobby-room-meta">
            <span className="lobby-room-code">#{room.code}</span>
            <span className="lobby-room-state" data-state={state} title={statusLabel}>
              <span>{statusLabel}</span>
            </span>
          </div>
          <div className="lobby-room-facts">
            <span className="lobby-room-watchers" role="img" aria-label={t('th09.spectatorCount', {count: room.spectators})}
              title={t('th09.spectatorCount', {count: room.spectators})}>
              <UsersRound aria-hidden="true"/>{t('th09.spectatorCount', {count: room.spectators})}
            </span>
            {room.phase !== 'lobby' && (duel || !!room.startedAt) && <span className="lobby-room-duration">
              <span className="lobby-room-fact-divider" aria-hidden="true">|</span>
              <RoomElapsed phase={room.phase} startedAt={duel ? undefined : room.startedAt} elapsedMinutes={duel ? minutes : undefined}/>
            </span>}
            {waitingSince !== undefined && <span className="lobby-room-duration"><span className="lobby-room-fact-divider" aria-hidden="true">|</span>
              <span>{t('lobby.waitTime', {time:`${Math.floor(waitingSeconds / 60)}:${String(waitingSeconds % 60).padStart(2,'0')}`})}</span>
            </span>}
            {room.listeningTrack && <span className="lobby-room-music">
              <Music2 aria-hidden="true"/>
              <OverflowText autoScroll>{[room.listeningTrack.name, room.listeningTrack.artist].filter(Boolean).join(' - ')}</OverflowText>
            </span>}
          </div>
        </div>
      </div>
      <div className="lobby-room-rules">
        {!room.disableCheatMovement && !room.challengeMode && !room.unrated ? <span className="lobby-room-rules-standard">{t('directory.regularRules')}</span> :
          [room.disableCheatMovement ? t('lobby.noCheat') : '', room.challengeMode ? t('room.challengeMode') : '', room.unrated ? t('room.unrated') : ''].filter(Boolean).map(rule => <span key={rule}>{rule}</span>)}
      </div>
      <fieldset className="lobby-party" aria-label={count}>
        <div
          className={`lobby-seats${room.seats.some((seat) => !!seat?.controlMode) ? ' has-controls' : ''}`}
          data-duel={duel ? 'true' : undefined}
          data-duel-records={duelRecords.length ? '' : undefined}
          data-capacity={room.capacity}
        >
          {room.seats.map((seat, index) => {
            const control = seat?.controlMode ? t(`multiplayer.control.${seat.controlMode}`) : '';
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
              <ParticipantAvatar
                className="lobby-seat"
                profile={seat?.profile}
                name={seat?.name || seat?.initial || ''}
                data-empty={String(!seat)}
                data-ready={String(!!seat?.ready)}
                data-offline={String(!!seat && !seat.online)}
                title={label}
                aria-label={label}
                role="img"
              >
                {seat?.initial || ''}
              </ParticipantAvatar>
            );
            // An approved public name may literally be P1/P2. Legacy duel
            // labels only apply when the relay did not publish a name field.
            const name = !seat ? t('th09.waitingPlayer')
              : (seat.name !== undefined ? seat.name : duel ? seat.machineSign || seat.initial : '') || `P${index + 1}`;
            // biome-ignore lint/suspicious/noArrayIndexKey: A numbered seat is the slot identity even when its occupant changes.
            return <Fragment key={index}>
              {duel && index === 1 && <>
                {duelRecords[0]}
                <strong className="book-room-score" role="img" aria-label={`${t('th09.score')} ${score}`} title={score}
                  data-wide={hasDuelPair && largestScore >= 10 ? '' : undefined} data-compact={compactScore ? '' : undefined}>
                  <span className="book-room-score-value">{hasDuelPair ? <>
                    <span className="book-room-score-number">{scoreNumber(firstWins)}</span>
                    <span className="book-room-score-separator"> : </span>
                    <span className="book-room-score-number">{scoreNumber(secondWins)}</span>
                  </> : score}</span>
                </strong>
                {duelRecords[1]}
              </>}
              <span className="lobby-public-player" data-empty={String(!seat)}>
                {avatar}
                <span className="lobby-seat-text">
                  {seat ? <OverflowText className="lobby-seat-name">{name}</OverflowText> : <span className="lobby-seat-name">{name}</span>}
                  {controlCaption}
                </span>
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
