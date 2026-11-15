// Minimal local host for the game's existing eagler-touhou/1 adapter.
// Game source, data, music, saves and this entry point do not use launcher/.
const protocol='eagler-touhou/1',game='th19',epoch=Date.now(),base=new URL('.',import.meta.url);
const loading=document.querySelector('#loading'),errorBox=document.querySelector('#error');
let frame,config,data,serial=0;const pending=new Map();
function fail(error){errorBox.textContent=error?.message||String(error);loading.textContent='';}
async function read(url){const response=await fetch(new URL(url,base));if(!response.ok)throw Error(`资源读取失败：${url} (${response.status})`);return response;}
window.__eaglerPrepareManagedRuntimeDataV1=async()=>{
 data??=read(config.gameData.source).then(r=>r.arrayBuffer());
 const buffer=await data;if(buffer.byteLength!==config.gameData.bytes)throw Error('原版 th19.dat 不完整；从仓库克隆后请运行 git lfs pull。');
 return {buffer};
};
function command(command,fields={}){
 const request=String(++serial);return new Promise((resolve,reject)=>{
  pending.set(request,{resolve,reject});frame.contentWindow.postMessage({protocol,game,epoch,request,command,...fields},location.origin);
 });
}
window.addEventListener('message',async event=>{
 const m=event.data;if(event.source!==frame?.contentWindow||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch)return;
 if(typeof m.request==='string'&&pending.has(m.request)){const p=pending.get(m.request);pending.delete(m.request);m.ok?p.resolve(m):p.reject(Error(m.error));return;}
 try{
  if(m.event==='error'){fail(Error(m.error));return;}
  if(m.event==='transfer')loading.textContent='正在载入原版音乐……';
  if(m.event==='ready'){
   const resources=config.music.ogg.files.map(name=>({path:'/music/'+name,url:new URL(config.music.ogg.base+name,base).href}));
   await command('configure',{music:'ogg',sharedResources:[{path:'/msgothic.ttc',url:new URL('content/msgothic.ttf',base).href}],resources});await command('launch');
  }
  if(m.event==='first-frame'){loading.textContent='';frame.contentWindow.focus();}
 }catch(error){fail(error);}
});
try{
 config=await (await read('game-resources.json')).json();
 frame=document.createElement('iframe');frame.title='東方獣王園';frame.allow='autoplay; fullscreen';
 const url=new URL(config.runtime,base);url.searchParams.set('managedData','1');url.searchParams.set('runtimeEpoch',String(epoch));url.searchParams.set('gameGeneration','standalone');frame.src=url.href;document.body.prepend(frame);
}catch(error){fail(error);}
