// TH12 1.00b profile formats, reconstructed from the retail executable.
// Score:43cf90/43d140/43d2e0; records:43ce00/43ceb0/43cdb0;
// configuration:423250/42feb0/4978b0. No retail profile is bundled here.
export const NATIVE_PROFILE_PATHS=Object.freeze({config:'/savesth12/th12.cfg',score:'/savesth12/scoreth12.dat',web:'/savesth12/web-profile.json'});
export const NATIVE_MUSIC_IDS=Object.freeze(['th12_01','th12_00','th12_02','th12_04','th12_05','th12_07','th12_08','th12_09','th12_10','th12_13','th12_14','th12_16','th12_17','th12_18','th12_19','th12_20','th12_21','th10_17']);
export const NATIVE_SCORE_LAYOUT=Object.freeze({headerSize:24,recordSize:0x45f4,statusSize:0x448,players:7,spellCount:113,spellOffset:0x664,spellStride:0x90,practiceOffset:0x5a4});
const CR=0x5243,ST=0x5453,RECORD_SIZE=0x45f4,STATUS_SIZE=0x448,MAX_BYTES=2*1024*1024;
const joyOffsets=Object.freeze({shoot:4,bomb:6,focus:8,pause:10,skip:20});
const defaultJoy=Object.freeze({shoot:0,bomb:1,focus:2,pause:4,skip:3});
const bytesOf=value=>value instanceof Uint8Array?value:new Uint8Array(value);
const viewOf=value=>new DataView(value.buffer,value.byteOffset,value.byteLength);
const bounded=(value,max=0x7fffffff)=>Math.min(max,Math.max(0,Math.trunc(Number.isFinite(value)?value:0)));
const machineKey=(player,difficulty)=>`${player}:${difficulty}`;
const count=value=>bounded(value,99999);
const scoreStored=run=>bounded(Number.isInteger(run.scoreStored)?run.scoreStored:Math.trunc((run.score??0)/10),999999999);
const difficultyOf=id=>id>=100?4:id<2?id+2:id>=54&&id<=56?id-53:id>=93&&id<=95?id-92:id<54?(id-2)%4:id<77?(id-57)%4:id<93?(id-77)%4:(id-96)%4;
const sjisDecoder=new TextDecoder('shift-jis');
const readText=bytes=>sjisDecoder.decode(bytes.subarray(0,bytes.indexOf(0)<0?bytes.length:bytes.indexOf(0))).replace(/ +$/,'');
let sjisCharacters;
function encodedCharacter(character){
  const code=character.codePointAt(0);if(code<128)return [code];
  if(!sjisCharacters){
    sjisCharacters=new Map();const decoder=new TextDecoder('shift-jis',{fatal:true});
    const add=bytes=>{try{const text=decoder.decode(Uint8Array.from(bytes));if([...text].length===1&&!sjisCharacters.has(text))sjisCharacters.set(text,bytes);}catch{}};
    for(let value=0x80;value<256;value++)add([value]);
    for(let lead=0x81;lead<=0xfc;lead++)if(lead<=0x9f||lead>=0xe0)for(let trail=0x40;trail<=0xfc;trail++)if(trail!==0x7f)add([lead,trail]);
  }
  // Browser-only Unicode remains in web-profile.json if Shift-JIS has no glyph.
  return sjisCharacters.get(character)??[63];
}
function writeText(bytes,offset,length,text,{pad=0,max=length-1}={}){
  bytes.fill(pad,offset,offset+length);let written=0;
  for(const character of String(text??'').split('\0')[0]){const encoded=encodedCharacter(character);if(written+encoded.length>max)break;bytes.set(encoded,offset+written);written+=encoded.length;}
  if(written<length)bytes[offset+written]=0;
}
function checksum(bytes){let total=0;for(let i=8;i<bytes.length;i++)total=(total+bytes[i])>>>0;return total;}
function finishRecord(bytes){viewOf(bytes).setUint32(4,checksum(bytes),true);return bytes;}

// 4639d0 and its inverse463af0. Small final tails and an odd final byte are
// deliberately not transformed; the rolling XOR key continues between blocks.
function transformScore(input,encrypt=false){
  const out=input.slice(),block=16,remainder=input.length%block;
  let remaining=input.length-(remainder<4?remainder:0)-(input.length&1),cursor=0,key=0xac;
  while(remaining>0){const length=Math.min(block,remaining);let at=0;
    for(let parity=1;parity<=2;parity++)for(let pos=length-parity;pos>=0;pos-=2){
      if(encrypt)out[cursor+at++]=input[cursor+pos]^key;else out[cursor+pos]=input[cursor+at++]^key;
      key=(key+0x35)&255;
    }
    cursor+=length;remaining-=length;
  }
  return out;
}
// 44c710 uses MSB-first literals and an8192-byte circular dictionary. A zero
// 13-bit offset terminates the stream. At most two implicit zero bytes are
// needed for the native decoder's end-of-input zero padding.
function inflateScore(input,size){
  if(!Number.isInteger(size)||size<12||size>MAX_BYTES)throw Error('成绩解压长度无效');
  const output=new Uint8Array(size),dictionary=new Uint8Array(8192);
  let cursor=0,mask=128,byte=0,written=0,head=1,padding=0;
  const bit=()=>{if(mask===128){if(cursor>=input.length){if(++padding>2)throw Error('成绩压缩流不完整');byte=0;}else byte=input[cursor++];}const value=(byte&mask)?1:0;mask>>=1;if(!mask)mask=128;return value;};
  const bits=length=>{let value=0;while(length--)value=value*2+bit();return value;};
  const put=value=>{if(written>=size)throw Error('成绩解压越界');output[written++]=value;dictionary[head]=value;head=(head+1)&8191;};
  for(;;){if(bit())put(bits(8));else{const offset=bits(13);if(!offset)break;const length=bits(4)+3;for(let index=0;index<length;index++)put(dictionary[(offset+index)&8191]);}}
  if(written!==size)throw Error('成绩解压长度与文件头不符');return output;
}
// A literal-only stream is a valid44c710 stream. Keeping the encoder simple
// also avoids changing preserved fields to match a particular compressor.
function compressScore(input){
  const out=new Uint8Array(Math.ceil((input.length*9+14)/8));let bit=0;
  for(const value of input){out[bit>>>3]|=1<<(7-(bit&7));bit++;for(let shift=7;shift>=0;shift--,bit++)out[bit>>>3]|=((value>>>shift)&1)<<(7-(bit&7));}
  return out;
}

export function createNativeConfig(){
  const bytes=new Uint8Array(60),view=viewOf(bytes);view.setUint32(0,0x120001,true);
  [0,1,2,4,-1,-1,-1,-1,3].forEach((value,index)=>view.setInt16(4+index*2,value,true));
  view.setUint16(0x16,600,true);view.setUint16(0x18,600,true);
  bytes[0x1b]=bytes[0x1c]=1;bytes[0x1f]=bytes[0x23]=2;bytes[0x20]=100;bytes[0x21]=80;
  view.setUint32(0x28,0x80000000,true);view.setUint32(0x2c,0x80000000,true);view.setUint32(0x38,0x100,true);
  return decodeNativeConfig(bytes);
}
export function decodeNativeConfig(source){
  const bytes=bytesOf(source).slice();if(bytes.length!==60)throw Error('th12.cfg 必须为60字节');
  const view=viewOf(bytes);if(view.getUint32(0,true)!==0x120001)throw Error('th12.cfg 版本无效');
  // These are exactly the six mode checks in42fefa..42ff2c.
  for(const [offset,limit] of [[0x1a,2],[0x1b,3],[0x1c,2],[0x1d,4],[0x1e,3],[0x1f,3]])if(bytes[offset]>=limit)throw Error('th12.cfg 模式字段无效');
  const joyButtons={};for(const [key,offset] of Object.entries(joyOffsets)){const value=view.getInt16(offset,true);joyButtons[key]=value>=-1&&value<32?value:defaultJoy[key];}
  return {kind:'config',bytes,settings:{bgmVolume:Math.min(100,bytes[0x20]),seVolume:Math.min(100,bytes[0x21]),joyButtons}};
}
export function encodeNativeConfig(settings={},native=createNativeConfig()){
  const bytes=native.bytes.slice(),view=viewOf(bytes),original=native.settings;
  for(const [key,offset] of [['bgmVolume',0x20],['seVolume',0x21]])if(Number.isFinite(settings[key])&&settings[key]!==original[key])bytes[offset]=bounded(settings[key],100);
  for(const [key,offset] of Object.entries(joyOffsets)){const value=settings.joyButtons?.[key];if(Number.isInteger(value)&&value>=-1&&value<32&&value!==original.joyButtons[key])view.setInt16(offset,value,true);}
  return bytes;
}

function createRecord(player){
  const bytes=new Uint8Array(RECORD_SIZE),view=viewOf(bytes);
  view.setUint16(0,CR,true);view.setUint16(2,2,true);view.setUint32(8,RECORD_SIZE,true);view.setUint32(12,player,true);
  for(let difficulty=0;difficulty<5;difficulty++)for(let rank=0;rank<10;rank++){
    const at=0x10+(difficulty*10+rank)*28;view.setInt32(at,100000-rank*10000,true);bytes[at+4]=1;writeText(bytes,at+6,9,'--------');
  }
  for(let id=0;id<113;id++){const at=0x664+id*0x90;view.setUint32(at+0x88,id,true);view.setUint32(at+0x8c,difficultyOf(id),true);}
  return finishRecord(bytes);
}
function createStatus(){
  const bytes=new Uint8Array(STATUS_SIZE),view=viewOf(bytes);view.setUint16(0,ST,true);view.setUint16(2,2,true);view.setUint32(8,STATUS_SIZE,true);writeText(bytes,12,9,'        ',{pad:32,max:8});
  // 43cf29..43cf77 fills512 reserved words using the game's16-bit RNG. Its
  // seed depends on startup time; zero is a reproducible legal initial seed.
  let seed=0;for(let at=0x46;at<0x446;at+=2){const value=((seed^0x9630)-0x6553)&65535;seed=((value<<2)|(value>>>14))&65535;view.setUint16(at,seed,true);}
  return finishRecord(bytes);
}
export function createNativeScore(){
  const header=new Uint8Array(24),view=viewOf(header);view.setUint32(0,0x31324854,true);view.setUint16(8,2,true);view.setUint32(12,0x100,true);
  const native={kind:'score',header,records:Array.from({length:7},(_,player)=>createRecord(player)),status:createStatus(),blocks:[],trailing:new Uint8Array()};
  native.data=projectNativeScore(native);return native;
}
export function decodeNativeScore(source){
  const bytes=bytesOf(source);if(bytes.length<24||bytes.length>MAX_BYTES)throw Error('scoreth12.dat 长度无效');
  const view=viewOf(bytes);if(view.getUint32(0,true)!==0x31324854||view.getUint16(8,true)!==2)throw Error('需要 TH12 的 TH21 v2 成绩文件');
  const packed=view.getUint32(16,true),size=view.getUint32(20,true),length=view.getUint32(4,true);
  if(!packed||packed>bytes.length-24||length!==packed+24)throw Error('成绩容器长度无效');
  const raw=inflateScore(transformScore(bytes.subarray(24,24+packed)),size),native=createNativeScore();
  native.header=bytes.slice(0,24);native.trailing=bytes.slice(24+packed);
  for(let offset=0;offset<raw.length;){
    if(offset>raw.length-12)throw Error('成绩记录头不完整');
    const type=viewOf(raw).getUint16(offset,true),version=viewOf(raw).getUint16(offset+2,true),length=viewOf(raw).getUint32(offset+8,true);
    if(version!==2||!((type===CR&&length===RECORD_SIZE)||(type===ST&&length===STATUS_SIZE))||length>raw.length-offset)throw Error('成绩记录类型或长度无效');
    const record=raw.slice(offset,offset+length),recordView=viewOf(record);if(recordView.getUint32(4,true)!==checksum(record))throw Error('成绩记录校验和无效');
    if(type===CR){const player=recordView.getUint32(12,true);if(player>=7)throw Error('成绩机体编号无效');native.records[player]=record;native.blocks.push({type,player,bytes:record});}
    else{native.status=record;native.blocks.push({type,bytes:record});}
    offset+=length;
  }
  native.data=projectNativeScore(native);return native;
}
function readRanking(bytes,player,difficulty,rank){
  const view=viewOf(bytes),at=0x10+(difficulty*10+rank)*28,stored=view.getInt32(at,true),finalStage=bytes[at+4],continues=bytes[at+5];
  const stamp=view.getBigInt64(at+16,true),timestamp=Number(stamp),date=timestamp!==0&&Number.isSafeInteger(timestamp)&&Math.abs(timestamp)<=8640000000000?new Date(timestamp*1000).toISOString():null;
  return {native:true,nativeId:`${player}:${difficulty}:${rank}`,nativeStage:finalStage,nativeTimestamp:String(stamp),nativePlaceholder:stamp===0n&&readText(bytes.subarray(at+6,at+15))==='--------',
    character:Math.floor(player/2),shot:player%2,difficulty,stage:finalStage===8?(difficulty===4?7:6):Math.max(1,Math.min(7,finalStage)),clear:finalStage===8,
    scoreStored:stored,score:stored*10+continues,continueCount:continues,name:readText(bytes.subarray(at+6,at+15)),timestamp:Number.isSafeInteger(timestamp)?timestamp:0,date,
    slowdown:view.getFloat32(at+24,true),practice:false,mode:difficulty===4?'extra':'story',ranked:true};
}
export function projectNativeScore(native){
  const data={schema:'th12-web-score/1',scores:[],spellRecords:{},clears:{},practice:{},nativePractice:{},music:[],endingsSeen:[],lastName:readText(native.status.subarray(12,21)),playSeconds:0,nativeTotals:{},nativeMachineTotals:[],nativeEndingFlags:Array.from(native.status.subarray(0x16,0x22)),nativeSpellHistory:Array(1792).fill(0)};
  for(let player=0;player<7;player++){
    const bytes=native.records[player],view=viewOf(bytes);
    if(player<6){
      const machine={playCount:view.getUint32(0x588,true),playFrames:view.getUint32(0x58c,true),clearCounts:Array.from({length:5},(_,difficulty)=>view.getUint32(0x590+difficulty*4,true))};
      data.nativeMachineTotals.push(machine);data.playSeconds+=machine.playFrames/60;
      for(let difficulty=0;difficulty<5;difficulty++){
        const key=machineKey(player,difficulty);data.nativeTotals[key]={playCount:machine.playCount,playFrames:machine.playFrames,clearCount:machine.clearCounts[difficulty]};
        data.clears[key]=machine.clearCounts[difficulty]>0;
        for(let rank=0;rank<10;rank++)data.scores.push(readRanking(bytes,player,difficulty,rank));
        if(difficulty<4){const entries=[];let maximum=0;
          for(let stage=1;stage<=6;stage++){const at=0x5a4+(difficulty*6+stage-1)*8,stored=view.getInt32(at,true);const entry={scoreStored:stored,completed:bytes[at+4]!==0,unlocked:bytes[at+5]!==0};entries.push(entry);if(entry.unlocked)maximum=stage;
            if(stored>0)data.scores.push({native:true,nativePractice:true,character:Math.floor(player/2),shot:player%2,difficulty,stage,scoreStored:stored,score:stored*10,continueCount:0,name:'',practice:true,mode:'practice',ranked:true,clear:entry.completed});
          }
          data.nativePractice[key]=entries;data.practice[key]=maximum;
        }
      }
    }
    for(let id=0;id<113;id++){
      const at=0x664+id*0x90,captures=view.getUint32(at+0x80,true),attempts=view.getUint32(at+0x84,true),title=readText(bytes.subarray(at,at+128));
      data.nativeSpellHistory[id*14+player*2]=attempts;data.nativeSpellHistory[id*14+player*2+1]=captures;
      if(attempts||captures||title)data.spellRecords[`${id}:${player}`]={attempts,captures,...(title?{title}:{})};
    }
  }
  data.nativeEndingFlags.forEach((flag,id)=>{if(flag&1)data.endingsSeen.push(id);});
  NATIVE_MUSIC_IDS.forEach((id,index)=>{if(native.status[0x26+index])data.music.push(id);});
  return data;
}

function rankingBytes(run,original){
  const bytes=original?original.slice():new Uint8Array(28),view=viewOf(bytes);
  if(!original||run.scoreStored!==view.getInt32(0,true))view.setInt32(0,scoreStored(run),true);
  const stage=run.clear?8:run.nativeStage??run.stage;if(!original||stage!==bytes[4])bytes[4]=bounded(stage,8);bytes[5]=bounded(run.continueCount??run.continues,255);
  if(!original||String(run.name??'')!==readText(original.subarray(6,15)))writeText(bytes,6,9,run.name??'PLAYER');
  const timestamp=Number.isFinite(run.timestamp)?Math.trunc(run.timestamp):run.date?Math.trunc(Date.parse(run.date)/1000):0;
  const retainedTimestamp=original&&run.nativeTimestamp===String(viewOf(original).getBigInt64(16,true))&&!run.date&&timestamp===0;
  if(!retainedTimestamp&&Number.isSafeInteger(timestamp)&&(!original||timestamp!==Number(viewOf(original).getBigInt64(16,true))))view.setBigInt64(16,BigInt(timestamp),true);
  if(Number.isFinite(run.slowdown)&&(!original||run.slowdown!==viewOf(original).getFloat32(24,true)))view.setFloat32(24,run.slowdown,true);
  return bytes;
}
const eligible=run=>run&&run.nativeEligible!==false&&run.ranked!==false&&run.mode!=='replay'&&run.mode!=='demo';
export function encodeNativeScore(data={},native=createNativeScore()){
  const records=native.records.map(bytes=>bytes.slice()),status=native.status.slice();
  for(let player=0;player<7;player++){
    const bytes=records[player],view=viewOf(bytes);
    if(player<6){
      for(let difficulty=0;difficulty<5;difficulty++){
        const key=machineKey(player,difficulty),runs=(data.scores??[]).filter(run=>run.character*2+run.shot===player&&run.difficulty===difficulty);
        const nativeRuns=Array.from({length:10},(_,rank)=>{const row=readRanking(native.records[player],player,difficulty,rank);return runs.find(run=>run.nativeId===row.nativeId)??row;});
        const additions=runs.filter(run=>!run.nativeId&&!run.practice&&run.mode!=='practice'&&eligible(run)&&!bounded(run.continueCount??run.continues));
        const ranked=[...additions,...nativeRuns].sort((a,b)=>scoreStored(b)-scoreStored(a)).slice(0,10);
        ranked.forEach((run,rank)=>{const sourceRank=run.nativeId?Number(run.nativeId.split(':')[2]):-1;const original=sourceRank>=0?native.records[player].subarray(0x10+(difficulty*10+sourceRank)*28,0x10+(difficulty*10+sourceRank+1)*28):null;bytes.set(rankingBytes(run,original),0x10+(difficulty*10+rank)*28);});
        const total=data.nativeTotals?.[key];if(Number.isFinite(total?.clearCount))view.setUint32(0x590+difficulty*4,count(total.clearCount),true);
        if(difficulty<4)for(let stage=1;stage<=6;stage++){
          const at=0x5a4+(difficulty*6+stage-1)*8,entry=data.nativePractice?.[key]?.[stage-1];
          const best=runs.filter(run=>eligible(run)&&(run.practice||run.mode==='practice')&&run.stage===stage&&!bounded(run.continueCount??run.continues)).reduce((value,run)=>Math.max(value,scoreStored(run)),view.getInt32(at,true));
          view.setInt32(at,Math.max(best,bounded(entry?.scoreStored,999999999)),true);
          if(entry){if(entry.completed&&!bytes[at+4])bytes[at+4]=1;if(entry.unlocked&&!bytes[at+5])bytes[at+5]=1;}
          else if(stage<=bounded(data.practice?.[key],6)&&!bytes[at+5])bytes[at+5]=1;
        }
      }
      const machine=data.nativeMachineTotals?.[player];
      if(machine){view.setUint32(0x588,count(machine.playCount),true);view.setUint32(0x58c,bounded(machine.playFrames,0xcdfe5ff),true);}
      else{
        const totals=Array.from({length:5},(_,difficulty)=>data.nativeTotals?.[machineKey(player,difficulty)]).filter(Boolean);
        if(totals.length){view.setUint32(0x588,count(totals.reduce((value,total)=>value+bounded(total.playCount),0)),true);view.setUint32(0x58c,bounded(totals.reduce((value,total)=>value+bounded(total.playFrames),0),0xcdfe5ff),true);}
      }
    }
    for(let id=0;id<113;id++){
      const at=0x664+id*0x90,record=data.spellRecords?.[`${id}:${player}`];
      const fallback=player===6?Array.from({length:6},(_,machine)=>data.spellRecords?.[`${id}:${machine}`]).filter(Boolean):[];
      const history=data.nativeSpellHistory,attempts=history?.[id*14+player*2]??record?.attempts??(player===6?fallback.reduce((value,item)=>value+count(item.attempts),0):undefined);
      const captures=history?.[id*14+player*2+1]??record?.captures??(player===6?fallback.reduce((value,item)=>value+count(item.captures),0):undefined);
      if(Number.isFinite(attempts))view.setUint32(at+0x84,count(attempts),true);if(Number.isFinite(captures))view.setUint32(at+0x80,count(captures),true);
      const title=record?.title??fallback.find(item=>item.title)?.title;if(typeof title==='string'&&title!==readText(bytes.subarray(at,at+128)))writeText(bytes,at,128,title);
    }
    finishRecord(bytes);
  }
  if(typeof data.lastName==='string'&&data.lastName!==readText(status.subarray(12,21)))writeText(status,12,9,data.lastName);
  if(Array.isArray(data.nativeEndingFlags))data.nativeEndingFlags.slice(0,12).forEach((flag,index)=>{status[0x16+index]=bounded(flag,255);});
  for(const id of data.endingsSeen??[])if(Number.isInteger(id)&&id>=0&&id<12){status[0x16+id]|=1;if(id>=6&&!status[0x22])status[0x22]=1;}
  // Old browser JSON used clears as no-continue unlock flags. Retail clear
  // counters also include continues, so only the legacy representation may
  // infer ending bits from that map. Native profiles update explicit flags.
  if(!Array.isArray(data.nativeEndingFlags))for(let player=0;player<6;player++)for(let difficulty=0;difficulty<4;difficulty++){
    const key=machineKey(player,difficulty);if(data.clears?.[key]){status[0x1c+player]|=1;if(difficulty>=1)status[0x1c+player]|=0x10;status[0x22]=1;}
  }
  for(const id of data.music??[]){const index=NATIVE_MUSIC_IDS.indexOf(id);if(index>=0&&!status[0x26+index])status[0x26+index]=1;}
  finishRecord(status);
  // Preserve duplicate valid records and every uninterpreted byte. The native
  // loader takes the last record of each kind, so patch only that occurrence.
  const blocks=native.blocks.map(block=>block.bytes),last=new Map();native.blocks.forEach((block,index)=>last.set(block.type===CR?'CR'+block.player:'ST',index));
  for(let player=0;player<7;player++){const index=last.get('CR'+player);if(index===undefined)blocks.push(records[player]);else blocks[index]=records[player];}
  const statusIndex=last.get('ST');if(statusIndex===undefined)blocks.push(status);else blocks[statusIndex]=status;
  const size=blocks.reduce((total,block)=>total+block.length,0);if(size>MAX_BYTES)throw Error('成绩文件超过原版缓冲区');
  const raw=new Uint8Array(size);let offset=0;for(const block of blocks){raw.set(block,offset);offset+=block.length;}
  const packed=transformScore(compressScore(raw),true),out=new Uint8Array(24+packed.length+native.trailing.length),view=viewOf(out);out.set(native.header);out.set(packed,24);out.set(native.trailing,24+packed.length);
  view.setUint32(4,24+packed.length,true);view.setUint32(16,packed.length,true);view.setUint32(20,raw.length,true);return out;
}

/** Import validation is side-effect free. Successful calls return the decoded
 * native profile; malformed or unsupported bytes throw a user-facing Error. */
export function validateNativeProfile(path,bytes){
  const name=String(path).replace(/\\/g,'/').split('/').at(-1).toLowerCase();
  if(name==='th12.cfg')return decodeNativeConfig(bytes);if(name==='scoreth12.dat')return decodeNativeScore(bytes);throw Error('不支持的 TH12 存档文件名');
}
