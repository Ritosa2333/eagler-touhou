import {useLocale} from '../../i18n';
import type {RoomOwnerSnapshot, MultiplayerRoomService} from '../../services/multiplayer-room';
import type {TransferSnapshot} from '../../models/transfer';
import type {MultiplayerResourceProgress} from '../../../src/launcher/multiplayer-lobby-snapshot.mts';
import {roomMiB} from './presentation';

export function ResourceTracks({progress, transfer, identifiers = false}: {progress: MultiplayerResourceProgress | null; transfer?: TransferSnapshot; identifiers?: boolean}) {
  const {t} = useLocale();
  const ready = progress?.status === 'ready';
  const base = ready || progress?.stage === 'runtime' ? 100 : progress?.basePercent ?? progress?.percent ?? null;
  const runtime = ready ? 100 : progress?.runtimePercent ?? (progress?.stage === 'runtime' ? progress.percent : 0);
  return <div className="mp-resource-tracks">{(['package', 'runtime'] as const).map(stage => {
    const raw = stage === 'package' ? base : runtime;
    const value = typeof raw === 'number' && Number.isFinite(raw) ? Math.max(0, Math.min(100, raw)) : null;
    const active = progress?.status === 'preparing' && progress.stage === stage;
    const native = stage === 'runtime' && active && transfer && !transfer.hidden ? transfer : undefined;
    const indeterminate = active && (native ? native.indeterminate : value === null);
    const label = t(stage === 'package' ? 'room.baseResources' : 'room.resourceLoading');
    return <div key={stage} className="mp-resource-row" data-resource-stage={stage}>
      <span>{label}</span><div className={'mp-room-resource-track' + (indeterminate ? ' indeterminate' : '')}
        id={identifiers ? stage === 'package' ? 'mpRoomResourceTrack' : 'mpRoomLoadingTrack' : undefined} role="progressbar"
        aria-label={label} aria-valuemin={0} aria-valuemax={100} aria-valuenow={indeterminate ? undefined : value ?? 0}>
        <span id={identifiers ? stage === 'package' ? 'mpRoomResourceFill' : 'mpRoomLoadingFill' : undefined} style={{width: native?.barWidth ?? ((value ?? 0) + '%')}}/>
      </div>
      <span className="mp-resource-percent">{indeterminate ? '—' : `${Math.round(value ?? 0)}%`}</span>
    </div>;
  })}</div>;
}

export function RoomProgress({state, service, transfer}: {state: RoomOwnerSnapshot; service: MultiplayerRoomService; transfer?: TransferSnapshot}) {
  const {t} = useLocale(), progress = state.preparation, status = progress?.status ?? 'preparing';
  const spectatorLoading = state.spectatorRequested && state.launchBusy && status === 'ready';
  const label = t(spectatorLoading ? 'room.loadingSpectator' : status === 'ready' ? 'room.resourcesReady' :
    status === 'failed' ? 'room.resourcesUnavailable' : status === 'cancelled' ? 'room.resourcesCancelled' :
    status === 'importing' ? 'package.importingSimple' : progress?.stage === 'runtime' ? 'room.preparingRuntime' : 'room.preparingResources');
  const displayed = spectatorLoading ? {...progress!, status: 'preparing' as const, stage: 'runtime' as const, runtimePercent: null, percent: null} : progress;
  return <div className="mp-room-resource-progress" id="mpRoomResourceProgress" hidden={!state.room}
    data-status={spectatorLoading ? 'preparing' : status} title={progress?.error || ''}>
    <span id="mpRoomResourceLabel" role="status">{label}</span>
    <span id="mpRoomResourceAmount">{progress?.total ? roomMiB(progress.loaded) + ' / ' + roomMiB(progress.total) : ''}</span>
    <button id="mpRoomResourceCancel" type="button" hidden={status !== 'preparing' || progress?.stage !== 'package'} onClick={service.cancelPreparation}>{t('room.cancelDownload')}</button>
    <button id="mpRoomResourceRetry" type="button" hidden={status !== 'failed' && status !== 'cancelled'} onClick={service.retryPreparation}>{t('room.retryDownload')}</button>
    <ResourceTracks progress={displayed} transfer={transfer} identifiers/>
  </div>;
}
