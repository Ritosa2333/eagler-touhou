import {useState, type CSSProperties} from 'react';

export const replayClock = (frame: number, fps = 60) => {const seconds = Math.floor(Math.max(0, frame) / fps); return `${Math.floor(seconds / 60)}:${String(seconds % 60).padStart(2, '0')}`;};

export function ReplaySeekBar({frame, total, roundStarts = [], fps = 60, disabled, onSeek}: {
  frame: number; total: number; roundStarts?: readonly number[]; fps?: number; disabled: boolean; onSeek(frame: number): void;
}) {
  const [preview, setPreview] = useState<number | null>(null), shown = preview ?? frame;
  const seek = (target: number) => {setPreview(null); onSeek(target);};
  return <div className="replay-seek-row">
    <div className="replay-seek-track">
      <input type="range" min="0" max={Math.max(1, total)} step="1" value={shown} disabled={!total || disabled}
        style={{'--replay-progress': `${Math.min(100, shown / Math.max(1, total) * 100)}%`} as CSSProperties}
        aria-label="录像进度" aria-valuetext={replayClock(shown, fps)}
        onChange={event => setPreview(Number(event.target.value))}
        onPointerUp={event => seek(Number(event.currentTarget.value))}
        onKeyUp={event => {if (['ArrowLeft','ArrowRight','Home','End','PageUp','PageDown'].includes(event.key)) seek(Number(event.currentTarget.value));}}
        onPointerCancel={() => setPreview(null)}/>
      {roundStarts.map((start, index) => Number.isSafeInteger(start) && start >= 0 && start < total && <button
        type="button" className="replay-round-marker" key={start} disabled={disabled}
        style={{left: `calc(6.5px + (100% - 13px) * ${start / total})`}}
        title={`第 ${index + 1} 回合 · ${replayClock(start, fps)}`} aria-label={`第 ${index + 1} 回合，${replayClock(start, fps)}`}
        onClick={() => seek(start)}>{index + 1}</button>)}
    </div>
    <span>{replayClock(shown, fps)}<small> / {total ? replayClock(total, fps) : '--:--'}</small></span>
  </div>;
}
