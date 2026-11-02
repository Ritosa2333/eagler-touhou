import {useEffect,useState,useSyncExternalStore} from 'react';
import {createRoot} from 'react-dom/client';
import {MemoryRouter} from 'react-router';
import {LocaleProvider,translate} from '../../app/i18n';
import {LobbyDirectoryView} from '../../app/components/directory/LobbyDirectoryView';
import {DirectoryRoomDialog} from '../../app/components/directory/DirectoryRoomDialog';
import {createLobbyDirectory} from '../../app/services/lobby-directory';
import {createLibraryProducts} from '../../app/components/launcher/products';
import {bookArtwork} from '../../app/components/launcher/book-artwork';
import {PRODUCT_GAMES} from '../../src/contracts/product-catalog.mts';
import '../../app/ui/foundation.css';
import '../../app/components/launcher/book-ui.css';
import '../../app/components/launcher/book-surfaces.css';
import '../../app/components/launcher/lobby-options.css';
import '../../app/components/room/room-presentation.css';
import '../../app/ui/system.css';
import '../../app/ui/tailwind.css';

// Actual directory owner, room rows and form. Only the manifest and transport
// are in memory; no relay request, membership or game launch can occur.
const games=['th06','th07','th09','th11'];
const manifest={schema:'eagler-touhou/site-manifest/1',protocol:'eagler-touhou/1',profile:'visual-directory',
  shared:{resourceMode:'hosted',testBuild:true,vanillaFont:'font.ttf',unicodeFont:'font.otf',netplayRelay:'wss://visual.invalid/netplay'},
  games:Object.fromEntries(games.map(id=>[id,{runtime:`runtime/${id}.html`,multiplayerRuntime:`runtime/${id}-mp.html`,
    gameData:{path:PRODUCT_GAMES[id].package.dataTarget.slice(1),bytes:1,sha256:'a'.repeat(64),version:`sha256-${'a'.repeat(64)}`,layout:`sha256-${'b'.repeat(64)}`},
    music:{midi:{files:[]}}}]))};
const rooms=[
  {product:'th06mp',code:'1234',capacity:3,players:2,ready:1,difficulty:1,spectators:1,phase:'lobby',joinable:true,
    seats:[{name:'界面示例',initial:'界',online:true,ready:true,loadout:0,controlMode:'normal'},{name:'示例玩家',initial:'示',online:true,ready:false,loadout:2,controlMode:'touch'},null],challengeMode:false,disableCheatMovement:true},
  {product:'th07mp',code:'2507',capacity:2,players:2,ready:2,difficulty:2,spectators:2,phase:'playing',joinable:false,
    seats:[{initial:'妖',online:true,ready:true,loadout:0,controlMode:'normal'},{initial:'梦',online:true,ready:true,loadout:1,controlMode:'normal'}],challengeMode:true,disableCheatMovement:true},
  {product:'th09mp',code:'9030',capacity:2,players:1,ready:0,difficulty:1,spectators:0,phase:'lobby',joinable:true,
    seats:[{initial:'花',online:true,ready:false,loadout:0,controlMode:'normal'},null],challengeMode:false,disableCheatMovement:false},
];
class Socket extends EventTarget{
  readyState=1;send(){}close(){this.readyState=3;}
  receive(value){this.dispatchEvent(new MessageEvent('message',{data:JSON.stringify(value)}));}
}
function createFixture(){
  let socket,empty=false;
  const service=createLobbyDirectory({selectedProduct:'',loadSiteManifest:async()=>manifest,createSocket:()=>socket=new Socket(),
    memberId:()=> 'visual-member',identity:{lobbyClientId:()=> 'visual-client'},sessions:{save(){},clear(){}},
    sessionStorage:{getItem:()=>null,removeItem(){}},translate:(key,params)=>translate('zh-CN',key,params),
    launcherUrl:()=>new URL('/',location.href).href,navigateToRoom(){},randomWord:()=>23});
  const receive=()=>socket?.receive({type:'directory',version:1,product:service.getSnapshot().selectedProduct,rooms:empty?[]:rooms,total:empty?0:rooms.length});
  return {service,async start(){await service.initialize();receive();},select(id){service.selectProduct(id);receive();},
    toggleEmpty(){empty=!empty;receive();},dispose(){service.dispose();}};
}
function Scenario(){
  const [model]=useState(createFixture),[theme,setTheme]=useState('light'),[formOpen,setFormOpen]=useState(false);
  const state=useSyncExternalStore(model.service.subscribe,model.service.getSnapshot,model.service.getSnapshot);
  const assetUrl=path=>'/'+path.replace(/^\//,'');
  const products=createLibraryProducts(state.products,assetUrl,id=>`/?game=${id}`).map(product=>({...product,artwork:bookArtwork(product)}));
  useEffect(()=>{document.body.classList.add('lobby-page','less-motion');void model.start();return()=>{model.dispose();document.body.classList.remove('lobby-page');};},[model]);
  useEffect(()=>{document.documentElement.dataset.bookTheme=theme;document.documentElement.className=theme;},[theme]);
  return <LocaleProvider locale="zh-CN"><MemoryRouter><header className="book-masthead" hidden aria-hidden="true"/>
    <LobbyDirectoryView service={model.service} assetUrl={assetUrl} masthead={null} products={products} interactive
      onSelect={model.select} onActivate={model.select} onGuide={null} onNetwork={null}
      onOpenForm={mode=>{if(model.service.prepareForm(mode))setFormOpen(true);}}/>
    <DirectoryRoomDialog service={model.service} open={formOpen} onCloseRequest={()=>setFormOpen(false)}/>
    <aside className="directory-specimen-note">本地房间数据 | 无真实联机 <button onClick={()=>setTheme(theme==='light'?'dark':'light')}>{theme==='light'?'深色':'亮色'}</button><button onClick={model.toggleEmpty}>切换空列表</button></aside>
  </MemoryRouter></LocaleProvider>;
}
const style=document.createElement('style');style.textContent='.directory-specimen-note{position:fixed;z-index:1000;bottom:0;left:0;right:0;min-height:32px;padding:5px 12px;background:var(--book-surface);color:var(--book-ink);font:11px sans-serif}.directory-specimen-note button{margin-left:12px}body.lobby-page .lobby-main{padding-bottom:56px}';document.head.append(style);
createRoot(document.getElementById('root')).render(<Scenario/>);
