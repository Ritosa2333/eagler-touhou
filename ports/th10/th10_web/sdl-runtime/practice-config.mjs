import {sections} from './practice-sections.mjs';
export const fields=['mode','stage','warp','section','phase','frame','dlg','score','life','power','faith','faith_bar','st6_boss9_spd','real_bullet_sprite'];
export const defaults=Object.freeze({mode:1,stage:0,warp:0,section:0,phase:0,frame:0,dlg:false,score:0,life:9,power:100,faith:50000,faith_bar:130,st6_boss9_spd:160,real_bullet_sprite:false});
const ranges={mode:[0,1],stage:[0,6],warp:[0,5],section:[0,19999],phase:[0,1],frame:[0,2147483647],score:[0,9999999990],life:[0,9],power:[0,100],faith:[0,999990],faith_bar:[0,130],st6_boss9_spd:[-1,160]};
const chapters=[5,5,7,8,6,4,6];
export function normalizePractice(input={}){
 const out={...defaults};for(const key of fields){if(typeof defaults[key]==='boolean')out[key]=input[key]===true;else{const value=Number(input[key]),[minimum,maximum]=ranges[key];out[key]=Number.isFinite(value)?Math.max(minimum,Math.min(maximum,Math.trunc(value))):defaults[key];}}
 if(out.section>=10000){if(Math.floor((out.section-10000)/100)!==out.stage+1||out.section%100<1||out.section%100>chapters[out.stage])out.section=0;}
 else if(out.section){const section=sections.find(value=>value.id===out.section);if(!section||section.stage!==out.stage)out.section=0;}
 out.score=Math.floor(out.score/10)*10;out.faith=Math.floor(out.faith/10)*10;return out;
}
