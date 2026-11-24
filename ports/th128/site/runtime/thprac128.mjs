// TH128-specific thprac parameters and real native boundary. MIT upstream attribution in licenses/THPRAC-MIT.txt.
import {THPRAC128_SECTIONS,THPRAC128_GLOSSARY} from './thprac128-catalog.mjs';
export {THPRAC128_SECTIONS};
export const THPRAC128_SCHEMA='eagler-touhou/thprac-session/1';
export const THPRAC128_DEFAULTS=Object.freeze({mode:1,stage:0,section:0,phase:0,dlg:false,score:0,motivation:100000,perfect_freeze:30000,ice_power:10000,ice_area:3000});
export const THPRAC128_CHAPTERS=Object.freeze([[5,2],[5,4],[4,3],[4,3],[5,4],[5,2],[5,3],[4,3],[4,3],[5,3],[4,2],[5,3],[5,4],[5,4],[5,3],[5,5]].map(Object.freeze));
export const THPRAC128_STAGE_MAP=Object.freeze([[[0,1,2],[0,3,4]],[[5,6,7],[5,8,9]],[[10,11,12],[10,13,14]],[[15,15,15],[15,15,15]]].map(paths=>Object.freeze(paths.map(Object.freeze))));
const ranges={mode:[0,1],stage:[0,15],section:[0,19999],phase:[0,3],score:[0,9999999990],motivation:[0,100000],perfect_freeze:[0,30000],ice_power:[0,10000],ice_area:[0,3000]},owned=new WeakSet();
const integer=(v,min,max,key)=>{if(!Number.isSafeInteger(v)||v<min||v>max)throw new TypeError('TH128 '+key+' requires an integer '+min+'..'+max);return v;};
const stageBase=s=>s<5?0:s<10?5:s<15?10:15;
export function thprac128Label(key,locale='zh'){return THPRAC128_GLOSSARY[key]?.[locale==='en'?1:locale==='ja'?2:0]??key;}
export function thprac128SectionChoices(stage,warp,locale='zh'){
 integer(stage,0,15,'stage');integer(warp,0,5,'warp');const language=locale==='en'?1:locale==='ja'?2:0;
 if(warp===0)return [];
 if(warp===1){const [first,last]=THPRAC128_CHAPTERS[stage];return Array.from({length:first+last},(_,n)=>({value:n+1,label:thprac128Label(first&&last?(n<first?'TH_STAGE_PORTION_1':'TH_STAGE_PORTION_2'):'TH_STAGE_PORTION_N',locale).replace('%d',String(n<first?n+1:n-first+1))}));}
 return THPRAC128_SECTIONS.filter(s=>s.stage===stage&&(warp===2?s.mid:warp===3?!s.mid:warp===4?!s.spell:s.spell)).map(s=>({value:s.id,label:s.labels[language]}));
}
export function createThprac128Session(input={},difficulty=1){
 if(!input||typeof input!=='object'||Array.isArray(input)||![Object.prototype,null].includes(Object.getPrototypeOf(input))||Object.getOwnPropertySymbols(input).length)throw new TypeError('TH128 params require a data-only object');
 const descriptors=Object.getOwnPropertyDescriptors(input),p={...THPRAC128_DEFAULTS};
 for(const [key,d] of Object.entries(descriptors)){if(!Object.hasOwn(p,key)||!Object.hasOwn(d,'value'))throw new TypeError('Unsupported or accessor TH128 field: '+key);p[key]=d.value;}
 for(const [key,[min,max]]of Object.entries(ranges))integer(p[key],min,max,key);
 if(typeof p.dlg!=='boolean')throw new TypeError('TH128 dlg requires boolean');
 integer(difficulty,0,4,'difficulty');if(p.stage===15?difficulty!==4:difficulty===4)throw new TypeError('TH128 stage/rank mismatch');
 if(p.mode===0){Object.assign(p,THPRAC128_DEFAULTS,{mode:0,stage:stageBase(p.stage)});}
 else{
  if(p.section>=10000){const target=Math.floor((p.section-10000)/100),chapter=p.section%100,[a,b]=THPRAC128_CHAPTERS[p.stage];if(target!==p.stage+1||chapter<1||chapter>a+b||p.phase||p.dlg)throw new TypeError('TH128 chapter/stage/phase/dialogue mismatch');}
  else if(p.section){const section=THPRAC128_SECTIONS[p.section-1];if(!section||section.stage!==p.stage||p.phase>section.phaseMax||p.dlg&&!section.dlg)throw new TypeError('TH128 section/stage/phase/dialogue mismatch');}
  else if(p.phase||p.dlg)throw new TypeError('TH128 beginning has no phase/dialogue override');
 }
 p.score=Math.floor(p.score/10)*10;const result=Object.freeze({schema:THPRAC128_SCHEMA,game:'th128',params:Object.freeze(p),difficulty});owned.add(result);return result;
}
function session(value){if(!owned.has(value))throw new TypeError('TH128 session must be created by its strict codec');return value;}
export function thprac128Payload(value){const s=session(value),p=s.params;if(!p.mode)return null;return Int32Array.of(1,p.stage,p.section,p.phase,p.dlg?1:0,s.difficulty,p.score/10,p.motivation,p.perfect_freeze,p.ice_power,p.ice_area);}
export function thprac128RuntimeSupported(module){return typeof module?._th128_world_prepare_thprac128==='function'&&typeof module?._th128_world_thprac128_state==='function';}
export function prepareThprac128Native(module,value){
 const payload=thprac128Payload(value);if(!payload)return null;if(!thprac128RuntimeSupported(module))throw new Error('TH128 thprac native runtime is unavailable');
 const pointer=module._malloc(payload.byteLength);if(!pointer)throw new Error('TH128 practice memory allocation failed');
 try{module.HEAPU8.set(new Uint8Array(payload.buffer),pointer);if(module._th128_world_prepare_thprac128(pointer,payload.byteLength)!==1)throw new Error(module.UTF8ToString(module._th128_world_error())||'TH128 native preparation rejected');}
 finally{module._free(pointer);}
}
export function readThprac128Native(module,value){
 const s=session(value),p=s.params;if(!p.mode)throw new Error('Normal mode has no practice native receipt');
 if(!thprac128RuntimeSupported(module))throw new Error('TH128 thprac native runtime is unavailable');
 const r=JSON.parse(module.UTF8ToString(module._th128_world_thprac128_state())),chapter=p.section>=10000?p.section%100:p.section?0:1,section=p.section>0&&p.section<10000?p.section:0;
 if(r.schema!=='th128-thprac-native-state/1'||r.active!==true||r.stage!==p.stage||r.chapter!==chapter||r.section!==section||r.phase!==p.phase||r.dlg!==p.dlg||r.difficulty!==s.difficulty||!Number.isSafeInteger(r.generation)||r.generation<1||r.frame!==0||r.nonNormal!==true||r.contexts!==15)throw new Error('TH128 native practice identity/taint/context receipt rejected');
 for(const key of ['score','motivation','perfect_freeze','ice_power','ice_area']){if(r.initial?.[key]!==p[key])throw new Error('TH128 initial practice resource receipt mismatch: '+key);if(!r.continued&&r.actual?.[key]!==p[key])throw new Error('TH128 actual native resource receipt mismatch: '+key);}
 if(r.continued){const a=r.actual;if(!a||!Number.isSafeInteger(a.score)||a.score<0||a.score>9999999990||a.score%10||!Number.isSafeInteger(a.motivation)||a.motivation<0||a.motivation>100000||!Number.isSafeInteger(a.perfect_freeze)||a.perfect_freeze<0||a.perfect_freeze>30000||!Number.isSafeInteger(a.ice_power)||a.ice_power<0||a.ice_power>10000||!Number.isFinite(a.ice_area)||a.ice_area<0||a.ice_area>1e9)throw new Error('Practice carried actual initial resources outside native domain');}
 return Object.freeze(r);
}
export function thprac128Presentation(value){const p=session(value).params,s=p.section>0&&p.section<10000?THPRAC128_SECTIONS[p.section-1]:null;return{skipIntro:!!p.mode&&p.section!==0&&!(p.section>=10000&&p.section%100===1),bossMusic:!!p.mode&&!p.dlg&&s?.bgm===1};}
