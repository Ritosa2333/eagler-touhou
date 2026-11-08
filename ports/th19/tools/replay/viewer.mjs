import {decodeTh19Replay} from './runtime/th19-recording.mjs';
const $=id=>document.getElementById(id),protocol='eagler-touhou/1',game='th19',epoch=Date.now();
const base=new URL('.',location.href),pending=new Map();let request=0,frame,ready=false,session,data,opening=false;
const fail=e=>{$('error').textContent=e.message||String(e);};
async function read(path){const r=await fetch(new URL(path,base));if(!r.ok)throw Error(`资源读取失败：${path}`);return r;}
window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(data??=read(session.config.gameData.source).then(r=>r.arrayBuffer()))});
function command(command,fields={}){return new Promise((resolve,reject)=>{const id=String(++request),timer=setTimeout(()=>{pending.delete(id);reject(Error('播放器响应超时'));},180000);pending.set(id,{resolve,reject,timer});frame.contentWindow.postMessage({protocol,game,epoch,request:id,command,...fields},location.origin);});}
function update(p){if(!p)return;$('seek').max=p.total;$('seek').value=p.frame;$('position').textContent=`${p.frame} / ${p.total}`;$('status').textContent=p.ended?'播放结束':p.seeking?'正在定位……':p.paused?'已暂停':'正在回放';}
async function open(bytes){
 if(!ready||opening)throw Error('播放器正在准备，请稍后再试');
 const tape=decodeTh19Replay(bytes);
 // Decoding validates the format; the runtime checks deterministic replay state.
 opening=true;$('error').textContent='';
 try{
  await command('write',{path:'replay/th19_ud0000.rpy',bytes:Array.from(bytes)});
  const r=await command('playback',{action:'open',path:'replay/th19_ud0000.rpy'});update(r.playback);
  for(const id of ['play','pause','restart','rate','seek'])$(id).disabled=false;
 }finally{opening=false;}
}
window.addEventListener('message',async e=>{
 const m=e.data;if(e.source!==frame?.contentWindow||e.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch)return;
 const p=pending.get(m.request);if(p){pending.delete(m.request);clearTimeout(p.timer);m.ok?p.resolve(m):p.reject(Error(m.error));return;}
 try{
  if(m.event==='error')fail(Error(m.error));
  if(m.event==='runtime-info')update(m.playback);
  if(m.event==='ready'){
   const c=session.config,resources=c.music.ogg.files.map(name=>({path:'/music/'+name,url:new URL(c.music.ogg.base+name,base).href}));
   await command('configure',{music:'ogg',options:{replayViewer:true,touchEnabled:false},resources,sharedResources:[{path:'/msgothic.ttc',url:new URL('content/msgothic.ttf',base).href}]});
   ready=true;$('status').textContent='请选择录像';
   if(session.hasReplay)await open(new Uint8Array(await(await read('recording.rpy')).arrayBuffer()));
  }
 }catch(e){fail(e);}
});
$('file').onchange=async()=>{try{const f=$('file').files[0];if(!f)return;if(f.size>32*1024*1024)throw Error('录像超过32 MiB');await open(new Uint8Array(await f.arrayBuffer()));}catch(e){fail(e);}};
const control=async(action,value)=>{try{update((await command('playback',{action,value})).playback);}catch(e){fail(e);}};
$('play').onclick=()=>control('play');$('pause').onclick=()=>control('pause');$('rate').onchange=()=>control('rate',Number($('rate').value));$('seek').onchange=()=>control('seek',Number($('seek').value));$('restart').onclick=async()=>{await control('seek',0);await control('play');};
try{
 session=await(await read('session.json')).json();
 frame=document.createElement('iframe');frame.title='录像画面';frame.allow='autoplay; fullscreen';
 const url=new URL('runtime/th19.html',base);url.searchParams.set('managedData','1');url.searchParams.set('runtimeEpoch',epoch);url.searchParams.set('gameGeneration','replay-viewer');frame.src=url.href;$('screen').append(frame);
}catch(e){fail(e);}
