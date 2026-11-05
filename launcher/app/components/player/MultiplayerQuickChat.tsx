import { Button as UiButton } from '../../ui/controls';
import {
  useCallback,
  useLayoutEffect,
  useMemo,
  useRef,
  type SyntheticEvent,
  type KeyboardEvent as ReactKeyboardEvent,
} from 'react';
import type { QuickChatPhrase } from '../../../src/contracts/multiplayer-quick-chat.mts';
import type { MultiplayerQuickChatModel, QuickChatEntry } from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import { useLocale } from '../../i18n';
import { animateUiFrames, type SurfaceAnimation } from '../../ui/surface-motion';
import { ChatComposer, ChatMessageContent } from '../room/ChatControls';
import { shallowEqual, useStoreSelector } from '../../shared/react-store';

const chatPresentation = (state: ReturnType<MultiplayerQuickChatModel['getSnapshot']>) => ({
  context: state.context,
  entries: state.entries,
  muted: state.muted,
  pickerOpen: state.pickerOpen,
  registered: state.registered,
});
const isolate = (event: SyntheticEvent) => event.stopPropagation();
function isolatePointer(event: SyntheticEvent) {
  event.stopPropagation();
  if (
    event.target instanceof Element &&
    event.target.closest('input,textarea,[contenteditable="true"],[aria-haspopup="menu"],[role="menu"]')
  )
    return;
  event.preventDefault();
}

/** React owns the message list and its events. Effects only measure scrolling
 * and animate row movement/expiry. Muting hides a row without moving its DOM. */
export function MultiplayerQuickChat({ model }: { model: MultiplayerQuickChatModel }) {
  const { t } = useLocale();
  const root = useRef<HTMLElement>(null),
    log = useRef<HTMLDivElement>(null);
  const rows = useRef(new Map<number, HTMLParagraphElement>());
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
          .filter((row) => row.parentElement === element && !row.hidden)
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
  const presentation = useMemo(
    () => ({
      getSnapshot: model.getSnapshot,
      subscribe(notify: () => void) {
        let previous = chatPresentation(model.getSnapshot());
        return model.subscribe(() => {
          const next = chatPresentation(model.getSnapshot());
          if (shallowEqual(previous, next)) return;
          previous = next;
          capture();
          notify();
        });
      },
    }),
    [model, capture],
  );
  const state = useStoreSelector(presentation, chatPresentation, shallowEqual);
  const context = state.context;
  const th09 = context?.room.startsWith('th09mp-') === true;
  const returnToGame = () => {
    model.dismiss();
    const frame = document.getElementById('gameFrame') as HTMLIFrameElement | null;
    if (frame) frame.focus({ preventScroll: true });
    else (document.activeElement as HTMLElement | null)?.blur();
  };
  const isolateKey = (event: ReactKeyboardEvent) => {
    event.stopPropagation();
    if (event.nativeEvent.isComposing) {
      if (event.key === 'Enter') event.preventDefault();
      return;
    }
    if (event.key === 'Escape' && !event.defaultPrevented) {
      event.preventDefault();
      returnToGame();
    }
  };
  useLayoutEffect(() => {
    if (!state.registered || !state.pickerOpen || !context?.visible || context.inRoom) return;
    root.current
      ?.querySelector<HTMLElement>('input:not(:disabled),button[aria-haspopup="menu"]')
      ?.focus({ preventScroll: true });
  }, [state.registered, state.pickerOpen, context?.visible, context?.inRoom]);

  const isReduced = useCallback(() => {
    return !!(model.getSnapshot().context?.lessMotion || matchMedia('(prefers-reduced-motion: reduce)').matches);
  }, [model]);
  useLayoutEffect(() => {
    const element = root.current;
    if (!element || context?.inRoom) return;
    const detach = model.setExpiryPresenter((entry: QuickChatEntry) => {
      const row = rows.current.get(entry.id);
      if (isReduced() || !model.getSnapshot().context?.visible || row?.parentElement !== log.current || row.hidden)
        return;
      const animation = animateUiFrames(row, { opacity: ['1', '0'] });
      if (!animation) return;
      fades.current.set(entry.id, animation);
      return animation.finished.then(() => {});
    });
    return () => {
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
    element.scrollTop = !previous || previous.following ? element.scrollHeight : previous.top;
    if (isReduced()) {
      for (const animation of fades.current.values()) animation.complete();
      return;
    }
    for (const entry of state.entries) {
      const row = rows.current.get(entry.id);
      if (!row || row.hidden || row.parentElement !== element) continue;
      const position = previous?.positions.get(row),
        delta = position ? position.top - row.getBoundingClientRect().top : 6;
      const fading = fades.current.has(entry.id),
        opacity = position?.opacity ?? 0;
      if (Math.abs(delta) < 0.5 && (fading || opacity >= 0.999)) continue;
      const animation = animateUiFrames(row, {
        translate: [`0 ${delta}px`, '0 0'],
        ...(!fading ? { opacity: [String(opacity), '1'] } : {}),
      });
      if (animation) moves.current.set(row, animation);
    }
  }, [state, isReduced]);
  const label = (phrase: QuickChatPhrase) => (context?.language === 'en' ? phrase.en : phrase.zh);
  const section = (
    <section
      ref={root}
      className={`mp-quick-chat${th09 ? ' th09-player-chat' : ''}`}
      data-member={state.registered}
      data-composer-open={state.registered && state.pickerOpen}
      hidden={!context?.visible || context.inRoom}
      aria-label={t('chat.players')}
      onPointerDown={isolatePointer}
      onMouseDown={isolatePointer}
      onPointerMove={isolate}
      onPointerUp={isolate}
      onPointerCancel={isolate}
      onTouchStart={isolate}
      onTouchMove={isolate}
      onTouchEnd={isolate}
      onTouchCancel={isolate}
      onKeyDown={isolateKey}
      onKeyUp={isolate}
    >
      {state.registered ? (
        <>
          <UiButton
            variant="quiet"
            type="button"
            className="mp-quick-chat-prompt text-inherit hover:text-inherit"
            aria-expanded={state.pickerOpen}
            disabled={!context?.connected}
            onClick={model.togglePicker}
            title={t('chat.keyboardHint')}
          >
            <img className="th09-chat-icon" src={new URL('assets/player-chat.svg', document.baseURI).href} alt="" />
            {t('chat.title')}
            <kbd className="mp-quick-chat-shortcut" aria-hidden="true">
              Alt
            </kbd>
          </UiButton>
          <div className="mp-quick-chat-picker" hidden={!state.pickerOpen}>
            <ChatComposer model={model} />
          </div>
        </>
      ) : (
        <ChatComposer model={model} />
      )}
      <div ref={log} className="mp-quick-chat-log" role="log" aria-live="polite">
        {state.entries.map((entry) => (
          <ChatRow
            key={entry.id}
            entry={entry}
            model={model}
            label={label(entry.phrase)}
            rows={rows.current}
            visible={!state.muted.includes(entry.clientId)}
          />
        ))}
      </div>
    </section>
  );
  return section;
}

function ChatRow({
  entry,
  model,
  label,
  rows,
  visible,
}: {
  entry: QuickChatEntry;
  model: MultiplayerQuickChatModel;
  label: string;
  rows: Map<number, HTMLParagraphElement>;
  visible: boolean;
}) {
  const ref = useCallback(
    (row: HTMLParagraphElement | null) => {
      if (!row) return;
      rows.set(entry.id, row);
      return () => {
        rows.delete(entry.id);
      };
    },
    [entry.id, rows],
  );
  const { t } = useLocale();
  const sender = `${entry.spectator ? t('chat.spectator') : `P${entry.seat + 1}`}${entry.name ? `(${entry.name})` : ''}`;
  return (
    <p ref={ref} hidden={!visible} title={`${sender}: ${label}`} className="room-chat-entry">
      <ChatMessageContent entry={entry} model={model} label={label} />
    </p>
  );
}
