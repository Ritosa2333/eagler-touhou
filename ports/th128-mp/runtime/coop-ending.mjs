import {OriginalEndingInput} from '../../th128/site/runtime/native-ending-bridge-v6.mjs';
import {OriginalEndingPixels,decodeEndingGlyph} from '../../th128/site/runtime/native-ending-pixels.mjs';
import {FrameStepper} from '../../th128/site/runtime/timing.mjs';
import {safeAssetUrl} from '../../th128/site/inspector.mjs';

export const mainEndingStages=Object.freeze([3,5,8,10,13,15]);
const schema='th128-original-recorded-native-ending-resources/1',decoder=new TextDecoder('utf-8',{fatal:true});
const track='bgm/th128_09.wav';
export async function readCoopEndingAssets(resources,stage,signal){
 const index=mainEndingStages.indexOf(stage),pin=resources.endingManifest;
 if(index<0||pin?.schema!==schema||pin.variant!=='zh'||pin.file!=='native-ending-resources.json')throw Error('Original cooperative ending route is unavailable');
 async function read(record){
  signal.throwIfAborted();
  if(!record||!Number.isSafeInteger(record.bytes)||record.bytes<1||record.bytes>8*1024*1024||!/^[0-9a-f]{64}$/.test(record.sha256))throw Error('Invalid authored ending resource receipt');
  const response=await resources.fetch(safeAssetUrl(resources.base,record.file),{signal});if(!response.ok)throw Error('Original ending asset missing: '+record.file);
  const bytes=new Uint8Array(await response.arrayBuffer()),hash=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),x=>x.toString(16).padStart(2,'0')).join('');
  signal.throwIfAborted();if(bytes.length!==record.bytes||hash!==record.sha256)throw Error('Original ending asset receipt differs');return bytes;
 }
 const pack=JSON.parse(decoder.decode(await read(pin))),branch=pack.routes?.[index];
 if(pack.schema!==schema||pack.variant!=='zh'||pack.sourceExecutableSHA256!=='ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'||pack.routeCount!==6||pack.textContexts!==118||branch?.stage!==stage||branch.index!==index||!Array.isArray(branch.glyphs))throw Error('Original ending route receipt does not match the completed stage');
 const inputs=await Promise.all(['message','art','base','text'].map(key=>read(branch[key])));
 return Object.freeze({index,stage,inputs,async glyph(ordinal,color){
   const glyph=branch.glyphs[ordinal];if(glyph?.route!==index||glyph.ordinal!==ordinal||glyph.color!==color)throw Error('Original ending glyph changed');
   return decodeEndingGlyph(await read(glyph.texture));
 }});
}

export function createCoopEndingOwner({native,signal,paint,media=()=>{},onComplete=()=>{}}){
 const inputs=new OriginalEndingInput(),clock=new FrameStepper();let complete=false,busy=null,closed=false,paused=false,primed=false;
 function key({code,down,repeat=false}){
  if(closed||complete||signal.aborted)return false;
  if(code==='Escape'){if(down&&!repeat){paused=!paused;clock.reset();inputs.release();}return true;}
  if(down){return repeat||paused?true:inputs.down(code);}inputs.up(code);return true;
 }
 function reset(){inputs.release();clock.reset();}
 function close(){if(closed)return;closed=true;reset();}
 async function drain(state){
   for(const command of state.media.filter(row=>!row.acknowledged)){
     if(command.path!==track||!((command.serial===1&&command.operation===8&&command.durationFrames===0)||(command.serial===2&&command.operation===5&&command.durationFrames===180)))throw Error('Invalid original ending media sequence');
     await media(command);signal.throwIfAborted();if(closed)return;native.acknowledge(command);
   }
 }
 async function advance(now,visible=true){
  if(closed||signal.aborted||complete||busy)return;
  if(!visible||paused){reset();return;}
  const {steps}=clock.advance(now,true);if(!steps)return;
  busy=(async()=>{
   for(let i=0;i<steps&&!complete;i++){
    signal.throwIfAborted();if(closed)return;
    if(!primed){primed=true;await drain(native.snapshot());if(closed)return;}
    const state=native.step(inputs.sample());await drain(state);if(closed)return;
    await paint(state);signal.throwIfAborted();if(closed)return;
    if(state.originalReturnRequested){if(state.returnScene!==16)throw Error('Original cooperative ending returned to an unexpected scene');complete=true;onComplete(state);}
   }
  })();
  try{await busy;}finally{busy=null;}
 }
 return Object.freeze({key,advance,reset,close,finished:()=>complete});
}

export async function openCoopEnding({scene,resources,signal,view,sound,onComplete}){
 const stage=scene.state().stageId,assets=await readCoopEndingAssets(resources,stage,signal);
 if(sound)await sound.prepareEnding();signal.throwIfAborted();
 const native=scene.startEnding(assets.inputs);
 const pixels=new OriginalEndingPixels(assets.inputs[2],assets.inputs[1]);let ordinal=-1,glyph=null;
 return createCoopEndingOwner({native,signal,onComplete,media:command=>sound?.endingMusic(command),async paint(state){
  const next=state.executedOp3-1;
  if(next!==ordinal){glyph=next<0?null:await assets.glyph(next,state.textColor);ordinal=next;}
  signal.throwIfAborted();sound?.endingSounds(state.sounds);view.displayEnding(pixels.frame(state,glyph),state);
 }});
}
