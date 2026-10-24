import {useLayoutEffect, useRef, useState, useSyncExternalStore} from 'react';
import {createPortal} from 'react-dom';
import {useLocale} from '../../i18n';
import type {RoomViewProps, RoomPanelKind} from './types';
import {roomPresentation, roomConnectionMessage} from './presentation';
import {RoomSeats} from './RoomSeats';
import {RoomProgress} from './RoomProgress';
import {RoomGameSettings} from './RoomGameSettings';
import {RoomQuickChat} from './RoomQuickChat';
import {RoomSpectators} from './RoomSpectators';
import {RoomSpectatorPreview} from './RoomSpectatorPreview';
import {RoomPlaybackPanel} from './RoomPlaybackPanel';
import {RoomPanel} from './RoomPanel';
import {RoomPersonal} from './RoomPersonal';
import {RoomNetwork} from './RoomNetwork';
import {playerUtilityIcons} from '../player/utility-icons';
import './room-workspace.css';

/** Composes chrome around Player's permanent iframe. Only React-owned UI uses
 * portals: iframe navigation, native preparation and room membership retain their
 * existing owners. No placeholder participant or progress is created here. */
export function RoomWorkspace(props: RoomViewProps) {
  const {t} = useLocale(), {service, network, context, assetUrl} = props;
  const state = useSyncExternalStore(service.subscribe, service.getSnapshot, service.getSnapshot);
  useSyncExternalStore(network.subscribe, network.getSnapshot, network.getSnapshot);
  const [tab, setTab] = useState<'chat'|'game'>('chat');
  const [narrow, setNarrow] = useState(false);
  const [loungeHost, setLoungeHost] = useState<HTMLDivElement | null>(null);
  const [playbackHost, setPlaybackHost] = useState<HTMLDivElement | null>(null);
  const trigger = useRef<HTMLElement | null>(null);
  const retainedPanel = useRef<NonNullable<RoomViewProps['panel']>>({kind:'personal'});
  if (props.panel) retainedPanel.current = props.panel;
  const currentPanel = props.panel || retainedPanel.current;
  const code = state.room?.code;
  useLayoutEffect(()=>{setTab('chat');}, [state.product,code]);
  useLayoutEffect(()=>{service.setRoomSettingsOpen(props.visible !== false && tab === 'game');return ()=>service.setRoomSettingsOpen(false);}, [service,tab,props.visible]);
  useLayoutEffect(()=>{
    const media = window.matchMedia('(max-width:760px)');
    const update = ()=>setNarrow(media.matches);
    update(); media.addEventListener('change',update);
    return ()=>media.removeEventListener('change',update);
  },[]);
  const view = roomPresentation(state,t);
  if (!view || !props.workspaceHost || !props.workspaceStage || props.visible === false) return null;
  const {room,game,config,ready,owner} = view;
  const viewport = props.workspaceStage.querySelector<HTMLElement>('.game-viewport');
  const activePanel = props.panel?.kind === 'personal' || props.panel?.kind === 'network' || props.panel?.kind === 'spectators';
  const openPanel = (kind:RoomPanelKind, element:HTMLElement, peerId?:string)=>{trigger.current=element;props.onOpenPanel(kind,peerId);};
  const sideLounge = narrow;
  const locked = room.phase !== 'lobby' || state.launchBusy || !!context.launchStage;
  const selecting = state.seat != null && !state.spectatorRequested;
  const preparation = state.preparation;
  const loading = !!context.launchStage || preparation?.status === 'preparing' || preparation?.status === 'importing';
  const loadingText = t(context.launchStage ? 'room.preparingRuntime' : loading ? 'room.workspaceLoading' : preparation?.status === 'ready' ? selecting ? 'room.workspaceReady' : state.spectatorRequested ? 'room.workspaceSpectating' : 'multiplayer.chooseSeatOrSpectate' : 'room.resourcesUnavailable');
  const icon = (name:string)=><img className="room-icon" src={assetUrl(`assets/${name}.svg`)} alt=""/>;
  const lounge = <RoomSpectatorPreview state={state} service={service} assetUrl={assetUrl} open={props.panel?.kind==='spectators'} onOpen={element=>openPanel('spectators',element)}/>;
  const roster = <div className="room-workspace-roster" data-player-count={room.playerCount}><RoomSeats state={state} service={service} network={network} context={context} openPanel={openPanel}/><div className="room-workspace-lounge-host" ref={setLoungeHost}/></div>;
  return <>
    {createPortal(<section id="mpRoomView" className="mp-room-view room-workspace-view" data-public-names={state.nickname ? 'true' : undefined} data-roster-identities="true" aria-label={`${game.title} ${t('multiplayer.roomAria')}`} inert={props.settingsOpen}>
      <header className="mp-room-head room-workspace-header">
        <button className="mp-room-back mp-room-top-action" id="mpLeaveRoom" type="button" aria-label={t('action.back')} onClick={()=>{void props.onLeave();}}>{icon('room-caret-left')}</button>
        <div className="room-workspace-heading"><h2 id="mpRoomTitle">{game.title}</h2><div className="room-workspace-meta"><span id="mpRoomMode">{t(state.product === 'th09mp' ? 'room.versus' : 'room.coop',{count:room.playerCount})}</span><span id="mpRoomDifficultyBadge">{config.difficulties[room.difficulty] || 'Normal'}</span>{room.challengeMode && <span className="room-challenge-on">{t('room.challengeEnabled')}</span>}</div></div>
        <div className="room-workspace-top-actions"><button className="mp-room-code mp-room-top-action" id="mpCopyRoomCode" type="button" aria-label={t('multiplayer.copyRoomCode')} onClick={()=>{void props.onCopyRoomCode(room.code);}}><span className="mp-room-code-label">{t('multiplayer.roomCode')}</span><strong id="mpRoomCode">{room.code}</strong>{icon('room-copy')}</button>
          <button className="mp-room-top-action" id="mpSettingsRoomDrawerToggle" type="button" aria-label={t('settings.title')} aria-expanded={props.settingsOpen} aria-controls="mpSettingsRoomDrawer" disabled={locked} onClick={props.onOpenSettings}>{icon('room-gear-six')}<span>{t('room.gameSettingsGroup')}</span></button></div>
      </header>
      <div className="room-workspace-column"><aside className="room-workspace-sidebar"><nav className="room-workspace-tabs" data-active-tab={tab} role="tablist" aria-label={t('multiplayer.gameSettings')}>
        {(['chat','game'] as const).map((name,index)=><button id={`roomWorkspaceTab-${name}`} key={name} type="button" role="tab" aria-selected={tab===name} tabIndex={tab===name ? 0 : -1} aria-controls={`roomWorkspacePanel-${name}`} onClick={()=>setTab(name)} onKeyDown={event=>{if(event.key==='ArrowLeft'||event.key==='ArrowRight'){event.preventDefault();const next=index ? 'chat' : 'game';setTab(next);document.getElementById(`roomWorkspaceTab-${next}`)?.focus();}}}>{t(name==='chat' ? 'room.quickChat' : 'multiplayer.gameSettings')}</button>)}
      </nav><div className="room-workspace-panels">
        <div className="room-workspace-chat" id="roomWorkspacePanel-chat" role="tabpanel" aria-labelledby="roomWorkspaceTab-chat" hidden={tab!=='chat'}>{props.quickChat && <><RoomQuickChat model={props.quickChat} part="log"/><RoomQuickChat model={props.quickChat} part="actions"/></>}</div>
        <div id="roomWorkspacePanel-game" role="tabpanel" aria-labelledby="roomWorkspaceTab-game" hidden={tab!=='game'}><fieldset disabled={locked}><RoomGameSettings state={state} service={service} hidden={false}/></fieldset><button id="mpRoomGuideOpen" type="button" onClick={props.onGuide}>{t('multiplayerGuide.action')}</button></div>
      </div></aside>
      <div className="room-workspace-playback-host" ref={setPlaybackHost}/>
      {!sideLounge && lounge}
      </div>
      <footer className="room-workspace-actions" hidden={!room.synced || !selecting}>
        <button className="mp-ready-button" id="mpReady" type="button" disabled={!ready || locked || state.loadoutPending || state.checkBusy || (!state.ready && preparation?.status !== 'ready')} aria-pressed={state.ready} onClick={()=>{void service.toggleReady();}}>{t(state.ready ? 'room.cancelReady' : 'multiplayer.ready')}</button>
        <button className="mp-start-button" id="mpStartGame" type="button" disabled={locked || !owner || !state.canStart} onClick={service.start}>{t('multiplayer.startGame')}</button>
      </footer>
      <RoomPanel open={!!activePanel} kind={currentPanel.kind} peerId={currentPanel.peerId} trigger={trigger} onCloseRequest={props.onClosePanel} onClosed={()=>{}}>
        <RoomPersonal state={state} service={service} network={network} assetUrl={assetUrl} th09NetworkOverlayOpen={context.th09NetworkOverlayOpen} hidden={currentPanel.kind!=='personal'}/>
        <RoomNetwork state={state} network={network} context={context} peerId={currentPanel.peerId} hidden={currentPanel.kind!=='network'}/>
        <RoomSpectators state={state} service={service} hidden={currentPanel.kind!=='spectators'}/>
      </RoomPanel>
    </section>,props.workspaceHost)}
    {createPortal(roster,props.workspaceStage)}
    {(narrow || playbackHost) && createPortal(<RoomPlaybackPanel workspace={props.workspaceHost}>{props.playbackControls}</RoomPlaybackPanel>, narrow ? props.workspaceStage : playbackHost!)}
    {sideLounge && loungeHost && createPortal(lounge,loungeHost)}
    {viewport && createPortal(<>
      {props.reloadPending && !context.launched && <section className="room-resource-reload" role="region" aria-label={t('room.reloadResources')}>
        <p role="status">{t('room.reloadSettingsPrompt')}</p>
        <div><button type="button" className="room-resource-reload-confirm" onClick={props.onReloadResources}>{t('room.reloadResources')}</button><button type="button" onClick={props.onRestoreSettings}>{t('room.restoreSettings')}</button></div>
      </section>}
      <div className="room-workspace-preparation" role="status" data-preview-ready={context.previewReady || context.launched || undefined} hidden={props.reloadPending || !context.launchStage && (context.previewReady || context.launched)}><div className="room-preparation-caption">{loading && <i className="room-preparation-spinner" aria-hidden="true"/>}<strong>{loadingText}</strong><span id="mpRoomConnection">{!ready && roomConnectionMessage(state,t)}</span><RoomProgress state={state} service={service} transfer={context.transfer}/>{preparation?.error && <p>{preparation.error}</p>}</div></div>
      <button className="room-workspace-fullscreen" type="button" aria-label={t('player.enterFullscreen')} onClick={props.onFullscreen} dangerouslySetInnerHTML={{__html:playerUtilityIcons.fullscreenToggle}}/>
    </>,viewport)}
  </>;
}
