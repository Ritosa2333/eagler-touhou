import { useLayoutEffect, useRef, useState } from 'react';
import { createPortal } from 'react-dom';
import { useLocale } from '../../i18n';
import { mountSpectatorPanel } from './spectator-panel-host';
import './spectator-playback.css';

const hostedPanels = new WeakSet<HTMLElement>();

/** The Runtime keeps its playback cursor/listeners; the Host information column
 * owns placement. Room wins render into the real controls' score slot. */
export function SpectatorMatchScore({ wins, minutes, names = ['', ''] }: {
  wins: readonly [number, number]; minutes: number; names?: readonly [string, string];
}) {
  const { t } = useLocale();
  const host = useRef<HTMLDivElement>(null);
  const [target, setTarget] = useState<HTMLElement | null>(null);
  useLayoutEffect(() => {
    const frame = document.getElementById('gameFrame') as HTMLIFrameElement | null;
    if (!frame || !host.current) return;
    return mountSpectatorPanel(frame, host.current, setTarget);
  }, []);
  useLayoutEffect(() => {
    if (!target) return;
    const panel = target.closest<HTMLElement>('#spectatorTimeline');
    if (panel && !hostedPanels.has(panel)) {
      hostedPanels.add(panel);
      if (panel.dataset.collapsed === 'true') panel.querySelector<HTMLButtonElement>('.spectator-toggle')?.click();
    }
    const icons: Record<string,string> = {pause:'player-pause.svg',play:'player-play.svg',live:'player-live.svg',toggle:'room-caret-right.svg'};
    panel?.querySelectorAll<HTMLImageElement>('[data-icon]').forEach(icon => {
      const path = icons[icon.dataset.icon || ''];
      if (path) icon.src = new URL(`assets/${path}`,document.baseURI).href;
    });
  }, [target]);
  useLayoutEffect(() => {
    const panel = target?.closest<HTMLElement>('#spectatorTimeline');
    if (!panel) return;
    panel.dataset.elapsedLabel = t('th09.elapsed', {minutes});
    const duration = panel.querySelector('#spectatorMatchDuration');
    if (duration) duration.textContent = panel.dataset.elapsedLabel;
  }, [target, minutes, t]);
  return <><div ref={host} className="th09-playback-host" />{target ? createPortal(<>
    <span className="spectator-score-player" title={names[0]}>{`P1${names[0] ? `(${names[0]})` : ''}`}</span>
    <span className="spectator-score-value">{`<${wins[0]} : ${wins[1]}>`}</span>
    <span className="spectator-score-player" title={names[1]}>{`P2${names[1] ? `(${names[1]})` : ''}`}</span>
  </>, target) : null}</>;
}
