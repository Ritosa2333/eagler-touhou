import { Button as UiButton } from '../../ui/controls';
import { useContext, useLayoutEffect, useRef, useState, useSyncExternalStore, type ReactNode } from 'react';
import { createPortal } from 'react-dom';
import { touchMovementUsesJoystick } from '../../../src/launcher/game-preferences.mts';
import { PRODUCT_GAMES } from '../../../src/contracts/product-catalog.mts';
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
import { NetplayPlayerStatus } from './NetplayPlayerStatus';
import { PlayerStage } from './PlayerSurface';
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
  const spectatorTimeline = state.runtime?.launched && room.spectatorRequested && room.room?.spectatorTimeline === true;
  const mode = session.getPlayerMode(),
    spectating = mode === 'spectator';
  const netplay = network?.diagnostics as NetplayDiagnosticLines | undefined;
  const th09Information = state.runtime?.launched === true && (room.product === 'th09mp' || room.product === 'th19mp') && !state.replayViewer;
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
        {cooperativeSpectator && !roomWorkspace && <SpectatorPlayback session={session} />}
        {th09Information &&
          !roomWorkspace &&
          (spectatorTimeline ? (
            <SpectatorMatchScore
              wins={wins}
              minutes={Math.max(...(room.room?.seats ?? []).map((seat) => seat?.stats?.minutes ?? 0), 0)}
              names={[room.room?.seats?.[0]?.name ?? '', room.room?.seats?.[1]?.name ?? '']}
            />
          ) : !room.spectatorRequested && room.seat != null ? (
            <Th09MatchStats
              wins={wins}
              minutes={Math.max(...(room.room?.seats ?? []).map((seat) => seat?.stats?.minutes ?? 0), 0)}
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
        !roomWorkspace &&
        navigation.surface !== 'touch' &&
        state.playerOpen && <GameplayControls session={session} />}
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
function GameplayControls({ session }: { session: BrowserSession }) {
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
  const hostDirect =
    /\bAndroid\b|iPhone|iPad|iPod/i.test(navigator.userAgent) ||
    (/Macintosh/i.test(navigator.userAgent) && navigator.maxTouchPoints > 1);
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
        hidden={!(state.runtime?.launched && enabled && hostDirect && !wheel)}
      />
      <TouchControlPreview settings={settings} live={touch} spectator={spectator} />
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
            data-fullscreen={session.nativeTouch.isFullscreen()}
            aria-label={t(session.nativeTouch.isFullscreen() ? 'player.exitFullscreen' : 'player.enterFullscreen')}
            title={t(session.nativeTouch.isFullscreen() ? 'player.exitFullscreenTitle' : 'player.enterFullscreenTitle')}
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
