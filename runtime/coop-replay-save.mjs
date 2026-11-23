import {FrameStepper} from '../../th128/site/runtime/timing.mjs';
import {makeTh128ReplayFile} from './replay-file.mjs';

const masks=Object.freeze({KeyZ:1,KeyC:1,ControlLeft:1,Enter:1,KeyX:2,Escape:256,ArrowUp:16,ArrowDown:32,ArrowLeft:64,ArrowRight:128});
const width=14,cols=18;
function appendGlyph(records,sprite,x,y,color){
  const u=sprite%cols*width,v=Math.floor(sprite/cols)*width;
  records.push({id:47500000+records.length,parent:0,bank:11,entry:0,sprite,layer:31,mode:0,
    anchorX:1,anchorY:1,blend:0,flags:3,flags2:0,drawPass:'original-menu-font0',color,secondary:color,
    x,y,z:0,rotation:0,sx:1,sy:1,u:0,v:0,uvScaleX:1,uvScaleY:1,width,height:width,
    u0:u/256,v0:v/256,u1:(u+width)/256,v1:(v+width)/256,rx:0,ry:0});
}
function appendText(records,text,x,y,color,shadow=true){
  for(const c of text){
    if(c!==' '){const sprite=c.charCodeAt(0)-32;if(shadow)appendGlyph(records,sprite,x+2,y+2,((color>>>25)<<24)>>>0);appendGlyph(records,sprite,x,y,color);}
    x+=9;
  }
}
const fixed=(n,len)=>String(n).padStart(len,'0');
const displayName=name=>name?.trim()||'--------';
function dateText(at){const date=new Date(at);return `${fixed(date.getFullYear()%100,2)}/${fixed(date.getMonth()+1,2)}/${fixed(date.getDate(),2)}`;}
function slotLine(slot,row){
  const index=String(slot+1).padStart(2);
  if(!row)return `${index} -------- --/--/-- --------- - ------`;
  return `${index} ${displayName(row.playerName).padEnd(8)} ${dateText(row.savedAt)} ${String(row.frames).padStart(8)}f ${'ENHLX'[row.difficulty]} ${row.route.padEnd(5)} ${row.players}P`;
}

// Runs on a presentation-only Module. The name entry and its animation are the
// native single-player controller and glyph generator, with separate MP files.
export async function openCoopReplaySave({bytes,gameOver=false,postrun=false,signal,resources,factory,shelf,sound=()=>{},onSaved,onError=()=>{}}){
  signal.throwIfAborted();
  const [wasm,file,existing]=await Promise.all([resources.read('coopWasm'),makeTh128ReplayFile(bytes,resources.build),shelf.list()]);
  signal.throwIfAborted();
  const module=await factory({wasmBinary:wasm,print(){},printErr(message){throw Error(message);}});
  signal.throwIfAborted();
  const ok=(result,message)=>{if(result!==1)throw Error(message);};
  ok(module._th128_coop_replay_save_open(Number(gameOver)),'Original replay menu did not open');
  const left=postrun?58:144,top=postrun?80:64;
  const rows=new Map(existing.filter(r=>Number.isInteger(r.slot)).map(r=>[r.slot,r]));
  const held=new Set(),edges=[],clock=new FrameStepper();
  let cached,revision=0,cursor=0,pending=false,finished=false,dirty=false,problem=false;
  function snapshot(){
    const ptr=module._th128_coop_replay_save_state(cursor);
    if(!ptr)throw Error('Original replay menu state unavailable');
    const value=JSON.parse(module.UTF8ToString(ptr));cursor+=value.sounds.length;
    for(const id of value.sounds)sound(id);
    return value;
  }
  let state=snapshot();
  function key({code,down,repeat=false}){
    const mask=masks[code];if(!mask)return false;
    if(!down)held.delete(code);
    else if(!repeat&&!held.has(code)){
      // Two routes can deliver the same physical key. Do not drain queued actions on repeats.
      held.add(code);if(!pending&&!finished&&edges.length<8)edges.push(mask);
    }
    return true;
  }
  function clear(){held.clear();edges.length=0;clock.reset();}
  signal.addEventListener('abort',clear,{once:true});
  function save(){
    if(pending)return;pending=true;problem=false;edges.length=0;
    const {serial,slot}=state,playerName=String.fromCharCode(...state.name).trimEnd();
    void shelf.save(file,{slot,playerName,signal}).then(row=>{
      if(signal.aborted)return;
      ok(module._th128_coop_replay_save_ack(serial,slot),'Saved replay slot acknowledgement rejected');
      rows.set(slot,row);state=snapshot();dirty=true;
      if(!gameOver)finished=true;onSaved?.(row);
    }).catch(error=>{
      if(signal.aborted)return;
      problem=true;dirty=true;onError(error);
    }).finally(()=>{pending=false;});
  }
  function frame(now){
    signal.throwIfAborted();
    if(finished)return {phase:5,slot:state.slot,revision,records:[]};
    const {steps}=clock.advance(now,true);
    if(state.phase===3&&problem&&edges.length){
      const edge=edges.shift();if(edge&(2|256))finished=true;else if(edge===1)save();
    }
    let changed=dirty||!cached;dirty=false;
    if(steps&&!pending&&state.phase<3){
      for(let i=0;i<steps&&state.phase<3;i++){
        ok(module._th128_coop_replay_save_tick(i===0?edges.shift()??0:0),'Original replay menu input rejected');
        const next=snapshot();changed ||= next.phase!==state.phase||next.slot!==state.slot||next.phase===2;
        state=next;
      }
      if(state.phase===3)save();
    }
    if(finished||state.phase===5)return {phase:5,slot:state.slot,revision,records:[]};
    if(!changed&&cached)return cached;
    const records=[];
    if(state.phase===1){
      for(let slot=0;slot<25;slot++)appendText(records,(postrun?'No.':'')+slotLine(slot,rows.get(slot)),left,top+15*slot,slot===state.slot?0xffffff00:0xff808080);
    }else if(state.phase===2&&postrun){
      const start=top+15*state.slot,y=start+(240-start)*Math.min(state.age,10)/10;
      appendText(records,`No.${fixed(state.slot+1,2)}          ${dateText(Date.now())}`,58,y,0xffffffff,false);
      if(state.age>=10){
        appendText(records,String.fromCharCode(...state.name),112,240,0xffffffff,false);
        appendText(records,'_',112+9*Math.min(state.length,7),240,0xffffff00,false);
        // The original pause name glyphs end in the 91 two-pass keyboard
        // cells. The standalone postrun uses the same atlas without shadows.
        for(const [sprite,x,y,color] of state.glyphs.slice(-182))if((color>>>24)===255)appendGlyph(records,sprite,x+4,y+40,color);
      }
    }else if(state.phase===2){
      for(const [sprite,x,y,color] of state.glyphs)appendGlyph(records,sprite,x,y,color);
      appendText(records,slotLine(state.slot),144,64,0xffffffff);
    }else appendText(records,problem?'Not saved. Z: Retry  X: Back':'Saving Replay...',144,64,0xffffffff);
    return cached={phase:state.phase,slot:state.slot,revision:++revision,records};
  }
  return Object.freeze({key,clear,frame});
}
