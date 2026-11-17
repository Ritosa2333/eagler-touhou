// Original modern USER/PRAC layout from thprac_games.cpp b5beb37a91e322d985c8f89bb6377d55bcf07734.
// MIT license retained in licenses/THPRAC-MIT.txt. File identity is consistency, not publisher authentication.
import {decodeReplay,encodeReplay} from './replay.mjs';
import {createThprac128Session,readThprac128Native} from './thprac128.mjs';
import {validateThprac128ToolHistory,installThprac128ToolPlayback} from './thprac128-tools.mjs';
const PRAC=0x43415250,SCHEMA='th128-thprac-web-replay/1',encoder=new TextEncoder();
function identity(input){if(!input||typeof input!=='object'||typeof input.coreSHA256!=='string'||typeof input.resourceSHA256!=='string'||!['coreSHA256','resourceSHA256'].every(k=>/^[a-f0-9]{64}$/.test(input[k])))throw new Error('practice core/resource SHA identity required');return{coreSHA256:input.coreSHA256,resourceSHA256:input.resourceSHA256};}
function matchInitial(decoded,session){
 const p=session.params,s=decoded.stages[0];if(p.mode!==1||decoded.stages.length!==1||s.stage!==p.stage+1||decoded.metadata.rank!==session.difficulty||s.numFrames<1||s.continues!==0||s.scoreRaw!==p.score/10||s.motivationRaw!==p.motivation||s.perfectFreezeRaw!==p.perfect_freeze||s.freezeArea!==p.ice_area)throw new Error('practice RPY stage/rank/initial resource identity differs from thprac params');
}
function jsonBlock(session,source){const p=session.params,value={version:'2.3.1.1',game:'th128',...p,_web:{schema:SCHEMA,...source}};const data=encoder.encode(JSON.stringify(value));if(data.length>65536)throw new Error('practice PRAC JSON budget');const size=(data.length+13+3)&~3,raw=new Uint8Array(size),v=new DataView(raw.buffer);v.setUint32(0,0x52455355,true);v.setUint32(4,size,true);v.setUint32(8,PRAC,true);raw.set(data,12);return{headerRaw:raw.slice(0,12),kind:PRAC,data:raw.slice(12),raw};}
export function attachThprac128Replay(bytes,input,source){
 const session=createThprac128Session(input.params,input.difficulty),fixedIdentity=identity(source),decoded=decodeReplay(bytes);matchInitial(decoded,session);
 const tools=validateThprac128ToolHistory(source.tools??{events:[],speed:[]},decoded.stages[0].numFrames);
 if(decoded.userBlocks.some(b=>b.kind===PRAC))throw new Error('duplicate PRAC practice metadata already present');
 if(decoded.fileTailRaw.length)throw new Error('practice PRAC cannot replace an unknown trailing container');
 decoded.userBlocks.push(jsonBlock(session,{...fixedIdentity,tools}));const file=encodeReplay(decoded);const read=readThprac128Replay(file,fixedIdentity);if(!read||JSON.stringify(read.session.params)!==JSON.stringify(session.params))throw new Error('practice metadata full RPY readback differs');return file;
}
export function readThprac128Replay(bytes,expectedIdentity){
 const decoded=decodeReplay(bytes),blocks=decoded.userBlocks.filter(b=>b.kind===PRAC);if(!blocks.length)return null;
 if(blocks.length!==1||decoded.userBlocks.at(-1)!==blocks[0]||decoded.fileTailRaw.length)throw new Error('duplicate or trailing PRAC metadata');
 const data=blocks[0].data;if(data.length<4||blocks[0].raw.length%4||data.length>65540)throw new Error('PRAC padding or size mismatch');
 const zero=data.indexOf(0);if(zero<0||data.subarray(zero).some(v=>v!==0))throw new Error('PRAC JSON null padding mismatch');
 const text=new TextDecoder('utf-8',{fatal:true}).decode(data.subarray(0,zero)),value=JSON.parse(text);if(!value||value.game!=='th128'||value.version!=='2.3.1.1'||value._web?.schema!==SCHEMA)throw new Error('unsupported practice PRAC game/version');
 const{game,version,_web,...params}=value,source=identity(_web);if(expectedIdentity){const expected=identity(expectedIdentity);if(source.coreSHA256!==expected.coreSHA256||source.resourceSHA256!==expected.resourceSHA256)throw new Error('practice replay core/resource identity revision mismatch');}
 const session=createThprac128Session(params,decoded.metadata.rank);matchInitial(decoded,session);
 return Object.freeze({session,tools:validateThprac128ToolHistory(_web.tools??{events:[],speed:[]},decoded.stages[0].numFrames),identity:Object.freeze(source),playLock:true,sourceAuthentication:false,version,stage:decoded.stages[0].stage,seed:decoded.stages[0].seed,frames:decoded.stages[0].numFrames});
}
export class Thprac128ReplayPlayback{
 #file;#stage;#metadata;#module=null;#closed=false;
 constructor(bytes,slot,expectedIdentity){if(!(bytes instanceof Uint8Array)||!Number.isInteger(slot)||slot<0||slot>24)throw new Error('practice replay file/slot identity rejected');this.#file=bytes.slice();this.#metadata=readThprac128Replay(this.#file,expectedIdentity);if(!this.#metadata)throw new Error('practice PRAC metadata missing');const s=decodeReplay(this.#file).stages[0];this.#stage={...s,rawHeader:s.rawHeader.slice(),inputRaw:s.inputRaw.slice()};this.slot=slot;this.totalFrames=s.numFrames;this.selectedChoice=0;}
 get playLock(){return !this.#closed;}
 get tools(){if(this.#closed)throw new Error("practice replay owner closed");return this.#metadata.tools;}
 get identity(){if(this.#closed)throw new Error('practice replay owner closed');return this.#metadata.identity;}
 get session(){if(this.#closed)throw new Error('practice replay owner closed');return this.#metadata.session;}
 firstSelection(){if(this.#closed)throw new Error('practice replay owner closed');return{stage:this.#stage.stage,difficulty:this.#metadata.session.difficulty,seed:this.#stage.seed,progress:0,autoFlow:false,debugExtra:this.#stage.stage===16,thprac:this.#metadata.session,thpracReplayIdentity:this.#metadata.identity};}
 stageInitial(){if(this.#closed)throw new Error('practice replay owner closed');return{stage:this.#stage.stage,seed:this.#stage.seed,horizontal:this.#stage.positionRaw[0],vertical:this.#stage.positionRaw[1],frozenArea:this.#stage.freezeArea,motivation:this.#stage.motivationRaw,perfect:this.#stage.perfectFreezeRaw,rawHeader:this.#stage.rawHeader.slice()};}
 stageChoices(){return[{stage:this.#stage.stage,available:true,recordedIndex:0,frames:this.#stage.numFrames,scoreRaw:decodeReplay(this.#file).metadata.scoreRaw}];}
 stageChoicePacket(){const packet=new Uint8Array(72),v=new DataView(packet.buffer);v.setUint32(0,0x31545352,true);v.setUint32(4,this.slot,true);v.setUint32(8,decodeReplay(this.#file).metadata.route,true);v.setUint32(12,this.#metadata.session.difficulty,true);[1,this.#stage.stage,decodeReplay(this.#file).metadata.scoreRaw,this.#stage.numFrames].forEach((n,i)=>v.setUint32(24+i*4,n,true));return packet;}
 chooseStage(choice){if(choice!==0||this.#closed)throw new Error('practice replay has one owned recorded stage');return this.firstSelection();}
 stagePacket(){if(this.#closed)throw new Error('practice replay owner closed');const b=new Uint8Array(144+this.#stage.inputRaw.length);b.set(this.#stage.rawHeader);b.set(this.#stage.inputRaw,144);return b;}
 bindNativeStage(module){if(this.#closed||this.#module||typeof module?._th128_world_thprac_replay_playback_load!=='function')throw new Error('practice replay native owner unavailable');if(typeof module._th128_world_thprac_tools_playback==='function'){const receipt=readThprac128Native(module,this.#metadata.session);installThprac128ToolPlayback(module,this.#metadata.tools.events,receipt.generation);}else if(this.#metadata.tools.events.length)throw new Error('Practice tool playback native ABI missing');const bytes=this.stagePacket(),p=module._malloc(bytes.length);try{module.HEAPU8.set(bytes,p);if(module._th128_world_thprac_replay_playback_load(p,bytes.length)!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'practice native input binding rejected');}finally{module._free(p);}this.#module=module;return this.snapshot();}
 snapshot(){if(this.#closed||!this.#module)throw new Error('practice replay native owner unavailable');const r=JSON.parse(this.#module.UTF8ToString(this.#module._th128_world_replay_playback_state()));if(r.active!==true||r.stage!==this.#stage.stage||r.rank!==this.#metadata.session.difficulty||r.frames!==this.#stage.numFrames||!Number.isSafeInteger(r.consumed)||r.consumed<0||r.consumed>r.frames||r.complete!==(r.consumed===r.frames)||!r.auxiliaryKnown||!Number.isSafeInteger(r.generation)||r.generation<1)throw new Error('practice playback actual native receipt differs');return{...r,slot:this.slot,stageIndex:0,totalStages:1,totalFrames:r.frames,consumedTotal:r.consumed,playLock:true};}
 hasNext(){return false;}
 restartSelected(){if(this.#closed)throw new Error('practice replay owner closed');return new Thprac128ReplayPlayback(this.#file,this.slot,this.#metadata.identity);}
 cancel(){if(this.#closed)return;if(this.#module&&this.#module._th128_world_replay_playback_cancel()!==1)throw new Error('practice replay cancellation rejected');this.#closed=true;this.#module=null;}
}
