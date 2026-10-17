import test from 'node:test';
import assert from 'node:assert/strict';
import http from 'node:http';
import WebSocket from 'ws';
import {attachNetplay} from '../../scripts/netplay-relay.mjs';
test('Relay separates rooms, checks build/origin/input order and removes disconnected sessions',async()=>{
 const server=http.createServer(),relay=attachNetplay(server,{build:'test-build'});await new Promise(r=>server.listen(0,'127.0.0.1',r));const base='http://127.0.0.1:'+server.address().port,clients=[];
 async function connect(origin=base){const ws=new WebSocket(base.replace('http','ws')+'/netplay',{origin});clients.push(ws);const queue=[],waiters=[];ws.on('message',b=>{const value=JSON.parse(b);if(waiters.length)waiters.shift()(value);else queue.push(value);});ws.on('error',()=>{});const next=()=>queue.length?Promise.resolve(queue.shift()):new Promise((resolve,reject)=>{const t=setTimeout(()=>reject(Error('message timeout')),3000);waiters.push(value=>{clearTimeout(t);resolve(value);});});await new Promise((r,j)=>{ws.once('open',r);ws.once('error',j);});return {ws,next,send:m=>ws.send(JSON.stringify(m))};}
 const settings={build:'test-build',unlocked:31,difficulty:1,focus:0};
 try{
  await assert.rejects(()=>connect('https://unrelated.invalid'));
  const bad=await connect();bad.send({...settings,type:'create',build:'old'});assert.equal((await bad.next()).type,'error');
  const host=await connect(),guest=await connect(),isolated=await connect();host.send({...settings,type:'create'});const room=await host.next();assert.match(room.code,/^[0-9A-F]{12}$/);isolated.send({...settings,type:'create'});const other=await isolated.next();assert.notEqual(room.code,other.code);
  guest.send({...settings,type:'join',code:room.code});assert.equal((await guest.next()).type,'room');const a=await host.next(),b=await guest.next();assert.deepEqual(a,b);assert.equal(a.type,'prepare');host.send({type:'ready'});guest.send({type:'ready'});assert.equal((await host.next()).type,'start');assert.equal((await guest.next()).type,'start');
  host.send({type:'input',frame:6,keys:129});assert.deepEqual(await guest.next(),{type:'input',frame:6,keys:129,moving:0,x:0,y:0});guest.send({type:'input',frame:6,keys:65});assert.deepEqual(await host.next(),{type:'input',frame:6,keys:65,moving:0,x:0,y:0});
  host.send({type:'input',frame:7,keys:1,moving:1,x:1.25,y:-0.375});assert.deepEqual(await guest.next(),{type:'input',frame:7,keys:1,moving:1,x:1.25,y:-0.375});
  host.send({type:'input',frame:9,keys:0});assert.equal((await host.next()).type,'error');assert.equal((await guest.next()).type,'ended');assert.equal(relay.roomCount(),1);isolated.send({type:'leave'});assert.equal((await isolated.next()).type,'ended');assert.equal(relay.roomCount(),0);
 }finally{for(const ws of clients)ws.terminate();relay.close();await new Promise(r=>server.close(r));}
});
