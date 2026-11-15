import {useCallback, useEffect, useLayoutEffect, useRef} from 'react';
import {animateSurface, type SurfaceAnimation, type SurfaceKind} from '../../ui/surface-motion';

export interface MainDialogProps {
  open: boolean;
  /** Preserve routing/DOM while a non-dialog fullscreen foreground owns input. */
  suspended?: boolean;
  /** Central Router owner consumes the intent; component never writes history. */
  onCloseRequest(): void;
  /** Domain cleanup after an actual user/Router close, not StrictMode disposal. */
  onClosed?(): void;
}
interface MainDialogOptions {motion?: SurfaceKind | false; immediateClose?: boolean; returnValue?: string;}
/** One native-dialog lifecycle. Shared motion never owns presence or navigation.
 * Router Back closes without recursively requesting another history change.
 * The parent dialog is left open when a child native dialog is presented.
 */
export function useMainDialog({open, suspended = false, onCloseRequest, onClosed}: MainDialogProps,
  {motion = 'dialog', immediateClose = false, returnValue}: MainDialogOptions = {}) {
  const ref = useRef<HTMLDialogElement>(null);
  const animation = useRef<SurfaceAnimation | null>(null);
  const finish = useRef<(() => void) | null>(null);
  const notifyAfterClose = useRef(false);
  const suspendedFocus = useRef<HTMLElement | null>(null);
  const lifetimeOpen = useRef(false);
  const ownedCloseEvents = useRef(0);
  const callback = useRef({open, suspended, onCloseRequest, onClosed}); callback.current = {open, suspended, onCloseRequest, onClosed};
  const closeNative = useCallback((dialog: HTMLDialogElement, value?: string) => {
    if (!dialog.open) return;
    ownedCloseEvents.current++;
    dialog.close(value);
  }, []);
  const clear = useCallback(() => {
    finish.current = null; notifyAfterClose.current = false;
    const previous = animation.current; animation.current = null; previous?.cancel();
  }, []);
  const observe = useCallback((active: SurfaceAnimation, completed?: () => void) => {
    const settled = () => {
      if (animation.current !== active) return;
      // Current cancellation also settles the native owner. Owner-triggered
      // cancellation clears this identity first, so stale results do nothing.
      if (completed) completed();
      else {animation.current = null; active.cancel();}
    };
    void Promise.resolve(active.finished).then(settled, settled);
  }, []);
  const close = useCallback((notify: boolean, immediate = false) => {
    const dialog = ref.current;
    if (!dialog?.open) return;
    if (finish.current) {
      // A committed Router Back supersedes an earlier button-close intent.
      if (!notify) notifyAfterClose.current = false;
      // Missing donation artwork closes immediately even during an exit.
      if (immediate) finish.current();
      return;
    }
    notifyAfterClose.current = notify;
    const complete = () => {
      if (finish.current !== complete) return;
      const shouldNotify = notifyAfterClose.current;
      const closingAnimation = animation.current; animation.current = null;
      clear();
      closeNative(dialog, returnValue);
      closingAnimation?.cancel();
      dialog.classList.remove('closing');
      if (lifetimeOpen.current) {lifetimeOpen.current = false; callback.current.onClosed?.();}
      if (shouldNotify) callback.current.onCloseRequest();
    };
    finish.current = complete;
    if (immediate || immediateClose || motion === false) complete();
    else {
      dialog.classList.add('closing');
      const next = animateSurface(dialog, 'exit', {kind: motion, fromCurrent: true}), previous = animation.current;
      animation.current = next; previous?.cancel();
      if (next) observe(next, complete); else complete();
    }
  }, [clear, closeNative, returnValue, immediateClose, motion, observe]);
  useLayoutEffect(() => {
    const dialog = ref.current;
    if (!dialog) return;
    dialog.toggleAttribute('data-window-managed', motion !== false);
    if (suspended) {
      if (open) lifetimeOpen.current = true;
      else if (lifetimeOpen.current) {lifetimeOpen.current = false; callback.current.onClosed?.();}
      clear(); dialog.classList.remove('closing');
      if (document.activeElement instanceof HTMLElement && dialog.contains(document.activeElement)) suspendedFocus.current = document.activeElement;
      closeNative(dialog);
    } else if (open) {
      lifetimeOpen.current = true;
      const wasOpen = dialog.open;
      if (!wasOpen) dialog.showModal();
      const opening = motion === false ? null : animateSurface(dialog, 'enter', {kind: motion, fromCurrent: wasOpen});
      clear(); dialog.classList.remove('closing');
      if (!dialog.open) dialog.showModal();
      if (opening) {animation.current = opening; observe(opening);}
      if (suspendedFocus.current?.isConnected && dialog.contains(suspendedFocus.current)) suspendedFocus.current.focus({preventScroll: true});
      suspendedFocus.current = null;
    } else {
      suspendedFocus.current = null;
      if (!dialog.open && lifetimeOpen.current) {lifetimeOpen.current = false; callback.current.onClosed?.();}
      close(false);
    }
  }, [open, suspended, close, closeNative, clear, motion, observe]);
  useEffect(() => {
    const dialog = ref.current;
    if (!dialog) return;
    const closed = () => {
      if (ownedCloseEvents.current) {ownedCloseEvents.current--; return;}
      if (dialog.open) return; // A queued event cannot close a newer opening.
      clear(); dialog.classList.remove('closing');
      if (lifetimeOpen.current) {lifetimeOpen.current = false; callback.current.onClosed?.();}
      if (callback.current.open && !callback.current.suspended) callback.current.onCloseRequest();
    };
    dialog.addEventListener('close', closed);
    return () => {
      clear(); dialog.removeEventListener('close', closed);
      // Capture the node before React clears its ref during deletion.
      if (dialog.open) dialog.close();
      ownedCloseEvents.current = 0;
    };
  }, [clear]);
  return {ref, finishPendingClose: () => finish.current?.(), cancelPendingClose: () => {clear(); ref.current?.classList.remove('closing');}, requestClose: () => close(true), requestImmediateClose: () => close(true, true),
    onCancel: (event: {preventDefault(): void}) => {event.preventDefault(); close(true);},
    onClick: (event: {target: EventTarget; currentTarget: EventTarget}) => {if (event.target === event.currentTarget) close(true);},
  };
}
