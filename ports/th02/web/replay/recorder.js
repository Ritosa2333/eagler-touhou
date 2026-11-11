import {ConfirmedTape,VERSION,encodeTape,fingerprint,randomId,stable} from './tape.js';
import {GAME,SOURCE_PATHS,PORTABLE} from './profile.js';
import {createBuilder} from './builder.js';
import {publishNetwork} from './network.js';
import {LOCAL_CHUNK,pointSize,pointIndex} from './local-points.js';
// Every public operation isolates failures from the player's execution loop.
export function createRecorder(toolbar){
  let tape=null,error='',channel=null,builder=null,pointError='',points=[],network=null,closed=false,localSolo=false;
  let id=randomId();
  const watch=document.createElement('a'),info=document.createElement('span');
  watch.textContent='同浏览器观战';watch.target='_blank';watch.rel='noopener';watch.hidden=true;
  watch.href=new URL('../replay.html?watch='+id,import.meta.url).href;
  toolbar?.append(watch,info);
  const open=document.createElement('a');open.textContent='单人 Replay 播放';open.href=new URL('../replay.html',import.meta.url).href;open.target='_blank';open.rel='noopener';open.hidden=true;toolbar?.append(open);
  const remote=document.createElement('a');remote.textContent='跨设备观战链接';remote.target='_blank';remote.rel='noopener';remote.hidden=true;toolbar?.append(remote);
  const upload=document.createElement('span');toolbar?.append(upload);
  function update(){info.textContent=error?'观战历史停止：'+error:tape?` · 已确认 ${tape.rows.length} 步${tape.end?'（本段结束）':''}${points.length?' · '+points.length+' 个定位点':''}${pointError?' · '+pointError:''}`:'';}
  function fail(e){error=String(e.message||e);tape?.finish('录像辅助链路失败：'+error);builder?.finish();network?.kick();update();}
  function guard(fn){try{const r=fn();update();return r;}catch(e){fail(e);}}
  function snapshot(){if(!tape)return;const value=tape.snapshot();if(points.length)value.points=points.filter(p=>p.frame<=value.rows.length);return value;}
  return {
    async boot({settings,players,hostSlot,opening,tickMs=1000/60,adapter='confirmed-worker/1'}){
      try{
        network?.stop();builder?.stop();channel?.close();
        network=builder=channel=null;points=[];pointError=error='';closed=false;
        id=randomId();watch.href=new URL('../replay.html?watch='+id,import.meta.url).href;
        remote.hidden=true;upload.textContent='';
        const identity=await fingerprint(SOURCE_PATHS);if(closed)return;
        tape=new ConfirmedTape({version:VERSION,game:GAME,adapter,settings:structuredClone(settings),players,hostSlot,tickMs,identity,opening});
        localSolo=adapter==='native-local/1'&&players===1;
        open.hidden=!localSolo;
        watch.hidden=localSolo;
        if(PORTABLE&&!localSolo)builder=createBuilder(tape.header,()=>tape.rows,p=>{points.push(p);update();},e=>{pointError='定位点生成停止：'+e.message;update();});
        if(!localSolo)channel=new BroadcastChannel('pc98-replay-'+id);
        let lastReply=0;
        if(channel)channel.onmessage=({data})=>guard(()=>{
          if(data?.type==='pull-point'){
            if(typeof data.viewer!=='string'||data.viewer.length>100||!Number.isInteger(data.frame)||!Number.isInteger(data.offset)||data.offset<0||data.offset%LOCAL_CHUNK)return;
            const p=points.find(p=>p.frame===data.frame);if(!p||data.offset>=pointSize(p))return;
            const start=data.offset/3*4;channel.postMessage({type:'point',viewer:data.viewer,frame:p.frame,sha:p.sha,size:pointSize(p),offset:data.offset,payload:p.data.slice(start,start+LOCAL_CHUNK/3*4)});return;
          }
          if(data?.type!=='pull'||typeof data.viewer!=='string'||data.viewer.length>100||!Number.isInteger(data.first)||data.first<0||data.first>tape.rows.length)return;
          // Bounded replies; a slow viewer never holds the player at a barrier.
          const now=performance.now();if(now-lastReply<10)return;lastReply=now;
          channel.postMessage({type:'slice',viewer:data.viewer,header:tape.header,first:data.first,rows:tape.rows.slice(data.first,data.first+60),available:tape.rows.length,end:tape.end,points:pointIndex(points),pointsDone:!!tape.end&&(!builder||builder.done)});
        });update();
      }catch(e){fail(e);}
    },
    stage:(frame,row)=>guard(()=>{if(!tape)return;if(frame<tape.rows.length){if(stable(row)!==stable(tape.rows[frame]))throw Error('已确认录像在分组回滚时改变');return;}tape.stage(frame,row);}),
    rewind:frame=>guard(()=>tape?.rewind(Math.max(frame,tape.rows.length))),
    confirm:(prefix,dirty)=>guard(()=>{tape?.confirm(prefix,dirty);builder?.kick();network?.kick();}),
    finish:reason=>guard(()=>{tape?.finish(reason);builder?.finish();network?.kick();}),
    connectNetwork(identity){guard(()=>{if(!tape||tape.header.adapter!=='confirmed-worker/1'||network||closed)return;network=publishNetwork(tape.header,()=>tape.rows,()=>tape.end,identity,{getPoints:()=>({points,done:!builder||builder.done}),onLink:url=>{remote.href=url;remote.hidden=false;},onStatus:text=>{remote.title=text;upload.textContent=' · '+text;}});});},
    async seal(timeout=30000){
      if(!tape)return true;
      tape.finish('联机会话结束');builder?.finish();network?.kick();
      const deadline=performance.now()+timeout;upload.textContent=' · 正在封存观战历史…';
      while(builder&&!builder.done&&performance.now()<deadline)await new Promise(resolve=>setTimeout(resolve,50));
      const done=network?await network.seal(Math.max(0,deadline-performance.now())):!builder||builder.done;
      upload.textContent=done?' · 观战历史已封存':' · 封存超时，保留已发布的确认历史';return done;
    },
    async exportSoloReplay(){
      if(!tape||tape.header.adapter!=='native-local/1'||tape.header.players!==1||!tape.end)throw Error('只有已结束的单人局可以导出 TH01 Replay');
      const blob=await encodeTape(snapshot());
      const url=URL.createObjectURL(blob),link=document.createElement('a');link.href=url;link.download=`th01-${Date.now()}.th01r`;link.click();setTimeout(()=>URL.revokeObjectURL(url),30000);
      return blob;
    },
    snapshot,
    get error(){return error;},
    get ended(){return !tape||!!tape.end;},
    close(){closed=true;tape?.finish('观战来源关闭');channel?.close();builder?.stop();network?.stop();}
  };
}
