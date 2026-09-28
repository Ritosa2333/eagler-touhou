export class Netplay {
 constructor(core,{sync,onStatus,onClose}){this.core=core;this.sync=sync;this.onStatus=onStatus;this.onClose=onClose;this.socket=null;this.timer=null;this.active=false;this.side=0;this.closed=false;this.hashFrame=0;this.prepared=false;this.settled=false;this.connecting=false;this.attempt=0;}
 info(){const c=this.core,p=c._th09_network_info()/4;return Array.from(c.HEAPU32.subarray(p,p+6));}
 send(message){if(this.socket?.readyState===WebSocket.OPEN)this.socket.send(JSON.stringify(message));}
 async connect(code=''){
  if(this.socket||this.connecting)throw Error('已连接房间');this.connecting=true;this.closed=false;this.prepared=false;this.settled=false;this.hashFrame=0;const attempt=++this.attempt;this.core._th09_loop_pause(1);
  const version=await fetch('/version.json').then(r=>{if(!r.ok)throw Error('版本读取失败');return r.json();});
  if(this.closed||attempt!==this.attempt)return;const url=new URL('/netplay',location.href);url.protocol=location.protocol==='https:'?'wss:':'ws:';const socket=this.socket=new WebSocket(url);this.onStatus('连接中……');
  socket.onopen=()=>{if(this.socket!==socket)return;const [unlocked,difficulty,focus]=this.info();this.build=version.build;this.send({type:code?'join':'create',code:code.trim().toUpperCase(),build:version.build,unlocked,difficulty,focus:focus?1:0});};
  socket.onerror=()=>{if(this.socket===socket)this.onStatus('连接失败，请检查服务。');};socket.onclose=()=>{if(this.socket===socket)this.finish();};
  socket.binaryType='arraybuffer';
  socket.onmessage=async event=>{if(this.socket!==socket)return;try{
   if(event.data instanceof ArrayBuffer){const state=globalThis.__eaglerPeerTransport;if(!state||state.received.length-state.receivedHead>=1024)throw Error('联机数据积压');state.received.push(new Uint8Array(event.data));return;}
   const m=JSON.parse(event.data);if(m.type==='room'){this.side=m.side;this.onStatus('房间 '+m.code+' · '+(m.side?'右侧 2P':'左侧 1P')+' · 等待双方');}
   else if(m.type==='prepare'){
    if(!this.core._th09_network_begin(m.seed,this.side,m.unlocked,m.difficulty,m.focus))throw Error('联机初始化失败');
    const state={route:'relay',relay:socket,peers:new Map(),received:[],receivedHead:0,localPlayer:this.side,playerCount:2,failed:false,closed:false,error:'',fail(message){this.failed=true;this.error=message;}};
    globalThis.__eaglerPeerTransport=state;
    const words=this.build.slice(0,24).match(/.{8}/g).map(v=>Number.parseInt(v,16));
    if(!this.core._th09_rollback_enable(m.seed,this.side,words[0]^m.seed,words[1],words[2]))throw Error('Rollback 初始化失败');
    this.prepared=true;this.send({type:'ready'});
   }
   else if(m.type==='start'){
    this.timer=setInterval(()=>{if(!this.prepared)return;const ready=this.core._th09_rollback_pump();if(!ready){this.onStatus('Rollback 会话中断');this.close(false);return;}if(ready===2&&!this.active&&!this.settled){this.active=true;this.onStatus('已连接 · rollback · '+(this.side?'右侧 2P':'左侧 1P'));this.core._th09_loop_pause(+document.hidden);}},16);
   }
   else if(m.type==='error'){this.onStatus('联机结束：'+m.message);this.close(false);}
   else if(m.type==='ended')this.close(false);
  }catch(e){this.onStatus(e.message);this.close(false);}};
 }
 input(){}
 frame(){if(!this.active)return;const at=this.core._th09_rollback_info()/4,stats=this.core.HEAPU32.subarray(at,at+8);globalThis.__eaglerNetplayLanFrame=stats[0];globalThis.__eaglerNetplayLanConfirmed=stats[1];globalThis.__eaglerNetplayLanRollback=stats[2];globalThis.__eaglerNetplayLanResimulated=stats[3];}
 result(){this.settled=true;this.active=false;this.send({type:'result'});this.onStatus('本局结束 · 存档和录像保存在本机');}
 close(notify=true){if(notify)this.send({type:'leave'});this.socket?.close();this.finish();}
 finish(){if(this.closed||(!this.socket&&!this.connecting&&!this.prepared))return;this.closed=true;clearInterval(this.timer);this.timer=null;++this.attempt;this.connecting=false;this.active=false;this.socket=null;if(this.prepared)this.core._th09_network_end();this.prepared=false;this.core._th09_loop_pause(+document.hidden);void this.sync().catch(console.error);this.onClose();}
}
