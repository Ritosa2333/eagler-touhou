export interface RuntimeMessageTarget {
  postMessage(message: unknown, targetOrigin: string): void;
  readonly __eaglerDirectInputBridge?: unknown;
}

interface ImmediateInputBridge {
  schema: "eagler-touhou/direct-input/1";
  protocol: string;
  game: string;
  epoch: number;
  origin: string;
  submit(message: unknown): boolean;
}

export interface TouchRuntimeContext {
  target: RuntimeMessageTarget | null;
  targetOrigin: string;
  protocol: string;
  game: string;
  epoch: number;
  launched: boolean;
  ready: boolean;
  spectator: boolean;
}

export interface TouchControlsSnapshot {
  fireEnabled: boolean;
  focusEnabled: boolean;
  bombSerial: number;
  escapeSerial: number;
  joystickX: number;
  joystickY: number;
}

export interface DirectTouchPoint {
  id: number;
  x: number;
  y: number;
}

export type DirectTouchType = "down" | "move" | "up";
export type ThpracMouseType = "move" | "down" | "up";

export interface HostedKeySpec {
  code: string;
  key: string;
  keyCode: number;
  location?: number;
}

// Optional same-origin input delivery; true means consumed exactly once,
// false means untouched. Never retry after a throwing or unknown outcome.
export function deliverRuntimeInput(
  context: Pick<TouchRuntimeContext, "target" | "targetOrigin" | "protocol" | "game" | "epoch">,
  message: unknown,
): boolean {
  if (!context.target || !Number.isSafeInteger(context.epoch) || context.epoch <= 0) return false;
  let candidate: unknown;
  try { candidate = context.target.__eaglerDirectInputBridge; }
  catch { /* Cross-origin Window lookup: preserve postMessage transport. */ }
  if (candidate && typeof candidate === "object") {
    const bridge = candidate as Partial<ImmediateInputBridge>;
    if (bridge.schema === "eagler-touhou/direct-input/1" &&
        bridge.protocol === context.protocol && bridge.game === context.game &&
        bridge.epoch === context.epoch &&
        bridge.origin === context.targetOrigin && typeof bridge.submit === "function") {
      const accepted = bridge.submit(message);
      if (accepted === true) return true;
      if (accepted !== false) throw new Error("invalid immediate-input acknowledgement");
    }
  }
  context.target.postMessage(message, context.targetOrigin);
  return true;
}

export function postHostedKey(
  context: Pick<TouchRuntimeContext, "target" | "targetOrigin" | "protocol" | "game" | "epoch" | "launched">,
  spec: HostedKeySpec,
  down: boolean,
): boolean {
  if (!context.launched || !context.target) return false;
  return deliverRuntimeInput(context, {
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "keyboard",
    down,
    code: spec.code,
    key: spec.key,
    keyCode: spec.keyCode,
    location: spec.location ?? 0,
  });
}

export function postTouchControls(
  context: TouchRuntimeContext,
  controls: TouchControlsSnapshot,
  touchSensitivity: number,
): boolean {
  if (!context.launched || !context.ready || !context.target || context.spectator) return false;
  return deliverRuntimeInput(context, {
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "touch-controls",
    ...controls,
    touchSensitivity,
  });
}

export function postDirectTouch(
  context: TouchRuntimeContext,
  type: DirectTouchType,
  touch: DirectTouchPoint | null,
): boolean {
  if (!context.launched || !context.ready || !context.target || !touch) return false;
  return deliverRuntimeInput(context, {
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "direct-touch",
    type,
    id: touch.id,
    x: touch.x,
    y: touch.y,
  });
}

export function postTouchCancel(context: TouchRuntimeContext): boolean {
  if (!context.launched || !context.ready || !context.target) return false;
  return deliverRuntimeInput(context, {
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "touch-cancel",
  });
}

export function postThpracMouse(
  context: Pick<TouchRuntimeContext, "target" | "targetOrigin" | "protocol" | "game" | "epoch">,
  type: ThpracMouseType,
  x: number,
  y: number,
): boolean {
  if (!context.target || !Number.isSafeInteger(context.epoch) || context.epoch <= 0 ||
      !Number.isFinite(x) || !Number.isFinite(y)) return false;
  context.target.postMessage({
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "thprac-mouse",
    type,
    x,
    y,
  }, context.targetOrigin);
  return true;
}
