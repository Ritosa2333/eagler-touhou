import {useEffect, useState, useSyncExternalStore} from 'react';
import {createRoot} from 'react-dom/client';
import {createMemoryRouter, RouterProvider} from 'react-router';
import {LocaleProvider, translate} from '../../app/i18n';
import {SurfaceNavigationProvider, useSurfaceNavigation} from '../../app/navigation/surface-navigation';
import {PlayerSurface} from '../../app/components/player/PlayerSurface';
import {ReplayWorkspace} from '../../app/components/player/ReplayWorkspace';
import {createReplayModel} from '../../app/models/replays';
import {createDecisionStore} from '../../app/models/decisions';
import '../../app/ui/foundation.css';
import '../../app/components/launcher/book-ui.css';
import '../../app/components/launcher/book-surfaces.css';
import '../../app/ui/system.css';
import '../../app/ui/tailwind.css';

// Real Replay model, navigation and components. Only file/Runtime edges are
// in memory. The specimen bytes are not playable recordings or user data.
function createScenario(product = 'th06') {
  const decisions = createDecisionStore(), listeners = new Set();
  const seed = product === 'th09mp' ? ['replay/th9_01.rpy', 'replay/th9_02.rpy', 'replay/练习/th9_03.rpy']
    : ['replay/th6_01.rpy', 'replay/th6_02.rpy', 'replay/练习/th6_03.rpy'];
  const files = new Map(seed.map((path,index)=>[path,{path,bytes:new Uint8Array(1024*(index+2)),size:1024*(index+2),modifiedAt:1793356800000+index*60000}]));
  let snapshot = {locale:'zh-CN',replayViewer:false,runtime:{runtimeInfo:{}}}, status = '';
  const emit = patch => {snapshot = {...snapshot,...patch};for(const listener of listeners)listener();};
  const notify = message => {status = message;emit({});};
  const description = {name:'界面示例',date:'2026/10/30',character:0,shot:0,difficulty:1,score:12864000,slowdown:.125,
    stages:[{id:1,label:'STAGE 1',score:120000,frames:3600},{id:2,label:'STAGE 2',score:750000,frames:4800},{id:3,label:'STAGE 3',score:12864000,frames:6000}]};
  const game = product === 'th09mp' ? 'th09' : 'th06';
  const runtime = {
    getSnapshot:()=>({launched:false,ready:true,epoch:1,game}),
    async withFileSession(_game, operation) {return operation({epoch:1,sync:async()=>{},async send(command,payload){
      if(command==='list')return {files:[...files.values()].map(({bytes,...row})=>row)};
      if(command==='read')return {bytes:[...files.get(payload.path).bytes]};
      if(command==='write'){files.set(payload.path,{path:payload.path,bytes:Uint8Array.from(payload.bytes),size:payload.bytes.length,modifiedAt:1793356800000});return {};}
      if(command==='remove'){files.delete(payload.path);return {};}
      throw Error('Unsupported specimen file action');
    }});},
    async send(command,{action,value}) {
      if(command!=='playback')throw Error('No game Runtime is running in this specimen');
      const playback = {...snapshot.runtime.runtimeInfo.playback};
      if(action==='pause'||action==='play')playback.paused=action==='pause';
      if(action==='seek')playback.frame=value;
      if(action==='rate')playback.rate=value;
      emit({runtime:{runtimeInfo:{playback}}});
    },
  };
  const model = createReplayModel({runtime,prepareFiles:async()=>{},prepareViewer:async()=>{},releasePrepared:async()=>{},
    storage:{async read(_product,{path,directory}={}){return [...files.values()].filter(row=>(!path||row.path===path)&&(!directory||row.path.startsWith(directory+'/')));}},
    translate:(key,params)=>translate('zh-CN',key,params),prompt:(message,initial)=>window.prompt(message,initial),confirm:decision=>decisions.askConfirmation(decision),
    download(name,bytes,mime){const url=URL.createObjectURL(new Blob([bytes],{type:mime})),anchor=document.createElement('a');anchor.href=url;anchor.download=name;anchor.click();URL.revokeObjectURL(url);notify('已下载示例文件');},
    toast:notify,afterPaint:async()=>{},afterClose:()=>notify('已关闭示例录像列表'),inspect:async()=>description,
    async watch(_product,path,stage){emit({replayViewer:true,runtime:{runtimeInfo:{playback:{kind:'replay',path,frame:600,total:18000,paused:true,seeking:false,target:null,rate:1,ended:false,difficulty:1,left:0,right:1,stage:stage??1,description,roundStarts:game==='th09'?[0,6000,12000]:undefined}}}});},
  });
  const session = {subscribe(listener){listeners.add(listener);return()=>listeners.delete(listener);},getSnapshot:()=>snapshot,getReplays:()=>model,getRuntime:()=>runtime,
    baseUrl:new URL('/',location.href).href,nativeTouch:{isFullscreen:()=>!!document.fullscreenElement},
    async togglePlayerFullscreen(){if(document.fullscreenElement)await document.exitFullscreen();else await document.getElementById('gameViewport').requestFullscreen();emit({});},
    async requestRuntimeClose(){emit({replayViewer:false,runtime:{runtimeInfo:{}}});return true;},reportError:error=>notify(String(error)),
    async chooseReplayFile(){notify('此页面使用内存文件。真实导入在启动器验证。');},async importFile(){notify('此页面使用内存文件。真实导入在启动器验证。');},
  };
  return {model,session,decisions,product,status:()=>status,dispose(){model.dispose();decisions.dispose();}};
}
function ReplayScene({scenario}) {
  const navigation = useSurfaceNavigation(), [theme,setTheme] = useState('light');
  const app = useSyncExternalStore(scenario.session.subscribe,scenario.session.getSnapshot,scenario.session.getSnapshot);
  useEffect(()=>{document.body.classList.add('less-motion');void scenario.model.open(scenario.product);return()=>scenario.dispose();},[scenario]);
  useEffect(()=>{document.documentElement.dataset.bookTheme=theme;document.documentElement.className=theme;},[theme]);
  return <>
    <header className="book-masthead" hidden aria-hidden="true"/>
    <PlayerSurface open editing={false} replayWorkspace onElement={()=>{}}><ReplayWorkspace session={scenario.session}/></PlayerSurface>
    <aside className="visual-specimen-note"><span>本地界面示例 | 内存文件与播放状态 | 无游戏运行</span><button onClick={()=>{if(!scenario.model.isOpen())void scenario.model.open(scenario.product);navigation.openInfoDialog('replayDialog');}}>打开录像</button><button onClick={()=>setTheme(theme==='light'?'dark':'light')}>{theme==='light'?'深色':'亮色'}</button><output>{scenario.status()}</output></aside>
  </>;
}
const product = new URL(location.href).searchParams.get('game') === 'th09mp' ? 'th09mp' : 'th06';
const scenario = createScenario(product);
const router = createMemoryRouter([{path:'*',element:<LocaleProvider locale="zh-CN"><SurfaceNavigationProvider dirty={false} isEditing={false} onDiscard={()=>{}} decisions={scenario.decisions}><ReplayScene scenario={scenario}/></SurfaceNavigationProvider></LocaleProvider>}],{initialEntries:[`/?game=${product}`]});
const style = document.createElement('style');style.textContent='#player.replay-workspace{bottom:32px;height:calc(100dvh - 32px)}.visual-specimen-note{position:fixed;z-index:1000;bottom:0;left:0;right:0;display:flex;align-items:center;gap:12px;min-height:32px;padding:4px 12px;background:var(--book-surface);color:var(--book-ink);font:11px/1.4 sans-serif}.visual-specimen-note output{margin-left:auto}.visual-specimen-note button{cursor:pointer}';document.head.append(style);
createRoot(document.getElementById('root')).render(<RouterProvider router={router}/>);
