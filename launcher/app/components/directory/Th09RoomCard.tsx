import type { DirectoryRoom } from '../../services/lobby-directory';
import { useLocale } from '../../i18n';
import { multiplayerDisplayInitial } from '../../../src/launcher/multiplayer-identity.mts';

/** TH09's duel presentation uses the existing directory's admission authority. */
export function Th09RoomCard({ room, difficulty, state, disabled, initial,
  assetUrl, conflict, onJoin }: {
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
  const { t } = useLocale();
  const complete = !!room.seats[0] && !!room.seats[1];
  const minutes = Math.max(...room.seats.map(seat => seat?.stats?.minutes ?? 0));
  return (
    <article className={`lobby-room-row th09-room-row${initial ? ' lobby-room-initial' : ''}`}
      role="listitem" data-key={`${room.product}-${room.code}`}>
      <div className="th09-room-meta">
        <strong className="lobby-room-code">#{room.code}</strong>
        <span className="lobby-difficulty" hidden={!difficulty}>{difficulty}</span>
        {room.disableCheatMovement && <span className="lobby-movement-rule">{t('lobby.noCheat')}</span>}
        {room.challengeMode && <span className="lobby-movement-rule">{t('room.challengeMode')}</span>}
      </div>
      <span className="lobby-room-state" data-state={state}>
        {t(state === 'playing' ? 'th09.playing' : !complete ? 'th09.waitingPlayer' : 'th09.waitingStart')}
      </span>
      <div className="th09-room-duel" aria-label={t('lobby.count', {players:room.players,capacity:room.capacity})}>
        {[0, 1].map(index => {
          const seat = room.seats[index];
          const name = seat ? (seat.name !== undefined ? seat.name : seat.machineSign) || `P${index + 1}` : t('th09.waitingPlayer');
          const hint = `${name}${seat?.online === false ? ` (${t('lobby.seatOffline', {seat:index+1,name})})` : ''}`;
          return (
            <div key={index} className={`th09-duel-player th09-duel-p${index+1}${seat?.name !== undefined ? ' public-name' : ''}`}
              data-empty={String(!seat)} data-offline={String(seat?.online === false)} title={hint}>
              <span className="lobby-seat th09-duel-avatar" data-empty={String(!seat)}
                data-ready={String(!!seat?.ready)} data-offline={String(seat?.online === false)} aria-hidden="true">
                {seat ? multiplayerDisplayInitial(seat.initial,'?')
                  : <img className="th09-duel-empty-icon" src={assetUrl('assets/room-user.svg')} alt="" />}
              </span>
              <strong className="th09-duel-name">{name}</strong>
            </div>
          );
        })}
        <strong className="th09-room-score" aria-label={t('th09.score')}>
          {complete ? `${room.seats[0]?.stats?.wins ?? 0} : ${room.seats[1]?.stats?.wins ?? 0}` : 'VS'}
        </strong>
      </div>
      <span className="th09-room-duration">
        {t(room.phase === 'playing' ? 'th09.elapsed' : 'th09.notStarted', {minutes})}
      </span>
      <button className="lobby-button lobby-join" type="button" disabled={disabled}
        aria-label={`${t('lobby.joinRoom')} #${room.code}`}
        title={conflict ? t('lobby.conflict') : ''} onClick={onJoin}>
        {t('lobby.joinRoom')}
      </button>
      <span className="th09-room-spectators">
        <img src={assetUrl('assets/room-users.svg')} alt="" />
        {t('th09.spectatorCount', {count:room.spectators})}
      </span>
    </article>
  );
}
