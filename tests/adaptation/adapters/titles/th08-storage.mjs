import {createHash} from 'node:crypto';
import {decodeTh08Lzss,decryptTh08ScoreEnvelope} from './th08-data-codec.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');

export function inspectTh08Configuration(input){
  const raw=Buffer.from(input);
  if(raw.length!==60||raw.readUInt32LE(20)!==0x80001)throw Error('Invalid th08 configuration size or version');
  const values={slowMode:raw[0x25],musicVolume:raw.readInt8(0x27),soundVolume:raw.readInt8(0x28)};
  const limits=[7,4,2,3,2,6,2,3,3,2,2];
  if(limits.some((limit,index)=>raw[28+index]>=limit))
    throw Error('Invalid th08 configuration values');
  return{kind:'configuration',schema:'th08/configuration/1',values,semanticSha256:hash(raw)};
}

export function inspectTh08Score(input){
  const raw=Buffer.from(input);
  if(raw.length<28||raw.length>4*1024*1024)throw Error('Invalid th08 score size');
  const plain=decryptTh08ScoreEnvelope(raw);
  let checksum=0;for(let index=4;index<plain.length;index++)checksum=(checksum+plain[index])&0xffff;
  const payloadSize=plain.readUInt32LE(20),compressedSize=plain.readUInt32LE(24);
  if(plain.readUInt16LE(2)!==checksum||plain.readUInt16LE(4)!==1||plain.readUInt32LE(8)!==28||
    payloadSize>0xa0000||plain.readUInt32LE(16)!==payloadSize+28||compressedSize>plain.length-28)
    throw Error('Invalid th08 score checksum or envelope');
  return inspectTh08ScorePayload(Buffer.concat([plain.subarray(0,28),decodeTh08Lzss(plain.subarray(28,28+compressedSize),payloadSize)]));
}

export function inspectTh08ScorePayload(input){
  const decoded=Buffer.from(input),stable=Buffer.from(decoded);let formatVersion=null,statistics=null,program=null;
  if(decoded.length<28||decoded.readUInt16LE(4)!==1||decoded.readUInt32LE(8)!==28||
    decoded.readUInt32LE(16)!==decoded.length||decoded.readUInt32LE(20)!==decoded.length-28)
    throw Error('Invalid th08 decoded score header');
  stable.fill(0,0,8);stable.fill(0,24,28);
  for(let offset=28;offset<decoded.length;){
    if(offset+12>decoded.length)throw Error('Truncated th08 score chapter');
    const type=decoded.toString('ascii',offset,offset+4),length=decoded.readUInt16LE(offset+4),version=decoded[offset+8];
    if(length<12||offset+length>decoded.length)throw Error('Invalid th08 score chapter length');
    if(type==='TH8K')formatVersion=version;
    if(type==='VRSM'&&version===1){
      if(length<28)throw Error('Invalid th08 program fingerprint');
      program={version:decoded.toString('ascii',offset+12,offset+17),size:decoded.readUInt32LE(offset+20),checksum:decoded.readUInt32LE(offset+24)};
      if(!compatibleProgram(program))throw Error('Incompatible th08 score program fingerprint');
    }
    // GameApplication uses copy_chapter(..., false) | last full matching record wins.
    if(type==='PLST'&&version===2&&length>=0x228){
      statistics=Array.from({length:7},(_,index)=>{
        const start=offset+44+index*68;
        return{total:decoded.readUInt32LE(start),characters:Array.from({length:12},(_,shot)=>decoded.readUInt32LE(start+4+shot*4)),
          restarts:decoded.readUInt32LE(start+52),clears:decoded.readUInt32LE(start+56),continues:decoded.readUInt32LE(start+60),practices:decoded.readUInt32LE(start+64)};
      });
      // Wall-clock play time and encryption randomness may change on a real sync.
      stable.fill(0,offset+12,offset+44);
    }
    offset+=length;
  }
  if(formatVersion!==1||!statistics)throw Error('th08 score lacks its format header or play statistics');
  return{kind:'score',schema:'th08/score/1',values:{statistics,program},semanticSha256:hash(stable)};
}

// ScoreFile::valid and ProgramVersion.cpp | score compatibility excludes the Replay-only fingerprint.
function compatibleProgram({version,size,checksum}){
  const builds=[['0009_',566784,993132091],['0009a',596992,489076697],['0010_',596992,3478009099],
    ['0011_',596992,3545283257],['0001_',800256,1644916963],['0001a',807424,256801575],
    ['0002_',809984,1745855920],['0003_',825344,922048924],['0003a',825344,2460321175],
    ['0100_',840192,3552390786],['0100a',841216,3000360128],['0100b',841216,4189886502],
    ['0100c',840704,1605176696],['0100c',840704,1588396223],['0100d',840704,2724749753]];
  return version==='debug'||builds.some(row=>row[0]===version&&row[1]===size&&row[2]===checksum);
}

export const TH08_STORAGE_CONTRACT=Object.freeze({
  schema:'eagler-touhou/native-storage/1',
  source:'ports/th08/web/cpp/game/GameConfiguration.hpp | ConfigurationFile.cpp | ScoreFile.cpp | ProgramVersion.cpp | ScoreStore.cpp | GameApplication.cpp',
  configuration:{path:'th08.cfg',field:'soundVolume',step:5,minimum:0,maximum:100},
  menu:{readExport:'status',fields:{scene:0,page:8,cursor:9,phase:11},main:{scene:1,page:0,phase:1},
    optionsCursor:7,optionsPage:2,soundCursor:4,difficultyPage:4,characterPage:5,characterCursor:0,character:0,
    source:'ports/th08/web/cpp/game/TitleMenuState.hpp | TitleMenus.cpp | TitleFlow.cpp'},
  selection:{readExport:'practice_status',wordCount:7,fields:{difficulty:1,character:2,practice:3,replay:4,cheats:5,assisted:6},
    gameFlagsField:19,ordinaryFlags:{mask:0x400f,value:4},source:'ports/th08/web/cpp/platform/BrowserExports.cpp | practice_status | BrowserRuntime.cpp | status | TitleMenus.cpp'},
  files:[
    {path:'th08.cfg',kind:'configuration',inspect:inspectTh08Configuration},
    {path:'score.dat',kind:'score',inspect:inspectTh08Score},
  ],
});
