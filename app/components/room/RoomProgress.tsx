import { useLocale } from '../../i18n';
import type { RoomOwnerSnapshot, MultiplayerRoomService } from '../../services/multiplayer-room';
import type { TransferSnapshot } from '../../models/transfer';
import { roomMiB } from './presentation';
/** Main app1274–1369; actual preparation measurements only. */
export function RoomProgress({
  state,
  service,
  transfer,
}: {
  state: RoomOwnerSnapshot;
  service: MultiplayerRoomService;
  transfer?: TransferSnapshot;
}) {
  const { t } = useLocale(),
    progress = state.preparation,
    status = progress?.status || 'preparing';
  const total = progress?.total || 0,
    loaded = progress?.loaded || 0;
  if (
    state.product === 'th09mp' &&
    state.spectatorRequested &&
    state.launchBusy &&
    status === 'ready'
  ) {
    const active = transfer && !transfer.hidden ? transfer : undefined;
    return (
      <div
        className="mp-room-resource-progress"
        id="mpRoomResourceProgress"
        data-status="preparing"
        aria-busy="true"
      >
        <strong id="mpRoomResourceLabel" role="status">
          {t('room.loadingSpectator')}
        </strong>
        <span id="mpRoomResourceAmount">
          {active
            ? [active.title, active.label, active.amount].filter(Boolean).join(' — ')
            : t('room.preparingRuntime')}
        </span>
        <div
          className={`mp-room-resource-track${!active || active.indeterminate ? ' indeterminate' : ''}`}
          id="mpRoomResourceTrack"
          role="progressbar"
          aria-label={t('room.loadingSpectator')}
        >
          <span id="mpRoomResourceFill" style={{ width: active?.barWidth }} />
        </div>
      </div>
    );
  }
  const label =
    status !== 'preparing'
      ? t(
          status === 'ready'
            ? 'room.resourcesReady'
            : status === 'cancelled'
              ? 'room.resourcesCancelled'
              : status === 'importing'
                ? 'package.importingSimple'
                : 'room.resourcesUnavailable',
        )
      : progress?.stage === 'runtime'
        ? t('room.preparingRuntime')
        : progress?.title || t('room.preparingResources');
  const amount =
    status === 'preparing'
      ? total > 0
        ? `${roomMiB(loaded)} / ${roomMiB(total)}`
        : loaded > 0
          ? roomMiB(loaded)
          : ''
      : '';
  const percent =
    status === 'ready'
      ? 100
      : status === 'preparing' && total > 0
        ? Math.min(100, (loaded / total) * 100)
        : undefined;
  const width =
    status !== 'preparing'
      ? status === 'ready'
        ? '100%'
        : '0%'
      : percent === undefined
        ? undefined
        : `${percent.toFixed(1)}%`;
  return (
    <div
      className="mp-room-resource-progress"
      id="mpRoomResourceProgress"
      hidden={!state.room}
      data-status={status}
      title={status === 'failed' ? progress?.error || '' : ''}
    >
      <span id="mpRoomResourceLabel">{label}</span>
      <span id="mpRoomResourceAmount">{amount}</span>
      <button
        id="mpRoomResourceCancel"
        type="button"
        hidden={status !== 'preparing' || progress?.stage !== 'package'}
        onClick={service.cancelPreparation}
      >
        {t('room.cancelDownload')}
      </button>
      <button
        id="mpRoomResourceRetry"
        type="button"
        hidden={status !== 'failed' && status !== 'cancelled'}
        onClick={service.retryPreparation}
      >
        {t('room.retryDownload')}
      </button>
      <div
        className={`mp-room-resource-track${status === 'importing' || (status === 'preparing' && total <= 0) ? ' indeterminate' : ''}`}
        id="mpRoomResourceTrack"
        role="progressbar"
        aria-label={t('room.preparingResources')}
        aria-valuenow={percent === undefined ? undefined : Math.round(percent)}
      >
        <span id="mpRoomResourceFill" style={{ width }} />
      </div>
    </div>
  );
}
