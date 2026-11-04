// Adapt the relay's ordered binary stream to the existing confirmed-history reader.
async function inflate(bytes){
  const reader=new Blob([bytes]).stream().pipeThrough(new DecompressionStream('gzip')).getReader(),parts=[];let size=0;
  for(;;){const {done,value}=await reader.read();if(done)break;size+=value.length;if(size>1024*1024){await reader.cancel();throw Error('观战数据过大');}parts.push(value);}
  return new TextDecoder().decode(await new Blob(parts).arrayBuffer());
}
export function createPc98SpectatorChannel(options,onError,WebSocketClass=WebSocket){
  const url=new URL(options.netplayUrl);url.searchParams.delete('player');url.searchParams.delete('signal');url.searchParams.set('spectator',options.netplaySpectatorId);url.searchParams.set('players',String(options.netplayPlayerCount));
  const ws=new WebSocketClass(url);ws.binaryType='arraybuffer';const rows=[];let header,end=null,pull=null,closed=false,chain=Promise.resolve();
  function deliver(){
    if(!pull||!header||closed)return;const {viewer,first}=pull;if(first>rows.length)return;pull=null;
    const data={type:'slice',viewer,header,first,rows:rows.slice(first,first+60),available:rows.length,end,points:[]};
    queueMicrotask(()=>{if(!closed)channel.onmessage?.({data});});
  }
  function fail(error){if(closed)return;channel.close();onError(error);}
  ws.onmessage=event=>{if(typeof event.data==='string')return;chain=chain.then(async()=>{
    if(closed)return;const bytes=new Uint8Array(event.data),v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
    if(bytes.length<16||bytes[0]!==0x50||bytes[1]!==0x39||bytes[2]!==0x38||bytes[3]!==0x53||bytes[4]!==1||bytes[6]!==options.netplayPlayerCount||bytes[7])throw Error('观战数据格式无效');
    const kind=bytes[5],first=v.getUint32(8,true),count=v.getUint32(12,true);
    if(first!==rows.length||end)throw Error('观战确认历史缺帧');
    if(kind===1){if(header||first||count)throw Error('观战开局数据无效');header=JSON.parse(new TextDecoder().decode(bytes.subarray(16)));}
    else if(kind===2){if(!header||count<1||count>60||first+count>216000)throw Error('观战输入范围无效');const batch=JSON.parse(await inflate(bytes.subarray(16)));if(!Array.isArray(batch)||batch.length!==count)throw Error('观战输入数量不符');rows.push(...batch);}
    else if(kind===3){if(!header||count||bytes.length!==16)throw Error('观战结束数据无效');end={frame:first,reason:'本局结束'};}
    else throw Error('未知观战数据');deliver();
  }).catch(fail);};
  ws.onerror=()=>fail(Error('观战连接失败'));
  ws.onclose=()=>{if(!closed&&!end)fail(Error('观战连接已断开'));};
  const channel={onmessage:null,postMessage(message){if(message.type==='pull'){pull=message;deliver();}},close(){if(closed)return;closed=true;ws.close();}};
  return channel;
}
