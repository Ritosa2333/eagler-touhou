import {createContext, useCallback, useContext, useEffect, useLayoutEffect, useRef, useState, useSyncExternalStore, type ReactNode} from 'react';
import {useBlocker, useLocation, useNavigate, useNavigationType, type Location, type To, type NavigateOptions} from 'react-router';
import {initialPageChain, nextPageHistory, pageOf, pageAddress, parentDistance, readPageHistory, PAGE_HISTORY_KEY, PAGE_PARENTS, LOCAL_LAYERS_KEY} from './page-history';
import {isProductId, isMultiplayerProductId, DEFAULT_MULTIPLAYER_PRODUCT_ID, DEFAULT_PRODUCT_ID, gameIdForProduct, type ProductId} from '../../src/contracts/product-catalog.mts';
import {FullscreenTransient} from '../components/FullscreenTransient';
import {ConfirmationDialog} from '../components/ConfirmationDialog';
import {useLocale} from '../i18n';
import type {DecisionStore} from '../models/decisions';
import {resolveRoomInvite, normalizeRoomCode, roomRouteHistoryOperation, launcherHomeHistoryOperation, returnToRoomHistoryOperation, roomPanelHistoryOperation, roomSettingsHistoryOperation, MP_PANEL_HISTORY_KEY, MP_SETTINGS_HISTORY_KEY, MP_ROOM_HISTORY_KEY, PLAYER_HISTORY_KEY, type HistoryOperation} from '../../src/launcher/route-state.mts';
import {encodeRoomInvite, ROOM_INVITE_KEY} from '../../src/launcher/room-invite.mts';
import {localeHome, uiLocaleSearch, type UiLocale} from '../../src/launcher/i18n.mts';
import {updateSearchParameters} from '../../src/launcher/url-query.mts';
import {SurfaceLayersContext, type LayerEntry, type LayerValue} from './surface-layer';
import {UnsavedChangesContext, type UnsavedChangesGuard} from './unsaved-changes';
const noDecisionSubscription = () => () => {};
const noDecisionSnapshot = () => null;
let dialogSessionSequence = 0;
// UI lifetime provenance only, matching main; also works on insecure HTTP.
const newDialogSession = () => `${Date.now().toString(36)}-${++dialogSessionSequence}`;

export type Surface = 'library' | 'options' | 'touch' | 'room';
export type SurfaceContext = 'library' | 'lobby';
export type RoomPanelKind = 'personal' | 'network' | 'spectators' | 'game' | 'music';
export type DirectoryFormMode = 'create' | 'join';
export const INFORMATION_DIALOG_IDS = ['mpGuideDialog', 'firstUseNoticeDialog', 'donationDialog', 'appleRefreshDialog', 'replayDialog', 'lobbyNetworkDialog'] as const;
export type InformationDialogId = typeof INFORMATION_DIALOG_IDS[number];
export interface SurfaceAddress {
  surface: Surface;
  productId: ProductId | null;
  context: SurfaceContext;
  roomCode: string | null;
  fromDirectory: boolean;
}
export function readSurface(location: Pick<Location, 'pathname' | 'search'>): SurfaceAddress {
  const params = new URLSearchParams(location.search);
  const game = params.get('game') || '';
  const productId = isProductId(game) ? game : null;
  const context = /\/(?:lobby|lobby\.html)\/?$/.test(location.pathname) ? 'lobby' : 'library';
  const invite = context === 'library' ? resolveRoomInvite(new URL(location.pathname + location.search, 'https://launcher.invalid')) : null;
  const roomCode = invite ? normalizeRoomCode(invite.r) : '';
  if (invite && roomCode) return {productId: isMultiplayerProductId(invite.g) ? invite.g : DEFAULT_MULTIPLAYER_PRODUCT_ID, context, surface: 'room', roomCode, fromDirectory: invite.f === true};
  const options = productId !== null && context === 'library';
  return {productId, context, surface: options ? 'options' : 'library', roomCode: null, fromDirectory: false};
}
/** User-requested correction to main's room URL rewrite: preserve directory
 * origin only for the same raw product and normalized room identity. The
 * canonical operation still retires all one-shot create/join/settings fields.
 */
export function preserveSameRoomDirectoryOrigin(operation: HistoryOperation, source: string | URL): HistoryOperation {
  const prior = resolveRoomInvite(source), url = new URL(operation.url), next = resolveRoomInvite(url);
  if (!prior?.f || !next || prior.g !== next.g || !normalizeRoomCode(next.r) ||
      normalizeRoomCode(prior.r) !== normalizeRoomCode(next.r)) return operation;
  url.searchParams.set(ROOM_INVITE_KEY, encodeRoomInvite({g: next.g, r: next.r, f: true}));
  return {...operation, url: url.href};
}
const entryKey = 'launcherSurfaceEntry';
const touchEntryKey = 'launcherTouchEntry';
const infoEntryKey = 'launcherInformationDialogs';
const directoryFormKey = 'launcherDirectoryForm';
const roomPanelKey = 'launcherRoomPanel';
const roomSettingsKey = 'launcherRoomSettings';
const roomProductKey = 'launcherRoomProductSelection';
const directoryFiltersKey = 'launcherRoomDirectoryFilters';
const hiddenSelectionKey = 'launcherClosedPlayerSelection';
const gameEntryKey = 'launcherGameEntry';
const roomFullscreenKey = 'launcherRoomFullscreen';
const href = (location: Pick<Location, 'pathname' | 'search' | 'hash'>) => location.pathname + location.search + location.hash;
function plainState(value: unknown): Record<string, unknown> {
  return value && typeof value === 'object' && !Array.isArray(value) ? {...value} : {};
}
function localLayerTransition(current: Location, next: Location) {
  if (href(current) !== href(next)) return false;
  const before = plainState(current.state), after = plainState(next.state);
  if (JSON.stringify(before[LOCAL_LAYERS_KEY]) === JSON.stringify(after[LOCAL_LAYERS_KEY])) return false;
  for (const state of [before, after]) {delete state[LOCAL_LAYERS_KEY]; delete state[PAGE_HISTORY_KEY];}
  return JSON.stringify(before) === JSON.stringify(after);
}
function withoutEntry(value: unknown) {
  const state = plainState(value);
  delete state[entryKey];
  delete state[touchEntryKey];
  delete state[infoEntryKey];
  delete state[directoryFormKey];
  delete state[roomPanelKey];
  delete state[roomSettingsKey];
  delete state[roomProductKey];
  delete state[hiddenSelectionKey];
  delete state[gameEntryKey];
  delete state[roomFullscreenKey];
  delete state[LOCAL_LAYERS_KEY];
  return state;
}
export interface HostSelection {productId: ProductId; hasSelection: boolean}
interface SurfaceNavigation extends SurfaceAddress {
  hasSelection: boolean;
  routedProductId: ProductId | null;
  applyHostSelection(selection: HostSelection): void;
  syncSelectionFromRoute(): ProductId | null;
  filterProductId: ProductId | null;
  navigateAddress(address: string): void;
  openOptions(productId: ProductId): void;
  selectLobbyProduct(productId: ProductId | ''): void;
  openDirectory(productId: ProductId): void;
  setLocaleAddress(locale: UiLocale): void;
  restoreRoomRoute(input: {product: ProductId; code: string; fromDirectory: boolean}): void;
  enterRoomRoute(input: {product: ProductId; code: string}): void;
  settleRoomInvite(): void;
  leaveTitleRoomRoute(baseProduct: ProductId): void;
  leaveRoomRoute(input: {product: ProductId; fromDirectory: boolean; message?: string}): 'options' | 'directory';
  roomPanel: RoomPanelKind | null;
  roomNetworkPeer: string | undefined;
  openRoomPanel(kind: RoomPanelKind, peer?: string): void;
  closeRoomPanel(): void;
  roomSettingsOpen: boolean;
  openRoomSettings(): void;
  closeRoomSettings(): void;
  directoryFormMode: DirectoryFormMode | null;
  /** Existing history owner of the form, used to reject superseded completion. */
  directoryFormParentKey: string | null;
  openDirectoryForm(mode: DirectoryFormMode): void;
  closeDirectoryForm(): void;
  openTouchLayout(): void;
  playerOpen: boolean;
  openPlayer(productId: ProductId, options?: {dismissInfo?: boolean}): void;
  openRoomFullscreen(): void;
  closeRoomFullscreen(): Promise<void>;
  closeSurface(): void;
  completeRuntimeClose(context?: {titleOverlayWasOpen?: boolean}): void;
  infoDialogOpen(id: InformationDialogId): boolean;
  openInfoDialog(id: InformationDialogId, options?: {replace?: boolean}): void;
  closeInfoDialog(id?: InformationDialogId): void;
}
const NavigationContext = createContext<SurfaceNavigation | null>(null);
export function useSurfaceNavigation(): SurfaceNavigation {
  const value = useContext(NavigationContext);
  if (!value) throw new Error('SurfaceNavigationProvider is required');
  return value;
}

/** Sole UI history owner. State is immediate-entry provenance, never a second stack. */
export function SurfaceNavigationProvider({dirty, isEditing, onDiscard, decisions, playerOpen = false, requestRuntimeClose, requestReplayClose, requestRoomLeave, requestRoomFullscreenExit, titleOverlayOpen = false, productAvailable = () => true, onRouteSelection, children}: {
  dirty: boolean;
  isEditing: boolean;
  onDiscard(): void;
  decisions?: DecisionStore;
  playerOpen?: boolean;
  requestRuntimeClose?(): Promise<boolean>;
  requestReplayClose?(): Promise<boolean>;
  requestRoomLeave?(): void;
  requestRoomFullscreenExit?(): Promise<void>;
  titleOverlayOpen?: boolean;
  productAvailable?(productId: ProductId): boolean;
  onRouteSelection?(productId: ProductId, context?: SurfaceContext): void;
  children: ReactNode;
}) {
  const {t, locale} = useLocale();
  const [unsavedGuard, setUnsavedGuard] = useState<UnsavedChangesGuard | null>(null);
  const registerUnsavedGuard = useCallback((guard: UnsavedChangesGuard) => {
    setUnsavedGuard(guard);
    return () => setUnsavedGuard(current => current === guard ? null : current);
  }, []);
  const confirmDiscard = async () => {
    if (!unsavedGuard) return true;
    const confirmed = await decisions?.askConfirmation({
      title: t('profile.unsaved.title'), message: t('profile.unsaved.message'),
      confirmText: t('touch.discardChanges'), cancelText: t('career.continue'), tone: 'danger',
    });
    if (!confirmed) return false;
    unsavedGuard.discard();
    return true;
  };
  const applicationDecision = useSyncExternalStore(decisions?.subscribe ?? noDecisionSubscription,
    decisions?.getSnapshot ?? noDecisionSnapshot, decisions?.getSnapshot ?? noDecisionSnapshot);
  const location = useLocation();
  const routerNavigate = useNavigate();
  // Dialog settlement can still call the previous binding during layout.
  // Number the new entry from the current Router location in that case.
  const currentLocation = useRef(location); currentLocation.current = location;
  const navigate = useCallback((to: To | number, options?: NavigateOptions) => {
    if (typeof to === 'number') return routerNavigate(to);
    const source = currentLocation.current;
    const next = typeof to === 'string' ? new URL(to, new URL(href(source), 'https://launcher.invalid'))
      : {pathname: to.pathname ?? source.pathname, search: to.search ?? '', hash: to.hash ?? ''};
    const nextPage = pageOf({pathname: next.pathname, search: next.search, hash: next.hash, state: options?.state});
    const state = plainState(options?.state);
    if (nextPage === 'room') {
      if (readSurface(source).context === 'lobby') {
        const params = new URLSearchParams(source.search);
        state[directoryFiltersKey] = [...new Set((params.get('game') || params.get('games') || '').split(','))].filter(isMultiplayerProductId);
      } else if (pageOf(source) === 'room' && directoryFiltersKey in plainState(source.state)) {
        state[directoryFiltersKey] = plainState(source.state)[directoryFiltersKey];
      }
    }
    const record = nextPageHistory(readPageHistory(source.state), nextPage, options?.replace === true);
    return routerNavigate(to, {...options, state: {...state, [PAGE_HISTORY_KEY]: record}});
  }, [routerNavigate]);
  const seeded = useRef(false);
  const navigationType = useNavigationType();
  const pendingLocaleAddress = useRef<{sourceKey: string; target: string} | null>(null);
  const [dialogSession] = useState(newDialogSession);
  const localEntry = plainState(plainState(location.state)[LOCAL_LAYERS_KEY]);
  const validLocalLayers = localEntry.session === dialogSession && localEntry.href === href(location);
  const localValues = validLocalLayers ? plainState(localEntry.values) as Record<string, LayerEntry> : {};
  const staleLocalLayers = LOCAL_LAYERS_KEY in plainState(location.state) && !validLocalLayers;
  const infoEntry = plainState(plainState(location.state)[infoEntryKey]);
  const activeInfo = INFORMATION_DIALOG_IDS.includes(infoEntry.active as InformationDialogId)
    ? infoEntry.active as InformationDialogId : null;
  const infoVisible = plainState(infoEntry.visible);
  const validInfo = infoEntry.session === dialogSession && infoEntry.href === href(location) &&
    activeInfo !== null && infoVisible[activeInfo] === true;
  const staleInfo = Object.keys(infoEntry).length > 0 && !validInfo;
  function resolvedSurface(value: Location): SurfaceAddress {
    const parsed = readSurface(value), hidden = plainState(plainState(value.state)[hiddenSelectionKey]);
    if (hidden.session === dialogSession) return {...parsed, surface: 'library', roomCode: null, fromDirectory: false,
      productId: typeof hidden.productId === 'string' && isProductId(hidden.productId) ? hidden.productId : parsed.productId};
    const selected = plainState(plainState(value.state)[roomProductKey]);
    return parsed.surface === 'room' && selected.session === dialogSession && selected.code === parsed.roomCode &&
      selected.requestedProduct === resolveRoomInvite(new URL(href(value), 'https://launcher.invalid'))?.g && typeof selected.productId === 'string' && isMultiplayerProductId(selected.productId)
      ? {...parsed, productId: selected.productId} : parsed;
  }
  const urlAddress = resolvedSurface(location);
  // main1499–1533 keeps business selection independent of the raw address.
  // This is one document-lived value, never a set of history-entry receipts.
  const [hostSelection, setHostSelection] = useState<HostSelection | null>(null);
  const selectionLocation = useRef(location);
  const previousSelectedProduct = useRef<ProductId>(urlAddress.productId && productAvailable(urlAddress.productId) ? urlAddress.productId : DEFAULT_PRODUCT_ID);
  const previousSurface = useRef<Surface>(urlAddress.surface);
  const locationChanged = selectionLocation.current.key !== location.key;
  const previousState = plainState(selectionLocation.current.state), nextState = plainState(location.state);
  const sameAddress = href(selectionLocation.current) === href(location);
  const overlayTransition = sameAddress && (
    previousSurface.current === 'touch' || (navigationType === 'PUSH' && touchEntryKey in nextState) ||
    [infoEntryKey, roomPanelKey, roomSettingsKey, directoryFormKey, gameEntryKey, LOCAL_LAYERS_KEY].some(key =>
      JSON.stringify(previousState[key]) !== JSON.stringify(nextState[key])));
  const localeTransition = pendingLocaleAddress.current?.sourceKey === selectionLocation.current.key &&
    pendingLocaleAddress.current.target === href(location);
  const retainHostSelection = !locationChanged || overlayTransition || localeTransition;
  const activeHostSelection = retainHostSelection ? hostSelection : null;
  // biome-ignore lint/correctness/useExhaustiveDependencies: Commit selection once per Router entry; Host metadata and callback renders must not replay a navigation intent.
  useLayoutEffect(() => {
    if (!locationChanged) return;
    selectionLocation.current = location;
    if (!retainHostSelection) {
      setHostSelection(null);
      const owner = plainState(nextState[entryKey]);
      const selected = urlAddress.context === 'lobby' && owner.surface === 'options' && isProductId(String(owner.productId))
        ? owner.productId as ProductId : urlAddress.productId;
      const freshLobbyOptions = urlAddress.context === 'lobby' && owner.surface === 'options' &&
        JSON.stringify(previousState[entryKey]) !== JSON.stringify(nextState[entryKey]);
      if (selected && productAvailable(selected) && (freshLobbyOptions || selected !== previousSelectedProduct.current))
        onRouteSelection?.(selected, urlAddress.context);
    }
  }, [location.key]);
  // Current visible layer lifetime, not a list of visited entries. Original
  // popstate dismisses these windows but never opens retired ones on Forward.
  const [panelLifetime, setPanelLifetime] = useState<string | null>(null);
  const [settingsLifetime, setSettingsLifetime] = useState<string | null>(null);
  const panelEntry = plainState(plainState(location.state)[roomPanelKey]);
  const settingsEntry = plainState(plainState(location.state)[roomSettingsKey]);
  const selectedRoomProduct = activeHostSelection?.productId ?? urlAddress.productId;
  const selectedRoomVisible = selectedRoomProduct !== null && isMultiplayerProductId(selectedRoomProduct);
  const validPanel = urlAddress.surface === 'room' && panelLifetime !== null && panelEntry.lifetime === panelLifetime &&
    panelEntry.session === dialogSession && panelEntry.href === href(location) &&
    ['personal', 'network', 'spectators', 'game', 'music'].includes(String(panelEntry.kind));
  const validSettings = urlAddress.surface === 'room' && settingsLifetime !== null && settingsEntry.lifetime === settingsLifetime &&
    settingsEntry.session === dialogSession && settingsEntry.href === href(location);
  const roomPanel = validPanel && selectedRoomVisible ? panelEntry.kind as RoomPanelKind : null;
  const roomSettingsOpen = validSettings && selectedRoomVisible;
  const stalePanel = Object.keys(panelEntry).length > 0 && !validPanel;
  const staleSettings = Object.keys(settingsEntry).length > 0 && !validSettings;
  useEffect(() => {
    if (!validPanel && !stalePanel) setPanelLifetime(null);
    if (!validSettings && !staleSettings) setSettingsLifetime(null);
  }, [validPanel, stalePanel, validSettings, staleSettings]);
  const formEntry = plainState(plainState(location.state)[directoryFormKey]);
  const validForm = urlAddress.context === 'lobby' && formEntry.session === dialogSession &&
    formEntry.href === href(location) && (formEntry.mode === 'create' || formEntry.mode === 'join');
  const directoryFormMode = validForm ? formEntry.mode as DirectoryFormMode : null;
  const retiredForm = Object.keys(formEntry).length > 0 && !validForm;
  const optionEntry = plainState(plainState(location.state)[entryKey]);
  // main lobby.mts58–65 retires its options sheet on a document reload.
  // Library ?game remains a durable options address.
  const retiredLobbyOptions = urlAddress.context === 'lobby' && optionEntry.surface === 'options' &&
    optionEntry.session !== dialogSession;
  const selectionHidden = plainState(plainState(location.state)[hiddenSelectionKey]).session === dialogSession;
  const staleHidden = hiddenSelectionKey in plainState(location.state) && !selectionHidden;
  const ownedOptions = !selectionHidden && !retiredLobbyOptions && optionEntry.version === 1 && optionEntry.surface === 'options' &&
    optionEntry.href === href(location) && typeof optionEntry.productId === 'string' && isProductId(optionEntry.productId);
  const baseAddress: SurfaceAddress = ownedOptions ? {...urlAddress, surface: 'options', productId: optionEntry.productId as ProductId, roomCode: null, fromDirectory: false} : urlAddress;
  const currentEntry = plainState(plainState(location.state)[touchEntryKey]);
  // main7185 permits the retained hidden editor action from launcher home.
  // The child owns its marker, not an invented Options prerequisite.
  const ownedTouch = currentEntry.version === 1 &&
    currentEntry.surface === 'touch' && currentEntry.href === href(location);
  // Main's touch editor is a live draft, not a bookmark. Forward/reload of a
  // retired same-URL entry must never recreate it.
  const staleTouch = ownedTouch && navigationType === 'POP' && !isEditing;
  const routeHasSelection = baseAddress.surface !== 'library' &&
    !!baseAddress.productId && productAvailable(baseAddress.productId);
  const hasSelection = activeHostSelection?.hasSelection ?? routeHasSelection;
  const selectedProductId = activeHostSelection?.productId ?? (baseAddress.productId &&
    (baseAddress.surface === 'room' || productAvailable(baseAddress.productId)) ? baseAddress.productId : previousSelectedProduct.current);
  const roomVisible = baseAddress.roomCode !== null && selectedProductId !== null && isMultiplayerProductId(selectedProductId);
  const address: SurfaceAddress = {...baseAddress, productId: selectedProductId,
    surface: ownedTouch && !staleTouch ? 'touch' : roomVisible ? 'room' : hasSelection ? 'options' : 'library'};
  useLayoutEffect(() => {previousSurface.current = address.surface; if (address.productId) previousSelectedProduct.current = address.productId;}, [address.surface, address.productId]);
  function applyHostSelection(selection: HostSelection) {
    setHostSelection(previous => previous?.productId === selection.productId && previous.hasSelection === selection.hasSelection ? previous : {...selection});
  }
  function syncSelectionFromRoute(): ProductId | null {
    // Main lobby.mts434–450/795–800 keeps ?game as its directory filter;
    // only the explicit options carrier selects a Launcher settings surface.
    if (baseAddress.context === 'lobby' && !ownedOptions) return null;
    const productId = baseAddress.productId;
    if (!productId || !productAvailable(productId)) return null;
    applyHostSelection({productId, hasSelection: true});
    if (productId !== address.productId) onRouteSelection?.(productId, address.context);
    return productId;
  }
  const retiredGameEntry = navigationType === 'POP' && !playerOpen && gameEntryKey in plainState(location.state);
  useEffect(() => {
    if (pendingRoomReturn.current?.destination === location.key || pendingRoomReturn.current?.appliedKey === location.key) return;
    const params = new URLSearchParams(location.search);
    const obsoleteTouchQuery = params.has('touchLayout');
    if (!staleTouch && !staleHidden && !staleInfo && !retiredLobbyOptions && !retiredForm && !stalePanel && !staleSettings && !retiredGameEntry && !staleLocalLayers && !obsoleteTouchQuery) return;
    params.delete('touchLayout');
    const normalizedSearch = params.toString();
    const search = obsoleteTouchQuery ? (normalizedSearch ? `?${normalizedSearch}` : '') : location.search;
    void navigate({pathname: location.pathname, search, hash: location.hash},
      {replace: true, preventScrollReset: true, state: (() => {
        const state = plainState(location.state);
        if (staleTouch || retiredLobbyOptions || obsoleteTouchQuery) delete state[touchEntryKey];
        if (retiredLobbyOptions) delete state[entryKey];
        if (staleInfo) delete state[infoEntryKey];
        if (staleHidden) delete state[hiddenSelectionKey];
        if (retiredForm) delete state[directoryFormKey];
        if (retiredGameEntry) delete state[gameEntryKey];
        if (staleLocalLayers) delete state[LOCAL_LAYERS_KEY];
        if (stalePanel) {delete state[roomPanelKey]; state[MP_PANEL_HISTORY_KEY] = false;}
        if (staleSettings) {delete state[roomSettingsKey]; state[MP_SETTINGS_HISTORY_KEY] = false; delete state[touchEntryKey];}
        return state;
      })()});
  }, [staleTouch, staleHidden, staleInfo, retiredLobbyOptions, retiredForm, stalePanel, staleSettings, retiredGameEntry, staleLocalLayers, location, navigate]);
  const pendingBlockKind = useRef<'touch' | 'unsaved' | 'runtime' | 'room' | 'page' | 'room-fullscreen' | 'replay' | null>(null);
  const roomPolicyNavigation = useRef(false);
  // Browser POP arrives later than cleanup, unlike a synchronous MemoryRouter.
  // Authorize only the exact cleanup destination until that event arrives.
  const pendingPolicyPop = useRef<number | null>(null);
  function navigateAfterCleanup(distance: number) {
    const current = readPageHistory(location.state);
    if (current) pendingPolicyPop.current = current.position + distance;
    void navigate(distance);
  }
  // biome-ignore lint/correctness/useExhaustiveDependencies: Seed physical Back parents from this document's initial address exactly once, before later Router and Host updates.
  useEffect(() => {
    if (seeded.current) return;
    seeded.current = true;
    const initialState = plainState(location.state);
    if (staleTouch || new URLSearchParams(location.search).has('touchLayout')) delete initialState[touchEntryKey];
    if (retiredLobbyOptions) delete initialState[entryKey];
    if (staleInfo) delete initialState[infoEntryKey];
    if (staleHidden) delete initialState[hiddenSelectionKey];
    if (retiredGameEntry) delete initialState[gameEntryKey];
    if (staleLocalLayers) delete initialState[LOCAL_LAYERS_KEY];
    if (retiredForm) delete initialState[directoryFormKey];
    if (stalePanel) {delete initialState[roomPanelKey]; initialState[MP_PANEL_HISTORY_KEY] = false;}
    if (staleSettings) {delete initialState[roomSettingsKey]; initialState[MP_SETTINGS_HISTORY_KEY] = false;}
    const existing = readPageHistory(location.state);
    if (existing && existing.page === pageOf({...location, state: initialState})) return;
    const chain = initialPageChain({...location, state: initialState}, address.productId, locale);
    // A direct room URL receives the same physical parents as a directory visit.
    // Capture the address before transient parent renders; room restore is independent.
    roomPolicyNavigation.current = true;
    void (async () => {
      try {
        for (let index = 0; index < chain.length; index++) {
          const entry = chain[index];
          if (index === chain.length - 1 && address.roomCode) entry.state[MP_ROOM_HISTORY_KEY] = address.roomCode;
          await routerNavigate({pathname: entry.pathname, search: entry.search, hash: entry.hash}, {replace: index === 0, state: entry.state, preventScrollReset: true});
        }
      } finally {roomPolicyNavigation.current = false;}
    })();
  }, []);
  const manualRoomPlayerExit = useRef(false);
  const pageHistory = readPageHistory(location.state);
  // Runtime launch and retirement replace the iframe's current child entry,
  // leaving BrowserHistory Back ownership with the parent Router.
  const gameEntry = plainState(plainState(location.state)[gameEntryKey]);
  const roomFullscreen = plainState(plainState(location.state)[roomFullscreenKey]);
  const ownsRoomFullscreen = address.roomCode !== null && roomFullscreen.session === dialogSession && roomFullscreen.href === href(location);
  const fullscreenCloseTask = useRef<{promise: Promise<void>; resolve(): void} | null>(null);
  useLayoutEffect(() => {
    if (!ownsRoomFullscreen) {fullscreenCloseTask.current?.resolve();fullscreenCloseTask.current = null;}
  }, [ownsRoomFullscreen]);
  useEffect(() => () => {fullscreenCloseTask.current?.resolve();fullscreenCloseTask.current = null;}, []);
  function openRoomFullscreen() {
    if (address.roomCode === null || ownsRoomFullscreen) return;
    void navigate({pathname:location.pathname,search:location.search,hash:location.hash},{preventScrollReset:true,
      state:{...plainState(location.state),[roomFullscreenKey]:{session:dialogSession,href:href(location)}}});
  }
  function closeRoomFullscreen(): Promise<void> {
    if (fullscreenCloseTask.current) return fullscreenCloseTask.current.promise;
    if (!ownsRoomFullscreen) return Promise.resolve();
    let resolve!: () => void;
    const promise = new Promise<void>(complete => {resolve = complete;});
    fullscreenCloseTask.current = {promise,resolve};
    if (blocker.state === 'unblocked') void navigate(-1);
    return promise;
  }
  const topGame = playerOpen && typeof gameEntry.position === 'number' && gameEntry.position === pageHistory?.position;
  const pendingDirectorySelection = useRef<{position: number; product: ProductId} | null>(null);
  function returnToPageParent(parentProduct?: ProductId) {
    const history = readPageHistory(location.state), parent = history && PAGE_PARENTS[history.page];
    if (!parent) return;
    const distance = parentDistance(history);
    roomPolicyNavigation.current = true;
    try {
      if (distance !== null) {
        if (parent === 'directory' && parentProduct) pendingDirectorySelection.current = {position: history!.position + distance, product: parentProduct};
        navigateAfterCleanup(distance);
      }
      else {
        const next=pageAddress(location,parent,parentProduct??address.productId,locale);
        void navigate(next,{replace:true,state:{},preventScrollReset:next.pathname===location.pathname});
      }
    } finally {roomPolicyNavigation.current = false;}
  }
  // biome-ignore lint/correctness/useExhaustiveDependencies: This pending POP operation belongs to one committed Router entry, never a metadata or callback render.
  useLayoutEffect(() => {
    const pending = pendingDirectorySelection.current;
    if (!pending || pageHistory?.page !== 'directory' || pageHistory.position !== pending.position) return;
    pendingDirectorySelection.current = null;
    const params = new URLSearchParams(location.search);
    if (params.get('game') === pending.product) return;
    params.set('game', pending.product);
    roomPolicyNavigation.current = true;
    try {void navigate({pathname: location.pathname, search: `?${params}`, hash: location.hash},
      {replace: true, state: location.state, preventScrollReset: true});} finally {roomPolicyNavigation.current = false;}
  }, [location.key]);
  const pendingHiddenSelection = useRef<{destination: string; productId: ProductId | null} | null>(null);
  const pendingRoomReturn = useRef<{destination?: string; position?: number; operation: HistoryOperation; appliedKey?: string} | null>(null);
  const blocker = useBlocker(({currentLocation, nextLocation, historyAction}) => {
    if (historyAction === 'POP' && pendingPolicyPop.current !== null && readPageHistory(nextLocation.state)?.position === pendingPolicyPop.current) {
      pendingPolicyPop.current = null; return false;
    }
    if (roomPolicyNavigation.current || currentLocation.key === nextLocation.key) return false;
    // Child-window Back never tears down its enclosing editor, Runtime or room.
    if (localLayerTransition(currentLocation, nextLocation)) return false;
    if (unsavedGuard?.shouldBlock({currentLocation, nextLocation, historyAction})) {
      pendingBlockKind.current = 'unsaved';
      return true;
    }
    if (selectionHidden && historyAction === 'POP' && (validInfo || plainState(plainState(nextLocation.state)[infoEntryKey]).session === dialogSession)) {
      pendingHiddenSelection.current = {destination: nextLocation.key, productId: address.productId};
    }
    // Replay owns its Back intent. Settle its close policy while the manager
    // owns the current entry, even if an ordinary Runtime remains underneath.
    if (historyAction === 'POP' && validInfo && activeInfo === 'replayDialog') {
      pendingBlockKind.current = 'replay';
      return true;
    }
    const nextInformation = plainState(plainState(nextLocation.state)[infoEntryKey]);
    if (pendingLocaleAddress.current?.sourceKey === currentLocation.key &&
        pendingLocaleAddress.current.target === href(nextLocation)) return false;
    const nextTouch = plainState(plainState(nextLocation.state)[touchEntryKey]);
    const nextAddress = resolvedSurface(nextLocation);
    const nextOptionsOwner = plainState(plainState(nextLocation.state)[entryKey]);
    const nextOwnedProduct = nextOptionsOwner.version === 1 && nextOptionsOwner.href === href(nextLocation)
      ? nextOptionsOwner.productId : nextAddress.productId;
    const sameTouch = nextTouch.version === currentEntry.version && nextTouch.surface === 'touch' &&
      nextTouch.parentKey === currentEntry.parentKey && nextTouch.href === href(nextLocation) &&
      currentEntry.href === href(currentLocation) && nextAddress.context === address.context &&
      nextOwnedProduct === baseAddress.productId;
    if (dirty && isEditing && ownedTouch && !sameTouch) {
      pendingBlockKind.current = 'touch';
      return true;
    }
    const sameRoom = address.roomCode !== null && nextAddress.roomCode === address.roomCode;
    // main5339–5363 gives live child dismissals priority, but a POP into a
    // retired same-room entry still closes the running MP Player.
    const roomChildConsumesPop = !topGame && (ownedTouch || validInfo ||
      nextInformation.session === dialogSession ||
      (validSettings && !plainState(nextLocation.state)[MP_SETTINGS_HISTORY_KEY]) ||
      (validPanel && !plainState(nextLocation.state)[MP_PANEL_HISTORY_KEY]));
    const sameRoomPlayerPop = historyAction === 'POP' && sameRoom && !roomChildConsumesPop;
    if (historyAction === 'POP' && ownsRoomFullscreen && !roomChildConsumesPop) {
      pendingBlockKind.current = 'room-fullscreen';
      return true;
    }
    if (titleOverlayOpen && address.surface === 'room' && (!sameRoom || sameRoomPlayerPop) && !manualRoomPlayerExit.current) {
      pendingBlockKind.current = 'room';
      return true;
    }
    const openingTouch = nextTouch.version === 1 && nextTouch.surface === 'touch' &&
      nextTouch.parentKey === currentLocation.key && nextTouch.href === href(nextLocation) &&
      href(currentLocation) === href(nextLocation) && surfaceOpenIssuedKey.current === currentLocation.key;
    // Settings intent rejects launched Runtime at its real owner. An open
    // preparation/preview Player alone is not main's editWhileRunning guard.
    if (playerOpen && !openingTouch) {
      const nextOptions = plainState(plainState(nextLocation.state)[entryKey]);
      const nextProduct = resolvedSurface(nextLocation).productId;
      const sameProductEntry = address.surface !== 'room' && nextOptions.version === 1 && nextOptions.surface === 'options' &&
        nextOptions.productId === baseAddress.productId && nextProduct === baseAddress.productId;
      // Informational windows retain the exact underlying native-entry state,
      // including direct options without an owned options marker.
      const sameDirectInfoLayer = !ownedOptions && href(currentLocation) === href(nextLocation) &&
        (infoEntryKey in plainState(currentLocation.state) || infoEntryKey in plainState(nextLocation.state));
      const closingGameEntry = topGame && historyAction === 'REPLACE' && !(gameEntryKey in plainState(nextLocation.state));
      const leavingGame = topGame && historyAction === 'POP' && !roomChildConsumesPop || closingGameEntry;
      if (leavingGame || (!sameProductEntry && !sameDirectInfoLayer && !sameRoom) || sameRoomPlayerPop) {
        pendingBlockKind.current = 'runtime';
        return true;
      }
    }
    const backwards = historyAction === 'POP' && (readPageHistory(nextLocation.state)?.position ?? -1) < (pageHistory?.position ?? 0);
    if (!playerOpen && address.roomCode !== null && (!sameRoom || backwards && !roomChildConsumesPop)) {
      pendingBlockKind.current = 'room';
      return true;
    }
    const currentPage = readPageHistory(currentLocation.state), nextPage = readPageHistory(nextLocation.state);
    if (historyAction === 'POP' && currentPage && nextPage && nextPage.position < currentPage.position &&
        !playerOpen && !validInfo && !validForm && !ownedTouch && !validPanel && !validSettings) {
      const parent = PAGE_PARENTS[currentPage.page];
      if (parent && (nextPage.page !== parent || nextPage.position !== currentPage.ancestors[parent])) {
        pendingBlockKind.current = 'page'; return true;
      }
    }
    return false;
  });
  const latestBlocker = useRef(blocker);
  latestBlocker.current = blocker;
  useEffect(() => {
    if (blocker.state === 'blocked' && pendingBlockKind.current === 'unsaved' && !unsavedGuard) blocker.proceed();
  }, [blocker, unsavedGuard]);
  const replayCloseTask = useRef<Promise<void> | null>(null);
  useEffect(() => {
    if (blocker.state !== 'blocked' || pendingBlockKind.current !== 'replay' || replayCloseTask.current) return;
    const task = (async () => {
      const closed = await (requestReplayClose?.() ?? Promise.resolve(true));
      const current = latestBlocker.current;
      if (current.state !== 'blocked' || pendingBlockKind.current !== 'replay') return;
      if (!closed) {pendingBlockKind.current = null; current.reset(); return;}
      const target = current.location;
      const targetInfo = plainState(plainState(target.state)[infoEntryKey]);
      const info = plainState(plainState(currentLocation.current.state)[infoEntryKey]);
      const hasRouterParent = typeof info.parentKey === 'string' && info.parentKey !== currentLocation.current.key &&
        target.key === info.parentKey && targetInfo.active !== 'replayDialog';
      pendingBlockKind.current = null;
      if (hasRouterParent) {
        current.proceed();
        return;
      }
      current.reset();
      const source = currentLocation.current, state = plainState(source.state);
      delete state[infoEntryKey]; delete state[LOCAL_LAYERS_KEY];
      void navigate({pathname: source.pathname, search: source.search, hash: source.hash},
        {replace: true, preventScrollReset: true, state});
    })().catch(() => {pendingBlockKind.current = null; const current = latestBlocker.current; if (current.state === 'blocked') current.reset();})
      .finally(() => {if (replayCloseTask.current === task) replayCloseTask.current = null;});
    replayCloseTask.current = task;
  }, [blocker, requestReplayClose, navigate]);
  const roomLeaveTaskKey = useRef<string | null>(null);
  const runtimeCloseTask = useRef<{destination: string; settled: boolean; manualRoomExit: boolean; titleOverlayWasOpen: boolean} | null>(null);
  const mounted = useRef(true);
  useEffect(() => {mounted.current = true; return () => {mounted.current = false;};}, []);
  // biome-ignore lint/correctness/useExhaustiveDependencies: A blocker transition consumes one page-Back intent using its committed address.
  useEffect(() => {
    if (blocker.state !== 'blocked' || pendingBlockKind.current !== 'page') return;
    blocker.reset(); returnToPageParent();
  }, [blocker]);
  const fullscreenBackTask = useRef<string | null>(null);
  useEffect(() => {
    if (blocker.state === 'unblocked') fullscreenBackTask.current = null;
    if (blocker.state !== 'blocked' || pendingBlockKind.current !== 'room-fullscreen') return;
    const destination = blocker.location.key;
    if (fullscreenBackTask.current === destination) return;
    fullscreenBackTask.current = destination;
    void Promise.resolve(requestRoomFullscreenExit?.()).then(()=>{
      const current = latestBlocker.current;
      if (mounted.current && current.state === 'blocked' && current.location.key === destination) current.proceed();
    });
  }, [blocker, requestRoomFullscreenExit]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: One blocked navigation owns the asynchronous Runtime close and its captured return destination; later renders cannot restart it.
  useEffect(() => {
    if (blocker.state !== 'blocked') {
      if (runtimeCloseTask.current?.settled) runtimeCloseTask.current = null;
      return;
    }
    if (pendingBlockKind.current !== 'runtime' || runtimeCloseTask.current) return;
    const task = {destination: blocker.location.key, settled: false, manualRoomExit: manualRoomPlayerExit.current, titleOverlayWasOpen: titleOverlayOpen};
    manualRoomPlayerExit.current = false;
    runtimeCloseTask.current = task;
    // Ordinary successful sync is silent. The Runtime service owns its exact
    // failure-only Retry/Leave/Stay decision and retains the mounted iframe.
    void Promise.resolve().then(() => mounted.current ? requestRuntimeClose?.() ?? false : false).catch(() => false).then(closed => {
      if (!mounted.current || runtimeCloseTask.current !== task) return;
      task.settled = true;
      const current = latestBlocker.current;
      if (current.state !== 'blocked') {runtimeCloseTask.current = null; return;}
      if (current.location.key !== task.destination) {
        closeIssuedKey.current = null; surfaceOpenIssuedKey.current = null; current.reset();
        return;
      }
      if (closed && typeof gameEntry.parentPosition === 'number' && pageHistory) {
        current.reset(); completeRuntimeClose();
      } else if (closed && address.surface === 'room' && !task.titleOverlayWasOpen) {
        // main app5289–5306/5336–5383: MP Exit/Back closes the player,
        // then stays in its existing room. It does not leave membership.
        closeIssuedKey.current = null; surfaceOpenIssuedKey.current = null;
        if (resolvedSurface(current.location).roomCode === address.roomCode && address.productId && address.roomCode) {
          // main5293–5304 keeps a same-room POP destination and normalizes that
          // entry; only departure from the room is undone with history.forward.
          pendingRoomReturn.current = {destination: current.location.key,
            operation: completedRoomReturnOperation(address.productId, address.roomCode, current.location)};
          current.proceed();
          return;
        }
        current.reset();
        if (task.manualRoomExit && address.productId && address.roomCode) {
          // Keep canonical manual Exit normalization while retaining this
          // room's directory origin, as required for the subsequent Leave.
          applyRoomOperations([completedRoomReturnOperation(address.productId, address.roomCode)]);
        }
      } else if (closed) current.proceed();
      else {
        closeIssuedKey.current = null; surfaceOpenIssuedKey.current = null; current.reset();
      }
    });
  }, [blocker, requestRuntimeClose, address.surface]);
  useEffect(() => {
    if (blocker.state === 'unblocked') roomLeaveTaskKey.current = null;
    if (blocker.state !== 'blocked' || pendingBlockKind.current !== 'room' || roomLeaveTaskKey.current === blocker.location.key) return;
    roomLeaveTaskKey.current = blocker.location.key;
    // Main's popstate commits the Back destination before computing its room
    // return operation. The membership service resets immediately, then its
    // leaveRoomRoute port commits that original sequence through this Router.
    if (requestRoomLeave) requestRoomLeave();
    else {closeIssuedKey.current = null; blocker.reset();}
  }, [blocker, requestRoomLeave]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Normalize this pending hidden selection once its exact Router entry commits.
  useLayoutEffect(() => {
    const pending = pendingHiddenSelection.current;
    if (!pending || location.key !== pending.destination) return;
    pendingHiddenSelection.current = null;
    applyRoomOperations([{kind: 'replace', url: currentUrl().href, state: {...plainState(location.state),
      [hiddenSelectionKey]: {session: dialogSession, productId: pending.productId}}}]);
  }, [location.key]);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Apply the captured room return once on the destination Router key; changing callbacks must not replace history again.
  useLayoutEffect(() => {
    const pending = pendingRoomReturn.current;
    if (!pending) return;
    if (pending.appliedKey) {if (pending.appliedKey !== location.key) pendingRoomReturn.current = null; return;}
    if (pending.position !== undefined ? readPageHistory(location.state)?.position !== pending.position : location.key !== pending.destination) {return;}
    pending.appliedKey = location.key;
    // Keep this intent through passive cleanup of the popped location. Its
    // replacement owns normalization; stale-marker cleanup must not overwrite it.
    applyRoomOperations([{...pending.operation, state: {...pending.operation.state, [PAGE_HISTORY_KEY]: readPageHistory(location.state)}}]);
  }, [location.key]);

  // A second navigation must not reuse consent for the first destination.
  const pendingKey = useRef<string | null>(null);
  const closeIssuedKey = useRef<string | null>(null);
  const infoOpenIssuedKey = useRef<string | null>(null);
  const surfaceOpenIssuedKey = useRef<string | null>(null);
  const roomRestoreIssuedKey = useRef<string | null>(null);
  const completedRuntimeCloseKey = useRef<string | null>(null);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Each new Router entry starts a fresh set of navigation intents.
  useEffect(() => {
    closeIssuedKey.current = null; infoOpenIssuedKey.current = null; surfaceOpenIssuedKey.current = null;
    pendingLocaleAddress.current = null;
  }, [location.key]);
  useEffect(() => {
    if (blocker.state !== 'blocked') { pendingKey.current = null; return; }
    if (pendingKey.current !== null && pendingKey.current !== blocker.location.key) {
      pendingKey.current = null;
      closeIssuedKey.current = null;
      surfaceOpenIssuedKey.current = null;
      blocker.reset();
      return;
    }
    pendingKey.current = blocker.location.key;
  }, [blocker]);

  useEffect(() => {
    // A decision already on screen wins. Main rejects a new confirmation while
    // its decisionResolver/native dialog is occupied, rather than replacing it.
    const draftDecision = blocker.state === 'blocked' && (pendingBlockKind.current === 'touch' || pendingBlockKind.current === 'unsaved');
    if (applicationDecision && draftDecision) {
      closeIssuedKey.current = null;
      surfaceOpenIssuedKey.current = null;
      blocker.reset();
    }
    decisions?.setNavigationDecisionOpen(!applicationDecision && draftDecision);
    return () => { decisions?.setNavigationDecisionOpen(false); };
  }, [applicationDecision, blocker, decisions]);

  function target(params: URLSearchParams) {
    const search = params.toString();
    return {pathname: location.pathname, search: search ? `?${search}` : '', hash: location.hash};
  }
  const layerWrite = useRef<{key: string; id: string; value: LayerValue} | null>(null);
  function setSurfaceLayer(id: string, value: LayerValue) {
    if (blocker.state !== 'unblocked') return;
    const prior = localValues[id];
    if ((prior?.value ?? null) === value) return;
    if (layerWrite.current?.key === location.key && layerWrite.current.id === id && layerWrite.current.value === value) return;
    layerWrite.current = {key: location.key, id, value};
    if (value === null && prior && pageHistory && prior.parentPosition < pageHistory.position) {
      void navigate(prior.parentPosition - pageHistory.position);
      return;
    }
    const values = {...localValues};
    if (value === null) delete values[id];
    else values[id] = {value, parentPosition: prior?.parentPosition ?? pageHistory?.position ?? 0};
    const state = plainState(location.state);
    if (Object.keys(values).length) state[LOCAL_LAYERS_KEY] = {session: dialogSession, href: href(location), values};
    else delete state[LOCAL_LAYERS_KEY];
    void navigate({pathname: location.pathname, search: location.search, hash: location.hash},
      {replace: !!prior, preventScrollReset: true, state});
  }
  function infoDialogOpen(id: InformationDialogId) {
    return validInfo && infoVisible[id] === true;
  }
  function openInfoDialog(id: InformationDialogId, options?: {replace?: boolean}) {
    if (!INFORMATION_DIALOG_IDS.includes(id) || infoDialogOpen(id) || blocker.state !== 'unblocked' ||
        infoOpenIssuedKey.current === location.key) return;
    infoOpenIssuedKey.current = location.key;
    // A manager opened from a non-running Launcher needs a physical parent,
    // especially when its carrier is the first BrowserHistory entry. Runtime
    // file actions still replace their carrier so Back cannot restore an old iframe.
    const replace = options?.replace === true && (id !== 'replayDialog' || playerOpen || (pageHistory?.position ?? 0) > 0);
    // Bounded visible flags are the current Router-rendered UI, not retained
    // history receipts. Main keeps an open parent native dialog beneath child.
    const visible = Object.fromEntries(INFORMATION_DIALOG_IDS.map(name => [name, infoDialogOpen(name)]));
    visible[id] = true;
    const next = {pathname: location.pathname, search: location.search, hash: location.hash};
    void navigate(next, {replace, preventScrollReset: true, state: {
      ...plainState(location.state),
      [infoEntryKey]: {session: dialogSession, active: id, visible, ...(replace ? {} : {parentKey: location.key, parentPosition: pageHistory?.position}), href: href(next)},
    }});
  }
  function closeInfoDialog(id?: InformationDialogId) {
    if (!validInfo || (id && id !== activeInfo) || blocker.state !== 'unblocked' ||
        closeIssuedKey.current === location.key) return;
    closeIssuedKey.current = location.key;
    if (typeof infoEntry.parentKey === 'string' && infoEntry.parentKey !== location.key) {
      const distance = typeof infoEntry.parentPosition === 'number' && pageHistory
        ? infoEntry.parentPosition - pageHistory.position : -1;
      void navigate(distance < 0 ? distance : -1);
    } else {
      const state = plainState(location.state); delete state[infoEntryKey]; delete state[LOCAL_LAYERS_KEY];
      void navigate({pathname: location.pathname, search: location.search, hash: location.hash},
        {replace: true, preventScrollReset: true, state});
    }
  }
  function openDirectoryForm(mode: DirectoryFormMode) {
    if (address.context !== 'lobby' || address.surface !== 'library' || validInfo || validForm ||
        blocker.state !== 'unblocked' || surfaceOpenIssuedKey.current === location.key) return;
    surfaceOpenIssuedKey.current = location.key;
    const next = {pathname: location.pathname, search: location.search, hash: location.hash};
    void navigate(next, {preventScrollReset: true, state: {...plainState(location.state),
      [directoryFormKey]: {session: dialogSession, mode, parentKey: location.key, href: href(next)},
    }});
  }
  function closeDirectoryForm() {
    if (validInfo) {closeInfoDialog(); return;}
    if (!validForm || blocker.state !== 'unblocked' || closeIssuedKey.current === location.key) return;
    closeIssuedKey.current = location.key;
    if (typeof formEntry.parentKey === 'string' && formEntry.parentKey !== location.key) void navigate(-1);
    else {
      const state = plainState(location.state); delete state[directoryFormKey];
      void navigate({pathname: location.pathname, search: location.search, hash: location.hash},
        {replace: true, preventScrollReset: true, state});
    }
  }
  function selectLobbyProduct(productId: ProductId | '') {
    if (address.context !== 'lobby' || address.surface !== 'library' ||
        (productId && !isMultiplayerProductId(productId)) || blocker.state !== 'unblocked') return;
    const search = updateSearchParameters(location.search, {games: null, game: productId || null});
    if (search === location.search) return;
    void navigate({pathname: location.pathname, search, hash: location.hash}, {replace: true, preventScrollReset: true, state: withoutEntry(location.state)});
  }
  function openOptions(productId: ProductId) {
    if (!isProductId(productId) || !productAvailable(productId) || blocker.state !== 'unblocked' || surfaceOpenIssuedKey.current === location.key ||
        (address.context === 'lobby' && !isMultiplayerProductId(productId))) return;
    const params = new URLSearchParams(location.search);
    params.set('game', productId);
    // Directory selection and opening its settings carrier are separate main
    // intents. Replacing the iframe must not mutate the directory's filter URL.
    const next = address.context === 'lobby'
      ? {pathname: location.pathname, search: location.search, hash: location.hash}
      : target(params);
    if (address.surface === 'options' && address.productId === productId) return;
    surfaceOpenIssuedKey.current = location.key;
    setHostSelection(null);
    const replace = address.surface !== 'library';
    void navigate(next, {replace, preventScrollReset: true, state: {
      ...withoutEntry(location.state),
      [entryKey]: {version: 1, surface: 'options', productId, session: dialogSession,
        parentKey: replace && ownedOptions ? optionEntry.parentKey : location.key, href: href(next)},
    }});
  }
  function setLocaleAddress(locale: UiLocale) {
    const base = new URL(href(location), 'https://launcher.invalid');
    const shared = /\/(?:login|profile)\.html\/?$/.test(location.pathname) || address.context === 'lobby';
    const destination = new URL(/\/login\.html\/?$/.test(location.pathname) ? 'login.html' : /\/profile\.html\/?$/.test(location.pathname) ? 'profile.html' : address.context === 'lobby' ? 'lobby.html' : localeHome(locale), base);
    const next = {pathname: destination.pathname, search: uiLocaleSearch(location.search, locale, shared), hash: location.hash};
    const state = plainState(location.state);
    // main keeps history.state through locale replacement. Our href checks are
    // provenance validation, so rebase only the current entry's known owners.
    for (const key of [entryKey, touchEntryKey, infoEntryKey, directoryFormKey, roomPanelKey, roomSettingsKey, LOCAL_LAYERS_KEY]) {
      const owner = plainState(state[key]);
      if (owner.href === href(location)) state[key] = {...owner, href: href(next)};
    }
    pendingLocaleAddress.current = {sourceKey: location.key, target: href(next)};
    void navigate(next, {replace: true, preventScrollReset: true, state});
  }
  function openDirectory(productId: ProductId) {
    if (!isMultiplayerProductId(productId) || !productAvailable(productId) || address.context !== 'library' || blocker.state !== 'unblocked' ||
        surfaceOpenIssuedKey.current === location.key) return;
    surfaceOpenIssuedKey.current = location.key;
    // main app.mts8619–8630: MP home cards enter the directory document. Its
    // address carries only the selected product; legacy direct MP options stay valid.
    const destination = new URL('lobby.html', new URL(href(location), 'https://launcher.invalid'));
    destination.searchParams.set('game', productId);
    void navigate({pathname: destination.pathname, search: destination.search, hash: destination.hash},
      {state: {}, preventScrollReset: false, viewTransition: true});
  }
  function openPlayer(productId: ProductId, options?: {dismissInfo?: boolean}) {
    if (playerOpen || typeof gameEntry.position === 'number' && gameEntry.position === pageHistory?.position) return;
    if (address.productId !== productId) {openOptions(productId); return;}
    const position = pageHistory?.position ?? 0;
    const state = plainState(location.state);
    if (options?.dismissInfo) delete state[infoEntryKey];
    void navigate({pathname: location.pathname, search: location.search, hash: location.hash}, {
      // The requested app return policy gives gameplay a Back layer above its
      // options/room. The Runtime retires the old browsing context on close so
      // this new layer cannot restore a previous game's document.
      preventScrollReset: true,
      state: {...state, [gameEntryKey]: {position: position + 1, parentPosition: position, ...(options?.dismissInfo ? {replayManager: true} : {})}},
    });
  }
  function applyRoomOperations(operations: HistoryOperation[]) {
    // A service-committed main route transition uses the same Router owner.
    // This synchronous intent flag is not retained entry/history state.
    roomPolicyNavigation.current = true;
    try {
      for (const operation of operations) {
        const url = new URL(operation.url);
        void navigate({pathname: url.pathname, search: url.search, hash: url.hash},
          {replace: operation.kind === 'replace', preventScrollReset: true, state: operation.state});
      }
    } finally {roomPolicyNavigation.current = false;}
  }
  function currentUrl() {return new URL(href(location), 'https://launcher.invalid');}
  function restoreRoomRoute(input: {product: ProductId; code: string; fromDirectory: boolean}) {
    const requested = readSurface(location);
    if (requested.surface !== 'room' || !isMultiplayerProductId(input.product) || requested.roomCode !== input.code || roomRestoreIssuedKey.current === location.key) return;
    roomRestoreIssuedKey.current = location.key;
    const currentState = withoutEntry(location.state);
    // Main7604/7637 may select the permitted default MP while retaining the
    // original rejected invite token. This current document selection keeps
    // settings/actions aligned without rewriting that source-exact address.
    const requestedProduct = resolveRoomInvite(currentUrl())?.g;
    if (input.product !== requestedProduct) currentState[roomProductKey] = {
      session: dialogSession, productId: input.product, requestedProduct, code: input.code,
    };
    if (currentState[MP_ROOM_HISTORY_KEY] === input.code && address.productId === input.product) return;
    applyRoomOperations([{kind: 'replace', url: currentUrl().href, state: {...currentState, [MP_ROOM_HISTORY_KEY]: input.code}}]);
  }
  function enterRoomRoute(input: {product: ProductId; code: string}) {
    if (!isMultiplayerProductId(input.product) || !normalizeRoomCode(input.code) || surfaceOpenIssuedKey.current === location.key) return;
    surfaceOpenIssuedKey.current = location.key;
    const source = currentUrl();
    const roomUrl = new URL(source); roomUrl.pathname = new URL(localeHome(locale), source).pathname;
    applyRoomOperations([preserveSameRoomDirectoryOrigin(roomRouteHistoryOperation({currentUrl: roomUrl, currentState: withoutEntry(location.state),
      product: input.product, roomCode: normalizeRoomCode(input.code), push: true}), source)]);
  }
  function settleRoomInvite() {
    const url = currentUrl(), invite = resolveRoomInvite(url);
    if (!invite || address.surface !== 'room') return;
    const before = url.href;
    if (url.searchParams.has(ROOM_INVITE_KEY)) {
      url.searchParams.set(ROOM_INVITE_KEY, encodeRoomInvite({g: invite.g, r: invite.r, ...(invite.f ? {f: true} : {})}));
    } else for (const key of ['lobbyAction', 'lobbyPlayers', 'lobbyDifficulty', 'lobbyVisibility', 'lobbyDisableCheatMovement']) url.searchParams.delete(key);
    if (url.href === before) return;
    const state = plainState(location.state);
    for (const key of [entryKey, touchEntryKey, infoEntryKey, directoryFormKey, roomPanelKey, roomSettingsKey]) {
      const entry = plainState(state[key]);
      if (entry.href === href(location)) state[key] = {...entry, href: url.pathname + url.search + url.hash};
    }
    applyRoomOperations([{kind: 'replace', state, url: url.href}]);
  }
  function leaveTitleRoomRoute(baseProduct: ProductId) {
    // Main app917–927: overlay Back resumes the retained normal Runtime.
    if (isMultiplayerProductId(baseProduct)) return;
    const blockedDeparture = blocker.state === 'blocked' && pendingBlockKind.current === 'room';
    const source = blockedDeparture ? blocker.location : location;
    const url = new URL(href(source), 'https://launcher.invalid'); url.searchParams.set('game', baseProduct);
    url.searchParams.delete('mpRoom'); url.searchParams.delete(ROOM_INVITE_KEY);
    const operation: HistoryOperation = {kind: 'replace', url: url.href, state: withoutEntry(source.state)};
    if (blockedDeparture) {pendingRoomReturn.current = {destination: blocker.location.key, operation}; blocker.proceed();}
    else applyRoomOperations([operation]);
  }
  function leaveRoomRoute(input: {product: ProductId; fromDirectory: boolean}): 'options' | 'directory' {
    // Membership is already retired by the service. The parent is always lobby,
    // including direct invitations; entry provenance never chooses the target.
    // main mpReturnToDirectory replaces a concrete destination. History distance
    // may be stale after reload or child-frame traversal; never depend on it to
    // finish a departure whose membership has already been cleared.
    if (blocker.state === 'blocked') blocker.reset();
    roomPolicyNavigation.current = true;
    try {
      const destination = pageAddress(location, 'directory', input.product, locale);
      const filters = plainState(location.state)[directoryFiltersKey];
      if (Array.isArray(filters) && filters.every(isMultiplayerProductId)) {
        const params = new URLSearchParams(destination.search);
        params.delete('game'); params.delete('games');
        if (filters.length === 1) params.set('game', filters[0]);
        else if (filters.length > 1) params.set('games', filters.join(','));
        destination.search = params.size ? `?${params}` : '';
      }
      void navigate(destination, {replace: true, state: {}});
    }
    finally {roomPolicyNavigation.current = false;}
    return 'directory';
  }
  function openRoomLayer(kind: 'panel' | 'settings', panel?: RoomPanelKind, peer?: string) {
    if (address.surface !== 'room' || blocker.state !== 'unblocked' || surfaceOpenIssuedKey.current === location.key) return;
    if (kind === 'settings' && validSettings) return;
    surfaceOpenIssuedKey.current = location.key;
    const key = kind === 'panel' ? roomPanelKey : roomSettingsKey;
    const previous = kind === 'panel' ? panelEntry : settingsEntry;
    const alreadyOpen = kind === 'panel' ? validPanel : validSettings;
    const lifetime = alreadyOpen ? String(previous.lifetime) : newDialogSession();
    if (kind === 'panel') setPanelLifetime(lifetime); else setSettingsLifetime(lifetime);
    const operation = (kind === 'panel' ? roomPanelHistoryOperation : roomSettingsHistoryOperation)({currentUrl: currentUrl(), currentState: location.state});
    operation.state[key] = {session: dialogSession, lifetime, parentKey: alreadyOpen ? previous.parentKey : location.key, parentPosition: alreadyOpen ? previous.parentPosition : pageHistory?.position,
      href: href(location), ...(kind === 'panel' ? {kind: panel, peer: peer ?? previous.peer} : {})};
    if (alreadyOpen) operation.kind = 'replace';
    applyRoomOperations([operation]);
  }
  function closeRoomLayer(kind: 'panel' | 'settings') {
    if (validInfo) {closeInfoDialog(); return;}
    if (address.surface === 'touch') {closeSurface(); return;}
    const entry = kind === 'panel' ? panelEntry : settingsEntry;
    const valid = kind === 'panel' ? validPanel : validSettings;
    if (!valid || blocker.state !== 'unblocked' || closeIssuedKey.current === location.key) return;
    closeIssuedKey.current = location.key;
    if (typeof entry.parentKey === 'string' && entry.parentKey !== location.key) void navigate(typeof entry.parentPosition === 'number' && pageHistory ? entry.parentPosition - pageHistory.position : -1);
    else {
      const state = plainState(location.state);
      delete state[kind === 'panel' ? roomPanelKey : roomSettingsKey];
      state[kind === 'panel' ? MP_PANEL_HISTORY_KEY : MP_SETTINGS_HISTORY_KEY] = false;
      applyRoomOperations([{kind: 'replace', url: currentUrl().href, state}]);
    }
  }
  function openRoomPanel(kind: RoomPanelKind, peer?: string) {openRoomLayer('panel', kind, peer);}
  function closeRoomPanel() {closeRoomLayer('panel');}
  function openRoomSettings() {openRoomLayer('settings');}
  function closeRoomSettings() {closeRoomLayer('settings');}
  function openTouchLayout() {
    const home = address.context === 'library' && address.surface === 'library';
    if (isEditing || address.surface === 'touch' || (!home && address.surface !== 'options' && !(address.surface === 'room' && roomSettingsOpen)) ||
        blocker.state !== 'unblocked' || surfaceOpenIssuedKey.current === location.key) return;
    surfaceOpenIssuedKey.current = location.key;
    const next = {pathname: location.pathname, search: location.search, hash: location.hash};
    // Flat native-entry flags retain the parent's identity, as main's
    // {...history.state, touchLayout: true} does. There is no saved route stack.
    void navigate(next, {preventScrollReset: true, state: {
      ...plainState(location.state),
      [touchEntryKey]: {version: 1, surface: 'touch', parentKey: location.key, href: href(next)},
    }});
  }
  function completedRoomReturnOperation(product: ProductId, code: string, source: Location = location) {
    const sourceUrl = new URL(href(source), 'https://launcher.invalid');
    const operation = preserveSameRoomDirectoryOrigin(returnToRoomHistoryOperation({currentUrl: sourceUrl, currentState: source.state, product, roomCode: code}), sourceUrl);
    // The canonical main helper knows its native Player flag, but not React's
    // extra game entry. Retiring both lets the next room launch own a new layer.
    delete operation.state[gameEntryKey];
    delete operation.state[roomFullscreenKey];
    const url = new URL(operation.url);
    // Main replacement preserves still-open native dialogs. Update only their
    // current-entry URL provenance when the canonical room URL changes.
    for (const key of [entryKey, touchEntryKey, infoEntryKey, directoryFormKey, roomPanelKey, roomSettingsKey]) {
      const entry = plainState(operation.state[key]);
      if (entry.href === href(source)) operation.state[key] = {...entry, href: url.pathname + url.search + url.hash};
    }
    return operation;
  }
  function completeRuntimeClose(context: {titleOverlayWasOpen?: boolean} = {}) {
    if (completedRuntimeCloseKey.current === location.key) return;
    if (gameEntry.replayManager === true) {
      completedRuntimeCloseKey.current = location.key;
      const state = plainState(location.state); delete state[gameEntryKey];
      // The Replay viewer reused this carrier. Clear child room layers in place
      // because their parent offsets describe the route that preceded it.
      for (const key of [roomPanelKey, roomSettingsKey]) {
        if (!(key in state)) continue;
        const entry = {...plainState(state[key])}; delete entry.parentKey; delete entry.parentPosition;
        state[key] = entry;
      }
      // Restore the manager in the reused carrier after native storage flushes.
      applyRoomOperations([{kind: 'replace', url: currentUrl().href, state: {...state,
        [PLAYER_HISTORY_KEY]: false, [infoEntryKey]: {session: dialogSession, active: 'replayDialog',
          visible: {replayDialog: true}, href: href(location)}}}]);
      return;
    }
    if (address.surface === 'room' && !context.titleOverlayWasOpen && address.productId && address.roomCode) {
      completedRuntimeCloseKey.current = location.key;
      applyRoomOperations([completedRoomReturnOperation(address.productId, address.roomCode)]);
      return;
    }
    if (!validInfo && typeof gameEntry.parentPosition === 'number' && pageHistory && gameEntry.parentPosition <= pageHistory.position) {
      completedRuntimeCloseKey.current = location.key;
      // The game covers these same options. After cleanup, clear its route
      // ownership in place so browser history cannot restore the retired Runtime.
      applyRoomOperations([{kind: 'replace', url: currentUrl().href,
        state: {...withoutEntry(location.state), [PLAYER_HISTORY_KEY]: false}}]);
      return;
    }
    // Internal completion port, after a real close returned true. It does not
    // ask for sync again, even before playerOpen's next render has committed.
    if (completedRuntimeCloseKey.current === location.key) return;
    if (address.surface !== 'room' && address.surface !== 'options') return;
    completedRuntimeCloseKey.current = location.key;
    if (validInfo && typeof infoEntry.parentKey === 'string') pendingHiddenSelection.current = {destination: infoEntry.parentKey,
      productId: context.titleOverlayWasOpen && address.productId ? gameIdForProduct(address.productId) : address.productId};
    if (validInfo || (context.titleOverlayWasOpen && address.surface === 'room') || (ownedOptions && typeof optionEntry.parentKey === 'string' && optionEntry.parentKey !== location.key) ||
        plainState(location.state)[PLAYER_HISTORY_KEY]) {
      roomPolicyNavigation.current = true;
      try {navigateAfterCleanup(-1);} finally {roomPolicyNavigation.current = false;}
    } else applyRoomOperations([launcherHomeHistoryOperation({currentUrl: currentUrl(), currentState: withoutEntry(location.state)})]);
  }
  function closeSurface() {
    const localTop = Object.entries(localValues).sort((a, b) => b[1].parentPosition - a[1].parentPosition)[0];
    if (localTop) {setSurfaceLayer(localTop[0], null); return;}
    if (topGame && blocker.state === 'unblocked' && closeIssuedKey.current !== location.key) {
      closeIssuedKey.current = location.key;
      manualRoomPlayerExit.current = address.surface === 'room';
      void navigate({pathname: location.pathname, search: location.search, hash: location.hash},
        {replace: true, preventScrollReset: true, state: {...withoutEntry(location.state), [PLAYER_HISTORY_KEY]: false}});
      return;
    }
    if (validInfo) { closeInfoDialog(); return; }
    if (validForm) { closeDirectoryForm(); return; }
    if (address.surface !== 'touch' && validSettings) {closeRoomSettings(); return;}
    if (address.surface !== 'touch' && validPanel) {closeRoomPanel(); return;}
    if (blocker.state !== 'unblocked' || closeIssuedKey.current === location.key) return;
    if (!playerOpen && address.surface !== 'touch' && address.roomCode === null) {returnToPageParent(); return;}
    if (!playerOpen && address.roomCode !== null && address.surface !== 'touch') {requestRoomLeave?.(); return;}
    closeIssuedKey.current = location.key;
    // main8549/8551–8564 only consumes the product entry when the enabled raw
    // route still equals business selection; a Host fallback closes in place.
    if (address.surface === 'options' && address.context === 'library' && !playerOpen &&
        (baseAddress.productId !== address.productId || !baseAddress.productId || !productAvailable(baseAddress.productId))) {
      applyRoomOperations([launcherHomeHistoryOperation({currentUrl: currentUrl(), currentState: withoutEntry(location.state)})]);
      return;
    }
    if (address.surface === 'room') {
      if (!playerOpen) {closeIssuedKey.current = null; requestRoomLeave?.(); return;}
      // Any attempted room departure enters the one Runtime blocker. Success
      // cancels that departure and exposes the still-current room.
      manualRoomPlayerExit.current = true;
      if (titleOverlayOpen) {void navigate(-1); return;}
      const operation = launcherHomeHistoryOperation({currentUrl: currentUrl(), currentState: location.state});
      const url = new URL(operation.url);
      void navigate({pathname: url.pathname, search: url.search, hash: url.hash}, {replace: true, state: operation.state});
      return;
    }
    if (address.surface === 'options' && plainState(location.state)[PLAYER_HISTORY_KEY]) {void navigate(-1); return;}
    const entry = plainState(plainState(location.state)[address.surface === 'touch' ? touchEntryKey : entryKey]);
    if (entry.version === 1 && entry.surface === address.surface && entry.href === href(location) &&
        typeof entry.parentKey === 'string' && entry.parentKey !== location.key) {
      void navigate(-1);
      return;
    }
    // Direct URLs have no owned parent entry. Replace with a known parent,
    // preserving locale, unrelated query values and the fragment.
    const params = new URLSearchParams(location.search);
    params.delete('touchLayout');
    if (address.surface === 'options') {
      if (address.context === 'library') params.delete('game');
    }
    void navigate(target(params), {replace: true, preventScrollReset: true, state: withoutEntry(location.state)});
  }
  return <NavigationContext.Provider value={{...address, navigateAddress: address => {void navigate(address);}, hasSelection, routedProductId: baseAddress.productId, applyHostSelection, syncSelectionFromRoute, filterProductId: urlAddress.context === 'lobby' ? urlAddress.productId : null, openOptions, selectLobbyProduct, openDirectory, setLocaleAddress, restoreRoomRoute, enterRoomRoute, settleRoomInvite, leaveTitleRoomRoute, leaveRoomRoute, roomPanel, roomNetworkPeer: validPanel && typeof panelEntry.peer === 'string' ? panelEntry.peer : undefined, openRoomPanel, closeRoomPanel, roomSettingsOpen, openRoomSettings, closeRoomSettings, directoryFormMode, directoryFormParentKey: validForm && typeof formEntry.parentKey === 'string' && formEntry.parentKey !== location.key ? formEntry.parentKey : null, openDirectoryForm, closeDirectoryForm, openTouchLayout, playerOpen, openPlayer, openRoomFullscreen, closeRoomFullscreen, closeSurface, completeRuntimeClose, infoDialogOpen, openInfoDialog, closeInfoDialog}}>
    <UnsavedChangesContext.Provider value={{register: registerUnsavedGuard, confirmDiscard}}><SurfaceLayersContext.Provider value={{values: localValues, set: setSurfaceLayer}}>{children}</SurfaceLayersContext.Provider></UnsavedChangesContext.Provider>
    <FullscreenTransient><ConfirmationDialog key={applicationDecision ? `application:${applicationDecision.requestId}` : 'navigation'}
      open={applicationDecision !== null || (blocker.state === 'blocked' && (pendingBlockKind.current === 'touch' || pendingBlockKind.current === 'unsaved'))}
      title={applicationDecision?.title || t(pendingBlockKind.current === 'unsaved' ? 'profile.unsaved.title' : 'dialog.confirmTitle')}
      message={applicationDecision ? applicationDecision.message || '' : t(pendingBlockKind.current === 'unsaved' ? 'profile.unsaved.message' : 'touch.layoutDiscardConfirm')}
      confirmText={applicationDecision ? applicationDecision.confirmText || t('action.confirm') : t('touch.discardChanges')}
      cancelText={applicationDecision?.cancelText || t(pendingBlockKind.current === 'unsaved' ? 'career.continue' : 'action.cancel')}
      secondaryText={applicationDecision?.secondaryText || ''}
      tone={applicationDecision ? applicationDecision.tone || 'normal' : 'danger'}
      variant={applicationDecision?.variant || ''} hideCancel={applicationDecision?.hideCancel || false}
      confirmOnEnter={applicationDecision?.confirmOnEnter || false}
      input={applicationDecision?.input}
      onSecondary={() => {if (applicationDecision) decisions?.resolve('secondary');}}
      onCancel={() => {
        if (applicationDecision) decisions?.resolve('cancel');
        else if (blocker.state === 'blocked') {
          closeIssuedKey.current = null; surfaceOpenIssuedKey.current = null; blocker.reset();
        }
      }}
      onConfirm={value => {
        if (applicationDecision) {decisions?.resolve('confirm', value); return;}
        if (blocker.state !== 'blocked' || pendingKey.current !== blocker.location.key) return;
        if (pendingBlockKind.current === 'unsaved') {
          unsavedGuard?.discard();
          return;
        }
        onDiscard();
        blocker.proceed();
      }}/></FullscreenTransient>

  </NavigationContext.Provider>;
}
