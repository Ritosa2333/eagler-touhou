// This is the portable semantic field contract, not THPrac's native raw struct.
export const DAMAGE_SCHEMA='eagler-touhou/th12-damage-sources/1';
export const DAMAGE_LAYOUT='th12-portable-damage-source/1';
const MAGIC=0x32315344,VERSION=1,HEADER=6,STRIDE=18,CAPACITY=128;
const floatView=new DataView(new ArrayBuffer(4));
const floating=word=>{floatView.setInt32(0,word,true);return floatView.getFloat32(0,true);};
const fail=text=>{throw Error('TH12 伤害源录像数据无效：'+text);};
export function validateDamagePacket(input,stage=null){
  if(!Array.isArray(input)&&!(input instanceof Int32Array))fail('需要版本化整数数组');
  if(input.length<HEADER||input.length>HEADER+CAPACITY*STRIDE||Array.from(input).some(value=>!Number.isInteger(value)||value< -0x80000000||value>0x7fffffff))fail('整数长度或范围');
  const words=Int32Array.from(input),count=words[4],nextId=words[3]>>>0;
  if(words[0]!==MAGIC||words[1]!==VERSION||words[2]<2||words[2]>6||stage!==null&&words[2]!==stage||
    !nextId||count<0||count>CAPACITY||words[5]!==STRIDE||words.length!==HEADER+count*STRIDE)fail('不兼容的字段布局或关卡');
  const ids=new Set();
  for(let index=0;index<count;++index){const p=HEADER+index*STRIDE,id=words[p]>>>0,flags=words[p+16]>>>0;
    if(!id||id>=nextId||ids.has(id)||!(flags&1)||(flags&~3)||words[p+12]<0||words[p+13]<0||words[p+14]<0||words[p+15]<0||words[p+12]>0x7fffffff-words[p+13])fail('伤害源标志、计数或身份');
    ids.add(id);for(const offset of [1,2,3,4,5,6,7,8,9,17])if(!Number.isFinite(floating(words[p+offset])))fail('非有限坐标/计时');
    const remaining=floating(words[p+17]);if(remaining< -0x80000000+16384||remaining>0x7fffffff-16384)fail('计时器不可表示');
  }
  return words;
}
export function validateDamageMetadata(input){
  if(!input)return null;
  if(input.schema!==DAMAGE_SCHEMA||input.layout!==DAMAGE_LAYOUT||input.character!==0||input.shot!==0||!Array.isArray(input.stages)||input.stages.length>5)fail('未知schema、原始native blob或机体');
  const seen=new Set(),stages=[];
  for(const record of input.stages){if(!record||!Number.isInteger(record.number)||record.number<2||record.number>6||seen.has(record.number))fail('无效或重复关卡');const words=validateDamagePacket(record.words,record.number);seen.add(record.number);stages.push({number:record.number,words:Array.from(words)});}
  stages.sort((a,b)=>a.number-b.number);
  return {schema:DAMAGE_SCHEMA,layout:DAMAGE_LAYOUT,character:0,shot:0,stages};
}
export class DamageSourceBridge {
  constructor(core){this.core=core;this.identity=null;}
  call(name,...args){const fn=this.core[name];if(typeof fn!=='function')throw Error('TH12 Runtime 缺少伤害源接口 '+name);return fn(...args);}
  begin({replay=false,practice=false,character,shot,metadata=null}){
    const normalized=validateDamageMetadata(metadata);
    if(normalized&&!replay)fail('录制会话不能导入录像伤害源');
    if(normalized&&(character!==0||shot!==0||practice))fail('仅完整流程灵梦A录像可以恢复此字段');
    if(this.call('_th12_practice_damage_begin',replay?1:0,practice?1:0,character,shot)!==1)throw Error(this.state().error||'TH12 伤害源会话创建失败');
    this.identity={replay,practice,character,shot};
    if(normalized)for(const record of normalized.stages)this.load(record.words);
    return this.state();
  }
  load(input){
    const words=validateDamagePacket(input),pointer=this.call('_malloc',words.byteLength);
    if(!pointer)throw Error('TH12 伤害源数据内存不足');
    try{this.core.HEAPU8.set(new Uint8Array(words.buffer),pointer);if(this.call('_th12_practice_damage_load',pointer,words.length)!==1)throw Error(this.state().error||'TH12 伤害源恢复被拒绝');}
    finally{this.call('_free',pointer);}
  }
  stage(number){
    const pointer=this.call('_th12_practice_damage_stage',number),count=this.call('_th12_practice_damage_stage_words',number);
    if(!count)return null;
    const bytes=this.core.HEAPU8.subarray(pointer,pointer+count*4),view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
    const words=Array.from({length:count},(_,index)=>view.getInt32(index*4,true));return {number,words:Array.from(validateDamagePacket(words,number))};
  }
  metadata(){
    if(!this.identity||this.identity.character!==0||this.identity.shot!==0||this.identity.practice||!this.state().applicable)return null;
    const stages=[2,3,4,5,6].map(stage=>this.stage(stage)).filter(Boolean);
    return validateDamageMetadata({schema:DAMAGE_SCHEMA,layout:DAMAGE_LAYOUT,character:0,shot:0,stages});
  }
  state(){
    const pointer=this.call('_th12_practice_damage_state'),size=this.call('_th12_practice_damage_state_size');
    if(!Number.isSafeInteger(pointer)||!Number.isSafeInteger(size)||pointer<0||size<2||size>1024*1024||pointer+size>this.core.HEAPU8.byteLength)throw Error('TH12 伤害源状态缓冲区无效');
    try{return JSON.parse(new TextDecoder().decode(this.core.HEAPU8.subarray(pointer,pointer+size)));}catch(error){throw Error('TH12 伤害源状态无法读取：'+error.message,{cause:error});}
  }
  reset(){this.call('_th12_practice_damage_reset');this.identity=null;}
}
