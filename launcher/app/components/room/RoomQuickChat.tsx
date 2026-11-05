import { useLayoutEffect, useRef, useSyncExternalStore } from 'react';
import {ChevronUp} from 'lucide-react';
import {RoomActionMenu} from './RoomActionMenu';
import { QUICK_CHAT_ROWS } from '../../../src/contracts/multiplayer-quick-chat.mts';
import type { MultiplayerQuickChatModel } from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import { useLocale } from '../../i18n';

/** Room presentation shares the session's message/timer owner with gameplay. */
export function RoomQuickChat({ model, part = 'all' }: { model: MultiplayerQuickChatModel; part?: 'all' | 'log' | 'actions' }) {
  const { locale, t } = useLocale();
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const log = useRef<HTMLDivElement>(null);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Scroll after React commits new message rows.
  useLayoutEffect(() => {
    if (log.current) log.current.scrollTop = log.current.scrollHeight;
  }, [state.entries]);
  const context = state.context;
  const enabled = context?.connected && (context.localSeat != null || context.localSpectator);
  const messages = <div ref={log} className="mp-room-chat-log" role="log" aria-live="polite" aria-label={t('chat.players')}>
        {state.entries.filter(entry => !state.muted.includes(entry.clientId)).map(entry => (
          <p key={entry.id}>
            <strong>{`${entry.spectator ? t('chat.spectator') : `P${entry.seat + 1}`}${entry.name ? `(${entry.name})` : ''}`}</strong>
            {`: ${locale === 'en' ? entry.phrase.en : entry.phrase.zh}`}
          </p>
        ))}
      </div>;
  if (part === 'log') return messages;
  return (
    <section id="mpRoomChat" className="mp-room-quick-chat" aria-label={t('chat.players')}>
      {part === 'all' && messages}
      <RoomActionMenu enabled={!!enabled} label={t('room.quickChat')} triggerContent={<>{t('room.quickChat')}<ChevronUp className="room-quick-chat-chevron" aria-hidden="true"/></>}
        triggerKind="quick-chat" menuClassName="room-quick-chat-picker"
        items={QUICK_CHAT_ROWS.flat().map(phrase=>({id:phrase.id,label:locale==='en'?phrase.en:phrase.zh,select:()=>model.sendPhrase(phrase.id)}))}/>
    </section>
  );
}
