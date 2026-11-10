import {Pause, Play, RotateCcw} from 'lucide-react';
import {useId} from 'react';
import {useLocale} from '../../i18n';
import {Button as UiButton} from '../../ui/controls';
import {MainSelect} from '../launcher/MainSelect';
import {ReplaySeekBar, replayClock} from './ReplaySeekBar';
import {FullscreenIcons} from './utility-icons';

export interface ReplayTransportState {
  path: string; frame: number; total: number; paused: boolean;
  seeking: boolean; target: number | null; rate: number; ended: boolean;
  roundStarts?: number[]; startFrame?: number;
}
export type ReplayControlAction = 'pause' | 'play' | 'seek' | 'rate';
const rates = [.5, 1, 2, 4, 8, 0];

/** Presentation shared by the runtime workspace and its interactive handbook example. */
export function ReplayTransport({playback, fps = 60, busy = false, fullscreen = false, speedId, onControl, onToggleFullscreen}: {
  playback?: ReplayTransportState; busy?: boolean; fps?: number; fullscreen?: boolean; speedId?: string;
  onControl(action: ReplayControlAction, value?: number): void;
  onToggleFullscreen(): void;
}) {
  const {t} = useLocale(), id = useId(), start = playback?.startFrame ?? 0;
  const paused = playback?.paused || playback?.ended;
  return <section className="replay-transport" aria-label={t('replay.ui.transport')}>
    <div className="replay-transport-buttons">
      <UiButton variant="primary" size="icon" type="button" className="replay-toggle replay-button" aria-label={t(paused ? 'replay.ui.play' : 'replay.ui.pause')} disabled={!playback || busy} onClick={() => onControl(paused ? 'play' : 'pause')}>
        {paused ? <Play aria-hidden="true"/> : <Pause aria-hidden="true"/>}
      </UiButton>
      <UiButton size="lg" type="button" className="replay-restart replay-button" aria-label={t('replay.ui.restart')} disabled={!playback || busy} onClick={() => onControl('seek', start)}><RotateCcw aria-hidden="true"/><span>{t('replay.ui.restart')}</span></UiButton>
      <div className="replay-speed"><MainSelect id={speedId ?? id} className="option-select" aria-label={t('replay.ui.speed')} value={playback?.rate ?? 1} disabled={!playback || busy} onChange={event => onControl('rate', Number(event.target.value))}>
        {rates.map(rate => <option key={rate} value={rate}>{rate ? `${rate}×` : t('replay.ui.fullSpeed')}</option>)}
      </MainSelect></div>
    </div>
    <ReplaySeekBar key={`${playback?.path ?? ''}:${start}`} frame={Math.max(0, (playback?.frame ?? 0) - start)} total={Math.max(0, (playback?.total ?? 0) - start)} fps={fps} roundStarts={playback?.roundStarts} disabled={busy || !playback} onSeek={target => onControl('seek', target + start)}/>
    <UiButton variant="icon" type="button" className="replay-fullscreen replay-button" aria-label={t('replay.ui.fullscreen')} onClick={onToggleFullscreen} data-fullscreen={fullscreen}><FullscreenIcons/></UiButton>
    <span className="replay-seek-status" role="status">{playback?.seeking ? t('replay.ui.seek', {time:replayClock(Math.max(0, (playback.target ?? 0) - start), fps)}) : playback?.ended ? t('replay.ui.ended') : ''}</span>
  </section>;
}
