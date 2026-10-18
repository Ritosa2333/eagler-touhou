import {sections} from './practice-sections.mjs';
export const fields=['mode','stage','warp','section','phase','frame','dlg','score','life','bomb','power','gauge','graze','point','point_total','point_stage','time','value','night','familiar','rank','rankLock'];
export const defaults=Object.freeze({mode:1,stage:0,warp:0,section:0,phase:0,frame:0,dlg:false,score:0,life:2,bomb:8,power:128,gauge:0,graze:0,point:0,point_total:0,point_stage:0,time:0,value:60000,night:0,familiar:0,rank:12,rankLock:false});
const ranges={mode:[0,1],stage:[0,8],warp:[0,6],section:[0,19999],phase:[0,6],frame:[0,2147483647],score:[0,9999999990],life:[0,8],bomb:[0,8],power:[0,128],gauge:[-10000,10000],graze:[0,2147483647],point:[0,9999],point_total:[0,9999],point_stage:[0,9999],time:[0,2147483647],value:[0,9999999],night:[0,11],familiar:[0,2000],rank:[8,99]};
const chapters=[2,4,3,6,6,5,2,2,7];
export function normalizePractice(input={}){
 const out={...defaults};for(const key of fields){if(typeof defaults[key]==='boolean')out[key]=input[key]===true;else{const value=Number(input[key]),[minimum,maximum]=ranges[key];out[key]=Number.isFinite(value)?Math.max(minimum,Math.min(maximum,Math.trunc(value))):defaults[key];}}
 if(out.section>=10000){if(Math.floor((out.section-10000)/100)!==out.stage+1||out.section%100<1||out.section%100>chapters[out.stage])out.section=0;}
 else if(out.section){const section=sections.find(value=>value.id===out.section);if(!section||section.stage!==out.stage)out.section=0;}
 out.score=Math.floor(out.score/10)*10;out.value=Math.floor(out.value/10)*10;return out;
}
