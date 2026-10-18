/* Test-only, bounded startup/connection evidence, adapted from TH08's
 * validated BROWSER_EVIDENCE_INIT. Install before transport construction.
 * No timers, per-frame sampling, SDP/ICE values or binary payload capture.
 * Promise observers retain the original Promise; their rejection reactions
 * mark it handled, so unhandledrejection bookkeeping can differ.
 */
(function (root) {
  "use strict";
  let installed=null;
  function installTh10BrowserEvidence(options={}) {
  const relayHopTrace=options.relayHopTrace===undefined?false:options.relayHopTrace;
  if(typeof relayHopTrace!=='boolean')throw Error('relayHopTrace must be boolean');
  if(installed){
    if(installed.relayHopTraceEnabled!==relayHopTrace)throw Error('relayHopTrace is fixed at observer installation');
    return installed;
  }
  if (root.window && root.window !== root.window.top) return null;
  const limits={stages:16,events:64,errors:16,connections:128,sockets:16,peers:8,controlChars:16384,
    peerConnections:8,pcOperations:128};
  const data={schema:'th10-browser-evidence/1',limits,stages:[],events:[],connections:[],errors:[],droppedStages:0,droppedEvents:0,droppedErrors:0,
    droppedConnections:0,droppedSockets:0,oversizedControlMessages:0,webSocketObserverInstalled:false,
    pcObservationRevision:1,pcOperations:[],peerConnectionCount:0,droppedPcOperations:0,
    droppedPeerConnectionCalls:0,pcObserverInstalled:false,pcObserverMethods:[],relayHopTraceEnabled:relayHopTrace};
  // Keep only a bounded word prefix, never an address, SDP/ICE line or URL
  // embedded in an exception. Colons are allowed only after an error class.
  const safeDetail=value=>{
    if(typeof value!=='string')return value==null?'':'[non-text detail omitted]';
    const text=value.trim();
    const prefix=text.match(/^(?:(?:[A-Za-z]{1,40}Error|Error|Exception): ?)?[A-Za-z _'()/-]{0,160}/)?.[0]||'';
    return prefix===text?prefix:(prefix.trimEnd()?prefix.trimEnd()+' ':'')+'[details omitted]';
  };
  const failed=(where,error)=>{
    let detail='[unavailable]';try{detail=safeDetail(String(error));}catch{}
    if(data.errors.length<limits.errors)data.errors.push({where,error:detail});
    else ++data.droppedErrors;
  };
  const clock=()=>({epochMs:Date.now(),timeMs:performance.now(),timeOrigin:performance.timeOrigin});
  const integer=value=>Number.isSafeInteger(value)?value:null;
  const choice=(value,allowed)=>allowed.includes(value)?value:null;
  const socketState=socket=>socket?integer(socket.readyState):null;
  const transportSnapshot=()=>{
    try{
      const state=globalThis.__eaglerPeerTransport;
      if(!state)return {available:false};
      const peers=[],map=state.peers instanceof Map?state.peers:null;
      if(map)for(const [id,peer] of map){
        if(peers.length>=limits.peers)break;
        peers.push({player:integer(id),inputOpen:!!peer.inputOpen,controlOpen:!!peer.controlOpen,
          connectionState:choice(peer.pc?.connectionState,['new','connecting','connected','disconnected','failed','closed']),
          iceConnectionState:choice(peer.pc?.iceConnectionState,['new','checking','connected','completed','disconnected','failed','closed']),
          iceGatheringState:choice(peer.pc?.iceGatheringState,['new','gathering','complete']),
          signalingState:choice(peer.pc?.signalingState,['stable','have-local-offer','have-remote-offer','have-local-pranswer','have-remote-pranswer','closed']),
          inputState:choice(peer.inputDc?.readyState,['connecting','open','closing','closed']),
          controlState:choice(peer.controlDc?.readyState,['connecting','open','closing','closed'])});
      }
      return {available:true,localPlayer:integer(state.localPlayer),playerCount:integer(state.playerCount),
        route:choice(state.route,['rtc','relay','spectator']),failed:!!state.failed,disconnected:!!state.disconnected,
        closed:!!state.closed,error:safeDetail(state.error),rtcReadySent:!!state.rtcReadySent,
        signalState:socketState(state.signal),relayState:socketState(state.relay),
        peerCount:map?map.size:null,peers,omittedPeers:map?Math.max(0,map.size-peers.length):0};
    }catch(error){failed('transport-snapshot',error);return {available:false,readError:true};}
  };
  const snapshot=()=>({...clock(),
    visibilityState:document.visibilityState,hidden:document.hidden,hasFocus:document.hasFocus(),
    geometry:{screenX:window.screenX,screenY:window.screenY,innerWidth:innerWidth,innerHeight:innerHeight,
      outerWidth:outerWidth,outerHeight:outerHeight,devicePixelRatio:devicePixelRatio,
      screenWidth:screen.width,screenHeight:screen.height,availWidth:screen.availWidth,availHeight:screen.availHeight},
    transport:transportSnapshot()});
  const marked=new Set();
  const mark=stage=>{
    if(marked.has(stage))return;
    marked.add(stage);
    try{if(data.stages.length<limits.stages)data.stages.push({stage,...snapshot()});else ++data.droppedStages;}
    catch(error){failed(stage,error);}
  };
  const changed=event=>{
    try{if(data.events.length<limits.events)data.events.push({event:event.type,...snapshot()});else ++data.droppedEvents;}
    catch(error){failed(event.type,error);}
  };
  const sockets=new WeakMap(),relayHops=relayHopTrace?new WeakMap():null;let socketCount=0;
  let relayHopReadError=null;
  const relayHopFailed=(where,error)=>{
    if(relayHopReadError!==null)return;
    relayHopReadError=where;failed(where,error);
  };
  const connectionEvent=(socket,event,fields={})=>{
    try{
      const identity=sockets.get(socket);if(!identity)return;
      if(data.connections.length>=limits.connections){++data.droppedConnections;return;}
      const entry={...identity,event,...clock(),readyState:socketState(socket),...fields};
      // These listeners precede the transport's own handlers. Preserve its
      // state before a route message/error can close and discard every peer.
      if(event!=='message'||['route','rtc-ready','rtc-failed'].includes(fields.type))entry.transport=transportSnapshot();
      data.connections.push(entry);
    }catch(error){failed('socket-'+event,error);}
  };
  const messageTypes=new Set(['peers','peer-join','signal','rtc-ready','rtc-failed','route','ice-restart-request','peer-disconnected']);
  const controlMessage=(socket,direction,payload)=>{
    try{
      if(!sockets.has(socket)||typeof payload!=='string')return; // Never inspect binary payloads.
      if(data.connections.length>=limits.connections){++data.droppedConnections;return;}
      if(payload.length>limits.controlChars){++data.oversizedControlMessages;return;}
      let message;try{message=JSON.parse(payload);}catch{return;}
      if(!message||typeof message!=='object'||!messageTypes.has(message.type))return;
      const fields={direction,type:message.type};
      if(message.type==='route')fields.mode=choice(message.mode,['rtc','relay']);
      if(message.type==='rtc-failed'){
        if(typeof message.reason==='string')fields.reason=safeDetail(message.reason);
        if(typeof message.error==='string')fields.error=safeDetail(message.error);
      }
      connectionEvent(socket,'message',fields);
    }catch(error){failed('socket-control-observation',error);}
  };
  // Reuse the existing listener. Native WebSocket non-text data is an opaque
  // binary object; neither its bytes, length, headers nor Blob contents are read.
  // Counts belong to a socket lifetime, and only accrue while it is state.relay.
  const binaryMessage=(socket,payload)=>{
    if(typeof payload!=='object'||payload===null||relayHopReadError!==null)return;
    try{
      const identity=sockets.get(socket);
      if(identity?.lane!=='gameplay'||globalThis.__eaglerPeerTransport?.relay!==socket)return;
      const hop=relayHops.get(socket),at=performance.now();
      if(!hop||!Number.isFinite(at)||at<0||
        hop.lastBinaryReceivePerformanceMs!==null&&at<hop.lastBinaryReceivePerformanceMs||
        !Number.isSafeInteger(hop.binaryMessagesReceived+1))throw Error('invalid relay receive observation');
      ++hop.binaryMessagesReceived;hop.lastBinaryReceivePerformanceMs=at;
    }catch(error){relayHopFailed('relay-hop-message',error);}
  };
  const readRelayHop=()=>{
    if(!relayHopTrace)return null; // No clock, socket or transport read while OFF.
    const result={schema:'th10-browser-relay-hop/1',enabled:true,valid:relayHopReadError===null,
      available:false,socketId:null,player:null,route:null,readyState:null,
      binaryMessagesReceived:null,lastBinaryReceivePerformanceMs:null};
    if(!result.valid)return result;
    try{
      if(!data.webSocketObserverInstalled)throw Error('relay socket observer unavailable');
      const state=globalThis.__eaglerPeerTransport;
      result.route=choice(state?.route,['rtc','relay','spectator']);
      const socket=state?.relay;
      if(!socket)return result; // Known absence is not an observed zero.
      result.available=true;
      const identity=sockets.get(socket),hop=relayHops.get(socket);
      if(!identity||identity.lane!=='gameplay'||!hop||identity.player===null)
        throw Error('current relay socket was not observed');
      result.socketId=identity.socketId;result.player=identity.player;
      result.readyState=socketState(socket);
      if(result.readyState===null||result.readyState<0||result.readyState>3)
        throw Error('invalid relay socket ready state');
      result.binaryMessagesReceived=hop.binaryMessagesReceived;
      result.lastBinaryReceivePerformanceMs=hop.lastBinaryReceivePerformanceMs;
    }catch(error){result.valid=false;relayHopFailed('relay-hop-read',error);}
    return result;
  };
  const observeSocket=socket=>{
    const query=new URL(socket.url).searchParams;
    if(query.has('lobby')||!query.has('player')||!query.has('players'))return;
    if(socketCount>=limits.sockets){++data.droppedSockets;return;}
    const player=query.get('player');
    sockets.set(socket,{socketId:++socketCount,lane:query.get('signal')==='1'?'signal':'gameplay',
      player:/^[0-2]$/.test(player)?Number(player):null});
    if(relayHopTrace)relayHops.set(socket,{binaryMessagesReceived:0,lastBinaryReceivePerformanceMs:null});
    connectionEvent(socket,'create');
    socket.addEventListener('open',()=>connectionEvent(socket,'open'));
    socket.addEventListener('error',()=>connectionEvent(socket,'error'));
    socket.addEventListener('close',event=>connectionEvent(socket,'close',
      {code:event.code,wasClean:event.wasClean,reason:safeDetail(event.reason)}));
    socket.addEventListener('message',event=>{
      const payload=event.data;
      if(relayHopTrace)binaryMessage(socket,payload);
      controlMessage(socket,'receive',payload);
    });
    const originalSend=socket.send;
    Object.defineProperty(socket,'send',{configurable:true,writable:true,value:new Proxy(originalSend,{
      apply(target,thisArg,args){
        // The original call executes once, with its original receiver/args.
        // Throws pass through unchanged; observations cannot replace returns.
        const result=Reflect.apply(target,thisArg,args);
        controlMessage(thisArg,'send',args[0]);
        return result;
      }
    })});
  };
  // Observe the original Promise through a separate reaction; never substitute
  // it, await it here, or inspect SDP/ICE arguments and fulfillment values.
  // PC identity storage and operation records both have independent bounds.
  const observedPcs=new Map();
  const pcObservationFailed=(where,error)=>{try{failed(where,error);}catch{}};
  const pcSnapshot=pc=>{
    try{
      const transport=globalThis.__eaglerPeerTransport;
      let player=null,visited=0;
      if(transport?.peers instanceof Map)for(const [id,peer] of transport.peers){
        if(++visited>limits.peers)break;
        if(peer.pc===pc){player=integer(id);break;}
      }
      return {player,route:choice(transport?.route,['rtc','relay','spectator']),
        signalState:socketState(transport?.signal),
        connectionState:choice(pc.connectionState,['new','connecting','connected','disconnected','failed','closed']),
        iceConnectionState:choice(pc.iceConnectionState,['new','checking','connected','completed','disconnected','failed','closed']),
        iceGatheringState:choice(pc.iceGatheringState,['new','gathering','complete']),
        signalingState:choice(pc.signalingState,['stable','have-local-offer','have-remote-offer','have-local-pranswer','have-remote-pranswer','closed']),
        localDescriptionType:choice(pc.localDescription?.type,['offer','answer','pranswer','rollback']),
        remoteDescriptionType:choice(pc.remoteDescription?.type,['offer','answer','pranswer','rollback'])};
    }catch(error){pcObservationFailed('pc-snapshot',error);return {readError:true};}
  };
  const beginPcOperation=(method,pc)=>{
    try{
      if(data.pcOperations.length>=limits.pcOperations){++data.droppedPcOperations;return null;}
      if(pc===null||(typeof pc!=='object'&&typeof pc!=='function'))return null;
      let pcId=observedPcs.get(pc);
      if(!pcId){
        if(observedPcs.size>=limits.peerConnections){++data.droppedPeerConnectionCalls;return null;}
        pcId=observedPcs.size+1;observedPcs.set(pc,pcId);data.peerConnectionCount=observedPcs.size;
      }
      const entry={operationId:data.pcOperations.length+1,pcId,method,
        call:{...clock(),state:pcSnapshot(pc)},outcome:'pending',settled:null};
      data.pcOperations.push(entry);return entry;
    }catch(error){pcObservationFailed('pc-operation-call',error);return null;}
  };
  const endPcOperation=(entry,pc,outcome,error)=>{
    if(!entry)return;
    try{
      entry.outcome=outcome;entry.settled={...clock(),state:pcSnapshot(pc)};
      if(outcome==='rejected'||outcome==='threw'){
        let detail='[unavailable]';try{detail=safeDetail(String(error));}catch{}
        entry.error=detail;
      }
    }catch(observerError){pcObservationFailed('pc-operation-settled',observerError);}
  };
  const evidence={mark,read:()=>data,relayHopTraceEnabled:relayHopTrace,readRelayHop};
  installed=evidence;
  root.__th10BrowserEvidence=evidence;
  try{
    const NativeWebSocket=globalThis.WebSocket;
    const ObservedWebSocket=new Proxy(NativeWebSocket,{construct(target,args,newTarget){
      const socket=Reflect.construct(target,args,newTarget);
      try{observeSocket(socket);}catch(error){
        if(relayHopTrace)relayHopFailed('socket-observer-install',error);
        else failed('socket-observer-install',error);
      }
      return socket;
    }});
    globalThis.WebSocket=ObservedWebSocket;
    if(globalThis.WebSocket!==ObservedWebSocket)throw Error('WebSocket observer installation failed');
    data.webSocketObserverInstalled=true;
  }catch(error){failed('websocket-install',error);}
  try{
    const prototype=globalThis.RTCPeerConnection?.prototype;
    const nativeThen=Promise.prototype.then;
    if(!prototype)throw Error('RTCPeerConnection unavailable');
    for(const method of ['createOffer','createAnswer','setLocalDescription','setRemoteDescription']){
      const descriptor=Object.getOwnPropertyDescriptor(prototype,method);
      if(!descriptor||typeof descriptor.value!=='function')throw Error('RTC operation unavailable: '+method);
      const observed=new Proxy(descriptor.value,{apply(target,thisArg,args){
        const entry=beginPcOperation(method,thisArg);
        let result;
        try{result=Reflect.apply(target,thisArg,args);}
        catch(error){endPcOperation(entry,thisArg,'threw',error);throw error;}
        if(entry)try{
          const observer=Reflect.apply(nativeThen,result,[
            ()=>endPcOperation(entry,thisArg,'fulfilled'),
            error=>endPcOperation(entry,thisArg,'rejected',error)
          ]);
          // Catch failures of either diagnostic reaction on its own branch.
          // The caller receives precisely the native result/rejection object.
          Reflect.apply(nativeThen,observer,[undefined,error=>pcObservationFailed('pc-promise-observer',error)]);
        }catch(error){
          try{entry.outcome='unobserved';}catch{}
          pcObservationFailed('pc-promise-observe',error);
        }
        return result;
      }});
      Object.defineProperty(prototype,method,{...descriptor,value:observed});
      if(prototype[method]!==observed)throw Error('RTC operation observer installation failed');
      data.pcObserverMethods.push(method);
    }
    data.pcObserverInstalled=true;
  }catch(error){pcObservationFailed('pc-observer-install',error);}
  try{
    document.addEventListener('visibilitychange',changed);
    window.addEventListener('focus',changed);
    window.addEventListener('blur',changed);
    mark('installed');
  }catch(error){failed('install',error);}
  return evidence;
  }
  root.installTh10BrowserEvidence=installTh10BrowserEvidence;
  if (typeof module !== "undefined") module.exports={installTh10BrowserEvidence};
})(globalThis);
