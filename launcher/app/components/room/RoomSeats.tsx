import { type CSSProperties, useEffect, useLayoutEffect, useRef } from 'react';
import { touchMovementUsesJoystick } from '../../../src/launcher/game-preferences.mts';
import { multiplayerDisplayInitial } from '../../../src/launcher/multiplayer-identity.mts';
import { useLocale } from '../../i18n';
import type { MultiplayerRoomService, RoomOwnerSnapshot } from '../../services/multiplayer-room';
import { Button } from '../ui/button';
import { networkMessage, roomPresentation } from './presentation';
import { RoomActionMenu } from './RoomActionMenu';
import { ResourceTracks } from './RoomProgress';
import type { RoomNetworkPresentation, RoomPanelKind, RoomViewContext } from './types';
export interface RoomSeatsProps {
  state: RoomOwnerSnapshot;
  service: MultiplayerRoomService;
  network: RoomNetworkPresentation;
  context: RoomViewContext;
  openPanel(kind: RoomPanelKind, trigger: HTMLElement, peerId?: string): void;
}
export function RoomSeats(props: RoomSeatsProps) {
  return (
    <div className="mp-seat-stage" id="mpSeatStage" data-player-count={props.state.room?.playerCount}>
      {[0, 1, 2].map((index) => (
        <RoomSeat {...props} index={index} key={index} />
      ))}
    </div>
  );
}
function RoomSeat({ state, service, network, context, openPanel, index }: RoomSeatsProps & { index: number }) {
  const { t } = useLocale(),
    presentation = roomPresentation(state, t)!;
  const { room, ready, owner, label } = presentation;
  const node = useRef<HTMLElement>(null),
    previous = useRef<{ room: string; occupant: string; animations: Animation[] } | null>(null);
  const active = index < room.playerCount,
    seat = room.synced ? room.seats?.[index] || null : null;
  const occupied = room.synced === true && (!!seat || state.seat === index),
    occupant = occupied ? seat?.clientId || state.localClientId : '';
  useLayoutEffect(() => {
    const element = node.current,
      before = previous.current;
    if (!element || (before && before.room === room.code && before.occupant === occupant)) return;
    before?.animations.forEach((animation) => {
      animation.cancel();
    });
    const next = { room: room.code, occupant, animations: [] as Animation[] };
    previous.current = next;
    if (
      !before ||
      before.room !== room.code ||
      !room.synced ||
      element.hidden ||
      document.body.classList.contains('less-motion') ||
      matchMedia('(prefers-reduced-motion: reduce)').matches
    )
      return;
    for (const child of element.querySelectorAll<HTMLElement>(
      ':scope > .mp-seat-face, :scope > .mp-seat-index, :scope > .mp-seat-name, :scope > .mp-seat-identity, :scope > .mp-seat-state',
    )) {
      if (!child.hidden && typeof child.animate === 'function') {
        const animation = child.animate([{ opacity: 0.25 }, { opacity: 1 }], {
          duration: 180,
          easing: 'cubic-bezier(.2,.8,.2,1)',
        });
        // A superseded occupant or React teardown cancels these visual leaves.
        // Cancellation is expected and must not become an unhandled rejection.
        void animation.finished.catch(() => {});
        next.animations.push(animation);
      }
    }
  }, [room.code, room.synced, occupant]);
  useEffect(
    () => () => {
      previous.current?.animations.forEach((animation) => {
        animation.cancel();
      });
    },
    [],
  );
  const seatLabel = label(seat ? seat.loadout : state.preferredLoadout);
  const control =
    state.seat === index
      ? !context.touchEnabled || touchMovementUsesJoystick(context.touchMovementMode)
        ? 'normal'
        : context.touchMovementMode === 'touch-unlimited'
          ? 'cheat'
          : 'touch'
      : seat?.controlMode;
  const controlLabel = control
    ? t(state.nickname && control === 'normal' ? 'multiplayer.control.keysOrStick' : `multiplayer.control.${control}`)
    : '';
  const controlCaption = (
    <span
      className="mp-seat-control"
      hidden={!occupied || !control}
      data-mode={control || ''}
      title={state.nickname ? controlLabel : undefined}
    >
      {controlLabel}
    </span>
  );
  const message = networkMessage(state, context, t),
    local = seat?.clientId === state.localClientId;
  const diagnostics = (['direct', 'turn'] as const).map((lane) => {
    const metric = seat ? network.metric(seat.clientId, lane) : null,
      measured =
        state.connected && room.synced && !seat?.offline && metric?.state === 'connected' && metric.rtt != null;
    return { lane, label: t(`room.${lane}`), value: measured ? `${Math.max(1, Math.round(metric!.rtt!))}ms` : '—' };
  });
  const values = diagnostics.map((metric) => `${metric.label} ${metric.value}`);
  const resource =
    state.seat === index && state.preparation
      ? {
          ...state.preparation,
          percent:
            state.preparation.status === 'ready'
              ? 100
              : state.preparation.status === 'preparing' && state.preparation.total
                ? Math.round(((state.preparation.loaded || 0) / state.preparation.total) * 100)
                : null,
        }
      : seat?.resource;
  const resourceLabel = !resource
    ? ''
    : resource.status === 'ready'
      ? t('room.resourcesReady')
      : resource.status === 'failed'
        ? t('room.resourcesUnavailable')
        : resource.status === 'cancelled'
          ? t('room.resourcesCancelled')
          : resource.status === 'importing'
            ? t('package.importingSimple')
            : resource.stage === 'runtime'
              ? t('room.preparingRuntime')
              : resource.percent == null
                ? t('room.preparingResources')
                : t('room.peerDownloading', { percent: resource.percent });
  const resourceReady = resource?.status === 'ready' || seat?.ready === true;
  const base = resourceReady || resource?.stage === 'runtime' ? 100 : (resource?.basePercent ?? resource?.percent);
  const loading = resourceReady
    ? 100
    : resource?.stage === 'runtime'
      ? (resource.runtimePercent ?? resource.percent)
      : 0;
  const degrees = (value: unknown) =>
    typeof value === 'number' && Number.isFinite(value) ? Math.max(0, Math.min(100, value)) * 3.6 : 0;
  const appearance = !occupied
    ? 'empty'
    : seat?.offline || !resourceReady
      ? 'loading'
      : seat?.ready
        ? 'ready'
        : 'unready';
  const name = (
    <strong className="mp-seat-name" data-mp-seat-name={index} title={seat?.name || `P${index + 1}`}>
      {seat?.name || (state.seat === index ? state.displayName : '') || `P${index + 1}`}
    </strong>
  );
  const connection = (
    <Button
      variant="legacy"
      size="legacy"
      className="mp-seat-latency room-seat-network"
      type="button"
      hidden={!seat}
      disabled={local || !!message || !!seat?.offline}
      aria-haspopup="dialog"
      aria-controls="mpRoomPanel"
      data-network-peer={seat && !local ? seat.clientId : undefined}
      aria-label={
        seat && !local
          ? `${t('multiplayer.you')} → P${index + 1} | ${values.join(' | ')} | ${t('room.connections')}`
          : undefined
      }
      onClick={(event) => {
        if (seat) openPanel('network', event.currentTarget, seat.clientId);
      }}
    >
      {local ? (
        <span className="room-seat-network-label">{t('room.localDevice')}</span>
      ) : (
        diagnostics.map((metric) => (
          <span className="room-seat-network-pair" key={metric.lane}>
            <span className="room-seat-network-label">{metric.label}</span>
            <span className="room-seat-network-value">{metric.value}</span>
          </span>
        ))
      )}
    </Button>
  );
  return (
    <article
      ref={node}
      className={`mp-seat${occupied ? ' occupied' : ''}${index === 0 && owner ? ' owner' : ''}${seat?.offline ? ' reconnecting' : ''}${occupied && seat?.ready ? ' is-ready' : ''}`}
      data-mp-seat={index}
      data-card-state={appearance}
      hidden={!active}
      title={occupied ? seatLabel : ''}
    >
      {occupied && (
        <span className="room-seat-decoration" aria-hidden="true">
          {(['loading', 'unready', 'ready'] as const).map((status) => (
            <span key={status} className={`room-seat-watermark${appearance === status ? ' active' : ''}`}>
              {[...status.toUpperCase()].map((letter, glyph) => (
                <span key={glyph} className="room-seat-letter" style={{ '--glyph': glyph } as CSSProperties}>
                  {letter}
                </span>
              ))}
            </span>
          ))}
        </span>
      )}
      <div
        className="mp-seat-face"
        style={
          occupied
            ? ({ '--base-angle': `${degrees(base)}deg`, '--loading-angle': `${degrees(loading)}deg` } as CSSProperties)
            : undefined
        }
      >
        {occupied && <span className="room-seat-player-tag">P{index + 1}</span>}
        {occupied && (
          <span
            className={`room-seat-ring${resource?.status === 'preparing' && (resource.stage === 'runtime' ? loading == null : base == null) ? ' indeterminate' : ''}`}
            role="progressbar"
            aria-label={t(
              resource?.stage === 'runtime' || resourceReady ? 'room.resourceLoading' : 'room.baseResources',
            )}
            aria-valuemin={0}
            aria-valuemax={100}
            aria-valuenow={resourceReady ? 100 : ((resource?.stage === 'runtime' ? loading : base) ?? undefined)}
          />
        )}
        <div className="mp-seat-drop" data-mp-seat-drop={index} hidden={occupied}>
          <Button
            variant="legacy"
            size="legacy"
            type="button"
            disabled={!ready || !active || occupied}
            onClick={() => {
              void service.takeSeat(index);
            }}
          >
            {t('multiplayer.join')} P{index + 1}
          </Button>
        </div>
        <span className="mp-seat-glyph" data-mp-seat-glyph={index} hidden={!occupied}>
          {multiplayerDisplayInitial(seat?.name ?? (state.seat === index ? state.displayName : ''), '?')}
        </span>
        {index === 0 && (
          <span className="mp-seat-owner-mark" role="img" aria-label={t('multiplayer.host')}>
            <svg viewBox="0 0 24 24" aria-hidden="true">
              <path
                d="m3 6 5 4 4-6 4 6 5-4-2 12H5Z"
                fill="none"
                stroke="currentColor"
                strokeWidth="1.8"
                strokeLinejoin="round"
              />
              <path d="M6 21h12" stroke="currentColor" strokeWidth="1.8" strokeLinecap="round" />
            </svg>
          </span>
        )}
      </div>
      {occupied ? (
        <div className="mp-seat-identity">
          {name}
          <span className="mp-seat-details">
            <span className="mp-seat-loadout" title={seatLabel}>
              {seatLabel}
            </span>
            {controlCaption}
          </span>
          {connection}
        </div>
      ) : (
        <strong className="mp-seat-name" data-mp-seat-name={index}>
          {occupied ? seatLabel : ''}
        </strong>
      )}
      <span className="mp-seat-state" data-mp-seat-state={index}>
        {!occupied
          ? ''
          : t(seat?.offline ? 'multiplayer.playerReconnecting' : seat?.ready ? 'room.ready' : 'room.notReady')}
      </span>
      <span className="mp-seat-me" data-mp-seat-me={index} hidden={state.seat !== index}>
        {t('multiplayer.you')}
      </span>
      {!occupied && controlCaption}
      <Button
        variant="legacy"
        size="legacy"
        className="mp-seat-edit"
        type="button"
        hidden={state.seat !== index || !occupied}
        aria-label={t('room.playerOptions')}
        onClick={(event) => openPanel('personal', event.currentTarget)}
      />
      {index > 0 && (
        <div
          key={seat?.clientId}
          className="mp-seat-moderation"
          hidden={!ready || !owner || room.phase !== 'lobby' || !seat}
        >
          <RoomActionMenu
            enabled={ready && owner && room.phase === 'lobby' && !!seat}
            label={t('room.managePlayer', { seat: index + 1 })}
            triggerContent={<span aria-hidden="true">⋯</span>}
            triggerClassName="room-member-menu-trigger"
            menuClassName="room-member-menu"
            minimumWidth={180}
            align="end"
            items={[
              {
                id: 'move-spectator',
                label: t('room.movePlayerToSpectator'),
                disabled: !!seat?.offline,
                select: () => {
                  if (seat) service.movePlayerToSpectator(index, seat.clientId);
                },
              },
              {
                id: 'remove-player',
                label: t('room.removePlayer', { seat: index + 1 }),
                danger: true,
                select: () => {
                  void service.removePlayer(index);
                },
              },
            ]}
          />
        </div>
      )}
      <div className="mp-seat-resource" hidden={!seat || !resource} data-status={resource?.status}>
        <span className="mp-seat-resource-label">{resourceLabel}</span>
        <ResourceTracks progress={resource ?? null} />
      </div>
    </article>
  );
}
