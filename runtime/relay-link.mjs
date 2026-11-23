const senderEnvelope=0xe6,maxPayload=4096,maxPending=128,maxBuffered=64*1024;
const memberId=/^[a-zA-Z0-9_-]{8,64}$/;
const encoding=new TextEncoder(),decoding=new TextDecoder('utf-8',{fatal:true});

// Wrap the author's binary gameplay lane. The launcher owns room/run
// membership, and an enveloped sender seat comes from Relay, not the payload.
export function openRelayLink({url,localPlayer,players=2,onMessage,onError=()=>{},WebSocketClass=globalThis.WebSocket,loopbackLegacy=false,
  requireRoute=false,spectatorUpload=false}){
  if((![2,3].includes(players)||!Number.isInteger(localPlayer)||localPlayer<0||localPlayer>=players)||typeof onMessage!=='function'||typeof onError!=='function'||typeof WebSocketClass!=='function')
    throw new TypeError('Relay requires a local seat and message consumer');
  const endpoint=new URL(url);
  if(!['ws:','wss:'].includes(endpoint.protocol)||endpoint.username||endpoint.password||endpoint.hash||
    !/^th128mp-[A-Za-z0-9_-]{1,52}$/.test(endpoint.searchParams.get('room')??'')||
    !/^[A-Za-z0-9_-]{1,64}$/.test(endpoint.searchParams.get('run')??'')||
    endpoint.searchParams.get('player')!==String(localPlayer)||endpoint.searchParams.get('players')!==String(players)||
    ['spectator','signal','lobby','directory','diagnostic'].some(key=>endpoint.searchParams.has(key)))
    throw new Error('Invalid TH128 gameplay Relay seat or endpoint');
  if(loopbackLegacy&&(players!==2||!['127.0.0.1','[::1]','localhost'].includes(endpoint.hostname)))
    throw new Error('Legacy two-seat Relay is restricted to loopback');
  if(requireRoute&&(!memberId.test(endpoint.searchParams.get('member')??'')||loopbackLegacy))
    throw new Error('Relay route selection requires a bound room member');
  if(spectatorUpload&&(localPlayer!==0||!requireRoute))throw new Error('Spectator upload requires the routed host seat');
  const socket=new WebSocketClass(endpoint.href);
  socket.binaryType='arraybuffer';
  const {promise:ready,resolve:opened,reject:openFailed}=Promise.withResolvers();
  const {promise:routeReady,resolve:routed,reject:routeFailed}=Promise.withResolvers();
  void ready.catch(()=>{});void routeReady.catch(()=>{});
  let chain=Promise.resolve(),pending=0,closed=false,routeAccepted=!requireRoute;
  function close(){
    if(closed)return;closed=true;
    openFailed(new Error('Relay gameplay link closed before opening'));
    routeFailed(new Error('Relay gameplay link closed before route selection'));
    if(socket.readyState<2)socket.close(1000,'TH128 session closed');
  }
  function fault(error){
    if(closed)return;const problem=error instanceof Error?error:new Error(String(error));
    routeFailed(problem);close();onError(problem);
  }
  socket.addEventListener('open',()=>{if(!closed){opened();if(routeAccepted)routed();}},{once:true});
  socket.addEventListener('error',()=>fault(new Error('Relay gameplay socket error')));
  socket.addEventListener('close',event=>fault(new Error(`Relay gameplay closed (${event.code??0})`)),{once:true});
  socket.addEventListener('message',event=>{
    if(closed)return;
    if(++pending>maxPending){fault(new Error('Relay gameplay queue exhausted'));return;}
    chain=chain.then(async()=>{
      if(closed)return;
      if(requireRoute&&typeof event.data==='string'){
        if(event.data.length>1024)throw new Error('Relay gameplay control exceeds its limit');
        const control=JSON.parse(event.data);
        if(control?.type==='route'&&control.mode==='relay'&&Object.keys(control).length===2){
          routeAccepted=true;routed();return;
        }
        if(control?.type==='route'&&control.mode==='failed'&&typeof control.reason==='string')throw new Error(`Relay route failed: ${control.reason}`);
        throw new Error('Unexpected control on the Relay gameplay lane');
      }
      if(!routeAccepted)throw new Error('Gameplay arrived before Relay route selection');
      if(!(event.data instanceof ArrayBuffer)||event.data.byteLength<2||event.data.byteLength>maxPayload+2)
        throw new Error('Invalid binary Relay gameplay packet');
      let payload=new Uint8Array(event.data),remote=1-localPlayer;
      if(payload[0]===senderEnvelope){
        remote=payload[1];if(!Number.isInteger(remote)||remote<0||remote>=players||remote===localPlayer)throw new Error('Relay gameplay sender is not the bound remote seat');
        payload=payload.subarray(2);
      }else if(!loopbackLegacy)throw new Error('Relay gameplay sender envelope missing');
      if(payload.length>maxPayload)throw new Error('Relay gameplay payload exceeds its limit');
      const message=JSON.parse(decoding.decode(payload));
      if(!message||Array.isArray(message)||typeof message!=='object')throw new Error('Relay gameplay payload is not an object');
      await onMessage(message,remote);
    }).catch(fault).finally(()=>{pending--;});
  });
  function ensureSend(){
    if(closed||socket.readyState!==1)throw new Error('Relay gameplay link is not open');
    if(!routeAccepted)throw new Error('Relay gameplay route is not selected');
  }
  function boundedSend(data,bytes){
    if(socket.bufferedAmount+bytes>maxBuffered)throw new Error('Relay gameplay send buffer exhausted');
    socket.send(data);
  }
  function send(message){
    ensureSend();
    const bytes=encoding.encode(JSON.stringify(message));
    if(bytes.length>maxPayload)throw new Error('Relay gameplay payload exceeds its limit');
    boundedSend(bytes,bytes.length);
  }
  function sendSpectator(value){
    ensureSend();if(!spectatorUpload)throw new Error('This seat does not own spectator upload');
    if(value instanceof Uint8Array){
      if(value.length<33||value.length>4137||value[0]!==0xe8||value[1]!==0x45||value[2]!==0x48)
        throw new Error('Invalid TH128 spectator upload envelope');
      boundedSend(value,value.length);return;
    }
    if(value?.type!=='spectator-session'||Object.keys(value).length!==4||typeof value.session!=='string'||
      !/^[a-f0-9]{16}$/.test(value.session)||/^0+$/.test(value.session)||!Number.isInteger(value.seed)||value.seed<0||value.seed>65535||
      !Number.isSafeInteger(value.generation)||value.generation<0||value.generation>0x7ffffffe)throw new Error('Invalid TH128 spectator announcement');
    const text=JSON.stringify(value);boundedSend(text,encoding.encode(text).length);
  }
  return Object.freeze({ready,routeReady,send,sendSpectator,close,drained:()=>chain});
}
