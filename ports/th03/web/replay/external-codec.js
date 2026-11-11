import {HOST_VERSION,CLOCK_BYTES} from './external-host.js';
import {sha256} from './sha256.js';
const MAGIC=new TextEncoder().encode('P98EXT02'),LEGACY_MAGIC=new TextEncoder().encode('P98EXT01'),HEADER_LIMIT=64*1024,FILE_LIMIT=256*1024*1024;
export const MAX_CLOCK_BYTES=512*1024*1024,MAX_EVENT_BYTES=32*1024*1024,MAX_DISK_BYTES=128*1024*1024;
export const MAX_DISK_DELTA_BYTES=8*1024*1024;
const DISK_PAGE_BYTES=4096;
async function gzip(bytes){return new Uint8Array(await new Response(new Blob([bytes]).stream().pipeThrough(new CompressionStream('gzip'))).arrayBuffer());}
async function inflate(bytes,expected){
  const reader=new Blob([bytes]).stream().pipeThrough(new DecompressionStream('gzip')).getReader(),parts=[];let total=0;
  for(;;){const {done,value}=await reader.read();if(done)break;total+=value.length;if(total>expected){await reader.cancel();throw Error('Replay 解压长度越界');}parts.push(value);}
  if(total!==expected)throw Error('Replay 解压长度不符');return new Uint8Array(await new Blob(parts).arrayBuffer());
}
function validAssist(game,input){
  if(!input||typeof input!=='object'||Array.isArray(input))return false;
  const integer=(value,min,max)=>Number.isSafeInteger(value)&&value>=min&&value<=max;
  if(input.address!==undefined&&!integer(input.address,-1,2**31-1)||
    input.generation!==undefined&&!integer(input.generation,0,65535))return false;
  if(game==='02')return integer(input.currentInput,0,256)&&!(input.currentInput&255)&&typeof input.focusEnabled==='boolean'&&
    integer(input.points,0,15)&&['add','keep','clear'].includes(input.touchAction)&&typeof input.unlimited==='boolean'&&
    Number.isSafeInteger(input.x)&&Number.isSafeInteger(input.y);
  if(game==='03')return integer(input.focus,0,3)&&Array.isArray(input.touch)&&input.touch.length===2&&
    input.touch.every(value=>integer(value,0,2**45-1));
  const touch=input.touch;
  return integer(input.points,0,15)&&(touch==null||typeof touch==='object'&&!Array.isArray(touch)&&
    typeof touch.active==='boolean'&&typeof touch.unlimited==='boolean'&&Number.isSafeInteger(touch.x)&&Number.isSafeInteger(touch.y));
}
function validate(header,clockBytes,diskBytes,eventBytes,events){
  if(header?.version!==HOST_VERSION||!['02','03','04'].includes(header.game)||header.mode!=='solo'||!['cn','jp'].includes(header.language)||
    !['sleep','clock'].includes(header.boundaryMode)||!['main','worker','worker-np2'].includes(header.runtime)||!Number.isSafeInteger(header.endBoundary)||header.endBoundary<1||
    !Number.isSafeInteger(header.clockCount)||header.clockCount<1||header.clockCount*CLOCK_BYTES!==clockBytes||clockBytes>MAX_CLOCK_BYTES||
    !Number.isSafeInteger(header.diskBytes)||header.diskBytes!==diskBytes||diskBytes<1||diskBytes>MAX_DISK_BYTES||
    !/^[a-f0-9]{64}$/.test(header.identity)||!/^([a-f0-9]{64})$/.test(header.diskSha256)||
    !/^[a-f0-9]{8}$/.test(header.result?.memoryHash)||!/^[a-f0-9]{64}$/.test(header.result?.memorySha256)||
    !Number.isSafeInteger(header.result?.memoryBytes)||header.result.memoryBytes<1||header.result.memoryBytes>1024*1024*1024||
    header.result.boundary!==header.endBoundary||header.result.clockCount!==header.clockCount||header.result.extraClockReads!==0||
    !['sleep','clock','exit'].includes(header.result.endReason)||
    header.result.endPhase!==undefined&&!['before-input','after-input'].includes(header.result.endPhase)||
    !Array.isArray(events)||eventBytes<2||eventBytes>MAX_EVENT_BYTES)throw Error('不支持或损坏的单人 Replay');
  const timeline=header.result.timeline;
  if(timeline!==undefined){
    if(header.boundaryMode!=='sleep'||!Number.isSafeInteger(timeline?.startBoundary)||timeline.startBoundary<1||timeline.startBoundary>header.endBoundary||
      !['manual','game-over','clear','quit','end'].includes(timeline.reason)||!Number.isSafeInteger(timeline.stage)||timeline.stage<0||timeline.stage>255||
      !Array.isArray(timeline.skipRanges)||timeline.skipRanges.length>1024)throw Error('Replay 局次信息无效');
    let previous=0;
    for(const range of timeline.skipRanges){
      if(!Number.isSafeInteger(range?.from)||!Number.isSafeInteger(range.to)||range.from<1||range.from<previous||range.to<=range.from||range.to>header.endBoundary)throw Error('Replay 跳过区间无效');
      previous=range.to;
    }
  }
  let last=-1,total=0;
  for(const row of events){
    if(!Number.isSafeInteger(row.boundary)||row.boundary<0||row.boundary<last||row.boundary>header.endBoundary||
      header.result.endPhase==='before-input'&&row.boundary===header.endBoundary||!Array.isArray(row.commands)||!row.commands.length)throw Error('Replay 输入边界无效');
    last=row.boundary;
    for(const command of row.commands){
      if(++total>1000000)throw Error('Replay 输入超过容量');
      if(command?.type==='key'){if(typeof command.code!=='string'||!/^[A-Za-z0-9]{1,24}$/.test(command.code)||typeof command.down!=='boolean')throw Error('Replay 按键命令无效');}
      else if(command?.type==='assist'){if(!validAssist(header.game,command.input))throw Error('Replay 辅助输入无效');}
      else throw Error('Replay 命令类型无效');
    }
  }
}
function diskDelta(disk,base){
  if(!(base instanceof Uint8Array)||base.length!==disk.length)throw Error('Replay 基础磁盘大小不匹配');
  const parts=[];let total=0;
  for(let at=0;at<disk.length;at+=DISK_PAGE_BYTES){
    const end=Math.min(at+DISK_PAGE_BYTES,disk.length);let changed=false;
    for(let i=at;i<end;i++)if(disk[i]!==base[i]){changed=true;break;}
    if(!changed)continue;
    total+=4+end-at;if(total>MAX_DISK_DELTA_BYTES)throw Error('Replay 开局磁盘差异超过容量');
    const page=new Uint8Array(4+end-at);new DataView(page.buffer).setUint32(0,at/DISK_PAGE_BYTES,true);
    for(let i=at;i<end;i++)page[4+i-at]=disk[i]^base[i];
    parts.push(page);
  }
  const delta=new Uint8Array(total);let at=0;for(const part of parts){delta.set(part,at);at+=part.length;}return delta;
}
function validateDelta(delta,diskBytes){
  const view=new DataView(delta.buffer,delta.byteOffset,delta.byteLength);let offset=0,previous=-1;
  while(offset<delta.length){
    if(offset+4>delta.length)throw Error('Replay 磁盘差异截断');
    const page=view.getUint32(offset,true),at=page*DISK_PAGE_BYTES;offset+=4;
    const length=Math.min(DISK_PAGE_BYTES,diskBytes-at);
    if(page<=previous||at>=diskBytes||length<1||offset+length>delta.length)throw Error('Replay 磁盘差异范围无效');
    previous=page;offset+=length;
  }
}
// Reconstruct only before starting the emulator. Never reads or writes a live DOS instance.
export async function restoreExternalReplayDisk(replay,base){
  if(replay.disk)return replay.disk.slice();
  const h=replay.header,delta=replay.diskDelta;
  if(!(base instanceof Uint8Array)||base.length!==h.diskBytes||await sha256(base)!==h.diskBaseSha256)throw Error('Replay 基础磁盘版本不匹配');
  if(!(delta instanceof Uint8Array)||delta.length>MAX_DISK_DELTA_BYTES)throw Error('Replay 磁盘差异无效');
  validateDelta(delta,base.length);
  const disk=base.slice(),view=new DataView(delta.buffer,delta.byteOffset,delta.byteLength);let offset=0;
  while(offset<delta.length){
    const at=view.getUint32(offset,true)*DISK_PAGE_BYTES,length=Math.min(DISK_PAGE_BYTES,disk.length-at);offset+=4;
    for(let i=0;i<length;i++)disk[at+i]^=delta[offset+i];offset+=length;
  }
  if(await sha256(disk)!==h.diskSha256)throw Error('Replay 启动磁盘摘要不匹配');return disk;
}
export async function encodeExternalReplay(header,{clockChunks,events,disk,baseDisk=disk}){
  const clocks=new Uint8Array(await new Blob(clockChunks).arrayBuffer()),eventRaw=new TextEncoder().encode(JSON.stringify(events));
  if(!(disk instanceof Uint8Array)||disk.length<1||disk.length>MAX_DISK_BYTES)throw Error('Replay 启动磁盘缺失或超过容量');
  const delta=diskDelta(disk,baseDisk),diskSha256=await sha256(disk),diskBaseSha256=await sha256(baseDisk);
  const eventTape=await gzip(eventRaw),diskTape=await gzip(delta),clockTape=await gzip(clocks);
  validate({...header,version:HOST_VERSION,mode:'solo',clockCount:clocks.length/CLOCK_BYTES,diskBytes:disk.length,diskSha256},clocks.length,disk.length,eventRaw.length,events);
  const payload=new Uint8Array(clockTape.length+diskTape.length+eventTape.length);payload.set(clockTape);payload.set(diskTape,clockTape.length);payload.set(eventTape,clockTape.length+diskTape.length);
  const json=new TextEncoder().encode(JSON.stringify({...header,version:HOST_VERSION,mode:'solo',compression:'gzip',clockCount:clocks.length/CLOCK_BYTES,
    clockRawBytes:clocks.length,diskBytes:disk.length,eventRawBytes:eventRaw.length,clockPayloadBytes:clockTape.length,diskPayloadBytes:diskTape.length,
    eventPayloadBytes:eventTape.length,payloadSha256:await sha256(payload),diskSha256,diskEncoding:'xor-pages/1',diskBaseSha256,diskDeltaRawBytes:delta.length}));
  if(json.length>HEADER_LIMIT||json.length+payload.length+12>FILE_LIMIT)throw Error('Replay 超过文件容量限制');
  const prefix=new Uint8Array(12);prefix.set(MAGIC);new DataView(prefix.buffer).setUint32(8,json.length,true);
  return new Blob([prefix,json,payload],{type:'application/octet-stream'});
}
export async function decodeExternalReplay(file,expectedGame){
  if(file.size<12||file.size>FILE_LIMIT)throw Error('Replay 文件大小无效');
  const prefix=new Uint8Array(await file.slice(0,12).arrayBuffer()),compact=MAGIC.every((value,index)=>value===prefix[index]);
  if(!compact&&!LEGACY_MAGIC.every((value,index)=>value===prefix[index]))throw Error('不是 PC-98 单人 Replay 文件');
  const headerBytes=new DataView(prefix.buffer).getUint32(8,true);if(headerBytes>HEADER_LIMIT||headerBytes+12>=file.size)throw Error('Replay 文件头损坏');
  const header=JSON.parse(await file.slice(12,12+headerBytes).text()),payload=new Uint8Array(await file.slice(12+headerBytes).arrayBuffer());
  if(header.game!==expectedGame||header.compression!=='gzip'||!Number.isSafeInteger(header.clockRawBytes)||header.clockRawBytes<1||header.clockRawBytes>MAX_CLOCK_BYTES||
    !Number.isSafeInteger(header.eventRawBytes)||header.eventRawBytes<2||header.eventRawBytes>MAX_EVENT_BYTES||!Number.isSafeInteger(header.diskBytes)||header.diskBytes<1||header.diskBytes>MAX_DISK_BYTES||
    ![header.clockPayloadBytes,header.diskPayloadBytes,header.eventPayloadBytes].every(size=>Number.isSafeInteger(size)&&size>0&&size<=payload.length)||
    header.clockPayloadBytes+header.diskPayloadBytes+header.eventPayloadBytes!==payload.length||await sha256(payload)!==header.payloadSha256)throw Error('Replay 数据损坏或作品不匹配');
  if(compact&&(header.diskEncoding!=='xor-pages/1'||!/^[a-f0-9]{64}$/.test(header.diskBaseSha256)||
    !Number.isSafeInteger(header.diskDeltaRawBytes)||header.diskDeltaRawBytes<0||header.diskDeltaRawBytes>MAX_DISK_DELTA_BYTES))throw Error('Replay 磁盘差异无效');
  if(!compact&&header.diskEncoding!==undefined)throw Error('Replay 磁盘格式不匹配');
  let offset=0;const clocks=await inflate(payload.subarray(offset,offset+=header.clockPayloadBytes),header.clockRawBytes);
  const diskData=await inflate(payload.subarray(offset,offset+=header.diskPayloadBytes),compact?header.diskDeltaRawBytes:header.diskBytes);
  const eventBytes=await inflate(payload.subarray(offset),header.eventRawBytes),events=JSON.parse(new TextDecoder().decode(eventBytes));
  validate(header,clocks.length,header.diskBytes,eventBytes.length,events);
  if(compact)validateDelta(diskData,header.diskBytes);
  else if(await sha256(diskData)!==header.diskSha256)throw Error('Replay 启动磁盘摘要不匹配');
  return {header,clocks,events,disk:compact?null:diskData,diskDelta:compact?diskData:null};
}
