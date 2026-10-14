import { useLayoutEffect, useRef, useSyncExternalStore } from 'react';
import { QUICK_CHAT_ROWS } from '../../../src/contracts/multiplayer-quick-chat.mts';
import type { MultiplayerQuickChatModel } from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import { useLocale } from '../../i18n';

/** Room presentation shares the session's message/timer owner with gameplay. */
export function RoomQuickChat({ model }: { model: MultiplayerQuickChatModel }) {
  const { locale, t } = useLocale();
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const log = useRef<HTMLDivElement>(null);
  useLayoutEffect(() => {
    if (log.current) log.current.scrollTop = log.current.scrollHeight;
  }, [state.entries]);
  const context = state.context;
  const enabled = context?.connected && (context.localSeat != null || context.localSpectator);
  return (
    <section id="mpRoomChat" className="mp-room-quick-chat" aria-label={t('chat.players')}>
      <div ref={log} className="mp-room-chat-log" role="log" aria-live="polite">
        {state.entries.filter(entry => !state.muted.includes(entry.clientId)).map(entry => (
          <p key={entry.id}>
            <strong>{`${entry.spectator ? (locale === 'en' ? 'Spectator' : '观众') : `P${entry.seat + 1}`}${entry.name ? `(${entry.name})` : ''}`}</strong>
            {`: ${locale === 'en' ? entry.phrase.en : entry.phrase.zh}`}
          </p>
        ))}
      </div>
      <details className="mp-room-chat-actions">
        <summary>{t('chat.prompt')}</summary>
        <div className="mp-room-chat-phrases">
          {QUICK_CHAT_ROWS.flat().map(phrase => (
            <button key={phrase.id} type="button" data-phrase={phrase.id}
              disabled={!enabled} onClick={() => model.sendPhrase(phrase.id)}>
              {locale === 'en' ? phrase.en : phrase.zh}
            </button>
          ))}
        </div>
      </details>
    </section>
  );
}
