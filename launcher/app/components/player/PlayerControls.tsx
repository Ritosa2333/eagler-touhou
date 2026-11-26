import { Button as UiButton } from '../../ui/controls';
import { useContext, useLayoutEffect, useRef, useState, useSyncExternalStore, type ReactNode } from 'react';
import { createPortal } from 'react-dom';
import { touchMovementUsesJoystick } from '../../../src/launcher/game-preferences.mts';
import { readTh075RoomScore } from '../../../src/launcher/th09-room-stats.mts';
import { multiplayerConfigForProduct, PRODUCT_GAMES } from '../../../src/contracts/product-catalog.mts';
import type { BrowserSession } from '../../session/browser-session';
import {
  bindPlayerInput,
  bindSpectatorChatKeyboard,
  bindPlayerLayoutUpdates,
  applySavedPlayerTouchLayout,
} from '../../services/player-input';
import type { PlayerTouch } from '../../services/player-touch';
import type { SettingsSnapshot } from '../../models/game-settings';
import { playerUsesGameInput } from '../../models/player-mode';
import { TouchHelp } from './TouchHelp';
import { TitleNetworkOverlay } from './TitleNetworkOverlay';
import { NetplayConnectionWindow } from '../NetplayConnectionWindow';
import { NetplayCalibrationReport } from '../NetplayCalibrationReport';
import { RuntimeDiagnostics, type NetplayDiagnosticLines } from './RuntimeDiagnostics';
import { TransferPanel } from './TransferPanel';
import { TouchControlPreview } from '../settings/TouchControlPreview';
import { GameSettingsPopover } from '../settings/GameAudioControls';
import { useSurfaceNavigation } from '../../navigation/surface-navigation';
import { useLocale } from '../../i18n';
import { FullscreenIcons, PlayerExitIcon, TouchHelpIcon, OrientationIcon } from './utility-icons';
import { SpectatorMatchScore } from './SpectatorMatchScore';
import { Th09MatchStats } from './Th09MatchStats';
import { SpectatorPlayback } from './SpectatorPlayback';
import {NetplayPlayerStatus} from './NetplayPlayerStatus';
import {PlayerStage} from './PlayerSurface';
import './browser-practice.css';
const noopSubscribe = () => () => {};
const initialTouch = () => undefined;
const noNetwork = () => null;
/** The editor and actual game use one original control markup tree. Only this
 * mounted gameplay binding can deliver native input. */
export function PlayerControls({
  session,
  replayWorkspace = false,
  roomWorkspace = false,
}: {
  session: BrowserSession;
  replayWorkspace?: boolean;
  roomWorkspace?: boolean;
}) {
  const stage = useContext(PlayerStage);
  const transferHost = roomWorkspace ? stage?.querySelector<HTMLElement>('.game-viewport') : null;
  const transferPanel = <TransferPanel model={session.transfer} />;
  const state = useSyncExternalStore(session.subscribe, session.getSnapshot, session.getSnapshot),
    navigation = useSurfaceNavigation();
  const feedback = useSyncExternalStore(
    session.feedback.subscribe,
    session.feedback.getSnapshot,
    session.feedback.getSnapshot,
  );
  const selected = useSyncExternalStore(
    session.settings.subscribe,
    session.settings.getSnapshot,
    session.settings.getSnapshot,
  );
  const settings = session.getActiveSettings() ?? selected,
    diagnostics = session.getDiagnostics();
  const gameplay = session.getGameplayNetwork();
  const network = useSyncExternalStore(
    gameplay?.subscribe ?? noopSubscribe,
    gameplay?.getSnapshot ?? noNetwork,
    noNetwork,
  );
  useLayoutEffect(() => {
    const player = document.getElementById('player');
    player?.classList.toggle('netplay-player-status-visible', network?.playerStatusVisible === true);
    player?.style.setProperty('--netplay-status-lines', String(network?.playerStatus.length || 1));
    return () => {
      player?.classList.remove('netplay-player-status-visible');
      player?.style.removeProperty('--netplay-status-lines');
    };
  }, [network?.playerStatusVisible, network?.playerStatus.length]);
  const room = useSyncExternalStore(
    session.room.service.subscribe,
    session.room.service.getSnapshot,
    session.room.service.getSnapshot,
  );
  const roomConfig = room.product ? multiplayerConfigForProduct(room.product) : null;
  // TH07 uses Bomb to manually break an active Border in challenge mode.
  const hideBomb = settings?.multiplayer === true && settings.gameId !== 'th07' &&
    room.product === settings.context.productId && room.room?.challengeMode === true &&
    roomConfig?.gameplay === 'cooperative' && roomConfig.challengeMode !== false;
  const spectatorTimeline = state.runtime?.launched && room.spectatorRequested && room.room?.spectatorTimeline === true;
  const mode = session.getPlayerMode(),
    spectating = mode === 'spectator';
  const netplay = network?.diagnostics as NetplayDiagnosticLines | undefined;
  const th09Information = state.runtime?.launched === true && (room.product === 'th075mp' || room.product === 'th09mp' || room.product === 'th19mp') && !state.replayViewer;
  const cooperativeSpectator =
    state.runtime?.launched === true &&
    state.runtime.spectator === true &&
    state.runtime.game !== 'th09' &&
    !state.replayViewer;
  const informationColumn = th09Information || cooperativeSpectator;
  useLayoutEffect(() => {
    const player = document.getElementById('player');
    if (!player) return;
    const enabled = state.playerOpen && state.runtime?.game === 'th13' && !state.replayViewer &&
      !roomWorkspace && !replayWorkspace && navigation.surface !== 'touch' &&
      !session.mobile && navigator.maxTouchPoints === 0 && !settings?.options.touchEnabled;
    player.classList.toggle('th13-desktop-tool-rail', enabled);
    if (!enabled) return () => player.classList.remove('th13-desktop-tool-rail');
    // The zoom owner writes the saved touch profile's translation even when
    // touch zoom is unavailable. Reserve its displacement without overriding
    // that owner's transform or clipping either edge of the game viewport.
    const updateOffset = () => {
      const width = window.visualViewport?.width || document.documentElement.clientWidth || player.clientWidth;
      const height = window.visualViewport?.height || document.documentElement.clientHeight || player.clientHeight;
      const orientation = width >= height ? 'landscape' : 'portrait';
      const x = session.touchLayout.getSnapshot().saved?.profiles[orientation]?.viewport.x ?? 0;
      player.style.setProperty('--th13-rail-offset', `${x * player.clientWidth}px`);
    };
    const unsubscribe = session.touchLayout.subscribe(updateOffset);
    window.addEventListener('resize', updateOffset);
    window.visualViewport?.addEventListener('resize', updateOffset);
    updateOffset();
    return () => {
      unsubscribe();
      window.removeEventListener('resize', updateOffset);
      window.visualViewport?.removeEventListener('resize', updateOffset);
      player.classList.remove('th13-desktop-tool-rail');
      player.style.removeProperty('--th13-rail-offset');
    };
  }, [state.playerOpen, state.runtime?.game, state.replayViewer, roomWorkspace, replayWorkspace,
      navigation.surface, session, settings?.options.touchEnabled]);
  const wins = [room.room?.seats?.[0]?.stats?.wins ?? 0, room.room?.seats?.[1]?.stats?.wins ?? 0] as const;
  const th075Score = room.product === 'th075mp' ? readTh075RoomScore(room.room?.seats) : undefined;
  const names = [room.room?.seats?.[0]?.name ?? '', room.room?.seats?.[1]?.name ?? ''] as const;
  const minutes = Math.max(...(room.room?.seats ?? []).map((seat) => seat?.stats?.minutes ?? 0), 0);
  return (
    <>
      <TitleNetworkOverlay model={session.titleNetwork} />
      {spectating && !roomWorkspace && !replayWorkspace && state.playerOpen && navigation.surface !== 'touch' && (
        <SpectatorChatKeyboard session={session} />
      )}
      {network && (
        <NetplayConnectionWindow
          network={network}
          calibration={session.calibration}
          onReturn={() => {
            if (room.room && network.connection?.ended) {
              void session
                .requestRuntimeClose()
                .then((closed) => (closed ? navigation.closeRoomFullscreen() : undefined))
                .catch(session.reportError);
            } else if (room.room)
              void session.exitRoomPlayerFullscreen().then(navigation.closeRoomFullscreen).catch(session.reportError);
            else navigation.closeSurface();
          }}
        />
      )}
      <PlayerInformationColumn
        enabled={informationColumn}
        className={th09Information ? 'th09-player-information' : undefined}
      >
        {cooperativeSpectator && !roomWorkspace && <SpectatorPlayback session={session} score={th075Score?.wins}
          minutes={th075Score?.minutes} names={names}/>}
        {th09Information &&
          !roomWorkspace &&
          (spectatorTimeline && room.product !== 'th075mp' ? (
            <SpectatorMatchScore
              wins={wins}
              minutes={minutes}
              names={names}
            />
          ) : !room.spectatorRequested && room.seat != null && (room.product !== 'th075mp' || th075Score) ? (
            <Th09MatchStats
              wins={th075Score?.wins ?? wins}
              minutes={th075Score?.minutes ?? minutes}
              spectators={Math.max(room.room?.spectatorCount ?? 0, room.room?.spectators?.length ?? 0)}
            />
          ) : null)}
        {diagnostics && !replayWorkspace && (
          <RuntimeDiagnostics model={diagnostics} netplay={netplay} playbackControls={!!spectatorTimeline}>
            <NetplayCalibrationReport
              model={session.calibration}
              currentNetwork={() => gameplay?.getSnapshot().diagnostics.runtimeNetplayQualityDiag ?? ''}
            />
          </RuntimeDiagnostics>
        )}
        <NetplayPlayerStatus visible={network?.playerStatusVisible === true} rows={network?.playerStatus ?? []} />
      </PlayerInformationColumn>
      {transferHost ? createPortal(transferPanel, transferHost) : transferPanel}
      <span id="playerStatus" className="visually-hidden" role="status">
        {feedback.playerStatus}
      </span>
      {settings && !replayWorkspace && (
        <PlayerUtilities
          session={session}
          settings={settings}
          roomWorkspace={roomWorkspace}
          canExitPlayback={state.replayViewer || room.spectatorRequested}
        />
      )}
      {settings &&
        !replayWorkspace &&
        !roomWorkspace &&
        !spectating &&
        state.bootTouchPreview &&
        !session.getActiveSettings() &&
        navigation.surface !== 'touch' && <BootTouchControls session={session} settings={settings} />}
      {session.getActiveSettings() &&
        playerUsesGameInput(mode) &&
        !replayWorkspace &&
        navigation.surface !== 'touch' &&
        state.playerOpen && <GameplayControls session={session} roomWorkspace={roomWorkspace} hideBomb={hideBomb} />}
    </>
  );
}
function PlayerInformationColumn({
  enabled,
  children,
  className,
}: {
  enabled: boolean;
  children: ReactNode;
  className?: string;
}) {
  return enabled ? (
    <div className={`player-information-column${className ? ` ${className}` : ''}`}>{children}</div>
  ) : (
    <>{children}</>
  );
}
/** Boot preview is original static presentation; it creates no native input owner. */
function BootTouchControls({ session, settings }: { session: BrowserSession; settings: SettingsSnapshot }) {
  const enabled = settings.options.touchEnabled,
    wheel = touchMovementUsesJoystick(settings.options.touchMovementMode);
  useLayoutEffect(() => {
    const player = document.getElementById('player')!;
    player.classList.toggle('touch-enabled', enabled);
    player.classList.toggle('touch-joystick-enabled', enabled && wheel);
    player.style.setProperty('--touch-control-opacity', String(settings.options.touchControlOpacity / 100));
    return () => {
      player.classList.remove('touch-enabled', 'touch-joystick-enabled');
      player.style.removeProperty('--touch-control-opacity');
    };
  }, [enabled, wheel, settings.options.touchControlOpacity]);
  useLayoutEffect(() => {
    const player = document.getElementById('player')!,
      safe = document.getElementById('touchLayoutSafeZone')!;
    const apply = () => {
      const width = window.visualViewport?.width || document.documentElement.clientWidth || player.clientWidth;
      const height = window.visualViewport?.height || document.documentElement.clientHeight || player.clientHeight;
      applySavedPlayerTouchLayout(
        player,
        safe,
        session.touchLayout.getSnapshot().saved?.profiles[width >= height ? 'landscape' : 'portrait'] ?? null,
      );
    };
    const updates = bindPlayerLayoutUpdates({ window, safeZone: safe, applyLayout: apply });
    const unsubscribe = session.touchLayout.subscribe(apply);
    apply();
    return () => {
      updates.dispose();
      unsubscribe();
      player.classList.remove('touch-layout-custom');
    };
  }, [session]);
  return (
    <>
      <div className="touch-layout-safe-zone" id="touchLayoutSafeZone" aria-hidden="true" />
      <div className="touch-direct-surface" id="touchDirectSurface" aria-hidden="true" hidden />
      <TouchControlPreview settings={settings} fireEnabled={session.touchFireState.getEnabled()} />
    </>
  );
}
function SpectatorChatKeyboard({ session }: { session: BrowserSession }) {
  useLayoutEffect(() => {
    const input = bindSpectatorChatKeyboard({ session, document, window });
    return () => input.dispose();
  }, [session]);
  return null;
}
function GameplayControls({ session, roomWorkspace, hideBomb }: { session: BrowserSession; roomWorkspace: boolean; hideBomb: boolean }) {
  const { t } = useLocale(),
    state = useSyncExternalStore(session.subscribe, session.getSnapshot, session.getSnapshot);
  const settings = session.getActiveSettings()!,
    latestT = useRef(t);
  latestT.current = t;
  const [input, setInput] = useState<PlayerTouch | null>(null);
  const touch = useSyncExternalStore(
    input?.subscribe ?? noopSubscribe,
    input?.getSnapshot ?? initialTouch,
    initialTouch,
  );
  const spectator = session.getPlayerMode() === 'spectator',
    enabled = settings.options.touchEnabled && playerUsesGameInput(session.getPlayerMode());
  const wheel = touchMovementUsesJoystick(settings.options.touchMovementMode);
  const product = PRODUCT_GAMES[settings.gameId];
  const pc98Direct = ['th01', 'th02', 'th03', 'th04'].includes(settings.gameId);
  const thibpCoop = settings.gameId === 'thibp' && settings.multiplayer;
  const embeddedTouch = roomWorkspace && (pc98Direct || thibpCoop) &&
    enabled && state.runtime?.launched === true;
  useLayoutEffect(() => {
    const player = document.getElementById('player'), frame = document.getElementById('gameFrame');
    if (!embeddedTouch || !player || !frame) return;
    player.classList.add('room-touch-active');
    const update = () => {
      const rect = frame.getBoundingClientRect();
      const viewport = window.visualViewport;
      const left = Math.max(rect.left, viewport?.offsetLeft ?? 0);
      const top = Math.max(rect.top, viewport?.offsetTop ?? 0);
      const right = Math.min(rect.right, (viewport?.offsetLeft ?? 0) + (viewport?.width ?? window.innerWidth));
      const bottom = Math.min(rect.bottom, (viewport?.offsetTop ?? 0) + (viewport?.height ?? window.innerHeight));
      const bounds = {left, top, width: Math.max(0, right - left), height: Math.max(0, bottom - top)};
      for (const [name, value] of Object.entries(bounds)) player.style.setProperty(`--room-touch-${name}`, `${value}px`);
      window.dispatchEvent(new Event('room-touch-surface-change'));
    };
    const observer = new ResizeObserver(update); observer.observe(frame);
    const classes = new MutationObserver(() => {
      if (!player.classList.contains('touch-enabled')) player.classList.add('touch-enabled');
      if (player.classList.contains('touch-joystick-enabled') !== wheel) player.classList.toggle('touch-joystick-enabled', wheel);
    });
    classes.observe(player, {attributes:true,attributeFilter:['class']});
    window.addEventListener('resize', update); window.addEventListener('scroll', update, true);
    window.visualViewport?.addEventListener('resize', update);
    window.visualViewport?.addEventListener('scroll', update);
    update();
    return () => {observer.disconnect(); classes.disconnect(); window.removeEventListener('resize', update); window.removeEventListener('scroll', update, true);
      window.visualViewport?.removeEventListener('resize', update);
      window.visualViewport?.removeEventListener('scroll', update);
      player.classList.remove('room-touch-active'); for (const name of ['left','top','width','height']) player.style.removeProperty(`--room-touch-${name}`);};
  }, [embeddedTouch, wheel]);
  useLayoutEffect(() => {if (roomWorkspace) input?.cancelPointers(); else input?.syncControls();}, [input, roomWorkspace]);
  const browserPractice = !settings.multiplayer && 'practiceUI' in product && product.practiceUI === 'browser';
  const hostDirect = navigator.maxTouchPoints > 0 || window.matchMedia('(pointer: coarse)').matches ||
    /\bAndroid\b|iPhone|iPad|iPod/i.test(navigator.userAgent);
  useLayoutEffect(() => {
    if (!browserPractice) return;
    const runtime = session.getRuntime(), player = document.getElementById('player');
    if (!runtime || !player) return;
    let nativeDocument: Document | null = null, observer: MutationObserver | null = null;
    const update = () => {
      const open = runtime.getSnapshot().launched && nativeDocument?.documentElement.dataset.browserPracticeOpen === 'true';
      if (open && !player.classList.contains('browser-practice-open')) input?.cancelPointers();
      player.classList.toggle('browser-practice-open',open);
    };
    const sync = () => {
      const next = runtime.getMidiEventContext()?.document as Document | null;
      if (next !== nativeDocument) {
        observer?.disconnect(); nativeDocument = next;
        if (next?.documentElement) {
          observer = new MutationObserver(update);
          observer.observe(next.documentElement,{attributes:true,attributeFilter:['data-browser-practice-open']});
        }
      }
      update();
    };
    const unsubscribe = runtime.subscribe(sync); sync();
    return () => {unsubscribe();observer?.disconnect();player.classList.remove('browser-practice-open');};
  },[session,browserPractice,input]);
  useLayoutEffect(() => {
    const binding = bindPlayerInput({
      session,
      document,
      window,
      translate: (key, params) => latestT.current(key, params),
    });
    setInput(binding.touch);
    const unbindRelayout = session.bindPlayerRelayout(binding.relayoutAfterOrientation);
    return () => {
      unbindRelayout();
      binding.dispose();
    };
  }, [session]);
  useLayoutEffect(() => {
    const player = document.getElementById('player')!;
    player.classList.toggle('touch-enabled', enabled);
    player.classList.toggle('touch-joystick-enabled', enabled && wheel);
    player.style.setProperty('--touch-control-opacity', String(settings.options.touchControlOpacity / 100));
    return () => {
      player.classList.remove('touch-enabled', 'touch-joystick-enabled', 'touch-layout-custom');
      player.style.removeProperty('--touch-control-opacity');
    };
  }, [settings, enabled, wheel]);
  return (
    <>
      <div className="touch-layout-safe-zone" id="touchLayoutSafeZone" aria-hidden="true" />
      <div
        className="touch-direct-surface"
        id="touchDirectSurface"
        aria-hidden="true"
        hidden={!(state.runtime?.launched && enabled && (hostDirect || pc98Direct) && (!wheel || thibpCoop))}
      />
      <TouchControlPreview settings={settings} live={touch} spectator={spectator} hideBomb={hideBomb} />
    </>
  );
}
/** Main keeps these controls/help content in its hidden Player document before
 * launch; original behavior tests query their mode/copy from the settings UI. */
function PlayerUtilities({
  session,
  settings,
  canExitPlayback,
  roomWorkspace,
}: {
  session: BrowserSession;
  settings: SettingsSnapshot;
  canExitPlayback: boolean;
  roomWorkspace: boolean;
}) {
  const { t } = useLocale(),
    navigation = useSurfaceNavigation(),
    state = useSyncExternalStore(session.subscribe, session.getSnapshot, session.getSnapshot);
  const [, updateOrientation] = useState(0);
  const spectating = session.getPlayerMode() === 'spectator';
  const inRoom = navigation.roomCode !== null;
  const fullscreen = session.isPlayerFullscreen();
  const canReturnToRoom = state.runtime?.runtimeVariant === 'multiplayer' && state.runtime.roomReturnSupported;
  const portrait =
    (window.visualViewport?.height || window.innerHeight) > (window.visualViewport?.width || window.innerWidth);
  useLayoutEffect(() => {
    const update = () => updateOrientation((value) => value + 1);
    window.addEventListener('resize', update);
    window.visualViewport?.addEventListener('resize', update);
    window.screen.orientation?.addEventListener('change', update);
    return () => {
      window.removeEventListener('resize', update);
      window.visualViewport?.removeEventListener('resize', update);
      window.screen.orientation?.removeEventListener('change', update);
    };
  }, []);
  return (
    <>
      <TouchHelp
        nativeControls={'touchActions' in PRODUCT_GAMES[settings.gameId]}
        th19Charge={settings.gameId === 'th19'}
        th01={settings.gameId === 'th01'}
        open={state.touchHelpOpen && !spectating && !roomWorkspace}
        onCloseRequest={session.closeTouchHelp}
        touchEnabled={settings.options.touchEnabled}
        focusMode={settings.options.touchFocusMode}
        showIosFullscreenHelp={
          /iPhone|iPad|iPod/i.test(navigator.userAgent) ||
          (navigator.maxTouchPoints > 1 && /Macintosh/i.test(navigator.userAgent)) ||
          new URLSearchParams(location.search).get('iosHelpPreview') === '1'
        }
      />
      <nav className="touch-utility-row" aria-label={t('touch.tools')}>
        {canExitPlayback && state.runtime?.launched && (
          <UiButton
            variant="unstyled"
            id="playbackExit"
            type="button"
            aria-label={t(state.replayViewer ? 'player.exitPlayback' : 'player.exitSpectating')}
            title={t(state.replayViewer ? 'player.exitPlayback' : 'player.exitSpectating')}
            onClick={navigation.closeSurface}
          >
            <PlayerExitIcon />
          </UiButton>
        )}
        {!roomWorkspace && (
          <UiButton
            variant="unstyled"
            className="fullscreen-toggle"
            id="fullscreenToggle"
            type="button"
            data-fullscreen={fullscreen}
            aria-label={t(fullscreen ? 'player.exitFullscreen' : 'player.enterFullscreen')}
            title={t(fullscreen ? 'player.exitFullscreenTitle' : 'player.enterFullscreenTitle')}
            onClick={() => {
              void session.togglePlayerFullscreen();
            }}
          >
            <FullscreenIcons />
          </UiButton>
        )}
        {!roomWorkspace && !spectating && (
          <UiButton
            variant="unstyled"
            className="touch-help-open"
            id="touchHelpOpen"
            type="button"
            aria-label={t('help.open')}
            title={t('help.title')}
            onClick={session.openTouchHelp}
          >
            <TouchHelpIcon />
          </UiButton>
        )}
        <UiButton
          variant="unstyled"
          className="orientation-toggle"
          id="orientationToggle"
          type="button"
          aria-label={t(portrait ? 'player.switchLandscape' : 'player.switchPortrait')}
          title={t('player.switchOrientationTitle')}
          hidden={roomWorkspace || !state.runtime?.launched || !session.nativeTouch.canSwitchOrientation()}
          onClick={(event) => {
            if (!state.runtime?.launched) return;
            event.preventDefault();
            event.stopPropagation();
            void session.switchPlayerOrientation(portrait ? 'landscape' : 'portrait');
          }}
        >
          <strong id="orientationTarget">{t(portrait ? 'touch.landscape' : 'touch.portrait')}</strong>
          <small>{t('player.rotate')}</small>
          <OrientationIcon />
        </UiButton>
        <UiButton
          variant="unstyled"
          className="game-zoom-toggle"
          id="gameZoomToggle"
          type="button"
          aria-label={t('player.restoreViewportAria')}
          title={t('player.restoreViewportTitle')}
          hidden
        >
          <strong>{t('action.reset')}</strong>
          <small id="gameZoomScale">100%</small>
        </UiButton>
        {!roomWorkspace && state.runtime?.ready && (
          <GameSettingsPopover model={session.settings}
            exit={state.runtime.launched && !state.replayViewer && !spectating
              ? {label: t(inRoom ? canReturnToRoom ? 'netplay.return' : 'player.leaveRoom' : 'player.exitGame'),
                onSelect: inRoom && !canReturnToRoom ? session.requestRoomLeave : navigation.closeSurface}
              : undefined}
          />
        )}
      </nav>
    </>
  );
}
