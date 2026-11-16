import {DatPackageArchiveStore,installDatPackage,reopenDatPackage} from './runtime/dat-package-install.mjs';
import {installResourceFetchProvider} from './resource-fetch.mjs';
import {readOnlineDeployment,loadOnlineResources} from './runtime/online-resources.mjs';
import {mountLauncherSession} from './runtime/launcher-session.mjs';
import {bindManagedDatProvider} from './runtime/managed-dat-owner.mjs';
import {createReplayFileBridge} from './runtime/replay-file-bridge.mjs';
import {createPageLaunchContext,installPageLaunchContext} from './runtime/page-launch-context.mjs';
const main=document.querySelector('main');
if(main)main.hidden=true;
const loader=document.createElement('section');
loader.id='resource-loader';
loader.innerHTML='<h1>妖精大战争</h1><p data-hint>正在准备游戏…</p><progress max="1" value="0" aria-label="资源下载进度" hidden></progress><div class="resource-actions"><input type="file" accept=".zip,application/zip" aria-label="TH128 资源 ZIP" disabled><button type="button" disabled>开始游戏</button><button type="button" data-retry hidden>重新下载</button></div><p role="status">正在读取网站配置…</p>';
document.body.prepend(loader);
const file=loader.querySelector('input'),start=loader.querySelector('button'),status=loader.querySelector('[role="status"]'),
 retry=loader.querySelector('[data-retry]'),hint=loader.querySelector('[data-hint]'),meter=loader.querySelector('progress');
let store=null,storeError=null;
if(typeof globalThis.indexedDB?.open==='function'){
 try{store=new DatPackageArchiveStore();}catch(error){storeError=error;}
}
let busy=true,launched=false,launchAttempted=false,pageGone=false,provider=null,deployment=null,launcher=null,pageLaunch=null,managedOwner=null,replayFiles=null;
const controller=new AbortController(),isCurrent=()=>!pageGone&&!controller.signal.aborted;
function requireCurrent(){if(!isCurrent())throw new DOMException('Game launch page was closed','AbortError');}
function progress(value){
 if(!isCurrent())return;
 if(value.phase==='download'){
  meter.hidden=false;meter.max=value.total;meter.value=value.loaded;
  status.textContent='正在下载游戏资源：'+(value.loaded/1024/1024).toFixed(1)+' / '+(value.total/1024/1024).toFixed(1)+' MB';
 }else if(value.phase==='hash')status.textContent='正在校验完整资源包…';
 else if(value.phase==='cache')status.textContent='正在读取已缓存的游戏资源…';
 else if(value.verified)status.textContent='正在检查资源：'+value.verified+'/'+value.total;
 else if(value.completed)status.textContent='正在读取资源：'+value.completed+'/'+value.total;
}
function options(){return{store,origin:location.origin,pageURL:location.href,signal:controller.signal,isCurrent,onProgress:progress};}
async function ready(next){
 let nextOwner=null;
 if(next&&new URLSearchParams(location.search??'').has('managedData')){
  if(!launcher)throw new Error('Managed DATA needs an authenticated launcher session');
  nextOwner=await bindManagedDatProvider(next,{window:parentWindow,document,location,signal:controller.signal,isCurrent});
 }
 try{requireCurrent();}catch(error){nextOwner?.dispose();throw error;}
 managedOwner?.dispose();provider?.dispose();managedOwner=nextOwner;provider=next;start.disabled=!provider;
 status.textContent=provider?'资源已准备好，点击“开始游戏”。':'请选择 TH128 资源 ZIP。';
}
async function launch(config=null){
 if(launchAttempted){if(config)throw new Error('Runtime initialization was already attempted; reopen this page');return;}
 if(!provider||!isCurrent()){if(config)throw new DOMException('Runtime page owner exited','AbortError');return;}
 launchAttempted=true;busy=true;start.disabled=true;file.disabled=true;retry.hidden=true;status.textContent='正在进入游戏…';
 try{
  requireCurrent();installResourceFetchProvider(provider);
  if(config){pageLaunch=createPageLaunchContext({config,onPresented:actual=>launcher?.notePresented(actual)});installPageLaunchContext(pageLaunch);}
  await import('./play.mjs');requireCurrent();
  if(pageLaunch)await pageLaunch.waitRuntime();requireCurrent();
  launched=true;loader.remove();if(main)main.hidden=false;store?.close();
 }catch(error){if(isCurrent())status.textContent='启动失败，请重新打开页面：'+error.message;throw error;}
}
async function runOnline(){
 if(launched||launchAttempted||!isCurrent())return;
 busy=true;retry.hidden=true;retry.disabled=true;
 let next=null;
 try{
  const result=await loadOnlineResources(deployment,options());next=result.provider;requireCurrent();
  await ready(next);next=null;
  if(!result.cached)hint.textContent='资源已就绪；此浏览器未保存缓存，关闭后需要重新下载。';
  if(launcher)hostedReady();else await launch();
 }catch(error){
  next?.dispose();
  if(isCurrent()&&!launchAttempted){status.textContent='游戏资源加载失败：'+error.message;retry.hidden=false;retry.disabled=false;}
 }finally{if(!launchAttempted)busy=false;}
}
file.addEventListener('change',async()=>{
 if(busy||launchAttempted||!file.files?.[0]||deployment)return;
 const selected=file.files[0];busy=true;file.disabled=true;start.disabled=true;
 let next=null;
 try{
  if(selected.size>256*1024*1024)throw new Error('资源包过大，请选择 DAT + OGG 版本。');
  status.textContent='正在导入资源…';
  next=await installDatPackage(new Uint8Array(await selected.arrayBuffer()),options());requireCurrent();
  await ready(next);next=null;if(launcher)hostedReady();
 }catch(error){next?.dispose();if(isCurrent()){status.textContent='导入失败：'+error.message+(provider?'。已保留原资源。':'');start.disabled=!provider;}}
 finally{busy=false;if(isCurrent())file.disabled=false;}
});
start.addEventListener('click',()=>{if(!busy)return launch().catch(()=>{});});
retry.addEventListener('click',()=>{if(!busy)return deployment?runOnline():discover();});
function closeEntry(){
 pageGone=true;controller.abort();
 try{replayFiles?.dispose();}finally{try{managedOwner?.dispose();}finally{try{pageLaunch?.abort();}finally{try{provider?.dispose();}finally{store?.close();status.textContent='游戏已退出。';}}}}
}
addEventListener('pagehide',()=>{try{closeEntry();}finally{launcher?.dispose();}},{once:true});
let reloadAfterRestore=false;
addEventListener('pageshow',event=>{
 if(event.persisted===true&&pageGone&&!reloadAfterRestore){reloadAfterRestore=true;location.reload();}
});
async function discover(){
 if(launched||launchAttempted||!isCurrent())return;
 busy=true;file.disabled=true;start.disabled=true;retry.hidden=true;retry.disabled=true;
 let next=null;
 try{
  deployment=await readOnlineDeployment({pageURL:location.href,signal:controller.signal,isCurrent});requireCurrent();
  if(deployment){
   file.hidden=true;start.hidden=true;hint.textContent='首次打开自动下载游戏；以后优先读取缓存。';await runOnline();
  }else{
   hint.textContent='选择 TH128 资源包，导入后即可开始游戏。';meter.hidden=true;
   if(!store)throw storeError??new Error('浏览器未启用本地资源存储，请允许此页面使用存储后重试。');
   next=await reopenDatPackage(options());requireCurrent();await ready(next);next=null;file.disabled=false;if(launcher)hostedReady();
  }
 }catch(error){
  next?.dispose();
  if(isCurrent()){
   status.textContent='游戏准备失败：'+error.message;retry.textContent='重试加载';retry.hidden=false;retry.disabled=false;
  }
 }finally{if(!launchAttempted)busy=false;}
}

function hostedReady(){
 if(!provider)return;
 file.hidden=true;start.hidden=true;hint.textContent='资源已准备好，等待启动器开始游戏。';
 launcher.runtimeInfo({version:'th128-hosted-bootstrap/1',partialAdapter:true,musicOutput:{modes:['ogg','none'],requiresCompletePackage:true,midi:false},dataMode:managedOwner?'managed-original-DAT-with-verified-package':'validated-installed-DAT-OGG',fileBridge:false,managedResources:!!managedOwner,replayFiles:{root:'replay/',slots:25,commands:['list','read','write','remove','sync'],mutableBeforeLaunchOnly:true},nativeScoreExport:{path:'scoreth128.dat',readOnly:true}});
 launcher.markReady();
}
const parentWindow=typeof window==='undefined'?globalThis.window:window;
launcher=mountLauncherSession({window:parentWindow,document,location,hooks:{
 async configure(config){
  requireCurrent();if(!provider)throw new Error('Installed game resources are not ready');
  if(config.music!=='ogg'&&config.music!=='none')throw Object.assign(new Error('This page currently supports OGG or no BGM'),{errno:'unsupported-command'});
  if(config.runtimePack!==undefined||!managedOwner&&[config.resources,config.runtimeResources,config.sharedResources].some(rows=>rows?.length))
   throw Object.assign(new Error('External resource maps are not installed by this page'),{errno:'unavailable'});
  const options=config.options??{},supported=new Set(['limitPresentationTo60','touchEnabled']);
  if(Object.entries(options).some(([key,value])=>!supported.has(key)||typeof value!=='boolean'))
   throw Object.assign(new Error('A requested page option is not implemented'),{errno:'unsupported-command'});
  if(config.language){
   const response=await provider.fetch(location.origin+'/assets/catalog.json',{signal:controller.signal}),catalog=await response.json();requireCurrent();
   if(!catalog.variants?.[config.language])throw Object.assign(new Error('Requested language is unavailable'),{errno:'unsupported-command'});
  }
  if(managedOwner)await managedOwner.resources([...(config.resources??[]),...(config.runtimeResources??[]),...(config.sharedResources??[])]);
 },
 acceptLiveResources(){return !!managedOwner;},
 async resources(rows){requireCurrent();if(managedOwner)return managedOwner.resources(rows);if(rows.length||!provider)throw Object.assign(new Error('External DATA maps are unavailable; this page uses its verified installed package'),{errno:'unavailable'});},
 async launch({config,signal}){
  if(signal.aborted)throw new DOMException('Launcher exited','AbortError');
  await launch(config);
 },
 keyboard(value){
  requireCurrent();const runtime=pageLaunch?.runtime;
  if(!runtime)throw new Error('Runtime page is not ready');runtime.keyboard(value);
 },
 keyboardClear(){pageLaunch?.runtime?.clearInput();},
 async files({command,data,phase,signal}){
  requireCurrent();
  if(signal.aborted||!provider)throw new DOMException('Replay file owner unavailable','AbortError');
  if(['write','remove'].includes(command)&&(phase==='running'||launchAttempted))throw Object.assign(new Error('Replay file mutations require the prelaunch owner'),{errno:'invalid-state'});
  replayFiles??=createReplayFileBridge({signal:controller.signal,isCurrent});
  return replayFiles.execute(command,data);
 },
 exit(){closeEntry();}
}});
await discover();
