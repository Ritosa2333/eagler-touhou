// Adapt the relay's ordered binary stream to the existing confirmed-history reader.
async function inflate(bytes){
  const reader=new Blob([bytes]).stream().pipeThrough(new DecompressionStream('gzip')).getReader(),parts=[];let size=0;
  for(;;){const {done,value}=await reader.read();if(done)break;size+=value.length;if(size>1024*1024){await reader.cancel();throw Error('观战数据过大');}parts.push(value);}
  return new TextDecoder().decode(await new Blob(parts).arrayBuffer());
}
export function createPc98SpectatorChannel(options,onError,WebSocketClass=WebSocket){
  const url=new URL(options.netplayUrl);url.searchParams.delete('player');url.searchParams.delete('signal');url.searchParams.set('spectator',options.netplaySpectatorId);url.searchParams.set('players',String(options.netplayPlayerCount));
  const ws=new WebSocketClass(url);ws.binaryType='arraybuffer';const rows=[],points=[],listeners=new Set();let header,end=null,pull=null,closed=false,ready=false,point=null,chain=Promise.resolve(),rowBase=0,segment=null,initialMin=0,haveRows=false;
  function deliver(){
    if(!pull||!header||closed||segment!==null&&!haveRows||points.some(p=>p.frame>rowBase+rows.length))return;const {viewer}=pull,first=Math.max(pull.first,rowBase);if(first>rowBase+rows.length)return;pull=null;
    const data={type:'slice',viewer,header,first,min:initialMin,rows:rows.slice(first-rowBase,first-rowBase+60),available:rowBase+rows.length,end,points:points.map(({frame,sha,size})=>({frame,sha,size}))};
    queueMicrotask(()=>{if(!closed)channel.onmessage?.({data});});
  }
  function fail(error){if(closed)return;channel.close();onError(error);}
  ws.onmessage=event=>{if(typeof event.data==='string')return;chain=chain.then(async()=>{
    if(closed)return;const bytes=new Uint8Array(event.data),v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
    if(bytes.length<16||bytes[0]!==0x50||bytes[1]!==0x39||bytes[2]!==0x38||bytes[3]!==0x53||bytes[4]!==1||bytes[6]!==options.netplayPlayerCount||bytes[7])throw Error('观战数据格式无效');
    const kind=bytes[5],first=v.getUint32(8,true),count=v.getUint32(12,true);
    if(kind!==4&&kind!==6&&!(kind===2&&!haveRows&&segment!==null)&&first!==rowBase+rows.length||end&&kind!==5)throw Error('观战确认历史缺帧');
    if(kind===1){if(header||first||count)throw Error('观战开局数据无效');header=JSON.parse(new TextDecoder().decode(bytes.subarray(16)));}
    else if(kind===2){if(!header||count<1||count>60||first+count>216000)throw Error('观战输入范围无效');if(!haveRows&&segment!==null){if(first>segment||first+count<segment)throw Error('观战局输入起点无效');rowBase=first;initialMin=segment;}const batch=JSON.parse(await inflate(bytes.subarray(16)));if(!Array.isArray(batch)||batch.length!==count)throw Error('观战输入数量不符');rows.push(...batch);haveRows=true;}
    else if(kind===3){if(!header||count||bytes.length!==16)throw Error('观战结束数据无效');end={frame:first,reason:'本局结束'};}
    else if(kind===4){
      if(!header||count||first>rowBase+rows.length&&first!==segment||bytes.length<=88)throw Error('观战检查点数据无效');
      const size=v.getUint32(16,true),offset=v.getUint32(20,true),sha=new TextDecoder().decode(bytes.subarray(24,88)),chunk=bytes.subarray(88);
      if(size<1||size>16*1024*1024||offset+chunk.length>size||!/^[a-f0-9]{64}$/.test(sha))throw Error('观战检查点元数据无效');
      if(!point){if(offset||points.length&&first<=points.at(-1).frame||!points.length&&first!==0)throw Error('观战检查点顺序无效');point={frame:first,sha,size,offset:0,bytes:new Uint8Array(size)};}
      if(point.frame!==first||point.sha!==sha||point.size!==size||point.offset!==offset)throw Error('观战检查点缺块');
      point.bytes.set(chunk,offset);point.offset+=chunk.length;
      if(point.offset===size){const digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',point.bytes)),x=>x.toString(16).padStart(2,'0')).join('');if(digest!==sha)throw Error('观战检查点摘要不一致');points.push(point);point=null;}
    }
    else if(kind===5){if(!header||count||bytes.length!==16)throw Error('观战开局边界无效');ready=true;}
    else if(kind===6){if(!header||bytes.length!==16||!count||count>first)throw Error('观战局起点无效');segment=count;}
    else if(kind===7){if(!header||bytes.length!==16||!count||count>first)throw Error('观战局切换无效');}
    else throw Error('未知观战数据');deliver();
  }).catch(fail);};
  ws.onerror=()=>fail(Error('观战连接失败'));
  ws.onclose=event=>{if(!closed&&!end)fail(Error('观战连接已断开'+(event.reason?'：'+event.reason:'')));};
  const channel={onmessage:null,get ready(){return ready&&(header?.game!=='03'||header?.spectatorInputOnly===true||segment!==null);},get relay(){return ws;},
    addEventListener(type,fn){if(type==='message')listeners.add(fn);},removeEventListener(type,fn){if(type==='message')listeners.delete(fn);},
    postMessage(message){
      if(message.type==='pull'){pull=message;deliver();return;}
      if(message.type==='pull-point'){
        const found=points.find(p=>p.frame===message.frame);if(!found||message.offset<0||message.offset>=found.size)return;
        const chunk=found.bytes.subarray(message.offset,message.offset+24576);let binary='';for(const value of chunk)binary+=String.fromCharCode(value);
        const data={type:'point',viewer:message.viewer,frame:found.frame,sha:found.sha,size:found.size,offset:message.offset,payload:btoa(binary)};
        queueMicrotask(()=>{if(!closed)for(const fn of listeners)fn({data});});
      }
    },close(){if(closed)return;closed=true;listeners.clear();ws.close();}};
  return channel;
}
