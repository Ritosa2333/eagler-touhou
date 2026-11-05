import {WebSocketServer,WebSocket} from 'ws';
import {randomBytes,randomInt} from 'node:crypto';
// Memory-only two-player rooms. The relay accepts bounded input words, never
// paths, game assets or save files. A room expires when either peer leaves.
export function attachNetplay(server,{build,origins=[]}={}){
 const rooms=new Map(),allowed=new Set(origins),wss=new WebSocketServer({noServer:true,maxPayload:2048,perMessageDeflate:false});
 const send=(peer,message)=>{if(peer?.socket.readyState===WebSocket.OPEN){if(peer.socket.bufferedAmount>65536){peer.socket.close(1009,'Slow connection');return;}peer.socket.send(JSON.stringify(message));}};
 const broadcast=(room,message)=>{for(const peer of room.peers)send(peer,message);};
 const end=room=>{if(!room||!rooms.delete(room.code))return;broadcast(room,{type:'ended'});for(const peer of room.peers){peer.room=null;peer.socket.close(1000,'Room ended');}};
 server.on('upgrade',(request,socket,head)=>{
  let origin;try{origin=new URL(request.headers.origin);}catch{socket.destroy();return;}
  const same=origin.host===request.headers.host&&(origin.protocol==='http:'||origin.protocol==='https:');
  if(request.url!=='/netplay'||(!same&&!allowed.has(origin.origin))||wss.clients.size>=64){socket.destroy();return;}
  wss.handleUpgrade(request,socket,head,ws=>wss.emit('connection',ws,request));
 });
 wss.on('connection',socket=>{
  const peer={socket,room:null,side:0,received:6,ready:false,last:Date.now(),window:Date.now(),messages:0,hashes:new Map(),lastHash:-1};
  socket.on('error',()=>{});socket.on('close',()=>end(peer.room));socket.peerState=peer;socket.on('pong',()=>peer.last=Date.now());
  socket.on('message',(bytes,binary)=>{try{
   const now=Date.now();if(now-peer.window>=1000){peer.window=now;peer.messages=0;}if(binary||++peer.messages>360)throw Error('Message rate');peer.last=now;
   const m=JSON.parse(bytes.toString());if(!m||typeof m!=='object'||Array.isArray(m))throw Error('Message');
   if(!peer.room){
    if(!['create','join'].includes(m.type)||m.build!==build||!Number.isInteger(m.unlocked)||m.unlocked<31||m.unlocked>65535||(m.unlocked&31)!==31||!Number.isInteger(m.difficulty)||m.difficulty<0||m.difficulty>3||![0,1].includes(m.focus))throw Error('Version or settings');
    peer.unlocked=m.unlocked;peer.difficulty=m.difficulty;peer.focus=m.focus;
    if(m.type==='create'){
     if(rooms.size>=32)throw Error('Rooms full');let code;do{code=randomBytes(6).toString('hex').toUpperCase();}while(rooms.has(code));
     const room={code,peers:[peer],born:now,seed:randomInt(65536),started:false};rooms.set(code,room);peer.room=room;send(peer,{type:'room',code,side:0});
    }else{
     const room=typeof m.code==='string'&&/^[0-9A-F]{12}$/.test(m.code)?rooms.get(m.code):null;if(!room||room.peers.length!==1)throw Error('Room unavailable');peer.side=1;peer.room=room;room.peers.push(peer);send(peer,{type:'room',code:room.code,side:1});
     const left=room.peers[0],configuration={type:'prepare',seed:room.seed,unlocked:left.unlocked&peer.unlocked,difficulty:left.difficulty,focus:left.focus|(peer.focus<<1)};broadcast(room,configuration);
    }return;
   }
   const room=peer.room;
   if(m.type==='leave'){end(room);return;}
   if(m.type==='ready'&&!room.started&&room.peers.length===2){peer.ready=true;if(room.peers.every(p=>p.ready)){room.started=true;broadcast(room,{type:'start'});}return;}
   if(m.type==='result'&&room.started){peer.settled=true;if(room.peers.every(p=>p.settled))end(room);return;}
   if(m.type==='input'&&room.started&&!peer.settled){if(!Number.isSafeInteger(m.frame)||m.frame!==peer.received||!Number.isInteger(m.keys)||m.keys<0||m.keys>65535)throw Error('Input order');const other=room.peers[1-peer.side];if(m.frame>other.received+120)throw Error('Input lead');const moving=m.moving??0,x=m.x??0,y=m.y??0;if(![0,1].includes(moving)||!Number.isFinite(x)||!Number.isFinite(y)||Math.abs(x)>16||Math.abs(y)>16)throw Error('Motion input');++peer.received;send(other,{type:'input',frame:m.frame,keys:m.keys,moving,x,y});return;}
   if(m.type==='hash'&&room.started){if(!Number.isSafeInteger(m.frame)||m.frame<0||m.frame>peer.received||!Number.isInteger(m.hash)||m.hash<0||m.hash>4294967295)throw Error('State hash');if(m.frame<=peer.lastHash)throw Error('Hash order');peer.lastHash=m.frame;peer.hashes.set(m.frame,m.hash);while(peer.hashes.size>16)peer.hashes.delete(peer.hashes.keys().next().value);const hash=room.peers[1-peer.side].hashes.get(m.frame);if(hash!==undefined&&hash!==m.hash){broadcast(room,{type:'error',message:'Simulation states differ'});end(room);}return;}
   throw Error('Unexpected message');
  }catch(e){send(peer,{type:'error',message:e.message});socket.close(1008,'Invalid session');}});
 });
 const timer=setInterval(()=>{const now=Date.now();for(const room of rooms.values())if(!room.started&&now-room.born>10*60*1000)end(room);for(const ws of wss.clients){if(now-ws.peerState.last>45000)ws.terminate();else if(ws.readyState===WebSocket.OPEN)ws.ping();}},15000);timer.unref();
 server.on('close',()=>{clearInterval(timer);for(const room of rooms.values())end(room);wss.close();});
 return {close(){clearInterval(timer);for(const room of rooms.values())end(room);for(const ws of wss.clients)ws.terminate();wss.close();},roomCount:()=>rooms.size};
}
