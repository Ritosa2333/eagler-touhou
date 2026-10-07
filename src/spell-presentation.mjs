// Read-only spell text output from TH12 1.00b 40def0/460800.
// ANM poses belong to the core; this module never advances their clocks.
const validHex=hex=>typeof hex==='string'&&hex.length%2===0&&hex.length<=4096&&/^[0-9a-f]*$/i.test(hex);
const signedInteger=value=>Number.isInteger(value)&&value>=-0x80000000&&value<=0x7fffffff;

function titleBytes(hex) {
  if(!validHex(hex))throw Error('符卡文字数据无效');
  const bytes=Uint8Array.from(hex.match(/../g)??[],value=>parseInt(value,16));
  const terminator=bytes.indexOf(0);
  return terminator===-1?bytes:bytes.subarray(0,terminator);
}
export function decodeSpellTitle(titleHex) {
  return new TextDecoder('shift-jis',{fatal:true}).decode(titleBytes(titleHex));
}
export function readCoreSpell(core) {
  if(!core._th12_spell||!core._th12_spell_size)return null;
  const ptr=core._th12_spell(),size=core._th12_spell_size(),heap=core.HEAPU8;
  if(!Number.isSafeInteger(ptr)||!Number.isSafeInteger(size)||ptr<0||size<2||size>65536||
    !heap||ptr+size>heap.byteLength)throw Error('符卡状态记录无效');
  const state=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(heap.subarray(ptr,ptr+size)).replace(/\0+$/,''));
  if(!state||Array.isArray(state)||!signedInteger(state.flags)||!signedInteger(state.bonusDisplayed)||
    !validHex(state.titleHex)||!Array.isArray(state.records)||state.records.length!==4||
    !state.records.every(n=>Number.isInteger(n)&&n>=0&&n<=0x7fffffff))throw Error('符卡状态字段无效');
  return state;
}

const matchPose=(p,name,sprite)=>p?.a?.name===name&&p.a.sprite===sprite;
const widthInteger=(value,size)=>String(value).padStart(size,' ');
const precisionInteger=(value,size)=>(value<0?'-':'')+String(Math.abs(value)).padStart(size,'0');
function fontSubmission(text,x,colorARGB) {
  return {text,font:2,x,y:51,z:0,scaleX:1,scaleY:1,colorARGB,alignX:1,alignY:1,drawFlag:1};
}

export function spellAsciiDescriptors(spell,logicalPoses=[]) {
  if(!spell||(spell.flags&1)===0)return [];
  // Native looks up ascii2, not ascii1. Both strings use this VM's primary
  // alpha byte and fixed screen positions, independently of its transform.
  const history=logicalPoses.find(p=>matchPose(p,'ascii',261));
  if(!history)return [];
  const alphaByte=Math.max(0,Math.min(255,Math.round(history.a.alpha*255)));
  const colorARGB=((alphaByte<<24)|0xffffff)>>>0;
  const bonus=spell.flags&2?fontSubmission(widthInteger(spell.bonusDisplayed,8),261,colorARGB):
    fontSubmission('$',275,colorARGB);
  const records=spell.records;
  // 40dfe6/40dfec reads the current loadout, captures before attempts. The
  // all-loadout counters are not substituted for missing persistent history.
  const historyText=precisionInteger(records[1],2)+'/'+precisionInteger(records[0],2);
  return [bonus,fontSubmission(historyText,356,colorARGB)];
}

export function spellTitleDescriptors(spell,logicalPoses=[]) {
  if(!spell?.titleHex)return [];
  const bytes=titleBytes(spell.titleHex),text=new TextDecoder('shift-jis',{fatal:true}).decode(bytes);
  if(!text)return [];
  // text74 selects dynamic sprite7 (320x17). Titles stay attached during
  // interrupt1's outro even though Spell.active and the numeric draw stop.
  return logicalPoses.filter(p=>matchPose(p,'text',7)&&p.a.visible&&p.a.alpha>0).map(p=>{
    const offset=320-8*bytes.length;
    return {kind:'spell-title',text,titleHex:spell.titleHex,
      pose:{...p,a:{...p.a}},width:320,height:17,
      metrics:{width:320,height:17,sourceWidth:666,sourceHeight:36},
      record:{textHex:spell.titleHex,style:0,color:0xffffff,outlineColor:0,offset,spacing:0},
      // 460800 right aligns by CP932 byte count before44e660 doubles the
      // x coordinate. The MSG helper's default +2px x inset is absent here.
      raster:{fontFamily:'ＭＳ ゴシック',fontPixelHeight:32,fontWeight:400,
        sourceX:2*offset,sourceY:2,foregroundColor:0xffffff,outlineColor:0,
        outlineOffsets:[[2,4],[-2,4],[2,0],[-2,0]]}};
  });
}

export function spellTextDescriptors(spell,logicalPoses=[]) {
  return [...spellTitleDescriptors(spell,logicalPoses),...spellAsciiDescriptors(spell,logicalPoses)];
}
