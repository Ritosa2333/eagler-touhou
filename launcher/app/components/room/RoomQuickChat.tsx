import { useLayoutEffect, useRef } from 'react';
import { ChatComposer, ChatMessageContent } from './ChatControls';
import type { MultiplayerQuickChatModel } from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import { useLocale } from '../../i18n';
import { shallowEqual, useStoreSelector } from '../../shared/react-store';

const chatLog = (state: ReturnType<MultiplayerQuickChatModel['getSnapshot']>) => ({
  entries: state.entries,
  muted: state.muted,
});

/** Room presentation shares the session's message/timer owner with gameplay. */
export function RoomQuickChat({
  model,
  part = 'all',
}: {
  model: MultiplayerQuickChatModel;
  part?: 'all' | 'log' | 'actions';
}) {
  const { locale, t } = useLocale();
  const state = useStoreSelector(model, chatLog, shallowEqual);
  const log = useRef<HTMLDivElement>(null);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Scroll after React commits new message rows.
  useLayoutEffect(() => {
    if (log.current) log.current.scrollTop = log.current.scrollHeight;
  }, [state.entries]);
  const messages = (
    <div ref={log} className="mp-room-chat-log" role="log" aria-live="polite" aria-label={t('chat.players')}>
      {state.entries
        .filter((entry) => !state.muted.includes(entry.clientId))
        .map((entry) => (
          <p key={entry.id} className="room-chat-entry">
            <ChatMessageContent
              entry={entry}
              model={model}
              label={locale === 'en' ? entry.phrase.en : entry.phrase.zh}
            />
          </p>
        ))}
    </div>
  );
  if (part === 'log') return messages;
  return (
    <section id="mpRoomChat" className="mp-room-quick-chat" aria-label={t('chat.players')}>
      {part === 'all' && messages}
      <ChatComposer model={model} />
    </section>
  );
}
