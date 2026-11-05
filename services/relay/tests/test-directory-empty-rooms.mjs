import assert from 'node:assert/strict';
import {EventEmitter} from 'node:events';
import {createRoomDirectory} from '../src/room-directory.mjs';

class Socket extends EventEmitter {
  readyState=1; bufferedAmount=0; messages=[];
  send(value){this.messages.push(JSON.parse(value));}
  close(){this.readyState=3;this.emit('close');}
}
const room = (patch={}) => ({multiplayer:{},createdAt:Date.now(),lastActivityAt:Date.now(),
  lobby:{visibility:'public',playerCount:2,seats:[null,null],spectators:new Map(),phase:'lobby',startSerial:1},
  lobbyClients:new Map(),lobbyDisconnectTimers:new Map(),clients:new Map(),runs:new Map(),...patch});
const socket=new Socket(),run={clients:new Map([[0,socket]]),signalClients:new Map(),spectatorClients:new Map()};
const empty=room({runs:new Map([['1',run]])});
const live=room();live.lobby.seats[0]={clientId:'host',name:'H'};live.lobbyClients.set('host',new Socket());
const replay=room({runs:new Map([['1',{...run,clients:new Map(),spectatorClients:new Map([['watch',new Socket()]])}]])});
replay.lobby.spectators.set('watch','W');replay.lobbyClients.set('watch',new Socket());
const reconnect=room({runs:new Map([['1',run]])});reconnect.lobby.seats[0]={clientId:'disconnected',name:'R'};
const rooms=new Map([['th10mp-1111',empty],['th10mp-2222',live],['th10mp-3333',replay],['th10mp-4444',reconnect]]);
const directory=createRoomDirectory({rooms,clearSeat:()=>false,invalidateReady(){},broadcast(){},maybeDelete(){}});
const viewer=new Socket();
const snapshot=()=>{const s=new Socket();directory.connect(s,'directory-viewer');return s.messages.at(-1);};
try{
  const value=snapshot();
  assert.deepEqual(value.rooms.map(r=>r.code).sort(),['2222','3333','4444']);
  assert.equal(value.total,3);
  assert.equal(value.rooms.find(r=>r.code==='3333').spectators,1,'spectator-only replay rooms remain visible');
  assert.equal(value.rooms.find(r=>r.code==='4444').players,1,'reconnecting seats keep their roster');
  assert.equal(socket.readyState,1,'hiding an empty room never closes its Runtime');
  assert.ok(rooms.has('th10mp-1111'),'discovery does not delete room ownership');
  empty.lobby.seats[0]={clientId:'returned',name:'A'};empty.lobbyClients.set('returned',new Socket());
  assert.ok(snapshot().rooms.some(r=>r.code==='1111'),'reseating restores the row');
  empty.lobby.seats[0]=null;empty.lobbyClients.delete('returned');
  directory.connect(viewer,'member-return');directory.admit(new Socket(),'th10mp-1111','unseated-client','member-return');
  viewer.emit('message',Buffer.from(JSON.stringify({type:'refresh',product:'th10mp'})),false);
  const member=viewer.messages.at(-1);
  assert.equal(member.mine?.code,'1111','membership recovery is independent of public rows');
  assert.ok(!member.rooms.some(r=>r.code==='1111'));
  console.log('PASS empty directory rows, retained Runtime, reconnecting seats, spectator replay, reseating and membership recovery');
}finally{directory.close();}
