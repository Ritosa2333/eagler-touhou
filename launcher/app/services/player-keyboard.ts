import { HostedKeyboard } from '../../src/launcher/hosted-keyboard.mts';
import {
  KeyboardMapping,
  keyboardMappingSchema,
  type KeyboardMappingPort,
} from '../../src/launcher/keyboard-mapping.mts';
import { keyboardInputBridge } from '../../src/launcher/touch-runtime-protocol.mts';
import type { RuntimeService, RuntimeMidiEventContext } from './runtime';
import type { KeyboardPreferences } from '../../src/launcher/keyboard-preferences.mts';
import type { Th09LocalInput } from '../../src/launcher/th09-local-input.mts';
import type { MultiplayerQuickChatModel } from '../../src/launcher/multiplayer-quick-chat-model.mts';
import type { HostedKeyboardEvent } from '../../src/launcher/hosted-keyboard.mts';

export interface PlayerKeyboardOptions {
  runtime: Pick<RuntimeService, 'getInputContext' | 'getMidiEventContext' | 'getSnapshot' | 'postInput' | 'subscribe'>;
  window: EventTarget;
  document: EventTarget & { readonly visibilityState: string };
  frame: EventTarget & { focus(options?: FocusOptions): void; readonly contentWindow: { focus(): void } | null };
  playerOpen(): boolean;
  /** Original Element.closest(input,select,textarea,button,dialog,[role=dialog]). */
  launcherOwnsTarget(target: EventTarget | null): boolean;
  releaseHeldTouchFire(): void;
  cancelTouchFunction(): void;
  toggleFullscreen(): void;
  isPlayerFullscreen(): boolean;
  android: boolean;
  shortcutsOnly?: boolean;
  chat?: Pick<MultiplayerQuickChatModel, 'getSnapshot' | 'subscribe' | 'openComposer' | 'dismiss'>;
  chatOwnsTarget?(target: EventTarget | null): boolean;
  preferences?: KeyboardPreferences;
  th09Local?: Th09LocalInput;
  now?: () => number;
  setInterval?: typeof globalThis.setInterval;
  clearInterval?: typeof globalThis.clearInterval;
}
function sameNativeContext(a: RuntimeMidiEventContext | null, b: RuntimeMidiEventContext | null) {
  return (
    a === b ||
    (!!a && !!b && a.epoch === b.epoch && a.game === b.game && a.document === b.document && a.target === b.target)
  );
}
/** Launcher owns the mapping policy. A capable Runtime calls that policy in
 * its native event stack; older Runtime shells keep their original keyboard. */
export function createPlayerKeyboard(options: PlayerKeyboardOptions) {
  const keyboard = new HostedKeyboard();
  const now = options.now ?? (() => performance.now());
  const setInterval = options.setInterval ?? globalThis.setInterval;
  const clearInterval = options.clearInterval ?? globalThis.clearInterval;
  let timer: ReturnType<typeof globalThis.setInterval> | null = null;
  let deadline = 0,
    disposed = false,
    chord = false;
  let bound: RuntimeMidiEventContext | null = null;
  let unbind: (() => void) | null = null;
  let identity = options.runtime.getInputContext();
  let inputAllowed = identity.launched && !identity.spectator;
  let mapping: KeyboardMapping | null = null;
  let pendingAlt: { code: string; used: boolean } | null = null,
    chatFocused = false;
  const modifiers = new Set<string>(),
    suppressed = new Set<string>();
  type ChatKey = HostedKeyboardEvent & { isComposing?: boolean; target?: EventTarget | null };
  function chatAvailable() {
    const state = options.chat?.getSnapshot(),
      context = state?.context;
    return (
      !!state?.registered &&
      !!context?.visible &&
      !context.inRoom &&
      context.connected &&
      (context.localSeat != null || context.localSpectator === true) &&
      options.playerOpen() &&
      options.runtime.getInputContext().launched
    );
  }
  function chatOpen() {
    const state = options.chat?.getSnapshot();
    return chatAvailable() && !!state?.pickerOpen;
  }
  function focusGame() {
    if (
      !options.playerOpen() ||
      !options.runtime.getInputContext().launched ||
      options.document.visibilityState === 'hidden'
    )
      return;
    try {
      options.frame.focus({ preventScroll: true });
    } catch {
      try {
        options.frame.focus();
      } catch {}
    }
    try {
      options.frame.contentWindow?.focus();
    } catch {}
  }
  function syncChat() {
    const open = chatOpen();
    if (open === chatFocused) return;
    chatFocused = open;
    if (open) {
      stopFocusRelay();
      clear();
    } else if (options.chat?.getSnapshot().context?.inRoom !== true) focusGame();
  }
  function consumeChatKey(event: ChatKey, down: boolean) {
    const code = event.code || (event.key === 'Alt' ? 'AltLeft' : event.key === 'Escape' ? 'Escape' : '');
    if (suppressed.has(code)) {
      if (!down) suppressed.delete(code);
      return true;
    }
    if (/^(Shift|Control|Meta)(Left|Right)$/.test(code)) {
      if (down) modifiers.add(code);
      else modifiers.delete(code);
    }
    const alt = code === 'AltLeft' || code === 'AltRight';
    const ownsTarget = options.launcherOwnsTarget(event.target ?? null),
      chatTarget = options.chatOwnsTarget?.(event.target ?? null) === true;
    if (pendingAlt && down && (!alt || pendingAlt.code !== code)) pendingAlt.used = true;
    if (alt && !down && pendingAlt?.code === code) {
      const activate =
        !pendingAlt.used &&
        !event.ctrlKey &&
        !event.metaKey &&
        !modifiers.size &&
        !event.isComposing &&
        (!ownsTarget || chatTarget) &&
        chatAvailable();
      pendingAlt = null;
      if (activate) {
        if (chatOpen()) options.chat?.dismiss();
        else options.chat?.openComposer();
      }
      return true;
    }
    if (!chatAvailable() || event.isComposing) return false;
    if (ownsTarget && !chatTarget) return false;
    if (alt && down && event.key !== 'AltGraph' && !event.ctrlKey && !event.metaKey && !modifiers.size) {
      if (!pendingAlt && !event.repeat) pendingAlt = { code, used: false };
      return !!pendingAlt;
    }
    if (down && code === 'Escape' && chatOpen()) {
      suppressed.add(code);
      options.chat?.dismiss();
      return true;
    }
    if (chatOpen() && !ownsTarget) {
      if (down) suppressed.add(code);
      return true;
    }
    return false;
  }
  function stopFocusRelay() {
    if (timer !== null) clearInterval(timer);
    timer = null;
    deadline = 0;
  }
  function focusBrowsingContext() {
    if (chatFocused || !options.android || !options.playerOpen() || !options.frame.contentWindow) return;
    try {
      options.frame.focus({ preventScroll: true });
    } catch {
      try {
        options.frame.focus();
      } catch {}
    }
    try {
      options.frame.contentWindow.focus();
    } catch {}
  }
  function keepFocused() {
    if (!deadline || now() >= deadline) {
      stopFocusRelay();
      return;
    }
    if (options.document.visibilityState !== 'hidden') focusBrowsingContext();
  }
  function startFocusRelay() {
    if (disposed || chatFocused || !options.android) return;
    stopFocusRelay();
    deadline = now() + 15000;
    keepFocused();
    timer = setInterval(keepFocused, 100);
  }
  const forward: EventListener = (raw) => {
    const event = raw as KeyboardEvent;
    if (consumeChatKey(event, event.type === 'keydown')) {
      event.preventDefault();
      event.stopImmediatePropagation();
      return;
    }
    if (options.shortcutsOnly) return;
    if (chatFocused && options.chatOwnsTarget?.(event.target)) return;
    const context = options.runtime.getInputContext();
    if (!context.launched || !options.playerOpen() || !options.frame.contentWindow) {
      keyboard.clear();
      return;
    }
    if (mapping && keyboardInputBridge(context)) {
      if (event.type === 'keydown' && event.isComposing) return;
      if (options.launcherOwnsTarget(event.target) && event.type === 'keydown') return;
      if (!mapping.recognizes(event) || (event.type === 'keydown' && (event.altKey || event.metaKey))) return;
      if (
        options.runtime.postInput('keyboard', {
          code: event.code,
          key: event.key,
          keyCode: event.keyCode,
          location: event.location,
          down: event.type === 'keydown',
          repeat: event.repeat,
          inputSource: 'physical',
          launcherOwnsFocus: options.launcherOwnsTarget(event.target),
        })
      )
        event.preventDefault();
      return;
    }
    const keys = keyboard.forward(event, context, options.launcherOwnsTarget(event.target));
    for (const key of keys) options.runtime.postInput('keyboard', { down: event.type === 'keydown', ...key });
    if (keys.length) event.preventDefault();
  };
  function clear() {
    options.releaseHeldTouchFire();
    options.cancelTouchFunction();
    keyboard.clear();
    if (options.shortcutsOnly) return;
    const context = options.runtime.getInputContext();
    const bridge = keyboardInputBridge(context);
    if (
      bridge?.submit({
        protocol: context.protocol,
        game: context.game,
        epoch: context.epoch,
        command: 'keyboard-clear',
      })
    )
      return;
    if (!context.launched || !options.frame.contentWindow) return;
    options.runtime.postInput('keyboard-clear', {});
  }
  function rebind() {
    if (disposed) return;
    const nextIdentity = options.runtime.getInputContext();
    if (
      identity.target !== nextIdentity.target ||
      identity.game !== nextIdentity.game ||
      identity.epoch !== nextIdentity.epoch
    ) {
      // Runtime owns flushing the outgoing epoch; never send its key releases
      // to the replacement document/epoch.
      keyboard.clear();
      options.releaseHeldTouchFire();
      options.cancelTouchFunction();
      chord = false;
      stopFocusRelay();
      pendingAlt = null;
      modifiers.clear();
      suppressed.clear();
      identity = nextIdentity;
    }
    const state = options.runtime.getSnapshot();
    const allowed = nextIdentity.launched && !nextIdentity.spectator;
    if (inputAllowed && !allowed) clear();
    inputAllowed = allowed;
    if (state.firstFrame || !state.ready) stopFocusRelay();
    syncChat();
    const next = options.runtime.getMidiEventContext();
    if (sameNativeContext(bound, next)) return;
    unbind?.();
    unbind = null;
    bound = next;
    mapping = null;
    if (!next) return;
    const fullscreen: EventListener = (raw) => {
      if (!sameNativeContext(next, options.runtime.getMidiEventContext())) return;
      const event = raw as KeyboardEvent;
      if (consumeChatKey(event, event.type === 'keydown')) {
        event.preventDefault();
        event.stopImmediatePropagation();
        return;
      }
      if (event.type === 'keydown' && event.code === 'Enter' && event.altKey && !event.ctrlKey && !event.metaKey) {
        event.preventDefault();
        event.stopImmediatePropagation();
        if (!event.repeat && !chord) {
          chord = true;
          options.toggleFullscreen();
        }
      } else if (event.type === 'keyup' && event.code === 'Enter' && chord) {
        event.preventDefault();
        event.stopImmediatePropagation();
        chord = false;
      }
    };
    const target = next.target as EventTarget;
    let port: KeyboardMappingPort | null = null;
    const inputContext = options.runtime.getInputContext();
    const mappingTarget = inputContext.target;
    if (!options.shortcutsOnly && mappingTarget && keyboardInputBridge(inputContext)) {
      const owner = new KeyboardMapping(options.preferences?.getSnapshot, options.th09Local);
      mapping = owner;
      let clearing = false;
      port = {
        schema: keyboardMappingSchema,
        protocol: inputContext.protocol,
        game: next.game,
        epoch: next.epoch,
        origin: inputContext.targetOrigin,
        supports: (code) => owner.supports(code),
        sampleLocal(seat) {
          if (!options.playerOpen() || options.document.visibilityState === 'hidden') owner.clear();
          return owner.sampleLocal(seat, options.runtime.getInputContext());
        },
        map(event, down, source, profile) {
          if (clearing || !sameNativeContext(next, options.runtime.getMidiEventContext()) || !options.playerOpen())
            return [];
          if (source === 'physical' && consumeChatKey(event, down)) return [];
          if (
            source === 'physical' &&
            event.code === 'Enter' &&
            ((down && event.altKey && !event.metaKey && !event.ctrlKey) || (!down && chord))
          ) {
            if (down) {
              if (!event.repeat && !chord) {
                chord = true;
                options.toggleFullscreen();
              }
            } else chord = false;
            return [];
          }
          if (source === 'physical' && down && (event.altKey || event.metaKey)) return null;
          return owner.map(event, down, source, profile, options.runtime.getInputContext());
        },
        clear() {
          clearing = true;
          try {
            options.cancelTouchFunction();
            options.releaseHeldTouchFire();
            return owner.clear();
          } finally {
            clearing = false;
          }
        },
      };
      mappingTarget.__eaglerKeyboardMapping = port;
    }
    target.addEventListener('keydown', fullscreen, true);
    target.addEventListener('keyup', fullscreen, true);
    unbind = () => {
      target.removeEventListener('keydown', fullscreen, true);
      target.removeEventListener('keyup', fullscreen, true);
      try {
        if (port && mappingTarget?.__eaglerKeyboardMapping === port) delete mappingTarget.__eaglerKeyboardMapping;
      } catch {
        /* The retired document may have navigated. */
      }
    };
  }
  const blur = () => {
    pendingAlt = null;
    modifiers.clear();
    clear();
  };
  const visibility = () => {
    if (options.document.visibilityState === 'hidden') blur();
  };
  const fullscreenChange = () => {
    if (!options.isPlayerFullscreen()) chord = false;
  };
  options.window.addEventListener('keydown', forward, true);
  options.window.addEventListener('keyup', forward, true);
  options.window.addEventListener('blur', blur);
  options.window.addEventListener('pagehide', blur);
  options.document.addEventListener('visibilitychange', visibility);
  options.document.addEventListener('fullscreenchange', fullscreenChange);
  options.document.addEventListener('webkitfullscreenchange', fullscreenChange);
  options.frame.addEventListener('load', rebind);
  const unsubscribe = options.runtime.subscribe(rebind);
  const unsubscribePreferences = options.preferences?.subscribe(clear);
  const unsubscribeLocal = options.th09Local?.subscribe(clear);
  const unsubscribeChat = options.chat?.subscribe(syncChat);
  rebind();
  return {
    clear,
    rebind,
    startFocusRelay,
    stopFocusRelay,
    focusBrowsingContext,
    dispose() {
      if (disposed) return;
      clear();
      disposed = true;
      pendingAlt = null;
      modifiers.clear();
      suppressed.clear();
      stopFocusRelay();
      unsubscribe();
      unsubscribePreferences?.();
      unsubscribeLocal?.();
      unsubscribeChat?.();
      unbind?.();
      unbind = null;
      bound = null;
      options.window.removeEventListener('keydown', forward, true);
      options.window.removeEventListener('keyup', forward, true);
      options.window.removeEventListener('blur', blur);
      options.window.removeEventListener('pagehide', blur);
      options.document.removeEventListener('visibilitychange', visibility);
      options.document.removeEventListener('fullscreenchange', fullscreenChange);
      options.document.removeEventListener('webkitfullscreenchange', fullscreenChange);
      options.frame.removeEventListener('load', rebind);
    },
  };
}
export type PlayerKeyboard = ReturnType<typeof createPlayerKeyboard>;
