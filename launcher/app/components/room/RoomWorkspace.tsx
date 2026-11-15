import {RoomMusicStrip,RoomMusicPanel,RoomMusicToggle,type MusicPanelTab} from './RoomMusic';
import {Tabs, TabsContent, TabsList, TabsTrigger} from '../../ui/tabs';
import {Button as UiButton} from '../../ui/controls';
import {BookProductMenu} from '../launcher/BookDirectoryGames';
import {ChevronDown} from 'lucide-react';
import {useLayoutEffect, useRef, useState, useSyncExternalStore} from 'react';
import {createPortal} from 'react-dom';
import {useLocale} from '../../i18n';
import {Th09RatingCardsContext, useTh09Ratings} from '../../services/th09-ratings';
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
import {FullscreenIcons} from '../player/utility-icons';
import './room-workspace.css';

const noMusicSubscription = () => () => {};

/** Composes chrome around Player's permanent iframe. Only React-owned UI uses
 * portals: iframe navigation, native preparation and room membership retain their
 * existing owners. No placeholder participant or progress is created here. */
export function RoomWorkspace(props: RoomViewProps) {
  const {t} = useLocale(), {service, network, context, assetUrl} = props;
  const state = useSyncExternalStore(service.subscribe, service.getSnapshot, service.getSnapshot);
  const {players, mine, cards} = useTh09Ratings(
    props.visible !== false && !!props.workspaceHost && !!props.workspaceStage && state.product === 'th09mp' && !!state.room,
    [...(state.room?.seats ?? []), ...(state.room?.spectators ?? [])].map(entry => entry?.profile?.username));
  useSyncExternalStore(network.subscribe, network.getSnapshot, network.getSnapshot);
  const [tab, setTab] = useState<'chat'|'game'>('chat');
  const [musicOpen,setMusicOpen]=useState(false);
  const [musicTab,setMusicTab]=useState<MusicPanelTab>('search');
  const musicJoined = useSyncExternalStore(
    props.music?.subscribe ?? noMusicSubscription, () => props.music?.getSnapshot().joined === true, () => false,
  );
  const [narrow, setNarrow] = useState(false);
  const [loungeHost, setLoungeHost] = useState<HTMLDivElement | null>(null);
  const [playbackHost, setPlaybackHost] = useState<HTMLDivElement | null>(null);
  const trigger = useRef<HTMLElement | null>(null);
  const focusMusicSearch = useRef(false);
  const retainedPanel = useRef<NonNullable<RoomViewProps['panel']>>({kind:'personal'});
  if (props.panel) retainedPanel.current = props.panel;
  const currentPanel = props.panel || retainedPanel.current;
  const code = state.room?.code;
  // biome-ignore lint/correctness/useExhaustiveDependencies: A different room starts on its chat tab.
  useLayoutEffect(()=>{setTab('chat');setMusicOpen(false);setMusicTab('search');}, [state.product,code]);
  useLayoutEffect(()=>{if(musicJoined)setMusicOpen(true);}, [musicJoined]);
  useLayoutEffect(()=>{service.setRoomSettingsOpen(props.visible !== false && tab === 'game');return ()=>service.setRoomSettingsOpen(false);}, [service,tab,props.visible]);
  useLayoutEffect(()=>{
    const media = window.matchMedia('(max-width: 760px), (max-width: 1000px) and (pointer: coarse)');
    const update = ()=>setNarrow(media.matches);
    update(); media.addEventListener('change',update);
    return ()=>media.removeEventListener('change',update);
  },[]);
  const view = roomPresentation(state,t);
  if (!view || !props.workspaceHost || !props.workspaceStage || props.visible === false) return null;
  const {room,game,config,ready,owner} = view;
  const viewport = props.workspaceStage.querySelector<HTMLElement>('.game-viewport');
  const activePanel = props.panel?.kind === 'personal' || props.panel?.kind === 'network' || props.panel?.kind === 'spectators' ||
    (props.panel?.kind === 'music' && !!props.music);
  const openPanel = (kind:RoomPanelKind, element:HTMLElement, peerId?:string)=>{
    trigger.current=element;
    props.onOpenPanel(kind,peerId);
  };
  const sideLounge = narrow;
  const locked = room.phase !== 'lobby' || state.launchBusy || !!context.launchStage;
  const canChangeProduct = owner && state.globalRoomCodes && !!state.product && props.products.length > 1;
  const selecting = state.seat != null && !state.spectatorRequested;
  const preparation = state.preparation;
  const loading = !!context.launchStage || preparation?.status === 'preparing' || preparation?.status === 'importing';
  const loadingText = t(context.launchStage ? 'room.preparingRuntime' : loading ? 'room.workspaceLoading' : preparation?.status === 'ready' ? selecting ? 'room.workspaceReady' : state.spectatorRequested ? 'room.workspaceSpectating' : 'multiplayer.chooseSeatOrSpectate' : preparation?.status === 'cancelled' ? 'room.resourcesCancelled' : 'room.resourcesUnavailable');
  const icon = (name:string)=><img className="room-icon" src={assetUrl(`assets/${name}.svg`)} alt=""/>;
  const lounge = <RoomSpectatorPreview state={state} service={service} assetUrl={assetUrl} open={props.panel?.kind==='spectators'} onOpen={element=>openPanel('spectators',element)}/>;
  const roster = <div className="room-workspace-roster" data-side-lounge={sideLounge} data-player-count={room.playerCount}><RoomSeats state={state} service={service} network={network} context={context} openPanel={openPanel} ratings={players} myRating={mine}/><div className="room-workspace-lounge-host" ref={setLoungeHost}/></div>;
  return <Th09RatingCardsContext.Provider value={state.product === 'th09mp' ? cards : undefined}>
    {createPortal(<section id="mpRoomView" className="mp-room-view room-workspace-view" data-public-names={state.nickname ? 'true' : undefined} data-roster-identities="true" aria-label={`${game.title} ${t('multiplayer.roomAria')}`} inert={props.settingsOpen}>
      <header className="mp-room-head room-workspace-header">
        <UiButton variant="icon" className="mp-room-back mp-room-top-action" id="mpLeaveRoom" type="button" aria-label={t('action.back')} onClick={()=>{void props.onLeave();}}>{icon('room-caret-left')}</UiButton>
        <div className="room-workspace-heading">
          <div className="workspace-product-heading">
            <h2 id="mpRoomTitle">{game.title}</h2>
            {canChangeProduct && state.product && <BookProductMenu products={props.products} value={state.product}
              assetUrl={assetUrl} onChange={service.switchProduct}
              trigger={<UiButton id="roomProductSelect" variant="unstyled" className="workspace-product-trigger"
                disabled={locked || !state.connected || state.checkBusy} aria-label={t('lobby.game')} title={t('lobby.game')}>
                <ChevronDown aria-hidden="true"/>
              </UiButton>}/>}
          </div>
          <div className="room-workspace-meta"><span id="mpRoomMode">{t(config.gameplay === 'versus' ? 'room.versus' : 'room.coop',{count:room.playerCount})}</span><span id="mpRoomDifficultyBadge">{config.difficulties[room.difficulty] || 'Normal'}</span>{room.challengeMode && <span className="room-challenge-on">{t('room.challengeEnabled')}</span>}</div>
        </div>
        <div className="room-workspace-top-actions"><UiButton size="lg" className="mp-room-code mp-room-top-action" id="mpCopyRoomCode" type="button" aria-label={t('multiplayer.copyRoomCode')} onClick={()=>{void props.onCopyRoomCode(room.code);}}><span className="mp-room-code-label">{t('multiplayer.roomCode')}</span><strong id="mpRoomCode">{room.code}</strong>{icon('room-copy')}</UiButton>
          <UiButton size="lg" className="mp-room-top-action" id="mpSettingsRoomDrawerToggle" type="button" aria-label={t('settings.title')} aria-expanded={props.settingsOpen} aria-controls="mpSettingsRoomDrawer" disabled={locked} onClick={props.onOpenSettings}>{icon('room-gear-six')}<span>{t('room.gameSettingsGroup')}</span></UiButton></div>
      </header>
      <div className="room-workspace-column"><Tabs value={tab} onValueChange={value=>setTab(value as 'chat'|'game')} asChild><aside className="room-workspace-sidebar"><div className="room-workspace-tab-bar"><TabsList asChild><div className="room-workspace-tabs" role="tablist" aria-label={t('multiplayer.gameSettings')}>
        {(['chat','game'] as const).map(name=><TabsTrigger key={name} value={name} id={`roomWorkspaceTab-${name}`} aria-controls={`roomWorkspacePanel-${name}`}>{t(name==='chat'?'chat.title':'multiplayer.gameSettings')}</TabsTrigger>)}
      </div></TabsList>
        {props.music && <RoomMusicToggle model={props.music} open={musicOpen} onToggle={()=>setMusicOpen(open=>!open)}/>}
      </div><div className="room-workspace-panels">
        <TabsContent value="chat" forceMount asChild>
          <div className="room-workspace-chat" id="roomWorkspacePanel-chat" role="tabpanel" aria-labelledby="roomWorkspaceTab-chat" hidden={tab!=='chat'}>
            {props.quickChat && <>
              <RoomQuickChat model={props.quickChat} part="log" mobile={narrow} decisions={props.decisions}/>
              <RoomQuickChat model={props.quickChat} part="actions" mobile={narrow}/>
            </>}
          </div>
        </TabsContent>
        <TabsContent value="game" forceMount asChild><div id="roomWorkspacePanel-game" role="tabpanel" aria-labelledby="roomWorkspaceTab-game" hidden={tab!=='game'}><fieldset disabled={locked}><RoomGameSettings state={state} service={service} hidden={false}/></fieldset><UiButton variant="quiet" size="sm" id="mpRoomGuideOpen" asChild><a href={props.guideHref} target="_blank" rel="noopener noreferrer">{t('multiplayerGuide.action')}</a></UiButton></div></TabsContent>
      </div></aside></Tabs>
      <div className="room-workspace-playback-host" ref={setPlaybackHost}/>
      {props.music && musicOpen && <RoomMusicStrip model={props.music} open={props.panel?.kind==='music'} onOpen={(element,tab,keyboard)=>{
        focusMusicSearch.current = tab === 'search' && keyboard;
        setMusicTab(tab);
        openPanel('music',element);
      }}/>}
      {!sideLounge && lounge}
      </div>
      <footer className="room-workspace-actions" hidden={!room.synced || !selecting}>
        <UiButton variant="primary" className="mp-ready-button" id="mpReady" type="button" disabled={!ready || locked || state.loadoutPending || state.checkBusy || (!state.ready && preparation?.status !== 'ready')} aria-pressed={state.ready} onClick={()=>{void service.toggleReady();}}>{t(state.ready ? 'room.cancelReady' : 'multiplayer.ready')}</UiButton>
        <UiButton variant="primary" className="mp-start-button" id="mpStartGame" type="button"
          disabled={locked || (owner ? !state.canStart : !state.canNudge)} onClick={owner ? service.start : service.nudge}>
          {t(owner ? 'multiplayer.startGame' : 'room.nudge')}
        </UiButton>
      </footer>
      <RoomPanel open={!!activePanel} kind={currentPanel.kind} peerId={currentPanel.peerId} trigger={trigger}
        initialFocus={currentPanel.kind==='music' && focusMusicSearch.current ? '#roomMusicQuery' : undefined}
        onCloseRequest={props.onClosePanel}>
        <RoomPersonal state={state} service={service} network={network} assetUrl={assetUrl} th09NetworkOverlayOpen={context.th09NetworkOverlayOpen} hidden={currentPanel.kind!=='personal'}/>
        <RoomNetwork state={state} network={network} context={context} peerId={currentPanel.peerId} hidden={currentPanel.kind!=='network'}/>
        {props.music && <RoomMusicPanel model={props.music} hidden={currentPanel.kind!=='music'} tab={musicTab} onTabChange={setMusicTab}/>}
        <RoomSpectators state={state} service={service} hidden={currentPanel.kind!=='spectators'}/>
      </RoomPanel>
    </section>,props.workspaceHost)}
    {createPortal(roster,props.workspaceStage)}
    {playbackHost && createPortal(<RoomPlaybackPanel workspace={props.workspaceHost}>{props.playbackControls}</RoomPlaybackPanel>, playbackHost)}
    {sideLounge && loungeHost && createPortal(lounge,loungeHost)}
    {viewport && createPortal(<>
      {props.reloadPending && !context.launched && <section className="room-resource-reload" aria-label={t('room.reloadResources')}>
        <p role="status">{t('room.reloadSettingsPrompt')}</p>
        <div><UiButton variant="primary" type="button" className="room-resource-reload-confirm" onClick={props.onReloadResources}>{t('room.reloadResources')}</UiButton><UiButton type="button" onClick={props.onRestoreSettings}>{t('room.restoreSettings')}</UiButton></div>
      </section>}
      <div className="room-workspace-preparation" role="status" data-preview-ready={context.previewReady || context.launched || undefined} hidden={props.reloadPending || !context.launchStage && (context.previewReady || context.launched)}><div className="room-preparation-caption">{loading && <i className="room-preparation-spinner" aria-hidden="true"/>}<strong>{loadingText}</strong><span id="mpRoomConnection">{!ready && roomConnectionMessage(state,t)}</span><RoomProgress state={state} service={service} transfer={context.transfer} showLabel={preparation?.status !== 'failed' && preparation?.status !== 'cancelled'}/>{preparation?.error && preparation.error !== loadingText && <p>{preparation.error}</p>}</div></div>
      <div className="room-workspace-viewport-tools">
        <UiButton variant="unstyled" className="room-workspace-fullscreen" type="button" aria-label={t('player.enterFullscreen')} onClick={props.onFullscreen}><FullscreenIcons/></UiButton>
        {props.playerSettings}
      </div>
    </>,viewport)}
  </Th09RatingCardsContext.Provider>;
}
