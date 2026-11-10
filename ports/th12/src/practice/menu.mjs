import {DEFAULT_PARAMS,validateParams,sections,chapterCounts} from './bridge.mjs';
const clamp=(value,min,max)=>Math.min(max,Math.max(min,value));
const wrap=(value,length)=>(value%length+length)%length;
const LABELS={
  mode:['模式','Mode','モード'],stage:['关卡','Stage','ステージ'],warp:['跳转','Warp','ワープ'],
  section:['分段','Section','区間'],phase:['符卡阶段','Spell phase','スペル段階'],dlg:['对话','Dialogue','会話'],
  life:['残机','Lives','残機'],life_fragment:['残机碎片','Life pieces','残機の欠片'],
  bomb:['符卡','Bombs','ボム'],bomb_fragment:['符卡碎片','Bomb pieces','ボムの欠片'],power:['火力','Power','パワー'],
  ufo_side:['飞碟方向','UFO side','ベントラー方向'],ventra_1:['飞碟 1','Ventra 1','ベントラー1'],
  ventra_2:['飞碟 2','Ventra 2','ベントラー2'],ventra_3:['飞碟 3','Ventra 3','ベントラー3'],
  value:['最大得点','Point value','最大得点'],graze:['擦弹','Graze','グレイズ'],score:['得分','Score','スコア'],
  start:['开始练习','Start practice','練習開始'],cancel:['返回机体选择','Return to shot selection','装備選択に戻る'],
};
const WARPS=[['整关','Full stage','全ステージ'],['道中章节','Chapter','道中区間'],['道中BOSS','Midboss','道中ボス'],
  ['关底BOSS','Boss','ボス'],['非符','Nonspell','通常弾幕'],['符卡','Spellcard','スペルカード']];
const MODES=[['原版练习','Original','オリジナル'],['自定义练习','Custom','カスタム']];
const PHASES1=[['正常','Normal','普通'],['发狂','Rage','発狂']];
const PHASES3=[['P1','Phase 1','P1'],['P2','Phase 2','P2'],['P3','Phase 3','P3'],['P4','Phase 4','P4'],['发狂','Rage','発狂']];
const COLORS=[['无','None','なし'],['红','Red','赤'],['蓝','Blue','青'],['绿','Green','緑']];
const ranges={life:[0,9,1],life_fragment:[0,3,1],bomb:[0,9,1],bomb_fragment:[0,2,1],power:[0,400,1],
  value:[0,999990,10],graze:[0,999999,1],score:[0,9999999990,10]};
export function localeIndex(locale='zh-CN'){return locale==='en-US'?1:locale==='ja-JP'?2:0;}

// Replaces the original stage-select body at the same title lifecycle position.
// It only chooses real parameters; ECL mutation belongs to Practice.cpp.
export class PracticeMenu {
  constructor({params={},difficulty=1,locale='zh-CN'}={}){
    this.params=validateParams({...DEFAULT_PARAMS,...params});this.difficulty=difficulty>=0&&difficulty<=3?difficulty:1;this.locale=locale;
    this.warp=this.params.section>=10000?1:this.params.section?sections[this.params.section-1].group===1?2:3:0;
    this.chapter=this.params.section>=10000?this.params.section%100:1;
    this.selection=0;this.repeat=new Map();this.previous=0;this.notice='';
  }
  setLocale(locale){this.locale=locale;}
  choices(){
    const p=this.params,d=p.stage===6?4:this.difficulty;
    return sections.filter(section=>section.stage===p.stage+1&&section.names[d]?.some(Boolean)&&
      (this.warp===2?section.group===1:this.warp===3?section.group===2:this.warp===4?!section.spell:this.warp===5?section.spell:true));
  }
  normalizeSection(){
    const p=this.params;
    if(this.warp===0)p.section=0;
    else if(this.warp===1){this.chapter=clamp(this.chapter,1,chapterCounts[p.stage].reduce((a,b)=>a+b));p.section=10000+(p.stage+1)*100+this.chapter;}
    else {const choices=this.choices();if(!choices.some(section=>section.id===p.section))p.section=choices[0]?.id??0;}
    const selected=sections.find(section=>section.id===p.section);
    p.phase=clamp(p.phase,0,(selected?.phaseCount??1)-1);if(!selected?.dialogue)p.dlg=false;
  }
  rows(){
    this.normalizeSection();const p=this.params,l=localeIndex(this.locale),out=[];
    const row=(id,value,values=null)=>out.push({id,label:LABELS[id][l],value,values});
    row('mode',MODES[p.mode][l]);row('stage',p.stage===6?'Extra':`Stage ${p.stage+1}`);
    if(p.mode){
      row('warp',WARPS[this.warp][l]);
      const section=sections.find(section=>section.id===p.section),d=p.stage===6?4:this.difficulty;
      if(this.warp){
        if(this.warp===1){const before=chapterCounts[p.stage][0];row('section',p.stage===5?String(this.chapter):`${this.chapter<=before?1:2} - ${this.chapter<=before?this.chapter:this.chapter-before}`);}
        else row('section',section?.names[d]?.[l]??'—');
        if(section?.phaseCount>1)row('phase',(section.phaseCount===5?PHASES3:PHASES1)[p.phase][l]);
        if(section?.dialogue)row('dlg',p.dlg?['开','On','有効'][l]:['关','Off','無効'][l]);
      }
      for(const id of ['life','life_fragment','bomb','bomb_fragment'])row(id,String(p[id]));
      row('power',(p.power/100).toFixed(2));row('ufo_side',p.ufo_side?['右','Right','右'][l]:['左','Left','左'][l]);
      row('ventra_1',COLORS[p.ventra_1][l]);
      if(p.ventra_1)row('ventra_2',COLORS[p.ventra_2][l]);
      if(p.ventra_1&&p.ventra_2)row('ventra_3',COLORS[p.ventra_3][l]);
      for(const id of ['value','graze','score'])row(id,String(p[id]));
    }
    row('start','Z / Enter');row('cancel','X / Esc');this.selection=clamp(this.selection,0,out.length-1);return out;
  }
  change(id,delta,{fast=false}={}){
    const p=this.params;
    if(id==='mode')p.mode=1-p.mode;
    else if(id==='stage'){p.stage=wrap(p.stage+delta,7);p.section=p.phase=0;this.chapter=1;}
    else if(id==='warp'){this.warp=wrap(this.warp+delta,WARPS.length);p.section=p.phase=0;this.chapter=1;}
    else if(id==='section'){
      if(this.warp===1)this.chapter=clamp(this.chapter+delta,1,chapterCounts[p.stage].reduce((a,b)=>a+b));
      else {const choices=this.choices(),index=choices.findIndex(section=>section.id===p.section);p.section=choices[wrap(index+delta,choices.length)]?.id??0;p.phase=0;}
    }else if(id==='phase'){const section=sections.find(section=>section.id===p.section);p.phase=wrap(p.phase+delta,section?.phaseCount??1);}
    else if(id==='dlg')p.dlg=!p.dlg;
    else if(id==='ufo_side')p.ufo_side=1-p.ufo_side;
    else if(id.startsWith('ventra_')){p[id]=wrap(p[id]+delta,4);if(!p.ventra_1)p.ventra_2=p.ventra_3=0;if(!p.ventra_2)p.ventra_3=0;}
    else if(ranges[id]){const [min,max,step]=ranges[id];p[id]=clamp(p[id]+delta*step*(fast?100:1),min,max);}
    this.normalizeSection();this.rows();
  }
  activate(id){
    if(id==='cancel')return {type:'cancel'};
    if(id!=='start'){this.change(id,1);return null;}
    try {return {type:'start',params:validateParams(this.params,this.params.stage===6?4:this.difficulty),difficulty:this.params.stage===6?4:this.difficulty};}
    catch(error){this.notice=error.message;return null;}
  }
  tick(held,pressed=held&~this.previous){
    const rising=pressed|(held&~this.previous);this.previous=held;let direction=rising&0xf0;
    for(const bit of [16,32,64,128]){const age=held&bit?(this.repeat.get(bit)??0)+1:0;this.repeat.set(bit,age);if(age>=26&&(age-26)%4===0)direction|=bit;}
    const rows=this.rows();if(direction&16)this.selection=wrap(this.selection-1,rows.length);if(direction&32)this.selection=wrap(this.selection+1,rows.length);
    const id=rows[this.selection]?.id;if(direction&64)this.change(id,-1,{fast:!!(held&8)});if(direction&128)this.change(id,1,{fast:!!(held&8)});
    if(rising&0x102)return {type:'cancel'};
    if(rising&0x80001)return this.activate(this.rows()[this.selection]?.id);
    return null;
  }
}
