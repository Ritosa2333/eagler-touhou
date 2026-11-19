import {useCallback, useLayoutEffect, useRef, useSyncExternalStore} from 'react';
import type {BrowserSession} from '../../session/browser-session';
import {useLocale} from '../../i18n';
import {createSpectatorTimelineView} from '../../shared/spectator-timeline-view.mjs';
import './spectator-playback.css';

const clock = (frame: number) => `${Math.floor(frame / 3600)}:${String(Math.floor(frame / 60) % 60).padStart(2, '0')}`;
const presentations = new WeakMap<BrowserSession, {epoch: number | null; expanded: boolean}>();

/** Cooperative reports/actions adapt to the same view used by TH09's native
 * timeline. The view owns controls, pointer handling and all timeline markup. */
export function SpectatorPlayback({session, score}: {session: BrowserSession; score?: readonly [number,number]}) {
  const state = useSyncExternalStore(session.spectatorPlayback.subscribe,
    session.spectatorPlayback.getSnapshot, session.spectatorPlayback.getSnapshot);
  const {t} = useLocale();
  const host = useRef<HTMLDivElement>(null);
  const view = useRef<ReturnType<typeof createSpectatorTimelineView> | null>(null);
  const viewEpoch = useRef<number | null>(null);

  const retireView = useCallback(() => {
    if (!view.current) return;
    presentations.set(session, {epoch: viewEpoch.current, expanded: view.current.panel.dataset.collapsed !== 'true'});
    view.current.destroy();
    view.current = null;
  }, [session]);
  useLayoutEffect(() => retireView, [retireView]);
  useLayoutEffect(() => {
    if (!state || !host.current) {retireView(); return;}
    const epoch = session.getSnapshot().runtime?.epoch ?? null;
    if (view.current && viewEpoch.current !== epoch) retireView();
    const keyFor = (run: string) => `${session.getSnapshot().runtime?.epoch ?? ''}:${run}`;
    if (!view.current) {
      const previous = presentations.get(session);
      viewEpoch.current = epoch;
      view.current = createSpectatorTimelineView({
        document: host.current.ownerDocument, mount: host.current, idPrefix: 'cooperativeSpectator',
        expanded: previous?.epoch === epoch ? previous.expanded : true,
        async onAction(action, frame, key) {
          const current = session.spectatorPlayback.getSnapshot();
          if (!current || key !== keyFor(current.run)) return;
          await session.controlSpectator(action, frame, current.run);
        },
      });
    }
    const icon = (file: string) => new URL(`assets/${file}`, session.baseUrl).href;
    view.current.updateLabels({controls: t('spectator.controls'), summary: t('spectator.controls'), error: t('spectator.actionFailed'), expand: t('spectator.expand'),
      collapse: t('spectator.collapse'), progress: t('spectator.progress'), nodes: t('spectator.nodes'),
      pause: t('spectator.pause'), play: t('spectator.resume'), live: t('spectator.returnLive')},
      {toggle: icon('room-caret-right.svg'), pause: icon('player-pause.svg'),
        play: icon('player-play.svg'), live: icon('player-live.svg')});
    const labelFor = (node: typeof state.nodes[number]) => node.label
      ? `${node.label}${node.kind === 'boss' ? ' Boss' : ''}`
      : node.stage > 255
        ? t(node.kind === 'boss' ? 'spectator.bossNodeUnknown' : 'spectator.stageNodeUnknown')
        : t(node.kind === 'boss' ? 'spectator.bossNode' : 'spectator.stageNode', {stage: node.stage});
    const position = state.position;
    // A paused seek still rebuilds in the background. Keep Pause available
    // until reconstruction stops, then show Play for the retained pause intent.
    const paused = state.paused && !state.seeking;
    let currentNode: typeof state.nodes[number] | undefined;
    for (const node of state.nodes) if (node.frame <= position && node.frame <= state.edge) currentNode = node;
    const statusKey = state.seeking ? 'spectator.seeking' : paused ? 'spectator.paused'
      : state.ended && position >= state.edge ? 'spectator.ended' : state.following ? 'spectator.live'
        : position >= state.edge ? 'spectator.waiting' : 'spectator.playing';
    view.current.update({key: keyFor(state.run), min: state.min, position, edge: state.edge,
      received: state.received, target: state.target, paused, seeking: state.seeking,
      following: state.following, ended: state.ended, timeOrigin: state.min,
      liveDisabled: state.following && !state.paused && !state.seeking && position >= state.edge,
      summaryText: score ? `<${score[0]} : ${score[1]}>` : currentNode ? labelFor(currentNode) : t('spectator.controls'),
      durationText: `${clock(Math.max(0, position - state.min))} / ${clock(state.edge - state.min)}`,
      statusText: t(statusKey), lagText: t('spectator.lag', {seconds: Math.ceil(Math.max(0, state.received - position) / 60)}),
      marks: state.nodes.map(node => ({id: String(node.id), frame: node.frame, kind: node.kind,
        label: labelFor(node), current: node.id === currentNode?.id})),
    });
  }, [session, state, t, retireView, score?.[0], score?.[1]]);

  return state ? <div ref={host} className="spectator-playback-host"/> : null;
}
