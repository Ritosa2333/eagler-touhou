import { useLayoutEffect, useRef, useState, useSyncExternalStore } from 'react';
import { isMultiplayerProductId } from '../../src/contracts/product-catalog.mts';
import {DEFAULT_GAME_OPTIONS} from '../../src/launcher/game-preferences.mts';
import { RoomView } from '../components/room';
import type { BrowserSession } from './browser-session';
import { useSurfaceNavigation } from '../navigation/surface-navigation';

/** Same room section is reparented at the two original fullscreen/launch seams.
 * Its React lifetime, native panel, controls and the Runtime iframe never reset. */
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
  const home = useRef<{ parent: Node; next: Node | null } | null>(null);
  const [workspace, setWorkspace] = useState<{host:HTMLElement;stage:HTMLElement} | null>(null);
  const workspaceVisible = roomWorkspace && navigation.surface !== 'touch';
  useLayoutEffect(()=>{
    const host = document.getElementById('player'), stage = host?.querySelector<HTMLElement>('.player-stage');
    setWorkspace(previous => workspaceVisible && host && stage ? previous?.host === host && previous.stage === stage ? previous : {host,stage} : null);
  }, [workspaceVisible]);
  useLayoutEffect(() => {
    if (workspaceVisible) return;
    const element = document.getElementById('mpRoomView'),
      player = document.getElementById('player');
    if (!element || !player) return;
    home.current ??= { parent: element.parentNode!, next: element.nextSibling };
    const target = state.th09NetworkOverlayOpen
      ? document.getElementById('th09NetworkRoom')
      : state.roomLaunchStage
        ? player
        : null;
    const restore = () => {
      const original = home.current;
      if (!original) return;
      if (original.next?.parentNode === original.parent)
        original.parent.insertBefore(element, original.next);
      else original.parent.appendChild(element);
      player.classList.remove('mp-room-launch-cover');
    };
    if (target) {
      target.append(element);
      player.classList.toggle('mp-room-launch-cover', !!state.roomLaunchStage);
    } else restore();
    return () => {
      // RoomView may already have removed its section when membership ends.
      // Moving that detached node back would resurrect an orphan room after
      // the real service/socket and Router had successfully completed Leave.
      if (element.isConnected) restore();
      else player.classList.remove('mp-room-launch-cover');
    };
  }, [state.th09NetworkOverlayOpen, state.roomLaunchStage, !!room.room, workspaceVisible]);
  return (
    <RoomView
      visible={!!navigation.productId && isMultiplayerProductId(navigation.productId)}
      service={session.room.service}
      quickChat={session.room.quickChat}
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
      onGuide={() => navigation.openInfoDialog('mpGuideDialog')}
      onOpenSettings={navigation.openRoomSettings}
      settingsOpen={navigation.roomSettingsOpen}
      settingsClosing={settingsClosing}
      workspaceHost={workspaceVisible ? workspace?.host : null}
      workspaceStage={workspaceVisible ? workspace?.stage : null}
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
  );
}
