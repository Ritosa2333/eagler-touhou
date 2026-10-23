import assert from 'node:assert/strict';
import {ADAPTATION_ADAPTERS} from './registry.mjs';

const MANIFEST_SCHEMA='eagler-touhou/dialogue-touch-manifest/1';
const OBSERVATION_SCHEMA='eagler-touhou/dialogue-touch-observation/1';
const SCENE_CONTRACT_SCHEMA='eagler-touhou/dialogue-touch-scene/1';

/** Resolve the title-owned contract for browser-side reads and the request stimulus. */
export function dialogueTouchSceneRequest({game,variant,adapterRegistry=ADAPTATION_ADAPTERS}={}){
  assert.equal(typeof adapterRegistry?.resolve,'function','Dialogue scene requests require an adaptation registry');
  assert.ok(['normal','multiplayer'].includes(variant),'Dialogue scene requests require an explicit Runtime variant');
  const adapter=adapterRegistry.resolve(game,variant);
  const declaration=adapter?.dialogueTouchScene;
  if(declaration){
    assert.equal(declaration.game,game,'Dialogue scene capability must be explicitly registered for its product');
    assert.equal(declaration.variant,variant,'Dialogue scene capability must be explicitly registered for its Runtime variant');
  }
  return{game,variant,contract:declaration?{...declaration,runtime:adapter.runtime}:null};
}

/**
 * Serialized frame.evaluate reader for a fixed native word export. The function
 * deliberately has no module closure and only resolves the selected Runtime's
 * declared core/Module owners and their own WebAssembly memory.
 */
export function readDialogueTouchWords({contract,operation}={}){
  const spec=contract?.[operation];
  const identifier=/^[A-Za-z_$][\w$]*$/;
  if(!contract||contract.schema!=='eagler-touhou/dialogue-touch-scene/1'||!spec||
     typeof spec.export!=='string'||!identifier.test(spec.export)||
     !Number.isSafeInteger(spec.magic)||spec.magic<=0||spec.magic>0xffffffff||
     !Number.isSafeInteger(spec.version)||spec.version<=0||
     !Number.isSafeInteger(spec.wordCount)||spec.wordCount<3||spec.wordCount>4096)
    return{available:false,reason:`Dialogue ${String(operation)} has no valid data-only native ABI declaration`};
  const mapping=contract.runtime;
  if(!mapping||['global','core','module'].some(key=>typeof mapping[key]!=='string'||!mapping[key]))
    return{available:false,reason:'Dialogue scene has no declared Runtime mapping'};
  const root=typeof window==='object'?window:globalThis;
  const runtime=root[mapping.global];
  if(!runtime)return{available:false,reason:`Declared Runtime mount ${mapping.global} is unavailable`};
  const core=runtime[mapping.core],module=runtime[mapping.module];
  const owners=[core,module].filter(Boolean);
  let owner=null,fn=null;
  for(const candidate of owners){const value=candidate[spec.export]??candidate[`_${spec.export}`];
    if(typeof value==='function'){owner=candidate;fn=value;break;}}
  if(!fn)return{available:false,reason:`Declared Runtime does not export ${spec.export}`};
  const coreBuffer=core?.memory?.buffer??null,moduleBuffer=module?.HEAPU8?.buffer??null;
  if(coreBuffer&&moduleBuffer&&coreBuffer!==moduleBuffer){
    const error=new Error('Dialogue reader found two different memories inside the declared Runtime');
    error.evidence={operation,global:mapping.global,core:mapping.core,module:mapping.module};throw error;
  }
  const buffer=coreBuffer??moduleBuffer;
  if(!buffer){
    const error=new Error('Declared Runtime has no owned WebAssembly memory for the dialogue snapshot');
    error.evidence={operation,global:mapping.global,core:mapping.core,module:mapping.module};throw error;
  }
  let pointer;
  try{pointer=fn.call(owner);}catch(cause){
    const error=new Error(`${spec.export} failed while reading the native dialogue snapshot`,{cause});
    error.evidence={operation,export:spec.export,global:mapping.global};throw error;
  }
  if(!Number.isInteger(pointer)||pointer<=0||pointer%4||pointer+spec.wordCount*4>buffer.byteLength){
    const error=new Error(`${spec.export} returned an invalid native snapshot pointer`);
    error.evidence={operation,export:spec.export,pointer,byteLength:buffer.byteLength};throw error;
  }
  return{available:true,operation,pointer,words:Array.from(new Uint32Array(buffer,pointer,spec.wordCount))};
}

/** Invoke the native stimulus export. Its ABI has one entry argument and no app argument. */
export function requestDialogueTouchMessage({contract,entry}={}){
  const request=contract?.request,identifier=/^[A-Za-z_$][\w$]*$/;
  if(!contract||contract.schema!=='eagler-touhou/dialogue-touch-scene/1'||!request||typeof request.export!=='string'||
     !identifier.test(request.export)||typeof request.appArgument!=='boolean'||
     !Number.isSafeInteger(request.acceptedStatus)||request.acceptedStatus<0||request.acceptedStatus>0xffffffff||
     !request.statusCodes||typeof request.statusCodes!=='object'||Array.isArray(request.statusCodes))
    return{available:false,accepted:false,reason:'No valid data-only native dialogue request contract',entry:Number.isSafeInteger(entry)?entry:null};
  if(!Number.isSafeInteger(entry)||entry<0)
    return{available:false,accepted:false,reason:'Dialogue request entry must be a nonnegative safe integer',entry};
  const mapping=contract.runtime;
  if(!mapping||['global','core','module'].some(key=>typeof mapping[key]!=='string'||!mapping[key])||
     (request.appArgument&&typeof mapping.app!=='string'))
    return{available:false,accepted:false,reason:'Dialogue request has no declared Runtime mapping',entry};
  const root=typeof window==='object'?window:globalThis,runtime=root[mapping.global];
  if(!runtime)return{available:false,accepted:false,reason:`Declared Runtime mount ${mapping.global} is unavailable`,entry};
  const core=runtime[mapping.core],module=runtime[mapping.module];let owner=null,fn=null;
  for(const candidate of [core,module])if(candidate){const value=candidate[request.export]??candidate[`_${request.export}`];
    if(typeof value==='function'){owner=candidate;fn=value;break;}}
  if(!fn)return{available:false,accepted:false,reason:`Declared Runtime does not export ${request.export}`,entry};
  const statusNames={};
  for(const [name,code]of Object.entries(request.statusCodes)){
    if(!Number.isSafeInteger(code)||code<0||code>0xffffffff||Object.hasOwn(statusNames,code))
      return{available:false,accepted:false,reason:'Dialogue request status-code declaration is invalid',entry};
    statusNames[code]=name;
  }
  if(!Object.hasOwn(statusNames,request.acceptedStatus))
    return{available:false,accepted:false,reason:'Accepted Dialogue status is absent from the status-code declaration',entry};
  const app=request.appArgument?runtime[mapping.app]:null;
  if(request.appArgument&&app==null)return{available:false,accepted:false,reason:'Dialogue request requires the declared Runtime app',entry};
  let nativeStatus;
  try{nativeStatus=fn.call(owner,...(request.appArgument?[app,entry]:[entry]));}catch(cause){
    const error=new Error(`${request.export} failed while requesting loaded Dialogue entry ${entry}`,{cause});
    error.evidence={entry,export:request.export,global:mapping.global};throw error;
  }
  if(!Number.isSafeInteger(nativeStatus)||!Object.hasOwn(statusNames,nativeStatus)){
    const error=new Error('Native Dialogue request returned an unknown status');
    error.evidence={entry,nativeStatus,export:request.export};throw error;
  }
  return{available:true,accepted:nativeStatus===request.acceptedStatus,statusCode:nativeStatus,status:statusNames[nativeStatus],entry};
}

function requireWords(raw,contract,operation,label){
  if(raw?.available===false)return null;
  const spec=contract?.[operation];
  if(contract?.schema!==SCENE_CONTRACT_SCHEMA||!spec||!Number.isSafeInteger(spec.wordCount)||spec.wordCount<3||spec.wordCount>4096||
     !Number.isSafeInteger(spec.magic)||spec.magic<=0||spec.magic>0xffffffff||!Number.isSafeInteger(spec.version)||spec.version<=0)
    throw new Error(`${label} requires an explicit bounded data-only ABI contract`);
  if(raw?.available!==true||!Array.isArray(raw.words)||raw.words.length!==spec.wordCount)
    throw new Error(`${label} reader did not return exactly ${spec.wordCount} native words`);
  const words=raw.words;
  if(words.some(value=>!Number.isSafeInteger(value)||value<0||value>0xffffffff))
    throw new Error(`${label} contains a non-uint32 native word`);
  if(words[0]!==spec.magic||words[1]!==spec.version||words[2]!==spec.wordCount)
    throw new Error(`${label} has an unsupported magic, version or word count`);
  return words;
}
function boolWord(value,label){
  if(value!==0&&value!==1)throw new Error(`${label} must be a native boolean word`);
  return value===1;
}
function runtimeIdentity(generation,pointer,epoch,label,{required=true}={}){
  const valid=Number.isSafeInteger(generation)&&generation>0&&Number.isSafeInteger(pointer)&&pointer>0&&
    Number.isSafeInteger(epoch)&&epoch>=0;
  if(required&&!valid)throw new Error(`${label} is missing its native Runtime identity`);
  return{generation:Number.isSafeInteger(generation)?generation:null,pointer:Number.isSafeInteger(pointer)?pointer:null,
    epoch:Number.isSafeInteger(epoch)?epoch:null};
}
function fnv64(low,high){return `${high.toString(16).padStart(8,'0')}${low.toString(16).padStart(8,'0')}`;}
function codeNames(codes,label){
  if(!codes||typeof codes!=='object'||Array.isArray(codes))throw new Error(`${label} needs a data-only code table`);
  const names={};
  for(const [name,code]of Object.entries(codes)){
    if(!Number.isSafeInteger(code)||code<0||code>0xffffffff||Object.hasOwn(names,code))
      throw new Error(`${label} has an invalid or duplicate code`);
    names[code]=name;
  }
  return names;
}
function requireSceneContract(contract,operation,label,{wordCount}={}){
  const spec=contract?.[operation];
  if(contract?.schema!==SCENE_CONTRACT_SCHEMA||!spec||
     !Number.isSafeInteger(spec.magic)||spec.magic<=0||spec.magic>0xffffffff||
     !Number.isSafeInteger(spec.version)||spec.version<=0||!Number.isSafeInteger(spec.wordCount)||
     spec.wordCount<3||spec.wordCount>4096||wordCount!==undefined&&spec.wordCount!==wordCount)
    throw new Error(`${label} requires an explicit data-only dialogue ABI contract`);
  return spec;
}

/** Normalize DTM1, preserving each actual native candidate row for the scene evidence. */
export function normalizeDialogueTouchManifest(raw,contract){
  const spec=requireSceneContract(contract,'manifest','Dialogue manifest',{wordCount:344});
  if(spec.headerWords!==24||spec.rowWords!==10||!Number.isSafeInteger(spec.capacity)||spec.capacity<1||spec.capacity>32)
    throw new Error('Dialogue manifest contract does not match the shared DTM1 row layout');
  const reasonCodes=codeNames(spec.reasonCodes,'Dialogue manifest reason codes');
  const readyCode=spec.reasonCodes.ready;
  if(!Number.isSafeInteger(readyCode))throw new Error('Dialogue manifest reason table has no ready code');
  const words=requireWords(raw,contract,'manifest','Dialogue manifest');
  if(!words)return{schema:MANIFEST_SCHEMA,available:false,reason:raw.reason??'Native Dialogue manifest is unavailable',candidates:[]};
  const reasonCode=words[4],reason=reasonCodes[reasonCode];
  if(reason===undefined)throw new Error(`Dialogue manifest has unknown reason code ${reasonCode}`);
  const available=boolWord(words[3],'Dialogue manifest available');
  if(available!==(reasonCode===readyCode))throw new Error('Dialogue manifest availability disagrees with its reason code');
  const runtime=runtimeIdentity(words[5],words[6],words[7],'Dialogue manifest',{required:available});
  const receipt={schema:MANIFEST_SCHEMA,available,reason:available?null:reason,reasonCode,runtime,
    source:null,candidates:[],rawWords:words};
  if(!available)return receipt;
  const stage=words[8],character=words[9],team=words[10],difficultyMask=words[11];
  const timelineCount=words[15],messageEntryCount=words[16],candidateCount=words[22],rowWords=words[23];
  if(!Number.isSafeInteger(contract.stage)||contract.stage<0||stage!==contract.stage||
     typeof contract.variant!=='string'||!contract.variant.trim()||
     !Array.isArray(spec.teamByCharacter)||!Number.isSafeInteger(spec.teamByCharacter[character])||
     spec.teamByCharacter[character]!==team||difficultyMask===0||timelineCount===0||messageEntryCount===0)
    throw new Error('Dialogue manifest has invalid source identity for its title-owned declaration');
  if(words[12]===0||words[17]===0||candidateCount<1||candidateCount>spec.capacity||rowWords!==spec.rowWords)
    throw new Error('Dialogue manifest has invalid loaded source sizes or candidate bounds');
  const eclAsset=spec.eclAssets?.[words[20]],messageAsset=spec.messageAssets?.[words[21]];
  if(typeof eclAsset!=='string'||!eclAsset.trim()||typeof messageAsset!=='string'||!messageAsset.trim())
    throw new Error('Dialogue manifest names an asset absent from its title-owned source map');
  const candidates=[],seen=new Set();
  for(let index=0;index<candidateCount;index++){
    const offset=spec.headerWords+index*spec.rowWords,row=words.slice(offset,offset+spec.rowWords);
    const [entry,timeline,time,difficultyBits,eclOffset,referenceCount,waitOffset,waitTime,timeoutFrames,instructionCount]=row;
    if(entry>=messageEntryCount||timeline>=timelineCount||difficultyBits===0||!(difficultyBits&difficultyMask)||
       eclOffset>=words[12]||referenceCount===0||waitOffset>=words[17]||timeoutFrames===0||instructionCount===0)
      throw new Error(`Dialogue manifest candidate ${index} has invalid ECL/MessageProgram bounds`);
    if(seen.has(entry))throw new Error(`Dialogue manifest repeats MessageProgram entry ${entry}`);
    seen.add(entry);
    candidates.push({entry,ecl:{timeline,time,difficultyBits,offset:eclOffset,referenceCount},
      message:{waitOffset,waitTime,timeoutFrames,instructionCount},rawWords:row});
  }
  receipt.source={variant:contract.variant,stage,character,team,difficultyMask,
    ecl:{asset:eclAsset,byteLength:words[12],fnv64:fnv64(words[13],words[14]),timelineCount},
    message:{asset:messageAsset,byteLength:words[17],fnv64:fnv64(words[18],words[19]),entryCount:messageEntryCount}};
  receipt.candidates=candidates;
  return receipt;
}

/** Normalize DTO1 without manufacturing an input edge or mutable Dialogue state. */
export function normalizeDialogueTouchObservation(raw,contract){
  const spec=requireSceneContract(contract,'observation','Dialogue observation',{wordCount:45});
  const reasonCodes=codeNames(spec.reasonCodes,'Dialogue observation reason codes');
  const readyCode=spec.reasonCodes.ready;
  const confirmMask=spec.confirmMask;
  if(!Number.isSafeInteger(readyCode)||!Number.isSafeInteger(confirmMask)||confirmMask<=0||confirmMask>0x80000000||
     !Number.isInteger(Math.log2(confirmMask)))throw new Error('Dialogue observation contract has invalid ready/confirm declarations');
  const words=requireWords(raw,contract,'observation','Dialogue observation');
  if(!words)return{schema:OBSERVATION_SCHEMA,available:false,reason:raw.reason??'Native Dialogue observation is unavailable'};
  const reasonCode=words[4],reason=reasonCodes[reasonCode];
  if(reason===undefined)throw new Error(`Dialogue observation has unknown reason code ${reasonCode}`);
  const available=boolWord(words[3],'Dialogue observation available');
  if(available&&reasonCode!==readyCode)throw new Error('Dialogue observation is available with a non-ready reason');
  const runtime=runtimeIdentity(words[5],words[6],words[7],'Dialogue observation',{required:available});
  if(!available){
    const unavailableReason=reasonCode===0?'Native Dialogue observation is outside a loaded gameplay scene':reason;
    return{schema:OBSERVATION_SCHEMA,available:false,reason:unavailableReason,reasonCode,runtime,rawWords:words};
  }
  for(const key of ['appActive','inGame','fixtureUsed','gateEnabled','gateUpdateStarted','writeLocked','loadScope',
    'dialoguePresent','skippable','selected','freshConfirmEdge','consumerValid','touchFire','hostedZ']){
    const index={appActive:9,inGame:10,fixtureUsed:14,gateEnabled:15,gateUpdateStarted:16,writeLocked:18,loadScope:19,
      dialoguePresent:21,skippable:29,selected:30,freshConfirmEdge:33,touchFire:37,hostedZ:39,consumerValid:44}[key];
    boolWord(words[index],`Dialogue observation ${key}`);
  }
  const inputCurrent=words[31],inputPrevious=words[32],consumerValid=boolWord(words[44],'Dialogue observation consumer valid');
  const freshEdge=boolWord(words[33],'Dialogue observation fresh confirm edge');
  if(freshEdge!==Boolean(consumerValid&&(inputCurrent&confirmMask)&&!(inputPrevious&confirmMask)))
    throw new Error('Dialogue observation fresh-confirm flag disagrees with its native current/previous input');
  const requestStatusNames=codeNames(contract.request?.statusCodes,'Dialogue request status codes');
  const requestStatus=requestStatusNames[words[41]];
  if(requestStatus===undefined)throw new Error(`Dialogue observation has unknown request status ${words[41]}`);
  const optional=value=>value===0xffffffff?null:value;
  const present=boolWord(words[21],'Dialogue observation dialoguePresent');
  const message=optional(words[22]),instructionOffset=optional(words[23]),opcode=optional(words[24]),instructionTime=optional(words[25]);
  if(present&&(message===null||instructionOffset===null||opcode===null||instructionTime===null))
    throw new Error('Active native Dialogue is missing its MessageProgram cursor');
  return{schema:OBSERVATION_SCHEMA,available,reason:available?null:reason,reasonCode,runtime,
    scene:{variant:contract.variant,stage:words[11],character:words[12],gameFlags:words[13]},hostFrames:words[8],
    fixture:{used:boolWord(words[14],'Dialogue fixture used'),gateEnabled:boolWord(words[15],'Dialogue gate enabled'),
      updateStarted:boolWord(words[16],'Dialogue gate update started'),withheldUpdates:words[17],
      writeLocked:boolWord(words[18],'Dialogue write lock'),loadScope:boolWord(words[19],'Dialogue write load scope'),
      committedBytes:words[20]},
    dialogue:{present,message,instructionOffset,opcode,instructionTime,timer:words[26],pausedFrames:words[27],
      minimumWait:words[28],skippable:boolWord(words[29],'Dialogue skippable'),selected:boolWord(words[30],'Dialogue selected'),
      currentInput:inputCurrent,previousInput:inputPrevious,freshConfirmEdge:freshEdge,
      consumer:{sequence:words[34],valid:consumerValid}},
    controlFrames:words[35],eclFrame:words[36],
    touch:{fireEnabled:boolWord(words[37],'Dialogue Touch Fire'),context:words[38],hostedZ:boolWord(words[39],'Dialogue hosted Z')},
    request:{serial:words[40],statusCode:words[41],status:requestStatus,entry:optional(words[42]),timeline:optional(words[43])},
    rawWords:words};
}

export const DIALOGUE_TOUCH_ABI=Object.freeze({manifestSchema:MANIFEST_SCHEMA,manifestMagic:0x44544d31,manifestWordCount:344,
  observationSchema:OBSERVATION_SCHEMA,observationMagic:0x44544f31,observationWordCount:45});
