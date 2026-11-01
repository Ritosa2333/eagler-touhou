import { useRef } from 'react';
import { multiplayerConfigForProduct } from '../../../src/contracts/product-catalog.mts';
import type { UiMessageKey } from '../../../src/launcher/i18n.mts';
import { multiplayerDisplayInitial } from '../../../src/launcher/multiplayer-identity.mts';
import { useLocale } from '../../i18n';
import type { DirectoryRoom } from '../../services/lobby-directory';
import { DifficultyLabel } from '../room/DifficultyLabel';
import { RoomElapsed } from '../room/RoomElapsed';
import { Button } from '../ui/button';

/** Shared large room card; TH09 retains its native duel score and names. */
export function Th09RoomCard({
  room,
  difficulty,
  state,
  spectating,
  disabled,
  initial,
  assetUrl,
  conflict,
  onJoin,
}: {
  room: DirectoryRoom;
  difficulty: string;
  state: 'recruiting' | 'full' | 'playing';
  spectating: boolean;
  disabled: boolean;
  initial: boolean;
  assetUrl(path: string): string;
  conflict: boolean;
  onJoin(): void;
}) {
  const { t } = useLocale(),
    policy = multiplayerConfigForProduct(room.product)!;
  const initialMotion = useRef(initial).current;
  const duel = room.product === 'th09mp',
    complete = !!room.seats[0] && !!room.seats[1];
  const minutes = Math.max(...room.seats.map((seat) => seat?.stats?.minutes ?? 0), 0);
  return (
    <article
      className={
        'lobby-room-row th09-room-row' + (!duel ? ' coop-room-row' : '') + (initialMotion ? ' lobby-room-initial' : '')
      }
      role="listitem"
      data-key={room.product + '-' + room.code}
    >
      <div className="th09-room-meta">
        <strong className="lobby-room-code">#{room.code}</strong>
        <span className="lobby-difficulty" hidden={!difficulty}>
          {difficulty && <DifficultyLabel name={difficulty} assetUrl={assetUrl} />}
        </span>
        {room.challengeMode && (
          <span className="lobby-challengeMode room-challenge-on">{t('room.challengeEnabled')}</span>
        )}
        {room.disableCheatMovement && <span className="lobby-movement-rule">{t('lobby.noCheat')}</span>}
      </div>
      <div
        className="th09-room-duel"
        data-player-count={room.capacity}
        aria-label={t('lobby.count', { players: room.players, capacity: room.capacity })}
      >
        {Array.from({ length: room.capacity }, (_, index) => {
          const seat = room.seats[index],
            name = seat
              ? (seat.name !== undefined ? seat.name : seat.machineSign || seat.initial) || 'P' + (index + 1)
              : t('th09.waitingPlayer');
          const loadout = seat?.loadout === undefined ? undefined : policy.loadouts[seat.loadout];
          const character = loadout && !policy.inGameSelection ? t(loadout.labelKey as UiMessageKey) : '';
          return (
            <div
              key={index}
              className={
                'th09-duel-player th09-duel-p' + (index + 1) + (seat?.name !== undefined ? ' public-name' : '')
              }
              data-empty={String(!seat)}
              data-offline={String(seat?.online === false)}
              title={name}
            >
              <span
                className="lobby-seat th09-duel-avatar"
                data-empty={String(!seat)}
                data-ready={String(!!seat?.ready)}
                data-offline={String(seat?.online === false)}
                aria-hidden="true"
              >
                {seat ? (
                  multiplayerDisplayInitial(seat.initial, '?')
                ) : (
                  <img className="th09-duel-empty-icon" src={assetUrl('assets/room-user.svg')} alt="" />
                )}
              </span>
              <div className="room-card-player-copy">
                <strong className="th09-duel-name">{name}</strong>
                {character && <span className="room-card-loadout">{character}</span>}
              </div>
            </div>
          );
        })}
        {duel && (
          <strong className="th09-room-score" aria-label={t('th09.score')}>
            {complete ? (room.seats[0]?.stats?.wins ?? 0) + ' : ' + (room.seats[1]?.stats?.wins ?? 0) : 'VS'}
          </strong>
        )}
      </div>
      <span className="th09-room-duration" data-state={state}>
        {state !== 'playing' ? (
          t(state === 'full' ? 'lobby.roomFull' : 'lobby.recruiting')
        ) : duel ? (
          t('th09.elapsed', { minutes })
        ) : (
          <RoomElapsed phase={room.phase} startedAt={room.startedAt} />
        )}
      </span>
      <Button
        variant="legacy"
        size="legacy"
        className="lobby-button lobby-join"
        type="button"
        disabled={disabled}
        aria-label={t(!duel && spectating ? 'multiplayer.joinSpectator' : 'lobby.joinRoom') + ' #' + room.code}
        title={conflict ? t('lobby.conflict') : ''}
        onClick={onJoin}
      >
        {t(!duel && spectating ? 'multiplayer.joinSpectator' : 'lobby.joinRoom')}
      </Button>
      <span className="th09-room-spectators">
        <img src={assetUrl('assets/room-users.svg')} alt="" />
        {t('th09.spectatorCount', { count: room.spectators })}
      </span>
    </article>
  );
}
