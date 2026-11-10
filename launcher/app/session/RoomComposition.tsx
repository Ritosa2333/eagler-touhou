import { useLayoutEffect, useState, useSyncExternalStore } from 'react';
import { gameIdForProduct, isMultiplayerProductId } from '../../src/contracts/product-catalog.mts';
import { multiplayerGuidePath } from '../../lib/multiplayer-guide.mjs';
import { withUiLanguage } from '../../src/launcher/i18n.mts';
import {DEFAULT_GAME_OPTIONS} from '../../src/launcher/game-preferences.mts';
import { RoomView } from '../components/room';
import { RoomMusicInvite } from '../components/room/RoomMusic';
import type { BrowserSession } from './browser-session';
import { useLocale } from '../i18n';
import { useSurfaceNavigation } from '../navigation/surface-navigation';
import {SpectatorPlayback} from '../components/player/SpectatorPlayback';
import {SpectatorMatchScore} from '../components/player/SpectatorMatchScore';
import {GameSettingsPopover} from '../components/settings/GameAudioControls';

/** Room chrome and fullscreen share one permanent Player and room owner. */
export function RoomComposition({
  session,
  settingsClosing,
  workspaceVisible: roomWorkspace,
}: {
  session: BrowserSession;
  settingsClosing: boolean;
  workspaceVisible: boolean;
}) {
  const navigation = useSurfaceNavigation();
  const {t, locale} = useLocale();
  const guidePath = multiplayerGuidePath(navigation.productId && gameIdForProduct(navigation.productId));
  const transfer = useSyncExternalStore(
    session.transfer.subscribe,
    session.transfer.getSnapshot,
    session.transfer.getSnapshot,
  );
  const state = useSyncExternalStore(session.subscribe, session.getSnapshot, session.getSnapshot);
  const room = useSyncExternalStore(
    session.room.service.subscribe,
    session.room.service.getSnapshot,
    session.room.service.getSnapshot,
  );
  const settings = useSyncExternalStore(
    session.settings.subscribe,
    session.settings.getSnapshot,
    session.settings.getSnapshot,
  );
  const [workspace, setWorkspace] = useState<{host:HTMLElement;stage:HTMLElement} | null>(null);
  const workspaceVisible = roomWorkspace && navigation.surface !== 'touch';
  const canReturnToRoom = state.runtime?.runtimeVariant === 'multiplayer' && state.runtime.roomReturnSupported;
  const playback = state.runtime?.launched && state.runtime.spectator
    ? state.runtime.game !== 'th09' ? <SpectatorPlayback session={session}/>
      : room.room?.spectatorTimeline ? <SpectatorMatchScore
        wins={[room.room.seats?.[0]?.stats?.wins ?? 0,room.room.seats?.[1]?.stats?.wins ?? 0]}
        minutes={Math.max(...(room.room.seats ?? []).map(seat=>seat?.stats?.minutes ?? 0),0)}
        names={[room.room.seats?.[0]?.name ?? '',room.room.seats?.[1]?.name ?? '']}/> : null
    : null;
  useLayoutEffect(()=>{
    const host = document.getElementById('player'), stage = host?.querySelector<HTMLElement>('.player-stage');
    if (host && stage) setWorkspace({host,stage});
  }, []);
  return <>
    <RoomView
      visible={workspaceVisible && !!navigation.productId && isMultiplayerProductId(navigation.productId)}
      service={session.room.service}
      quickChat={session.room.quickChat}
      decisions={session.decisions}
      products={session.metadata.getSnapshot().products.filter(isMultiplayerProductId)}
      music={session.room.music}
      network={session.room.network}
      context={{
        transfer,
        launched: state.runtime?.launched === true,
        previewReady: state.runtime?.roomPreview === true,
        th09NetworkOverlayOpen: state.th09NetworkOverlayOpen,
        launchStage: state.roomLaunchStage === 'runtime' ? 'preparing' : state.roomLaunchStage,
        touchEnabled: settings?.options.touchEnabled === true,
        touchMovementMode: settings?.options.touchMovementMode ?? DEFAULT_GAME_OPTIONS.touchMovementMode,
      }}
      assetUrl={(path) => new URL(path, session.baseUrl).href}
      onLeave={session.leaveRoom}
      onCopyRoomCode={session.copyRoomCode}
      guideHref={withUiLanguage(new URL(guidePath.slice(1), session.baseUrl).href, locale)}
      onOpenSettings={navigation.openRoomSettings}
      settingsOpen={navigation.roomSettingsOpen}
      settingsClosing={settingsClosing}
      workspaceHost={workspace?.host}
      workspaceStage={workspace?.stage}
      playbackControls={playback}
      playerSettings={state.runtime?.ready && (
        <GameSettingsPopover model={session.settings}
          exit={state.playerOpen && state.runtime.launched
            ? {label: t(canReturnToRoom ? state.runtime.spectator ? 'player.exitSpectating' : 'netplay.return' : 'player.leaveRoom'),
              onSelect: canReturnToRoom ? navigation.closeSurface : session.requestRoomLeave}
            : undefined}
        />
      )}
      onFullscreen={()=>{void session.togglePlayerFullscreen();}}
      reloadPending={state.roomReloadPending}
      onReloadResources={session.reloadRoomResources}
      onRestoreSettings={session.restoreRoomSettings}
      panel={
        navigation.roomPanel
          ? { kind: navigation.roomPanel, peerId: navigation.roomNetworkPeer }
          : null
      }
      onOpenPanel={navigation.openRoomPanel}
      onClosePanel={navigation.closeRoomPanel}
    />
    {room.room && session.room.music && navigation.surface !== 'touch' &&
      <RoomMusicInvite model={session.room.music} panelOpen={!!navigation.roomPanel} />}
  </>;
}
