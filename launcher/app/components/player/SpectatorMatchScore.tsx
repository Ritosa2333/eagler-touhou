import { useLayoutEffect, useRef, useState } from 'react';
import { createPortal } from 'react-dom';
import { useLocale } from '../../i18n';
import { mountSpectatorPanel } from './spectator-panel-host';
import './spectator-playback.css';

/** The Runtime keeps its playback cursor/listeners; the Host information column
 * owns placement. Room wins render into the real controls' score slot. */
export function SpectatorMatchScore({
  wins,
  minutes,
  names = ['', ''],
}: {
  wins: readonly [number, number];
  minutes: number;
  names?: readonly [string, string];
}) {
  const { locale, t } = useLocale();
  const host = useRef<HTMLDivElement>(null);
  const [target, setTarget] = useState<HTMLElement | null>(null);
  useLayoutEffect(() => {
    const frame = document.getElementById('gameFrame') as HTMLIFrameElement | null;
    if (!frame || !host.current) return;
    return mountSpectatorPanel(frame, host.current, setTarget);
  }, []);
  useLayoutEffect(() => {
    if (!target) return;
    const native = target.ownerDocument;
    native.documentElement.lang = locale;
    const panel = native.getElementById('spectatorTimeline');
    if (panel?.dataset.collapsed === 'true') panel.querySelector<HTMLButtonElement>('.spectator-toggle')?.click();
    const icons: Record<string, string> = {
      pause: 'player-pause.svg',
      play: 'player-play.svg',
      live: 'player-live.svg',
      toggle: 'room-caret-right.svg',
    };
    panel?.querySelectorAll<HTMLImageElement>('[data-icon]').forEach((icon) => {
      const path = icons[icon.dataset.icon || ''];
      if (path) icon.src = new URL(`assets/${path}`, document.baseURI).href;
    });
    panel?.style.setProperty('--ui-font', getComputedStyle(document.documentElement).getPropertyValue('--ui-font'));
    if (native === document) return;
    // Copy only the page's existing font faces. Loading the whole launcher CSS
    // in the game frame would alter its canvas/dialog layout.
    const fonts = native.createElement('style');
    fonts.id = 'spectatorUiFonts';
    const faces: string[] = [];
    for (const sheet of Array.from(document.styleSheets)) {
      try {
        for (const rule of Array.from(sheet.cssRules)) {
          if (rule.type !== 5 || !/ET (?:Yatra|Chill Round)/.test(rule.cssText)) continue;
          faces.push(
            rule.cssText.replace(
              /url\(["']?([^"')]+)["']?\)/g,
              (_, path) => `url("${new URL(path, sheet.href || document.baseURI).href}")`,
            ),
          );
        }
      } catch {
        /* Cross-origin styles do not own the launcher fonts. */
      }
    }
    fonts.textContent = faces.join('\n');
    native.head.append(fonts);
    return () => fonts.remove();
  }, [target, locale]);
  useLayoutEffect(() => {
    const panel = target?.closest<HTMLElement>('#spectatorTimeline');
    if (!panel) return;
    panel.dataset.elapsedLabel = t('th09.elapsed', { minutes });
    const duration = panel.querySelector('#spectatorMatchDuration');
    if (duration) duration.textContent = panel.dataset.elapsedLabel;
  }, [target, minutes, locale, t]);
  return (
    <>
      <div ref={host} className="th09-playback-host" />
      {target
        ? createPortal(
            <>
              <span className="spectator-score-player" title={names[0]}>{`P1${names[0] ? `(${names[0]})` : ''}`}</span>
              <span className="spectator-score-value">{`<${wins[0]} : ${wins[1]}>`}</span>
              <span className="spectator-score-player" title={names[1]}>{`P2${names[1] ? `(${names[1]})` : ''}`}</span>
            </>,
            target,
          )
        : null}
    </>
  );
}
