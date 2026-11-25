import {useCallback, useLayoutEffect, useRef, useState, useSyncExternalStore} from 'react';
import {createPortal} from 'react-dom';
import type {BrowserSession} from '../../session/browser-session';
import {useLocale} from '../../i18n';
import {createSpectatorTimelineView} from '../../shared/spectator-timeline-view.mjs';
import {SpectatorScore} from './SpectatorMatchScore';
import {spectatorStageBounds} from '../../../src/launcher/spectator-playback.mts';
import './spectator-playback.css';

const clock = (frame: number) => `${Math.floor(frame / 3600)}:${String(Math.floor(frame / 60) % 60).padStart(2, '0')}`;
const presentations = new WeakMap<BrowserSession, {epoch: number | null; expanded: boolean}>();

/** Cooperative reports/actions adapt to the same view used by TH09's native
 * timeline. The view owns controls, pointer handling and all timeline markup. */
export function SpectatorPlayback({session, score, minutes, names}: {
  session: BrowserSession; score?: readonly [number,number]; minutes?: number; names?: readonly [string,string];
}) {
  const state = useSyncExternalStore(session.spectatorPlayback.subscribe,
    session.spectatorPlayback.getSnapshot, session.spectatorPlayback.getSnapshot);
  const {t} = useLocale();
  const [firstWins, secondWins] = score ?? [];
  const host = useRef<HTMLDivElement>(null);
  const view = useRef<ReturnType<typeof createSpectatorTimelineView> | null>(null);
  const viewEpoch = useRef<number | null>(null);
  const [scoreSlot, setScoreSlot] = useState<HTMLElement | null>(null);

  const retireView = useCallback(() => {
    if (!view.current) return;
    presentations.set(session, {epoch: viewEpoch.current, expanded: view.current.panel.dataset.collapsed !== 'true'});
    view.current.destroy();
    view.current = null;
    setScoreSlot(null);
  }, [session]);
  useLayoutEffect(() => retireView, [retireView]);
  useLayoutEffect(() => {
    if (!state || !host.current) {retireView(); return;}
    const runtime = session.getSnapshot().runtime;
    const epoch = runtime?.epoch ?? null;
    const stageBounds = runtime?.game === 'th14' ? spectatorStageBounds(state) : null;
    if (view.current && viewEpoch.current !== epoch) retireView();
    const keyFor = (playback: NonNullable<typeof state>) => {
      const activeRuntime = session.getSnapshot().runtime;
      const segment = activeRuntime?.game === 'th14' ? spectatorStageBounds(playback).min
        : activeRuntime?.game === 'th075'
          ? playback.nodes.find(node => node.kind === 'match' && node.frame === playback.min)?.id : undefined;
      return `${activeRuntime?.epoch ?? ''}:${playback.run}${segment === undefined ? '' : ':' + segment}`;
    };
    if (!view.current) {
      const previous = presentations.get(session);
      viewEpoch.current = epoch;
      view.current = createSpectatorTimelineView({
        document: host.current.ownerDocument, mount: host.current, idPrefix: 'cooperativeSpectator',
        expanded: previous?.epoch === epoch ? previous.expanded : true,
        async onAction(action, frame, key) {
          const current = session.spectatorPlayback.getSnapshot();
          if (!current || key !== keyFor(current)) return;
          await session.controlSpectator(action, frame, current.run);
        },
      });
    }
    const icon = (file: string) => new URL(`assets/${file}`, session.baseUrl).href;
    const versus = firstWins !== undefined && secondWins !== undefined;
    if (versus) setScoreSlot(view.current.slots.summary);
    view.current.updateLabels({controls: t('spectator.controls'), summary: t(versus ? 'th09.matchInformation' : 'spectator.controls'), error: t('spectator.actionFailed'), expand: t('spectator.expand'),
      collapse: t('spectator.collapse'), progress: t('spectator.progress'), nodes: t(versus ? 'spectator.versusNodes' : 'spectator.nodes'),
      pause: t('spectator.pause'), play: t('spectator.resume'), live: t('spectator.returnLive')},
      {toggle: icon('room-caret-right.svg'), pause: icon('player-pause.svg'),
        play: icon('player-play.svg'), live: icon('player-live.svg')});
    const labelFor = (node: typeof state.nodes[number]) => node.kind === 'match' || node.kind === 'round'
      ? t(node.kind === 'match' ? 'spectator.matchNode' : 'spectator.roundNode', {number:node.stage})
      : node.label
      ? `${node.label}${node.kind === 'boss' ? ' Boss' : ''}`
      : node.stage > 255
        ? t(node.kind === 'boss' ? 'spectator.bossNodeUnknown' : 'spectator.stageNodeUnknown')
        : t(node.kind === 'boss' ? 'spectator.bossNode' : 'spectator.stageNode', {stage: node.stage});
    const {min, edge} = stageBounds ?? state;
    const position = stageBounds ? Math.max(min, Math.min(edge, state.position)) : state.position;
    const nodes = stageBounds ? state.nodes.filter(node => node.frame >= min && node.frame <= edge) : state.nodes;
    const liveEdge = runtime?.game === 'th075' ? state.received : state.edge;
    // A paused seek still rebuilds in the background. Keep Pause available
    // until reconstruction stops, then show Play for the retained pause intent.
    const paused = state.paused && !state.seeking;
    let currentNode: typeof state.nodes[number] | undefined;
    for (const node of nodes) if (node.frame <= position && node.frame <= edge) currentNode = node;
    const statusKey = state.seeking ? 'spectator.seeking' : paused ? 'spectator.paused'
      : state.ended && state.position >= state.edge ? 'spectator.ended' : state.following ? 'spectator.live'
        : position >= state.edge ? 'spectator.waiting' : 'spectator.playing';
    view.current.update({key: keyFor(state), min, position, edge,
      received: state.received, target: stageBounds ? Math.max(min, Math.min(edge, state.target)) : state.target,
      paused, seeking: state.seeking, following: state.following, ended: state.ended, timeOrigin: min,
      liveDisabled: state.following && !state.paused && !state.seeking && state.position >= liveEdge,
      summaryText: versus ? undefined : currentNode ? labelFor(currentNode) : t('spectator.controls'),
      durationText: minutes === undefined ? `${clock(position - min)} / ${clock(edge - min)}` : t('th09.elapsed', {minutes}),
      statusText: t(statusKey), lagText: t('spectator.lag', {seconds: Math.ceil(Math.max(0, state.received - state.position) / 60)}),
      marks: nodes.map(node => ({id: String(node.id), frame: node.frame, kind: node.kind,
        label: labelFor(node), current: node.id === currentNode?.id})),
    });
  }, [session, state, t, retireView, firstWins, secondWins, minutes]);

  return state ? <><div ref={host} className="spectator-playback-host"/>
    {score && scoreSlot && createPortal(<SpectatorScore wins={score} names={names}/>, scoreSlot)}</> : null;
}
