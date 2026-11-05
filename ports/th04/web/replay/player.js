import {decodeTape,validateHeader,validateRow,stable,MAX_FRAMES,MAX_RAW,randomId,pointBytes,digest} from './tape.js';
import {GAME} from './profile.js';
import {createReplica} from './replica.js';
import {createReplayAudio} from './audio.js';
import {subscribeNetwork,readNetworkPoint,validatePointIndex} from './network.js';
import {readLocalPoint} from './local-points.js';
import {sceneKey,sceneLabel} from './scenes.js';
const $=id=>document.getElementById(id),ctx=$('canvas').getContext('2d',{alpha:false});
let header=null,rows=[],end=null,replica=null,paused=false,target=null,epoch=0,channel=null,poll=null,available=0,loading=false,failed=false,latest=false;
const viewer=randomId();let expectedHeader='',lastReply=performance.now(),receivedBytes=0;
let audio,soundEnabled=false;
let points=[],bootAbort,network,joinStarted=false,sceneIndex='';
function status(text){$('status').textContent=text;}
function update(){const frame=replica?.frame||0;$('seek').max=rows.length;$('seek').value=frame;$('frame').max=rows.length;$('position').textContent=`${frame} / ${rows.length}${end?' · 片段结束':''}`;$('pause').textContent=paused?'继续':'暂停';
  const index=stable(points.filter(p=>p.frame<=rows.length).map(p=>[p.frame,p.scene]));
  if(index!==sceneIndex){sceneIndex=index;const options=[new Option('选择已确认场景','')];let previous='';
    for(const p of points){if(p.frame>rows.length)break;const key=sceneKey(p.scene);if(p.frame===0||key&&key!==previous)options.push(new Option(`${sceneLabel(p.scene,GAME)} · ${p.frame} 步`,String(p.frame)));if(key)previous=key;}
    $('scene').replaceChildren(...options);
  }
}
function cancel(){epoch++;bootAbort?.abort();replica?.stop();replica=null;loading=false;audio?.stop();audio=null;}
function fail(e){failed=true;cancel();clearInterval(poll);channel?.close();network?.stop();status(String(e.message||e));}
function resetAudio(token){
  audio?.stop();audio=createReplayAudio(GAME,e=>{if(token===epoch)$('sound').textContent='声音读取失败，可重新打开录像';});
  if(soundEnabled)void audio.enable();
}
async function boot(token){
  loading=true;status('正在启动独立回放实例…');
  const destination=target;
  bootAbort=new AbortController();const signal=bootAbort.signal;
  resetAudio(token);
  const options={signal,onEvent:data=>{if(token===epoch)audio?.event(data);},onFrame:bitmap=>{if(token===epoch)ctx.drawImage(bitmap,0,0);},onProgress:text=>{if(token===epoch)status(text);}};
  try{let r=await createReplica(header,options);if(token!==epoch){r.stop();return;}
    const point=destination!==null?points.filter(p=>p.frame<=destination).at(-1):null;
    if(point?.frame){
      try{
        const read=p=>p.data?Promise.resolve(pointBytes(p)):remote?readNetworkPoint(remote,p,signal):readLocalPoint(channel,viewer,p,signal);
        const base=points[0],bytes=await read(point),baseBytes=await read(base);
        if(await digest(bytes)!==point.sha||await digest(baseBytes)!==base.sha)throw Error('定位点摘要不一致');
        const restored=await r.restore(bytes,baseBytes);if(restored.frame!==point.frame||restored.proof!==rows[point.frame-1]?.proof)throw Error('定位点与确认历史不一致');
        if(token!==epoch){r.stop();return;}status(`已恢复第 ${point.frame} 步，继续重演到目标`);
      }catch(e){r.stop();if(token!==epoch)return;resetAudio(token);status('定位点不可用，从完整确认输入重建：'+e.message);r=await createReplica(header,options);}
    }
    if(token!==epoch){r.stop();return;}replica=r;await r.present();if(token!==epoch)return;
    if(target!==null&&r.frame>=target){target=null;budget=0;}
    loading=false;update();status('回放已就绪');
  }catch(e){if(token===epoch)fail(e);}
}
async function load(data){cancel();clearInterval(poll);channel?.close();network?.stop();channel=null;expectedHeader='';receivedBytes=0;header=data.header;rows=data.rows;points=data.points||[];end=data.end;available=rows.length;paused=false;target=null;failed=false;latest=false;await boot(epoch);update();}
if(GAME==='01'&&$('file'))$('file').onchange=async event=>{try{
  const file=event.target.files[0];if(!file)return;const data=await decodeTape(file);
  if(data.header.game!==GAME||data.header.adapter!=='native-local/1'||data.header.players!==1)throw Error('TH01 仅支持单人 Replay 文件；多人历史只能通过旁观链接观看');
  await load(data);
}catch(e){fail(e);}};
function seekTo(value){
  if(!header||failed||!Number.isSafeInteger(value)||value<0||value>rows.length)return;
  target=value;latest=false;paused=false;const point=points.filter(p=>p.frame<=target).at(-1);
  if(loading||!replica||target<replica.frame||point&&point.frame>replica.frame){cancel();void boot(epoch);}update();
}
function pausePosition(){const frame=replica?.frame??0;latest=false;paused=true;target=null;
  if(loading){cancel();target=frame;void boot(epoch);}update();
}
$('pause').onclick=()=>{if(!paused)pausePosition();else{paused=false;update();}};
$('seek').onchange=()=>seekTo(Number($('seek').value));
$('jump').onclick=()=>seekTo(Number($('frame').value));
$('scene').onchange=()=>{if($('scene').value!=='')seekTo(Number($('scene').value));};
$('live').onclick=()=>seekTo(rows.length);
$('cancel').onclick=pausePosition;
$('stop').onclick=()=>{cancel();clearInterval(poll);channel?.close();network?.stop();failed=true;status('观战已停止，刷新页面可重新连接。');};
addEventListener('keydown',event=>{if(['INPUT','SELECT','TEXTAREA','BUTTON'].includes(event.target.tagName))return;if(event.code==='Space'||event.code==='Escape'){event.preventDefault();$('pause').click();}});
$('sound').onclick=async()=>{soundEnabled=true;await audio?.enable();$('sound').textContent='声音已启用';audio?.mode(paused||target!==null||loading);};
const watch=new URLSearchParams(location.search).get('watch');
const remote=new URLSearchParams(location.search).get('remote');
function receiveSlice(data){
  validateHeader(data.header);if(data.header.game!==GAME)throw Error('观战作品不匹配');
  const identity=stable(data.header);if(expectedHeader&&identity!==expectedHeader)throw Error('观战来源身份改变');
  // A slow local source can answer a retried pull after its first reply. Only
  // an exactly identical already-retained prefix is an idempotent duplicate.
  if(Number.isInteger(data.first)&&data.first>=0&&data.first<rows.length&&Array.isArray(data.rows)&&data.rows.length<=60&&data.first+data.rows.length<=rows.length){
    if(data.rows.some((row,i)=>stable(row)!==stable(rows[data.first+i])))throw Error('观战重传内容改变');return;
  }
  if(!Array.isArray(data.rows)||data.rows.length>60||data.first!==rows.length||!Number.isInteger(data.available)||data.available<Math.max(available,rows.length+data.rows.length)||data.available>MAX_FRAMES)throw Error('观战批次范围无效');
  data.rows.forEach((r,i)=>validateRow(r,data.header,rows.length+i));
  const index=validatePointIndex(data.points||[],data.available);
  for(let i=0;i<points.length;i++)if(stable(points[i])!==stable(index[i]))throw Error('已发布的观战检查点改变');
  points=index;
  if(data.end&&(data.end.frame!==data.available||typeof data.end.reason!=='string'))throw Error('观战结尾无效');
  if(end&&stable(data.end)!==stable(end))throw Error('观战结束边界改变');
  receivedBytes+=stable(data.rows).length*3;if(receivedBytes>MAX_RAW)throw Error('观战历史达到容量上限');
  rows.push(...data.rows);available=data.available;end=data.end;lastReply=performance.now();
  if(!header){header=data.header;expectedHeader=identity;latest=true;status('正在接收确认历史与定位点…');}
  if(!joinStarted&&rows.length===available){joinStarted=true;target=rows.length;void boot(epoch);}
  update();
}
if(remote){if(!/^[\w-]{32,80}$/.test(remote))fail(Error('网络观战链接无效'));else network=subscribeNetwork(remote,receiveSlice,status,fail);}
if(watch){
  if(!/^[a-f0-9]{32}$/.test(watch))fail(Error('观战链接无效'));
  else{
    channel=new BroadcastChannel('pc98-replay-'+watch);status('正在连接同浏览器的游戏页面…');
    channel.onmessage=({data})=>{try{
      if(data?.type!=='slice'||data.viewer!==viewer)return;
      receiveSlice(data);
    }catch(e){fail(e);}};
    poll=setInterval(()=>{channel?.postMessage({type:'pull',viewer,first:rows.length});if(performance.now()-lastReply>15000)status('观战来源暂时无响应，保留已收到的历史。');},250);
  }
}
let running=false,last=performance.now(),budget=0;
async function tick(){
  if(running)return;running=true;
  const activeEpoch=epoch;
  try{
    const now=performance.now();budget=Math.min(budget+Math.max(0,now-last),100);last=now;
    if(!replica||failed||loading)return;
    audio?.mode(paused||target!==null||$('speed').value!=='1'||end&&replica.frame===end.frame);
    const token=epoch,r=replica,start=performance.now();let count=0;
    while(r===replica&&token===epoch&&r.frame<rows.length&&count<16&&performance.now()-start<12){
      const seeking=target!==null&&r.frame<target;if(!seeking&&(paused||target!==null||budget<header.tickMs/Number($('speed').value)))break;
      await r.advance(rows[r.frame]);if(token!==epoch)return;count++;if(!seeking)budget-=header.tickMs/Number($('speed').value);
    }
    if(count&&token===epoch)await r.present();if(token!==epoch)return;
    if(target!==null&&r.frame>=target){target=null;budget=0;}
    update();
    if(end&&r.frame===end.frame)status('片段播放完毕：'+end.reason);
    else if(r.frame===rows.length)status(watch||remote?'等待新的确认输入…':'播放到当前结尾');
    else status(target!==null?'正在重演到目标位置…':paused?'已暂停':'正在播放');
  }catch(e){if(activeEpoch===epoch&&!loading)fail(e);}finally{running=false;}
}
const playbackTimer=setInterval(tick,8);
addEventListener('pagehide',()=>{cancel();clearInterval(playbackTimer);clearInterval(poll);channel?.close();network?.stop();});
export function playbackState(){return {frame:replica?.frame,executed:replica?.executed,received:rows.length,available,paused,target,loading,failed,status:$('status').textContent};}
