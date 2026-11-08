import {createPc98MatchIndex} from './pc98-match-index.mjs';
// A viewer owns its replica and only receives the host's confirmed prefix.
export function createLocalSpectator({game,options,createReplica,validateHeader,validateRow,validatePointIndex,readLocalPoint,stable,smoothPresentation=false,channelFactory=name=>new BroadcastChannel(name),onHeader=()=>{},onFrame=()=>{},onEvent=()=>{},onError=()=>{}}){
  const url=new URL(options.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
  if(!room||!run)throw Error('观战房间身份无效');
  const viewer=Array.from(crypto.getRandomValues(new Uint8Array(16)),n=>n.toString(16).padStart(2,'0')).join('');
  const channel=channelFactory(`pc98-room-${game}-${room}-${run}`),rows=[],matches=createPc98MatchIndex(game);
  const abort=new AbortController();
  let header,identity,replica,available=0,minFrame=0,end=null,points=[],closed=false,busy=false,booting=false,paused=false,following=true,target=null,generation=0,timer=null,pullTimer=null,frames=0;
  let last=performance.now(),lastPresent=0,budget=0;
  let catching=false,delayed=false,delayFrames=0,catchSince=0,catchStartLag=0;
  const fail=error=>{stop();onError(error);};
  function stop(){if(closed)return;closed=true;abort.abort();clearTimeout(timer);clearInterval(pullTimer);channel.close();replica?.stop();}
  function pull(){if(!closed)channel.postMessage({type:'pull',viewer,first:rows.length});}
  async function pump(){
    if(closed||busy||!replica)return;busy=true;
    const owner=replica;let workPending=false;
    try{
      const now=performance.now(),tick=header.tickMs;budget=Math.min(8,budget+(now-last)/tick);last=now;
      const lag=rows.length-owner.frame,normalDelay=Math.round((smoothPresentation?1200:600)/tick);
      catching=target!==null||following&&!delayed&&lag>normalDelay+30;
      if(catching&&following){
        if(!catchSince){catchSince=now;catchStartLag=lag;}
        else if(now-catchSince>=4000){
          if(smoothPresentation&&catchStartLag-lag>=(header.spectatorInputOnly?1:30)){catchSince=now;catchStartLag=lag;}
          else{delayed=true;delayFrames=lag;target=null;catching=false;}
        }
      }else catchSince=0;
      const delay=delayed?delayFrames:normalDelay;
      const limit=catching?32:smoothPresentation?Math.min(1,Math.floor(budget)):Math.floor(budget),edge=following&&!end&&target===null&&!catching?Math.min(rows.length,Math.max(1,rows.length-delay)):rows.length;let count=0;
      while(!closed&&owner===replica&&(!paused||target!==null)&&owner.frame<edge&&count<limit){
        if(target!==null&&owner.frame>=target)break;
        if(owner.advanceBatch){const end=Math.min(edge,owner.frame+limit-count,target??edge),batch=rows.slice(owner.frame,end);await owner.advanceBatch(batch);count+=batch.length;}
        else{await owner.advance(rows[owner.frame]);count++;}
      }
      if(!catching)budget=Math.max(0,budget-count);
      workPending=count>0&&budget>=1;
      if(owner===replica&&target!==null&&owner.frame>=target){target=null;paused=!following;}
      if(count&&owner===replica&&(!catching||performance.now()-lastPresent>=150)){await owner.present();lastPresent=performance.now();}
    }catch(error){if(!closed&&owner===replica)fail(error);}finally{busy=false;if(!closed)timer=setTimeout(pump,catching||smoothPresentation&&workPending?0:8);}
  }
  async function boot(){
    if(booting||replica||closed)return;booting=true;
    const own=generation;
    try{const r=await createReplica(header,{onFrame:bitmap=>{onFrame(bitmap);frames++;},onEvent,signal:abort.signal});if(closed||own!==generation){r.stop();return;}replica=r;
      const destination=target??rows.length,point=points.filter(p=>p.frame>0&&p.frame<=destination).at(-1);
      if(point&&readLocalPoint){const base=points[0],baseBytes=await readLocalPoint(channel,viewer,base,abort.signal),bytes=await readLocalPoint(channel,viewer,point,abort.signal);
        const restored=await r.restore(bytes,baseBytes);if(restored.frame!==point.frame||restored.proof!==rows[point.frame-1].proof)throw Error('观战检查点与确认历史不一致');}
      if(closed||own!==generation)return;last=performance.now();await replica.present();void pump();}
    catch(error){if(!closed&&own===generation)fail(error);}finally{booting=false;if(!closed&&!replica&&own!==generation)void boot();}
  }
  channel.onmessage=({data})=>{
    if(closed||data?.type!=='slice'||data.viewer!==viewer)return;
    try{
      validateHeader(data.header);if(data.header.game!==game)throw Error('观战作品不匹配');
      const nextIdentity=stable(data.header);if(identity&&identity!==nextIdentity)throw Error('观战来源身份改变');
      if(!Array.isArray(data.rows)||data.rows.length>60||!Number.isSafeInteger(data.first)||data.first<0||!Number.isSafeInteger(data.available)||data.available<0)throw Error('观战历史无效');
      if(rows.length===0&&!replica&&data.min>0){if(!Number.isSafeInteger(data.min)||data.min<data.first||data.min>data.available)throw Error('观战局起点无效');rows.length=data.first;minFrame=data.min;}
      if(data.first<rows.length){if(data.first+data.rows.length>rows.length||data.rows.some((row,i)=>stable(row)!==stable(rows[data.first+i])))throw Error('观战历史重传冲突');return;}
      if(data.first!==rows.length||data.available<rows.length+data.rows.length||data.available<available)throw Error('观战历史缺帧');
      data.rows.forEach((row,i)=>validateRow(row,data.header,rows.length+i));
      if(data.end&&(data.end.frame!==data.available||typeof data.end.reason!=='string'))throw Error('观战结束边界无效');
      if(end&&stable(end)!==stable(data.end))throw Error('观战结束边界改变');
      const index=validatePointIndex?validatePointIndex(data.points||[],data.available):[];
      for(let i=0;i<points.length;i++)if(stable(points[i])!==stable(index[i]))throw Error('已发布观战检查点改变');
      if(!header){header=data.header;onHeader(header);}identity??=nextIdentity;matches.append(data.rows,data.first);rows.push(...data.rows);available=data.available;end=data.end||end;points=index;
      if(rows.length===available&&!replica&&channel.ready!==false)void boot();if(rows.length<available)pull();
    }catch(error){fail(error);}
  };
  pullTimer=setInterval(pull,50);pull();
  return {stop,pause(value){paused=!!value;following=!paused;target=null;last=performance.now();},
    seek(frame){if(!Number.isInteger(frame)||frame<minFrame||frame>rows.length)throw Error('观战位置无效');following=false;paused=true;target=frame;catching=true;
      if(!replica||frame<replica.frame){generation++;replica?.stop();replica=null;void boot();}},
    live(){following=true;paused=false;const delay=['02','04'].includes(game)?Math.round((smoothPresentation?1200:600)/(header?.tickMs||1000/60)):0;target=Math.max(minFrame,rows.length-delay);delayed=false;catchSince=0;catching=true;if(replica&&target<=replica.frame)target=null;},
    get frame(){return replica?.frame??minFrame;},get min(){return minFrame;},get executed(){return replica?.executed??0;},get received(){return rows.length;},get frames(){return frames;},
    get nodes(){return matches.nodes.filter(node=>node.frame>=minFrame-1).map(node=>node.frame<minFrame?{...node,frame:minFrame}:node);},get paused(){return paused;},get following(){return following;},get catching(){return catching;},get seeking(){return target!==null||!replica||!frames;},get target(){return target??(following?rows.length:this.frame);},get ended(){return !!end&&replica?.frame===end.frame;}};
}
