import {animate} from 'motion/mini';

export type SurfacePhase = 'enter' | 'exit';
export type SurfaceKind = 'panel' | 'dialog' | 'menu' | 'backdrop' | 'content';
export interface SurfaceAnimation {
  finished: Promise<void>;
  cancel(): void;
  complete(): void;
}
export interface SurfaceMotionOptions {
  kind?: SurfaceKind;
  /** Continue an interrupted entrance or exit from its actual appearance. */
  fromCurrent?: boolean;
  reduced?: boolean;
}

type Easing = [number, number, number, number];
const defaultEase: Easing = [.2, 0, 0, 1];
const distances: Record<SurfaceKind, number> = {panel: 0, dialog: 6, menu: 4, backdrop: 0, content: 0};
const active = new WeakMap<HTMLElement, Map<string, SurfaceAnimation>>();

export function surfaceMotionTokens(element: HTMLElement, kind: SurfaceKind = 'panel') {
  const style = element.ownerDocument.defaultView!.getComputedStyle(element);
  const rawDuration = style.getPropertyValue('--ui-surface-duration').trim();
  const parsedDuration = Number.parseFloat(rawDuration);
  const duration = Number.isFinite(parsedDuration)
    ? Math.max(0, parsedDuration * (rawDuration.endsWith('ms') ? 1 : 1000)) : 180;
  const rawEase = style.getPropertyValue('--ui-surface-easing').match(/cubic-bezier\(([^)]+)\)/);
  const curve = rawEase?.[1]?.split(',').map(Number);
  const ease = curve?.length === 4 && curve.every(Number.isFinite) && curve[0]! >= 0 && curve[0]! <= 1 && curve[2]! >= 0 && curve[2]! <= 1
    ? curve as Easing : defaultEase;
  const rawDistance = Number.parseFloat(style.getPropertyValue(`--ui-surface-distance-${kind}`));
  return {duration, ease, distance: Number.isFinite(rawDistance) ? rawDistance : distances[kind]};
}

/** One short-lived preference observer per document, regardless of surface count. */
function trackPreference(element: HTMLElement, animation: SurfaceAnimation) {
  const doc = element.ownerDocument;
  let scope = scopes.get(doc);
  if (!scope) {
    const view = doc.defaultView!;
    const media = view.matchMedia?.('(prefers-reduced-motion: reduce)');
    const animations = new Map<SurfaceAnimation, HTMLElement>();
    const changed = () => {
      for (const [control, node] of [...animations]) {
        if (media?.matches || node.closest('.less-motion')) control.complete();
      }
    };
    const observer = new view.MutationObserver(changed);
    observer.observe(doc.documentElement, {attributes: true, attributeFilter: ['class']});
    if (doc.body) observer.observe(doc.body, {attributes: true, attributeFilter: ['class']});
    media?.addEventListener('change', changed);
    scope = {animations, dispose() {observer.disconnect(); media?.removeEventListener('change', changed); scopes.delete(doc);}};
    scopes.set(doc, scope);
  }
  scope.animations.set(animation, element);
  return () => {
    scope.animations.delete(animation);
    if (!scope.animations.size) scope.dispose();
  };
}
const scopes = new WeakMap<Document, {animations: Map<SurfaceAnimation, HTMLElement>; dispose(): void}>();

/** Presentation only. The caller owns visibility, focus, navigation and retention. */
export function animateSurface(element: HTMLElement, phase: SurfacePhase, options: SurfaceMotionOptions = {}): SurfaceAnimation | null {
  const view = element.ownerDocument.defaultView;
  const previous = active.get(element)?.get('opacity');
  const current = previous && options.fromCurrent && view ? view.getComputedStyle(element) : null;
  // Copy computed values before stopping the previous effect changes its styles.
  const currentOpacity = current?.opacity;
  const currentTranslate = current?.translate;
  if (!view) {previous?.cancel(); return null;}
  const {distance} = surfaceMotionTokens(element, options.kind);
  const opening = phase === 'enter';
  const keyframes = {
    opacity: [currentOpacity || (opening ? options.kind === 'content' ? '.25' : '0' : '1'), opening ? '1' : '0'],
    ...(distance ? {translate: [currentTranslate && currentTranslate !== 'none' ? currentTranslate : opening ? `0 ${distance}px` : '0 0', opening ? '0 0' : `0 ${distance}px`]} : {}),
  };
  return animateUiFrames(element, keyframes, options);
}

/** Independent compositor properties can move a chat row while its expiry fades.
 * Layout, model deadlines and DOM retention stay with the caller. */
export function animateUiFrames(element: HTMLElement, keyframes: {opacity?: string[]; translate?: string[]}, options: Pick<SurfaceMotionOptions, 'kind' | 'reduced'> = {}): SurfaceAnimation | null {
  const properties = Object.keys(keyframes);
  const existing = active.get(element);
  for (const previous of new Set(properties.map(name => existing?.get(name)))) previous?.cancel();
  const view = element.ownerDocument.defaultView;
  if (!view || typeof element.animate !== 'function' || typeof element.getAnimations !== 'function' || options.reduced
    || view.matchMedia?.('(prefers-reduced-motion: reduce)').matches || element.closest('.less-motion')) return null;
  const {duration, ease} = surfaceMotionTokens(element, options.kind);
  if (!duration) return null;
  const original = properties.map(name => ({name, value: element.style.getPropertyValue(name), priority: element.style.getPropertyPriority(name)}));
  let controls: ReturnType<typeof animate>;
  const before = new Set(element.getAnimations());
  const restore = () => {for (const {name, value, priority} of original) element.style.setProperty(name, value, priority);};
  try {controls = animate([element], keyframes, {duration: duration / 1000, ease});}
  catch {
    for (const animation of element.getAnimations().filter(animation => !before.has(animation))) {
      void animation.finished?.catch(() => {});
      animation.onfinish = null; animation.cancel();
    }
    restore(); return null;
  }
  const native = element.getAnimations().filter(animation => !before.has(animation));
  if (!native.length || native.some(animation => !animation.finished?.then)) {
    for (const animation of native) animation.onfinish = null;
    controls.cancel(); controls.stop();
    restore();
    return null;
  }

  let settled = false;
  let resolveFinished!: () => void;
  let untrack = () => {};
  const finished = new Promise<void>(resolve => {resolveFinished = resolve;});
  const settle = () => {
    if (settled) return;
    settled = true;
    // Retire finish events first. Cancel before stop so Motion never commits
    // transient styles on a hidden node; the host restores its owned styles.
    for (const animation of native) animation.onfinish = null;
    controls.cancel();
    controls.stop();
    const owners = active.get(element);
    for (const {name, value, priority} of original) if (owners?.get(name) === animation) {
      owners.delete(name); element.style.setProperty(name, value, priority);
    }
    if (!owners?.size) active.delete(element);
    untrack();
    resolveFinished();
  };
  const animation: SurfaceAnimation = {
    finished,
    cancel: settle,
    complete() {if (!settled) {controls.complete(); settle();}},
  };
  const owners = active.get(element) ?? new Map<string, SurfaceAnimation>();
  for (const name of properties) owners.set(name, animation);
  active.set(element, owners);
  untrack = trackPreference(element, animation);
  // Native cancellation must also release the caller's retained presentation.
  void Promise.all(native.map(animation => animation.finished)).then(settle, settle);
  return animation;
}
