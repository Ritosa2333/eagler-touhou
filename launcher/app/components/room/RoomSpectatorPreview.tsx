import {Button as UiButton} from '../../ui/controls';
import {multiplayerDisplayInitial} from '../../../src/launcher/multiplayer-identity.mts';
import type {RoomOwnerSnapshot, MultiplayerRoomService} from '../../services/multiplayer-room';
import {useLocale} from '../../i18n';
import {RoomSpectatorJoin} from './RoomSpectators';
import {ParticipantAvatar} from './ParticipantAvatar';
import {accountsEnabled} from '../../release-features';

/** The room chrome previews avatars; the shared spectator panel owns the list. */
export function RoomSpectatorPreview({state, service, assetUrl, open, onOpen}: {
  state: RoomOwnerSnapshot;
  service: MultiplayerRoomService;
  assetUrl(path: string): string;
  open: boolean;
  onOpen(trigger: HTMLElement): void;
}) {
  const {t} = useLocale(), entries = state.room?.spectators || [];
  const count = Math.max(entries.length, Number(state.room?.spectatorCount) || 0);
  const label = (index: number) => state.nickname && entries[index]?.name
    ? entries[index].name : t('room.spectatorNumber', {number:index + 1});
  return <section className="room-workspace-spectators" data-member-count={entries.length} aria-label={t('room.spectatorLounge')}>
    <UiButton variant="unstyled" className="room-spectator-preview-trigger" type="button" aria-haspopup="dialog"
      aria-controls="mpRoomPanel" aria-expanded={open} onClick={event=>onOpen(event.currentTarget)}>
      <img className="room-icon" src={assetUrl('assets/room-users.svg')} alt=""/>
      <span>{t('room.spectatorLounge')}</span><span id="mpSpectatorCount">{count}</span>
    </UiButton>
    <div className="room-spectator-preview-avatars">
      {entries.map((entry, index) => {
        const className = 'room-spectator-preview-avatar' + (entry.clientId === state.localClientId ? ' mine' : '');
        const initial = multiplayerDisplayInitial(entry.name, '?');
        return accountsEnabled && entry.profile
          ? <ParticipantAvatar key={entry.clientId} className={className} profile={entry.profile} name={label(index)}>{initial}</ParticipantAvatar>
          : <UiButton variant="unstyled" key={entry.clientId} type="button" className={className}
              title={label(index)} aria-label={label(index)} aria-haspopup="dialog" aria-controls="mpRoomPanel"
              onClick={event => onOpen(event.currentTarget)}>{initial}</UiButton>;
      })}
    </div>
    <RoomSpectatorJoin state={state} service={service} id="mpSpectatorPreviewJoin" className="room-spectator-preview-join"/>
  </section>;
}
