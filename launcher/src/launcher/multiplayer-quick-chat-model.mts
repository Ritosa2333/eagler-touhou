import { QUICK_CHAT_PHRASES, quickChatPhrase, type QuickChatPhrase } from '../contracts/multiplayer-quick-chat.mjs';
import type {RoomAccountProfile} from './room-account-profile.mjs';

export const QUICK_CHAT_SLOT_COUNT = 9;
const QUICK_CHAT_SLOTS_KEY = 'eagler.quick-chat.slots.v1';
const defaultSlots = () => QUICK_CHAT_PHRASES.slice(0, QUICK_CHAT_SLOT_COUNT).map(phrase => phrase.id);
function loadSlots(): string[] {
  try {
    const value: unknown = JSON.parse(globalThis.localStorage?.getItem(QUICK_CHAT_SLOTS_KEY) ?? 'null');
    if (Array.isArray(value) && value.length === QUICK_CHAT_SLOT_COUNT && value.every(id => quickChatPhrase(id))) return value;
  } catch { /* Storage can be unavailable or contain an obsolete value. */ }
  return defaultSlots();
}

export interface QuickChatSeat {
  clientId: string;
  name: string;
  profile?: RoomAccountProfile;
}
export interface QuickChatContext {
  visible: boolean;
  room: string;
  serial: number;
  localSeat: number | null;
  seats: readonly (QuickChatSeat | null)[];
  spectators?: readonly QuickChatSeat[];
  localSpectator?: boolean;
  localClientId?: string;
  inRoom?: boolean;
  connected: boolean;
  language: string;
  lessMotion?: boolean;
}
export interface QuickChatEntry {
  readonly id: number;
  readonly clientId: string;
  readonly seat: number;
  readonly spectator: boolean;
  readonly name: string;
  readonly phrase: QuickChatPhrase;
  readonly messageId?: number;
  readonly authorId?: string;
  readonly at?: number;
  readonly rating?: RatingAnnouncement;
  readonly expiredInGame?: boolean;
  readonly delivery?: 'pending' | 'failed';
}
export interface RatingAnnouncement {
  id: number;
  author: string;
  authorProfile?: {name: string; avatar?: string};
  settledAt: number;
  changes: {username: string; before: number | null; after: number}[];
}
function ratingAnnouncement(value: unknown): RatingAnnouncement | undefined {
  if (!value || typeof value !== 'object') return;
  const item = value as Partial<RatingAnnouncement>;
  const username = /^[a-z0-9_.]{3,12}$/i;
  if (!Number.isSafeInteger(item.id) || item.id! <= 0 || !Number.isSafeInteger(item.settledAt) ||
      typeof item.author !== 'string' || !username.test(item.author) || !Array.isArray(item.changes) ||
      item.changes.length > 2 || !item.changes.every(change => change &&
        typeof change.username === 'string' && username.test(change.username) &&
        (change.before === null || Number.isSafeInteger(change.before)) && Number.isSafeInteger(change.after))) return;
  const profile = item.authorProfile;
  const authorProfile = profile && typeof profile.name === 'string' && profile.name.length > 0 && profile.name.length <= 32
    ? {name: profile.name, ...(typeof profile.avatar === 'string' && /^[a-f0-9]{16}$/.test(profile.avatar) ? {avatar: profile.avatar} : {})}
    : undefined;
  return {id: item.id!, author: item.author, settledAt: item.settledAt!, changes: item.changes,
    ...(authorProfile ? {authorProfile} : {})};
}
export interface QuickChatSnapshot {
  readonly context: QuickChatContext | null;
  readonly entries: readonly QuickChatEntry[];
  readonly muted: readonly string[];
  readonly pickerOpen: boolean;
  readonly slots: readonly string[];
  readonly pickerPage: number;
  readonly editingText: boolean;
  readonly registered: boolean;
  readonly authorId: string;
  readonly bannedUntil: number;
  readonly draft: string;
  readonly sending: boolean;
  readonly sendError?: string;
}
interface Ports {
  send(message: Record<string, unknown>): void;
  voice(id: string): void;
  notify?(code: string): void;
  now?(): number;
  timers: {
    setTimeout(callback: () => void, delay: number): number;
    clearTimeout(id: number): void;
  };
}
type ExpiryPresenter = (entry: QuickChatEntry) => Promise<void> | void;
/** One document-lived state/timer owner. Presentation may finish the original
 * expiry fade; geometry, DOM, focus, animation and audio objects stay outside. */
export function createMultiplayerQuickChatModel(ports: Ports) {
  let snapshot: QuickChatSnapshot = {
    context: null,
    entries: [],
    muted: [],
    pickerOpen: false,
    slots: loadSlots(),
    pickerPage: 0,
    editingText: false,
    registered: false,
    authorId: '',
    bannedUntil: 0,
    draft: '',
    sending: false,
  };
  let sequence = 0,
    disposed = false,
    presenter: ExpiryPresenter | null = null;
  const retracted = new Map<string, number>();
  let historyThrough = 0;
  let banTimer: number | undefined,
    sendTimer: number | undefined,
    pendingId: number | undefined;
  const now = ports.now ?? Date.now;
  const listeners = new Set<() => void>(),
    timers = new Map<number, number>(),
    expiring = new Set<number>();
  function publish(next: QuickChatSnapshot, silent = false) {
    snapshot = next;
    if (silent) return;
    for (const listener of listeners) listener();
  }
  function failPending(code = 'service-unavailable') {
    const id = pendingId;
    pendingId = undefined;
    if (id === undefined) return;
    publish({...snapshot, sending:false, sendError:code, entries:snapshot.entries.map(entry => entry.id === id ? {...entry, delivery:'failed'} : entry)});
  }
  function cancel(entry: QuickChatEntry) {
    const timer = timers.get(entry.id);
    if (timer !== undefined) ports.timers.clearTimeout(timer);
    timers.delete(entry.id);
    expiring.delete(entry.id);
  }
  function finishExpiry(id: number) {
    const entry = snapshot.entries.find(item => item.id === id);
    if (disposed || !entry) return;
    cancel(entry);
    const entries = entry.messageId !== undefined || entry.rating
      ? snapshot.entries.map(item => item.id === id ? {...item, expiredInGame: true} : item)
      : snapshot.entries.filter(item => item.id !== id);
    publish({...snapshot, entries});
  }
  function expire(id: number) {
    timers.delete(id);
    const entry = snapshot.entries.find(item => item.id === id);
    if (disposed || !entry) return;
    expiring.add(id);
    const pending = !snapshot.context?.inRoom && presenter?.(entry);
    if (pending)
      void pending.then(
        () => finishExpiry(id),
        () => {},
      );
    else finishExpiry(id);
  }
  function append(entry: QuickChatEntry, silent: boolean) {
    const entries = [...snapshot.entries, entry];
    if (entries.length > 50) cancel(entries.shift()!);
    publish({...snapshot, entries}, silent);
    if (!entry.expiredInGame && !disposed && snapshot.entries.some(item => item.id === entry.id))
      timers.set(entry.id, ports.timers.setTimeout(() => expire(entry.id), 10000));
  }
  function update(next: QuickChatContext) {
    if (disposed) return;
    const previous = snapshot.context;
    const reset = previous?.room !== next.room || previous?.serial !== next.serial;
    const changed =
      reset ||
      !previous ||
      previous.visible !== next.visible ||
      previous.localSeat !== next.localSeat ||
      previous.connected !== next.connected ||
      previous.language !== next.language ||
      previous.lessMotion !== next.lessMotion ||
      previous.localSpectator !== next.localSpectator ||
      previous.inRoom !== next.inRoom ||
      JSON.stringify(previous.spectators) !== JSON.stringify(next.spectators) ||
      previous.seats.length !== next.seats.length ||
      next.seats.some(
        (seat, index) =>
          seat?.clientId !== previous.seats[index]?.clientId || seat?.name !== previous.seats[index]?.name || JSON.stringify(seat?.profile) !== JSON.stringify(previous.seats[index]?.profile),
      );
    if (!changed) return;
    if ((reset || !next.connected) && sendTimer !== undefined) {
      ports.timers.clearTimeout(sendTimer);
      sendTimer = undefined;
    }
    if (!next.connected && !reset) failPending();
    if (reset) pendingId = undefined;
    if (reset) {
      historyThrough = 0;
      for (const entry of snapshot.entries) if (previous?.room !== next.room || !entry.rating) cancel(entry);
    }
    const context = {
      ...next,
      seats: next.seats.map((seat) => (seat ? { ...seat } : null)),
      spectators: next.spectators?.map((member) => ({ ...member })),
    };
    publish({
      ...snapshot,
      context,
      ...(reset ? { entries: previous?.room === next.room ? snapshot.entries.filter(entry => entry.rating) : [], muted: [], draft: '', sending: false, sendError: undefined } : {}),
      ...(reset || next.inRoom || !next.visible || !next.connected ? { pickerOpen: false, editingText: false } : {}),
      ...(!next.connected ? { registered: false, authorId: '', bannedUntil: 0, sending: false } : {}),
    });
  }
  function receive(message: Record<string, unknown>, silent = false) {
    if (disposed) return;
    if (message.type === 'th09-rating') {
      const rating = ratingAnnouncement(message.settlement);
      if (!rating || !snapshot.context?.visible || message.room !== snapshot.context.room) return;
      const name = rating.authorProfile?.name ?? 'bot';
      const index = snapshot.entries.findIndex(entry => entry.rating?.id === rating.id);
      if (index >= 0) {
        const entry = snapshot.entries[index];
        const previous = entry.rating!;
        if (previous.author === rating.author && previous.authorProfile?.name === rating.authorProfile?.name &&
            previous.authorProfile?.avatar === rating.authorProfile?.avatar) return;
        const entries = [...snapshot.entries];
        entries[index] = {...entry, name, rating: {...previous, author: rating.author, authorProfile: rating.authorProfile}};
        publish({...snapshot, entries}, silent);
        return;
      }
      const entry: QuickChatEntry = {id: ++sequence, clientId: 'th09-rating', seat: -1, spectator: false,
        name, phrase: {id: 'th09-rating', zh: '', en: ''}, rating, at: rating.settledAt,
        ...(silent ? {expiredInGame: true} : {})};
      append(entry, silent);
      return;
    }
    if (message.type === 'chat-status') {
      const registered = message.registered === true && typeof message.authorId === 'string';
      const bannedUntil =
        typeof message.bannedUntil === 'number' && Number.isSafeInteger(message.bannedUntil) ? message.bannedUntil : 0;
      if (!registered && sendTimer !== undefined) {
        ports.timers.clearTimeout(sendTimer);
        sendTimer = undefined;
      }
      if (!registered) failPending();
      if (banTimer !== undefined) ports.timers.clearTimeout(banTimer);
      banTimer = undefined;
      publish({ ...snapshot, registered, authorId: registered ? String(message.authorId) : '', bannedUntil,
        ...(!registered ? {sending: false, editingText: false} : {}) });
      if (bannedUntil > now())
        banTimer = ports.timers.setTimeout(
          () => {
            banTimer = undefined;
            if (!disposed) publish({ ...snapshot, bannedUntil: 0 });
          },
          Math.min(bannedUntil - now(), 2147483647),
        );
      return;
    }
    if (message.type === 'chat-retract') {
      if (typeof message.authorId !== 'string' || !Number.isSafeInteger(message.throughId)) return;
      const through = Number(message.throughId);
      retracted.set(message.authorId, Math.max(retracted.get(message.authorId) || 0, through));
      if (retracted.size > 256) retracted.delete(retracted.keys().next().value!);
      const removed = snapshot.entries.filter(
        (entry) => entry.authorId === message.authorId && (entry.messageId || 0) <= through,
      );
      for (const entry of removed) cancel(entry);
      publish({ ...snapshot, entries: snapshot.entries.filter((entry) => !removed.includes(entry)) });
      if (message.authorId === snapshot.authorId)
        receive({
          type: 'chat-status',
          registered: snapshot.registered,
          authorId: snapshot.authorId,
          bannedUntil: message.bannedUntil,
        });
      return;
    }
    if (message.type === 'chat-result') {
      if (message.action === 'free-chat' && message.ok !== true && sendTimer !== undefined) {
        ports.timers.clearTimeout(sendTimer);
        sendTimer = undefined;
      }
      const code = typeof message.code === 'string' ? message.code : 'service-unavailable';
      if (message.action === 'free-chat' && message.ok !== true) failPending(code);
      if (message.ok !== true) {
        if (message.code === 'sign-in-required') publish({ ...snapshot, registered: false, editingText: false });
        if (message.action !== 'free-chat') ports.notify?.(code);
      } else if (message.action === 'chat-report') ports.notify?.('reported');
      return;
    }
    if (message.type === 'chat-history') {
      if (
        !Number.isSafeInteger(message.throughId) ||
        Number(message.throughId) < historyThrough ||
        !Array.isArray(message.entries)
      )
        return;
      historyThrough = Number(message.throughId);
      const retained = snapshot.entries.filter(
        (entry) => entry.messageId === undefined || entry.messageId > historyThrough,
      );
      for (const entry of snapshot.entries) if (!retained.includes(entry)) cancel(entry);
      publish({ ...snapshot, entries: [] }, true);
      for (const entry of message.entries.slice(-50)) {
        if (entry && typeof entry === 'object' && Number(entry.messageId) <= historyThrough)
          receive({ ...entry, type: 'free-chat' }, true);
      }
      const history = [...snapshot.entries].sort((left, right) => left.messageId! - right.messageId!);
      const entries = [...history, ...retained];
      if (Array.isArray(message.ratings) && message.room === snapshot.context?.room) {
        publish({...snapshot, entries}, true);
        for (const settlement of message.ratings.slice(-50)) receive({type: 'th09-rating', room: message.room, settlement}, true);
        entries.splice(0, entries.length, ...snapshot.entries);
        entries.sort((left, right) => (left.at ?? 0) - (right.at ?? 0));
      }
      for (const entry of entries.slice(0, -50)) cancel(entry);
      publish({ ...snapshot, entries: entries.slice(-50) });
      return;
    }
    const context = snapshot.context,
      free = message.type === 'free-chat',
      phrase =
        free && typeof message.text === 'string'
          ? { id: 'free-chat', zh: message.text, en: message.text }
          : quickChatPhrase(message.phrase);
    if (
      !context?.visible ||
      !phrase ||
      message.room !== context.room ||
      message.serial !== context.serial ||
      typeof message.seat !== 'number' ||
      !Number.isInteger(message.seat)
    )
      return;
    if (
      free &&
      (typeof message.text !== 'string' ||
        !message.text.trim() ||
        message.text.length > 500 ||
        !Number.isSafeInteger(message.messageId) ||
        Number(message.messageId) < 1 ||
        typeof message.authorId !== 'string' ||
        typeof message.clientId !== 'string' ||
        typeof message.name !== 'string' ||
        message.name.length > 32 ||
        message.seat < -1 ||
        message.seat > 2 ||
        (message.seat === -1) !== (message.spectator === true) ||
        snapshot.entries.some((entry) => entry.messageId === message.messageId) ||
        Number(message.messageId) <= (retracted.get(message.authorId) || 0))
    )
      return;
    const spectator = message.spectator === true && message.seat === -1;
    const seat = spectator
      ? context.spectators?.find((member) => member.clientId === message.clientId)
      : context.seats[message.seat];
    if (!free && (!seat || message.clientId !== seat.clientId)) return;
    const entry: QuickChatEntry = {
      id: ++sequence,
      clientId: free ? String(message.clientId) : seat!.clientId,
      seat: message.seat,
      spectator,
      name: free ? String(message.name) : seat!.name,
      phrase,
      ...(free ? { messageId: Number(message.messageId), authorId: String(message.authorId) } : {}),
      ...(Number.isSafeInteger(message.createdAt) ? {at: Number(message.createdAt)} : {}),
      ...(silent ? {expiredInGame: true} : {}),
    };
    const pending = free && !silent && pendingId !== undefined && message.clientId === context.localClientId &&
      message.authorId === snapshot.authorId && snapshot.entries.find(item => item.id === pendingId && item.phrase.zh === message.text);
    if (pending) {
      if (sendTimer !== undefined) ports.timers.clearTimeout(sendTimer);
      sendTimer = undefined;
      pendingId = undefined;
      const confirmed = {...entry, id: pending.id};
      publish({...snapshot, sending:false, entries:snapshot.entries.map(item => item.id === pending.id ? confirmed : item)});
      timers.set(confirmed.id, ports.timers.setTimeout(() => expire(confirmed.id), 10000));
      return;
    }
    if (
      !free &&
      !snapshot.muted.includes(entry.clientId) &&
      entry.clientId !== context.localClientId &&
      (!spectator || context.localSeat == null)
    )
      ports.voice(phrase.id);
    append(entry, silent);
  }
  function sendPhrase(id: unknown) {
    const context = snapshot.context,
      phrase = quickChatPhrase(id);
    if (disposed || !phrase || !context?.connected || (context.localSeat == null && !context.localSpectator)) return;
    ports.voice(phrase.id);
    ports.send({ type: 'quick-chat', phrase: phrase.id, serial: context.serial });
    publish({ ...snapshot, pickerOpen: false, editingText: false });
  }
  return {
    getSnapshot: () => snapshot,
    subscribe(listener: () => void) {
      listeners.add(listener);
      return () => {
        listeners.delete(listener);
      };
    },
    update,
    receive,
    setDraft(draft: string) {
      const value = draft.slice(0, 500);
      if (!disposed && snapshot.draft !== value)
        publish({
          ...snapshot,
          draft: value,
          editingText: snapshot.editingText || (snapshot.pickerOpen && snapshot.registered),
        });
    },
    sendText() {
      const context = snapshot.context;
      if (
        disposed ||
        !snapshot.registered ||
        !context?.connected ||
        (context.localSeat == null && !context.localSpectator) ||
        snapshot.sending
      )
        return;
      if (snapshot.bannedUntil === -1 || snapshot.bannedUntil > now()) {
        ports.notify?.('chat-banned');
        return;
      }
      const text = snapshot.draft.trim();
      if (!text) return;
      const local = context.localSeat == null ? context.spectators?.find(member => member.clientId === context.localClientId) : context.seats[context.localSeat];
      const entry:QuickChatEntry = {id:++sequence, clientId:context.localClientId ?? '', seat:context.localSeat ?? -1,
        spectator:context.localSeat == null, name:local?.name ?? '', authorId:snapshot.authorId, phrase:{id:'free-chat', zh:text, en:text}, delivery:'pending'};
      pendingId = entry.id;
      const entries = [...snapshot.entries, entry];
      if (entries.length > 50) cancel(entries.shift()!);
      publish({ ...snapshot, draft:'', sending:true, sendError:undefined, entries });
      sendTimer = ports.timers.setTimeout(() => {
        sendTimer = undefined;
        if (!disposed) failPending();
      }, 12000);
      ports.send({ type: 'free-chat', text, serial: context.serial });
    },
    reportMessage(messageId: number, reason: string) {
      const context = snapshot.context;
      const entry = snapshot.entries.find((item) => item.messageId === messageId);
      if (
        !disposed &&
        context?.connected &&
        (context.localSeat != null || context.localSpectator) &&
        typeof reason === 'string' && reason.trim().length > 0 && reason.trim().length <= 500 &&
        entry?.authorId &&
        entry.clientId !== context.localClientId &&
        entry.authorId !== snapshot.authorId
      )
        ports.send({ type: 'chat-report', messageId, reason: reason.trim() });
    },
    openComposer() {
      if (!disposed && !snapshot.pickerOpen) publish({ ...snapshot, pickerOpen: true, pickerPage: 0, editingText: false });
    },
    togglePicker() {
      if (!disposed) publish({ ...snapshot, pickerOpen: !snapshot.pickerOpen, pickerPage: 0, editingText: false });
    },
    toggleMember(clientId: string) {
      if (!disposed)
        publish({
          ...snapshot,
          muted: snapshot.muted.includes(clientId)
            ? snapshot.muted.filter((id) => id !== clientId)
            : [...snapshot.muted, clientId],
        });
    },
    dismiss() {
      if (!disposed && snapshot.pickerOpen) publish({ ...snapshot, pickerOpen: false, editingText: false });
    },
    setSlot(index: number, id: string) {
      if (disposed || !Number.isInteger(index) || index < 0 || index >= QUICK_CHAT_SLOT_COUNT || !quickChatPhrase(id)) return;
      const slots = [...snapshot.slots];
      slots[index] = id;
      try { globalThis.localStorage?.setItem(QUICK_CHAT_SLOTS_KEY, JSON.stringify(slots)); } catch { /* Keep the current session usable. */ }
      publish({ ...snapshot, slots });
    },
    setPickerPage(page: number) {
      if (!disposed && Number.isInteger(page) && page >= 0 && page <= Math.ceil(QUICK_CHAT_PHRASES.length / QUICK_CHAT_SLOT_COUNT))
        publish({ ...snapshot, pickerPage: page });
    },
    sendPhrase,
    sendSlot(slot: number) {
      if (snapshot.pickerOpen && !snapshot.editingText && slot >= 1 && slot <= QUICK_CHAT_SLOT_COUNT)
        sendPhrase(snapshot.pickerPage === 0 ? snapshot.slots[slot - 1] : QUICK_CHAT_PHRASES[(snapshot.pickerPage - 1) * QUICK_CHAT_SLOT_COUNT + slot - 1]?.id);
    },
    setExpiryPresenter(next: ExpiryPresenter) {
      presenter = next;
      return () => {
        if (presenter !== next) return;
        presenter = null;
        for (const id of [...expiring]) finishExpiry(id);
      };
    },
    dispose() {
      if (disposed) return;
      for (const entry of snapshot.entries) cancel(entry);
      if (banTimer !== undefined) ports.timers.clearTimeout(banTimer);
      if (sendTimer !== undefined) ports.timers.clearTimeout(sendTimer);
      retracted.clear();
      disposed = true;
      presenter = null;
      publish({ ...snapshot, entries: [], muted: [], pickerOpen: false, editingText: false });
      listeners.clear();
    },
  };
}
export type MultiplayerQuickChatModel = ReturnType<typeof createMultiplayerQuickChatModel>;
