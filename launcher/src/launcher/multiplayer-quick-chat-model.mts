import { quickChatPhrase, type QuickChatPhrase } from '../contracts/multiplayer-quick-chat.mjs';

export interface QuickChatSeat {
  clientId: string;
  name: string;
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
}
export interface QuickChatSnapshot {
  readonly context: QuickChatContext | null;
  readonly entries: readonly QuickChatEntry[];
  readonly muted: readonly string[];
  readonly pickerOpen: boolean;
  readonly registered: boolean;
  readonly authorId: string;
  readonly bannedUntil: number;
  readonly draft: string;
  readonly sending: boolean;
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
    submittedDraft = '';
  const now = ports.now ?? Date.now;
  const listeners = new Set<() => void>(),
    timers = new Map<QuickChatEntry, number>(),
    expiring = new Set<QuickChatEntry>();
  function publish(next: QuickChatSnapshot, silent = false) {
    snapshot = next;
    if (silent) return;
    for (const listener of listeners) listener();
  }
  function cancel(entry: QuickChatEntry) {
    const timer = timers.get(entry);
    if (timer !== undefined) ports.timers.clearTimeout(timer);
    timers.delete(entry);
    expiring.delete(entry);
  }
  function remove(entry: QuickChatEntry) {
    if (disposed || !snapshot.entries.includes(entry)) return;
    cancel(entry);
    publish({ ...snapshot, entries: snapshot.entries.filter((item) => item !== entry) });
  }
  function expire(entry: QuickChatEntry) {
    timers.delete(entry);
    if (disposed || !snapshot.entries.includes(entry)) return;
    expiring.add(entry);
    const pending = presenter?.(entry);
    if (pending)
      void pending.then(
        () => remove(entry),
        () => {},
      );
    else remove(entry);
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
          seat?.clientId !== previous.seats[index]?.clientId || seat?.name !== previous.seats[index]?.name,
      );
    if (!changed) return;
    if ((reset || !next.connected) && sendTimer !== undefined) {
      ports.timers.clearTimeout(sendTimer);
      sendTimer = undefined;
    }
    if (reset) {
      historyThrough = 0;
      for (const entry of snapshot.entries) cancel(entry);
    }
    const context = {
      ...next,
      seats: next.seats.map((seat) => (seat ? { ...seat } : null)),
      spectators: next.spectators?.map((member) => ({ ...member })),
    };
    publish({
      ...snapshot,
      context,
      ...(reset ? { entries: [], muted: [], pickerOpen: false, draft: '', sending: false } : {}),
      ...(!next.connected ? { registered: false, authorId: '', bannedUntil: 0, sending: false } : {}),
    });
  }
  function receive(message: Record<string, unknown>, silent = false) {
    if (disposed) return;
    if (message.type === 'chat-status') {
      const registered = message.registered === true && typeof message.authorId === 'string';
      const bannedUntil =
        typeof message.bannedUntil === 'number' && Number.isSafeInteger(message.bannedUntil) ? message.bannedUntil : 0;
      if (!registered && sendTimer !== undefined) {
        ports.timers.clearTimeout(sendTimer);
        sendTimer = undefined;
      }
      if (banTimer !== undefined) ports.timers.clearTimeout(banTimer);
      banTimer = undefined;
      publish({ ...snapshot, registered, authorId: registered ? String(message.authorId) : '', bannedUntil,
        ...(!registered ? {sending: false} : {}) });
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
      if (message.action === 'free-chat' && sendTimer !== undefined) {
        ports.timers.clearTimeout(sendTimer);
        sendTimer = undefined;
      }
      if (message.action === 'free-chat')
        publish({
          ...snapshot,
          sending: false,
          ...(message.ok === true && snapshot.draft === submittedDraft ? { draft: '' } : {}),
        });
      if (message.ok !== true) {
        if (message.code === 'sign-in-required') publish({ ...snapshot, registered: false });
        ports.notify?.(typeof message.code === 'string' ? message.code : 'service-unavailable');
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
    };
    if (
      !free &&
      !snapshot.muted.includes(entry.clientId) &&
      entry.clientId !== context.localClientId &&
      (!spectator || context.localSeat == null)
    )
      ports.voice(phrase.id);
    const entries = [...snapshot.entries, entry];
    if (entries.length > 50) cancel(entries.shift()!);
    publish({ ...snapshot, entries }, silent);
    if (!free && !disposed && snapshot.entries.includes(entry))
      timers.set(
        entry,
        ports.timers.setTimeout(() => expire(entry), 10000),
      );
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
      if (!disposed) publish({ ...snapshot, draft: draft.slice(0, 500) });
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
      submittedDraft = snapshot.draft;
      publish({ ...snapshot, sending: true });
      sendTimer = ports.timers.setTimeout(() => {
        sendTimer = undefined;
        if (!disposed) {
          publish({ ...snapshot, sending: false });
          ports.notify?.('service-unavailable');
        }
      }, 12000);
      ports.send({ type: 'free-chat', text, serial: context.serial });
    },
    reportMessage(messageId: number) {
      const entry = snapshot.entries.find((item) => item.messageId === messageId);
      if (
        !disposed &&
        snapshot.context?.connected &&
        snapshot.registered &&
        entry?.authorId &&
        entry.authorId !== snapshot.authorId
      )
        ports.send({ type: 'chat-report', messageId });
    },
    openComposer() {
      if (!disposed && !snapshot.pickerOpen) publish({ ...snapshot, pickerOpen: true });
    },
    togglePicker() {
      if (!disposed) publish({ ...snapshot, pickerOpen: !snapshot.pickerOpen });
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
      if (!disposed) publish({ ...snapshot, pickerOpen: false });
    },
    sendPhrase(id: unknown) {
      const context = snapshot.context,
        phrase = quickChatPhrase(id);
      if (
        disposed ||
        !phrase ||
        !context ||
        (context.localSeat == null && !context.localSpectator) ||
        !context.connected
      )
        return;
      ports.voice(phrase.id);
      ports.send({ type: 'quick-chat', phrase: phrase.id, serial: context.serial });
      publish({ ...snapshot, pickerOpen: false });
    },
    setExpiryPresenter(next: ExpiryPresenter) {
      presenter = next;
      return () => {
        if (presenter !== next) return;
        presenter = null;
        for (const entry of [...expiring]) remove(entry);
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
      publish({ ...snapshot, entries: [], muted: [], pickerOpen: false });
      listeners.clear();
    },
  };
}
export type MultiplayerQuickChatModel = ReturnType<typeof createMultiplayerQuickChatModel>;
