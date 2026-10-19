import {loadGameVersion} from "../../package/game-version.mjs";
import {DEFAULT_GAME_OPTIONS} from '../../src/launcher/game-preferences.mts';
import {loadCompatibleSiteManifest} from '../../src/contracts/compat/host-manifest.mts';
import {createNetplayTelemetry} from '../../src/launcher/netplay-telemetry.mts';
import {linkAbortSignals, waitWithAbort} from '../services/abort-signals';
import {DEFAULT_PRODUCT_ID, DEFAULT_MULTIPLAYER_PRODUCT_ID, PRODUCT_IDS, PRODUCT_GAMES, createLocalProductManifest, productEnabledForBuild, gameIdForProduct, isMultiplayerProductId, multiplayerConfigForProduct, type ProductId, type GameId} from '../../src/contracts/product-catalog.mts';
import type {SiteManifest} from '../../src/contracts/site-manifest.mts';
import {detectUiLocale, isUiMessageKey, UI_LOCALE_STORAGE_KEY, type UiLocale} from '../../src/launcher/i18n.mts';
import {replayImportAccept} from '../../src/launcher/replay-files.mts';
import {discouragedBrowserId} from '../../src/launcher/browser-support.mts';
import {createMultiplayerIdentityStore, multiplayerMemberId} from '../../src/launcher/multiplayer-identity.mts';
import {createMultiplayerRoomSessionStore} from '../../src/launcher/multiplayer-room-session.mts';
import {buildMultiplayerDiagnosticRelayUrl} from '../../src/launcher/multiplayer-relay-url.mts';
import {createNetworkDiagnosticsModel} from '../components/directory/network-diagnostics-model';
import {createLobbyDirectory} from '../services/lobby-directory';
import {translate, type Translate} from '../i18n';
import {createGameSettingsModel, settingsChangeResetsRuntime, type SettingsSnapshot} from '../models/game-settings';
import {createGamepadMappingModel} from '../../src/launcher/gamepad-mapping.mts';
import {createKeyboardPreferences} from '../../src/launcher/keyboard-preferences.mts';
import {createTh09LocalInput} from '../../src/launcher/th09-local-input.mts';
import {createTouchLayoutModel, type TouchLayoutOrientation} from '../models/touch-layout';
import {createDecisionStore} from '../models/decisions';
import {createGameUpdatePrompt, type GameUpdateContext} from '../models/game-update-prompt';
import {createTransferModel} from '../models/transfer';
import {createNetworkActivityTracker} from '../../src/launcher/network-activity.mts';
import {createGameDataImportModel} from '../models/game-data-import';
import {roomCodeFromUrl, resolveRoomInvite, routedProductFromUrl} from '../../src/launcher/route-state.mts';
import {createStartupErrorModel} from '../models/startup-error';
import {createFeedbackModel} from '../models/feedback';
import {createRuntimeDiagnostics} from '../models/runtime-diagnostics';
import {createSpectatorPlayback, spectatorPlaybackCommand, type SpectatorPlaybackAction} from '../../src/launcher/spectator-playback.mts';
import {createGameplayNetwork} from '../models/gameplay-network';
import {createNetplayCalibration} from '../models/netplay-calibration';
import {createTitleNetwork} from '../models/title-network';
import {describeBrowserEnvironment} from '../../src/launcher/runtime-diagnostics-model.mts';
import {createSitePreferencesModel, bindSitePreferencesToDocument} from '../models/site-preferences';
import {createReplayModel, type ReplayModel} from '../models/replays';
import {createMetadataService} from '../services/metadata';
import {createPackageAcquisition} from '../services/package-acquisition';
import {prepareFilePlan, prepareGame, prepareGameCheck, prepareRoomResourcePlan, type MultiplayerPreparation, type GamePreparationInput, type PreparedGame} from '../services/game-preparation';
import {createPreparationNetwork} from '../services/preparation-network';
import {createRoomPreparation} from '../services/room-preparation';
import {createRoomSession, type RoomSession} from './room-session';
import type {RoomLaunchContext, RoomOperationContext} from '../services/multiplayer-room';
import {isGameDataAcquisitionFailure} from '../services/game-data-acquisition';
import {createRuntimeService, RuntimeSessionSupersededError, type RuntimeService, type RuntimeSnapshot} from '../services/runtime';
import {bindPixelPerfectDisplay} from '../services/pixel-perfect-display';
import {createExternalMidiPort} from '../services/external-midi';
import {createMidiBridge} from '../services/midi-bridge';
import {createTouchNativePorts} from '../services/touch-native';
import {createStartupController} from '../services/startup';
import {createLocalPackageMaintenance} from '../services/local-package-maintenance';
import {createMetadataRetry} from '../services/metadata-retry';
import {createAppShellService, type AppShellOptions} from '../services/app-shell';
import {createBrowserPorts} from '../services/browser-ports';
import {createFileActions, type ImportFileKind} from '../services/files';
import {createReplayStorageReader} from '../services/replay-storage';
import {errorText} from '../services/error-text';
import {launcherBaseUrl} from '../navigation/addresses';
import type {SettingsActions} from '../components/settings/types';
import type {InformationDialogId, HostSelection, SurfaceContext, Surface} from '../navigation/surface-navigation';

export interface BrowserNavigationPorts {
  /** Current parent selection, restored when an in-player browser changes mode. */
  productId?: ProductId | null;
  hasSelection: boolean;
  applyHostSelection(selection: HostSelection): void;
  syncSelectionFromRoute(): ProductId | null;
  openPlayer(product: ProductId, options?: {dismissInfo?: boolean}): void;
  openDirectory(product: ProductId): void;
  closeSurface(): void;
  completeRuntimeClose(context?: {titleOverlayWasOpen?: boolean}): void;
  openInfoDialog(id: InformationDialogId, options?: {replace?: boolean}): void;
  closeInfoDialog(id?: InformationDialogId): void;
  selectLobbyProduct(product: ProductId): void;
  navigateToRoom(url: string): void;
  setLocaleAddress(locale: UiLocale): void;
  restoreRoomRoute(input: {product: ProductId; code: string; fromDirectory: boolean}): void;
  enterRoomRoute(input: {product: ProductId; code: string}): void;
  settleRoomInvite(): void;
  leaveTitleRoomRoute(baseProduct: ProductId): void;
  leaveRoomRoute(input: {product: ProductId; fromDirectory: boolean; message?: string}): 'options' | 'directory';
  closeRoomPanel(): void;
  closeRoomSettings(): void;
}
export interface BrowserSessionSnapshot {
  locale: UiLocale;
  runtime: RuntimeSnapshot | null;
  playerOpen: boolean;
  touchHelpOpen: boolean;
  activity: number;
  metadataRevision: number;
  startupError: unknown | null;
  roomLaunchStage: 'runtime' | 'path' | null;
  roomReloadPending?: boolean;
  replayViewer: boolean;
  spectatorViewer?: boolean;
  th09NetworkOverlayOpen: boolean;
  bootTouchPreview: 'touch' | 'touch-hud' | null;
  bootTouchPreviewImage: string | undefined;
}
/** One document owner. Construction is browser-only and intentionally separate
 * from React render. The permanent iframe is attached once by its callback ref. */
export function createBrowserSession({document, window, baseUrl: configuredBaseUrl, appShellDeployment}: {document: Document; window: Window; baseUrl?: string; appShellDeployment?: AppShellOptions['deployment']}) {
  const baseUrl = configuredBaseUrl ?? launcherBaseUrl(window.location.href);
  let storage: Storage | null = null;
  try {storage = window.localStorage;} catch {}
  const gamepad = createGamepadMappingModel({storage,getGamepads:typeof window.navigator.getGamepads==='function'?()=>window.navigator.getGamepads():undefined});
  const keyboardPreferences = createKeyboardPreferences(storage);
  const th09LocalInput = createTh09LocalInput(storage);
  const mobile = (window.navigator as Navigator & {userAgentData?: {mobile?: boolean}}).userAgentData?.mobile === true || /Android|iPhone|iPad|iPod|Mobile/i.test(window.navigator.userAgent) || window.navigator.maxTouchPoints > 1 && /Macintosh/i.test(window.navigator.userAgent);
  const primaryTouchPointer = window.matchMedia('(pointer: coarse)');
  const preferTouchSettings = () => mobile || primaryTouchPointer.matches;
  const audio = 'AudioContext' in window || 'webkitAudioContext' in window;
  const listeners = new Set<() => void>();
  let snapshot: BrowserSessionSnapshot = Object.freeze({locale: window.location.pathname.endsWith('/en.html') ? 'en' : detectUiLocale(), runtime: null, playerOpen: false, touchHelpOpen: false, activity: 0, metadataRevision: 0, startupError: null, roomLaunchStage: null, replayViewer: false, th09NetworkOverlayOpen: false, bootTouchPreview: null, bootTouchPreviewImage: undefined});
  let helpSeen = false; try {helpSeen = storage?.getItem('eagler-touch-help-seen-v8') === '1';} catch {}
  let touchFireEnabled = true;
  const touchFireState = {getEnabled: () => touchFireEnabled, setEnabled(value: boolean) {touchFireEnabled = value;}};
  let roomReturnTimer: number | null = null, pendingRoomReturnProduct: ProductId | null = null;
  let playerOrientationPending = false;
  let playerRelayout: (() => Promise<boolean>) | null = null;
  let disposed = false, navigation: BrowserNavigationPorts | null = null;
  let pendingHostSelection: HostSelection | null = null;
  let appliedHost: SiteManifest | null = null;
  let runtime: RuntimeService | null = null, frame: HTMLIFrameElement | null = null;
  let unbindPixelPerfectDisplay: (() => void) | null = null;
  let replays: ReplayModel | null = null, files: ReturnType<typeof createFileActions> | null = null;
  let midi: ReturnType<typeof createMidiBridge> | null = null;
  let diagnostics: ReturnType<typeof createRuntimeDiagnostics> | null = null;
  let telemetryTimer: number | null = null;
  let gameplayNetwork: ReturnType<typeof createGameplayNetwork> | null = null;
  let roomSession: RoomSession | null = null;
  let activeSettings: SettingsSnapshot | null = null;
  let selectedReplayPath: string | undefined;
  let invalidatedSettingsEpoch: number | null = null;
  let launchAbort: AbortController | null = null, launchPending: Promise<void> | null = null;
  let updateAbort: AbortController | null = null, musicAbort: AbortController | null = null, updating = false, downloadingMusic = false, downloadingPackage = false, downloadingLanguage = false;
  let fileDownload: {controller: AbortController; ownsPresentation(): boolean} | null = null;
  let fileRuntimePresentation: (() => boolean) | null = null;
  let appShell: ReturnType<typeof createAppShellService> | null = null;
  let blockingDownload: {label: string; cancel(): Promise<void>} | null = null;
  let unsubscribeRuntime: (() => void) | null = null;
  const temporaryFiles = new Map<ProductId, number>();
  // A repeated open is a new presentation intent even if Player was already open.
  // Completion guards retain identity after launch/preparation busy flags settle.
  let playerIntentRevision = 0;
  let gameUpdates: ReturnType<typeof createGameUpdatePrompt> | null = null;
  function publish(patch: Partial<BrowserSessionSnapshot>) {
    if (disposed) return;
    if (patch.playerOpen === true) playerIntentRevision++;
    snapshot = Object.freeze({...snapshot, ...patch}); for (const listener of listeners) listener(); appShell?.notifyActivityChanged(); gameUpdates?.flush();
  }
  const t: Translate = (key, params) => translate(snapshot.locale, key, params);
  const stringTranslate = (key: string, params?: Record<string, string | number>) => {
    if (!isUiMessageKey(key)) throw new Error(`Unknown main translation key: ${key}`);
    return t(key, params);
  };
  const feedback = createFeedbackModel(), decisions = createDecisionStore();
  feedback.translatedStatus(() => t('status.selectGame'));
  const calibration = createNetplayCalibration({timers: window, userAgent: window.navigator.userAgent});
  const spectatorPlayback = createSpectatorPlayback();
  let roomPreload: {product: ProductId; epoch: number; prepared: PreparedGame} | null = null;
  let roomPreloadTask: Promise<unknown> = Promise.resolve();
  let replayRoomEpoch: number | null = null;
  function retireChangedSettings() {
    const owner = runtime, current = owner?.getSnapshot();
    if (!owner || !current || invalidatedSettingsEpoch === null) return;
    if (current.epoch !== invalidatedSettingsEpoch || current.launched) {invalidatedSettingsEpoch = null; return;}
    // The sole file owner finishes its transaction before a cold configuration
    // is retired. Never add a second writer or clear a saving game underneath it.
    if (current.fileOperationBusy || current.phase === 'saving' || current.phase === 'launching') return;
    invalidatedSettingsEpoch = null;
    try {owner.cancel(); gameData.clear(); transfer.hide(); midi?.reset();}
    catch (error) {reportError(error);}
  }
  let versionSwitch: Promise<void> | null = null;
  let roomSettingsBaseline: {settings: SettingsSnapshot; epoch: number} | null = null;
  const selectedVersion = (game: GameId) => settings.getSnapshot()?.gameId === game
    ? settings.getSnapshot()!.gameVersion : loadGameVersion(storage, game);
  const settings = createGameSettingsModel({storage, onChange: change => {
    const inRoom = roomSession?.service.getSnapshot();
    if (change.previous && inRoom?.product === change.current.context.productId && inRoom.room &&
        !runtime?.getSnapshot().launched && (settingsChangeResetsRuntime(change) ||
          change.reason === 'language' && change.previous.language !== change.current.language ||
          change.reason === 'music' && (change.previous.music !== change.current.music || change.previous.musicPreference !== change.current.musicPreference || change.previous.musicPreferenceExplicit !== change.current.musicPreferenceExplicit))) {
      if (change.reason === 'game-version') queueMicrotask(() => {void metadata.refreshInstalled(change.current.gameId);});
      if (!roomSettingsBaseline || roomSettingsBaseline.epoch !== inRoom.epoch) {
        roomSettingsBaseline = {settings: change.previous, epoch: inRoom.epoch};
        publish({roomReloadPending: true});
        roomSession!.service.suspendForSettingsReload();
      }
      return;
    }
    if (!settingsChangeResetsRuntime(change)) return;
    if (change.reason === 'game-version') {
      queueMicrotask(() => {void metadata.refreshInstalled(change.current.gameId).then(() => {
        if (settings.getSnapshot()?.gameId === change.current.gameId) settings.refreshContext(settingsContext(change.current.context.productId, snapshot.locale));
      });});
      if (runtime?.getSnapshot().launched && !snapshot.replayViewer && !change.current.multiplayer && !versionSwitch) {
        // configuredLaunch owns the existing save-sync close barrier. Resource
        // version never changes the Runtime's game/save identity.
        versionSwitch = (async () => {
          while (!disposed && settings.getSnapshot()?.context.productId === change.current.context.productId) {
            const desired = settings.getSnapshot()!.gameVersion;
            await configuredLaunch(change.current.context.productId);
            if (settings.getSnapshot()?.gameVersion === desired) break;
          }
        })().catch(reportError).finally(() => {versionSwitch = null;});
        return;
      }
        // A second choice during the save/load barrier is handled by the loop.
        if (versionSwitch && !change.current.multiplayer) return;
    }
    if (change.reason === 'sharing') {const key = change.current.shareSingleplayerSettings ? 'status.shareSettings' : 'status.separateSettings'; feedback.translatedStatus(() => t(key));}
    if (!change.current.multiplayer && change.reason === 'music') {
      const mode = change.current.music;
      feedback.status(t('status.musicMode', {music: mode === 'midi' ? 'midi' : t(mode === 'ogg-stream' ? 'settings.music.oggStream' : mode === 'ogg-full' ? 'settings.music.oggFull' : 'settings.music.none')}));
    }
    if (!change.current.multiplayer && change.reason === 'language') {
      const entry = change.current.context.languages.find(value => value.id === change.current.language);
      feedback.status(t('status.selectedLanguage', {language: entry?.title || entry?.id || t('language.fallbackName')}));
    }
    if (!runtime) return;
    const current = runtime.getSnapshot();
    if (current.epoch === null || current.launched) return;
    invalidatedSettingsEpoch = current.epoch;
    launchAbort?.abort(new RuntimeSessionSupersededError()); fileDownload?.controller.abort(new RuntimeSessionSupersededError());
    queueMicrotask(retireChangedSettings);
  }});
  const touchLayout = createTouchLayoutModel({storage}); touchLayout.hydrate();
  const preferences = createSitePreferencesModel({storage});
  const unbindPreferences = bindSitePreferencesToDocument(preferences, { windowObj: window});
  const ports = createBrowserPorts({document, window, baseUrl});
  const startup = createStartupController({document, window, baseUrl,
    preloadLabel: context => t(context === 'lobby' ? 'lobby.loading' : 'preload.aria')});
  const startupError = createStartupErrorModel({translate: t, launched: () => runtime?.getSnapshot().launched === true, copyText: ports.copyText, toast: feedback.toast});
  const transfer = createTransferModel({translate: t,
    context: () => ({epoch: runtime?.getSnapshot().epoch ?? null, launched: runtime?.getSnapshot().launched === true,
      iosWebKitTouch: /iPad|iPhone|iPod/i.test(window.navigator.userAgent) || /Macintosh/i.test(window.navigator.userAgent) && window.navigator.maxTouchPoints > 1}),
    frames: {request: callback => window.requestAnimationFrame(callback), cancel: id => window.cancelAnimationFrame(id)},
    ports: {retryMusic: timeout => requiredRuntime().send('retry-music', {}, timeout),
      cancelDownload: async () => {if (fileDownload?.ownsPresentation()) fileDownload.controller.abort(new DOMException('已取消下载', 'AbortError')); else if (updating) updateAbort?.abort(); else if (downloadingMusic) musicAbort?.abort(); else if (downloadingPackage || downloadingLanguage) launchAbort?.abort(new DOMException('已取消下载', 'AbortError')); else if (blockingDownload) await blockingDownload.cancel(); else launchAbort?.abort(new DOMException('已取消下载', 'AbortError'));},
      playerStatus: feedback.playerStatus, toast: feedback.toast, noteGameDataTransfer: message => gameData.noteTransfer(message)},
  });
  const network = createNetworkActivityTracker({fetchImpl: window.fetch.bind(window), onChange: transfer.networkSnapshot});
  const metadata = createMetadataService({baseUrl, storage, getGameVersion: selectedVersion, indexedDBFactory: window.indexedDB,
    translate: (locale, key, params) => {if (!isUiMessageKey(key)) throw new Error(`Unknown main translation key: ${key}`); return translate(locale === 'en' ? 'en' : 'zh-CN', key, params);},
    webAudioAvailable: audio, webMidiAvailable: 'requestMIDIAccess' in window.navigator, mobile: preferTouchSettings(), locale: () => snapshot.locale,
    testBuild: new URL(window.location.href).searchParams.has('test')});
  function settingsContext(product: ProductId, locale: string) {const room = roomSession?.service.getSnapshot(); return {...metadata.settingsContext(product, locale), mobile: preferTouchSettings(), forbidUnlimitedMovement: !!room?.room?.disableCheatMovement && room.seat != null};}
  const entryUrl = new URL(window.location.href), directoryEntry = /\/lobby(?:\.html)?\/?$/.test(entryUrl.pathname);
  const rawInitialProduct = directoryEntry ? null : routedProductFromUrl(entryUrl, new Set(PRODUCT_IDS));
  const initialProduct = rawInitialProduct && productEnabledForBuild(rawInitialProduct, entryUrl.searchParams.has('test')) ? rawInitialProduct : DEFAULT_PRODUCT_ID;
  settings.hydrate(settingsContext(DEFAULT_PRODUCT_ID, snapshot.locale));
  if (initialProduct !== DEFAULT_PRODUCT_ID) settings.hydrate(settingsContext(initialProduct, snapshot.locale));
  touchLayout.setGame(settings.getSnapshot()?.gameId ?? DEFAULT_PRODUCT_ID);
  const unsubscribeTouchLayoutGame = settings.subscribe(() => touchLayout.setGame(settings.getSnapshot()?.gameId ?? DEFAULT_PRODUCT_ID));
  // Main9742: one boot-only, non-persisted preview write after routed preferences
  // and before the first real Host reread. It opens no Runtime/history/fullscreen.
  const touchPreview = directoryEntry ? null : entryUrl.searchParams.get('preview');
  if (touchPreview === 'touch' || touchPreview === 'touch-hud') {
    settings.applyBootTouchPreview();
    const game = PRODUCT_GAMES[gameIdForProduct(initialProduct)];
    const artwork = 'cardArtwork' in game && game.cardArtwork ? `url(${JSON.stringify(new URL(`assets/${game.cardArtwork}`, baseUrl).href)})` : 'none';
    publish({bootTouchPreview: touchPreview, bootTouchPreviewImage: artwork, playerOpen: true, touchHelpOpen: touchPreview === 'touch'});
  }
  const syncTouchSettingsOrder = () => settings.setTouchSettingsPriority(preferTouchSettings());
  primaryTouchPointer.addEventListener('change', syncTouchSettingsOrder);
  const unsubscribeMetadata = metadata.subscribe(() => {
    const current = metadata.getSnapshot();
    if (current.siteManifest && !current.siteManifestError && current.siteManifest !== appliedHost) {
      const firstHost = appliedHost === null, host = current.siteManifest;
      const previousProduct = settings.getSnapshot()!.context.productId;
      let product = previousProduct;
      if (!Object.hasOwn(host.games, gameIdForProduct(product))) {
        const fallback = Object.keys(PRODUCT_GAMES).find(game => Object.hasOwn(host.games, game)) as ProductId | undefined;
        if (!fallback) throw new Error('Site Manifest 没有可用游戏');
        product = fallback;
        settings.restoreHostPreferences(settingsContext(product, snapshot.locale));
        publish({replayViewer: false});
      }
      if (firstHost && !runtime?.getSnapshot().launched) settings.restoreHostPreferences(settingsContext(product, snapshot.locale));
      appliedHost = host;
      localPackageMaintenance.schedule();
      const hadSelection = pendingHostSelection?.hasSelection ?? navigation?.hasSelection ?? (!!rawInitialProduct && productEnabledForBuild(rawInitialProduct, entryUrl.searchParams.has('test')));
      pendingHostSelection = {productId: product, hasSelection: hadSelection && current.products.includes(product)};
      if (navigation) {navigation.applyHostSelection(pendingHostSelection); pendingHostSelection = null;}
      if (roomSession?.service.getSnapshot().room && host.shared.netplayRelay) roomSession.service.reconnect();
    }
    diagnostics?.refresh();
    publish({metadataRevision: snapshot.metadataRevision + 1});
  });
  const localManifest = createLocalProductManifest();
  const localPackageMaintenance = createLocalPackageMaintenance({
    getGames: () => metadata.getSnapshot().siteManifest?.games ?? localManifest.games,
    origin: window.location.origin, onInstalled: metadata.acceptInstalledHint,
    onHydrated: () => publish({metadataRevision: snapshot.metadataRevision + 1}),
    scheduler: {requestFrame: callback => window.requestAnimationFrame(callback), cancelFrame: id => window.cancelAnimationFrame(id),
      setTimer: callback => window.setTimeout(callback, 0), clearTimer: id => window.clearTimeout(id)},
  });
  const preparationNetwork = createPreparationNetwork({foreground: network, baseUrl, translate: t, fetchImpl: window.fetch.bind(window)});
  const acquisition = createPackageAcquisition({metadata, baseUrl, getGameVersion: selectedVersion, translate: t, fetchImpl: window.fetch.bind(window), network: preparationNetwork,
    testBuild: entryUrl.searchParams.has('test'),
    requestPersistence: async () => {try {await window.navigator.storage?.persist?.();} catch {}}});
  let sessionStorage: Storage | null = null; try {sessionStorage = window.sessionStorage;} catch {}
  const identity = createMultiplayerIdentityStore({persistentStorage: storage, sessionStorage});
  const sessions = createMultiplayerRoomSessionStore({storage: sessionStorage});
  const directory = createLobbyDirectory({
    loadSiteManifest: signal => loadCompatibleSiteManifest(async file => {const response = await window.fetch(new URL(file, baseUrl), {cache: 'no-store', signal}); if (!response.ok) throw Object.assign(new Error(`HTTP ${response.status}`), {status: response.status}); return response.json();}),
    createSocket: url => new WebSocket(url), memberId: multiplayerMemberId,
    identity, sessions, sessionStorage, translate: t,
    launcherUrl: () => new URL(snapshot.locale === 'en' ? 'en.html' : './', baseUrl).href,
    navigateToRoom: url => requiredNavigation().navigateToRoom(url),
    selectedProduct: new URL(window.location.href).searchParams.get('game') ?? undefined,
    onSelectedProductNormalized: product => requiredNavigation().selectLobbyProduct(product),
  });
  const networkDiagnostics = createNetworkDiagnosticsModel({ translate: stringTranslate,
    getRelayUrl: () => {const host = /\/lobby(?:\.html)?\/?$/.test(window.location.pathname) ? directory.getSnapshot().siteManifest : metadata.getSnapshot().siteManifest; const relay = host?.shared.netplayRelay; return relay ? buildMultiplayerDiagnosticRelayUrl(relay) : '';},
    getFallbackIceServers: () => [{urls: ['stun:stun.cloudflare.com:3478']}]});
  const nativeTouch = createTouchNativePorts({document, screen: window.screen, mobile, translate: t});
  const playerGestureTypes = ['contextmenu', 'selectstart', 'dragstart', 'gesturestart', 'gesturechange', 'gestureend'];
  const preventPlayerBrowserGesture = (event: Event) => {
    const player = document.getElementById('player'), target = event.target;
    if (!player?.classList.contains('open') || target instanceof Element && target.closest('#gameDataLinkWindow')) return;
    if (target === player || target instanceof Node && player.contains(target)) event.preventDefault();
  };
  for (const type of playerGestureTypes) document.addEventListener(type, preventPlayerBrowserGesture, {capture: true, passive: false});
  function requiredRuntime() {if (!runtime) throw new Error('Launcher Runtime frame is not attached'); return runtime;}
  function requiredNavigation() {if (!navigation) throw new Error('Launcher Router is not attached'); return navigation;}
  function reportError(error: unknown) {if (error instanceof RuntimeSessionSupersededError) return; feedback.status(errorText(error)); feedback.toast(errorText(error));}
  const externalMidi = createExternalMidiPort({settings, navigator: window.navigator, translate: t, feedback: feedback.toast,
    beforeExternalTakeover: () => midi?.resetSynth(),
    onConfigurationChange() {if (runtime && !runtime.getSnapshot().launched && runtime.getSnapshot().epoch !== null) runtime.cancel();}});
  async function activity<T>(operation: () => Promise<T>): Promise<T> {
    publish({activity: snapshot.activity + 1});
    try {return await operation();} finally {publish({activity: Math.max(0, snapshot.activity - 1)});}
  }
  function capturedSettings(product: ProductId) {
    const current = settings.getSnapshot();
    if (current?.context.productId === product) return current;
    // File operations keep their captured product without rehydrating another
    // visible product's shared settings or overwriting current edits.
    const capture = createGameSettingsModel({storage}); capture.hydrate(settingsContext(product, snapshot.locale));
    return capture.getSnapshot()!;
  }
  function captureFilePresentation(playerIntent = playerIntentRevision) {
    const launchOwnedAtCapture = !!launchAbort;
    return () => !disposed && !launchOwnedAtCapture && playerIntent === playerIntentRevision && !launchAbort;
  }
  async function prepareFiles(product: ProductId, options: {retireRunning?: boolean; ownsPresentation?: () => boolean} = {}) {
    const game = gameIdForProduct(product);
    const variant = isMultiplayerProductId(product) ? 'multiplayer' : 'normal';
    // Imports retain their enqueue-time context across the Replay queue. Other
    // file tasks capture here. Progress, native UI and Cancel share this owner.
    const ownsPresentation = options.ownsPresentation ?? captureFilePresentation();
    // A write requested during Replay startup joins that preparation instead
    // of replacing its still-loading native document with a file-only Runtime.
    if (replayPreparation && replays?.getSnapshot().productId === product) await replayPreparation.catch(() => {});
    const owner = requiredRuntime(), before = owner.getSnapshot();
    if (before.ready && before.game === game && before.runtimeVariant === variant && !(options.retireRunning && before.launched)) return;
    if (before.epoch !== null && !await owner.close({decide: closeDecision})) throw new RuntimeSessionSupersededError();
    const controller = new AbortController();
    let preparationEpoch: number | null = null;
    const ownsCurrentEpoch = () => {
      const current = owner.getSnapshot();
      return current.epoch === null || current.epoch === preparationEpoch && current.game === game && current.runtimeVariant === variant;
    };
    const requireIdle = () => {if (owner.getSnapshot().epoch !== null) throw new RuntimeSessionSupersededError();};
    try {
      const plan = await prepareFilePlan({productId: product, settings: capturedSettings(product), acquisition, baseUrl, translate: t,
        signal: controller.signal, onAcquisitionActivity: active => {
          if (active) {requireIdle(); fileDownload = {controller, ownsPresentation}; if (ownsPresentation()) transfer.setCancellation(t('package.cancelDownload'));}
          else if (fileDownload?.controller === controller) {fileDownload = null; if (ownsCurrentEpoch() && ownsPresentation()) transfer.setCancellation(blockingDownload?.label ?? null);}
        },
        onRuntimePlan: plan => {requireIdle(); if (ownsPresentation()) {transfer.beginRuntime(plan); if (!plan.generation && plan.directPreloadHost) gameData.beginDirectDownload();}},
        onProgress: progress => {if (ownsCurrentEpoch() && ownsPresentation()) transfer.preparationProgress(progress);}});
      // Acquisition owns no native epoch. A newer Start may have acquired one
      // while it waited; never cancel that owner in this operation's catch.
      requireIdle();
      fileRuntimePresentation = ownsPresentation;
      const preparing = owner.prepare(plan, controller.signal);
      preparationEpoch = owner.getSnapshot().epoch;
      const ready = await preparing;
      if (ready.epoch !== owner.getSnapshot().epoch) throw new RuntimeSessionSupersededError();
      if (ready.epoch !== null) temporaryFiles.set(product, ready.epoch);
    } catch (error) {
      // A published-code retry may have advanced this prepare to a later epoch.
      // Runtime owns its failure reset; do not cancel an unproven current owner
      // or replace the original error if a later retry could not retire itself.
      if (error instanceof RuntimeSessionSupersededError || !ownsCurrentEpoch()) throw error;
      if (!owner.getSnapshot().launched) {try {owner.cancel();} catch {} if (ownsPresentation()) {gameData.clear(); transfer.hide();}}
      throw error;
    } finally {if (fileDownload?.controller === controller) {fileDownload = null; if (ownsCurrentEpoch() && ownsPresentation()) transfer.setCancellation(null);}}
  }
  async function releasePrepared(product: ProductId, expectedEpoch = temporaryFiles.get(product)) {
    const owner = requiredRuntime(), current = owner.getSnapshot();
    if (expectedEpoch === undefined || current.epoch !== expectedEpoch || current.game !== gameIdForProduct(product) || current.launched) return;
    if (await owner.close({decide: closeDecision})) temporaryFiles.delete(product);
  }
  async function closeDecision(error: unknown): Promise<'retry' | 'leave' | 'stay'> {
    const choice = await decisions.askDecision({message: t('dialog.saveSyncFailed', {reason: errorText(error)}),
      confirmText: t('action.retrySave'), secondaryText: t('action.leaveAnyway'), cancelText: t('action.stayInGame'), tone: 'danger'});
    return choice === 'confirm' ? 'retry' : choice === 'secondary' ? 'leave' : 'stay';
  }
  async function requestRuntimeClose({nativeExit = false}: {nativeExit?: boolean} = {}): Promise<boolean> {
    const owner = requiredRuntime();
    const currentRoom = roomSession?.service.getSnapshot();
    if (!nativeExit && owner.getSnapshot().launched && !snapshot.replayViewer && currentRoom?.room &&
        !snapshot.th09NetworkOverlayOpen) {
      try {await owner.returnToRoom();}
      catch (error) {reportError(error); return false;}
      await nativeTouch.exitPlayerFullscreen().catch(() => {});
      transfer.hide(); startupError.close(); activeSettings = null;
      publish({playerOpen:false,touchHelpOpen:false,startupError:null,roomLaunchStage:null,spectatorViewer:false});
      return true;
    }
    const replayReturnProduct = snapshot.replayViewer ? navigation?.productId : null;
    // Pending preparation cancellation is fenced before asynchronous acquisition
    // can publish another document. A launched writer remains intact until sync.
    if (!owner.getSnapshot().launched) launchAbort?.abort(new RuntimeSessionSupersededError());
    // Main native exit (including failure status) has no surviving sync receiver.
    // Only the authenticated event owner supplies this completed-engine flag.
    if (!await owner.close({decide: closeDecision, discardUnsaved: nativeExit || !owner.getSnapshot().launched,
      beforeReset: () => nativeTouch.exitPlayerFullscreen().catch(() => {})})) return false;
    gameData.clear(); if (titleNetwork.getSnapshot().open) roomSession?.service.resetRoomWithoutNavigation(); titleNetwork.close(false); transfer.hide(); startupError.close(); midi?.reset(); activeSettings = null; selectedReplayPath = undefined;
    if (replayReturnProduct && settings.getSnapshot()?.context.productId !== replayReturnProduct) settings.hydrate(settingsContext(replayReturnProduct, snapshot.locale));
    publish({playerOpen: false, touchHelpOpen: false, startupError: null, roomLaunchStage: null, replayViewer: false, spectatorViewer: false});
    if (replayReturnProduct) replayRoomEpoch = null;
    const returnedRoom = roomSession?.service.getSnapshot();
    if (returnedRoom?.room && returnedRoom.room.phase === 'lobby')
      queueMicrotask(() => {
        const currentRoom = roomSession?.service.getSnapshot();
        if (currentRoom?.epoch === returnedRoom.epoch && currentRoom.room?.code === returnedRoom.room?.code)
          roomSession?.service.resourcesChanged();
      });
    return true;
  }
  const gameData = createGameDataImportModel({acquisition, translate: t,
    getContext: () => ({product: settings.getSnapshot()?.context.productId ?? DEFAULT_PRODUCT_ID,
      roomCode: roomCodeFromUrl(window.location.href) || null, replayViewer: snapshot.replayViewer,
      runtimeEpoch: runtime?.getSnapshot().epoch ?? null,
      runtimeReady: runtime?.getSnapshot().ready === true && !roomPreload, launched: runtime?.getSnapshot().launched === true,
      playerOpen: snapshot.playerOpen, importServer: metadata.getSnapshot().siteManifest?.shared.resourceMode === 'import',
      fallback: metadata.getSnapshot().siteManifest?.shared.gameDataFallback ?? null}),
    ports: {
      pickFile: ({accept}) => ports.pickFile(accept), feedback, showGameDataTransfer: transfer.revealGameData,
      packageInstalled(context) {
        const room = roomSession?.service.getSnapshot();
        if (room && room.product === context.product && room.room?.code === context.roomCode && room.preparation?.status === 'ready')
          roomSession!.service.resourcesChanged();
      },
      closePlayer: async () => {const titleOverlayWasOpen = titleNetwork.getSnapshot().open; if (!await requestRuntimeClose()) return false; requiredNavigation().completeRuntimeClose({titleOverlayWasOpen}); return true;},
      resetUnlaunchedRuntime: () => {if (runtime && !runtime.getSnapshot().launched && runtime.getSnapshot().epoch !== null) runtime.cancel();},
      captureUnlaunchedRuntimeReset() {
        const owner = runtime, controller = launchAbort;
        return async expectedEpoch => {
          if (!owner || runtime !== owner || launchAbort !== controller) return;
          const current = owner.getSnapshot();
          if (current.launched || current.epoch !== expectedEpoch) return;
          controller?.abort(new RuntimeSessionSupersededError());
          if (!await owner.close({decide: closeDecision})) throw new Error('Runtime retirement was not completed');
        };
      },
      openPlayer(product) {requiredNavigation().openPlayer(product); publish({playerOpen: true});},
      launchConfigured: () => {const product = settings.getSnapshot()?.context.productId ?? DEFAULT_PRODUCT_ID; return configuredLaunch(product, {replayPath: snapshot.replayViewer ? selectedReplayPath : undefined, multiplayer: isMultiplayerProductId(product) ? snapshot.replayViewer ? {kind: 'replay'} : roomSession?.service.getSnapshot().room ? {kind: 'room', options: roomSession.service.runtimeOptions()} : undefined : undefined});},
      roomPreparationForImport(continuation) {
        const owner = roomSession?.service, captured = owner?.getSnapshot();
        // Main app9664–9668 resumes only the matching interrupted room
        // preparation. An ordinary import must not cancel a ready/loading room.
        if (!owner || !captured?.room || continuation?.kind !== 'install-only' || !captured.product ||
            gameIdForProduct(captured.product) !== settings.getSnapshot()?.gameId ||
            !['cancelled', 'failed', 'importing'].includes(captured.preparation?.status ?? '')) return null;
        const current = () => owner.getSnapshot().epoch === captured.epoch && owner.getSnapshot().room?.code === captured.room?.code;
        return {markImporting() {if (current()) owner.setImporting(true);}, cancelIfImporting() {if (current() && owner.getSnapshot().preparation?.status === 'importing') owner.cancelImport();}, async resume() {if (current()) {owner.setImporting(false); await owner.prepareResources();}}};
      },
      beginBlockingDownload({label, onCancel}) {
        const operation = {label, async cancel() {if (blockingDownload !== operation) return; blockingDownload = null; transfer.setCancellation(null); launchAbort?.abort(new DOMException('已取消下载', 'AbortError')); runtime?.postInput('network-cancel', {}); await onCancel();}};
        blockingDownload = operation; transfer.setCancellation(label);
        return {finish() {if (blockingDownload === operation) {blockingDownload = null; transfer.setCancellation(null);}}, cancel: operation.cancel};
      },
      onActivityChange: () => publish({}),
    },
  });
  const titleNetwork = createTitleNetwork({context: () => ({game: runtime?.getSnapshot().game ?? activeSettings?.gameId ?? 'th06',
    product: roomSession?.service.getSnapshot().product ?? activeSettings?.context.productId ?? DEFAULT_PRODUCT_ID,
    launched: runtime?.getSnapshot().launched === true, relayUrl: metadata.getSnapshot().siteManifest?.shared.netplayRelay ?? ''}),
    translate: stringTranslate, toast: feedback.toast, randomValues: value => {const bytes = new Uint32Array(value.length); window.crypto.getRandomValues(bytes); value.set(bytes); return value;},
    sendNetworkCancel: () => requiredRuntime().cancelTitleNetwork(),
    enterRoom(product, code, created) {if (isMultiplayerProductId(product)) roomSession!.service.enterRoom(product, code, created);},
    leaveRoom() {const game = runtime?.getSnapshot().game; roomSession?.service.resetRoomWithoutNavigation(); if (game) requiredNavigation().leaveTitleRoomRoute(game);},
  });
  const unsubscribeTitle = titleNetwork.subscribe(() => {const state = titleNetwork.getSnapshot(); document.getElementById('player')?.classList.toggle('th09-network-open', state.open); publish({th09NetworkOverlayOpen: state.open}); roomSession?.sync();});
  const fullscreenChanged = () => {publish({}); if (nativeTouch.isFullscreen()) void nativeTouch.probeOrientation().then(() => publish({}));};
  document.addEventListener('fullscreenchange', fullscreenChanged); document.addEventListener('webkitfullscreenchange', fullscreenChanged);
  const settingsActions: SettingsActions = {
    confirm: decisions.askConfirmation, externalMidi, gamepad, keyboard:keyboardPreferences, th09Local:th09LocalInput, reportError,
    showAppleNotice: () => requiredNavigation().openInfoDialog('appleRefreshDialog'),
    feedback(message, status) {if (message) feedback.toast(message); if (status) feedback.status(status);},
    file: (action, product) => activity(async () => {if (!files) throw new Error('File service is not attached'); await files.run(action, product);}),
  };
  function attachFrame(element: HTMLIFrameElement | null) {
    if (!element || element === frame) return;
    if (frame) throw new Error('Launcher cannot replace its live Runtime iframe');
    frame = element;
    unbindPixelPerfectDisplay = bindPixelPerfectDisplay(element, () => activeSettings?.options.pixelPerfectEnabled === true);
    runtime = createRuntimeService({frame: element, hostWindow: window, baseUrl, translate: t, network, worker: appShellDeployment ? appShell!.activeWorker() : undefined, onLocalResourceProgress: transfer.localProgress, onWarning: reportError,
      onUpdateReady: (game, epoch) => {
        if (activeSettings?.gameId === game && runtime?.getSnapshot().epoch === epoch)
          gameUpdates?.ready({product:activeSettings.context.productId, gameVersion:activeSettings.gameVersion, epoch});
      },
      onFrameRetired() {
        // Reuse the same React-owned node, but discard the completed game's
        // browsing context and its child-history entries before the next launch.
        const parent = element.parentNode, next = element.nextSibling;
        if (element.isConnected && parent) {element.remove(); parent.insertBefore(element, next);}
      },
      onEvent: (message, context) => {const ownsPresentation = !context.fileOnly || fileRuntimePresentation?.() !== false; if (ownsPresentation) transfer.runtimeEvent(message); if (ownsPresentation && message.event === 'ready') {gameData.finish(); feedback.playerStatus(t('runtime.readyStatus')); const selected = activeSettings ?? settings.getSnapshot(); if (selected) feedback.status(t('runtime.readyWithMusic', {game: selected.gameId.toUpperCase(), music: selected.music === 'midi' ? 'midi' : t(selected.music === 'none' ? 'settings.music.none' : selected.music === 'ogg-full' ? 'settings.music.oggFull' : 'settings.music.oggStream')}));}
      if (message.event === 'network-request') titleNetwork.open();
      if (message.event === 'runtime-info' && message.netplayTiming) {calibration.record(message.netplayTiming, runtime!.getSnapshot().epoch ?? 0); roomSession?.service.acceptMeasuredTiming(message.netplayTiming);}
      if (message.event === 'runtime-info' && message.th09Match) roomSession?.service.acceptTh09Match(message.th09Match);
      if (message.event === 'notice' && typeof message.message === 'string' && message.message) feedback.toast(message.message);
      if (message.event === 'room-menu' && runtime?.getSnapshot().roomPreview && !runtime.getSnapshot().launched) {
        activeSettings = null; transfer.hide(); startupError.close();
        publish({playerOpen:false,touchHelpOpen:false,startupError:null,roomLaunchStage:null,spectatorViewer:false});
        requiredNavigation().completeRuntimeClose({titleOverlayWasOpen:false});
      }
      if (ownsPresentation && message.event === 'error') feedback.playerStatus(errorText(message.error || t('runtime.startFailed')));
      if (ownsPresentation && message.event === 'exit') {
        feedback.playerStatus(t(message.status === 'success' ? 'runtime.gameExited' : 'runtime.gameExitedAbnormally'));
        const titleOverlayWasOpen = titleNetwork.getSnapshot().open;
        const roomAtExit = roomSession?.service.getSnapshot();
        const leaveRoom = !context.fileOnly && message.returnToMenu === true && roomAtExit?.product && isMultiplayerProductId(roomAtExit.product);
        void requestRuntimeClose({nativeExit: true}).then(closed => {
          if (!closed) return;
          requiredNavigation().completeRuntimeClose({titleOverlayWasOpen});
          // Main's explicit native Return to Menu retires membership after the
          // Player closes. An unrelated/newer room must never inherit that exit.
          const currentRoom = roomSession?.service.getSnapshot();
          if (leaveRoom && currentRoom?.epoch === roomAtExit?.epoch && currentRoom?.room?.code === roomAtExit?.room?.code)
            roomSession?.service.leave();
        }).catch(reportError);
      }}});
    const browser = describeBrowserEnvironment({userAgent: window.navigator.userAgent, platform: window.navigator.platform,
      userAgentDataPlatform: (window.navigator as Navigator & {userAgentData?: {platform?: string}}).userAgentData?.platform,
      mobile, brave: !!(window.navigator as Navigator & {brave?: unknown}).brave});
    diagnostics = createRuntimeDiagnostics({runtime, frame: element, window, storage, translate: stringTranslate,
      browser: () => browser.browser, testBuild: () => metadata.getSnapshot().siteManifest?.shared.testBuild === true,
      frameLimit60: () => activeSettings?.options.frameLimit60Enabled === true});
    gameplayNetwork = createGameplayNetwork({runtime, timers: window, english: () => snapshot.locale === 'en', translate: stringTranslate,
      context: () => {
        const control = runtime?.getLauncherControlContext(), config = control?.options;
        const room = roomSession?.service.getSnapshot();
        const product = activeSettings?.context.productId ?? DEFAULT_PRODUCT_ID;
        return {product, runtimeVariant: control?.runtimeVariant ?? 'normal', roomCode: room?.room?.code ?? null,
          player: config?.netplayPlayer ?? 0, playerCount: config?.netplayPlayerCount ?? 2,
          inputDelay: config?.netplayInputDelay ?? 0, adonisMode: config?.netplayAdonisMode ?? 0,
          adonisSupported: multiplayerConfigForProduct(product)?.inputTiming?.measuredStartup === true,
          replayViewer: snapshot.replayViewer, spectator: config?.netplaySpectator === true};
      }});
    const telemetry = createNetplayTelemetry({origin: window.location.origin,
      endpoint: () => metadata.getSnapshot().siteManifest?.shared.netplayTelemetry,
      snapshot: () => {
        const state = runtime?.getSnapshot(), token = runtime?.getMidiEventContext();
        const net = gameplayNetwork?.readNativeSnapshot(), config = runtime?.getLauncherControlContext()?.options;
        if (disposed || !state?.launched || !token || !net?.active || net.failed || net.frame == null || net.frame < 0 || net.frame === 4294967295) return null;
        return {identity: token.document, product: activeSettings?.context.productId ?? DEFAULT_PRODUCT_ID,
          players: config?.netplayPlayerCount ?? 2, spectator: net.spectator, transport: net.transport, peers: net.peerState?.peers ?? []};
      }});
    telemetryTimer = window.setInterval(() => {void telemetry.tick();}, 1000);
    midi = createMidiBridge({runtime, external: externalMidi, loadSynth: ports.loadSynth, webAudioAvailable: audio, translate: t,
      frame: element, window, document, playerOpen: () => snapshot.playerOpen});
    unsubscribeRuntime = runtime.subscribe(() => {const current = runtime!.getSnapshot(); if (calibration.getSnapshot().epoch !== (current.epoch ?? 0)) calibration.reset(current.epoch ?? 0); spectatorPlayback.update(current, current.runtimeInfo.spectatorPlayback); publish({runtime: current}); roomSession?.sync(); if (invalidatedSettingsEpoch !== null) queueMicrotask(retireChangedSettings);});
    const replayStorage = createReplayStorageReader(() => ({indexedDB: window.indexedDB, keyRange: (window as Window & {IDBKeyRange: typeof IDBKeyRange}).IDBKeyRange}));
    replays = createReplayModel({runtime, prepareFiles, prepareViewer: prepareReplayWorkspace, releasePrepared, translate: stringTranslate,
      storage: {async read(product, selection) {
        const owner = requiredRuntime(), state = owner.getSnapshot(), variant = isMultiplayerProductId(product) ? 'multiplayer' : 'normal';
        // Flush only an already-owned matching Runtime. Never start/replace a
        // game to list or download, and never hydrate an active writer from IDB.
        if (state.ready && state.game === gameIdForProduct(product) && state.runtimeVariant === variant && state.epoch !== null) {
          await owner.withFileSession(state.game, access => access.sync(), {readOnly: true, runtimeVariant: variant, epoch: state.epoch});
        }
        return replayStorage.read(product, {...selection, language: activeSettings?.context.productId === product ? activeSettings.language : capturedSettings(product).language});
      }},
      prompt: (message, initial) => window.prompt(message, initial), confirm: decisions.askConfirmation,
      download: ports.download, toast: feedback.toast, afterPaint: ports.afterPaint,
      closeViewer: () => {if (snapshot.replayViewer) void requestRuntimeClose().catch(reportError);},
      afterClose: () => {
        if (replayRoomEpoch !== null && !runtime?.getSnapshot().launched) {
          if (roomSession?.service.getSnapshot().epoch === replayRoomEpoch) roomSession.service.resourcesChanged();
          replayRoomEpoch = null;
        }
        if (snapshot.playerOpen && !snapshot.replayViewer) frame?.focus({preventScroll: true});
      },
      inspect: async (_product, path) => (await requiredRuntime().send('playback', {action: 'inspect', path})).description,
      watch: async (_product, path, stage) => {await requiredRuntime().send('playback', {action: 'open', path, ...(stage !== undefined ? {stage} : {})});}});
    files = createFileActions({runtime, prepareFiles, releasePrepared, translate: stringTranslate, confirm: decisions.askConfirmation,
      pickFile: ports.pickFile, download: ports.download, feedback, replayMutations: replays.mutations,
      replayManager: {open: async product => {requiredNavigation().openInfoDialog('replayDialog', {replace: true}); await replays!.open(product);},
        refresh: () => replays!.refresh({afterMutation: true}), isOpen: () => replays!.getSnapshot().open, close: () => requiredNavigation().closeInfoDialog('replayDialog')},
      captureImportContext: product => {
        const playerIntent = playerIntentRevision;
        const capturedEpoch = requiredRuntime().getSnapshot().epoch;
        return {
          ownsPresentation: captureFilePresentation(playerIntent),
          assertRuntimeOwnership() {
            const current = requiredRuntime().getSnapshot();
            const ownsFileRuntime = current.epoch !== null && temporaryFiles.get(product) === current.epoch &&
              current.game === gameIdForProduct(product) && current.runtimeVariant === (isMultiplayerProductId(product) ? 'multiplayer' : 'normal') && !current.launched;
            if (disposed || playerIntent !== playerIntentRevision && current.epoch !== null && current.epoch !== capturedEpoch && !ownsFileRuntime) throw new RuntimeSessionSupersededError();
          },
          complete() {
            if (disposed) return;
            // Main app6393–6397 hides Player only after retirement and Replay
            // refresh succeed. An awaited refresh cannot hide a newer Start.
            if (playerIntent === playerIntentRevision && !(snapshot.replayViewer)) publish({playerOpen: false});
            void metadata.refreshInstalled(gameIdForProduct(product));
          },
        };
      }});
    publish({runtime: runtime.getSnapshot()});
  }
  async function confirmMusicWarnings() {
    const state = settings.getSnapshot(); if (!state) return false;
    const music = state.music;
    if (music === 'none' || music === 'midi') return decisions.askConfirmation({message: t(music === 'none' ? 'music.noneLaunchWarning' : 'music.midiLaunchWarning'), confirmText: t('music.startAnyway')});
    return true;
  }
  function preparationIntent(product: ProductId, multiplayer?: MultiplayerPreparation, externalSignal?: AbortSignal, replayPath?: string) {
    const owner = requiredRuntime(), clickSettings = capturedSettings(product), controller = new AbortController();
    const linked = linkAbortSignals(controller.signal, externalSignal), signal = linked.signal;
    let directAttemptId: number | null = null;
    launchAbort = controller; activeSettings = clickSettings; updateAbort = new AbortController(); musicAbort = new AbortController();
    startupError.close();
    const cancellation = () => {if (launchAbort === controller) {transfer.setCancellation(updating ? t('package.cancelUpdate') : downloadingMusic ? t('music.cancelDownload') : downloadingPackage || downloadingLanguage ? t('package.cancelDownload') : blockingDownload?.label ?? null); publish({});}};
    const input: GamePreparationInput & {runtime: RuntimeService; signal: AbortSignal} = {productId: product, settings: clickSettings, acquisition, baseUrl, translate: t,
      signal, multiplayer, replayPath, updateSignal: updateAbort.signal, musicDownloadSignal: musicAbort.signal,
      onUpdateActivity: value => {if (launchAbort === controller) {updating = value; cancellation();}},
      onMusicDownloadActivity: value => {if (launchAbort === controller) {downloadingMusic = value; cancellation();}},
      onAcquisitionActivity: value => {if (launchAbort === controller) {downloadingPackage = value; cancellation();}},
      onLanguageDownloadActivity: value => {if (launchAbort === controller) {downloadingLanguage = value; cancellation();}},
      onTransferHide: transfer.hide, onLocalMusicFailure: transfer.localFailure, touchLayout: touchLayout.getSnapshot().saved, runtime: owner,
      onRuntimePlan: plan => {updating = false; transfer.beginRuntime(plan); if (!plan.generation && plan.directPreloadHost) {gameData.beginDirectDownload(); directAttemptId = gameData.getSnapshot().attempt?.id ?? null;}},
      prepareMidi: music => midi!.prepare(music), decideUpdate: value => decisions.askDecision(value),
      onProgress: transfer.preparationProgress, onWarning: feedback.toast, onStatus: feedback.playerStatus,
      onUpdateReady: epoch => gameUpdates?.ready({product, gameVersion:clickSettings.gameVersion, epoch}),
      onBackgroundError: (error, message) => {if (message) console.warn(message, error); else console.warn(error);}};
    return {owner, clickSettings, controller, input,
      recoverDirectPreloadTimeout(error: unknown) {
        // Main app5979–5982 unlocks the current direct preload before showing
        // its startup failure. Runtime may already have reset its failed epoch;
        // the existing launch/attempt identities fence this presentation instead.
        if (launchAbort === controller && !signal.aborted && directAttemptId !== null &&
            gameData.getSnapshot().attempt?.id === directAttemptId && /超时/.test(errorText(error))) gameData.unlock(t('runtime.loadTimedOutImport'));
      },
      finish() {linked.dispose(); if (launchAbort === controller) {launchAbort = null; updateAbort = null; musicAbort = null; updating = false; downloadingMusic = false; downloadingPackage = false; downloadingLanguage = false; transfer.setCancellation(null); publish({});}}};
  }
  async function configuredLaunch(product: ProductId, intent: {multiplayer?: MultiplayerPreparation; replayPath?: string; replayWorkspace?: boolean; signal?: AbortSignal; awaitFirstFrame?: boolean; throwErrors?: boolean} = {}) {
    const operation = preparationIntent(product, intent.multiplayer, intent.signal, intent.replayPath), {owner, clickSettings, controller} = operation;
    try {
      if (owner.getSnapshot().epoch !== null && !await owner.close({decide: closeDecision})) return;
      const prepared = await prepareGame({...operation.input, replayWorkspace: intent.replayWorkspace});
      // Main publishes readiness and starts optional background work on ACK,
      // before the distinct first-frame wait used by room launch covers.
      let acknowledged = false;
      const onAcknowledged = () => {
        const current = owner.getSnapshot();
        if (acknowledged || disposed || controller.signal.aborted || current.epoch !== prepared.runtimeSnapshot.epoch || !current.launched) return;
        acknowledged = true; transfer.launchAcknowledged(); feedback.playerStatus(t('runtime.ready')); frame?.focus({preventScroll: true}); transfer.setCancellation(null);
        void prepared.startBackground().catch(error => {if (!disposed && owner.getSnapshot().epoch === current.epoch) reportError(error);});
      };
      const unsubscribe = owner.subscribe(onAcknowledged);
      try {await owner.launch({awaitFirstFrame: intent.awaitFirstFrame}); onAcknowledged();} finally {unsubscribe();}
    } catch (error) {
      if (error instanceof RuntimeSessionSupersededError || controller.signal.reason instanceof RuntimeSessionSupersededError) return;
      operation.recoverDirectPreloadTimeout(error);
      if (intent.throwErrors) throw error;
      if (await gameData.handleLaunchFailure(error)) return;
      publish({startupError: error}); startupError.show(error, `${clickSettings.gameId.toUpperCase()} / ${clickSettings.music === 'midi' ? 'midi' : t(clickSettings.music === 'none' ? 'settings.music.none' : clickSettings.music === 'ogg-full' ? 'settings.music.oggFull' : 'settings.music.oggStream')}`); feedback.playerStatus(errorText(error)); feedback.toast(errorText(error));
    } finally {operation.finish();}
  }
  async function showRoomPlayer(context: RoomOperationContext, fullscreen: boolean, allowHelp = false) {
    if (!context.isCurrent()) throw new RuntimeSessionSupersededError();
    activeSettings = capturedSettings(context.product); if (fullscreen) requiredNavigation().openPlayer(context.product);
    let touchHelpOpen = false; if (allowHelp && activeSettings.options.touchEnabled && !helpSeen) {helpSeen = true; touchHelpOpen = true; try {storage?.setItem('eagler-touch-help-seen-v8', '1');} catch {}}
    publish({playerOpen: true, touchHelpOpen, replayViewer: false, startupError: null});
    const player = document.querySelector<HTMLElement>('#player');
    if (!player) throw new Error('Launcher Player is not mounted');
    player.classList.add('open'); player.setAttribute('aria-hidden', 'false'); document.body.classList.add('player-active');
    if (fullscreen) {try {await nativeTouch.enterFullscreen(player);} catch (error) {feedback.toast(t('fullscreen.autoBlocked', {reason: errorText(error)}));}}
    if (!context.isCurrent()) throw new RuntimeSessionSupersededError();
  }
  async function launchRoom(context: RoomLaunchContext) {
    const fromTitle = snapshot.th09NetworkOverlayOpen;
    if (!fromTitle) {
      await roomPreloadTask;
      const current = requiredRuntime().getSnapshot(), preloaded = roomPreload;
      const retainedTitle = current.roomPreview && current.game === gameIdForProduct(context.product) && current.runtimeVariant === 'multiplayer';
      if (current.phase !== 'prepared' || (!retainedTitle && (!preloaded || preloaded.product !== context.product || preloaded.epoch !== current.epoch))) {
        // Never replace a missing/stale native preload with a cold launch after
        // synchronized Start. Return the room to its preparation gate instead.
        roomSession?.service.resourcesChanged();
        throw new Error(t('room.resourcesUnavailable'));
      }
    }
    // Native title previews already completed this consent before room-ready;
    // never put a fresh dialog between synchronized Start and network binding.
    if (context.snapshot.seat != null && !requiredRuntime().getSnapshot().roomPreview && !await confirmMusicWarnings()) return;
    if (!context.isCurrent()) throw new RuntimeSessionSupersededError();
    const cover = !fromTitle;
    if (fromTitle) {
      const owner = requiredRuntime();
      if (owner.getSnapshot().runtimeVariant === 'multiplayer' && owner.getSnapshot().roomReturnSupported) await owner.returnToRoom();
      else {await owner.sync(); if (!context.isCurrent()) throw new RuntimeSessionSupersededError(); await owner.close({discardUnsaved:true});}
      titleNetwork.close(false);
    }
    requiredNavigation().closeRoomPanel(); requiredNavigation().closeRoomSettings();
    publish({roomLaunchStage: cover ? 'runtime' : null});
    await showRoomPlayer(context, mobile, context.snapshot.seat != null);
    publish({spectatorViewer: context.snapshot.spectatorRequested});
    const owner = requiredRuntime(), preloaded = roomPreload;
    const current = owner.getSnapshot();
    if (current.phase === 'prepared' && current.epoch !== null &&
        (current.roomPreview && current.game === gameIdForProduct(context.product) && current.runtimeVariant === 'multiplayer' ||
          preloaded?.product === context.product && preloaded.epoch === current.epoch)) {
      await owner.configurePreparedRoom(current.epoch, context.runtimeOptions);
      if (!context.isCurrent()) throw new RuntimeSessionSupersededError();
      await owner.launch({awaitFirstFrame: cover});
      if (preloaded?.epoch === current.epoch) void preloaded.prepared.startBackground().catch(reportError);
      // The prepared plan and permanent iframe remain the room's title owner.
      transfer.launchAcknowledged(); feedback.playerStatus(t('runtime.ready')); frame?.focus({preventScroll: true});
    } else if (fromTitle) await configuredLaunch(context.product, {signal: context.signal, multiplayer: {kind: 'room', options: context.runtimeOptions}, awaitFirstFrame: cover, throwErrors: true});
    else throw new Error(t('room.resourcesUnavailable'));
    if (!context.isCurrent()) throw new RuntimeSessionSupersededError();
    if (cover) {publish({roomLaunchStage: 'path'}); await gameplayNetwork!.waitForGameplayPath({isCurrent: context.isCurrent});}
    publish({roomLaunchStage: null});
  }
  async function checkRoom(context: RoomOperationContext) {
    const owner = requiredRuntime();
    await roomPreloadTask;
    const current = owner.getSnapshot();
    if (current.phase === 'prepared' && current.epoch !== null && current.runtimeVariant === 'multiplayer' &&
        current.game === gameIdForProduct(context.product)) {
      await owner.configurePreparedRoom(current.epoch, {}, {checkGame:true});
      if (!context.isCurrent()) throw new RuntimeSessionSupersededError();
      await showRoomPlayer(context, false);
      await owner.launch();
      if (context.isCurrent()) feedback.status(t('multiplayer.checkGamePassed'));
      return;
    }
    if (owner.getSnapshot().epoch !== null && !await owner.close({decide: closeDecision, discardUnsaved: !owner.getSnapshot().launched})) return;
    await showRoomPlayer(context, false); feedback.playerStatus(t('multiplayer.checkingGame'));
    const operation = preparationIntent(context.product, {kind: 'preflight'}, context.signal);
    try {
      await prepareGameCheck({...operation.input, multiplayer: {kind: 'preflight'}});
      if (context.isCurrent()) feedback.status(t('multiplayer.checkGamePassed'));
    } finally {operation.finish(); if (context.isCurrent()) {await nativeTouch.exitPlayerFullscreen().catch(() => {}); activeSettings = null; selectedReplayPath = undefined; publish({playerOpen: false, roomLaunchStage: null});}}
  }
  async function closeRoomPlayerWithoutSync() {
    launchAbort?.abort(new RuntimeSessionSupersededError());
    await requiredRuntime().close({discardUnsaved: true, beforeReset: () => nativeTouch.exitPlayerFullscreen().catch(() => {})});
    gameData.clear(); transfer.hide(); midi?.reset(); activeSettings = null;
    publish({playerOpen: false, touchHelpOpen: false, roomLaunchStage: null});
  }
  async function roomOperationFailed(error: unknown, kind: 'launch' | 'check', context: RoomOperationContext) {
    if (error instanceof RuntimeSessionSupersededError || !context.isCurrent()) return;
    const reason = errorText(error), owner = requiredRuntime();
    if (kind === 'check') {
      await closeRoomPlayerWithoutSync();
      if (isGameDataAcquisitionFailure(error)) gameData.beginManual({reason, kind: 'install-only'});
      else startupError.show(error, t('multiplayer.checkGame'));
      feedback.status(t('multiplayer.checkGameFailed', {reason})); return; // Room owner emits the original 4000 ms check toast once.
    }
    const cancelled = !!error && typeof error === 'object' && 'name' in error && error.name === 'AbortError' || /已取消下载/.test(reason);
    if (!owner.getSnapshot().launched && cancelled) {await closeRoomPlayerWithoutSync(); feedback.status(t('runtime.downloadCancelled')); return;}
    if (!owner.getSnapshot().launched && isGameDataAcquisitionFailure(error)) {
      publish({roomLaunchStage: null}); feedback.playerStatus(t('runtime.missingResourcesPlayer'));
      gameData.beginManual({reason, kind: 'launch'}); feedback.status(t('runtime.missingResourcesLauncher')); feedback.toast(reason); return;
    }
    if (snapshot.roomLaunchStage && snapshot.playerOpen) await closeRoomPlayerWithoutSync(); else publish({roomLaunchStage: null});
    feedback.status(reason); feedback.playerStatus(reason); feedback.toast(reason);
    const seat = roomSession?.service.getSnapshot().seat;
    startupError.show(error, seat == null ? t('runtime.multiplayerContext', {game: gameIdForProduct(context.product).toUpperCase()}) : t('runtime.multiplayerPlayerContext', {game: gameIdForProduct(context.product).toUpperCase(), player: seat + 1}));
  }
  let replayPreparation: Promise<void> | null = null;
  function prepareReplayWorkspace(product: ProductId): Promise<void> {
    if (!replays?.getSnapshot().open) return Promise.reject(new RuntimeSessionSupersededError());
    const owner = requiredRuntime(), state = owner.getSnapshot();
    if (snapshot.replayViewer && state.launched && state.game === gameIdForProduct(product) && state.runtimeVariant === (isMultiplayerProductId(product) ? 'multiplayer' : 'normal')) return Promise.resolve();
    if (replayPreparation) return replayPreparation;
    if (state.launched) return Promise.reject(new Error('请先退出当前游戏'));
    publish({playerOpen: true, replayViewer: true, touchHelpOpen: false});
    replayPreparation = configuredLaunch(product, {multiplayer: isMultiplayerProductId(product) ? {kind: 'replay'} : undefined, replayWorkspace: true, awaitFirstFrame: true, throwErrors: true}).then(() => {
      if (!replays?.getSnapshot().open || replays.getSnapshot().productId !== product || !owner.getSnapshot().launched) throw new RuntimeSessionSupersededError();
    }).catch(error => {
      const current = owner.getSnapshot();
      if (!current.launched && current.epoch === null && replays?.getSnapshot().productId === product) publish({playerOpen: false, replayViewer: false});
      throw error;
    }).finally(() => {replayPreparation = null;});
    return replayPreparation;
  }
  function openReplayViewer(product: ProductId, path?: string): Promise<void> {
    return (async () => {
      if (roomSession?.service.suspendPreparation()) replayRoomEpoch = roomSession.service.getSnapshot().epoch;
      if (!replays?.getSnapshot().open || replays.getSnapshot().productId !== product) {requiredNavigation().openInfoDialog('replayDialog', {replace: true}); await replays!.open(product);}
      if (path) {
        if (gameIdForProduct(product) === 'th09') await replays!.watch(path);
        else await replays!.inspect(path);
      }
    })();
  }
  function launch(product: ProductId): Promise<void> {
    if (launchPending) return launchPending;
    product = navigation?.syncSelectionFromRoute() ?? settings.getSnapshot()?.context.productId ?? product;
    launchPending = activity(async () => {
      const owner = requiredRuntime();
      if (owner.getSnapshot().launched) {frame?.focus({preventScroll: true}); return;}
      if (isMultiplayerProductId(product)) {await roomSession!.service.launch(); return;}
      if (metadata.getSnapshot().siteManifest?.shared.resourceMode === 'import' && !(await acquisition.readCurrent(gameIdForProduct(product))).generation) {
        publish({startupError: null}); feedback.status(t('package.needImport')); gameData.beginImportRequired(); return;
      }
      if (!await confirmMusicWarnings()) return;
      // Main9560–9569 reads business selection again after each awaited UI
      // boundary. A first Host may have replaced the provisional game while
      // its warning/fullscreen request was pending; the raw URL stays intact.
      product = settings.getSnapshot()?.context.productId ?? product;
      activeSettings = capturedSettings(product);
      requiredNavigation().openPlayer(product);
      let touchHelpOpen = false; if (activeSettings.options.touchEnabled && !helpSeen) {helpSeen = true; touchHelpOpen = true; try {storage?.setItem('eagler-touch-help-seen-v8', '1');} catch {}}
      publish({playerOpen: true, touchHelpOpen, startupError: null});
      const player = document.querySelector<HTMLElement>('#player');
      if (!player) throw new Error('Launcher Player is not mounted');
      player.classList.add('open'); player.setAttribute('aria-hidden', 'false'); document.body.classList.add('player-active');
      try {await nativeTouch.enterFullscreen(player);} catch (error) {feedback.toast(t('fullscreen.autoBlocked', {reason: errorText(error)}));}
      await configuredLaunch(settings.getSnapshot()?.context.productId ?? product);
    }).finally(() => {launchPending = null; gameUpdates?.flush();});
    return launchPending;
  }
  gameUpdates = createGameUpdatePrompt({decisions, translate:t,
    current() {
      const current = runtime?.getSnapshot();
      return current?.launched && current.epoch !== null && activeSettings
        ? {product:activeSettings.context.productId, gameVersion:activeSettings.gameVersion, epoch:current.epoch} : null;
    },
    busy() {
      const room = roomSession?.service.getSnapshot();
      return !!launchPending || snapshot.activity > 0 || !!fileDownload || !!blockingDownload ||
        runtime?.getSnapshot().fileOperationBusy === true || runtime?.getSnapshot().phase === 'saving' ||
        room?.launchBusy === true || !!room?.room && room.room.phase !== 'lobby' ||
        settings.getSnapshot()?.context.productId !== activeSettings?.context.productId ||
        settings.getSnapshot()?.gameVersion !== activeSettings?.gameVersion ||
        document.querySelector('dialog[open]') !== null;
    },
    restart(context: GameUpdateContext) {
      const replayWorkspace = snapshot.replayViewer, replayPath = selectedReplayPath;
      launchPending = activity(async () => {
        // Reuse native sync/close and the ordinary preparation owner. A failed
        // save can still choose to stay; never reload the Launcher document.
        if (isMultiplayerProductId(context.product) && roomSession?.service.getSnapshot().room) {
          // The title-room launch path owns its own sync/retirement sequence.
          if (!snapshot.th09NetworkOverlayOpen && !await requiredRuntime().close({decide:closeDecision})) return;
          await roomSession.service.launch();
        } else {
          await configuredLaunch(context.product, {replayWorkspace, replayPath,
            multiplayer:isMultiplayerProductId(context.product) ? {kind:'replay'} : undefined, throwErrors:true});
        }
      }).finally(() => {launchPending = null; gameUpdates?.flush();});
      return launchPending;
    }, onError:reportError,
  });
  document.documentElement.lang = snapshot.locale; document.documentElement.dataset.uiLocale = snapshot.locale;
  window.dispatchEvent(new CustomEvent('eagler-ui-locale-change', {detail: {locale: snapshot.locale}}));
  appShell = createAppShellService({baseUrl, deployment: appShellDeployment, serviceWorker: window.navigator.serviceWorker,
    secureContext: window.isSecureContext, translate: stringTranslate, online: () => window.navigator.onLine,
    reload: () => window.location.reload(), timers: window, fetchImpl: window.fetch.bind(window),
    activity: () => ({launched: runtime?.getSnapshot().launched === true, runtimeReady: runtime?.getSnapshot().ready === true,
      runtimeSessionActive: runtime?.getSnapshot().epoch != null, touchLayoutEditing: touchLayout.getSnapshot().isEditing,
      blockingOperation: !!blockingDownload || !!fileDownload || updating || downloadingMusic || downloadingPackage || downloadingLanguage, gameDataAttempt: !!gameData.getSnapshot().attempt,
      launchInFlight: snapshot.activity > 0 || !!launchPending || roomSession?.service.getSnapshot().launchBusy === true,
      decisionOpen: document.querySelector('dialog[open]') !== null, replayOpen: replays?.getSnapshot().open === true}),
  });
  const appShellOwner = appShell;
  const shellChanged = () => document.body.classList.toggle('app-shell-activation-pending', appShellOwner.getSnapshot().state.activationPending);
  const unsubscribeAppShell = appShellOwner.subscribe(shellChanged); shellChanged();
  const activityObserver = new MutationObserver(() => {appShellOwner.notifyActivityChanged(); gameUpdates?.flush();});
  activityObserver.observe(document.body, {subtree: true, attributes: true, attributeFilter: ['open']});
  const unsubscribeTouchActivity = touchLayout.subscribe(() => {
    if (touchLayout.getSnapshot().isEditing && snapshot.bootTouchPreview) publish({bootTouchPreview: null, bootTouchPreviewImage: undefined, playerOpen: false, touchHelpOpen: false});
    appShellOwner.notifyActivityChanged();
  });
  const metadataRetry = createMetadataRetry({metadata, window, shell: appShellOwner,
    markBoot: phase => (window as Window & {__eaglerBoot?: {mark(value: string): void}}).__eaglerBoot?.mark(phase),
    onSettled: () => {if (!runtime?.getSnapshot().launched && navigation && !navigation.hasSelection) navigation.syncSelectionFromRoute(); publish({metadataRevision: snapshot.metadataRevision + 1});},
  });
  const roomPreparation = createRoomPreparation({acquisition, metadataReady: metadata.initialize, baseUrl, translate: t,
    activeWorker: () => appShellOwner.activeWorker()});
  roomSession = createRoomSession({document, window, storage, identity, sessions, memberId: multiplayerMemberId,
    createSocket: url => new WebSocket(url), relayUrl: () => metadata.getSnapshot().siteManifest?.shared.netplayRelay ?? '',
    settingsModel: settings, sitePreferences: preferences, runtime: () => runtime, replayViewer: () => snapshot.replayViewer,
    titleOverlayOpen: () => snapshot.th09NetworkOverlayOpen,
    settings: () => ({touchEnabled: settings.getSnapshot()?.options.touchEnabled === true, touchMovementMode: settings.getSnapshot()?.options.touchMovementMode ?? DEFAULT_GAME_OPTIONS.touchMovementMode, ...(settings.getSnapshot()?.context.productId === 'th09mp' ? {replayAutoSave: settings.getSnapshot()?.replayAutoSave} : {}), mobileDevice: mobile, iceServers: [{urls: ['stun:stun.cloudflare.com:3478']}]}),
    setMovementMode: mode => settings.setOption('touchMovementMode', mode), decisions, translate: t, notify: feedback.toast,
    prepareResources(context, report) {
      const task = roomPreloadTask.catch(() => {}).then(async () => {
        if (!context.isCurrent()) return;
        // In-game TH09 entry already owns a running title engine. Do not close
        // it or take its FS ownership away just to warm a second engine.
        if (runtime?.getSnapshot().launched && !titleNetwork.getSnapshot().open) return roomPreparation.prepare(context, report);
        const prior = roomPreload, priorRuntime = runtime?.getSnapshot();
        const canReuse = prior?.product === context.product && prior.epoch === priorRuntime?.epoch && priorRuntime.phase === 'prepared';
        // A current-program check must not announce a download/reload to the
        // relay. If the identity changed, the real reload below reports it.
        const agreement = await waitWithAbort(roomPreparation.prepare(context, canReuse ? () => {} : report), context.signal);
        if (!context.isCurrent()) return;
        const cached = roomPreload, current = runtime?.getSnapshot();
        if (cached?.product === context.product && cached.epoch === current?.epoch && current.phase === 'prepared' &&
            cached.prepared.plan.multiplayerProgram?.generation === agreement?.runtimeGeneration) {
          // Ready verifies the current program without reloading unchanged DATA,
          // localization or music that the room already prepared on entry.
          return agreement;
        }
        const operation = preparationIntent(context.product, {kind: 'room-preload'}, context.signal);
        let decisionId: number | undefined;
        const cancelDecision = () => {if (decisionId !== undefined && decisions.getSnapshot()?.requestId === decisionId) decisions.resolve('cancel');};
        context.signal.addEventListener('abort', cancelDecision, {once: true});
        try {
          report({status: 'preparing', stage: 'package', percent: null, error: '', title: t('room.preparingResources')});
          const owner = operation.owner;
          if (owner.getSnapshot().launched) {
            await waitWithAbort(prepareRoomResourcePlan({...operation.input, decideUpdate(value) {
              const choice = decisions.askDecision(value); decisionId = decisions.getSnapshot()?.requestId;
              return choice.finally(() => {decisionId = undefined;});
            }}), context.signal);
            return agreement;
          }
          if (owner.getSnapshot().epoch !== null) await owner.close({discardUnsaved: true});
          if (!context.isCurrent()) return;
          const prepared = await waitWithAbort(prepareGame({...operation.input,
            multiplayer: {kind: 'room-preload', runtimeGeneration: agreement?.runtimeGeneration},
            decideUpdate(value) {
              const choice = decisions.askDecision(value); decisionId = decisions.getSnapshot()?.requestId;
              return choice.finally(() => {decisionId = undefined;});
            },
            onProgress(value) {if (!context.isCurrent()) return; operation.input.onProgress?.(value);
              const loaded = value.loaded ?? value.completed, total = value.totalBytes ?? value.total;
              report({status: 'preparing', stage: value.stage === 'package' ? 'package' : 'runtime',
                percent: total && loaded != null ? Math.max(0, Math.min(100, loaded / total * 100)) : null,
                ...(value.totalBytes ? {loaded: value.loaded, total: value.totalBytes} : {}), error: ''});},
            onRuntimePlan(plan) {operation.input.onRuntimePlan?.(plan); report({status: 'preparing', stage: 'runtime', percent: null, error: '', title: t('room.preparingRuntime')});},
          }), context.signal);
          if (!context.isCurrent()) return;
          if (owner.getSnapshot().roomPreviewSupported && roomSession?.service.getSnapshot().seat != null &&
              !await waitWithAbort(confirmMusicWarnings(), context.signal)) throw new Error(t('room.resourcesCancelled'));
          if (!context.isCurrent()) return;
          await waitWithAbort(owner.prepareRoomPreview(prepared.runtimeSnapshot.epoch!), context.signal);
          if (!context.isCurrent()) return;
          const updateReady = await waitWithAbort(owner.checkPreparedCodeUpdate(prepared.runtimeSnapshot.epoch!), context.signal);
          if (!context.isCurrent()) return;
          if (updateReady) feedback.toast(t('update.readyMessage'), 6000);
          roomPreload = {product: context.product, epoch: prepared.runtimeSnapshot.epoch!, prepared};
          transfer.hide();
          return agreement;
        } finally {context.signal.removeEventListener('abort', cancelDecision); operation.finish();}
      });
      roomPreloadTask = task;
      return task;
    },
    releasePreparedResources() {
      const preloaded = roomPreload; roomPreload = null;
      if (preloaded && runtime?.getSnapshot().epoch === preloaded.epoch && !runtime.getSnapshot().launched && !runtime.getSnapshot().fileOperationBusy)
        runtime.cancel();
    },
    preparationFailed(error) {if (isGameDataAcquisitionFailure(error)) gameData.beginManual({reason: errorText(error), kind: 'install-only'}); else feedback.toast(errorText(error), 4000);},
    beginManualImport: () => gameData.beginManual(),
    beforePreparationRetry() {const attempt = gameData.getSnapshot().attempt; if (attempt?.manual && attempt.continuation?.kind === 'install-only') gameData.clear();},
    launch: launchRoom, checkGame: checkRoom,
    canLaunchFromTitle: () => titleNetwork.getSnapshot().open, operationFailed: roomOperationFailed, isLaunched: () => runtime?.getSnapshot().launched === true,
    routes: {restore: input => requiredNavigation().restoreRoomRoute(input), enter: input => {requiredNavigation().enterRoomRoute(input); feedback.translatedStatus(() => t(input.created ? 'status.roomCreated' : 'status.roomJoined', {code: input.code}));},
      settleInvite: () => requiredNavigation().settleRoomInvite(), leave: input => {
        // Main758–761 carries terminal closure copy into the directory's
        // persistent notice. Store it before that destination resumes.
        if (input.message) {
          try {sessionStorage?.setItem('eagler-lobby-message', input.message);} catch {}
        }
        const destination = requiredNavigation().leaveRoomRoute(input);
        if (!input.message && destination === 'options') {
          pendingRoomReturnProduct = settings.getSnapshot()!.context.productId;
          if (roomReturnTimer !== null) window.clearTimeout(roomReturnTimer);
          roomReturnTimer = null;
          const reduced = preferences.getSnapshot().lessMotion || window.matchMedia('(prefers-reduced-motion: reduce)').matches;
          document.body.classList.toggle('mp-room-returning', !reduced);
          if (!reduced) roomReturnTimer = window.setTimeout(() => {document.body.classList.remove('mp-room-returning'); roomReturnTimer = null;}, 180);
          publish({});
        }
        if (input.message && destination === 'options') feedback.toast(input.message);
      }},
  });
  const room = roomSession;
  let spectatorEntry = '';
  const unsubscribeRoom = room.service.subscribe(() => {
    const current = room.service.getSnapshot();
    if (roomSettingsBaseline && (!current.room || current.epoch !== roomSettingsBaseline.epoch)) {
      roomSettingsBaseline = null;
      publish({roomReloadPending: false});
    }
    settings.setMovementRestriction(!!current.room?.disableCheatMovement && current.seat != null);
    const entry = current.spectatorRequested && current.room ? `${current.product}:${current.room.code}:${current.startSerial}` : '';
    if (entry !== spectatorEntry) {spectatorEntry = entry; publish({spectatorViewer: !!entry});}
    else publish({});
  });
  function restoreRoomFromUrl(url: string): Promise<void> {
    const expected = resolveRoomInvite(url); if (!expected?.r) return Promise.resolve();
    const existing = room.service.getSnapshot();
    // Retained membership is independent of later Host business fallback.
    if (existing.room?.code === expected.r) return Promise.resolve();
    const businessProduct = settings.getSnapshot()!.context.productId;
    const selected = isMultiplayerProductId(businessProduct) ? businessProduct : DEFAULT_MULTIPLAYER_PRODUCT_ID;
    if (!disposed) {
      settings.hydrate(settingsContext(selected, snapshot.locale));
      room.service.restoreInvite(url, selected);
    }
    return Promise.resolve();
  }
  let initializationPromise: Promise<void> | null = null;
  function initialize() {
    if (initializationPromise) return initializationPromise;
    // Initial main room restoration is synchronous and precedes the first
    // asynchronous Host application; the metadata owner reconnects afterward.
    if (!directoryEntry) void restoreRoomFromUrl(entryUrl.href);
    localPackageMaintenance.schedule();
    initializationPromise = metadataRetry.initialize();
    return initializationPromise;
  }
  return {
    spectatorPlayback,
    async controlSpectator(action: SpectatorPlaybackAction, frame?: number, run?: string) {
      const owner = requiredRuntime(), current = owner.getSnapshot();
      if (!current.launched || !current.spectator || current.game === 'th09') return;
      const state = spectatorPlayback.getSnapshot();
      if (run !== undefined && run !== state?.run) return;
      const command = spectatorPlaybackCommand(state, action, frame);
      if (command) await owner.send('spectator-playback', command);
    },
    room, titleNetwork, calibration, networkDiagnostics, appShell: appShellOwner, baseUrl, storage, mobile, metadata, settingsContext, acquisition, directory, gameData, transfer, startupError, startup, settings, gamepad, keyboardPreferences, th09LocalInput, touchLayout, preferences, feedback, decisions, nativeTouch, settingsActions,
    subscribe(listener: () => void) {listeners.add(listener); return () => {listeners.delete(listener);};}, getSnapshot: () => snapshot,
    initialize, attachFrame, requestRuntimeClose, launch, openReplayViewer,
    restoreRoomFromUrl,
    reloadRoomResources() {
      if (!roomSettingsBaseline) return;
      roomSettingsBaseline = null;
      publish({roomReloadPending: false});
      room.service.resumeAfterSettingsReload();
    },
    restoreRoomSettings() {
      const baseline = roomSettingsBaseline;
      if (!baseline || room.service.getSnapshot().epoch !== baseline.epoch) return;
      settings.restoreRoomSettings(baseline.settings);
      roomSettingsBaseline = null;
      publish({roomReloadPending: false});
      void metadata.refreshInstalled(baseline.settings.gameId);
      room.service.resumeAfterSettingsReload(true);
    },
    requestRoomLeave: () => {if (titleNetwork.getSnapshot().open) titleNetwork.leave(); else room.service.leave();},
    async leaveRoom() {
      if (titleNetwork.getSnapshot().open) {titleNetwork.leave(); return;}
      if (snapshot.roomLaunchStage && snapshot.playerOpen) {launchAbort?.abort(new RuntimeSessionSupersededError()); await requiredRuntime().close({discardUnsaved: true, beforeReset: () => nativeTouch.exitPlayerFullscreen().catch(() => {})}); activeSettings = null; selectedReplayPath = undefined; publish({playerOpen: false, roomLaunchStage: null});}
      room.service.leave();
    },
    async copyRoomCode(code: string) {feedback.toast(await ports.copyText(code) ? t('status.roomCodeCopied') : t('status.roomCode', {code}));},
    bindNavigation(value: BrowserNavigationPorts) {navigation = value; if (pendingHostSelection) {value.applyHostSelection(pendingHostSelection); pendingHostSelection = null;} return () => {if (navigation === value) navigation = null;};},
    // The directory can offer MP settings before a hosted MP Runtime exists.
    // Acquisition/launch retains its own resource gate, including local import.
    productAvailable: (product: ProductId) => metadata.getSnapshot().products.includes(product) ||
      isMultiplayerProductId(product) && directory.getSnapshot().products.includes(product),
    reportProductSelection(product: ProductId, changed: boolean) {
      const title = PRODUCT_GAMES[gameIdForProduct(product)].title;
      feedback.translatedStatus(() => t(changed ? 'status.switchedProduct' : 'status.selectedProduct', {product: title}));
    },
    selectRouteProduct(product: ProductId, context?: SurfaceContext) {
      if (settings.getSnapshot()?.context.productId !== product || context === 'lobby') settings.hydrate(settingsContext(product, snapshot.locale), {resetDisclosure: context === 'lobby'});
    },
    settleRoomReturnPresentation(surface: Surface, product: ProductId | null) {
      if (!pendingRoomReturnProduct || surface === 'room' || surface === 'touch') return;
      const expected = pendingRoomReturnProduct; pendingRoomReturnProduct = null;
      if (surface !== 'options' || product !== expected) return;
      document.getElementById('libraryBack')?.focus({preventScroll: true});
      window.scrollTo({top: 0, left: 0, behavior: 'instant'});
      feedback.translatedStatus(() => t('status.roomLeft'));
    },
    getActiveSettings: () => activeSettings, touchFireState,
    openTouchHelp: () => publish({touchHelpOpen: true}),
    closeTouchHelp() {publish({touchHelpOpen: false}); if (document.activeElement !== frame) frame?.focus({preventScroll: true});},
    bindPlayerRelayout(value: () => Promise<boolean>) {playerRelayout = value; return () => {if (playerRelayout === value) playerRelayout = null;};},
    async switchPlayerOrientation(orientation: TouchLayoutOrientation) {
      if (!runtime?.getSnapshot().launched || playerOrientationPending || !nativeTouch.canSwitchOrientation()) return;
      playerOrientationPending = true;
      try {await nativeTouch.switchOrientation(orientation); if (!disposed) {feedback.toast(t('touch.orientationRequested', {orientation: t(orientation === 'landscape' ? 'touch.landscape' : 'touch.portrait')})); await playerRelayout?.();}}
      catch {if (!disposed) feedback.toast(t('touch.orientationFailed'));}
      finally {playerOrientationPending = false; publish({});}
    },
    async togglePlayerFullscreen() {try {if (nativeTouch.isFullscreen()) await nativeTouch.exitPlayerFullscreen(); else {const target = document.querySelector<HTMLElement>('#player.replay-workspace #gameViewport, #player.room-workspace #gameViewport') ?? document.querySelector<HTMLElement>('#player'); if (target) await nativeTouch.enterFullscreen(target);}} catch (error) {feedback.playerStatus(t('fullscreen.switchFailed', {reason: errorText(error)}));}},
    getRuntime: () => runtime, getReplays: () => replays, getDiagnostics: () => diagnostics, getGameplayNetwork: () => gameplayNetwork,
    async chooseReplayFile(product: ProductId) {const file = await ports.pickFile(replayImportAccept); if (file) {if (!files) throw new Error('File service is not attached'); await activity(() => files!.importFile('replay', file, product));}},
    async warnDiscouragedBrowser(isCurrent: () => boolean = () => true): Promise<boolean> {
      let dismissed = false; try {dismissed = storage?.getItem('browser-warning-dismissed') === '1';} catch {}
      if (dismissed || !discouragedBrowserId(window.navigator.userAgent)) return !disposed && isCurrent();
      const choice = await decisions.askDecision({title: t('browserWarning.title'), message: t('browserWarning.message'),
        secondaryText: t('action.viewFaq'), confirmText: t('action.continueVisit'), hideCancel: true, variant: 'browser-warning'});
      if (disposed || !isCurrent()) return false;
      try {storage?.setItem('browser-warning-dismissed', '1');} catch {}
      if (choice === 'secondary') {window.location.href = new URL('faq.html', baseUrl).href; return false;}
      return true;
    },
    importFile: (kind: ImportFileKind, file: File, product: ProductId) => activity(async () => {if (!files) throw new Error('File service is not attached'); await files.importFile(kind, file, product);}),
    setLocale(locale: UiLocale) {try {storage?.setItem(UI_LOCALE_STORAGE_KEY, locale);} catch {} requiredNavigation().setLocaleAddress(locale); if (!runtime?.getSnapshot().launched) settings.restoreLocalePreferences(settingsContext(settings.getSnapshot()!.context.productId, locale)); publish({locale}); feedback.refreshLocale(); externalMidi.refreshLocale(t); document.documentElement.lang = locale; document.documentElement.dataset.uiLocale = locale; window.dispatchEvent(new CustomEvent('eagler-ui-locale-change', {detail: {locale}})); appShellOwner.refreshLocale(); diagnostics?.refresh(); gameplayNetwork?.refresh(); const canonical = document.querySelector<HTMLLinkElement>('link[rel=canonical]'); if (canonical) canonical.href = new URL(locale === 'en' ? 'en.html' : './', canonical.href).href;},
    reportError,
    async dispose() {
      unbindPixelPerfectDisplay?.();
      if (disposed) return; disposed = true; if (telemetryTimer !== null) window.clearInterval(telemetryTimer); launchAbort?.abort(); if (roomReturnTimer !== null) window.clearTimeout(roomReturnTimer); document.body.classList.remove('mp-room-returning'); for (const type of playerGestureTypes) document.removeEventListener(type, preventPlayerBrowserGesture, true); primaryTouchPointer.removeEventListener('change', syncTouchSettingsOrder); activityObserver.disconnect(); unsubscribeTouchLayoutGame(); unsubscribeTouchActivity(); unsubscribeAppShell(); metadataRetry.dispose(); localPackageMaintenance.dispose(); appShellOwner.dispose(); document.body.classList.remove('app-shell-activation-pending'); unsubscribeRoom(); room.dispose(); networkDiagnostics.dispose(); metadata.dispose(); directory.dispose(); startup.dispose(); unsubscribeMetadata(); unbindPreferences();
      document.removeEventListener('fullscreenchange', fullscreenChanged); document.removeEventListener('webkitfullscreenchange', fullscreenChanged); unsubscribeTitle(); titleNetwork.dispose(); calibration.dispose(); gameData.dispose(); transfer.dispose(); startupError.dispose(); gameUpdates?.dispose(); decisions.dispose(); feedback.dispose(); replays?.dispose(); unsubscribeRuntime?.(); diagnostics?.dispose(); gameplayNetwork?.dispose(); midi?.dispose(); externalMidi.dispose();
      await nativeTouch.dispose();
      if (runtime && frame?.isConnected === false) runtime.disposeDetachedFrame();
      listeners.clear();
    },
  };
}
export type BrowserSession = ReturnType<typeof createBrowserSession>;
