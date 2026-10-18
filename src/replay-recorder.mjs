import {appendMotionReplay} from './replay-motion.mjs';
// TH12 1.00b recording layout:43ae80/43c590/43b7e0/43bc10.
// The original loader43c350 has no replay checksum. Its reserved checksum-
// looking header words are zero, as initialized at43aec8..43aeee.
const MAX_BYTES=64*1024*1024,END={held:0xffff,pressed:0xffff,released:0xffff};
const integer=(value,min,max,label)=>{if(!Number.isInteger(value)||value<min||value>max)throw Error(label+'无效');return value;};
const signed=value=>integer(value,-0x80000000,0x7fffffff,'录像整数字段');
const short=value=>integer(value,-0x8000,0x7fff,'录像资源字段');
const word=value=>integer(value,0,0xffff,'录像输入字段');
const isEnd=value=>value.held===0xffff&&value.pressed===0xffff&&value.released===0xffff;
const bytesOf=value=>value instanceof Uint8Array?value:new Uint8Array(value);

export function defaultReplaySpellTimes(fullscreen=false){
  // Native43b080 initializes 20 unused time words with an arithmetic marker;
  // +9c is separately a flags word, copied as word21 by the compatibility API.
  return [...Array.from({length:20},(_,i)=>Math.imul(-i,0x21522153)),fullscreen?1:0];
}

export function defaultReplayConfig(){
  // Native423250. The 9 controller action words are configuration, not keys
  // recorded in the input stream. Callers may supply their current 60 bytes.
  const bytes=new Uint8Array(60),view=new DataView(bytes.buffer);
  view.setUint32(0,0x120001,true);
  [0,1,2,4,0xffff,0xffff,0xffff,0xffff,3].forEach((n,i)=>view.setUint16(4+i*2,n,true));
  view.setUint16(0x16,600,true);view.setUint16(0x18,600,true);
  bytes[0x1b]=bytes[0x1c]=1;bytes[0x1f]=bytes[0x23]=2;bytes[0x20]=100;bytes[0x21]=80;
  view.setUint32(0x28,0x80000000,true);view.setUint32(0x2c,0x80000000,true);view.setUint32(0x38,0x100,true);
  return bytes;
}

export function encodeReplayName(name='PLAYER'){
  if(name instanceof Uint8Array){if(name.length>8||name.some(b=>!b))throw Error('录像名字无效');const out=new Uint8Array(8).fill(32);out.set(name);return out;}
  const text=String(name).split('\0')[0];
  // Native naming UI offers ASCII glyphs. Explicit Shift-JIS bytes are also
  // accepted above when a host has its own multibyte naming UI.
  if([...text].some(c=>c.charCodeAt(0)<32||c.charCodeAt(0)>126))throw Error('录像名字请使用英文字母、数字或符号');
  const out=new Uint8Array(8).fill(32);out.set(Uint8Array.from(text.slice(0,8),c=>c.charCodeAt(0)));return out;
}

// Reverse native4639d0 (native encoder463af0): shuffle/XOR by odd/even
// positions, retaining the small tail and odd last byte exactly as the game.
export function encryptReplayBytes(source,key,step,block,limit=source.length){
  const input=bytesOf(source),out=new Uint8Array(input),tail=(input.length%block<block/4?input.length%block:0)+(input.length&1);
  let cursor=0,remaining=input.length-tail;
  while(remaining>0&&limit>0){const n=Math.min(block,remaining);let at=0;
    for(let parity=1;parity<=2;parity++)for(let p=n-parity;p>=0;p-=2){out[cursor+at++]=input[cursor+p]^key;key=(key+step)&255;}
    cursor+=n;remaining-=n;limit-=n;
  }
  return out;
}

// Legal LZ stream for native44c710: every byte is a literal1+8 bits, ending
// with a zero13-bit dictionary offset. A full game is still only a few MB;
// reproducing the retail compressor's byte choices is unnecessary.
export function compressReplayLiterals(source){
  const input=bytesOf(source);if(input.length>MAX_BYTES)throw Error('录像超过保存大小限制');
  const out=new Uint8Array(Math.ceil((input.length*9+14)/8));let bit=0;
  for(const value of input){out[bit>>>3]|=1<<(7-(bit&7));bit++;
    for(let shift=7;shift>=0;shift--,bit++)out[bit>>>3]|=((value>>>shift)&1)<<(7-(bit&7));}
  return out;
}

function initialHeader(initial){
  if(!initial||!Array.isArray(initial.ufoColors)||initial.ufoColors.length!==3||!Array.isArray(initial.nativeGlobals)||initial.nativeGlobals.length!==4)throw Error('录像开局记录无效');
  const copy={};for(const key of ['score','pointValue','rank','x','y'])copy[key]=signed(initial[key]);
  for(const key of ['power','lives','lifeFragments','bombs','bombFragments'])copy[key]=short(initial[key]);
  copy.ufoColors=initial.ufoColors.map(signed);copy.nativeGlobals=initial.nativeGlobals.map(signed);return copy;
}
function normalizeStage(stage){
  const number=integer(stage?.number,1,7,'录像关卡'),seed=word(stage.seed),initial=initialHeader(stage.initial);
  const spellTimes=(stage.spellTimes??defaultReplaySpellTimes()).map(signed);if(spellTimes.length!==21)throw Error('录像符卡时间记录无效');
  if(!Array.isArray(stage.inputs)||stage.inputs.length>10000000)throw Error('录像输入记录无效');
  const inputs=stage.inputs.map((input,index)=>{const r={held:word(input?.held),pressed:word(input?.pressed),released:word(input?.released)};
    if(isEnd(r)&&index!==stage.inputs.length-1)throw Error('录像结束标记后还有输入');return r;});
  if(stage.end&&!inputs.some(isEnd))inputs.push({...END});
  if(!inputs.length)throw Error('录像没有输入记录');
  const playable=inputs.length-(isEnd(inputs.at(-1))?1:0),count=Math.ceil(playable/30);
  const fpsSource=stage.fps??stage.fpsSamples??[];
  if(!Array.isArray(fpsSource)&&!(fpsSource instanceof Uint8Array))throw Error('录像帧率记录无效');
  if(fpsSource.length!==count)throw Error('录像帧率数量与输入不符');
  const fps=Uint8Array.from(fpsSource,value=>integer(value,0,255,'录像帧率'));
  return {number,seed,initial,spellTimes,inputs,fps,payload:inputs.length*6+fps.length};
}

function userBlock(text,type=0){
  const value=new TextEncoder().encode(text+'\0'),size=(12+value.length+3)&~3,block=new Uint8Array(size),view=new DataView(block.buffer);
  view.setUint32(0,0x52455355,true);view.setUint32(4,size,true);block[8]=type;block.set(value,12);return block;
}

export function encodeReplay({name='PLAYER',character,shot,difficulty,stages,timestamp=Math.floor(Date.now()/1000),scoreStored,slowdown=0,continues=0,finalStage,cleared=false,practice=false,configBytes=defaultReplayConfig(),includeUserBlocks=true}={}){
  character=integer(character,0,2,'录像机体');shot=integer(shot,0,1,'录像装备');difficulty=integer(difficulty,0,4,'录像难度');
  if(!Array.isArray(stages)||stages.length<1||stages.length>7)throw Error('录像关卡数量无效');
  const records=stages.map(normalizeStage);if(new Set(records.map(s=>s.number)).size!==records.length)throw Error('录像包含重复关卡');
  const size=112+records.reduce((sum,s)=>sum+160+s.payload,0);if(size>MAX_BYTES)throw Error('录像超过保存大小限制');
  const raw=new Uint8Array(size),view=new DataView(raw.buffer),cfg=bytesOf(configBytes),nameBytes=encodeReplayName(name);
  if(cfg.length!==60)throw Error('录像设置记录无效');
  const seconds=timestamp instanceof Date?Math.floor(timestamp.getTime()/1000):timestamp;
  if(!Number.isSafeInteger(seconds)||seconds<0)throw Error('录像日期无效');
  if(!Number.isFinite(slowdown)||slowdown<0||slowdown>100)throw Error('录像减速率无效');
  continues=integer(continues,0,0x7fffffff,'录像续关次数');
  const lastStage=integer(finalStage??(cleared?8:records.at(-1).number),1,8,'录像最后关卡');
  raw.set(nameBytes);raw[10]=practice?1:0;view.setBigUint64(12,BigInt(seconds),true);
  view.setInt32(20,signed(scoreStored??records.at(-1).initial.score),true);raw.set(cfg,24);
  view.setFloat32(84,slowdown,true);view.setUint32(88,records.length,true);view.setUint32(92,character,true);view.setUint32(96,shot,true);view.setUint32(100,difficulty,true);view.setUint32(104,lastStage,true);view.setInt32(108,continues,true);
  let at=112;
  for(const s of records){
    const i=s.initial;view.setUint16(at,s.number,true);view.setUint16(at+2,s.seed,true);view.setUint32(at+4,s.inputs.length,true);view.setUint32(at+8,s.payload,true);
    view.setInt32(at+12,i.score,true);view.setInt16(at+16,i.power,true);view.setInt32(at+20,i.pointValue,true);
    [i.lives,i.lifeFragments,i.bombs,i.bombFragments].forEach((v,n)=>view.setInt16(at+24+n*2,v,true));
    i.ufoColors.forEach((v,n)=>view.setInt32(at+32+n*4,v,true));view.setInt32(at+44,i.rank,true);view.setInt32(at+48,i.x,true);view.setInt32(at+52,i.y,true);
    i.nativeGlobals.forEach((v,n)=>view.setInt32(at+56+n*4,v,true));s.spellTimes.forEach((v,n)=>view.setInt32(at+76+n*4,v,true));
    s.inputs.forEach((v,n)=>{const p=at+160+n*6;view.setUint16(p,v.held,true);view.setUint16(p+2,v.pressed,true);view.setUint16(p+4,v.released,true);});raw.set(s.fps,at+160+s.inputs.length*6);at+=160+s.payload;
  }
  const compressed=compressReplayLiterals(raw),packed=encryptReplayBytes(encryptReplayBytes(compressed,0x7d,0x3a,0x40),0x5e,0xe1,0x800);
  const displayName=new TextDecoder('shift-jis').decode(nameBytes),date=new Date(seconds*1000).toISOString().replace('T',' ').slice(0,19);
  const extra=includeUserBlocks?[userBlock(`TH12 Undefined Fantastic Object\r\nVersion 1.00b\r\nName ${displayName}\r\nDate ${date}\r\nCharacter ${character}/${shot}\r\nRank ${difficulty}\r\nStage ${records[0].number}-${lastStage}\r\nScore ${(scoreStored??records.at(-1).initial.score)*10}\r\nSlow Rate ${slowdown.toFixed(2)}%\r\n`),userBlock('',1)]:[];
  const file=new Uint8Array(36+packed.length+extra.reduce((n,b)=>n+b.length,0)),header=new DataView(file.buffer);
  header.setUint32(0,0x72323174,true);header.setUint16(4,4,true);header.setUint32(12,36+packed.length,true);header.setUint32(16,0x100,true);header.setUint32(28,packed.length,true);header.setUint32(32,raw.length,true);file.set(packed,36);
  at=36+packed.length;for(const block of extra){file.set(block,at);at+=block.length;}return file;
}

export class ReplayRecorder {
  constructor(options={}){this.options={...options};this.stages=[];this.active=null;this.resultInfo={};this.previousHeld=0;}
  recordStage(header){
    if(this.active)throw Error('上一关录像尚未结束');
    const number=integer(header?.number,1,7,'录像关卡');if(this.stages.some(s=>s.number===number))throw Error('录像包含重复关卡');
    this.active={number,seed:word(header.seed),initial:initialHeader(header.initial),spellTimes:(header.spellTimes??defaultReplaySpellTimes()).map(signed),inputs:[],fps:[],motion:[]};
    if(this.active.spellTimes.length!==21){this.active=null;throw Error('录像符卡时间记录无效');}
    this.previousHeld=0;return this.active;
  }
  pushInput(input,fps=60){
    const stage=this.active;if(!stage)throw Error('尚未开始记录关卡');if(stage.inputs.length>=10000000)throw Error('录像超过输入长度限制');
    const held=word(input?.held),pressed=word(input.pressed??(held&~this.previousHeld)),released=word(input.released??(this.previousHeld&~held));
    if(isEnd({held,pressed,released}))throw Error('结束标记请通过结束录像写入');
    if(!Number.isFinite(fps)||fps<0)throw Error('录像帧率无效');
    if(stage.inputs.length%30===0)stage.fps.push(Math.min(255,Math.trunc(fps+.5)));
    stage.inputs.push({held,pressed,released});this.previousHeld=held;return stage.inputs.length;
  }
  recordMotion(dx,dy,unlimited=false){
    if(!this.active||!this.active.inputs.length)throw Error('尚未记录当前帧输入');
    if(dx||dy)this.active.motion.push([this.active.inputs.length-1,dx,dy]);
    if(unlimited){this.resultInfo.slowdown=100;this.resultInfo.unlimitedMovement=true;}
  }
  setSpellTime(index,encoded){if(!this.active)throw Error('尚未开始记录关卡');this.active.spellTimes[integer(index,0,19,'录像符卡序号')]=signed(encoded);}
  finishStage({spellTimes,end=false}={}){
    if(!this.active)return null;
    if(spellTimes!==undefined){if(!Array.isArray(spellTimes)||spellTimes.length!==21)throw Error('录像符卡时间记录无效');this.active.spellTimes=spellTimes.map(signed);}
    if(end)this.active.inputs.push({...END});
    if(!this.active.inputs.length)throw Error('录像没有输入记录');
    const stage=this.active;this.active=null;this.stages.push(stage);return stage;
  }
  result(info={}){
    const {end=true,...metadata}=info;this.resultInfo={...this.resultInfo,...metadata};
    if(this.active)this.finishStage({end});
    const file=encodeReplay({...this.options,...this.resultInfo,stages:this.stages});
    return appendMotionReplay(file,this.stages,this.resultInfo.unlimitedMovement===true,this.webExtensions);
  }
  setWebExtensions(value){this.webExtensions={...this.webExtensions,...value};}
}
