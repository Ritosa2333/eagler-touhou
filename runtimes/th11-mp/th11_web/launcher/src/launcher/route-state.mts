export const PLAYER_HISTORY_KEY = "eaglerTouhouPlayer";
export const MP_ROOM_HISTORY_KEY = "eaglerTouhouMpRoom";
export const MP_ROOM_URL_KEY = "mpRoom";

export type HistoryState = Record<string, unknown>;

export interface HistoryOperation {
  kind: "replace" | "push";
  state: HistoryState;
  url: string;
}

function historyState(value: unknown): HistoryState {
  return value !== null && typeof value === "object" && !Array.isArray(value)
    ? { ...(value as HistoryState) }
    : {};
}

export function normalizeRoomCode(value: unknown): string {
  return String(value || "").replace(/\D/g, "").slice(0, 8);
}

export function routedProductFromUrl<Product extends string>(
  source: string | URL,
  productIds: ReadonlySet<Product>,
): Product | null {
  const value = new URL(source).searchParams.get("game");
  return value !== null && productIds.has(value as Product) ? value as Product : null;
}

export function launcherHomeUrl(source: string | URL): URL {
  const url = new URL(source);
  url.searchParams.delete("game");
  url.searchParams.delete(MP_ROOM_URL_KEY);
  return url;
}

export function launcherHomeHistoryState(currentState: unknown): HistoryState {
  const next = historyState(currentState);
  next[PLAYER_HISTORY_KEY] = false;
  next[MP_ROOM_HISTORY_KEY] = false;
  delete next.game;
  return next;
}

export function launcherHomeHistoryOperation({
  currentUrl,
  currentState,
}: {
  currentUrl: string | URL;
  currentState: unknown;
}): HistoryOperation {
  return {
    kind: "replace",
    state: launcherHomeHistoryState(currentState),
    url: launcherHomeUrl(currentUrl).href,
  };
}

export function playerRouteUrl(source: string | URL, product: string): URL {
  const url = new URL(source);
  url.searchParams.set("game", product);
  return url;
}

export function roomRouteUrl(source: string | URL, product: string, roomCode: string): URL {
  const url = playerRouteUrl(source, product);
  url.searchParams.set(MP_ROOM_URL_KEY, roomCode);
  return url;
}

export function roomRouteHistoryOperation({
  currentUrl,
  currentState,
  product,
  roomCode,
  push = false,
}: {
  currentUrl: string | URL;
  currentState: unknown;
  product: string;
  roomCode: string;
  push?: boolean;
}): HistoryOperation {
  const url = new URL(currentUrl);
  if (roomCode) {
    url.searchParams.set(MP_ROOM_URL_KEY, roomCode);
    url.searchParams.set("game", product);
  } else {
    url.searchParams.delete(MP_ROOM_URL_KEY);
  }
  const nextState = historyState(currentState);
  nextState[MP_ROOM_HISTORY_KEY] = roomCode || false;
  return { kind: push ? "push" : "replace", state: nextState, url: url.href };
}

export function returnToRoomHistoryOperation({
  currentUrl,
  currentState,
  product,
  roomCode,
}: {
  currentUrl: string | URL;
  currentState: unknown;
  product: string;
  roomCode: string;
}): HistoryOperation {
  const url = roomRouteUrl(currentUrl, product, roomCode);
  const nextState = historyState(currentState);
  nextState[PLAYER_HISTORY_KEY] = false;
  nextState[MP_ROOM_HISTORY_KEY] = roomCode;
  nextState.game = product;
  return { kind: "replace", state: nextState, url: url.href };
}

export function initialRoutedHistoryOperations({
  currentUrl,
  currentState,
  routedProduct,
  navigationType,
  multiplayerProduct,
}: {
  currentUrl: string | URL;
  currentState: unknown;
  routedProduct: string;
  navigationType: string;
  multiplayerProduct: boolean;
}): HistoryOperation[] {
  if (navigationType === "reload") {
    const reloadUrl = new URL(currentUrl);
    const reloadRoom = normalizeRoomCode(reloadUrl.searchParams.get(MP_ROOM_URL_KEY));
    const reloadState = historyState(currentState);
    reloadState[PLAYER_HISTORY_KEY] = false;
    if (reloadRoom && multiplayerProduct) {
      reloadUrl.searchParams.set("game", routedProduct);
      reloadState.game = routedProduct;
      reloadState[MP_ROOM_HISTORY_KEY] = reloadRoom;
    } else {
      reloadUrl.searchParams.delete("game");
      delete reloadState.game;
    }
    return [{ kind: "replace", state: reloadState, url: reloadUrl.href }];
  }

  const previous = historyState(currentState);
  if (previous[PLAYER_HISTORY_KEY]) return [];
  const gameUrl = new URL(currentUrl);
  const homeUrl = new URL(currentUrl);
  homeUrl.searchParams.delete("game");
  return [
    {
      kind: "replace",
      state: { ...previous, [PLAYER_HISTORY_KEY]: false },
      url: homeUrl.href,
    },
    {
      kind: "push",
      state: { [PLAYER_HISTORY_KEY]: true, game: routedProduct },
      url: gameUrl.href,
    },
  ];
}

export function playerRouteHistoryOperation({
  currentUrl,
  currentState,
  routedProduct,
  product,
}: {
  currentUrl: string | URL;
  currentState: unknown;
  routedProduct: string | null;
  product: string;
}): HistoryOperation | null {
  const previous = historyState(currentState);
  const url = playerRouteUrl(currentUrl, product);
  const nextState = { ...previous, [PLAYER_HISTORY_KEY]: true, game: product };
  if (!previous[PLAYER_HISTORY_KEY]) return { kind: "push", state: nextState, url: url.href };
  if (routedProduct !== product || previous.game !== product) {
    return { kind: "replace", state: nextState, url: url.href };
  }
  return null;
}

export function directRoomHistorySeed({
  currentUrl,
  currentState,
  roomCode,
}: {
  currentUrl: string | URL;
  currentState: unknown;
  roomCode: string;
}): HistoryOperation[] {
  const previous = historyState(currentState);
  if (previous[MP_ROOM_HISTORY_KEY]) return [];
  const roomUrl = new URL(currentUrl);
  const homeState = launcherHomeHistoryState(previous);
  return [
    { kind: "replace", state: homeState, url: launcherHomeUrl(roomUrl).href },
    {
      kind: "push",
      state: { ...homeState, [MP_ROOM_HISTORY_KEY]: roomCode },
      url: roomUrl.href,
    },
  ];
}

export function applyHistoryOperations(
  historyObj: Pick<History, "replaceState" | "pushState">,
  operations: readonly HistoryOperation[],
): void {
  for (const operation of operations) {
    if (operation.kind === "replace") historyObj.replaceState(operation.state, "", operation.url);
    else historyObj.pushState(operation.state, "", operation.url);
  }
}
