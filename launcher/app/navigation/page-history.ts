import {isProductId, isMultiplayerProductId, type ProductId} from '../../src/contracts/product-catalog.mts';
import {launcherBaseUrl} from './addresses';
import {isUiLocale, localeFromPath, localeHome, uiLocaleSearch} from '../../src/launcher/i18n.mts';
import {resolveRoomInvite, normalizeRoomCode, launcherHomeHistoryOperation, PLAYER_HISTORY_KEY, MP_ROOM_HISTORY_KEY, MP_PANEL_HISTORY_KEY, MP_SETTINGS_HISTORY_KEY} from '../../src/launcher/route-state.mts';

export type Page = 'launcher' | 'account' | 'profile' | 'directory' | 'single-options' | 'multi-options' | 'room';
export const PAGE_HISTORY_KEY = 'launcherPageHistory';
export const LOCAL_LAYERS_KEY = 'launcherLocalLayers';
export const PAGE_PARENTS: Readonly<Record<Page, Page | null>> = Object.freeze({
  launcher: null, account: 'launcher', profile: 'launcher', directory: 'launcher', 'single-options': 'launcher', 'multi-options': 'directory', room: 'directory',
});
export interface PageHistory {version: 1; position: number; page: Page; pagePosition: number; ancestors: Partial<Record<Page, number>>}
export interface PageLocation {pathname: string; search: string; hash: string; state?: unknown}
const object = (value: unknown): Record<string, unknown> => value && typeof value === 'object' && !Array.isArray(value) ? value as Record<string, unknown> : {};
export function readPageHistory(state: unknown): PageHistory | null {
  const entry = object(object(state)[PAGE_HISTORY_KEY]);
  if (entry.version !== 1 || !Number.isSafeInteger(entry.position) || Number(entry.position) < 0 ||
      !Number.isSafeInteger(entry.pagePosition) || Number(entry.pagePosition) < 0 || Number(entry.pagePosition) > Number(entry.position) ||
      typeof entry.page !== 'string' || !Object.hasOwn(PAGE_PARENTS, entry.page) ||
      !entry.ancestors || typeof entry.ancestors !== 'object' || Array.isArray(entry.ancestors)) return null;
  for (const [page, position] of Object.entries(entry.ancestors)) {
    if (!Object.hasOwn(PAGE_PARENTS, page) || !Number.isSafeInteger(position) || position < 0 || position > Number(entry.position)) return null;
  }
  return entry as unknown as PageHistory;
}
export function pageOf(location: PageLocation): Page {
  if (/\/login\.html\/?$/.test(location.pathname)) return 'account';
  if (/\/profile\.html\/?$/.test(location.pathname)) return 'profile';
  const state = object(location.state), options = object(state.launcherSurfaceEntry);
  if (state.launcherClosedPlayerSelection) return 'launcher';
  if (/\/(?:lobby|lobby\.html)\/?$/.test(location.pathname)) return options.surface === 'options' ? 'multi-options' : 'directory';
  if (normalizeRoomCode(resolveRoomInvite(new URL(location.pathname + location.search, 'https://launcher.invalid'))?.r)) return 'room';
  const game = new URLSearchParams(location.search).get('game');
  return game && isProductId(game) ? isMultiplayerProductId(game) ? 'multi-options' : 'single-options' : 'launcher';
}
export function pageAddress(location: PageLocation, page: Page, product?: ProductId | null, locale?: string): PageLocation {
  const home = new URL(launcherHomeHistoryOperation({currentUrl: new URL(location.pathname + location.search + location.hash, 'https://launcher.invalid'), currentState: {}}).url);
  const language = isUiLocale(locale) ? locale : localeFromPath(location.pathname);
  home.pathname = new URL(localeHome(language), launcherBaseUrl(home.href)).pathname;
  if (page === 'account') home.pathname = new URL('login.html', home).pathname;
  if (page === 'profile') home.pathname = new URL('profile.html', home).pathname;
  if (page === 'directory') {
    home.pathname = new URL('lobby.html', home).pathname;
    if (product && isMultiplayerProductId(product)) home.searchParams.set('game', product);
  }
  return {pathname: home.pathname, search: uiLocaleSearch(home.search, language, ['account', 'profile', 'directory'].includes(page)), hash: home.hash};
}
/** Only positions are retained, not a second route stack or inferred entry sources. */
export function nextPageHistory(current: PageHistory | null, page: Page, replace: boolean): PageHistory {
  const position = current ? current.position + (replace ? 0 : 1) : 0;
  const ancestors = {...current?.ancestors};
  if (current && current.page !== page) ancestors[current.page] = current.pagePosition;
  return {version: 1, position, page, pagePosition: current?.page === page ? current.pagePosition : position, ancestors};
}
export function parentDistance(current: PageHistory | null): number | null {
  if (!current) return null;
  const parent = PAGE_PARENTS[current.page], position = parent && current.ancestors[parent];
  return typeof position === 'number' && position < current.position ? position - current.position : null;
}
export function initialPageChain(location: PageLocation, product?: ProductId | null, locale?: string): Array<PageLocation & {state: Record<string, unknown>}> {
  const leaf = pageOf(location), pages: Page[] = leaf === 'launcher' ? [leaf]
    : leaf === 'account' || leaf === 'profile' || leaf === 'directory' || leaf === 'single-options' ? ['launcher', leaf] : ['launcher', 'directory', leaf];
  const parentState = {...object(location.state)};
  for (const key of [PAGE_HISTORY_KEY, LOCAL_LAYERS_KEY, PLAYER_HISTORY_KEY, MP_ROOM_HISTORY_KEY, MP_PANEL_HISTORY_KEY, MP_SETTINGS_HISTORY_KEY,
    'launcherSurfaceEntry', 'launcherTouchEntry', 'launcherInformationDialogs', 'launcherDirectoryForm', 'launcherRoomPanel',
    'launcherRoomSettings', 'launcherRoomProductSelection', 'launcherClosedPlayerSelection', 'launcherGameEntry']) delete parentState[key];
  let previous: PageHistory | null = null;
  return pages.map((page, index) => {
    const address = index === pages.length - 1 ? location : pageAddress(location, page, product, locale);
    previous = nextPageHistory(previous, page, false);
    return {...address, state: {...(index === pages.length - 1 ? object(location.state) : parentState), [PAGE_HISTORY_KEY]: previous}};
  });
}
