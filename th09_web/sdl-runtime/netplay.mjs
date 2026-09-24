export class Netplay {
 constructor(core,{sync,onStatus,onClose}){this.core=core;this.sync=sync;this.onStatus=onStatus;this.onClose=onClose;this.socket=null;this.active=false;this.side=0;this.closed=false;this.hashFrame=0;this.prepared=false;this.settled=false;this.connecting=false;this.attempt=0;}
 info(){const c=this.core,p=c._th09_network_info()/4;return Array.from(c.HEAPU32.subarray(p,p+6));}
 send(message){if(this.socket?.readyState===WebSocket.OPEN)this.socket.send(JSON.stringify(message));}
 async connect(code=''){
  if(this.socket||this.connecting)throw Error('已连接房间');this.connecting=true;this.closed=false;this.prepared=false;this.settled=false;this.hashFrame=0;const attempt=++this.attempt;this.core._th09_loop_pause(1);
  const version=await fetch('version.json').then(r=>{if(!r.ok)throw Error('版本读取失败');return r.json();});
  if(this.closed||attempt!==this.attempt)return;const url=new URL('/netplay',location.href);url.protocol=location.protocol==='https:'?'wss:':'ws:';const socket=this.socket=new WebSocket(url);this.onStatus('连接中……');
  socket.onopen=()=>{if(this.socket!==socket)return;const [unlocked,difficulty,focus]=this.info();this.send({type:code?'join':'create',code:code.trim().toUpperCase(),build:version.build,unlocked,difficulty,focus:focus?1:0});};
  socket.onerror=()=>{if(this.socket===socket)this.onStatus('连接失败，请检查服务。');};socket.onclose=()=>{if(this.socket===socket)this.finish();};
  socket.onmessage=async event=>{if(this.socket!==socket)return;try{const m=JSON.parse(event.data);if(m.type==='room'){this.side=m.side;this.onStatus('房间 '+m.code+' · '+(m.side?'右侧 2P':'左侧 1P')+' · 等待双方');}
   else if(m.type==='prepare'){if(!this.core._th09_network_begin(m.seed,this.side,m.unlocked,m.difficulty,m.focus))throw Error('联机初始化失败');this.prepared=true;this.send({type:'ready'});}
   else if(m.type==='start'){this.active=true;this.onStatus('已连接 · '+(this.side?'右侧 2P':'左侧 1P'));this.core._th09_loop_pause(+document.hidden);}
   else if(m.type==='input'&&!this.settled){if(!this.core._th09_network_receive(m.frame,m.keys,m.moving||0,m.x||0,m.y||0))throw Error('联机输入序号错误');}
   else if(m.type==='error'){this.onStatus('联机结束：'+m.message);this.close(false);}
   else if(m.type==='ended')this.close(false);
  }catch(e){this.onStatus(e.message);this.close(false);}};
 }
 input(frame,keys,moving=0,x=0,y=0){if(this.active)this.send({type:'input',frame,keys,moving,x,y});}
 frame(){if(!this.active)return;const frame=this.info()[3];if(frame&&frame%120===0&&frame!==this.hashFrame){this.hashFrame=frame;this.send({type:'hash',frame,hash:this.core._th09_network_hash()>>>0});}}
 result(){this.settled=true;this.active=false;this.send({type:'result'});this.onStatus('本局结束 · 存档和录像保存在本机');}
 close(notify=true){if(notify)this.send({type:'leave'});this.socket?.close();this.finish();}
 finish(){if(this.closed||(!this.socket&&!this.connecting&&!this.prepared))return;this.closed=true;++this.attempt;this.connecting=false;this.active=false;this.socket=null;if(this.prepared&&!this.settled)this.core._th09_network_end();this.prepared=false;this.core._th09_loop_pause(+document.hidden);void this.sync().catch(console.error);this.onClose();}
}
