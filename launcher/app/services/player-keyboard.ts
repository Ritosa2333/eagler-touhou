import {HostedKeyboard} from '../../src/launcher/hosted-keyboard.mts';
import {KeyboardMapping, keyboardMappingSchema, type KeyboardMappingPort} from '../../src/launcher/keyboard-mapping.mts';
import {keyboardInputBridge} from '../../src/launcher/touch-runtime-protocol.mts';
import type {RuntimeService, RuntimeMidiEventContext} from './runtime';
import type {KeyboardPreferences} from '../../src/launcher/keyboard-preferences.mts';
import type {Th09LocalInput} from '../../src/launcher/th09-local-input.mts';

export interface PlayerKeyboardOptions {
  runtime: Pick<RuntimeService, 'getInputContext' | 'getMidiEventContext' | 'getSnapshot' | 'postInput' | 'subscribe'>;
  window: EventTarget;
  document: EventTarget & {readonly visibilityState: string};
  frame: EventTarget & {focus(options?: FocusOptions): void; readonly contentWindow: {focus(): void} | null};
  playerOpen(): boolean;
  /** Original Element.closest(input,select,textarea,button,dialog,[role=dialog]). */
  launcherOwnsTarget(target: EventTarget | null): boolean;
  releaseHeldTouchFire(): void;
  cancelTouchFunction(): void;
  toggleFullscreen(): void;
  isPlayerFullscreen(): boolean;
  android: boolean;
  preferences?:KeyboardPreferences;
  th09Local?:Th09LocalInput;
  now?: () => number;
  setInterval?: typeof globalThis.setInterval;
  clearInterval?: typeof globalThis.clearInterval;
}
function sameNativeContext(a: RuntimeMidiEventContext | null, b: RuntimeMidiEventContext | null) {
  return a === b || !!a && !!b && a.epoch === b.epoch && a.game === b.game && a.document === b.document && a.target === b.target;
}
/** Launcher owns the mapping policy. A capable Runtime calls that policy in
 * its native event stack; older Runtime shells keep their original keyboard. */
export function createPlayerKeyboard(options: PlayerKeyboardOptions) {
  const keyboard = new HostedKeyboard();
  const now = options.now ?? (() => performance.now());
  const setInterval = options.setInterval ?? globalThis.setInterval;
  const clearInterval = options.clearInterval ?? globalThis.clearInterval;
  let timer: ReturnType<typeof globalThis.setInterval> | null = null;
  let deadline = 0, disposed = false, chord = false;
  let bound: RuntimeMidiEventContext | null = null;
  let unbind: (() => void) | null = null;
  // True while a launcher lifecycle cancellation (blur/pagehide/visibility or
  // a preference swap) flushes held keys. Runtime-requested resets — such as
  // the between-stage loading barrier — must instead let a still-held physical
  // key rebuild ownership from its browser-reported auto-repeat.
  let cancelling = false;
  let identity = options.runtime.getInputContext();
  let inputAllowed = identity.launched && !identity.spectator;
  let mapping: KeyboardMapping | null = null;
  function stopFocusRelay() {
    if (timer !== null) clearInterval(timer);
    timer = null; deadline = 0;
  }
  function focusBrowsingContext() {
    if (!options.android || !options.playerOpen() || !options.frame.contentWindow) return;
    try {options.frame.focus({preventScroll: true});} catch {try {options.frame.focus();} catch {}}
    try {options.frame.contentWindow.focus();} catch {}
  }
  function keepFocused() {
    if (!deadline || now() >= deadline) {stopFocusRelay(); return;}
    if (options.document.visibilityState !== 'hidden') focusBrowsingContext();
  }
  function startFocusRelay() {
    if (disposed || !options.android) return;
    stopFocusRelay(); deadline = now() + 15000; keepFocused();
    timer = setInterval(keepFocused, 100);
  }
  const forward: EventListener = raw => {
    const event = raw as KeyboardEvent;
    const state = options.runtime.getSnapshot();
    if (!state.launched || !options.playerOpen() || !options.frame.contentWindow) {keyboard.clear(); return;}
    const context = options.runtime.getInputContext();
    if (mapping && keyboardInputBridge(context)) {
      if (event.type === 'keydown' && event.isComposing) return;
      if (options.launcherOwnsTarget(event.target) && event.type === 'keydown') return;
      // Mapping recognition and modifier chords gate fresh input only. A
      // keyup must always reach the Runtime so no held key can be stranded.
      if (event.type === 'keydown' && (!mapping.recognizes(event) || event.altKey || event.metaKey)) return;
      if (options.runtime.postInput('keyboard', {code:event.code,key:event.key,keyCode:event.keyCode,location:event.location,
          down:event.type === 'keydown',repeat:event.repeat,inputSource:'physical',launcherOwnsFocus:options.launcherOwnsTarget(event.target)})) event.preventDefault();
      return;
    }
    const keys = keyboard.forward(event, context, options.launcherOwnsTarget(event.target));
    for (const key of keys) options.runtime.postInput('keyboard', {down: event.type === 'keydown', ...key});
    if (keys.length) event.preventDefault();
  };
  function clear() {
    cancelling = true;
    try {
      options.releaseHeldTouchFire(); options.cancelTouchFunction(); keyboard.clear();
      const context = options.runtime.getInputContext();
      const bridge = keyboardInputBridge(context);
      if (bridge && bridge.submit({protocol:context.protocol,game:context.game,epoch:context.epoch,command:'keyboard-clear'})) return;
      if (!options.runtime.getSnapshot().launched || !options.frame.contentWindow) return;
      options.runtime.postInput('keyboard-clear', {});
    } finally { cancelling = false; }
  }
  function rebind() {
    if (disposed) return;
    const nextIdentity = options.runtime.getInputContext();
    if (identity.target !== nextIdentity.target || identity.game !== nextIdentity.game || identity.epoch !== nextIdentity.epoch) {
      // Runtime owns flushing the outgoing epoch; never send its key releases
      // to the replacement document/epoch.
      keyboard.clear(); options.releaseHeldTouchFire(); options.cancelTouchFunction(); chord = false;
      stopFocusRelay(); identity = nextIdentity;
    }
    const state = options.runtime.getSnapshot();
    const allowed = nextIdentity.launched && !nextIdentity.spectator;
    if (inputAllowed && !allowed) clear();
    inputAllowed = allowed;
    if (state.firstFrame || !state.ready) stopFocusRelay();
    const next = options.runtime.getMidiEventContext();
    if (sameNativeContext(bound, next)) return;
    unbind?.(); unbind = null; bound = next; mapping = null;
    if (!next) return;
    const fullscreen: EventListener = raw => {
      if (!sameNativeContext(next, options.runtime.getMidiEventContext())) return;
      const event = raw as KeyboardEvent;
      if (event.type === 'keydown' && event.code === 'Enter' && event.altKey && !event.ctrlKey && !event.metaKey) {
        event.preventDefault(); event.stopImmediatePropagation();
        if (!event.repeat && !chord) {chord = true; options.toggleFullscreen();}
      } else if (event.type === 'keyup' && event.code === 'Enter' && chord) {
        event.preventDefault(); event.stopImmediatePropagation(); chord = false;
      }
    };
    const target = next.target as EventTarget;
    let port: KeyboardMappingPort | null = null;
    const inputContext = options.runtime.getInputContext();
    const mappingTarget = inputContext.target;
    if (mappingTarget && keyboardInputBridge(inputContext)) {
      const owner = new KeyboardMapping(options.preferences?.getSnapshot,options.th09Local);
      mapping = owner;
      let clearing = false;
      port = {
        schema:keyboardMappingSchema,protocol:inputContext.protocol,game:next.game,epoch:next.epoch,origin:inputContext.targetOrigin,
        supports:code=>owner.supports(code),
        sampleLocal(seat){if(!options.playerOpen()||options.document.visibilityState==='hidden')owner.clear();return owner.sampleLocal(seat,options.runtime.getInputContext());},
        map(event,down,source,profile) {
          if (clearing || !sameNativeContext(next,options.runtime.getMidiEventContext()) || !options.playerOpen()) return [];
          if (source === 'physical' && event.code === 'Enter' && (down && event.altKey && !event.metaKey && !event.ctrlKey || !down && chord)) {
            if (down) {if (!event.repeat && !chord) {chord = true;options.toggleFullscreen();}}
            else chord = false;
            return [];
          }
          if (source === 'physical' && down && (event.altKey || event.metaKey)) return null;
          return owner.map(event,down,source,profile,options.runtime.getInputContext());
        },
        clear() {
          clearing = true;
          try {options.cancelTouchFunction();options.releaseHeldTouchFire();return owner.clear(cancelling);}
          finally {clearing = false;}
        },
      };
      mappingTarget.__eaglerKeyboardMapping = port;
    }
    target.addEventListener('keydown', fullscreen, true); target.addEventListener('keyup', fullscreen, true);
    unbind = () => {
      target.removeEventListener('keydown', fullscreen, true);target.removeEventListener('keyup', fullscreen, true);
      try {if (port && mappingTarget?.__eaglerKeyboardMapping === port) delete mappingTarget.__eaglerKeyboardMapping;}
      catch { /* The retired document may have navigated. */ }
    };
  }
  const visibility = () => {if (options.document.visibilityState === 'hidden') clear();};
  const fullscreenChange = () => {if (!options.isPlayerFullscreen()) chord = false;};
  options.window.addEventListener('keydown', forward, true);
  options.window.addEventListener('keyup', forward, true);
  options.window.addEventListener('blur', clear);
  options.window.addEventListener('pagehide', clear);
  options.document.addEventListener('visibilitychange', visibility);
  options.document.addEventListener('fullscreenchange', fullscreenChange);
  options.document.addEventListener('webkitfullscreenchange', fullscreenChange);
  options.frame.addEventListener('load', rebind);
  const unsubscribe = options.runtime.subscribe(rebind);
  const unsubscribePreferences = options.preferences?.subscribe(clear);
  const unsubscribeLocal = options.th09Local?.subscribe(clear);
  rebind();
  return {
    clear, rebind, startFocusRelay, stopFocusRelay, focusBrowsingContext,
    dispose() {
      if (disposed) return;
      clear(); disposed = true; stopFocusRelay(); unsubscribe(); unsubscribePreferences?.();unsubscribeLocal?.(); unbind?.(); unbind = null; bound = null;
      options.window.removeEventListener('keydown', forward, true);
      options.window.removeEventListener('keyup', forward, true);
      options.window.removeEventListener('blur', clear);
      options.window.removeEventListener('pagehide', clear);
      options.document.removeEventListener('visibilitychange', visibility);
      options.document.removeEventListener('fullscreenchange', fullscreenChange);
      options.document.removeEventListener('webkitfullscreenchange', fullscreenChange);
      options.frame.removeEventListener('load', rebind);
    },
  };
}
export type PlayerKeyboard = ReturnType<typeof createPlayerKeyboard>;
