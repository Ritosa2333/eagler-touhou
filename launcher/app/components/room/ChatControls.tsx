import { memo, useCallback, useState, useSyncExternalStore } from 'react';
import { ArrowLeft, ChevronLeft, ChevronRight, ChevronUp, Ellipsis, Flag, Send, Settings, VolumeX, Zap } from 'lucide-react';
import { Button } from '../../ui/button';
import { Button as UiButton } from '../../ui/controls';
import { InputGroup, InputGroupInput } from '../../ui/input-group';
import { ActionMenu, ActionMenuCheckboxItem, ActionMenuItem } from '../../ui/action-menu';
import { QUICK_CHAT_PHRASES, quickChatPhrase } from '../../../src/contracts/multiplayer-quick-chat.mts';
import {
  type MultiplayerQuickChatModel,
  type QuickChatContext,
  type QuickChatEntry,
} from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import type { DecisionStore } from '../../models/decisions';
import { useLocale } from '../../i18n';
import { shallowEqual, useStoreSelector } from '../../shared/react-store';
import './chat-controls.css';
import {ParticipantAvatar} from './ParticipantAvatar';

export function ChatComposer({ model, gameplay = false, openOnClick = false }: {
  model: MultiplayerQuickChatModel;
  gameplay?: boolean;
  openOnClick?: boolean;
}) {
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
        : t(gameplay ? 'chat.freeInputHint' : 'chat.message');
  const quick = !gameplay && <RoomQuickPicker model={model} enabled={enabled} openOnClick={openOnClick} registered={state.registered} locale={locale} />;
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
      <ChatMuteMenu model={model} compact={gameplay} openOnClick={openOnClick} />
    </div>
  );
}

function RoomQuickPicker({model, enabled, openOnClick, registered, locale}: {
  model: MultiplayerQuickChatModel; enabled: boolean; openOnClick: boolean; registered: boolean; locale: string;
}) {
  const {t} = useLocale();
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const [view, setView] = useState<'favorites' | 'all' | 'slots' | 'choose'>('favorites');
  const [slot, setSlot] = useState(0);
  const [page, setPage] = useState(0);
  const zh = locale === 'zh-CN', ja = locale === 'ja';
  const copy = {
    favorites: zh ? '常用' : ja ? 'お気に入り' : 'Favorites',
    all: zh ? '全部' : ja ? 'すべて' : 'All',
    settings: zh ? '设置常用发言' : ja ? 'お気に入りを設定' : 'Edit favorites',
    back: zh ? '返回' : ja ? '戻る' : 'Back',
  };
  const text = (phrase: typeof QUICK_CHAT_PHRASES[number]) => locale === 'en' ? phrase.en : phrase.zh;
  const ids = view === 'favorites' || view === 'slots' ? state.slots : QUICK_CHAT_PHRASES.slice(page * 9, page * 9 + 9).map(phrase => phrase.id);
  const pageCount = Math.ceil(QUICK_CHAT_PHRASES.length / 9);
  const switchView = (next: typeof view) => {setView(next); setPage(0);};
  return <ActionMenu label={t('room.quickChat')} openOnClick={openOnClick} side="top" align="start" minimumWidth={260}
    className="room-action-menu room-quick-chat-picker quick-picker-menu"
    trigger={<Button variant={registered ? 'outline' : 'secondary'} size={registered ? 'default' : 'lg'}
      className={registered ? undefined : 'room-quick-chat-trigger h-12 w-full justify-between gap-3 data-[state=open]:bg-accent'}
      title={t('room.quickChat')} aria-label={t('room.quickChat')} disabled={!enabled}>
      {registered ? <Zap aria-hidden="true"/> : <>{t('room.quickChat')}<ChevronUp className="room-quick-chat-chevron" aria-hidden="true"/></>}
    </Button>}>
    <div className="quick-picker-toolbar">
      {(view === 'slots' || view === 'choose') ? <Button type="button" size="icon-sm" variant="ghost" aria-label={copy.back} title={copy.back} onClick={() => switchView(view === 'choose' ? 'slots' : 'favorites')}><ArrowLeft/></Button> : <>
        <Button type="button" size="sm" variant={view === 'favorites' ? 'secondary' : 'ghost'} onClick={() => switchView('favorites')}>{copy.favorites}</Button>
        <Button type="button" size="sm" variant={view === 'all' ? 'secondary' : 'ghost'} onClick={() => switchView('all')}>{copy.all}</Button>
        <Button type="button" size="icon-sm" variant="ghost" className="quick-picker-settings" aria-label={copy.settings} title={copy.settings} onClick={() => switchView('slots')}><Settings/></Button>
      </>}
      {(view === 'slots' || view === 'choose') && <span>{view === 'choose' ? `${copy.settings} · ${slot + 1}` : copy.settings}</span>}
    </div>
    {ids.map((id, index) => {
      const phrase = quickChatPhrase(id);
      if (!phrase) return null;
      return <ActionMenuItem key={`${id}-${index}`} data-phrase={id} onSelect={event => {
        if (view === 'slots') {event.preventDefault(); setSlot(index); switchView('choose');}
        else if (view === 'choose') {event.preventDefault(); model.setSlot(slot, id); switchView('slots');}
      }} onAction={view === 'favorites' || view === 'all' ? () => {model.sendPhrase(id); switchView('favorites');} : undefined}>
        {(view === 'favorites' || view === 'slots') && <kbd>{index + 1}</kbd>}{text(phrase)}
      </ActionMenuItem>;
    })}
    {(view === 'all' || view === 'choose') && <div className="quick-picker-pages">
      <Button type="button" variant="ghost" size="icon-sm" aria-label={zh ? '上一页' : 'Previous page'} disabled={!page} onClick={() => setPage(page - 1)}><ChevronLeft/></Button>
      <span>{page + 1} / {pageCount}</span>
      <Button type="button" variant="ghost" size="icon-sm" aria-label={zh ? '下一页' : 'Next page'} disabled={page + 1 >= pageCount} onClick={() => setPage(page + 1)}><ChevronRight/></Button>
    </div>}
  </ActionMenu>;
}

export function QuickChatPhrases({ enabled, language, numbered, onSelect, model }: {
  enabled: boolean; language: string; numbered?: boolean; onSelect(phraseId: string): void; model: MultiplayerQuickChatModel;
}) {
  const { t } = useLocale();
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const page = state.pickerPage;
  const ids = page === 0 ? state.slots : QUICK_CHAT_PHRASES.slice((page - 1) * 9, page * 9).map(phrase => phrase.id);
  const maxPage = Math.ceil(QUICK_CHAT_PHRASES.length / 9);
  return <div className="quick-picker-game">
    <div className="quick-picker-toolbar">
      <Button type="button" size="sm" variant={page === 0 ? 'secondary' : 'ghost'} onClick={() => model.setPickerPage(0)}>{language === 'en' ? 'Favorites' : '常用'}</Button>
      <Button type="button" size="sm" variant={page !== 0 ? 'secondary' : 'ghost'} onClick={() => model.setPickerPage(1)}>{language === 'en' ? 'All' : '全部'}</Button>
      {page > 0 && <div className="quick-picker-pages"><Button type="button" size="icon-sm" variant="ghost" disabled={page === 1} aria-label={language === 'en' ? 'Previous page' : '上一页'} onClick={() => model.setPickerPage(page - 1)}><ChevronLeft/></Button><span>{page} / {maxPage}</span><Button type="button" size="icon-sm" variant="ghost" disabled={page === maxPage} aria-label={language === 'en' ? 'Next page' : '下一页'} onClick={() => model.setPickerPage(page + 1)}><ChevronRight/></Button></div>}
    </div>
    <fieldset className={`chat-phrase-grid m-0 min-w-0 border-0 p-0${numbered ? ' mp-quick-chat-slots' : ''}`} aria-label={t('room.quickChat')}>
      {ids.map((id, index) => {
        const phrase = quickChatPhrase(id);
        if (!phrase) return null;
        const key = numbered ? String(index + 1) : undefined;
        const text = language === 'en' ? phrase.en : phrase.zh;
        return <Button key={`${id}-${index}`} type="button" variant="outline" size="row"
          className="min-h-9 min-w-0 items-center gap-1.5 py-1.5 pointer-coarse:min-h-10 max-[780px]:min-h-10"
          disabled={!enabled} data-phrase={id} title={text} aria-label={text} aria-keyshortcuts={key} onClick={() => onSelect(id)}>
          {key && <kbd>{key}</kbd>}<span className="min-w-0 truncate">{text}</span>
        </Button>;
      })}
    </fieldset>
  </div>;
}

export function useChatReportConfirmation(model: MultiplayerQuickChatModel, decisions?: Pick<DecisionStore, 'askText'>) {
  const { t, locale } = useLocale();
  return useCallback(async (entry: QuickChatEntry) => {
    if (!entry.messageId || !decisions) return;
    const reason = await decisions.askText({
      title: t('chat.report'),
      message: entry.phrase.zh,
      confirmText: t('chat.report'),
      confirmOnEnter: false,
      input: {label: locale === 'zh-CN' ? '举报理由' : locale === 'ja' ? '通報理由' : 'Report reason', defaultValue:'', validate:value => !value.trim() || value.trim().length > 500 ? (locale === 'zh-CN' ? '请填写 1 至 500 个字符的理由' : locale === 'ja' ? '理由を1～500文字で入力してください' : 'Enter a reason of 1 to 500 characters') : null},
    });
    if (reason && model.getSnapshot().entries.includes(entry)) model.reportMessage(entry.messageId, reason);
  }, [decisions, model, t, locale]);
}

export function ChatMessageText({ entry, label }: {entry: QuickChatEntry; label: string}) {
  const {t} = useLocale();
  const badge = entry.spectator ? t('chat.spectator') : `P${entry.seat + 1}`;
  return <><span className="chat-sender"><strong>{entry.name || badge}</strong><span className="chat-role">{badge}</span></span><span className="chat-body">{label}</span></>;
}

export function ChatAvatar({entry, model}: {entry:QuickChatEntry; model:MultiplayerQuickChatModel}) {
  const context = useStoreSelector(model, state=>state.context);
  const member = [...(context?.seats ?? []), ...(context?.spectators ?? [])].find(item=>item?.clientId===entry.clientId);
  const initial = Array.from(entry.name.trim())[0] || '?';
  return <ParticipantAvatar className="chat-avatar" name={entry.name} profile={member?.profile}>{initial}</ParticipantAvatar>;
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
const ChatMuteMenu = memo(function ChatMuteMenu({ model, compact, openOnClick }: { model: MultiplayerQuickChatModel; compact: boolean; openOnClick: boolean }) {
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
      openOnClick={openOnClick}
      side="top"
      className="room-chat-mute-menu"
      minimumWidth={220}
      variant="selection"
      trigger={
        <Button
          variant="outline"
          size={state.registered ? 'default' : compact ? 'icon-sm' : 'icon'}
          className={state.registered ? 'room-chat-mute-trigger px-3' : compact ? 'room-chat-mute-trigger' : 'room-chat-mute-trigger size-12'}
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
  canReport: !!state.context?.connected && (state.context.localSeat != null || state.context.localSpectator === true),
  authorId: state.authorId,
  localClientId: localClientId(state.context),
  muted: state.muted,
});
export const ChatMessageContent = memo(function ChatMessageContent({
  entry,
  model,
  label,
  confirmReport,
  directReport = false,
}: {
  entry: QuickChatEntry;
  model: MultiplayerQuickChatModel;
  label: string;
  confirmReport?(entry: QuickChatEntry): void;
  directReport?: boolean;
}) {
  const { t } = useLocale(),
    state = useStoreSelector(model, chatIdentity, shallowEqual);
  const own = entry.clientId === state.localClientId || (!!entry.authorId && entry.authorId === state.authorId),
    canReport = !!entry.messageId && !!entry.authorId && state.canReport && !own;
  const content = <ChatMessageText entry={entry} label={label}/>;
  if (directReport && confirmReport) return canReport ? (
    <UiButton variant="unstyled" className="room-chat-message chat-report-trigger"
      aria-haspopup="dialog" title={t('chat.report')} onClick={() => confirmReport(entry)}>
      {content}
    </UiButton>
  ) : <span className="room-chat-message">{content}</span>;
  return (
    <>
      <span className="room-chat-message" data-actions={!own || undefined}>
        {content}
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
          {canReport && confirmReport && (
            <ActionMenuItem onAction={() => confirmReport(entry)}>
              <Flag aria-hidden="true" />
              {t('chat.report')}
            </ActionMenuItem>
          )}
        </ActionMenu>
      )}
    </>
  );
});
