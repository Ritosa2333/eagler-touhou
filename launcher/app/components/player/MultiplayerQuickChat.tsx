import {Button as UiButton} from '../../ui/controls';
import { useCallback, useLayoutEffect, useRef, useState, useSyncExternalStore } from 'react';
import { createPortal } from 'react-dom';
import {
  QUICK_CHAT_ROWS,
  type QuickChatPhrase,
} from '../../../src/contracts/multiplayer-quick-chat.mts';
import type {
  MultiplayerQuickChatModel,
  QuickChatEntry,
} from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import { useLocale } from '../../i18n';
import {animateUiFrames, type SurfaceAnimation} from '../../ui/surface-motion';

/** React owns every node. Shared effects retain the original iframe input,
 * scrolling and independent move/expiry animations; they never own chat state. */
export function MultiplayerQuickChat({ model }: { model: MultiplayerQuickChatModel }) {
  const { t } = useLocale();
  const root = useRef<HTMLElement>(null),
    log = useRef<HTMLDivElement>(null);
  const rows = useRef(new Map<number, HTMLParagraphElement>());
  const [rowCarrier] = useState(() => document.createElement('div'));
  const fades = useRef(new Map<number, SurfaceAnimation>()),
    moves = useRef(new Map<HTMLParagraphElement, SurfaceAnimation>());
  const before = useRef<{
    top: number;
    following: boolean;
    positions: Map<HTMLParagraphElement, { top: number; opacity: number }>;
  } | null>(null);
  const capture = useCallback(() => {
    const element = log.current;
    if (!element) return;
    const top = element.scrollTop;
    before.current = {
      top,
      following: element.scrollHeight - element.clientHeight - top <= 4,
      positions: new Map(
        [...rows.current.values()]
          .filter((row) => row.parentElement === element)
          .map((row) => [
            row,
            {
              top: row.getBoundingClientRect().top,
              opacity: Number(getComputedStyle(row).opacity),
            },
          ]),
      ),
    };
  }, []);
  const subscribe = useCallback(
    (notify: () => void) =>
      model.subscribe(() => {
        capture();
        notify();
      }),
    [model, capture],
  );
  const state = useSyncExternalStore(subscribe, model.getSnapshot, model.getSnapshot);
  const context = state.context;
  const th09 = context?.room.startsWith('th09mp-') === true;

  const isReduced = useCallback(() => {
    return !!(model.getSnapshot().context?.lessMotion ||
      matchMedia('(prefers-reduced-motion: reduce)').matches);
  }, [model]);
  useLayoutEffect(() => {
    const element = root.current;
    if (!element || context?.inRoom) return;
    const prevent = (event: Event) => event.preventDefault();
    const stop = (event: Event) => event.stopPropagation();
    const key = (event: KeyboardEvent) => {
      event.stopPropagation();
      if (event.key === 'Escape') model.dismiss();
    };
    const blocked = [
      'keyup',
      'pointerdown',
      'pointermove',
      'pointerup',
      'pointercancel',
      'touchstart',
      'touchmove',
      'touchend',
      'touchcancel',
    ];
    element.addEventListener('keydown', key);
    for (const name of ['pointerdown', 'mousedown'])
      element.addEventListener(name, prevent, { capture: true });
    for (const name of blocked) element.addEventListener(name, stop);
    const detach = model.setExpiryPresenter((entry: QuickChatEntry) => {
      const row = rows.current.get(entry.id);
      if (
        isReduced() ||
        !model.getSnapshot().context?.visible ||
        row?.parentElement !== log.current
      )
        return;
      const animation = animateUiFrames(row, {opacity: ['1', '0']});
      if (!animation) return;
      fades.current.set(entry.id, animation);
      return animation.finished.then(() => {});
    });
    return () => {
      element.removeEventListener('keydown', key);
      for (const name of ['pointerdown', 'mousedown'])
        element.removeEventListener(name, prevent, { capture: true });
      for (const name of blocked) element.removeEventListener(name, stop);
      for (const animation of fades.current.values()) animation.cancel();
      fades.current.clear();
      for (const animation of moves.current.values()) animation.cancel();
      moves.current.clear();
      detach();
      before.current = null;
    };
  }, [model, context?.inRoom, isReduced]);
  useLayoutEffect(() => {
    const element = log.current;
    if (!element) return;
    const previous = before.current;
    before.current = null;
    for (const animation of moves.current.values()) animation.cancel();
    moves.current.clear();
    for (const [id, animation] of fades.current)
      if (!state.entries.some((entry) => entry.id === id)) {
        animation.cancel();
        fades.current.delete(id);
      }
    // A muted row is detached, not destroyed: the same React-owned node and
    // its in-flight expiry animation return on unmute, exactly as main.
    const visible = state.entries.filter((entry) => !state.muted.includes(entry.clientId));
    const visibleIds = new Set(visible.map((entry) => entry.id));
    for (const [id, row] of rows.current) if (!visibleIds.has(id)) row.remove();
    visible.forEach((entry, index) => {
      const row = rows.current.get(entry.id),
        current = element.children[index];
      if (row && current !== row) element.insertBefore(row, current ?? null);
    });
    element.scrollTop = !previous || previous.following ? element.scrollHeight : previous.top;
    if (isReduced()) {for (const animation of fades.current.values()) animation.complete(); return;}
    for (const entry of state.entries) {
      const row = rows.current.get(entry.id);
      if (!row || row.parentElement !== element) continue;
      const position = previous?.positions.get(row),
        delta = position ? position.top - row.getBoundingClientRect().top : 6;
      const fading = fades.current.has(entry.id),
        opacity = position?.opacity ?? 0;
      if (Math.abs(delta) < 0.5 && (fading || opacity >= 0.999)) continue;
      const animation = animateUiFrames(row, {translate: [`0 ${delta}px`, '0 0'], ...(!fading ? {opacity: [String(opacity), '1']} : {})});
      if (animation) moves.current.set(row, animation);
    }
  }, [state, isReduced]);
  const label = (phrase: QuickChatPhrase) => (context?.language === 'en' ? phrase.en : phrase.zh);
  const muteButton = (
    <UiButton variant="quiet"
      type="button"
      className="mp-quick-chat-mute"
      aria-expanded={state.muteOpen}
      onClick={model.toggleMute}
    >
      {t(state.muteOpen ? 'chat.back' : 'chat.mute')}
    </UiButton>
  );
  const section = (
    <section
      ref={root}
      className={`mp-quick-chat${th09 ? ' th09-player-chat' : ''}`}
      hidden={!context?.visible || context.inRoom}
      aria-label={t('chat.players')}
    >
      <UiButton variant="quiet"
        type="button"
        className="mp-quick-chat-prompt"
        aria-expanded={state.pickerOpen || state.muteOpen}
        disabled={!context?.connected}
        onClick={model.togglePicker}
      >
        <img
          className="th09-chat-icon"
          src={new URL('assets/player-chat.svg', document.baseURI).href}
          alt=""
        />
        {t('room.quickChat')}
      </UiButton>
      <div className="mp-quick-chat-picker" hidden={!state.pickerOpen}>
        {QUICK_CHAT_ROWS.map(phrases => (
          <div key={phrases.map(phrase => phrase.id).join('|')} className={`mp-quick-chat-row${phrases.length === 2 ? ' paired' : ''}`}>
            {phrases.map((phrase) => (
              <UiButton
                key={phrase.id}
                type="button"
                className="mp-quick-chat-phrase"
                data-phrase={phrase.id}
                title={label(phrase)}
                disabled={
                  !context?.connected || (context.localSeat == null && !context.localSpectator)
                }
                onClick={() => model.sendPhrase(phrase.id)}
              >
                {label(phrase)}
              </UiButton>
            ))}
          </div>
        ))}
        {!QUICK_CHAT_ROWS.length && <p>{t('chat.empty')}</p>}
        {!state.muteOpen && muteButton}
      </div>
      <div className="mp-quick-chat-picker" hidden={!state.muteOpen}>
        {context?.seats.flatMap((seat, index) =>
          !seat || index === context.localSeat
            ? []
            : [
                <UiButton variant="quiet"
                  key={seat.clientId}
                  type="button"
                  className="mp-quick-chat-mute-member"
                  aria-pressed={state.muted.includes(seat.clientId)}
                  onClick={() => model.toggleMember(seat.clientId)}
                >{`P${index + 1} ${seat.name} | ${t(state.muted.includes(seat.clientId) ? 'chat.unmute' : 'chat.mute')}`}</UiButton>,
              ],
        )}
        {context?.spectators
          ?.filter((member) => member.clientId !== context.localClientId)
          .map((member) => (
            <UiButton variant="quiet"
              key={member.clientId}
              type="button"
              className="mp-quick-chat-mute-member"
              aria-pressed={state.muted.includes(member.clientId)}
              onClick={() => model.toggleMember(member.clientId)}
            >
              {`${t('multiplayer.spectators')} ${member.name} | ${t(state.muted.includes(member.clientId) ? 'chat.unmute' : 'chat.mute')}`}
            </UiButton>
          ))}
        {state.muteOpen && muteButton}
      </div>
      <div ref={log} className="mp-quick-chat-log" role="log" aria-live="polite">
        {state.entries.map((entry) =>
          createPortal(
            <RetainedRow
              entry={entry}
              label={label(entry.phrase)}
              carrier={rowCarrier}
              rows={rows.current}
              log={log}
              visible={!state.muted.includes(entry.clientId)}
            />,
            rowCarrier,
            entry.id,
          ),
        )}
      </div>
    </section>
  );
  return section;
}

/** Detached portal carrier is presentation storage only. React creates and
 * updates the paragraph; attach/detach preserves native fade and row identity.
 * Return the node to its actual React parent before reconciliation removes it. */
function RetainedRow({
  entry,
  label,
  carrier,
  rows,
  log,
  visible,
}: {
  entry: QuickChatEntry;
  label: string;
  carrier: HTMLElement;
  rows: Map<number, HTMLParagraphElement>;
  log: { current: HTMLDivElement | null };
  visible: boolean;
}) {
  const ref = useCallback(
    (row: HTMLParagraphElement | null) => {
      if (!row) return;
      rows.set(entry.id, row);
      if (visible && log.current) log.current.append(row);
      return () => {
        rows.delete(entry.id);
        if (row.parentElement !== carrier) carrier.append(row);
      };
    },
    [entry.id, carrier, rows, log, visible],
  );
  const {t} = useLocale();
  const sender = `${entry.spectator ? t('chat.spectator') : `P${entry.seat + 1}`}${entry.name ? `(${entry.name})` : ''}`;
  return (
    <p ref={ref} title={`${sender}: ${label}`}>
      <strong>{sender}</strong>
      {`: ${label}`}
    </p>
  );
}
