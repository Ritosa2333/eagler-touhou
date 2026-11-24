const memberId=/^[a-zA-Z0-9_-]{8,64}$/;
const bound=1024,maxPendingBytes=1024*1024,maxPacketBytes=4136;

// The room assigns spectator membership before this receive-only socket opens.
// The reader owns only its queue; disconnecting it cannot close either player.
export function openTh128SpectatorRelay({url,players=2,decoder,onPacket=()=>{},onError=()=>{},WebSocketClass=globalThis.WebSocket}){
  const endpoint=new URL(url),query=endpoint.searchParams;
  if(!['ws:','wss:'].includes(endpoint.protocol)||endpoint.username||endpoint.password||endpoint.hash||
    !/^th128mp-[A-Za-z0-9_-]{1,52}$/.test(query.get('room')??'')||!/^\d{1,10}$/.test(query.get('run')??'')||
    ![2,3].includes(players)||query.get('players')!==String(players)||!memberId.test(query.get('spectator')??'')||!memberId.test(query.get('member')??'')||
    ['player','signal','lobby','directory','diagnostic'].some(key=>query.has(key))||
    ['room','run','players','spectator','member'].some(key=>query.getAll(key).length!==1))throw new Error('Invalid TH128 spectator endpoint');
  if(typeof decoder?.announce!=='function'||typeof decoder?.accept!=='function'||typeof onPacket!=='function'||
    typeof onError!=='function'||typeof WebSocketClass!=='function')throw new TypeError('Spectator needs a stream decoder');
  const socket=new WebSocketClass(endpoint.href);socket.binaryType='arraybuffer';
  const {promise:ready,resolve,reject}=Promise.withResolvers();void ready.catch(()=>{});
  let closed=false,chain=Promise.resolve(),pending=0,pendingBytes=0;
  function close(){
    if(closed)return;closed=true;reject(new Error('Spectator closed before connecting'));
    if(socket.readyState<2)socket.close(1000,'TH128 spectator left');
  }
  function fail(error){if(!closed){reject(error);close();onError(error);}}
  socket.addEventListener('open',()=>{if(!closed)resolve();},{once:true});
  socket.addEventListener('error',()=>fail(new Error('TH128 spectator link failed')));
  socket.addEventListener('close',event=>fail(new Error(`TH128 spectator link closed (${event.code??0})`)),{once:true});
  socket.addEventListener('message',({data})=>{
    if(closed)return;
    const size=typeof data==='string'?data.length*2:data instanceof ArrayBuffer?data.byteLength:Infinity;
    if(size>maxPacketBytes||pending>=bound||pendingBytes+size>maxPendingBytes){fail(new Error('TH128 spectator receive queue limit'));return;}
    pending++;pendingBytes+=size;
    chain=chain.then(async()=>{
      if(closed)return;
      let kind;
      if(typeof data==='string'){
        const message=JSON.parse(data);if(message?.type!=='spectator-session')throw new Error('Unexpected TH128 spectator control');
        decoder.announce(message);kind='announcement';
      }else kind=await decoder.accept(new Uint8Array(data));
      if(!closed)await onPacket(kind);
    }).catch(fail).finally(()=>{pending--;pendingBytes-=size;});
  });
  return Object.freeze({ready,close,drained:()=>chain});
}
