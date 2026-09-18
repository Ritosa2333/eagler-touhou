export interface RuntimeMessageTarget {
  postMessage(message: unknown, targetOrigin: string): void;
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

export function postHostedKey(
  context: Pick<TouchRuntimeContext, "target" | "targetOrigin" | "protocol" | "game" | "epoch" | "launched">,
  spec: HostedKeySpec,
  down: boolean,
): boolean {
  if (!context.launched || !context.target) return false;
  context.target.postMessage({
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "keyboard",
    down,
    code: spec.code,
    key: spec.key,
    keyCode: spec.keyCode,
    location: spec.location ?? 0,
  }, context.targetOrigin);
  return true;
}

export function postTouchControls(
  context: TouchRuntimeContext,
  controls: TouchControlsSnapshot,
  touchSensitivity: number,
): boolean {
  if (!context.launched || !context.ready || !context.target || context.spectator) return false;
  context.target.postMessage({
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "touch-controls",
    ...controls,
    touchSensitivity,
  }, context.targetOrigin);
  return true;
}

export function postDirectTouch(
  context: TouchRuntimeContext,
  type: DirectTouchType,
  touch: DirectTouchPoint | null,
): boolean {
  if (!context.launched || !context.ready || !context.target || !touch) return false;
  context.target.postMessage({
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "direct-touch",
    type,
    id: touch.id,
    x: touch.x,
    y: touch.y,
  }, context.targetOrigin);
  return true;
}

export function postTouchCancel(context: TouchRuntimeContext): boolean {
  if (!context.launched || !context.ready || !context.target) return false;
  context.target.postMessage({
    protocol: context.protocol,
    game: context.game,
    epoch: context.epoch,
    command: "touch-cancel",
  }, context.targetOrigin);
  return true;
}

export function postThpracMouse(
  context: Pick<TouchRuntimeContext, "target" | "targetOrigin" | "protocol" | "game" | "epoch">,
  type: ThpracMouseType,
  x: number,
  y: number,
): boolean {
  if (!context.target || !Number.isSafeInteger(context.epoch) || context.epoch <= 0) return false;
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
