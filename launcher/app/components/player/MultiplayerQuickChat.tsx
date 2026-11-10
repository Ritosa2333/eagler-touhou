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
import type { DecisionStore } from '../../models/decisions';
import { animateUiFrames, type SurfaceAnimation } from '../../ui/surface-motion';
import { ChatAvatar, ChatComposer, ChatMessageContent, QuickChatPhrases, useChatReportConfirmation } from '../room/ChatControls';
import { shallowEqual, useStoreSelector } from '../../shared/react-store';

const chatPresentation = (state: ReturnType<MultiplayerQuickChatModel['getSnapshot']>) => ({
  context: state.context,
  entries: state.entries,
  muted: state.muted,
  pickerOpen: state.pickerOpen,
  editingText: state.editingText,
});
const isolate = (event: SyntheticEvent) => event.stopPropagation();

/** React owns the message list and its events. Effects only measure scrolling
 * and animate row movement/expiry. Muting hides a row without moving its DOM. */
export function MultiplayerQuickChat({ model, iconUrl, decisions }: {
  model: MultiplayerQuickChatModel;
  iconUrl: string;
  decisions: Pick<DecisionStore, 'askText'>;
}) {
  const { t } = useLocale();
  const root = useRef<HTMLElement>(null),
    picker = useRef<HTMLDivElement>(null),
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
  const playing = context?.localSeat != null && context.localSpectator !== true;
  const enabled = !!context?.connected && (playing || context.localSpectator === true);
  const confirmReport = useChatReportConfirmation(model, decisions);
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
    if (state.pickerOpen && context?.visible && !context.inRoom) picker.current?.focus({ preventScroll: true });
  }, [state.pickerOpen, context?.visible, context?.inRoom]);

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
      if (!state.entries.some((entry) => entry.id === id && !entry.expiredInGame)) {
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
      className="mp-quick-chat"
      data-player={playing || undefined}
      data-composer-open={state.pickerOpen}
      hidden={!context?.visible || context.inRoom}
      aria-label={t('chat.players')}
      onPointerDown={isolate}
      onMouseDown={isolate}
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
      <UiButton
        variant="quiet"
        type="button"
        className="mp-quick-chat-prompt text-inherit hover:text-inherit"
        aria-expanded={state.pickerOpen}
        aria-label={t('chat.title')}
        aria-keyshortcuts="Alt"
        disabled={!enabled}
        onClick={model.togglePicker}
        title={t('chat.keyboardHint')}
      >
        <img className="mp-quick-chat-icon" src={iconUrl} alt="" />
        {t('chat.title')}
        <kbd className="mp-quick-chat-shortcut">Alt</kbd>
      </UiButton>
      <div ref={picker} className="mp-quick-chat-picker" tabIndex={-1} hidden={!state.pickerOpen}>
        <ChatComposer model={model} gameplay />
        {!state.editingText && <QuickChatPhrases model={model} enabled={enabled} language={context?.language ?? ''} numbered onSelect={model.sendPhrase}/>}
      </div>
      <div ref={log} className="mp-quick-chat-log" role="log" aria-live="polite">
        {state.entries.map((entry) => (
          <ChatRow
            key={entry.id}
            entry={entry}
            model={model}
            confirmReport={confirmReport}
            label={label(entry.phrase)}
            rows={rows.current}
            visible={!entry.expiredInGame && !state.muted.includes(entry.clientId)}
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
  confirmReport,
}: {
  entry: QuickChatEntry;
  model: MultiplayerQuickChatModel;
  label: string;
  rows: Map<number, HTMLParagraphElement>;
  visible: boolean;
  confirmReport(entry: QuickChatEntry): void;
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
    <p ref={ref} hidden={!visible} title={entry.rating ? undefined : `${sender}: ${label}`} className="room-chat-entry" data-speaker={entry.rating ? 'rating' : entry.spectator?'spectator':'player'}>
      <ChatAvatar entry={entry} model={model}/>
      <ChatMessageContent entry={entry} model={model} label={label} confirmReport={confirmReport} directReport />
    </p>
  );
}
