import {Button as UiButton} from '../../ui/controls';
import {useState} from 'react';
import {Slider} from '../../ui/slider';
import {useLocale} from '../../i18n';

export const replayClock = (frame: number, fps = 60) => {const seconds = Math.floor(Math.max(0, frame) / fps); return `${Math.floor(seconds / 60)}:${String(seconds % 60).padStart(2, '0')}`;};

export function ReplaySeekBar({frame, total, roundStarts = [], fps = 60, disabled, onSeek}: {
  frame: number; total: number; roundStarts?: readonly number[]; fps?: number; disabled: boolean; onSeek(frame: number): void;
}) {
  const {t}=useLocale();
  const [preview, setPreview] = useState<number | null>(null), shown = preview ?? frame;
  const seek = (target: number) => {setPreview(null); onSeek(target);};
  return <div className="replay-seek-row">
    <div className="replay-seek-track">
      <Slider min={0} max={Math.max(1, total)} step={1} value={shown} disabled={!total || disabled}
        aria-label={t('replay.ui.progress')} aria-valuetext={replayClock(shown, fps)}
        onValueChange={setPreview} onValueCommit={seek} onValueCancel={() => setPreview(null)}/>
      {roundStarts.map((start, index) => Number.isSafeInteger(start) && start >= 0 && start < total && <UiButton variant="unstyled"
        type="button" className="replay-round-marker" key={start} disabled={disabled}
        style={{left: `calc(6.5px + (100% - 13px) * ${start / total})`}}
        title={t('replay.ui.round',{round:index+1,time:replayClock(start, fps)})} aria-label={t('replay.ui.roundAria',{round:index+1,time:replayClock(start, fps)})}
        onClick={() => seek(start)}>{index + 1}</UiButton>)}
    </div>
    <span>{replayClock(shown, fps)}<small> / {total ? replayClock(total, fps) : '--:--'}</small></span>
  </div>;
}
