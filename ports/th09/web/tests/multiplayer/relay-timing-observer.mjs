// Optional --import preloader for the LOCAL test relay only. No game payloads,
// room codes or credentials are logged; this is not part of a published relay.
import {createRequire} from 'node:module';
import {performance} from 'node:perf_hooks';

if(process.env.EAGLER_NETPLAY_RELAY_HOST!=='127.0.0.1'||!process.env.TH09_OBSERVED_RELAY)
 throw Error('Relay timing observation requires an explicitly named loopback test relay');
const require=createRequire(process.env.TH09_OBSERVED_RELAY);
const WebSocket=require('ws');
const slots=new Map();let sequence=0,last=performance.now(),samples=0;
const serverEmit=WebSocket.Server.prototype.emit;
WebSocket.Server.prototype.emit=function(event,...args){
 if(event==='connection'&&slots.size<24){
  const [socket,request]=args,url=new URL(request.url,'http://127.0.0.1');
  const kind=url.searchParams.has('lobby')?'lobby':url.searchParams.has('signal')?'signal':
    url.searchParams.has('spectator')?'spectator':'player';
  const seat=Number(url.searchParams.get('player'));
  const row={id:++sequence,kind,seat:url.searchParams.has('player')&&Number.isInteger(seat)&&seat>=0&&seat<=2?seat:null,
   received:0,sent:0,completed:0,errors:0,receivedBytes:0,sentBytes:0,lastReceiveMs:null,lastSendMs:null,lastCompleteMs:null};
  slots.set(socket,row);
  socket.on('close',()=>slots.delete(socket));
 }
 return serverEmit.call(this,event,...args);
};
const emit=WebSocket.prototype.emit,send=WebSocket.prototype.send;
WebSocket.prototype.emit=function(event,...args){
 const row=slots.get(this);
 if(row&&event==='message'&&args[1]){++row.received;row.receivedBytes+=args[0]?.byteLength??0;row.lastReceiveMs=performance.now();}
 return emit.call(this,event,...args);
};
WebSocket.prototype.send=function(data,options,callback){
 const row=slots.get(this),binary=typeof data!=='string';
 if(!row||!binary)return send.call(this,data,options,callback);
 if(typeof options==='function'){callback=options;options=undefined;}
 ++row.sent;row.sentBytes+=data?.byteLength??0;row.lastSendMs=performance.now();
 return send.call(this,data,options,function(error){
  ++row.completed;if(error)++row.errors;row.lastCompleteMs=performance.now();
  if(callback)callback.call(this,error);
 });
};
const timer=setInterval(()=>{
 const now=performance.now(),gapMs=now-last;last=now;
 const sockets=[...slots].map(([socket,row])=>({...row,bufferedAmount:socket.bufferedAmount,
  receiveAgeMs:row.lastReceiveMs===null?null:now-row.lastReceiveMs,
  sendAgeMs:row.lastSendMs===null?null:now-row.lastSendMs}));
 process.stdout.write('TH09_RELAY_TIMING '+JSON.stringify({wallMs:performance.timeOrigin+now,gapMs,sockets})+'\n');
 if(++samples>=600)clearInterval(timer);
},100);
timer.unref();
