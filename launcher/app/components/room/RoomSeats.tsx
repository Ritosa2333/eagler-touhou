import {Button as UiButton} from '../../ui/controls';
import {OverflowText} from '../../ui/overflow-text';
import {Ellipsis, Plus, UserRound} from 'lucide-react';
import { useEffect, useLayoutEffect, useRef, type CSSProperties } from 'react';
import { multiplayerDisplayInitial } from '../../../src/launcher/multiplayer-identity.mts';
import type { MultiplayerRoomService, RoomOwnerSnapshot } from '../../services/multiplayer-room';
import { useLocale } from '../../i18n';
import {accountsEnabled} from '../../release-features';
import { networkMessage, roomPresentation } from './presentation';
import {RoomActionMenu} from './RoomActionMenu';
import {ParticipantAvatar} from './ParticipantAvatar';
import {animateSurface, animateUiFrames, type SurfaceAnimation} from '../../ui/surface-motion';
import type { RoomNetworkPresentation, RoomPanelKind, RoomViewContext } from './types';
import {useTh09Ratings, type Th09Rating} from '../../services/th09-ratings';
import {Th09RatingText} from './Th09RatingText';
export interface RoomSeatsProps {
  state: RoomOwnerSnapshot;
  service: Pick<MultiplayerRoomService, 'takeSeat' | 'movePlayerToSpectator' | 'removePlayer' | 'getSnapshot'>;
  network: Pick<RoomNetworkPresentation, 'metric'>;
  context: RoomViewContext;
  openPanel(kind: RoomPanelKind, trigger: HTMLElement, peerId?: string): void;
}
export function RoomSeats(props: RoomSeatsProps) {
  const {players, mine} = useTh09Ratings(props.state.product === 'th09mp');
  return (
    <div
      className="mp-seat-stage"
      id="mpSeatStage"
      data-player-count={props.state.room?.playerCount}
    >
      {[0, 1, 2].map((index) => (
        <RoomSeat {...props} index={index} ratings={players} myRating={mine} key={index} />
      ))}
    </div>
  );
}
function RoomSeat({
  state,
  service,
  network,
  context,
  openPanel,
  index,
  ratings,
  myRating,
}: RoomSeatsProps & { index: number; ratings: Readonly<Record<string, Th09Rating>>; myRating?: Th09Rating }) {
  const { t } = useLocale(),
    presentation = roomPresentation(state, t)!;
  const { room, ready, owner, label } = presentation;
  const node = useRef<HTMLElement>(null), watermark = useRef<HTMLSpanElement>(null),
    previous = useRef<{ room: string; occupant: string; animations: SurfaceAnimation[] } | null>(null);
  const active = index < room.playerCount,
    seat = room.synced ? room.seats?.[index] || null : null;
  const occupied = room.synced === true && (!!seat || state.seat === index),
    occupant = occupied ? seat?.clientId || state.localClientId : '';
  useLayoutEffect(() => {
    const element = node.current,
      before = previous.current;
    if (!element || (before && before.room === room.code && before.occupant === occupant)) return;
    before?.animations.forEach((animation) => {animation.cancel();});
    const next = { room: room.code, occupant, animations: [] as SurfaceAnimation[] };
    previous.current = next;
    if (
      !before ||
      before.room !== room.code ||
      !room.synced ||
      element.hidden
    )
      return;
    for (const child of element.querySelectorAll<HTMLElement>(
      ':scope > .mp-seat-face, :scope > .mp-seat-index, :scope > .mp-seat-name, :scope > .mp-seat-identity, :scope > .mp-seat-state',
    )) {
      if (!child.hidden) {
        const animation = animateSurface(child, 'enter', {kind: 'content'});
        if (animation) next.animations.push(animation);
      }
    }
  }, [room.code, room.synced, occupant]);
  useEffect(
    () => () => {
      previous.current?.animations.forEach((animation) => {animation.cancel();});
    },
    [],
  );
  const seatLabel = label(seat ? seat.loadout : state.preferredLoadout);
  const control =
    state.seat === index
      ? !context.touchEnabled || context.touchMovementMode === 'joystick'
        ? 'normal'
        : context.touchMovementMode === 'touch-unlimited'
          ? 'cheat'
          : 'touch'
      : seat?.controlMode;
  const controlLabel = control ? t(`multiplayer.control.${control}`) : '';
  const message = networkMessage(state, context, t),
    local = seat?.clientId === state.localClientId;
  const diagnostics = (['direct', 'turn'] as const).map((lane) => {
    const metric = seat ? network.metric(seat.clientId, lane) : null,
      measured =
        state.connected &&
        room.synced &&
        !seat?.offline &&
        metric?.state === 'connected' &&
        metric.rtt != null;
    return {lane, label:t(`room.${lane}`), value:measured ? `${Math.max(1, Math.round(metric!.rtt!))}ms` : '—'};
  });
  const values = diagnostics.map(metric => `${metric.label} ${metric.value}`);
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
  const resourceStopped = resource?.status === 'failed' || resource?.status === 'cancelled';
  const duel = state.product === 'th09mp';
  const ratingProfile = duel && accountsEnabled && !resourceStopped ? seat?.profile : undefined;
  const caption = resourceStopped ? resourceLabel : [duel ? '' : seatLabel, controlLabel && `(${controlLabel})`].filter(Boolean).join(' ');
  const resourceReady = !resourceStopped && (resource?.status === 'ready' || seat?.ready === true);
  const canManage = index > 0 && ready && owner && room.phase === 'lobby' && !!seat;
  const base = resourceStopped ? 0 : resourceReady || resource?.stage === 'runtime' ? 100 : resource?.basePercent ?? resource?.percent;
  const loading = resourceStopped ? 0 : resourceReady ? 100 : resource?.stage === 'runtime' ? resource.runtimePercent ?? resource.percent : 0;
  const degrees = (value: unknown) => typeof value === 'number' && Number.isFinite(value) ? Math.max(0, Math.min(100, value)) * 3.6 : 0;
  const appearance = !occupied ? 'empty' : resourceStopped ? resource.status : seat?.offline || !resourceReady ? 'loading' : seat?.ready ? 'ready' : 'unready';
  useLayoutEffect(() => {
    if (!watermark.current || appearance === 'empty' || !active) return;
    const animation = animateUiFrames(watermark.current, {opacity: ['0', '.36'], translate: ['12px 0', '0 0']}, {kind: 'content'});
    return () => animation?.cancel();
  }, [appearance, active]);
  const status = resourceStopped ? resourceLabel : seat?.offline ? t('multiplayer.playerReconnecting') :
    !resourceReady ? resourceLabel || t('room.resourceLoading') : t(seat?.ready ? 'room.ready' : 'room.notReady');
  const ringClass = `room-seat-ring${resource?.status === 'preparing' && (resource.stage === 'runtime' ? loading == null : base == null) ? ' indeterminate' : ''}`;
  const name = <strong className="mp-seat-name" data-mp-seat-name={index} title={seat?.name || `P${index + 1}`}>
    <OverflowText>{seat?.name || (state.seat === index ? state.displayName : '') || `P${index + 1}`}</OverflowText>
  </strong>;
  const connection = <UiButton variant="unstyled"
    className="mp-seat-latency room-seat-network"
    type="button" hidden={!seat} disabled={local || !!message || !!seat?.offline}
    aria-haspopup="dialog" aria-controls="mpRoomPanel"
    data-network-peer={seat && !local ? seat.clientId : undefined}
    aria-label={seat && !local ? `${t('multiplayer.you')} → P${index + 1} | ${values.join(' | ')} | ${t('room.connections')}` : undefined}
    onClick={event => {if (seat) openPanel('network', event.currentTarget, seat.clientId);}}
  >{local ? <span className="room-seat-network-label">{t('room.localDevice')}</span> : diagnostics.map(metric =>
    <span className="room-seat-network-pair" key={metric.lane}><span className="room-seat-network-label">{metric.label}</span><span className="room-seat-network-value">{metric.value}</span></span>
  )}</UiButton>;
  return (
    <article
      ref={node}
      className={`mp-seat${occupied ? ' occupied' : ''}${index === 0 && owner ? ' owner' : ''}${seat?.offline ? ' reconnecting' : ''}${appearance === 'ready' ? ' is-ready' : ''}`}
      data-mp-seat={index}
      data-card-state={appearance}
      data-manageable={canManage || undefined}
      hidden={!active}
      title={occupied ? seatLabel : ''}
    >
      {occupied && <span className="room-seat-decoration" aria-hidden="true">
        <span ref={watermark} className="room-seat-watermark">{appearance.toUpperCase()}</span>
      </span>}
      <div className="mp-seat-face" data-profile-link={accountsEnabled && !!seat?.profile || undefined}
        style={occupied ? {'--base-angle': `${degrees(base)}deg`, '--loading-angle': `${degrees(loading)}deg`} as CSSProperties : undefined}>
        {occupied && <span className="room-seat-player-tag">P{index + 1}</span>}
        {occupied && (resourceStopped ? (
          <span className={ringClass} aria-hidden="true" />
        ) : (
          <span className={ringClass}
            role="progressbar"
            aria-label={t(resource?.stage === 'runtime' || resourceReady ? 'room.resourceLoading' : 'room.baseResources')}
            aria-valuetext={resourceLabel || undefined}
            aria-valuemin={0}
            aria-valuemax={100}
            aria-valuenow={resourceReady ? 100 : (resource?.stage === 'runtime' ? loading : base) ?? undefined}/>
        ))}
        <div className="mp-seat-drop" data-mp-seat-drop={index} hidden={occupied}>
          <UiButton variant="unstyled"
            type="button"
            disabled={!ready || !active || occupied}
            onClick={() => {
              void service.takeSeat(index);
            }}
          >
            <Plus className="size-4 shrink-0" aria-hidden="true"/>{t('multiplayer.join')} P{index + 1}
          </UiButton>
        </div>
        <ParticipantAvatar className="mp-seat-glyph" data-mp-seat-glyph={index} hidden={!occupied}
          profile={seat?.profile} name={seat?.name || ''}>
          {multiplayerDisplayInitial(
            seat?.name ?? (state.seat === index ? state.displayName : ''),
            '?',
          )}
        </ParticipantAvatar>
        {occupied && index > 0 && <span className="room-seat-member-mark" aria-hidden="true"><UserRound/></span>}
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
            <span className="mp-seat-loadout" hidden={!caption && !ratingProfile} title={caption || undefined} role={resourceStopped ? 'status' : undefined}>
              {ratingProfile ? <><Th09RatingText player={ratings[ratingProfile.username]} mine={myRating} games={ratingProfile.th09?.games}/>{caption && ` ${caption}`}</> : caption}
            </span>
            {!resourceStopped && <span className="sr-only" role="status">{status}</span>}
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
          : t(
              seat?.offline
                ? 'multiplayer.playerReconnecting'
                : seat?.ready
                  ? 'room.ready'
                  : 'room.notReady',
            )}
      </span>
      <span className="mp-seat-me" data-mp-seat-me={index} hidden={state.seat !== index}>
        {t('multiplayer.you')}
      </span>
      <UiButton variant="unstyled"
        className="mp-seat-edit"
        type="button"
        hidden={state.seat !== index || !occupied}
        aria-haspopup="dialog"
        aria-controls="mpRoomPanel"
        aria-label={t('room.playerOptions')}
        onClick={(event) => openPanel('personal', event.currentTarget)}
      />
      {index > 0 && <div key={seat?.clientId} className="mp-seat-moderation" hidden={!canManage}>
        <RoomActionMenu enabled={canManage} label={t('room.managePlayer',{seat:index+1})}
          triggerContent={<Ellipsis aria-hidden="true"/>} triggerKind="player" menuClassName="room-member-menu" minimumWidth={180} align="end"
          items={[
            {id:'move-spectator',label:t('room.movePlayerToSpectator'),disabled:!!seat?.offline,select:()=>{if(seat)service.movePlayerToSpectator(index,seat.clientId);}},
            {id:'remove-player',label:t('room.removePlayer',{seat:index+1}),danger:true,select:()=>{
              if(seat&&service.getSnapshot().room?.seats?.[index]?.clientId===seat.clientId)void service.removePlayer(index);
            }},
          ]}/>
      </div>}
    </article>
  );
}
