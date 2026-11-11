// Shared by local launchers and synchronized room initialization.
export function stageSelection(value='normal'){
  if(value==='normal')return {stage:1,route:0,practice:false};
  if(typeof value!=='string'||! /^(s[1-5]|[mj](?:[6-9]|1[0-9]|20))$/.test(value))throw Error('起始关卡无效');
  return {stage:Number(value.slice(1)),route:value[0]==='j'?1:0,practice:true};
}
export function stageLabel(value='normal'){
  const s=stageSelection(value);if(!s.practice)return '正常流程 · 从第 1 关开始';
  const boss=s.stage===5?'神玉':s.stage===10?(s.route?'魅魔':'幽玄魔眼'):s.stage===15?(s.route?'菊理':'Elis'):s.stage===20?(s.route?'矜羯罗':'Sariel'):'';
  return `${s.stage<=5?'神社':s.route?'地狱':'魔界'} · 第 ${s.stage} 关${boss?' · '+boss:''}`;
}
export function populateStages(select){
  select.replaceChildren(new Option(stageLabel(),'normal'));
  for(const [prefix,title,start,end] of [['s','神社',1,5],['m','魔界',6,20],['j','地狱',6,20]]){
    const group=document.createElement('optgroup');group.label=title;
    for(let stage=start;stage<=end;stage++){const value=prefix+stage;group.append(new Option(stageLabel(value),value));}
    select.append(group);
  }
}
export function applyStage(core,value='normal'){
  const s=stageSelection(value);
  if(s.practice)core.th_stage_command(14,0,s.stage,s.route,0,0);
  core.th_start_stage_entrance();
}
