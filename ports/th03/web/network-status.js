// Presentation only. These observations never change input or recovery policy.
const listeners=new Set();
let latest={phase:'idle',peers:[]},observedAt=0;
const finite=n=>typeof n==='number'&&Number.isFinite(n)&&n>=0;
const ms=n=>finite(n)?`${Math.round(n)} ms`:'—';
const text=(value,max=180)=>typeof value==='string'?value.slice(0,max):'';
export function normalizeNetworkStatus(value={}){
  if(!value||typeof value!=='object')value={};
  const phase=['idle','connecting','connected','stopped'].includes(value.phase)?value.phase:'idle';
  const number=n=>finite(n)?Math.min(n,86400000):null;
  return {phase,bufferMs:number(value.bufferMs),bufferFrames:number(value.bufferFrames),
    serviceRetry:value.serviceRetry===true,message:text(value.message),
    peers:(Array.isArray(value.peers)?value.peers:[]).filter(p=>p&&typeof p==='object').slice(0,3).map(p=>({
      name:text(p.name,24),path:text(p.path,80),rttMs:number(p.rttMs),
      rttAgeMs:number(p.rttAgeMs),packetAgeMs:number(p.packetAgeMs),
      state:['connecting','connected','recovering','offline'].includes(p.state)?p.state:'connecting',
      remainingMs:number(p.remainingMs),attempt:Math.floor(number(p.attempt)||0),
      recovery:text(p.recovery,100),reason:text(p.reason),hidden:p.hidden===true,
    }))};
}
export function describeNetworkStatus(value,elapsed=0){
  const s=normalizeNetworkStatus(value),recovering=s.peers.filter(p=>p.state==='recovering'),offline=s.peers.some(p=>p.state==='offline');
  const online=s.peers.filter(p=>p.state!=='offline');
  const fresh=online.filter(p=>p.state==='connected'&&p.rttMs!==null&&p.rttAgeMs!==null&&p.rttAgeMs+elapsed<=6000);
  const rtt=fresh.length===online.length&&fresh.length?Math.max(...fresh.map(p=>p.rttMs)):null;
  let state=s.phase==='idle'?'等待开局':s.phase==='connecting'?'连接中':s.phase==='stopped'?'本局连接已结束':recovering.length?'连接恢复中':elapsed>6500?'连接状态待更新':offline?'队友已离线':online.some(p=>p.state!=='connected')?'等待连接': '已连接';
  const tone=s.phase==='stopped'?'error':recovering.length||offline||s.serviceRetry||elapsed>6500&&s.phase==='connected'?'warning':s.phase==='connected'?'good':'neutral';
  const summary=`${state}${s.phase==='connected'?` · 往返 ${ms(rtt)}${s.bufferMs!==null?` · 缓冲 ${ms(s.bufferMs)}`:''}`:''}`;
  const rows=s.peers.map(p=>{
    const remaining=p.remainingMs===null?'':` · 剩余 ${Math.max(0,(p.remainingMs-elapsed)/1000).toFixed(1)} 秒`;
    const status=p.state==='offline'?'已离线':p.state==='recovering'?`${p.recovery||'等待恢复'}${remaining}${p.attempt?` · 重试 ${p.attempt} 次`:''}`:p.state==='connected'?'已连接':'连接中';
    const rttFresh=p.rttAgeMs!==null&&p.rttAgeMs+elapsed<=6000;
    return {name:p.name,status,path:p.path||'路径待确认',latency:p.state==='connected'&&rttFresh?ms(p.rttMs):'—',
      detail:[p.packetAgeMs!==null&&p.state!=='offline'?`距最近数据 ${((p.packetAgeMs+elapsed)/1000).toFixed(1)} 秒`:'',p.hidden?'对方页面在后台':'',p.state==='recovering'?p.reason:''].filter(Boolean).join(' · ')};
  });
  return {summary,tone,rows,note:[s.serviceRetry?'房间服务暂时不可用，正在重试；玩家连接单独显示。':'',s.message].filter(Boolean).join(' '),buffer:s.bufferMs===null?'输入缓冲尚未确定':`输入缓冲 ${s.bufferFrames??'—'} 帧（${ms(s.bufferMs)}）`};
}
export function publishNetworkStatus(value){
  latest=normalizeNetworkStatus(value);observedAt=Date.now();
  for(const listener of listeners)listener();
  return latest;
}
export function mountNetworkStatus(target,{details=false}={}){
  if(!target)return;
  const existing=target.querySelector(':scope > .network-status');if(existing)return existing;
  if(!document.querySelector('link[data-network-status-style]')){
    const link=document.createElement('link');link.rel='stylesheet';link.href=new URL('network-status.css',import.meta.url).href;link.dataset.networkStatusStyle='';document.head.append(link);
  }
  const box=document.createElement('section');box.className=`network-status${details?' network-status-details':''}`;box.setAttribute('aria-label',details?'延迟与连接详情':'延迟与连接状态');
  const summary=document.createElement('span');summary.className='network-status-summary';box.append(summary);
  const announcement=document.createElement('span');announcement.className='network-status-announcement';announcement.setAttribute('role','status');box.append(announcement);
  let body;
  if(details){body=document.createElement('div');box.append(body);}else box.title='往返为玩家之间的实测网络延迟；输入缓冲为同步设置，均不代表屏幕端到端延迟。';
  target.append(box);
  let signature='',lastState='';
  const update=()=>{
    const view=describeNetworkStatus(latest,observedAt?Math.max(0,Date.now()-observedAt):0),next=JSON.stringify(view);
    if(signature===next)return;signature=next;box.dataset.tone=view.tone;summary.textContent=view.summary;
    // Announce connection transitions only, not each ping or countdown tick.
    const state=view.summary.split(' · ')[0];if(state!==lastState){lastState=state;announcement.textContent=state;}
    if(body){
      body.replaceChildren();
      const add=(tag,value,where=body)=>{const node=document.createElement(tag);node.textContent=value;where.append(node);return node;};
      add('p',view.buffer);
      for(const row of view.rows){const card=add('div','');card.className='network-status-peer';add('strong',`${row.name} · ${row.status}`,card);add('p',`${row.path} · 往返 ${row.latency}`,card);if(row.detail)add('p',row.detail,card);}
      if(view.note)add('p',view.note);
      add('small','往返延迟是玩家间实测网络耗时，包含浏览器调度；输入缓冲是同步设置。两者均不等于屏幕端到端延迟。');
    }
  };
  listeners.add(update);update();
  const timer=setInterval(()=>{if(box.isConnected)update();else{clearInterval(timer);listeners.delete(update);}},500);
  return box;
}
