// Stage assets are fully read before the active core is changed.
export const stageMusic=Object.freeze([null,'th12_02','th12_05','th12_08','th12_10','th12_14','th12_17','th12_19']); // Original4aebf0+stage*64, field14.
export function stageResources(number,atlases={}){
  if(!Number.isInteger(number)||number<1||number>7)throw Error('关卡编号无效');
  const suffix=String(number).padStart(2,'0'),resources=[];
  for(const file of ['default.ecl',`stage${suffix}.ecl`,...(number===7?['stage07boss.ecl']:[])])resources.push({file,method:'_th12_load_ecl'});
  resources.push({file:`stage${suffix}.std`,method:'_th12_load_std'});
  for(let i=0;i<6;i++){resources.push({file:`pl0${i>>1}${i%2?'b':'a'}.sht`,method:'_th12_load_sht',index:i});resources.push({file:`st${suffix}_0${i>>1}${i%2?'b':'a'}.msg`,method:'_th12_load_msg',index:i});}
  for(const [index,name]of [[0,'bullet'],[1,'enemy'],[2,`stgenm${suffix}`],[4,'pl00'],[5,'pl01'],[6,'pl02'],[7,'front'],[8,'text'],[11,'ascii'],[9,`st${suffix}logo`],[10,`stage${suffix}`],...(atlases[`stgenm${suffix}m`]?[[3,`stgenm${suffix}m`]]:[])])resources.push({file:name+'.anm',method:'_th12_load_anm',index});
  return resources;
}
export class StageLoader {
  constructor(core,atlases,readBytes){this.core=core;this.atlases=atlases;this.readBytes=readBytes;this.cache=new Map();this.epoch=0;}
  async bytes(file){if(!this.cache.has(file))this.cache.set(file,Promise.resolve().then(()=>this.readBytes(file)).catch(error=>{this.cache.delete(file);throw error;}));return this.cache.get(file);}
  cancel(){this.epoch++;}
  async load(number,isCurrent=()=>true){
    const epoch=++this.epoch,resources=stageResources(number,this.atlases),bundle=await Promise.all(resources.map(async r=>({...r,bytes:await this.bytes(r.file)})));
    if(epoch!==this.epoch||!isCurrent())return false;
    const c=this.core;c._th12_select_stage(number);
    if(!resources.some(r=>r.method==='_th12_load_anm'&&r.index===3))c._th12_unload_anm?.(3);
    for(const resource of bundle){const p=c._malloc(resource.bytes.length);try{c.HEAPU8.set(resource.bytes,p);const method=c[resource.method];if(!method||(resource.index===undefined?method(p,resource.bytes.length):method(resource.index,p,resource.bytes.length))!==1)throw Error('原版资源格式无效 '+resource.file);}finally{c._free(p);}}
    this.number=number;return true;
  }
}
