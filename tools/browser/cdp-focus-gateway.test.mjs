import assert from 'node:assert/strict';
import {after, test} from 'node:test';
import {createRequire} from 'node:module';
import {assessFocusRewriteForTarget,createCdpFocusGateway,rewriteFocusEmulationMessage,resolveFullChromeExecutable,validateDevToolsEndpoint} from './cdp-focus-gateway.mjs';
import {createServer} from 'node:http';

const require=createRequire(new URL('../../launcher/package.json',import.meta.url));
const {WebSocket,WebSocketServer}=require('ws');
assert.equal(typeof WebSocketServer,'function');
const servers=[];
const stalledSockets=new Set();
after(async()=>{
  for(const socket of stalledSockets)socket.destroy();
  for(const server of servers){
    for(const client of server.clients??[])client.terminate();
    if(server.listening||typeof server.address==='function'&&server.address())await new Promise(resolve=>server.close(resolve));
  }
});
const waitFor=(emitter,event,predicate=()=>true)=>new Promise((resolve,reject)=>{
  const timer=setTimeout(()=>{cleanup();reject(new Error(`Timed out waiting for ${event}`));},3000);
  const message=value=>{if(!predicate(value))return;cleanup();resolve(value);};
  const error=reason=>{cleanup();reject(reason);};
  const cleanup=()=>{clearTimeout(timer);emitter.off(event,message);emitter.off('error',error);};
  emitter.on(event,message);emitter.once('error',error);
});

test('focus command rewrite preserves the request and changes only enabled=true',()=>{
  const source={id:7,sessionId:'page-session',method:'Emulation.setFocusEmulationEnabled',params:{enabled:true,extra:'kept'}};
  const rewritten=rewriteFocusEmulationMessage(JSON.stringify(source));
  assert.equal(rewritten.rewritten,true);
  assert.deepEqual(JSON.parse(rewritten.text),{...source,params:{enabled:false,extra:'kept'}});
  assert.deepEqual(source.params,{enabled:true,extra:'kept'});
  assert.equal(rewriteFocusEmulationMessage(JSON.stringify({...source,params:{enabled:false}})).rewritten,false);
  assert.equal(rewriteFocusEmulationMessage(JSON.stringify({...source,method:'Page.enable'})).rewritten,false);
  assert.match(rewriteFocusEmulationMessage('{').error,/Invalid CDP JSON/);
});

test('owned DevToolsActivePort must match the advertised loopback websocket endpoint',()=>{
  assert.equal(validateDevToolsEndpoint({port:9222,wsPath:'/devtools/browser/owned',
    webSocketDebuggerUrl:'ws://127.0.0.1:9222/devtools/browser/owned'}),
  'ws://127.0.0.1:9222/devtools/browser/owned');
  assert.throws(()=>validateDevToolsEndpoint({port:9222,wsPath:'/devtools/browser/owned',
    webSocketDebuggerUrl:'ws://127.0.0.1:9223/devtools/browser/owned'}),/does not match its owned DevToolsActivePort/);
  assert.throws(()=>validateDevToolsEndpoint({port:9222,wsPath:'/devtools/browser/owned',
    webSocketDebuggerUrl:'ws://example.test:9222/devtools/browser/owned'}),/must use loopback ws/);
  assert.throws(()=>validateDevToolsEndpoint({port:9222,wsPath:'/devtools/browser/owned',
    webSocketDebuggerUrl:'ws://127.0.0.1:9222/devtools/browser/other'}),/does not match its owned DevToolsActivePort/);
});

test('gateway accepts only a local websocket upstream before binding its route',async()=>{
  await assert.rejects(createCdpFocusGateway('ws://example.test:9222/devtools/browser/id'),/Expected a local ws:/);
  await assert.rejects(createCdpFocusGateway('wss://127.0.0.1:9222/devtools/browser/id'),/Expected a local ws:/);
});

test('headed launch refuses missing or headless-shell executables without fallback',async()=>{
  const chromium={executablePath:()=>''};
  await assert.rejects(resolveFullChromeExecutable(chromium,{env:{}}),/do not fall back to chrome-headless-shell/);
  await assert.rejects(resolveFullChromeExecutable({executablePath:()=> 'C:/browser/chrome-headless-shell.exe'},{env:{}}),/require full Chrome/);
  await assert.rejects(resolveFullChromeExecutable({executablePath:()=> 'C:/missing/chrome.exe'},{env:{}}),/unavailable/);
});

test('loopback gateway forwards the rewritten command and audits its target and response',async()=>{
  let received;
  const upstreamServer=new WebSocketServer({host:'127.0.0.1',port:0});servers.push(upstreamServer);
  await waitFor(upstreamServer,'listening');
  upstreamServer.on('connection',socket=>{
    socket.send(JSON.stringify({method:'Target.attachedToTarget',params:{sessionId:'page-session',targetInfo:{targetId:'target-1',type:'page',url:'about:blank'}}}));
    socket.on('message',data=>{
      const command=JSON.parse(data.toString());received=command;
      socket.send(JSON.stringify({id:command.id,result:{acknowledged:true},sessionId:command.sessionId}));
    });
  });
  const gateway=await createCdpFocusGateway(`ws://127.0.0.1:${upstreamServer.address().port}/devtools/browser/test`);
  const client=new WebSocket(gateway.endpoint);
  try {
    await waitFor(client,'open');
    await waitFor(client,'message',data=>JSON.parse(data.toString()).method==='Target.attachedToTarget');
    const response=waitFor(client,'message',data=>JSON.parse(data.toString()).id===17);
    client.send(JSON.stringify({id:17,sessionId:'page-session',method:'Emulation.setFocusEmulationEnabled',params:{enabled:true}}));
    assert.deepEqual(JSON.parse((await response).toString()),{id:17,result:{acknowledged:true},sessionId:'page-session'});
    assert.equal(received.params.enabled,false);
    const audit=gateway.snapshot();
    assert.equal(audit.rewrites.length,1);
    assert.deepEqual(audit.rewrites[0],{
      at:audit.rewrites[0].at,id:17,sessionId:'page-session',target:{targetId:'target-1',type:'page',url:'about:blank'},
      requestedEnabled:true,forwardedEnabled:false,rewritten:true,status:'acknowledged',responseAt:audit.rewrites[0].responseAt,
      response:{result:{acknowledged:true}},
    });
    assert.equal(assessFocusRewriteForTarget(audit,'target-1').ready,true);
    assert.equal(assessFocusRewriteForTarget(audit,'other-target').ready,false);
    assert.deepEqual(audit.errors,[]);
  } finally {
    await gateway.close();
    if(client.readyState===WebSocket.OPEN)client.close();
  }
});

test('focus response acknowledgement is correlated by request id and session',async()=>{
  const upstreamServer=new WebSocketServer({host:'127.0.0.1',port:0});servers.push(upstreamServer);
  await waitFor(upstreamServer,'listening');
  upstreamServer.on('connection',socket=>{
    for(const [sessionId,targetId] of [['page-session','target-1'],['other-session','target-2']])
      socket.send(JSON.stringify({method:'Target.attachedToTarget',params:{sessionId,targetInfo:{targetId,type:'page',url:'about:blank'}}}));
    socket.on('message',data=>{
      const command=JSON.parse(data.toString());
      socket.send(JSON.stringify({id:command.id,sessionId:'other-session',result:{wrongSession:true}}));
      setTimeout(()=>socket.send(JSON.stringify({id:command.id,sessionId:command.sessionId,result:{acknowledged:true}})),25);
    });
  });
  const gateway=await createCdpFocusGateway(`ws://127.0.0.1:${upstreamServer.address().port}/devtools/browser/test`);
  const client=new WebSocket(gateway.endpoint);
  try{
    await waitFor(client,'open');
    await waitFor(client,'message',data=>JSON.parse(data.toString()).method==='Target.attachedToTarget');
    const response=waitFor(client,'message',data=>{const message=JSON.parse(data.toString());return message.id===18&&message.sessionId==='page-session';});
    const wrongSession=waitFor(client,'message',data=>{const message=JSON.parse(data.toString());return message.id===18&&message.sessionId==='other-session';});
    client.send(JSON.stringify({id:18,sessionId:'page-session',method:'Emulation.setFocusEmulationEnabled',params:{enabled:true}}));
    await wrongSession;
    assert.equal(gateway.snapshot().rewrites[0].status,'pending','A same-id response from another CDP session must not acknowledge this request.');
    await response;
    const audit=gateway.snapshot();
    assert.equal(audit.rewrites[0].status,'acknowledged');
    assert.equal(audit.rewrites[0].target.targetId,'target-1');
    assert.equal(assessFocusRewriteForTarget(audit,'target-1').ready,true);
    assert.equal(assessFocusRewriteForTarget(audit,'target-2').ready,false);
  }finally{
    await gateway.close();if(client.readyState===WebSocket.OPEN)client.close();
  }
});

test('Chrome focus response errors are fatal and retained with their CDP identity',async()=>{
  const upstreamServer=new WebSocketServer({host:'127.0.0.1',port:0});servers.push(upstreamServer);
  await waitFor(upstreamServer,'listening');
  upstreamServer.on('connection',socket=>{
    socket.send(JSON.stringify({method:'Target.attachedToTarget',params:{sessionId:'page-session',targetInfo:{targetId:'target-1',type:'page',url:'about:blank'}}}));
    socket.on('message',data=>{const command=JSON.parse(data.toString());socket.send(JSON.stringify({id:command.id,sessionId:command.sessionId,error:{code:-32000,message:'focus denied'}}));});
  });
  const gateway=await createCdpFocusGateway(`ws://127.0.0.1:${upstreamServer.address().port}/devtools/browser/test`);
  const client=new WebSocket(gateway.endpoint);
  try{
    await waitFor(client,'open');
    await waitFor(client,'message',data=>JSON.parse(data.toString()).method==='Target.attachedToTarget');
    const closed=waitFor(client,'close');
    client.send(JSON.stringify({id:19,sessionId:'page-session',method:'Emulation.setFocusEmulationEnabled',params:{enabled:true}}));
    await closed;
    const audit=gateway.snapshot();
    assert.equal(audit.rewrites[0].status,'error');
    assert.deepEqual(audit.rewrites[0].responseError,{code:-32000,message:'focus denied',data:null});
    assert.ok(audit.errors.some(item=>item.where==='focus-command-response'));
    assert.equal(assessFocusRewriteForTarget(audit,'target-1').ready,false);
  }finally{await assert.rejects(gateway.close(),/protocol or cleanup error/);}
});

test('page-target focus proof rejects absent, unresolved, non-page, pending, and unacknowledged rewrites',()=>{
  assert.equal(assessFocusRewriteForTarget({rewrites:[]},'target-1').ready,false);
  for(const target of [null,{targetId:'worker',type:'worker',url:'worker.js'}]){
    const snapshot={rewrites:[{rewritten:true,status:'acknowledged',target}]};
    assert.equal(assessFocusRewriteForTarget(snapshot,'target-1').ready,false);
  }
  for(const status of ['pending','forwarded-without-rewrite','error']){
    const snapshot={rewrites:[{rewritten:true,status,target:{targetId:'target-1',type:'page'}}]};
    assert.equal(assessFocusRewriteForTarget(snapshot,'target-1').ready,false);
  }
});

test('malformed controller messages are recorded and close the protocol session',async()=>{
  const upstreamServer=new WebSocketServer({host:'127.0.0.1',port:0});servers.push(upstreamServer);
  await waitFor(upstreamServer,'listening');
  const gateway=await createCdpFocusGateway(`ws://127.0.0.1:${upstreamServer.address().port}/devtools/browser/test`);
  const client=new WebSocket(gateway.endpoint);
  try {
    await waitFor(client,'open');
    const closed=waitFor(client,'close');
    client.send('{');
    await closed;
    assert.ok(gateway.snapshot().errors.some(item=>item.where==='client-invalid-json'));
  } finally {
    await assert.rejects(gateway.close(),/protocol or cleanup error/);
    await assert.rejects(gateway.close(),/protocol or cleanup error/);
  }
});

test('controller queue has a hard byte limit while the owned Chrome handshake is pending',async()=>{
  const stalled=createServer();servers.push(stalled);
  await new Promise(resolve=>stalled.listen(0,'127.0.0.1',resolve));
  stalled.on('upgrade',(_request,socket)=>{stalledSockets.add(socket);setTimeout(()=>socket.destroy(),350);});
  const gateway=await createCdpFocusGateway(`ws://127.0.0.1:${stalled.address().port}/devtools/browser/stalled`,
    {handshakeTimeoutMs:1000,maxQueueBytes:64,closeTimeoutMs:500});
  const client=new WebSocket(gateway.endpoint);
  try {
    await waitFor(client,'open');
    const closed=waitFor(client,'close');
    client.send(JSON.stringify({id:1,method:'Runtime.evaluate',params:{expression:'x'.repeat(100)}}));
    await closed;
    assert.ok(gateway.snapshot().errors.some(item=>item.where==='upstream-queue-limit'));
  } finally {await assert.rejects(gateway.close(),/protocol or cleanup error/);}
});

test('an unresponsive upstream websocket handshake is bounded and audited',async()=>{
  const stalled=createServer();servers.push(stalled);
  await new Promise(resolve=>stalled.listen(0,'127.0.0.1',resolve));
  stalled.on('upgrade',(_request,socket)=>stalledSockets.add(socket));
  const gateway=await createCdpFocusGateway(`ws://127.0.0.1:${stalled.address().port}/devtools/browser/stalled`,
    {handshakeTimeoutMs:100,closeTimeoutMs:500});
  const client=new WebSocket(gateway.endpoint);
  try {
    await waitFor(client,'open');
    await waitFor(client,'close');
    assert.ok(gateway.snapshot().errors.some(item=>item.where==='browser-socket'));
  } finally {await assert.rejects(gateway.close(),/protocol or cleanup error/);}
});
