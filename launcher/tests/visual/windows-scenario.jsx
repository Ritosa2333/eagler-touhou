import {useEffect,useState} from 'react';
import {createRoot} from 'react-dom/client';
import {LocaleProvider,translate} from '../../app/i18n';
import {GameDataImportWindows} from '../../app/components/player/GameDataImportWindows';
import {StartupError} from '../../app/components/feedback/StartupError';
import {Feedback} from '../../app/components/feedback/Feedback';
import {createFeedbackModel} from '../../app/models/feedback';
import {TransferPanel} from '../../app/components/player/TransferPanel';
import {TitleNetworkOverlay} from '../../app/components/player/TitleNetworkOverlay';
import {SpectatorPlayback} from '../../app/components/player/SpectatorPlayback';
import {NetplayCalibrationReport} from '../../app/components/NetplayCalibrationReport';
import {NetplayConnectionWindow} from '../../app/components/NetplayConnectionWindow';
import {NetworkDiagnosticResults} from '../../app/components/directory/NetworkDiagnosticResults';
import {TouchHelp} from '../../app/components/player/TouchHelp';
import {createGameDataImportModel} from '../../app/models/game-data-import';
import {createStartupErrorModel} from '../../app/models/startup-error';
import {createTransferModel} from '../../app/models/transfer';
import {createTitleNetwork} from '../../app/models/title-network';
import {createNetplayCalibration} from '../../app/models/netplay-calibration';
import '../../app/ui/foundation.css';
import '../../app/components/launcher/book-ui.css';
import '../../app/components/launcher/book-surfaces.css';
import '../../app/ui/system.css';
import '../../app/ui/tailwind.css';

// Actual presenters and owners with inert file/network/clipboard edges. The
// spectator transport supplies a fixed local stream; no game or relay runs.
function store(value){const listeners=new Set();return{subscribe:fn=>{listeners.add(fn);return()=>listeners.delete(fn);},getSnapshot:()=>value,update:patch=>{value={...value,...patch};for(const fn of listeners)fn();}};}
function fixture(){
  const t=(key,params)=>translate('zh-CN',key,params),timers={setTimeout:()=>0,clearTimeout(){}},feedback=createFeedbackModel(timers);
  const importer=createGameDataImportModel({translate:t,timers,
    getContext:()=>({product:'th06',roomCode:null,replayViewer:false,runtimeEpoch:null,runtimeReady:false,launched:false,playerOpen:false,importServer:false,
      fallback:{url:'https://example.invalid/visual-package.zip',hint:'界面示例'}}),
    acquisition:{importPackage:()=>new Promise(()=>{})},ports:{pickFile:async()=>null,closePlayer:async()=>true,resetUnlaunchedRuntime(){},openPlayer(){},launchConfigured:async()=>{},feedback,
      beginBlockingDownload:()=>({release(){}})}});
  const error=createStartupErrorModel({translate:t,launched:()=>false,copyText:async()=>true,toast(){},now:()=>new Date('2026-10-30T12:00:00Z')});
  const transfer=createTransferModel({translate:t,context:()=>({epoch:1,launched:false,iosWebKitTouch:false}),timers,
    frames:{request:fn=>{queueMicrotask(fn);return 0;},cancel(){}},ports:{retryMusic:async()=>{},cancelDownload(){transfer.hide();},...feedback}});
  const title=createTitleNetwork({translate:t,context:()=>({game:'th09',product:'th09',launched:true,relayUrl:'wss://visual.invalid'}),
    toast(){},sendNetworkCancel:async()=>{},enterRoom(){},leaveRoom(){},randomValues:array=>{array[0]=1234;return array;}});
  const calibration=createNetplayCalibration({epoch:1,userAgent:'Local UI specimen',now:()=>1793361600000,timers});
  const ready={phase:'ready',automatic:true,adonisMode:2,inputDelay:1,fullDelay:3,predictionReserve:2,rttP95Us:100000,samples:120,lost:0,route:'rtc',
    calibration:{game:'th10mp',build:'visual-specimen',completedAt:100,localPlayer:0,players:[0,1].map(player=>({player,p95Us:100000,samples:120,lost:0,minUs:1000,maxUs:150000,meanUs:50000}))}};
  calibration.record(ready,1);calibration.dismiss();
  const playback=store({run:'visual-run',min:0,position:7200,edge:18000,received:18000,target:null,paused:true,seeking:false,following:false,ended:false,
    nodes:[{id:1,frame:0,stage:1,kind:'stage'},{id:2,frame:6000,stage:1,kind:'boss'},{id:3,frame:12000,stage:2,kind:'stage'}]});
  const spectator={baseUrl:new URL('/',location.href).href,getSnapshot:()=>({runtime:{epoch:1}}),spectatorPlayback:playback,
    async controlSpectator(action,frame){if(action==='seek')playback.update({position:frame,following:false});else if(action==='live')playback.update({position:18000,following:true,paused:false});else playback.update({paused:action==='pause'});}};
  return{importer,error,transfer,title,calibration,ready,spectator,feedback,dispose(){for(const model of [importer,error,transfer,title,calibration,feedback])model.dispose();}};
}
function Scenario(){
  const [model]=useState(fixture),[theme,setTheme]=useState('light'),[view,setView]=useState('spectator'),[framing,setFraming]=useState(false);
  useEffect(()=>()=>model.dispose(),[model]);
  useEffect(()=>{document.documentElement.dataset.bookTheme=theme;document.documentElement.className=theme;document.body.classList.add('less-motion');},[theme]);
  const closeWindows=()=>{model.importer.closeLink();model.importer.dismissImport();model.error.close();model.title.close(false);model.transfer.hide();model.calibration.closeReport();model.calibration.dismiss();model.feedback.dismiss();};
  const show=kind=>{closeWindows();setView(kind);if(kind==='import')model.importer.openManual();if(kind==='error')model.error.show('资源读取失败 | 本地界面示例','读取游戏资源');
    if(kind==='title')model.title.open();if(kind==='transfer'){model.transfer.runtimeTransfer({kind:'game',title:'准备游戏资源',label:'th06 | 游戏数据',loaded:48*1024*1024,total:128*1024*1024,speed:4*1024*1024,phase:'downloading'});model.transfer.setCancellation('取消下载');}
    if(kind==='transfer-error')model.transfer.musicFailure(2);if(kind==='connection')model.calibration.record({phase:'measuring',probes:72,replies:67},1);if(kind==='calibration'){model.calibration.record(model.ready,1);model.calibration.openReport('本地界面示例');}if(kind==='toast')model.feedback.toast('游戏资源已准备完成，可以开始游玩。');};
  return <LocaleProvider locale="zh-CN"><style>{framing?'.window-specimen-nav,.window-specimen-note{visibility:hidden}':''}</style><button id="specimenFraming" style={{position:'fixed',bottom:0,left:0,zIndex:1100,font:'11px sans-serif',padding:4}} onClick={()=>setFraming(!framing)}>{framing?'显示示例工具':'截图视图'}</button><header className="book-masthead" hidden aria-hidden="true"/>
    <nav className="window-specimen-nav" aria-label="界面示例">{[['import','资源导入'],['transfer','下载任务'],['transfer-error','下载失败'],['error','错误详情'],['title','标题联机'],['spectator','旁观控制'],['connection','联机状态'],['calibration','标定报告'],['diagnostics','网络诊断'],['help','触控帮助'],['fighting','格斗操作'],['toast','提示消息']].map(([id,label])=><button key={id} onClick={()=>show(id)}>{label}</button>)}</nav>
    <section id="player" className="player active window-specimen-player"><div className="window-specimen-game">游戏画面示例</div>
      {view==='spectator'&&<SpectatorPlayback session={model.spectator}/>}
      {view==='connection'&&<NetplayConnectionWindow network={{calibrationEligible:true,connection:null,returnToRoom:true}} calibration={model.calibration} english={false} onReturn={()=>show('spectator')}/>}
      {view==='diagnostics'&&<div className="window-specimen-diagnostics" style={{position:'relative',zIndex:2,width:440,maxWidth:'100%'}}><NetworkDiagnosticResults snapshot={{hidden:false,rows:{ws:{state:'good',value:'正常'},turn:{state:'good',value:'可用'},nat:{state:'bad',value:'Symmetric'},ipv6:{state:'bad',value:'不可用'}}}}/></div>}
      <TransferPanel model={model.transfer}/><TitleNetworkOverlay model={model.title}/>
      {(view==='help'||view==='fighting')&&<TouchHelp open touchEnabled={view==='help'} nativeControls={view==='fighting'} focusMode="hold-button" showIosFullscreenHelp={false} onCloseRequest={()=>setView('spectator')}/>}
    </section>
    <GameDataImportWindows model={model.importer} getRuntimeReady={()=>false}/><StartupError model={model.error}/><Feedback model={model.feedback}/>
    <div hidden><NetplayCalibrationReport model={model.calibration} english={false} currentNetwork={()=>'本地界面示例'} clipboard={{writeText:async()=>{}}}/></div>
    <aside className="window-specimen-note">本地状态示例 | 无游戏、网络或文件操作 <button onClick={()=>setTheme(theme==='light'?'dark':'light')}>{theme==='light'?'深色':'亮色'}</button></aside>
  </LocaleProvider>;
}
const style=document.createElement('style');style.textContent='.window-specimen-nav{position:fixed;top:0;left:0;right:0;z-index:1000;display:flex;flex-wrap:wrap;gap:8px;padding:8px;background:var(--book-surface);color:var(--book-ink);font:12px sans-serif}.window-specimen-nav button{padding:5px 8px}#root #player.window-specimen-player{position:fixed;inset:64px 12px 44px;display:flex;align-items:center;justify-content:center;gap:20px;padding:20px;border:1px solid var(--book-border);background:var(--book-bg);color:var(--book-ink);font:13px sans-serif}.window-specimen-game{position:absolute;inset:0;display:grid;place-items:center;background:#050505;color:#a0a0aa}.window-specimen-note{position:fixed;z-index:1000;bottom:0;left:0;right:0;min-height:32px;padding:5px 12px;background:var(--book-surface);color:var(--book-ink);font:11px sans-serif}.window-specimen-note button{margin-left:12px}#root .spectator-playback-host{position:relative;width:560px;max-width:100%;z-index:2}#root #player .netplay-connection-window{position:relative;z-index:2}';document.head.append(style);
createRoot(document.getElementById('root')).render(<Scenario/>);
