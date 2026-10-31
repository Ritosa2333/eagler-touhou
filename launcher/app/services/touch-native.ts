import type { TouchEditorNativePorts } from '../components/settings/types';
import type { Translate } from '../i18n';
import type { TouchLayoutOrientation } from '../models/touch-layout';

type FullscreenDocument = Pick<Document, 'documentElement' | 'fullscreenElement' | 'exitFullscreen'> & {
  addEventListener?: Document['addEventListener'];
  removeEventListener?: Document['removeEventListener'];
  webkitFullscreenElement?: Element | null;
  webkitExitFullscreen?: () => void | Promise<void>;
};
type FullscreenElement = HTMLElement & { webkitRequestFullscreen?: () => void };
export interface TouchNativeScreen {
  orientation?: { type?: string; lock?(orientation: string): Promise<void>; unlock?(): void };
}
export interface TouchNativeController extends TouchEditorNativePorts {
  isFullscreen(): boolean;
  /** User Player toggle/close also exits an inherited document-root fullscreen. */
  exitPlayerFullscreen(): Promise<void>;
  probeOrientation(): Promise<void>;
  dispose(): Promise<void>;
}
/** The document host supplies native objects after hydration. This adapter
 * owns only fullscreen it acquired for its editor, never history or keyboard. */
export function createTouchNativePorts({
  document: documentLike,
  screen: screenLike,
  mobile,
  translate,
}: {
  document: FullscreenDocument;
  screen: TouchNativeScreen;
  mobile: boolean;
  translate: Translate;
}): TouchNativeController {
  let target: FullscreenElement | null = null,
    owned: FullscreenElement | null = null;
  let unsupported = false,
    checked = false,
    pendingOrientation = false,
    disposed = false;
  let entering: Promise<boolean> | null = null;
  const current = () => documentLike.fullscreenElement || documentLike.webkitFullscreenElement || null;
  const isFullscreen = () => (!!target && current() === target) || current() === documentLike.documentElement;
  const canSwitchOrientation = () =>
    !disposed && mobile && typeof screenLike.orientation?.lock === 'function' && !unsupported;
  async function exitBrowserFullscreen() {
    let resolveExit!: () => void;
    const exited = new Promise<void>((resolve) => {
      resolveExit = resolve;
    });
    const changed = () => {
      if (!current()) resolveExit();
    };
    documentLike.addEventListener?.('fullscreenchange', changed);
    documentLike.addEventListener?.('webkitfullscreenchange', changed);
    // Embedded browser hosts can update fullscreenElement without delivering
    // the completion event. Keep observing the native state until either form
    // of completion arrives, and retire the observer with the request.
    const observation = globalThis.setInterval(changed, 100);
    try {
      const request =
        typeof documentLike.exitFullscreen === 'function'
          ? documentLike.exitFullscreen()
          : documentLike.webkitExitFullscreen?.();
      // Retiring an iframe can finish the browser's fullscreen transition while
      // its exit request remains pending. The actual document state also proves
      // completion; it must not strand the already-closed game behind its cover.
      changed();
      await Promise.race([Promise.resolve(request), exited]);
    } finally {
      globalThis.clearInterval(observation);
      documentLike.removeEventListener?.('fullscreenchange', changed);
      documentLike.removeEventListener?.('webkitfullscreenchange', changed);
    }
  }
  async function exitFullscreen() {
    const previous = owned;
    owned = null;
    if (previous && current() === previous) await exitBrowserFullscreen();
  }
  async function exitPlayerFullscreen() {
    // Main5048–5058 accepts either the dedicated Player or the document root
    // for an explicit Player exit. Editor cleanup above remains ownership-only.
    owned = null;
    if (isFullscreen()) await exitBrowserFullscreen();
  }
  async function enterFullscreen(element: HTMLElement): Promise<boolean> {
    if (disposed) throw new Error(translate('fullscreen.unsupported'));
    target = element as FullscreenElement;
    if (isFullscreen()) return owned === target;
    if (entering) return entering;
    const requestTarget = target;
    entering = (async () => {
      if (current()) await exitBrowserFullscreen();
      if (typeof requestTarget.requestFullscreen === 'function')
        await requestTarget.requestFullscreen({ navigationUI: 'hide' });
      else if (typeof requestTarget.webkitRequestFullscreen === 'function') requestTarget.webkitRequestFullscreen();
      else throw new Error(translate('fullscreen.unsupported'));
      if (current() === requestTarget) owned = requestTarget;
      if (disposed) {
        await exitFullscreen();
        return false;
      }
      return owned === requestTarget;
    })();
    try {
      return await entering;
    } finally {
      entering = null;
    }
  }
  async function probeOrientation() {
    if (!canSwitchOrientation() || checked || pendingOrientation || !isFullscreen()) return;
    pendingOrientation = true;
    try {
      await screenLike.orientation!.lock!(screenLike.orientation!.type || 'landscape');
      screenLike.orientation!.unlock?.();
      checked = true;
    } catch (error) {
      if (error && typeof error === 'object' && 'name' in error && error.name === 'NotSupportedError')
        unsupported = true;
    } finally {
      pendingOrientation = false;
    }
  }
  return Object.freeze({
    enterFullscreen,
    exitFullscreen,
    exitPlayerFullscreen,
    isFullscreen,
    canSwitchOrientation,
    probeOrientation,
    async switchOrientation(orientation: TouchLayoutOrientation) {
      if (!canSwitchOrientation() || pendingOrientation) throw new Error(translate('touch.orientationUnsupported'));
      pendingOrientation = true;
      try {
        if (!target) throw new Error(translate('touch.previewUnavailable'));
        if (!isFullscreen()) await enterFullscreen(target);
        await screenLike.orientation!.lock!(orientation);
        checked = true;
      } catch (error) {
        if (error && typeof error === 'object' && 'name' in error && error.name === 'NotSupportedError')
          unsupported = true;
        throw error;
      } finally {
        pendingOrientation = false;
      }
    },
    async dispose() {
      disposed = true;
      await exitFullscreen();
      target = null;
    },
  });
}
