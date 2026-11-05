import { memo, useSyncExternalStore } from 'react';
import { ChevronUp, Ellipsis, Flag, Send, VolumeX, Zap } from 'lucide-react';
import { Button } from '../../ui/button';
import { InputGroup, InputGroupInput } from '../../ui/input-group';
import { ActionMenu, ActionMenuCheckboxItem, ActionMenuItem } from '../../ui/action-menu';
import { RoomActionMenu } from './RoomActionMenu';
import { QUICK_CHAT_ROWS } from '../../../src/contracts/multiplayer-quick-chat.mts';
import type {
  MultiplayerQuickChatModel,
  QuickChatContext,
  QuickChatEntry,
} from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import { useLocale } from '../../i18n';
import { shallowEqual, useStoreSelector } from '../../shared/react-store';
import './chat-controls.css';

export function ChatComposer({ model }: { model: MultiplayerQuickChatModel }) {
  const { t, locale } = useLocale();
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot),
    context = state.context;
  const enabled = !!context?.connected && (context.localSeat != null || context.localSpectator === true);
  const banned = state.bannedUntil === -1 || state.bannedUntil > Date.now();
  const placeholder =
    state.bannedUntil === -1
      ? t('chat.permanentlyBanned')
      : banned
        ? t('chat.bannedUntil', {
            time: new Intl.DateTimeFormat(locale, {
              hour: '2-digit',
              minute: '2-digit',
              month: 'numeric',
              day: 'numeric',
            }).format(state.bannedUntil),
          })
        : t('chat.message');
  const quick = (
    <RoomActionMenu
      enabled={enabled}
      label={t('room.quickChat')}
      triggerContent={
        state.registered ? (
          <Zap aria-hidden="true" />
        ) : (
          <>
            {t('room.quickChat')}
            <ChevronUp className="room-quick-chat-chevron" aria-hidden="true" />
          </>
        )
      }
      triggerKind={state.registered ? 'action' : 'quick-chat'}
      menuClassName="room-quick-chat-picker"
      items={QUICK_CHAT_ROWS.flat().map((phrase) => ({
        id: phrase.id,
        label: locale === 'en' ? phrase.en : phrase.zh,
        select: () => model.sendPhrase(phrase.id),
      }))}
    />
  );
  return (
    <div className="room-chat-controls" data-member={state.registered}>
      {state.registered && (
        <form
          onSubmit={(event) => {
            event.preventDefault();
            model.sendText();
          }}
        >
          <InputGroup>
            <InputGroupInput
              aria-label={t('chat.message')}
              placeholder={placeholder}
              maxLength={500}
              value={state.draft}
              disabled={!enabled || banned}
              onChange={(event) => model.setDraft(event.target.value)}
              onKeyDown={(event) => {
                if (event.key === 'Enter' && event.nativeEvent.isComposing) event.preventDefault();
              }}
            />
            <Button
              type="submit"
              variant="ghost"
              size="icon-sm"
              aria-label={t('chat.send')}
              disabled={!enabled || banned || state.sending || !state.draft.trim()}
            >
              <Send aria-hidden="true" />
            </Button>
          </InputGroup>
        </form>
      )}
      {quick}
      <ChatMuteMenu model={model} />
    </div>
  );
}

function localClientId(context: QuickChatContext | null) {
  return (
    context?.localClientId ?? (context?.localSeat != null ? context.seats[context.localSeat]?.clientId : undefined)
  );
}
const chatMembers = (state: ReturnType<MultiplayerQuickChatModel['getSnapshot']>) => ({
  context: state.context,
  entries: state.entries,
  muted: state.muted,
  registered: state.registered,
  authorId: state.authorId,
});
const ChatMuteMenu = memo(function ChatMuteMenu({ model }: { model: MultiplayerQuickChatModel }) {
  const { t } = useLocale(),
    state = useStoreSelector(model, chatMembers, shallowEqual),
    context = state.context,
    self = localClientId(context),
    members = new Map<string, string>();
  context?.seats.forEach((seat, index) => {
    if (seat && seat.clientId !== self) members.set(seat.clientId, `P${index + 1} ${seat.name}`);
  });
  for (const member of context?.spectators ?? [])
    if (member.clientId !== self) members.set(member.clientId, `${t('chat.spectator')} ${member.name}`);
  for (const entry of state.entries)
    if (entry.clientId !== self && entry.authorId !== state.authorId && !members.has(entry.clientId))
      members.set(entry.clientId, `${entry.spectator ? t('chat.spectator') : `P${entry.seat + 1}`} ${entry.name}`);
  return (
    <ActionMenu
      label={t('chat.mute')}
      side="top"
      className="room-chat-mute-menu"
      minimumWidth={220}
      variant="selection"
      trigger={
        <Button
          variant="outline"
          size={state.registered ? 'default' : 'icon'}
          className={state.registered ? 'room-chat-mute-trigger px-3' : 'room-chat-mute-trigger size-12'}
          aria-label={t('chat.mute')}
          title={t('chat.mute')}
          disabled={!members.size}
        >
          <VolumeX aria-hidden="true" />
        </Button>
      }
    >
      {[...members].map(([clientId, label]) => (
        <ActionMenuCheckboxItem
          key={clientId}
          checked={state.muted.includes(clientId)}
          onCheckedChange={() => model.toggleMember(clientId)}
          onSelect={(event) => event.preventDefault()}
        >
          {label}
        </ActionMenuCheckboxItem>
      ))}
    </ActionMenu>
  );
});

const chatIdentity = (state: ReturnType<MultiplayerQuickChatModel['getSnapshot']>) => ({
  registered: state.registered,
  authorId: state.authorId,
  localClientId: localClientId(state.context),
  muted: state.muted,
});
export const ChatMessageContent = memo(function ChatMessageContent({
  entry,
  model,
  label,
}: {
  entry: QuickChatEntry;
  model: MultiplayerQuickChatModel;
  label: string;
}) {
  const { t } = useLocale(),
    state = useStoreSelector(model, chatIdentity, shallowEqual);
  const sender = `${entry.spectator ? t('chat.spectator') : `P${entry.seat + 1}`}${entry.name ? `(${entry.name})` : ''}`;
  const own = entry.clientId === state.localClientId || (!!entry.authorId && entry.authorId === state.authorId),
    canReport = !!entry.messageId && state.registered && !own;
  return (
    <>
      <span className="room-chat-message" data-actions={!own || undefined}>
        <strong>{sender}</strong>
        {`: ${label}`}
      </span>
      {!own && (
        <ActionMenu
          label={t('chat.messageActions')}
          className="room-chat-message-menu"
          minimumWidth={140}
          trigger={
            <Button
              type="button"
              variant="ghost"
              size="icon-sm"
              className="room-chat-message-action text-inherit hover:text-inherit"
              aria-label={t('chat.messageActions')}
            >
              <Ellipsis aria-hidden="true" />
            </Button>
          }
        >
          <ActionMenuItem onAction={() => model.toggleMember(entry.clientId)}>
            <VolumeX aria-hidden="true" />
            {t(state.muted.includes(entry.clientId) ? 'chat.unmute' : 'chat.mute')}
          </ActionMenuItem>
          {canReport && (
            <ActionMenuItem onAction={() => model.reportMessage(entry.messageId!)}>
              <Flag aria-hidden="true" />
              {t('chat.report')}
            </ActionMenuItem>
          )}
        </ActionMenu>
      )}
    </>
  );
});
