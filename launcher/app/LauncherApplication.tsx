import type {CloudSaveSync} from './services/cloud-save-sync';
import { useLocation } from 'react-router';
import { bookArtwork } from './components/launcher/book-artwork';
import {
  Activity,
  useCallback,
  useEffect,
  useLayoutEffect,
  useMemo,
  useRef,
  useState,
  useSyncExternalStore,
  lazy,
  Suspense,
  type ReactNode,
  type RefCallback,
} from 'react';
import { isProductId, isMultiplayerProductId, type ProductId } from '../src/contracts/product-catalog.mts';
import type { UiLocale } from '../src/launcher/i18n.mts';
import {localeHome} from '../src/launcher/i18n.mts';
import { TH09_RATINGS_DOCUMENT } from '../lib/frontend-static-manifest.mjs';
import { LocaleProvider, translate } from './i18n';
import { PlayerSurface } from './components/player/PlayerSurface';
import { LibrarySurface } from './components/launcher/LibrarySurface';
import { BookGameDetail } from './components/launcher/BookGameDetail';
import { MobileGameDetail } from './components/launcher/MobileGameDetail';
import { BookGameStatistics, useBookGameStatistics } from './components/launcher/BookGameStatistics';
import { GameFriends, SocialProvider } from './components/launcher/SocialSection';
import { LobbySurface, type LobbySurfaceProps } from './components/launcher/LobbySurface';
import { ProfileLoading } from './components/launcher/ProfileLoading';
import { OptionsPanel } from './components/launcher/OptionsPanel';
import { OptionsBackdrop } from './components/launcher/OptionsBackdrop';
import { LobbyOptionsHost } from './components/launcher/LobbyOptionsHost';
import { OptionsOwnership } from './components/launcher/options-ownership';
import { useLibraryOptionsPresence } from './components/launcher/use-library-options-presence';
import type { LibraryProduct } from './components/launcher/products';
import { SettingsBody } from './components/settings/SettingsBody';
import type { SettingsActions, TouchEditorNativePorts } from './components/settings/types';
import type { GameSettingsModel, SettingsContext, SettingsDisclosureState } from './models/game-settings';
import type { WatchTh09Replay } from './models/th09-replay-archive';
import type { SitePreferencesModel } from './models/site-preferences';
import type { DecisionStore } from './models/decisions';
import type { TouchLayoutModel } from './models/touch-layout';
import { SurfaceNavigationProvider, useSurfaceNavigation } from './navigation/surface-navigation';
import { useSurfaceLayerState } from './navigation/surface-layer';
import { useCompactNavigation, usePageEntrance } from './navigation/navigation-motion';
import { launcherPageContent } from './navigation/page-entrance';
import { useStoreSelector, shallowEqual } from './shared/react-store';
import { accountsEnabled } from './release-features';
import type {LocalSaveReader} from './services/local-save-storage';
import {primaryLauncherView, useLauncherSection} from './components/launcher/launcher-sections';
import type {LauncherPreferencesProps} from './components/launcher/LauncherPreferences';
import {LauncherHome} from './components/launcher/LauncherHome';
import {MessageCenter} from './components/launcher/MessageCenter';

const AccountPage = lazy(() => import('./components/launcher/AccountPage').then(module => ({default:module.AccountPage})));
const ProfilePage = lazy(() => import('./components/launcher/ProfilePreview').then(module => ({default:module.ProfilePage})));
const TouchLayoutEditor = lazy(() => import('./components/settings/TouchLayoutEditor').then(module => ({default:module.TouchLayoutEditor})));

export type LobbyPresentation = Omit<
  LobbySurfaceProps,
  'products' | 'selectedProduct' | 'openedProduct' | 'onSelect' | 'onActivate' | 'children' | 'roomUsersArtwork' | 'rankingsHref'
>;
/** One document-lived domain host. Tests may explicitly inject fixture ports;
 * production must supply the actual storage, file, runtime and room owners. */
export interface LauncherHost {
  desktopLibraryDetails?: boolean;
  readLocalSaves?:LocalSaveReader;
  cloudSync?:CloudSaveSync|null;
  watchTh09Replay?: WatchTh09Replay;
  locale: UiLocale;
  lessMotion?: boolean;
  products: readonly LibraryProduct[];
  catalogProducts?: readonly LibraryProduct[];
  productAvailable?(productId: ProductId): boolean;
  onProductSelected?(productId: ProductId, changed: boolean): void;
  onRouteSelection?(productId: ProductId, context?: 'library' | 'lobby'): void;
  settings: GameSettingsModel;
  preferences?: SitePreferencesModel;
  touchLayout: TouchLayoutModel;
  settingsContext(productId: ProductId, locale: UiLocale): SettingsContext;
  settingsActions: SettingsActions;
  decisions?: DecisionStore;
  nativeTouch: TouchEditorNativePorts;
  assetUrl(path: string): string;
  lobbyHref: string;
  masthead: ReactNode;
  librarySearchTarget?: HTMLElement | null;
  libraryTools?: ReactNode;
  installsPage?: ReactNode;
  launcherPreferences?: LauncherPreferencesProps;
  lobby: LobbyPresentation;
  renderLobby?(props: LobbySurfaceProps): ReactNode;
  /** Full launch/room owners compose their original controls at this seam. */
  optionsActions?: ReactNode;
  multiplayerControls?: ReactNode;
  multiplayerDiagnostics?: ReactNode;
  multiplayerOnline?: ReactNode;
  overlays?: ReactNode;
  previewImage?: string;
  touchFireEnabled?: boolean;
  bootTouchPreview?: boolean;
  bootTouchPreviewImage?: string;
  runtimeOpen?: boolean;
  replayWorkspace?: boolean;
  roomWorkspace?: boolean;
  roomGameplayKeyboard?: boolean;
  roomPlayerExpanded?: boolean;
  runtimeLaunched?: boolean;
  titleOverlayOpen?: boolean;
  requestRuntimeClose?(): Promise<boolean>;
  requestReplayClose?(): Promise<boolean>;
  requestRoomLeave?(): void;
  requestRoomFullscreenExit?(): Promise<void>;
  roomSurface?: ReactNode;
  roomDrawer?: ReactNode;
  runtimeFrame?: RefCallback<HTMLIFrameElement>;
  runtimeControls?: ReactNode;
  /** Incremented for host/catalog/current-install changes, not every render. */
  metadataRevision?: number;
}

/** Actual React entry shared by Framework routes and the mounted regression.
 * It deliberately has no hidden default ports, demo preferences or fake room. */
export function LauncherApplication({ host }: { host: LauncherHost }) {
  const layout = useStoreSelector(
    host.touchLayout,
    (value) => ({ dirty: value.dirty, isEditing: value.isEditing }),
    shallowEqual,
  );
  return (
    <LocaleProvider locale={host.locale}>
      <SurfaceNavigationProvider
        dirty={layout.dirty}
        isEditing={layout.isEditing}
        onDiscard={host.touchLayout.discard}
        decisions={host.decisions}
        playerOpen={host.runtimeOpen}
        requestRuntimeClose={host.requestRuntimeClose}
        requestReplayClose={host.requestReplayClose}
        requestRoomLeave={host.requestRoomLeave}
        requestRoomFullscreenExit={host.requestRoomFullscreenExit}
        titleOverlayOpen={host.titleOverlayOpen}
        productAvailable={host.productAvailable}
        onRouteSelection={host.onRouteSelection}
      >
        <LauncherSurfaces host={host} />
      </SurfaceNavigationProvider>
    </LocaleProvider>
  );
}

function LauncherSurfaces({ host }: { host: LauncherHost }) {
  const navigation = useSurfaceNavigation();
  const location = useLocation();
  const { pathname, key: locationKey } = location;
  const section = useLauncherSection();
  const showHome = section === 'home';
  const [homeVisited, setHomeVisited] = useState(showHome);
  useEffect(() => {
    if (showHome) setHomeVisited(true);
  }, [showHome]);
  const primary = primaryLauncherView(location);
  const accountPage = accountsEnabled && /\/login\.html\/?$/.test(pathname);
  const profilePage = accountsEnabled && /^(?:\/profile\.html|\/u\/[^/]+)\/?$/.test(pathname);
  const page = section ?? (accountPage ? 'account' : profilePage ? 'profile' : navigation.context);
  const compact = useCompactNavigation();
  const pageSurface = usePageEntrance(accountPage || profilePage ? pathname : page, {
    reduced: host.lessMotion === true,
    contentSelector: launcherPageContent[page],
  });
  const showLibrary = !accountPage && !profilePage && !section && navigation.context === 'library';
  const keepLibrary = primary === 'home' || primary === 'lobby' || section === 'installs';
  const [libraryVisited, setLibraryVisited] = useState(showLibrary);
  useEffect(() => {
    if (showLibrary) setLibraryVisited(true);
    else if (!keepLibrary) setLibraryVisited(false);
  }, [showLibrary, keepLibrary]);
  const settings = useSyncExternalStore(host.settings.subscribe, host.settings.getSnapshot, host.settings.getSnapshot);
  const isEditing = useStoreSelector(host.touchLayout, (value) => value.isEditing);
  const [preview, setPreview] = useState<string | undefined>(navigation.productId ?? host.products[0]?.id);
  const selectedLifetime = useRef<string | null>(
    settings ? `${navigation.context}:${settings.context.productId}` : null,
  );
  const contextRevision = useRef<number | undefined>(undefined);
  const [playerElement, setPlayerElement] = useState<HTMLElement | null>(null);
  const touchReturn = useRef<HTMLElement | null>(null);
  const [gamePageDisclosures, setGamePageDisclosures] = useState<Partial<SettingsDisclosureState>>({});
  const browseIssuedFor = useRef<string | null>(null);
  useLayoutEffect(() => {
    if (browseIssuedFor.current !== locationKey) browseIssuedFor.current = null;
  }, [locationKey]);
  const roomOpen =
    navigation.roomCode !== null && navigation.productId !== null && isMultiplayerProductId(navigation.productId);
  const roomWorkspace = roomOpen && host.roomWorkspace === true && navigation.surface !== 'touch';
  const catalog = host.catalogProducts ?? host.products;
  const optionsOpen = !roomOpen && navigation.hasSelection;
  const retainedProduct = useRef<ProductId | undefined>(host.products[0]?.id);
  if (navigation.productId && catalog.some((product) => product.id === navigation.productId))
    retainedProduct.current = navigation.productId;
  // Main keeps the selected panel in the DOM during its closing transition.
  const selectedProduct = catalog.find((product) => product.id === retainedProduct.current);
  const inlineLibrary = host.desktopLibraryDetails && !compact && !section && !accountPage && !profilePage && navigation.context === 'library';
  const inlineProduct = inlineLibrary && optionsOpen && selectedProduct?.id === navigation.productId &&
    !selectedProduct.multiplayer ? selectedProduct : undefined;
  const mobileDetail = !!(host.desktopLibraryDetails && compact && !section && navigation.context === 'library' && !accountPage && !profilePage);
  const presence = useLibraryOptionsPresence({
    open: optionsOpen && !inlineProduct,
    context: navigation.context,
    product: selectedProduct,
    lessMotion: host.lessMotion === true,
    mobileDetail,
    concealed:
      !!inlineProduct || ((host.runtimeOpen === true || host.runtimeLaunched === true || host.replayWorkspace === true) &&
      navigation.surface !== 'touch'),
    focusActive: navigation.surface === 'options',
  });
  const bookSheet = mobileDetail && !presence.product?.multiplayer;
  const mobileProduct = bookSheet && (optionsOpen || presence.open) ? presence.product : undefined;
  const detailProduct = inlineProduct ?? mobileProduct;
  const [detailSettingsOpen, setDetailSettingsOpen] = useSurfaceLayerState<boolean>('game-detail-settings', false);
  const gameStatistics = useBookGameStatistics(detailProduct?.gameId);
  const save = gameStatistics?.status === 'ready' ? gameStatistics.save : undefined;
  const statistics = detailProduct && <BookGameStatistics game={detailProduct.gameId} view={gameStatistics}/>;
  const friends = detailProduct && gameStatistics?.owner && (!compact || gameStatistics.status !== 'loading') ? (
    <SocialProvider key={gameStatistics.owner} username={gameStatistics.owner} includeLists list="friends">
      <GameFriends username={gameStatistics.owner} game={detailProduct.gameId}/>
    </SocialProvider>
  ) : undefined;
  useLayoutEffect(() => {
    if (!optionsOpen && !roomOpen) {
      if (navigation.context === 'lobby') selectedLifetime.current = null;
      return;
    }
    if (!navigation.productId) return;
    const context = host.settingsContext(navigation.productId, host.locale);
    const lifetime = `${navigation.context}:${navigation.productId}`;
    if (host.onRouteSelection && settings?.context.productId !== navigation.productId) return;
    if (selectedLifetime.current !== lifetime) {
      selectedLifetime.current = lifetime;
      if (!host.onRouteSelection) host.settings.hydrate(context, { resetDisclosure: navigation.context === 'lobby' });
    } else if (settings?.context.uiLocale !== host.locale || contextRevision.current !== host.metadataRevision)
      host.settings.refreshContext(context);
    contextRevision.current = host.metadataRevision;
  }, [
    host,
    navigation.productId,
    navigation.context,
    optionsOpen,
    roomOpen,
    settings?.context.uiLocale,
    settings?.context.productId,
  ]);
  useEffect(() => {
    // Draft lifetime ends only after Router commits leaving the editor. A
    // cancelled blocker leaves both the surface and draft intact.
    if (navigation.surface !== 'touch' && isEditing) {
      host.touchLayout.discard();
      host.settingsActions.feedback(translate(host.locale, 'touch.layoutEditorClosed'));
    }
  }, [navigation.surface, isEditing, host.touchLayout, host.locale, host.settingsActions.feedback]);
  useLayoutEffect(() => {
    document.body.classList.toggle('lobby-page', navigation.context === 'lobby');
    document.body.classList.toggle('mp-room-active', roomOpen);
    return () => {
      document.body.classList.remove('lobby-page', 'mp-room-active');
    };
  }, [navigation.context, roomOpen]);
  const activate = (id: string) => {
    if (!isProductId(id)) return;
    if (navigation.context === 'library' && isMultiplayerProductId(id)) navigation.openDirectory(id);
    else {
      const previous = host.settings.getSnapshot()?.context.productId;
      navigation.openOptions(id);
      if (navigation.context === 'library' && !isMultiplayerProductId(id)) {
        if (host.onProductSelected) host.onProductSelected(id, previous !== id);
        else
          host.settingsActions.feedback(
            '',
            translate(host.locale, previous === id ? 'status.selectedProduct' : 'status.switchedProduct', {
              product: host.products.find((product) => product.id === id)?.title ?? id,
            }),
          );
      }
    }
  };
  const touchEntry = useRef({open: navigation.openTouchLayout, playing: host.runtimeLaunched, locale: host.locale,
    feedback: host.settingsActions.feedback, multiplayer: settings?.multiplayer});
  useLayoutEffect(() => {
    touchEntry.current = {open: navigation.openTouchLayout, playing: host.runtimeLaunched, locale: host.locale,
      feedback: host.settingsActions.feedback, multiplayer: settings?.multiplayer};
  });
  const openTouchLayout = useCallback(() => {
    const {open, playing, locale, feedback, multiplayer} = touchEntry.current;
    if (playing) {
      const reason = translate(locale, 'touch.editWhileRunning');
      feedback(reason, translate(locale, 'status.errorReason', { reason }));
      return;
    }
    touchReturn.current = document.getElementById(multiplayer ? 'mpTouchLayoutEdit' : 'touchLayoutEdit');
    open();
  }, []);
  const browseLibrary = () => {
    // Several input events can arrive before the same history return commits.
    if (browseIssuedFor.current === locationKey) return;
    browseIssuedFor.current = locationKey;
    navigation.closeSurface();
  };
  const gamePage = navigation.context === 'library' &&
    !(navigation.productId && isMultiplayerProductId(navigation.productId));
  const multiplayerSettings = gamePage ? undefined : host.multiplayerControls;
  const settingsBody = useMemo(() => (
    <SettingsBody
      model={host.settings}
      gamePage={gamePage ? {
        disclosures: gamePageDisclosures,
        setDisclosure: (name, open) => setGamePageDisclosures(previous => ({...previous, [name]: open})),
      } : undefined}
      actions={host.settingsActions}
      onOpenTouchLayout={openTouchLayout}
      multiplayerControls={multiplayerSettings}
      preferences={host.preferences}
    />
  ), [gamePage, gamePageDisclosures, host.settings, host.settingsActions, host.preferences, multiplayerSettings, openTouchLayout]);
  const settingsPanel = !inlineProduct && presence.product && (
    <OptionsPanel
      product={presence.product}
      coverArtwork={roomOpen ? undefined : bookArtwork(presence.product)}
      open={presence.open || (roomOpen && navigation.context === 'lobby')}
      closing={presence.closing && !roomOpen}
      onBack={roomOpen ? navigation.closeRoomSettings : navigation.closeSurface}
      roomDrawer={roomOpen}
      multiplayerDiagnostics={host.multiplayerDiagnostics}
      multiplayerOnline={host.multiplayerOnline}
      lobbyHref={host.lobbyHref}
      embeddedInLobby={navigation.context === 'lobby'}
      bookSheet={bookSheet}
      detail={mobileProduct && <MobileGameDetail key={mobileProduct.id} product={mobileProduct} cover={bookArtwork(mobileProduct)}
        actions={settings?.context.productId === mobileProduct.id ? host.optionsActions : undefined}
        statistics={statistics} friends={friends} playtime={save?.statistics.playtimeSeconds} saved={!!save}
        settings={detailSettingsOpen} setSettings={setDetailSettingsOpen} onBack={navigation.closeSurface}>
        {settings?.context.productId === mobileProduct.id && settingsBody}
      </MobileGameDetail>}
      actions={host.optionsActions}
    >
      {settingsBody}
    </OptionsPanel>
  );
  const editor = playerElement && navigation.surface === 'touch' && settings && (
    <Suspense><TouchLayoutEditor
      playerElement={playerElement}
      model={host.touchLayout}
      settings={host.settings}
      actions={host.settingsActions}
      native={host.nativeTouch}
      onCloseIntent={navigation.closeSurface}
      onClosed={() => {
        const trigger = touchReturn.current;
        touchReturn.current = null;
        if (trigger?.isConnected && !trigger.closest('[hidden], [inert]')) trigger.focus({preventScroll:true});
        else presence.restoreFocus(true);
      }}
      fireEnabled={host.touchFireEnabled}
      previewImage={
        host.previewImage ?? (selectedProduct?.artwork ? `url(${JSON.stringify(selectedProduct.artwork)})` : 'none')
      }
    /></Suspense>
  );
  const lobbyProps: LobbySurfaceProps = {
    ...host.lobby,
    masthead: null,
    rankingsHref: host.assetUrl(TH09_RATINGS_DOCUMENT.file),
    products: host.products.filter((product) => product.multiplayer),
    selectedProduct: navigation.filterProductId ?? undefined,
    viewingProduct: navigation.filterProductId ?? undefined,
    openedProduct: presence.openedProduct,
    onSelect: (id) => {
      navigation.selectLobbyProduct(isProductId(id) ? id : '');
    },
    onActivate: activate,
    roomUsersArtwork: host.assetUrl('assets/room-users.svg'),
    children: (
      <LobbyOptionsHost
        open={presence.open}
        foregroundActive={navigation.surface === 'touch' || host.runtimeOpen === true || host.replayWorkspace === true}
        onCloseRequest={navigation.closeSurface}
      >
        {host.replayWorkspace && !roomOpen ? null : settingsPanel}
      </LobbyOptionsHost>
    ),
  };
  return (
    <OptionsOwnership value={presence.targets}>
      {host.masthead}
      <div
        ref={pageSurface}
        className="launcher-page-surface"
        data-page={page}
      >
        {accountPage ? (
          <Suspense fallback={<main className="account-page" aria-busy="true"/>}><AccountPage /></Suspense>
        ) : profilePage ? (
          <Suspense fallback={<ProfileLoading/>}><ProfilePage readLocalSaves={host.readLocalSaves} cloudSync={host.cloudSync} watchTh09Replay={host.watchTh09Replay}/></Suspense>
        ) : section === 'announcements' || section === 'messages' ? (
          <MessageCenter section={section} homeHref={host.assetUrl(localeHome(host.locale))} assetUrl={host.assetUrl}/>
        ) : section === 'installs' ? (
          host.installsPage
        ) : !showHome && navigation.context === 'lobby' ? (
          host.renderLobby ? (
            host.renderLobby(lobbyProps)
          ) : (
            <LobbySurface {...lobbyProps} />
          )
        ) : null}
        {(showHome || homeVisited) && <Activity mode={showHome ? 'visible' : 'hidden'}>
          <LauncherHome active={showHome} settings={host.launcherPreferences} assetUrl={host.assetUrl}/>
        </Activity>}
        {(showLibrary || keepLibrary && libraryVisited) && <Activity mode={showLibrary ? 'visible' : 'hidden'}>
          <LibrarySurface
            products={host.products}
            selectedProduct={preview}
            openedProduct={inlineProduct?.id ?? presence.openedProduct}
            onSelect={setPreview}
            onActivate={activate}
            masthead={null}
            mobileSearchTarget={host.librarySearchTarget}
            tools={host.libraryTools}
            active={showLibrary}
            optionsOpen={!inlineProduct && presence.open}
            compact={compact}
            onBrowse={browseLibrary}
            detail={inlineProduct && (
              <BookGameDetail
                product={inlineProduct}
                onBack={navigation.closeSurface}
                active={navigation.surface === 'options' && !host.runtimeOpen && !host.runtimeLaunched && !host.replayWorkspace}
                actions={settings?.context.productId === inlineProduct.id ? host.optionsActions : undefined}
                statistics={statistics} friends={friends} playtime={save?.statistics.playtimeSeconds} saved={!!save}
                settings={detailSettingsOpen} setSettings={setDetailSettingsOpen}
              >
                {settings?.context.productId === inlineProduct.id && settingsBody}
              </BookGameDetail>
            )}
            roomOpen={roomOpen}
            roomDrawer={host.roomDrawer}
          >
            {host.roomSurface}
          </LibrarySurface>
        </Activity>}
      </div>
      {!accountPage && !profilePage && !section && navigation.context === 'library' && (
        <>
          {!inlineProduct && <OptionsBackdrop bookSheet={bookSheet} onBack={navigation.closeSurface}/>}
          {host.replayWorkspace && !roomOpen && !mobileDetail ? null : settingsPanel}
        </>
      )}
      <PlayerSurface
        roomWorkspace={roomWorkspace}
        roomGameplayKeyboard={host.roomGameplayKeyboard}
        replayWorkspace={host.replayWorkspace}
        preview={host.bootTouchPreview}
        previewImage={host.bootTouchPreviewImage}
        onElement={setPlayerElement}
        onFrame={host.runtimeFrame}
        open={
          roomWorkspace ||
          (roomOpen && host.roomPlayerExpanded === true) ||
          navigation.surface === 'touch' ||
          host.runtimeOpen === true ||
          host.replayWorkspace === true
        }
        editing={navigation.surface === 'touch'}
      >
        {host.runtimeControls}
      </PlayerSurface>
      {editor}
      {host.overlays}
    </OptionsOwnership>
  );
}
