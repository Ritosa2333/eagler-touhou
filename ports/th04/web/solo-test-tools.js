// Single-player test commands only. Never imported by local or network co-op.
export function readSoloDiagnostics(heap,at,patch){
  if(!patch.test_tools||!patch.diagnostics)return {};
  const data=at-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16;
  const v=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),f=patch.diagnostics,t=patch.test_tools;
  return {turbo:!!heap[data+f.turbo],rank:heap[data+f.rank],stageFrame:v.getUint16(data+f.stage_frame,true),
    slowFrames:v.getUint32(data+f.slow_frames,true),totalFrames:v.getUint32(data+f.total_frames,true),
    invincible:!!(heap[at+t.options]&1),stageRequest:heap[at+t.stage_request],testResult:heap[at+t.result],testUsed:!!heap[at+t.used]};
}

export function writeSoloTestCommand(heap,at,patch,{generation,invincible,stage}){
  const t=patch?.test_tools,size=patch?.mailbox_size;
  if(!t||!size||at<0||at+size>heap.length)throw Error('请先进入单机关卡。');
  const signature=new TextEncoder().encode(patch.signature);
  if(!signature.every((b,i)=>heap[at+i]===b))throw Error('测试工具的游戏状态已失效。');
  const v=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),mode=heap[at+22];
  if(![1,2].includes(mode)||generation!==v.getUint16(at+24,true))throw Error('请在当前单机关卡中使用测试工具。');
  if(invincible!==undefined&&typeof invincible!=='boolean')throw Error('无效的无敌设置。');
  if(stage!==undefined){
    const state=readSoloDiagnostics(heap,at,patch);
    if(!Number.isInteger(stage)||stage<1||stage>6)throw Error('请选择第 1～6 关。');
    if(mode!==1)throw Error('请先退出原版暂停菜单再跳关。');
    if(state.rank===4)throw Error('Extra 使用独立流程，普通关卡跳关仅用于主线。');
    if(heap[at+t.stage_request])throw Error('正在切换关卡，请稍候。');
  }
  if(invincible!==undefined)heap[at+t.options]=(heap[at+t.options]&~1)|(invincible?1:0);
  if(stage!==undefined){heap[at+t.result]=0;heap[at+t.stage_request]=stage;}
}

export function mountSoloTestTools({host,readState,command}){
  const panel=document.createElement('details');panel.dataset.soloTest='';
  panel.style.cssText='padding:8px 0';
  panel.innerHTML='<summary>单机测试：跳关与无敌</summary><p style="font-size:11px;line-height:1.5">进入关卡后使用。跳关从目标关卡开头加载，保留当前难度和机体；不改变 Turbo / Slow。无敌默认关闭，重新启动后重置。</p><div style="display:flex;gap:6px;align-items:center"><label style="flex:1">目标关卡 <select data-test-stage aria-label="跳关目标"></select></label><button type="button" data-test-jump disabled>跳关</button></div><button type="button" data-test-invincible aria-pressed="false" disabled style="margin-top:8px">无敌：关</button><p data-test-status role="status" style="font-size:11px;line-height:1.5">请先进入单机关卡。</p>';
  const select=panel.querySelector('[data-test-stage]'),jump=panel.querySelector('[data-test-jump]'),god=panel.querySelector('[data-test-invincible]'),output=panel.querySelector('[data-test-status]');
  for(let stage=1;stage<=6;stage++)select.add(new Option(`第 ${stage} 关`,String(stage)));
  select.value='4';host.querySelector('.layout-settings').append(panel);
  let last=-Infinity,pending=false,requested=null,message='';
  async function send(value){
    if(pending)return;pending=true;
    try{const before=readState();await command(value);if(value.stage)requested={stage:value.stage,generation:before.generation};message=value.stage?'正在从目标关卡开头加载…':value.invincible?'无敌已开启。':'无敌已关闭。';}
    catch(error){message=error.message;}finally{pending=false;update(performance.now(),true);}
  }
  jump.onclick=()=>send({stage:Number(select.value)});
  god.onclick=()=>send({invincible:!readState()?.invincible});
  function update(now,force=false){
    if(!force&&!panel.open)return;if(!force&&now-last<250)return;last=now;
    const s=readState(),active=!!s&&[1,2].includes(s.mode)&&s.invincible!==undefined;
    god.disabled=!active||pending;jump.disabled=!active||pending||s.mode!==1||s.rank===4||!!s.stageRequest;
    god.textContent=`无敌：${s?.invincible?'开':'关'}`;god.setAttribute('aria-pressed',String(!!s?.invincible));
    if(requested&&s?.testResult===2){message='当前流程无法跳关，请在正常战斗中重试。';requested=null;}
    if(requested&&s?.mode===1&&s.generation!==requested.generation&&s.stage+1===requested.stage){message=`已进入第 ${requested.stage} 关。`;requested=null;}
    if(!s&&!requested)message='';
    if(requested&&s?.mode===1&&!s.testUsed){message='';requested=null;}
    const detail=active?`当前第 ${s.stage+1} 关 · ${s.turbo?'Turbo':'Slow'}${s.testUsed?' · 测试局':''}`:'请先进入单机关卡。';
    output.textContent=message?`${message}\n${detail}`:detail;
  }
  panel.ontoggle=()=>update(performance.now(),true);
  return {update};
}
