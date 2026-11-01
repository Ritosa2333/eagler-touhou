import { multiplayerDisplayInitial } from '../../../src/launcher/multiplayer-identity.mts';
import { useLocale } from '../../i18n';
import type { MultiplayerRoomService, RoomOwnerSnapshot } from '../../services/multiplayer-room';
import { Button } from '../ui/button';
import { RoomSpectatorJoin } from './RoomSpectators';

/** The room chrome previews avatars; the shared spectator panel owns the list. */
export function RoomSpectatorPreview({
  state,
  service,
  assetUrl,
  open,
  onOpen,
}: {
  state: RoomOwnerSnapshot;
  service: MultiplayerRoomService;
  assetUrl(path: string): string;
  open: boolean;
  onOpen(trigger: HTMLElement): void;
}) {
  const { t } = useLocale(),
    entries = state.room?.spectators || [];
  const count = Math.max(entries.length, Number(state.room?.spectatorCount) || 0);
  const label = (index: number) =>
    state.nickname && entries[index]?.name ? entries[index].name : t('room.spectatorNumber', { number: index + 1 });
  return (
    <section
      className="room-workspace-spectators"
      data-member-count={entries.length}
      aria-label={t('room.spectatorLounge')}
    >
      <Button
        variant="legacy"
        size="legacy"
        className="room-spectator-preview-trigger"
        type="button"
        aria-haspopup="dialog"
        aria-controls="mpRoomPanel"
        aria-expanded={open}
        onClick={(event) => onOpen(event.currentTarget)}
      >
        <img className="room-icon" src={assetUrl('assets/room-users.svg')} alt="" />
        <span>{t('room.spectatorLounge')}</span>
        <span id="mpSpectatorCount">{count}</span>
      </Button>
      <div className="room-spectator-preview-avatars">
        {entries.map((entry, index) => (
          <Button
            variant="legacy"
            size="legacy"
            key={entry.clientId}
            type="button"
            className={`room-spectator-preview-avatar${entry.clientId === state.localClientId ? ' mine' : ''}`}
            title={label(index)}
            aria-label={label(index)}
            aria-haspopup="dialog"
            aria-controls="mpRoomPanel"
            onClick={(event) => onOpen(event.currentTarget)}
          >
            {multiplayerDisplayInitial(entry.name, '?')}
          </Button>
        ))}
      </div>
      <RoomSpectatorJoin
        state={state}
        service={service}
        id="mpSpectatorPreviewJoin"
        className="room-spectator-preview-join"
      />
    </section>
  );
}
