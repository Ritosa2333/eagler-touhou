import assert from 'node:assert/strict';

const panelNames=['cheats','advanced','tracker'];
/** Common read-only UI ABI. Titles may map their own ABI into this same view. */
export function decodePracticeUiSnapshot(raw){
  assert.equal(raw?.available,true,raw?.reason);const words=raw.words;
  assert.ok(Array.isArray(words)&&words.length===128&&words.every(word=>Number.isSafeInteger(word)&&word>=0&&word<=0xffffffff));
  assert.equal(words[0],1,'Unsupported native practice UI snapshot');assert.equal(words[1],1,'Native practice UI is not initialized');
  for(const index of [4,5,6,7,8,9,10,38,39])assert.ok(words[index]<=1,`Invalid native UI boolean ${index}`);
  assert.ok(words[36]<=2&&words[37]<=8);
  const f32=word=>{const buffer=new ArrayBuffer(4),view=new DataView(buffer);view.setUint32(0,word,true);const value=view.getFloat32(0,true);assert.ok(Number.isFinite(value));return value;};
  const rect=(x,y,w,h)=>{const result={x:f32(x),y:f32(y),width:f32(w),height:f32(h)};assert.ok(result.width>=0&&result.height>=0);return result;};
  const panels=Object.fromEntries(panelNames.map((name,index)=>{const offset=12+index*8,flags=words[offset];assert.ok(flags<=7&&words[offset+6]<=1);
    if(flags===0)assert.ok(words.slice(offset,offset+8).every(word=>word===0));else assert.ok((flags&1)!==0&&words[offset+7]>0);
    return[name,{exists:Boolean(flags&1),active:Boolean(flags&2),hidden:Boolean(flags&4),rect:rect(...words.slice(offset+1,offset+5)),
      vertexCount:words[offset+5],expanded:Boolean(words[offset+6]),id:words[offset+7]}];}));
  const items=Array.from({length:words[37]},(_,index)=>{
    const offset=40+index*8,code=words[offset],key=code>=112&&code<=118?'F'+(code-111):code===85?'KeyU':null;
    assert.ok(key,'Unknown submitted practice hotkey');assert.ok(words[offset+1]<=1&&words[offset+7]>0);
    const x=f32(words[offset+2]),y=f32(words[offset+3]),right=f32(words[offset+4]),bottom=f32(words[offset+5]);assert.ok(right>x&&bottom>y);
    return{key,value:Boolean(words[offset+1]),rect:{x,y,width:right-x,height:bottom-y},generation:words[offset+6],id:words[offset+7]};
  });assert.equal(new Set(items.map(item=>item.key)).size,items.length);
  assert.ok(words[110]===0||words[110]===0x50554b31,'Unknown native practice keyboard extension');
  let input={};
  if(words[110]){assert.equal(words[111],1);for(const index of [104,105,106,107])assert.ok(words[index]<=1);
    const held=key=>Boolean((words[112+(key>>>5)]>>>(key&31))&1);
    assert.equal(Boolean(words[106]),held(90)||held(13),'Native confirm hold differs from its key bitmap');
    assert.equal(Boolean(words[107]),held(88)||held(27),'Native cancel hold differs from its key bitmap');
    input={practiceInput:{armed:!!words[104],widgetBusy:!!words[105],confirmHeld:!!words[106],cancelHeld:!!words[107]},
      keyboard:{down:words.slice(112,120),pressed:words.slice(120,128)}};}
  return{schema:'eagler-touhou/practice-ui-observation/1',inputGeneration:words[2],renderedGeneration:words[3],
    menuOpen:Boolean(words[4]),trackerOpen:Boolean(words[5]),advancedOpen:Boolean(words[6]),mobileEnabled:Boolean(words[7]),mobileVisible:Boolean(words[8]),
    frameDrawn:Boolean(words[9]),pointerDown:Boolean(words[10]),pendingPointerEdges:words[11],locale:words[36],textEditing:Boolean(words[38]),practiceMenuOpen:Boolean(words[39]),panels,items,...input};
}

/** Recompute the actual menu input boundary from the retained native UI words. */
export function assertPracticeMenuInputReady(sample,{decodeNative,decodeUi=decodePracticeUiSnapshot}){
  assert.deepEqual(sample.ui,decodeUi(sample.uiRaw));assert.deepEqual(sample.native,decodeNative(sample.nativeRaw));
  const {ui,native}=sample;assert.equal(native.state.menu,true);assert.equal(ui.practiceMenuOpen,true);
  assert.equal(native.state.accepted,false);assert.equal(native.state.active,false);assert.equal(native.state.inGame,false);
  assert.equal(ui.inputGeneration,ui.renderedGeneration);assert.equal(ui.frameDrawn,true);
  assert.equal(ui.practiceInput?.armed,true);assert.equal(ui.practiceInput.widgetBusy,false);
  assert.equal(ui.practiceInput.confirmHeld,false);assert.equal(ui.practiceInput.cancelHeld,false);
  assert.ok(ui.keyboard&&ui.keyboard.down.length===8&&ui.keyboard.pressed.length===8);
  return sample;
}

/** Actual timeline consumer state from the existing immutable initialization boundary. */
export function decodePracticeApplicationSnapshot(raw){
  assert.equal(raw?.available,true,raw?.reason);const words=raw.words;assert.ok(Array.isArray(words)&&words.length===80);
  assert.equal(words[0],1);assert.ok((words[1]&1)!==0);
  assert.equal(words[70],1,'Native practice timeline consumer observation is unavailable');
  for(const index of [71,72,73,74,75,76,77,79])assert.ok(Number.isSafeInteger(words[index]));
  assert.ok(words[71]>0&&words[75]>0&&[0,1].includes(words[76])&&[0,1].includes(words[79]));
  if(words[79])assert.ok(words[73]>=0&&words[73]+8<=words[75]);else assert.ok(words[73]===-1&&words[74]===-1);
  assert.ok(Number.isFinite(words[78]));
  return{schema:'eagler-touhou/practice-application-observation/1',timelineCount:words[71],timelineFrame:words[72],instructionSelected:Boolean(words[79]),instructionOffset:words[79]?words[73]:null,opcode:words[79]?words[74]:null,
    programBytes:words[75],practiceInstructions:Boolean(words[76]),previousFrame:words[77],fraction:words[78]};
}
