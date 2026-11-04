// A viewer owns its replica and only receives the host's confirmed prefix.
export function createLocalSpectator({game,options,createReplica,validateHeader,validateRow,validatePointIndex,readLocalPoint,stable,channelFactory=name=>new BroadcastChannel(name),onFrame=()=>{},onEvent=()=>{},onError=()=>{}}){
  const url=new URL(options.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
  if(!room||!run)throw Error('观战房间身份无效');
  const viewer=Array.from(crypto.getRandomValues(new Uint8Array(16)),n=>n.toString(16).padStart(2,'0')).join('');
  const channel=channelFactory(`pc98-room-${game}-${room}-${run}`),rows=[];
  const abort=new AbortController();
  let header,identity,replica,available=0,end=null,points=[],closed=false,busy=false,booting=false,paused=false,following=true,target=null,generation=0,timer=null,pullTimer=null,frames=0;
  let last=performance.now(),budget=0;
  const fail=error=>{stop();onError(error);};
  function stop(){if(closed)return;closed=true;abort.abort();clearTimeout(timer);clearInterval(pullTimer);channel.close();replica?.stop();}
  function pull(){if(!closed)channel.postMessage({type:'pull',viewer,first:rows.length});}
  async function pump(){
    if(closed||busy||!replica)return;busy=true;
    const owner=replica;
    try{
      const now=performance.now(),tick=header.tickMs;budget=Math.min(8,budget+(now-last)/tick);last=now;
      const catching=target!==null||following&&rows.length-owner.frame>12,limit=catching?32:Math.floor(budget);let count=0;
      while(!closed&&owner===replica&&(!paused||target!==null)&&owner.frame<rows.length&&count<limit){
        if(target!==null&&owner.frame>=target)break;
        if(owner.advanceBatch){const end=Math.min(rows.length,owner.frame+limit-count,target??rows.length),batch=rows.slice(owner.frame,end);await owner.advanceBatch(batch);count+=batch.length;}
        else{await owner.advance(rows[owner.frame]);count++;}
      }
      if(!catching)budget=Math.max(0,budget-count);
      if(owner===replica&&target!==null&&owner.frame>=target){target=null;paused=!following;}
      if(count&&owner===replica){await owner.present();frames++;}
    }catch(error){if(!closed&&owner===replica)fail(error);}finally{busy=false;if(!closed)timer=setTimeout(pump,8);}
  }
  async function boot(){
    if(booting||replica||closed)return;booting=true;
    const own=generation;
    try{const r=await createReplica(header,{onFrame,onEvent,signal:abort.signal});if(closed||own!==generation){r.stop();return;}replica=r;
      const destination=target??rows.length,point=points.filter(p=>p.frame>0&&p.frame<=destination).at(-1);
      if(point&&readLocalPoint){const base=points[0],baseBytes=await readLocalPoint(channel,viewer,base,abort.signal),bytes=await readLocalPoint(channel,viewer,point,abort.signal);
        const restored=await r.restore(bytes,baseBytes);if(restored.frame!==point.frame||restored.proof!==rows[point.frame-1].proof)throw Error('观战检查点与确认历史不一致');}
      if(closed||own!==generation)return;last=performance.now();await replica.present();frames++;void pump();}
    catch(error){if(!closed&&own===generation)fail(error);}finally{booting=false;if(!closed&&!replica&&own!==generation)void boot();}
  }
  channel.onmessage=({data})=>{
    if(closed||data?.type!=='slice'||data.viewer!==viewer)return;
    try{
      validateHeader(data.header);if(data.header.game!==game)throw Error('观战作品不匹配');
      const nextIdentity=stable(data.header);if(identity&&identity!==nextIdentity)throw Error('观战来源身份改变');
      if(!Array.isArray(data.rows)||data.rows.length>60||!Number.isSafeInteger(data.first)||data.first<0||!Number.isSafeInteger(data.available)||data.available<0)throw Error('观战历史无效');
      if(data.first<rows.length){if(data.first+data.rows.length>rows.length||data.rows.some((row,i)=>stable(row)!==stable(rows[data.first+i])))throw Error('观战历史重传冲突');return;}
      if(data.first!==rows.length||data.available<rows.length+data.rows.length||data.available<available)throw Error('观战历史缺帧');
      data.rows.forEach((row,i)=>validateRow(row,data.header,rows.length+i));
      if(data.end&&(data.end.frame!==data.available||typeof data.end.reason!=='string'))throw Error('观战结束边界无效');
      if(end&&stable(end)!==stable(data.end))throw Error('观战结束边界改变');
      const index=validatePointIndex?validatePointIndex(data.points||[],data.available):[];
      for(let i=0;i<points.length;i++)if(stable(points[i])!==stable(index[i]))throw Error('已发布观战检查点改变');
      header??=data.header;identity??=nextIdentity;rows.push(...data.rows);available=data.available;end=data.end||end;points=index;
      if(rows.length===available&&!replica)void boot();if(rows.length<available)pull();
    }catch(error){fail(error);}
  };
  pullTimer=setInterval(pull,250);pull();
  return {stop,pause(value){paused=!!value;following=!paused;target=null;last=performance.now();},
    seek(frame){if(!Number.isInteger(frame)||frame<0||frame>rows.length)throw Error('观战位置无效');following=false;paused=true;target=frame;
      if(!replica||frame<replica.frame){generation++;replica?.stop();replica=null;void boot();}},
    live(){following=true;paused=false;target=rows.length;},
    get frame(){return replica?.frame??0;},get executed(){return replica?.executed??0;},get received(){return rows.length;},get frames(){return frames;},
    get paused(){return paused;},get following(){return following;},get seeking(){return target!==null||!replica||following&&rows.length-replica.frame>12;},get target(){return target??(following?rows.length:this.frame);},get ended(){return !!end&&replica?.frame===end.frame;}};
}
