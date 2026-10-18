// Portable TH12 practice owner and fixed Wasm ABI. No host-side game warping.
import {sections, chapterCounts, source} from './sections.mjs';
export {sections, chapterCounts, source};
export const DEFAULT_PARAMS=Object.freeze({mode:1,stage:0,section:0,phase:0,dlg:false,score:0,
  life:8,life_fragment:0,bomb:8,bomb_fragment:0,power:400,value:10000,graze:0,ufo_side:0,
  ventra_1:0,ventra_2:0,ventra_3:0});
export const TOOL=Object.freeze({invulnerable:1,infiniteLives:2,infiniteBombs:4,infinitePower:8,
  timeLock:16,autoBomb:32,everlastingBgm:64,allClearBonus:128});
export const PRACTICE_SCHEMA='eagler-touhou/th12-practice/1';
const bounds={mode:[0,1],stage:[0,6],phase:[0,4],score:[0,9999999990],life:[0,8],life_fragment:[0,3],
  bomb:[0,8],bomb_fragment:[0,2],power:[0,400],value:[0,999990],graze:[0,999999],ufo_side:[0,1],
  ventra_1:[0,3],ventra_2:[0,3],ventra_3:[0,3]};
const integer=(value,min,max)=>Number.isSafeInteger(value)&&value>=min&&value<=max;
export function validateParams(input,difficulty=null) {
  const params={...DEFAULT_PARAMS,...input};
  for(const [key,[min,max]]of Object.entries(bounds))if(!integer(params[key],min,max))throw Error('TH12 练习参数无效：'+key);
  if(params.score%10||params.value%10||typeof params.dlg!=='boolean')throw Error('TH12 得分/最大得点须为10的倍数');
  if(!integer(params.section,0,19999))throw Error('TH12 分段编号无效');
  if(!params.ventra_1&&(params.ventra_2||params.ventra_3)||!params.ventra_2&&params.ventra_3)throw Error('请从第一个飞碟槽开始设置');
  if(!params.mode)return params;
  if(params.section>=10000){
    const stage=Math.floor((params.section-10000)/100),chapter=(params.section-10000)%100;
    if(stage!==params.stage+1||chapter<1||chapter>chapterCounts[params.stage].reduce((a,b)=>a+b)||params.phase)throw Error('道中章节与当前关卡不符');
  }else if(params.section){
    const section=sections.find(section=>section.id===params.section);
    if(!section||section.stage!==params.stage+1||params.phase>=section.phaseCount||difficulty!==null&&!section.names[difficulty]?.some(Boolean))throw Error('当前难度没有此分段或阶段');
    if(params.dlg&&!section.dialogue)throw Error('此分段没有可选对话');
  }else if(params.phase)throw Error('整关练习没有符卡阶段');
  return params;
}
export function parameterWords(input,{replay=false,difficulty=null}={}){
  const p=validateParams(input,difficulty),score=BigInt(p.score);
  return Int32Array.of(1,p.mode,p.stage,p.section,p.phase,p.dlg?1:0,Number(score&0xffffffffn),Number(score>>32n),
    p.life,p.life_fragment,p.bomb,p.bomb_fragment,p.power,p.value,p.graze,p.ufo_side,p.ventra_1,p.ventra_2,p.ventra_3,replay?1:0);
}
export function createPracticeMetadata(input,{assisted=false}={}){
  return {schema:PRACTICE_SCHEMA,game:'th12',params:validateParams(input),assisted:!!assisted,
    source:{referenceSha256:source.referenceSha256,definitionsRevision:source.revision}};
}
export function readPracticeMetadata(metadata){
  if(!metadata)return null;
  if(metadata.schema!==PRACTICE_SCHEMA||metadata.game!=='th12')throw Error('TH12 练习录像元数据格式不符');
  return {...metadata,params:validateParams(metadata.params)};
}
export class PracticeBridge {
  constructor(core){this.core=core;this.params={...DEFAULT_PARAMS};this.replay=false;this.lastSnapshot=null;}
  call(name,...args){const method=this.core[name];if(typeof method!=='function')throw Error('TH12 Runtime 缺少练习接口 '+name);return method(...args);}
  configure(input,{replay=false,difficulty=null}={}){
    const words=parameterWords(input,{replay,difficulty}),pointer=this.call('_malloc',words.byteLength);
    if(!pointer)throw Error('TH12 练习参数内存分配失败');
    try{
      this.core.HEAPU8.set(new Uint8Array(words.buffer),pointer);
      if(this.call('_th12_practice_configure',pointer,words.length)!==1)throw Error(this.snapshot()?.error||'练习参数被Runtime拒绝');
    }finally{this.call('_free',pointer);}
    this.params=validateParams(input,difficulty);this.replay=replay;return this.params;
  }
  start({character,shot,difficulty,seed}){
    if(this.call('_th12_practice_start',character,shot,difficulty,seed)!==1)throw Error(this.snapshot()?.error||'TH12 练习ECL补丁未提交');
    return this.snapshot();
  }
  setEnabled(enabled){this.call('_th12_practice_enabled',enabled?1:0);}
  setReplay(replay){this.call('_th12_practice_replay',replay?1:0);this.replay=!!replay;}
  reset(clearTools=false){this.call('_th12_practice_reset',clearTools?1:0);this.replay=false;this.params={...DEFAULT_PARAMS};}
  setTools(mask){if(this.replay)return false;this.call('_th12_practice_tools',mask&255);return true;}
  setSpeed({fps=60,slow=15,fast=60,debug=false}){this.call('_th12_practice_speed',fps,slow,fast,debug?1:0);}
  interval({replay=this.replay,fast=false,slow=false,debug=false}={}){return this.call('_th12_practice_interval',replay?1:0,fast?1:0,slow?1:0,debug?1:0);}
  snapshot(){
    const pointer=this.call('_th12_practice_state'),size=this.call('_th12_practice_state_size');
    if(!Number.isSafeInteger(pointer)||!Number.isSafeInteger(size)||pointer<0||size<2||size>1024*1024||pointer+size>this.core.HEAPU8.byteLength)throw Error('TH12 练习状态缓冲区无效');
    const text=new TextDecoder().decode(this.core.HEAPU8.subarray(pointer,pointer+size));
    try{return this.lastSnapshot=JSON.parse(text);}catch(error){throw Error('TH12 练习状态无法读取：'+error.message,{cause:error});}
  }
  metadata(){const state=this.snapshot();return createPracticeMetadata(state.params,{assisted:state.assisted});}
}
