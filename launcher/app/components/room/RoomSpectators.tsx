import { multiplayerDisplayInitial } from '../../../src/launcher/multiplayer-identity.mts';
import { useLocale } from '../../i18n';
import type { MultiplayerRoomService, RoomOwnerSnapshot } from '../../services/multiplayer-room';
import { Button } from '../ui/button';
import { roomPresentation } from './presentation';
export function RoomSpectators({
  state,
  service,
  hidden,
}: {
  state: RoomOwnerSnapshot;
  service: MultiplayerRoomService;
  hidden: boolean;
}) {
  const { t } = useLocale(),
    { room, ready, owner } = roomPresentation(state, t)!,
    entries = room.spectators || [];
  return (
    <div id="mpSpectatorContent" className="mp-spectator-content" data-room-panel="spectators" hidden={hidden}>
      <p className="mp-spectator-intro">
        {t(state.product === 'th09mp' && room.spectatorTimeline ? 'room.th09SpectatorHint' : 'room.spectatorHint')}
      </p>
      <div className="mp-spectator-list" id="mpSpectatorList">
        {entries.length ? (
          entries.map((entry, index) => {
            const mine = entry.clientId === state.localClientId,
              title = state.nickname && entry.name ? entry.name : t('room.spectatorNumber', { number: index + 1 });
            return (
              <div className={`mp-spectator-entry${mine ? ' mine' : ''}`} key={entry.clientId} title={title}>
                <span className="mp-spectator-avatar">{multiplayerDisplayInitial(entry.name, '?')}</span>
                <div className="mp-spectator-copy">
                  <strong>{title}</strong>
                  <small>
                    {t(state.product === 'th09mp' && room.phase !== 'lobby' ? 'room.spectatorJoined' : 'room.watching')}
                  </small>
                </div>
                <span className="mp-spectator-marker">{mine ? t('multiplayer.you') : ''}</span>
                {owner && ready && room.phase === 'lobby' && !mine && (
                  <Button
                    variant="legacy"
                    size="legacy"
                    type="button"
                    className="mp-spectator-remove"
                    aria-label={t('room.removeSpectator')}
                    title={t('room.removeSpectator')}
                    onClick={() => {
                      void service.removeSpectator(entry.clientId);
                    }}
                  >
                    ×
                  </Button>
                )}
              </div>
            );
          })
        ) : (
          <div className="mp-spectator-empty">{t('status.noSpectators')}</div>
        )}
      </div>
      <RoomSpectatorJoin state={state} service={service} />
    </div>
  );
}

export function RoomSpectatorJoin({
  state,
  service,
  id = 'mpSpectatorJoin',
  className = 'mp-spectator-join',
}: {
  state: RoomOwnerSnapshot;
  service: MultiplayerRoomService;
  id?: string;
  className?: string;
}) {
  const { t } = useLocale(),
    { room, ready, config } = roomPresentation(state, t)!;
  return (
    <Button
      variant="legacy"
      size="legacy"
      className={className}
      id={id}
      type="button"
      hidden={
        (state.seat != null && room.phase !== 'lobby') ||
        (config as typeof config & { spectator?: boolean }).spectator === false
      }
      disabled={!ready || (state.seat != null && room.phase !== 'lobby')}
      onClick={state.spectatorRequested ? service.leaveSpectator : service.spectate}
    >
      {t(state.spectatorRequested ? 'multiplayer.leaveSpectator' : 'multiplayer.joinSpectator')}
    </Button>
  );
}
