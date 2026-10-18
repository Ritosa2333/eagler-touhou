export interface GameZoomPoint {
  x: number;
  y: number;
}

export interface GameZoomPointerInput {
  pointerId: number;
  pointerType: string;
  clientX: number;
  clientY: number;
  currentTarget: EventTarget | null;
}

interface GameZoomPinch {
  ids: [number, number];
  distance: number;
  scale: number;
  contentX: number;
  contentY: number;
}

export interface GameZoomControllerOptions {
  player: HTMLElement;
  frame: HTMLElement;
  viewport: HTMLElement;
  toggle: HTMLElement;
  toggleLabel: HTMLElement;
  scaleLabel: HTMLElement;
  directSurface: EventTarget;
  getBaseOffset: () => GameZoomPoint;
  getResetLabel: () => string;
  isAvailable: () => boolean;
  minScale?: number;
  maxScale?: number;
}

export interface GameZoomSnapshot {
  active: boolean;
  scale: number;
  x: number;
  y: number;
  pointerCount: number;
}

export function createGameZoomController(options: GameZoomControllerOptions) {
  const {
    player,
    frame,
    viewport,
    toggle,
    toggleLabel,
    scaleLabel,
    directSurface,
    getBaseOffset,
    getResetLabel,
    isAvailable,
  } = options;
  const minScale = Number.isFinite(options.minScale) ? Number(options.minScale) : 1;
  const maxScale = Number.isFinite(options.maxScale) ? Number(options.maxScale) : 3;
  if (!(minScale > 0) || !(maxScale >= minScale)) throw new Error("Invalid game zoom scale range");

  let inputWindow: Window | null = null;
  const state: {
    active: boolean;
    scale: number;
    x: number;
    y: number;
    pointers: Map<number, GameZoomPoint>;
    pinch: GameZoomPinch | null;
  } = { active: false, scale: 1, x: 0, y: 0, pointers: new Map(), pinch: null };

  const clampScale = (scale: number) => Math.max(minScale, Math.min(maxScale, scale));

  function clampTransform(scale: number, x: number, y: number) {
    const width = player.clientWidth;
    const height = player.clientHeight;
    if (!width || !height) return { scale, x: 0, y: 0 };
    if (scale < 1) {
      return { scale, x: (width - width * scale) / 2, y: (height - height * scale) / 2 };
    }
    return {
      scale,
      x: Math.max(width - width * scale, Math.min(0, x)),
      y: Math.max(height - height * scale, Math.min(0, y)),
    };
  }

  function refreshUi() {
    state.active = !!isAvailable();
    toggle.hidden = !state.active;
    toggle.classList.remove("is-on");
    toggleLabel.textContent = getResetLabel();
    scaleLabel.textContent = `${Math.round(state.scale * 100)}%`;
    player.classList.remove("game-zoom-editing");
  }

  function applyTransform(scale = state.scale, x = state.x, y = state.y) {
    const clamped = clampTransform(clampScale(scale), x, y);
    const base = getBaseOffset();
    state.scale = clamped.scale;
    state.x = clamped.x;
    state.y = clamped.y;
    viewport.style.transform = `translate3d(${base.x + clamped.x}px,${base.y + clamped.y}px,0) scale(${clamped.scale})`;
    refreshUi();
  }

  function cancelGesture() {
    state.pointers.clear();
    state.pinch = null;
  }

  function reset() {
    state.scale = 1;
    state.x = 0;
    state.y = 0;
    cancelGesture();
    applyTransform(1, 0, 0);
  }

  function beginPinch() {
    if (state.pointers.size < 2) {
      state.pinch = null;
      return;
    }
    const [first, second] = [...state.pointers.entries()].slice(0, 2);
    if (!first || !second) {
      state.pinch = null;
      return;
    }
    const a = first[1];
    const b = second[1];
    const rect = player.getBoundingClientRect();
    const midX = (a.x + b.x) / 2 - rect.left;
    const midY = (a.y + b.y) / 2 - rect.top;
    const distance = Math.max(1, Math.hypot(b.x - a.x, b.y - a.y));
    const base = getBaseOffset();
    state.pinch = {
      ids: [first[0], second[0]],
      distance,
      scale: state.scale,
      contentX: (midX - base.x - state.x) / state.scale,
      contentY: (midY - base.y - state.y) / state.scale,
    };
  }

  function pointerClientPoint(event: GameZoomPointerInput): GameZoomPoint {
    // Runtime-window PointerEvents are iframe-local. The host direct-touch
    // surface reports visual host coordinates, so undo the current iframe
    // transform before the common zoom mapping applies the scale again.
    if (event.currentTarget === directSurface) {
      const rect = frame.getBoundingClientRect();
      const scale = Math.max(1, state.scale);
      return {
        x: (event.clientX - rect.left) / scale,
        y: (event.clientY - rect.top) / scale,
      };
    }
    return { x: event.clientX, y: event.clientY };
  }

  function beginPointer(event: GameZoomPointerInput) {
    if (!state.active || event.pointerType === "mouse") return;
    const base = getBaseOffset();
    const point = pointerClientPoint(event);
    state.pointers.set(event.pointerId, {
      x: base.x + state.x + point.x * state.scale,
      y: base.y + state.y + point.y * state.scale,
    });
    if (state.pointers.size >= 2) beginPinch();
  }

  function movePointer(event: GameZoomPointerInput) {
    if (!state.active || !state.pointers.has(event.pointerId)) return;
    const base = getBaseOffset();
    const point = pointerClientPoint(event);
    state.pointers.set(event.pointerId, {
      x: base.x + state.x + point.x * state.scale,
      y: base.y + state.y + point.y * state.scale,
    });
    const pinch = state.pinch;
    if (!pinch || !pinch.ids.every(id => state.pointers.has(id))) return;
    const a = state.pointers.get(pinch.ids[0]);
    const b = state.pointers.get(pinch.ids[1]);
    if (!a || !b) return;
    const distance = Math.max(1, Math.hypot(b.x - a.x, b.y - a.y));
    const scale = clampScale(pinch.scale * distance / pinch.distance);
    const rect = player.getBoundingClientRect();
    const midX = (a.x + b.x) / 2 - rect.left;
    const midY = (a.y + b.y) / 2 - rect.top;
    applyTransform(scale, midX - base.x - pinch.contentX * scale, midY - base.y - pinch.contentY * scale);
  }

  function endPointer(event: GameZoomPointerInput) {
    if (!state.pointers.has(event.pointerId)) return;
    state.pointers.delete(event.pointerId);
    state.pinch = null;
    if (state.pointers.size >= 2) beginPinch();
  }

  function uninstallInputBridge() {
    if (inputWindow) {
      try { inputWindow.removeEventListener("pointerdown", beginPointer, true); } catch {}
      try { inputWindow.removeEventListener("pointermove", movePointer, true); } catch {}
      try { inputWindow.removeEventListener("pointerup", endPointer, true); } catch {}
      try { inputWindow.removeEventListener("pointercancel", endPointer, true); } catch {}
    }
    inputWindow = null;
    cancelGesture();
  }

  function bindInputWindow(win: Window | null) {
    // Iframe navigation can replace the underlying Window even when the
    // WindowProxy identity looks stable. Always detach and re-attach.
    uninstallInputBridge();
    if (!win) return;
    inputWindow = win;
    win.addEventListener("pointerdown", beginPointer, true);
    win.addEventListener("pointermove", movePointer, true);
    win.addEventListener("pointerup", endPointer, true);
    win.addEventListener("pointercancel", endPointer, true);
  }

  function snapshot(): GameZoomSnapshot {
    return Object.freeze({
      active: state.active,
      scale: state.scale,
      x: state.x,
      y: state.y,
      pointerCount: state.pointers.size,
    });
  }

  return Object.freeze({
    isActive: () => state.active,
    snapshot,
    refreshUi,
    applyTransform,
    reset,
    cancelGesture,
    beginPointer,
    movePointer,
    endPointer,
    bindInputWindow,
    uninstallInputBridge,
  });
}
