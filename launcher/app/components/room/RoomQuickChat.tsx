import { useLayoutEffect, useRef, useState } from 'react';
import { ChevronsDown } from 'lucide-react';
import { Slider } from '../../ui/slider';
import { Button } from '../../ui/button';
import { ChatAvatar, ChatComposer, ChatMessageContent, useChatReportConfirmation } from './ChatControls';
import type { MultiplayerQuickChatModel } from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import type { DecisionStore } from '../../models/decisions';
import { useLocale } from '../../i18n';
import { shallowEqual, useStoreSelector } from '../../shared/react-store';

const chatLog = (state: ReturnType<MultiplayerQuickChatModel['getSnapshot']>) => ({
  entries: state.entries,
  muted: state.muted,
});

function RoomChatLog({model, mobile, decisions}: {
  model: MultiplayerQuickChatModel;
  mobile: boolean;
  decisions?: Pick<DecisionStore, 'askText'>;
}) {
  const {locale, t} = useLocale();
  const {entries, muted} = useStoreSelector(model, chatLog, shallowEqual);
  const log = useRef<HTMLDivElement>(null);
  const rows = useRef<HTMLDivElement>(null);
  const position = useRef({max: 0, offset: 0});
  const [scroll, setScroll] = useState(position.current);
  const [unread, setUnread] = useState(0);
  const lastEntry = useRef<number | null>(null);
  const pointerStart = useRef<number | null>(null);
  const [dragging, setDragging] = useState(false);
  const confirmReport = useChatReportConfirmation(model, decisions);
  function readScroll(element: HTMLDivElement) {
    const max = Math.max(0, element.scrollHeight - element.clientHeight);
    const offset = Math.min(max, Math.max(0, Math.round(element.scrollTop)));
    if (max - offset <= 2) setUnread(0);
    if (max === position.current.max && offset === position.current.offset) return;
    position.current = {max, offset};
    setScroll(position.current);
  }
  // biome-ignore lint/correctness/useExhaustiveDependencies: New or newly unmuted messages change the measured height.
  useLayoutEffect(() => {
    const element = log.current;
    if (!element) return;
    const previous = lastEntry.current;
    const previousIndex = previous === null ? -1 : entries.findIndex(entry => entry.id === previous);
    if (previous !== null && previousIndex >= 0 && position.current.max - position.current.offset > 2) {
      const count = entries.slice(previousIndex + 1).filter(entry => !muted.includes(entry.clientId)).length;
      if (count) setUnread(value => value + count);
    } else if (previous !== null && previousIndex < 0) setUnread(0);
    lastEntry.current = entries.at(-1)?.id ?? null;
    const update = () => {
      if (!element.clientHeight) return;
      const {max, offset} = position.current;
      element.scrollTop = max - offset <= 2 ? element.scrollHeight : offset;
      readScroll(element);
    };
    update();
    if (typeof ResizeObserver === 'undefined') return;
    const observer = new ResizeObserver(update);
    observer.observe(element);
    if (rows.current) observer.observe(rows.current);
    return () => observer.disconnect();
  }, [entries, muted, mobile]);
  const messages = entries.filter(entry => !muted.includes(entry.clientId)).map(entry => (
    <p key={entry.id} className="room-chat-entry" data-speaker={entry.rating ? 'rating' : entry.spectator?'spectator':'player'}>
      <ChatAvatar entry={entry} model={model}/>
      <ChatMessageContent entry={entry} model={model} label={locale === 'en' ? entry.phrase.en : entry.phrase.zh}
        confirmReport={decisions ? confirmReport : undefined} directReport={mobile}/>
    </p>
  ));
  const unreadLabel = t('chat.newMessages', {count:unread});
  const unreadButton = unread > 0 && <Button type="button" variant="default" className="room-chat-unread" aria-label={unreadLabel} title={unreadLabel} onClick={() => {
    if (!log.current) return;
    log.current.scrollTop = log.current.scrollHeight;
    readScroll(log.current);
    setUnread(0);
  }}><span>{unread}</span><ChevronsDown aria-hidden="true"/></Button>;
  if (!mobile) return <div className="room-chat-desktop-history">
    <div className="mp-room-chat-log" ref={log} role="log" aria-live="polite" aria-label={t('chat.players')}
      onScroll={event => {if (event.currentTarget.clientHeight) readScroll(event.currentTarget);}}>{messages}</div>
    <Slider orientation="vertical" className="room-chat-history-slider" disabled={!scroll.max} data-dragging={dragging || undefined}
      onPointerDownCapture={event => {pointerStart.current=event.clientY;}}
      onPointerMove={event => {if(pointerStart.current !== null && Math.abs(event.clientY-pointerStart.current)>3)setDragging(true);}}
      onPointerUp={() => {pointerStart.current=null;setDragging(false);}}
      onPointerCancelCapture={() => {pointerStart.current=null;setDragging(false);}}
      onLostPointerCapture={() => {pointerStart.current=null;setDragging(false);}}
      aria-label={t('chat.historyScroll')} value={scroll.max - scroll.offset} max={Math.max(1, scroll.max)}
      onValueChange={value => {
        if (!log.current) return;
        log.current.scrollTop = scroll.max - value;
        readScroll(log.current);
      }}/>
    {unreadButton}
  </div>;

  return <div className="room-chat-history">
    <div className="room-chat-history-viewport" ref={log} role="log" aria-live="polite" aria-label={t('chat.players')}
      onScroll={event => {if (event.currentTarget.clientHeight) readScroll(event.currentTarget);}}>
      <div className="room-chat-history-rows" ref={rows}>
        {messages.length ? messages : <p className="room-chat-history-empty">{t('chat.noMessages')}</p>}
      </div>
    </div>
    <Slider orientation="vertical" inverted className="room-chat-history-slider" disabled={!scroll.max}
      aria-label={t('chat.historyScroll')} value={scroll.offset} max={Math.max(1, scroll.max)}
      onValueChange={offset => {
        if (!log.current) return;
        log.current.scrollTop = offset;
        readScroll(log.current);
      }}/>
    {unreadButton}
  </div>;
}

/** Room presentation shares the session's message and timer owner with gameplay. */
export function RoomQuickChat({model, part = 'all', mobile = false, decisions}: {
  model: MultiplayerQuickChatModel;
  part?: 'all' | 'log' | 'actions';
  mobile?: boolean;
  decisions?: Pick<DecisionStore, 'askText'>;
}) {
  const {t} = useLocale();
  const log = <RoomChatLog model={model} mobile={mobile} decisions={decisions}/>;
  if (part === 'log') return log;
  return (
    <section id="mpRoomChat" className="mp-room-quick-chat" aria-label={t('chat.players')}>
      {part === 'all' && log}
      <ChatComposer model={model} openOnClick={mobile}/>
    </section>
  );
}
