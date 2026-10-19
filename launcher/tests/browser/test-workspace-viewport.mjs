// Production room/replay components with explicit room and file-list fixtures.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile, mkdir} from 'node:fs/promises';
import {resolve, extname, sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {authoredSourcesPlugin} from '../../lib/authored-source-resolution.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const out = resolve(project, '.cache/workspace-viewport');
await mkdir(out, {recursive: true});
const entry = `
import React,{useState} from 'react';import{createRoot}from'react-dom/client';
import{RoomView}from'./app/components/room/RoomView.tsx';
import{RoomSettingsDrawer}from'./app/components/room/RoomSettingsDrawer.tsx';
import{OptionsPanel}from'./app/components/launcher/OptionsPanel.tsx';
import{SettingsBody}from'./app/components/settings/SettingsBody.tsx';
import{createGameSettingsModel}from'./app/models/game-settings.ts';
import{createLibraryProducts}from'./app/components/launcher/products.ts';
import{ReplayWorkspace}from'./app/components/player/ReplayWorkspace.tsx';
import{LocaleProvider}from'./app/i18n.tsx';
import{PlayerSurface}from'./app/components/player/PlayerSurface.tsx';
import{TouchControlPreview}from'./app/components/settings/TouchControlPreview.tsx';
import{DEFAULT_GAME_OPTIONS}from'./src/launcher/game-preferences.mts';
import{applySavedPlayerTouchLayout}from'./app/services/player-input.ts';
import{touchLayoutControlNames}from'./src/launcher/touch-layout-model.mts';
import{QUICK_CHAT_ROWS}from'./src/contracts/multiplayer-quick-chat.mts';
const noop=()=>{},subscribe=()=>noop;
const params=new URLSearchParams(location.search),replay=params.has('replay');
window.roomPanelIntents=[];
window.applyTouchFixture=()=>{const player=document.querySelector('#player');player.classList.add('touch-enabled','touch-joystick-enabled');applySavedPlayerTouchLayout(player,document.querySelector('#touchLayoutSafeZone'),{viewport:{x:0},controls:Object.fromEntries(touchLayoutControlNames.map((name,index)=>[name,{x:index%2,y:index%2,scale:3,priority:index}]))});};
let state={product:'th06mp',connected:true,localClientId:'alice',displayName:'灵梦',seat:0,ready:false,timingChoice:'auto',rollbackEnabled:false,preferredLoadout:0,canStart:false,preparation:{status:'ready',percent:100},room:{code:'12345678',synced:true,playerCount:2,difficulty:1,phase:'lobby',inputDelay:0,startSerial:0,seats:[{clientId:'alice',name:'灵梦',loadout:0,ready:false,controlMode:'normal',resource:{status:'ready',percent:100}},{clientId:'bob',name:'魔理沙',loadout:2,ready:true,controlMode:'touch',resource:{status:'ready',percent:100}}],spectators:Array.from({length:Math.max(0,Math.min(30,Number(params.get('members'))||0))},(_,i)=>({clientId:'s'+i,name:'观众'+i}))}};
if(params.get('players')==='3'){state={...state,room:{...state.room,playerCount:3,seats:[...state.room.seats,params.has('empty-seat')?null:{clientId:'carol',name:'咲夜',loadout:1,ready:true,controlMode:'normal',resource:{status:'ready',percent:100}}]}};}
if(params.has('alone'))state.room.seats=state.room.seats.map((seat,index)=>index?null:seat);
if(params.has('empty-room'))state={...state,seat:null,room:{...state.room,seats:state.room.seats.map(()=>null)}};
if(params.has('spectator'))state={...state,seat:null,spectatorRequested:true,room:{...state.room,seats:state.room.seats.map(seat=>seat?.clientId===state.localClientId?null:seat),spectators:[...state.room.spectators,{clientId:'alice',name:'灵梦'}]}};
if(params.has('long-name'))state.room.seats=state.room.seats.map(seat=>seat?{...seat,name:'一位名字很长的幻想乡玩家'}:null);
const roomListeners=new Set();
const publishRoom=()=>{for(const fn of roomListeners)fn();};
function moveToSpectators(clientId){const member=state.room.seats.find(seat=>seat?.clientId===clientId)??(clientId===state.localClientId?{name:state.displayName}:null);if(!member)return;state={...state,...(clientId===state.localClientId?{seat:null,ready:false,spectatorRequested:true}:{}),room:{...state.room,seats:state.room.seats.map(seat=>seat?.clientId===clientId?null:seat),spectators:[...state.room.spectators,{clientId,name:member.name}]}};publishRoom();}
const service=new Proxy({subscribe:fn=>{roomListeners.add(fn);return()=>roomListeners.delete(fn);},getSnapshot:()=>state,timingRecommendation:()=>({inputDelay:0}),setRoomSettings:patch=>{state={...state,ready:false,room:{...state.room,...patch,seats:Array.from({length:patch.playerCount??state.room.playerCount},(_,index)=>state.room.seats[index]?{...state.room.seats[index],ready:false}:null)}};publishRoom();},setTimingChoice:choice=>{state={...state,timingChoice:choice,room:{...state.room,inputDelay:typeof choice==='number'?choice:0}};publishRoom();},setRollbackEnabled:enabled=>{state={...state,rollbackEnabled:enabled};publishRoom();},removePlayer:index=>{state={...state,room:{...state.room,seats:state.room.seats.map((seat,i)=>i===index?null:seat)}};publishRoom();},changeLoadout:delta=>{const loadout=(state.preferredLoadout+delta+4)%4;state={...state,preferredLoadout:loadout,room:{...state.room,seats:state.room.seats.map((seat,index)=>index===state.seat?{...seat,loadout}:seat)}};publishRoom();},toggleReady:()=>{state={...state,ready:!state.ready,room:{...state.room,seats:state.room.seats.map((seat,index)=>index===state.seat?{...seat,ready:!state.ready}:seat)}};publishRoom();},spectate:()=>moveToSpectators(state.localClientId),movePlayerToSpectator:index=>moveToSpectators(state.room.seats[index]?.clientId),leaveSpectator:()=>{state={...state,spectatorRequested:false,room:{...state.room,spectators:state.room.spectators.filter(member=>member.clientId!==state.localClientId)}};publishRoom();},takeSeat:index=>{if(state.room.seats[index])return;state={...state,seat:index,ready:false,spectatorRequested:false,room:{...state.room,seats:state.room.seats.map((seat,i)=>i===index?{clientId:'alice',name:'灵梦',loadout:state.preferredLoadout,ready:false,controlMode:'normal',resource:{status:'ready',percent:100}}:seat?.clientId==='alice'?null:seat),spectators:state.room.spectators.filter(member=>member.clientId!==state.localClientId)}};publishRoom();}},{get:(o,k)=>o[k]??noop});
const network={subscribe,getSnapshot:()=>0,metric:()=>({state:'connected',rtt:38,jitter:2}),capabilities:()=>({supported:true,rtcAvailable:true,turnConfigured:true}),retry:noop};
let chatState={context:{connected:true,localSeat:0},entries:[],muted:[]};const chatListeners=new Set();const quickChat={subscribe:fn=>{chatListeners.add(fn);return()=>chatListeners.delete(fn);},getSnapshot:()=>chatState,sendPhrase:id=>{chatState={...chatState,entries:[...chatState.entries,{id:chatState.entries.length+1,clientId:'alice',seat:0,spectator:false,name:'灵梦',phrase:QUICK_CHAT_ROWS.flat().find(phrase=>phrase.id===id)}]};for(const fn of chatListeners)fn();}};
const files={open:true,phase:'ready',productId:params.has('versus')?'th09mp':'th06',rows:Array.from({length:8},(_,i)=>({path:'replay/th_0'+i+'.rpy',name:'th_0'+i+'.rpy',size:2048,modifiedAt:1792368000000})),descriptions:{}};
const model={subscribe,getSnapshot:()=>files,close:noop};const app={locale:'zh-CN'};
const session={baseUrl:location.origin+'/',subscribe,getSnapshot:()=>app,getReplays:()=>model,nativeTouch:{isFullscreen:()=>false},togglePlayerFullscreen:noop};
const settingsModel=createGameSettingsModel({storage:null});
settingsModel.hydrate({productId:'th06mp',uiLocale:'zh-CN',languages:[{id:'ja',title:'日本語'},{id:'zh-CN',title:'简体中文'}],musicAvailability:{audio:true,midiAvailable:false,importServer:false,remoteOggAdvertised:false},webMidiAvailable:false,mobile:innerWidth<=760});
const settingsProduct={...createLibraryProducts(['th06mp'],path=>'/'+path,id=>'/?game='+id)[0],artwork:null};
const midi={enabled:false,granted:false,busy:false,supported:false,outputs:[],selectedId:'',hint:''};
const settingsActions={confirm:async()=>false,file:async()=>{},showAppleNotice:noop,feedback:noop,reportError:error=>{throw error;},externalMidi:{subscribe,getSnapshot:()=>midi,setEnabled:async()=>{},selectOutput:async()=>{}}};
function App(){
 const[host,setHost]=useState(null),[panel,setPanel]=useState(null),[settingsOpen,setSettingsOpen]=useState(false),touch=params.has('touch');
 const roomVisible=!replay&&!touch,closeSettings=()=>setSettingsOpen(false);
 return <LocaleProvider locale="zh-CN">
  <PlayerSurface open roomWorkspace={roomVisible} replayWorkspace={replay} editing={false} onElement={setHost}>
   {replay&&<ReplayWorkspace session={session}/>}<div id="touchLayoutSafeZone" className="touch-layout-safe-zone"/>
   <TouchControlPreview settings={{gameId:'th06',options:{...DEFAULT_GAME_OPTIONS,touchEnabled:true,touchMovementMode:'joystick'}}}/>
  </PlayerSurface>
  {roomVisible&&host&&<RoomView service={service} quickChat={quickChat} network={network} context={{launched:false,previewReady:false,th09NetworkOverlayOpen:false,launchStage:null,touchEnabled:false,touchMovementMode:'joystick'}} assetUrl={p=>'/'+p} workspaceHost={host} workspaceStage={host.querySelector('.player-stage')} onLeave={noop} onCopyRoomCode={noop} onGuide={noop} onOpenSettings={()=>setSettingsOpen(true)} settingsOpen={settingsOpen} panel={panel} onOpenPanel={(kind,peerId)=>{window.roomPanelIntents.push([kind,peerId]);setPanel({kind,peerId});}} onClosePanel={()=>setPanel(null)}/>}
  {roomVisible&&<><RoomSettingsDrawer roomOpen open={settingsOpen} onCloseRequest={closeSettings}/>
   <div hidden><OptionsPanel product={settingsProduct} open={false} roomDrawer onBack={closeSettings} assetUrl={p=>'/'+p}>
    <SettingsBody model={settingsModel} actions={settingsActions} onOpenTouchLayout={noop}/>
   </OptionsPanel></div>
  </>}
 </LocaleProvider>;
}
createRoot(document.getElementById('root')).render(<App/>);
`;
const result = await build({absWorkingDir: project, stdin: {contents: entry, loader: 'tsx', resolveDir: project}, bundle: true, format: 'esm', jsx: 'automatic', write: false, outdir: out, external: ['/assets/*'], plugins: [
  {name: 'fixture-navigation', setup(ctx) {
    ctx.onResolve({filter: /navigation\/surface-navigation$/}, () => ({path: 'navigation', namespace: 'fixture'}));
    ctx.onLoad({filter: /.*/, namespace: 'fixture'}, () => ({contents: 'const navigation={infoDialogOpen:()=>true}; export const useSurfaceNavigation=()=>navigation;'}));
  }}, authoredSourcesPlugin(project),
]});
const js = result.outputFiles.find(file => file.path.endsWith('.js')).text;
const css = result.outputFiles.find(file => file.path.endsWith('.css')).text;
const publicRoot = resolve(project, 'public');
const server = createServer(async (req, res) => {
  try {
    const path = new URL(req.url, 'http://localhost').pathname;
    if (path === '/') {res.setHeader('content-type', 'text/html; charset=utf-8'); res.end('<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><link rel="stylesheet" href="/styles.css"><link rel="stylesheet" href="/features.css"><link rel="stylesheet" href="/ui-fonts.css"><link rel="stylesheet" href="/ui-fonts-deferred.css"><link rel="stylesheet" href="/fixture.css"><div id="root"></div><script type="module" src="/fixture.js"></script>'); return;}
    if (path === '/fixture.js' || path === '/fixture.css') {res.setHeader('content-type', path.endsWith('js') ? 'text/javascript' : 'text/css'); res.end(path.endsWith('js') ? js : css); return;}
    const file = resolve(publicRoot, '.' + path);
    if (!file.startsWith(publicRoot + sep)) throw Error('asset path');
    res.setHeader('content-type', ({'.css': 'text/css', '.svg': 'image/svg+xml', '.woff2': 'font/woff2'})[extname(path)] || 'application/octet-stream');
    res.end(await readFile(file));
  } catch {res.writeHead(404).end();}
});
const previewPort = Number(process.env.WORKSPACE_VIEWPORT_PREVIEW_PORT) || 0;
await new Promise(done => server.listen(previewPort, '127.0.0.1', done));
let browser;
if (process.env.WORKSPACE_VIEWPORT_PREVIEW_ONLY === '1') {
  console.log(`Preview http://127.0.0.1:${server.address().port}`);
} else {
try {
  browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable()});
  const page = await browser.newPage(), errors = [];
  await page.emulateMediaFeatures([{name: 'prefers-reduced-motion', value: 'reduce'}]);
  page.on('pageerror', error => errors.push(error.message));
  const origin = `http://127.0.0.1:${server.address().port}`;
  const closePanel = async()=>{await page.click('#mpRoomPanelClose');await page.waitForSelector('#mpRoomPanel:not([open])');};
  const openSpectators = async()=>{await page.$eval('.room-spectator-preview-trigger',node=>node.scrollIntoView({block:'center'}));await page.click('.room-spectator-preview-trigger');await page.waitForSelector('#mpRoomPanel[open][data-panel="spectators"]');};
  for (const mode of ['room', 'replay', 'replay-versus']) {
    for (const [label, width, height] of [['desktop',1440,900],['mobile',390,844],['landscape',844,390],['short-desktop',1024,600],['tall-desktop',1440,1200]]) {
      await page.setViewport({width, height});
      await page.goto(origin + (mode === 'room' ? '/' : '/?replay=1' + (mode === 'replay-versus' ? '&versus=1' : '')));
      await page.waitForSelector(mode === 'room' ? '#mpRoomTitle' : '.replay-file-browser');
      await page.evaluate(() => document.fonts.ready);
      await page.waitForFunction(() => {
        const node = document.querySelector('#gameViewport'), box = node.getBoundingClientRect();
        if (!node.style.getPropertyValue('--workspace-game-height') || box.height <= 0) return false;
        return node.hasAttribute('data-workspace-stacked') ? Math.abs(box.width / box.height - 4/3) < .01 : box.height <= box.width * .75 + 1;
      });
      const geometry = await page.evaluate(() => {
        const box = selector => {const r=document.querySelector(selector).getBoundingClientRect();return {x:r.x,y:r.y,width:r.width,height:r.height,right:r.right,bottom:r.bottom};};
        const stage=document.querySelector('.player-stage'),style=getComputedStyle(stage);
        return {game:box('#gameViewport'),stage:box('.player-stage'),contentWidth:stage.clientWidth-parseFloat(style.paddingLeft)-parseFloat(style.paddingRight),spectators:document.querySelector('.room-workspace-spectators')?box('.room-workspace-spectators'):null};
      });
      assert(geometry.game.width > 150, `${mode}/${label}: usable image`);
      assert(geometry.game.right <= width + 1, `${mode}/${label}: fits viewport width`);
      assert(geometry.game.bottom <= geometry.stage.bottom + 1, `${mode}/${label}: fits stage`);
      assert(Math.abs(geometry.game.width - geometry.contentWidth) < 2, `${mode}/${label}: uses full content width`);
      if (label === 'desktop') assert(geometry.game.width / geometry.game.height > 4/3 + .01, `${mode}: desktop fills available space without fixing aspect ratio`);
      if (mode !== 'room' && width > 760 && width > height) {
        const top = await page.$eval('.replay-file-browser', node => node.getBoundingClientRect().top);
        assert(Math.abs(top - geometry.stage.y) < 1, `${mode}/${label}: file list aligns with player panel`);
      }
      await page.$eval('#player', node => node.classList.add('touch-enabled','touch-joystick-enabled'));
      assert(await page.$$eval('[data-touch-layout-control]', nodes => nodes.every(node => !node.getClientRects().length)), `${mode}/${label}: gameplay buttons cannot spill into panels`);
      if (mode === 'room' && width <= 760) assert(geometry.spectators.height <= 180, 'compact spectator rail beside two players');
      await page.screenshot({path: resolve(out, `${mode}-${label}.png`), fullPage: true});
      if (mode === 'room') {
        const cards = await page.$$eval('.mp-seat.occupied', nodes => nodes.map(node => {
          const card=node.getBoundingClientRect(),identity=node.querySelector('.mp-seat-identity').getBoundingClientRect(),avatar=node.querySelector('.mp-seat-face').getBoundingClientRect(),style=getComputedStyle(node);
          return {center:Math.abs((card.top+card.bottom-identity.top-identity.bottom)/2),avatarTop:Math.abs(avatar.top-card.top-parseFloat(style.paddingTop)-parseFloat(style.borderTopWidth)),seatAtAvatar:!!node.querySelector('.mp-seat-face .room-seat-player-tag'),networkAtIdentity:!!node.querySelector('.mp-seat-identity .mp-seat-latency')};
        }));
        assert(cards.every(card => card.center < 1 && card.avatarTop < 1 && card.seatAtAvatar && card.networkAtIdentity), `${label}: centered text, top-aligned avatar, avatar seat label and grouped connection info`);
        await page.click('[data-mp-seat="1"] .mp-seat-latency');
        assert.deepEqual(await page.evaluate(() => window.roomPanelIntents.at(-1)), ['network','bob'], 'network button remains interactive above the decorative background');
        await closePanel();
        await page.click('[data-mp-seat="0"] .mp-seat-name');
        assert.deepEqual(await page.evaluate(() => window.roomPanelIntents.at(-1)), ['personal',null], 'foreground identity retains the player-options hit area');
        await closePanel();
        await page.$eval('.room-quick-chat-trigger',node=>node.scrollIntoView({block:'center'}));
        if(width<height)assert(await page.evaluate(()=>document.querySelector('.room-quick-chat-trigger').getBoundingClientRect().bottom<=document.querySelector('.mp-room-chat-log').getBoundingClientRect().top),'portrait quick chat sits above the messages');
        await page.click('.room-quick-chat-trigger');
        await page.waitForSelector('.room-quick-chat-picker');
        const picker = await page.$eval('.room-quick-chat-picker', node => {const r=node.getBoundingClientRect();return {left:r.left,top:r.top,right:r.right,bottom:r.bottom,clipped:!!node.closest('.room-workspace-panels')};});
        assert(!picker.clipped && picker.left >= 0 && picker.top >= 0 && picker.right <= width + 1 && picker.bottom <= height + 1, `${label}: picker escapes the clipping panel and fits the viewport`);
        await page.click('.room-quick-chat-picker [data-phrase="thanks"]');
        await page.waitForSelector('.room-quick-chat-picker', {hidden:true});
        assert(await page.$eval('.mp-room-chat-log', node => node.textContent.includes('谢谢指教')), 'phrase reaches room messages');
      }
    }
  }
  for (const width of [320,390,760]) {
    await page.setViewport({width,height:844});
    await page.goto(origin+'/?players=3');
    await page.waitForSelector('[data-player-count="3"]');
    await page.evaluate(() => document.fonts.ready);
    const rows = await page.$$eval('.mp-seat-stage[data-player-count="3"] .mp-seat', nodes => nodes.map(node=>{const r=node.getBoundingClientRect();return {left:r.left,right:r.right,top:r.top,bottom:r.bottom,height:r.height};}));
    assert.equal(rows.length,3);
    assert(rows.every((row,index)=>row.left>=0&&row.right<=width&&row.height<=90&&Math.abs(row.left-rows[0].left)<1&&(!index||row.top>=rows[index-1].bottom)), `${width}: three original cards stack beside the spectator rail`);
    const rail = await page.$eval('.room-workspace-spectators',node=>{const r=node.getBoundingClientRect();return {left:r.left,right:r.right,top:r.top};});
    assert(rail.left>=rows[2].right&&rail.right<=width&&Math.abs(rail.top-rows[0].top)<1,'spectator avatars sit to the right of the three cards');
    assert(await page.$eval('#player',node=>node.scrollWidth<=node.clientWidth), 'three-player room has no horizontal overflow');
    await page.screenshot({path:resolve(out,`room-three-${width}.png`),fullPage:true});
    await page.$eval('[data-mp-seat="2"] .mp-seat-latency',node=>node.scrollIntoView({block:'center'}));
    await page.click('[data-mp-seat="2"] .mp-seat-latency');
    await page.waitForFunction(()=>window.roomPanelIntents.at(-1)?.[1]==='carol');
    assert.deepEqual(await page.evaluate(()=>window.roomPanelIntents.at(-1)),['network','carol']);
  }
  for (const width of [320,390]) {
    await page.setViewport({width,height:844});
    await page.goto(origin+'/?players=3&empty-seat=1&long-name=1');
    await page.waitForSelector('[data-player-count="3"]');
    await page.evaluate(() => document.fonts.ready);
    assert(await page.$eval('[data-mp-seat="2"] .mp-seat-drop button',node=>!node.disabled&&node.getBoundingClientRect().width>=64),'third seat can still be joined');
    assert(await page.$$eval('.mp-seat.occupied .mp-seat-name',nodes=>nodes.every(node=>{const r=node.getBoundingClientRect(),card=node.closest('.mp-seat').getBoundingClientRect();return r.width>0&&r.right<=card.right&&getComputedStyle(node).textOverflow==='ellipsis';})),'long names fit their cards with truncation available');
    if(width===320)assert(await page.$$eval('.mp-seat.occupied .mp-seat-name',nodes=>nodes.every(node=>node.scrollWidth>node.clientWidth)),'narrow phones truncate long names');
    assert(await page.$eval('#player',node=>node.scrollWidth<=node.clientWidth),'long names and empty seats stay inside the phone');
    await page.screenshot({path:resolve(out,`room-three-empty-${width}.png`),fullPage:true});
  }
  for (const width of [320,390,1440]) {
    await page.setViewport({width,height:width===1440?900:844});
    for (const count of [2,3]) {
      const query='/?players='+count;
      await page.goto(origin+query+'&alone=1');
      await page.waitForSelector('#mpRoomTitle');
      await page.evaluate(()=>document.fonts.ready);
      assert.equal(await page.$$eval('.mp-seat[data-card-state="empty"]:not([hidden])',nodes=>nodes.length),count-1,'vacant positions remain available at the configured player count');
      const emptyTargets=await page.$$eval('.mp-seat[data-card-state="empty"]:not([hidden])',nodes=>nodes.map(card=>{const box=card.getBoundingClientRect(),button=card.querySelector('.mp-seat-drop button').getBoundingClientRect();return {cardWidth:box.width,cardHeight:box.height,width:button.width,height:button.height,right:button.right-box.right,bottom:button.bottom-box.bottom};}));
      assert(emptyTargets.every(target=>target.width>=target.cardWidth-3&&target.height>=target.cardHeight-3&&target.right<=1&&target.bottom<=1),'the whole empty card is a contained join target: '+JSON.stringify(emptyTargets));
      await page.screenshot({path:resolve(out,`room-alone-${count}-${width}.png`),fullPage:true});
      await page.goto(origin+query);
      await page.waitForSelector('#mpSpectatorPreviewJoin');
      await page.click('#mpSpectatorPreviewJoin');
      await page.waitForSelector('.mp-spectator-entry.mine');
      assert(await page.$eval('.room-workspace-actions',node=>node.hidden),'ready and start actions hide after spectating');
      assert.equal(await page.$eval('[data-mp-seat="0"]',node=>node.dataset.cardState),'empty','the former seat becomes a join target');
      assert(await page.$eval('.room-preparation-caption strong',node=>node.textContent.includes('已进入旁观席')),'spectators receive a role-specific caption');
      assert(await page.$eval('.mp-spectator-entry.mine .mp-spectator-marker',node=>node.textContent.includes('我')),'the spectator strip marks the local member');
      if(width<=760)assert.equal(await page.$eval('#player',node=>getComputedStyle(node).paddingBottom),'12px','spectators reclaim the fixed action area');
      await page.evaluate(()=>document.fonts.ready);
      assert(await page.$('.room-spectator-preview-avatar.mine'),'local spectator appears in the avatar preview');
      assert.equal(await page.$eval('.room-spectator-preview-avatar.mine',node=>node.getBoundingClientRect().width),40,'a sparse spectator preview uses larger avatars');
      assert(await page.$eval('#mpSpectatorPreviewJoin',node=>node.textContent.includes('离开旁观')),'the preview retains the leave action');
      await page.screenshot({path:resolve(out,`room-spectator-${count}-${width}.png`),fullPage:true});
      await page.$eval('[data-mp-seat="0"] .mp-seat-drop button',node=>node.scrollIntoView({block:'center'}));
      await page.click('[data-mp-seat="0"] .mp-seat-drop button');
      await page.waitForSelector('[data-mp-seat="0"].occupied');
      assert(await page.$eval('.room-workspace-actions',node=>!node.hidden),'taking a seat restores player actions');
      assert.equal(await page.$$('.mp-spectator-entry.mine').then(nodes=>nodes.length),0,'taking a seat removes the local spectator entry');
      await page.click('[data-mp-seat="1"] .room-member-menu-trigger');
      await page.click('.room-member-menu [data-phrase="move-spectator"]');
      await page.waitForSelector('[data-mp-seat="1"][data-card-state="empty"]');
      assert.equal(await page.$eval('#mpSpectatorCount',node=>node.textContent),'1','moving a peer replaces their card with a spectator entry');
      assert(await page.$eval('.room-workspace-actions',node=>!node.hidden),'moving another player keeps the local player actions');
      await page.goto(origin+query+'&empty-room=1');
      await page.waitForSelector('#mpRoomTitle');
      assert.equal(await page.$$eval('.mp-seat[data-card-state="empty"]:not([hidden])',nodes=>nodes.length),count,'a room without seated players retains every join target');
      assert(await page.$eval('.room-preparation-caption strong',node=>node.textContent.includes('选择 P 位')),'unseated members receive the seat selection caption');
      await openSpectators();
      await page.click('#mpSpectatorJoin');
      await page.waitForSelector('.mp-spectator-entry.mine');
      await page.evaluate(()=>document.fonts.ready);
      await closePanel();
      await page.screenshot({path:resolve(out,`room-empty-spectator-${count}-${width}.png`),fullPage:true});
      await openSpectators();
      await page.click('#mpSpectatorJoin');
      await page.waitForSelector('.mp-spectator-entry.mine',{hidden:true});
      assert(await page.$eval('#mpSpectatorJoin',node=>node.textContent.includes('加入旁观')),'leaving spectating returns the join action');
      await closePanel();
      assert(await page.$eval('.room-workspace-actions',node=>node.hidden),'unseated members keep player actions hidden');
    }
  }
  for (const count of [1,2,8]) {
    await page.setViewport({width:320,height:844});
    await page.goto(origin+'/?players=3&members='+count);
    await page.waitForSelector('.room-spectator-preview-avatar');
    const dimensions=await page.$$eval('.room-spectator-preview-avatar',nodes=>nodes.map(node=>node.getBoundingClientRect().width));
    assert(dimensions.every(width=>width===(count<=2?40:28)),'avatar density follows spectator count');
    const footer=await page.$eval('#mpSpectatorPreviewJoin',node=>{const r=node.getBoundingClientRect(),rail=node.closest('.room-workspace-spectators').getBoundingClientRect();return {bottom:r.bottom,railBottom:rail.bottom};});
    assert(Math.abs(footer.railBottom-footer.bottom)<=10,'the join action stays at the bottom of the spectator rail');
    assert(await page.$eval('#player',node=>node.scrollWidth<=node.clientWidth),'sparse and dense avatar previews fit narrow phones');
  }
  for (const [width,height] of [[390,844],[1440,900]]) {
    await page.setViewport({width,height});
    await page.goto(origin);
    await page.waitForSelector('#mpSettingsRoomDrawerToggle');
    await page.click('#mpSettingsRoomDrawerToggle');
    await page.waitForSelector('#mpSettingsRoomDrawer:not([hidden])');
    assert(await page.$('#mpSettingsRoomDrawer .mp-settings-body'),'settings opens the shared controls');
    const sharing=await page.$eval('#mpShareSettingsToggle',node=>node.getAttribute('aria-checked'));
    await page.click('#mpShareSettingsToggle');
    assert.notEqual(await page.$eval('#mpShareSettingsToggle',node=>node.getAttribute('aria-checked')),sharing,'preview settings update through the real model');
    await page.screenshot({path:resolve(out,`room-settings-drawer-${width}.png`),fullPage:true});
    await page.click('#libraryBack');
    await page.waitForSelector('#mpSettingsRoomDrawer[hidden]');
    await page.click('#roomWorkspaceTab-game');
    await page.$eval('#mpInputDelaySetting',node=>node.scrollIntoView({block:'center'}));
    const gap=await page.$eval('#mpInputDelaySetting',node=>node.getBoundingClientRect().top-node.closest('.room-settings-group').querySelector('h3').getBoundingClientRect().bottom);
    assert(gap>=0&&gap<=24,'input delay follows its heading without an empty control row');
    assert.equal(await page.$eval('#mpRoomCheatHint',node=>node.textContent),'禁用手机默认使用的无限速触控移动');
    assert.equal(await page.$eval('#mpRoomChallengeHint',node=>node.textContent),'无法使用 Bomb，原先的残机个数改为死亡次数');
    assert.equal(await page.$('#mpRoomSettingsHint'),null,'the local host paragraph is removed');
    await page.select('#mpRoomVisibility','private');
    assert.equal(await page.$eval('#mpRoomVisibility',node=>node.value),'private','room setting controls update preview state');
    assert(await page.$eval('#mpInputDelayPlus',node=>node.disabled),'the preview follows th06 timing capabilities');
    await page.screenshot({path:resolve(out,`room-game-settings-${width}.png`),fullPage:true});
  }
  await page.setViewport({width:390,height:844});
  await page.goto(origin+'/?members=8');
  await page.waitForSelector('.room-spectator-preview-avatar');
  assert(await page.$eval('.room-workspace-spectators', node => node.getBoundingClientRect().height <= 180), 'populated spectators stay compact');
  assert(await page.$eval('.room-spectator-preview-avatars', node => node.scrollHeight > node.clientHeight), 'avatar preview scrolls inside the rail');
  assert.equal(await page.$$('.room-workspace-spectators .mp-spectator-copy').then(nodes=>nodes.length),0,'the preview contains avatars without member names');
  await openSpectators();
  assert.equal(await page.$$('#mpRoomPanel .mp-spectator-entry').then(nodes=>nodes.length),8,'the spectator panel contains the complete member list');
  await page.screenshot({path:resolve(out,'room-spectators-panel-mobile.png'),fullPage:true});
  await page.keyboard.press('Escape');
  await page.waitForSelector('#mpRoomPanel:not([open])');
  assert(await page.$eval('.room-spectator-preview-trigger',node=>node===document.activeElement),'closing restores focus to the spectator trigger');
  await page.click('#roomWorkspaceTab-game');
  assert(await page.$eval('#roomWorkspacePanel-game', node => !node.hidden), 'room settings remain reachable');
  await page.setViewport({width:320,height:568});
  await page.goto(origin+'/?touch=1');
  await page.waitForSelector('#touchJoystick');
  await page.evaluate(() => window.applyTouchFixture());
  const overflow = await page.evaluate(() => {
    const safe=document.querySelector('#touchLayoutSafeZone').getBoundingClientRect();
    return [...document.querySelectorAll('[data-touch-layout-control]')].filter(node => node.getClientRects().length).filter(node => {const r=node.getBoundingClientRect();return r.left<safe.left-.5||r.top<safe.top-.5||r.right>safe.right+.5||r.bottom>safe.bottom+.5;}).map(node=>node.id);
  });
  assert.deepEqual(overflow, [], 'large saved controls fit a narrow non-fullscreen player');
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'no-preference'}]);
  await page.setViewport({width:390,height:844});
  await page.goto(origin);
  await page.waitForSelector('#mpReady');
  await page.evaluate(() => new Promise(done => setTimeout(done, 1000)));
  await page.click('[data-mp-seat="1"] .room-member-menu-trigger');
  await page.waitForSelector('.room-member-menu');
  assert(await page.$eval('.room-member-menu',node=>node.getAnimations().some(animation=>animation.animationName==='room-chat-picker-in')),'member menu animates on entry');
  const memberSurface=await page.$eval('.room-member-menu',node=>({background:getComputedStyle(node).backgroundColor,radius:getComputedStyle(node).borderRadius}));
  await page.screenshot({path:resolve(out,'room-member-menu-mobile.png'),fullPage:false});
  await page.keyboard.press('End');
  assert(await page.$eval('.room-member-menu [data-danger=true]',node=>node===document.activeElement),'member actions support keyboard navigation');
  await page.keyboard.press('Escape');
  assert(await page.$eval('.room-member-menu',node=>node.dataset.closing==='true'&&node.getAnimations().some(animation=>animation.animationName==='room-chat-picker-out')),'member menu animates on exit');
  await page.waitForSelector('.room-member-menu',{hidden:true});
  assert(await page.$eval('[data-mp-seat="1"] .room-member-menu-trigger',node=>node===document.activeElement),'member menu returns focus to its trigger');
  await page.$eval('.room-quick-chat-trigger',node=>node.scrollIntoView({block:'center'}));
  await page.click('.room-quick-chat-trigger');
  assert.deepEqual(await page.$eval('.room-quick-chat-picker',node=>({background:getComputedStyle(node).backgroundColor,radius:getComputedStyle(node).borderRadius})),memberSurface,'member and quick-chat menus share the same surface');
  assert(await page.$eval('.room-quick-chat-picker', node => node.getAnimations().some(animation => animation.animationName === 'room-chat-picker-in')), 'quick-chat picker animates');
  await page.keyboard.press('Escape');
  await page.waitForSelector('.room-quick-chat-picker', {hidden:true});
  await page.click('#roomWorkspaceTab-game');
  assert(await page.$eval('#roomWorkspacePanel-game', node => node.getAnimations().some(animation => animation.animationName === 'room-panel-enter')), 'room tab panel animates');
  await page.click('#roomWorkspaceTab-chat');
  await page.click('#mpReady');
  await page.waitForSelector('[data-mp-seat="0"][data-card-state="ready"]');
  assert(await page.$eval('[data-mp-seat="0"]', node => node.querySelector('.room-seat-watermark.active').textContent === 'READY'), 'ready state word matches room state');
  assert(await page.$eval('[data-mp-seat="0"]', node => node.getAnimations({subtree:true}).some(animation => animation.animationName === 'room-seat-word-arrive')), 'ready letters animate');
  await page.click('#mpReady');
  await page.waitForSelector('[data-mp-seat="0"][data-card-state="unready"]');
  await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
  await page.click('#mpReady');
  assert(await page.$eval('[data-mp-seat="0"]', node => node.getAnimations({subtree:true}).length === 0), 'reduced motion settles directly');
  assert.deepEqual(errors, []);
  console.log('PASS room/replay layouts, vacant seats, spectator transitions, touch bounds, ready state animation and reduced motion');
} finally {await browser?.close(); if (previewPort) console.log(`Preview http://127.0.0.1:${previewPort}`); else server.close();}
}
