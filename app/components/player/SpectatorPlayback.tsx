import {useEffect, useRef, useState, useSyncExternalStore} from 'react';
import type {BrowserSession} from '../../session/browser-session';
import {useLocale} from '../../i18n';

const clock = (frame: number) => `${Math.floor(frame / 3600)}:${String(Math.floor(frame / 60) % 60).padStart(2, '0')}`;
export function SpectatorPlayback({session}: {session: BrowserSession}) {
  const state = useSyncExternalStore(session.spectatorPlayback.subscribe, session.spectatorPlayback.getSnapshot, session.spectatorPlayback.getSnapshot);
  const {t} = useLocale();
  const [expanded, setExpanded] = useState(false), [drag, setDrag] = useState<{run: string; min: number; edge: number; frame: number} | null>(null);
  const [failure, setFailure] = useState(''); const request = useRef(0);
  useEffect(() => {request.current++; setFailure(''); setDrag(null);}, [state?.run]);
  if (!state) return null;
  const value = drag?.run === state.run ? drag : null;
  const position = value?.frame ?? (state.seeking ? state.target : state.position);
  const min = value?.min ?? state.min, edge = value?.edge ?? state.edge;
  const perform = async (action: 'pause' | 'play' | 'seek' | 'live', frame?: number) => {
    const id = ++request.current; setFailure('');
    try {await session.controlSpectator(action, frame, state.run);}
    catch (error) {if (id === request.current) setFailure(error instanceof Error ? error.message : String(error));}
  };
  const finish = () => {if (value) {void perform('seek', Math.max(state.min, Math.min(state.edge, value.frame))); setDrag(null);}};
  return <section className="spectator-playback" aria-label={t('spectator.controls')}>
    <button type="button" className="spectator-playback-toggle" aria-expanded={expanded} aria-controls="spectatorPlaybackDetails" onClick={() => setExpanded(!expanded)}>
      {t(state.paused ? 'spectator.paused' : state.seeking ? 'spectator.seeking' : state.following ? 'spectator.live' : 'spectator.playing')}
      {' · '}{clock(Math.max(0, position - state.min))}
    </button>
    <div id="spectatorPlaybackDetails" hidden={!expanded}>
      <div className="spectator-playback-nodes" aria-label={t('spectator.nodes')}>
        {state.nodes.map(node => <button type="button" key={`${state.run}:${node.id}`} disabled={node.frame > state.edge}
          title={clock(node.frame - state.min)} onClick={() => {void perform('seek', node.frame);}}>
          {node.label ? `${node.label}${node.kind === 'boss' ? ' Boss' : ''}` : t(node.kind === 'boss' ? 'spectator.bossNode' : 'spectator.stageNode', {stage: node.stage})}
        </button>)}
      </div>
      <input type="range" aria-label={t('spectator.progress')} min={min} max={Math.max(min, edge)} step={1} value={Math.min(edge, Math.max(min, position))}
        aria-valuetext={clock(Math.max(0, position - state.min))} disabled={edge <= min}
        onPointerDown={event => {setDrag({run: state.run, min: state.min, edge: state.edge, frame: position}); event.currentTarget.setPointerCapture?.(event.pointerId);}}
        onChange={event => {const frame = Number(event.currentTarget.value); setDrag({run: state.run, min, edge, frame});}}
        onPointerUp={finish} onKeyUp={finish} onBlur={finish} onPointerCancel={() => setDrag(null)}/>
      <div className="spectator-playback-actions">
        <span>{clock(Math.max(0, position - state.min))} / {clock(Math.max(0, edge - state.min))}</span>
        <button type="button" onClick={() => {void perform(state.paused ? 'play' : 'pause');}}>{t(state.paused ? 'spectator.resume' : 'spectator.pause')}</button>
        <button type="button" onClick={() => {void perform('live');}}>{t('spectator.returnLive')}</button>
      </div>
      {failure && <p role="alert">{failure}</p>}
    </div>
  </section>;
}
