import {useEffect, useState} from 'react';
import {createRoot} from 'react-dom/client';
import {MemoryRouter} from 'react-router';
import {LocaleProvider, translate} from '../../app/i18n';
import {RoomWorkspace} from '../../app/components/room/RoomWorkspace';
import {RoomSettingsDrawer} from '../../app/components/room/RoomSettingsDrawer';
import {MultiplayerSettingsControls} from '../../app/components/room/MultiplayerSettingsControls';
import {OptionsPanel} from '../../app/components/launcher/OptionsPanel';
import {createLibraryProducts} from '../../app/components/launcher/products';
import {bookGameArtwork} from '../../app/components/launcher/book-artwork';
import {SettingsBody} from '../../app/components/settings/SettingsBody';
import {createGameSettingsModel} from '../../app/models/game-settings';
import {createKeyboardPreferences} from '../../src/launcher/keyboard-preferences.mts';
import {createGamepadMappingModel} from '../../src/launcher/gamepad-mapping.mts';
import {createMultiplayerQuickChatModel} from '../../src/launcher/multiplayer-quick-chat-model.mts';
import {createMultiplayerRoom} from '../../app/services/multiplayer-room';
import {createDecisionStore} from '../../app/models/decisions';
import {createMultiplayerIdentityStore, multiplayerDisplayNameStorageKey} from '../../src/launcher/multiplayer-identity.mts';
import {createMultiplayerRoomSessionStore} from '../../src/launcher/multiplayer-room-session.mts';
import {createMultiplayerPreferenceStore} from '../../src/launcher/multiplayer-preferences.mts';
import '../../app/ui/foundation.css';
import '../../app/components/launcher/book-ui.css';
import '../../app/components/launcher/book-surfaces.css';
import '../../app/components/room/room-light.css';
import '../../app/components/player/workspace-viewport.css';
import '../../app/components/room/room-presentation.css';
import '../../app/ui/system.css';
import '../../app/ui/scrollbars.css';
import '../../app/ui/tailwind.css';
import '../../app/components/launcher/options-sheet.css';
import '../../app/components/settings/settings-ui.css';

// Real room owner and views, with an in-memory transport and no runtime launch.
// This entry lives outside public/ and is never a published application route.
class Storage {
  values = new Map();
  getItem(key) {return this.values.get(key) ?? null;}
  setItem(key, value) {this.values.set(key, String(value));}
  removeItem(key) {this.values.delete(key);}
}
class Socket extends EventTarget {
  readyState = 0;
  send() {}
  open() {this.readyState = 1; this.dispatchEvent(new Event('open'));}
  receive(room) {this.dispatchEvent(new MessageEvent('message', {data:JSON.stringify({type:'state',roomDirectory:{version:1,controlModes:true},room})}));}
  close() {this.readyState = 3; this.dispatchEvent(new CloseEvent('close', {code:1000}));}
}
function fixture() {
  const persistent = new Storage(), session = new Storage(), decisions = createDecisionStore();
  persistent.setItem(multiplayerDisplayNameStorageKey, '界面检查');
  const identity = createMultiplayerIdentityStore({persistentStorage:persistent,sessionStorage:session,randomWords:()=>[1234567,7654321]});
  const socket = new Socket();
  const network = {
    subscribe:()=>()=>{},getSnapshot:()=>0,
    metric:()=>({state:'connected',rtt:43,jitter:2}), capabilities:()=>({supported:true,rtcAvailable:true,turnConfigured:true}),
    retry(){},testLoss(){},lossReady:()=>false,loss:()=>({state:'idle',sent:0}),
  };
  const service = createMultiplayerRoom({identity,decisions,
    sessions:createMultiplayerRoomSessionStore({storage:session}),preferences:createMultiplayerPreferenceStore({storage:persistent}),
    memberId:()=> 'visual-member',createSocket:()=>socket,relayUrl:()=> 'wss://visual.invalid/socket',
    settings:()=>({touchMovementMode:'joystick',touchEnabled:false,mobileDevice:false,iceServers:[]}),setMovementMode(){},
    translate:(key,params)=>translate('zh-CN',key,params),notify(){},
    prepareResources:()=>new Promise(()=>{}),preparationFailed(){},beginManualImport(){},launch:async()=>{},checkGame:async()=>{},operationFailed(){},isLaunched:()=>false,
    routes:{restore(){},enter(){},settleInvite(){},leave(){}},network:{reset(){},receive(){},minimumRtt:()=>43},online:()=>true,visible:()=>true,
  });
  const settings=createGameSettingsModel({storage:persistent});
  settings.hydrate({productId:'th06mp',uiLocale:'zh-CN',languages:[{id:'ja',title:'日本語'}],
    musicAvailability:{audio:true,midiAvailable:true,importServer:false,remoteOggAdvertised:false},webMidiAvailable:false,mobile:false});
  const midi={enabled:false,granted:false,busy:false,supported:false,outputs:[],selectedId:'',hint:''};
  const actions={confirm:async()=>false,file:async()=>{},showAppleNotice(){},feedback(){},reportError:console.error,
    externalMidi:{subscribe:()=>()=>{},getSnapshot:()=>midi,setEnabled:async()=>{},selectOutput:async()=>{}},
    keyboard:createKeyboardPreferences(persistent),gamepad:createGamepadMappingModel({storage:persistent,getGamepads:()=>[]})};
  const assetUrl=path=>'/'+path.replace(/^\//,'');
  const quickChat=createMultiplayerQuickChatModel({send(){},voice(){},timers:{setTimeout:()=>0,clearTimeout(){}}});
  return {service,network,decisions,socket,settings,actions,assetUrl,quickChat,
    product:{...createLibraryProducts(['th06mp'],assetUrl,id=>`/?game=${id}`)[0],artwork:bookGameArtwork('th06')},id:identity.lobbyClientId('th06mp')};
}
function Scenario() {
  const [model] = useState(fixture), [host,setHost] = useState(null), [stage,setStage] = useState(null), [panel,setPanel] = useState(null), [theme,setTheme] = useState('light');
  const [settingsOpen,setSettingsOpen]=useState(false),[settingsClosing,setSettingsClosing]=useState(false);
  useEffect(()=>{
    document.body.classList.add('mp-room-active');
    model.service.enterRoom('th06mp','1234',true);
    model.socket.open();
    model.socket.receive({code:'1234',playerCount:2,difficulty:1,phase:'lobby',inputDelay:0,startSerial:0,
        seats:[{clientId:model.id,name:'界面检查',loadout:0,ready:false,controlMode:'normal'},{clientId:'visual-peer',name:'示例玩家',loadout:1,ready:true,controlMode:'normal'}],
        spectators:[{clientId:'visual-watch',name:'示例观众'}]});
    model.quickChat.update({visible:true,room:'1234',serial:0,localSeat:0,localClientId:model.id,connected:true,inRoom:true,language:'zh-CN',
      seats:[{clientId:model.id,name:'界面检查'},{clientId:'visual-peer',name:'示例玩家'}],spectators:[{clientId:'visual-watch',name:'示例观众'}]});
    model.quickChat.receive({room:'1234',serial:0,seat:1,clientId:'visual-peer',phrase:'follow-me'});
    return ()=>{model.service.dispose();model.decisions.dispose();model.quickChat.dispose();document.body.classList.remove('mp-room-active');};
  },[model]);
  useEffect(()=>{document.documentElement.dataset.bookTheme=theme;document.documentElement.className=theme;document.body.classList.add('less-motion');},[theme]);
  return <LocaleProvider locale="zh-CN"><MemoryRouter>
    <header className="book-masthead" hidden aria-hidden="true"/>
    <style>{`@layer foundation {#root #player.room-workspace{height:calc(100dvh - 32px)!important;inset:0 0 32px!important}#root .room-workspace-actions{bottom:32px}}#root #player.room-workspace{height:calc(100dvh - 32px)!important;inset:0 0 32px!important}#root .room-workspace-actions{bottom:32px}`}</style>
    <aside style={{position:'fixed',zIndex:1000,bottom:0,left:0,right:0,background:'var(--book-surface)',color:'var(--book-ink)',padding:'6px 12px',font:'12px sans-serif',display:'flex',gap:12,alignItems:'center'}}>
      <span>本地界面示例 | 无网络、无游戏运行</span><button onClick={()=>setTheme(theme==='light'?'dark':'light')}>{theme==='light'?'深色':'亮色'}</button>
    </aside>
    <section ref={setHost} id="player" className="player active room-workspace" style={{bottom:32,height:'calc(100dvh - 32px)'}}><div ref={setStage} className="player-stage"><div id="gameViewport" className="game-viewport" style={{aspectRatio:'4/3'}}/></div></section>
    <RoomWorkspace service={model.service} network={model.network} workspaceHost={host} workspaceStage={stage}
      context={{launched:false,th09NetworkOverlayOpen:false,launchStage:null,touchEnabled:false,touchMovementMode:'joystick'}} assetUrl={model.assetUrl} quickChat={model.quickChat}
      panel={panel} onOpenPanel={(kind,peerId)=>setPanel({kind,peerId})} onClosePanel={()=>setPanel(null)} onLeave={()=>{}} onCopyRoomCode={()=>{}} onGuide={()=>{}} onOpenSettings={()=>setSettingsOpen(true)} settingsOpen={settingsOpen} settingsClosing={settingsClosing}/>
    <RoomSettingsDrawer roomOpen open={settingsOpen} onCloseRequest={()=>setSettingsOpen(false)} onClosingChange={setSettingsClosing}/>
    <OptionsPanel product={model.product} open={false} roomDrawer onBack={()=>setSettingsOpen(false)} assetUrl={model.assetUrl} lobbyHref="/lobby.html">
      <SettingsBody model={model.settings} actions={model.actions} onOpenTouchLayout={()=>{}}
        multiplayerControls={<MultiplayerSettingsControls onReplayViewer={()=>{}} onImport={()=>{}}/>}/>
    </OptionsPanel>
  </MemoryRouter></LocaleProvider>;
}
createRoot(document.getElementById('root')).render(<Scenario/>);
