// Manual development host, deliberately excluded from Runtime Release.
const $=id=>document.getElementById(id),protocol='eagler-touhou/1';
let active=null,sequence=0;const pending=new Map();
function state(text){$('state').textContent=text;}
function fail(error){state('接入未完成：'+error.message);$('start').disabled=false;console.error(error);}
function rpc(command,fields={}){
  if(!active)throw Error('请先启动接入版本');
  const request='manual-'+(++sequence),session=active;
  return new Promise((resolve,reject)=>{
    const timer=setTimeout(()=>{pending.delete(request);reject(Error(command+' 尚未完成'));},60000);
    pending.set(request,{resolve,reject,timer,epoch:session.epoch});
    session.frame.contentWindow.postMessage({protocol,game:'th12',epoch:session.epoch,command,request,...fields},location.origin);
  });
}
window.__eaglerPrepareManagedRuntimeDataV1=async({game,generation,epoch})=>{
  const session=active;
  if(!session||game!=='th12'||generation!==session.generation||epoch!==session.epoch)throw Error('运行时导航已更换');
  state('正在加载本地素材包…');
  const response=await fetch('/__eagler-content/th12.data');if(!response.ok)throw Error('请先生成本地素材包');
  const buffer=await response.arrayBuffer();if(active!==session)throw Error('运行时导航已更换');
  if(buffer.byteLength!==session.provenance.gameData.bytes)throw Error('本地素材包大小不匹配');
  return {buffer,bytes:buffer.byteLength,fileId:'game-data'};
};
window.addEventListener('message',event=>{
  const session=active,message=event.data;
  if(!session||event.source!==session.frame.contentWindow||event.origin!==location.origin||message?.protocol!==protocol||message.game!=='th12'||message.epoch!==session.epoch)return;
  if(typeof message.request==='string'){
    const waiter=pending.get(message.request);if(!waiter||waiter.epoch!==session.epoch)return;
    pending.delete(message.request);clearTimeout(waiter.timer);message.ok?waiter.resolve(message):waiter.reject(Error(message.error??'请求失败'));return;
  }
  if(message.event==='ready'){
    void(async()=>{
      state('资源已准备，正在安装音乐并配置游戏…');
      const resources=session.music==='ogg'?Object.entries(session.provenance.music).map(([file,identity])=>({path:'/music/'+file.split('/').at(-1),url:'/__eagler-content/'+file,...identity})):[];
      await rpc('configure',{language:'ja',music:session.music,resources,options:{limitPresentationTo60:true,touchEnabled:false,touchMovementMode:'touch',touchSensitivity:100,touchFocusMode:'hold-button',doubleTapBombEnabled:false,alwaysHitbox:false,oggDecodeMode:'full'}});
      state('正在启动原版加载画面…');await rpc('launch');session.frame.contentWindow.focus();$('files').disabled=false;
    })().catch(fail);
  }else if(message.event==='first-frame')state('游戏已呈现。可直接用键盘操作；Quit 会保存并退出本次接入会话。');
  else if(message.event==='runtime-info')$('details').textContent=JSON.stringify(message,null,2);
  else if(message.event==='error')fail(Error(message.message??message.error??'运行中断'));
  else if(message.event==='exit'){state('游戏已保存并退出，可以重新启动。');$('start').disabled=false;}
});
$('start').onclick=async()=>{
  $('start').disabled=true;$('files').disabled=true;state('正在准备接入入口…');
  for(const waiter of pending.values()){clearTimeout(waiter.timer);waiter.reject(Error('已启动新的会话'));}pending.clear();
  try{
    const Audio=window.AudioContext??window.webkitAudioContext;
    if(Audio){window.__touhouAudioContext??=new Audio();await window.__touhouAudioContext.resume();}
    const response=await fetch('/__eagler-content/provenance.json');if(!response.ok)throw Error('本地接入素材包尚未生成');
    const provenance=await response.json();if(provenance.schema!=='th12-prepared-content-provenance/1'||provenance.game!=='th12')throw Error('本地素材来源清单无效');
    const frame=document.createElement('iframe'),epoch=sequence+1,generation=provenance.gameData.sha256;
    sequence=epoch;active={frame,epoch,generation,provenance,music:$('music').value};
    frame.allow='autoplay; fullscreen';frame.title='星莲船正式入口接入预览';
    frame.src='/__eagler-runtime/th12.html?'+new URLSearchParams({managedData:'1',runtimeEpoch:String(epoch),gameGeneration:generation});$('game').replaceChildren(frame);
  }catch(error){fail(error);}
};
$('files').onclick=()=>rpc('list').then(result=>{$('details').textContent=JSON.stringify(result.files,null,2);}).catch(fail);
