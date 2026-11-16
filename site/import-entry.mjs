import {DatPackageArchiveStore,installDatPackage,reopenDatPackage} from './runtime/dat-package-install.mjs';
import {installResourceFetchProvider} from './resource-fetch.mjs';
const main=document.querySelector('main');
if(main)main.hidden=true;
const loader=document.createElement('section');
loader.id='resource-loader';
loader.innerHTML='<h1>妖精大战争</h1><p>选择 TH128 资源包，导入后即可开始游戏。</p><div class="resource-actions"><input type="file" accept=".zip,application/zip" aria-label="TH128 资源 ZIP" disabled><button type="button" disabled>开始游戏</button></div><p role="status">正在读取已安装资源…</p>';
document.body.prepend(loader);
const file=loader.querySelector('input'),start=loader.querySelector('button'),status=loader.querySelector('[role="status"]');
if(typeof globalThis.indexedDB?.open!=='function'){
 status.textContent='浏览器未启用本地资源存储，请允许此页面使用存储后重试。';
 throw new Error('TH128 resource storage is unavailable');
}
const store=new DatPackageArchiveStore();
let busy=true,launched=false,pageGone=false,provider=null;
const controller=new AbortController();
function progress(value){if(value.verified)status.textContent='正在检查资源：'+value.verified+'/'+value.total;else if(value.completed)status.textContent='正在读取资源：'+value.completed+'/'+value.total;}
function options(){return{store,origin:location.origin,signal:controller.signal,isCurrent:()=>!pageGone&&!controller.signal.aborted,onProgress:progress};}
function ready(next){
 provider=next;start.disabled=!provider;
 status.textContent=provider?'资源已准备好，点击“开始游戏”。':'请选择 TH128 资源 ZIP。';
}
file.addEventListener('change',async()=>{
 if(busy||launched||!file.files?.[0])return;
 const selected=file.files[0];busy=true;file.disabled=true;start.disabled=true;
 let next=null;
 try{
  if(selected.size>256*1024*1024)throw new Error('资源包过大，请选择 DAT + OGG 版本。');
  status.textContent='正在导入资源…';
  next=await installDatPackage(new Uint8Array(await selected.arrayBuffer()),options());
  provider?.dispose();ready(next);next=null;
 }catch(error){next?.dispose();status.textContent='导入失败：'+error.message+(provider?'。已保留原资源。':'');start.disabled=!provider;}
 finally{busy=false;file.disabled=false;}
});
start.addEventListener('click',async()=>{
 if(busy||launched||!provider)return;
 busy=true;start.disabled=true;file.disabled=true;status.textContent='正在启动…';
 try{
  installResourceFetchProvider(provider);
  await import('./play.mjs');
  launched=true;loader.remove();if(main)main.hidden=false;store.close();
 }catch(error){status.textContent='启动失败，请重新打开页面：'+error.message;busy=false;}
});
addEventListener('pagehide',()=>{pageGone=true;controller.abort();provider?.dispose();store.close();},{once:true});
try{ready(await reopenDatPackage(options()));}
catch(error){status.textContent='已保存资源暂时无法读取，请重新导入：'+error.message;}
finally{busy=false;file.disabled=false;}
