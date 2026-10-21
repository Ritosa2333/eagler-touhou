import {LabController} from './controller.mjs';
import {OWNERS,STATUS,compactReport,keyOf} from './analyzer.mjs';
import {SOURCE_PATHS} from './owners.mjs';
import {createVisualReview} from '/common/testkit/presentation-lab/review-record.mjs';
import {captureReplayPairRun,compareReplayPairRuns,compareReplayCadenceRuns} from '/common/testkit/presentation-lab/replay-pair.mjs';
const $=id=>document.getElementById(id),identity=await fetch('/build.json').then(r=>r.json());
const displayReviewChecks=['DISPLAY-02','DISPLAY-04','DISPLAY-05','DISPLAY-06','DISPLAY-08'];
let controller=null,dataBuffer=null,report=null,selected=null,selectedGroup=null,busy=false,scanAbort=null,epoch=0,bootPending=null,readOnly=false,pendingReplay=null,pendingSfxEventProbe=false,displayBounds=new Map();
let runtimeHotkeyWindow=null,runtimeHotkeyHandler=null;
let sessionDataBuffer=null,pendingSessionIdentity=null;
let reviewMaterials=[];
const firstFrameReceipts=new Map();
function updateReviewControl(){
 $('reviewExport').disabled=!reviewMaterials.length||!$('reviewGroup').checked||!$('reviewVerdict').value;
}
function clearReviewVerdict(){
 $('reviewVerdict').value='';$('reviewNotes').value='';updateReviewControl();
}
function renderReviewMaterials(){
 const list=$('reviewMaterials');list.replaceChildren();
 if(!reviewMaterials.length){const row=document.createElement('li');row.textContent='暂无窗口素材';list.append(row);updateReviewControl();return;}
 for(const [index,item] of reviewMaterials.entries()){
  const row=document.createElement('li'),label=document.createElement('span'),remove=document.createElement('button');
  const source=JSON.parse(item.reportText);
  label.textContent=`${item.filename} | tick ${source.previousTick} → ${source.tick} | ${source.objects?.length??0} 个对象`;
  remove.type='button';remove.textContent='移除';remove.setAttribute('aria-label',`移除 ${item.filename}`);
  remove.addEventListener('click',()=>{reviewMaterials.splice(index,1);clearReviewVerdict();renderReviewMaterials();});
  row.append(label,remove);list.append(row);
 }
 updateReviewControl();
}
function invalidateReview(){
 reviewMaterials=[];clearReviewVerdict();renderReviewMaterials();
 $('reviewStatus').textContent='先准备多个场景窗口，再选择一次整组结论。';
}
function prepareReview(reportText,filename){
 if(reviewMaterials.some(item=>item.reportText===reportText)){
  $('reviewStatus').textContent='这份窗口报告已加入素材集。';return;
 }
 reviewMaterials.push({reportText,filename});clearReviewVerdict();renderReviewMaterials();
 $('reviewStatus').textContent=`已加入 ${filename}；新增窗口会清除先前结论。`;
}
async function contentIdentity(value){
 const bytes=value instanceof ArrayBuffer?new Uint8Array(value):Uint8Array.from(value);
 const sha256=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),b=>b.toString(16).padStart(2,'0')).join('');
 return {bytes:bytes.byteLength,sha256};
}
function productionCadenceController(c,rateHz){
 if(![60,120,144].includes(rateHz))throw Error('Production callback cadence must be 60, 120, or 144 Hz');
 const core=c.core,interval=1000/rateHz,logicalTicks=[];let timestampMs=0,callbackCount=0,prepared=false;
 let timingCursor=c.observer.readTiming();
 const readSequence=()=>c.observer.readReplayConsumerTrace().nextSequence;
 const cursorEvidence=timing=>({schema:timing?.schema??null,base:timing?.base??null,capacity:timing?.capacity??null,
  count:timing?.count??null,next:timing?.next??null,stride:timing?.stride??null,decodedRows:timing?.rows?.length??null});
 const scheduler={
  freeze(){
   const token=c.freeze();
   if(!prepared){core.sdl_loop_pause(0);timingCursor=c.observer.readTiming();prepared=true;}
   return token;
  },
  step(){
   if(!prepared)throw Error('Production callback scheduler was not armed by the frozen Runtime');
   const sequenceBefore=readSequence(),callbacks=[];
   for(let attempt=0;attempt<Math.ceil(rateHz/60)+4;attempt++){
    const beforeTiming=timingCursor;
    const now=timestampMs;let nativeResult=null,nativeError=null,afterTiming=null,timingReadError=null;
    try{nativeResult=c.frameAt(now);}catch(error){nativeError=String(error?.message??error);}
    try{afterTiming=c.observer.readTiming();}catch(error){timingReadError=String(error?.message??error);}
    const timing=afterTiming?.rows.at(-1),appendedExactlyOne=!!afterTiming&&!!timing&&
     afterTiming.base===beforeTiming.base&&afterTiming.capacity===beforeTiming.capacity&&afterTiming.stride===beforeTiming.stride&&
     afterTiming.next===(beforeTiming.next+1)%beforeTiming.capacity&&
     afterTiming.count===Math.min(beforeTiming.capacity,beforeTiming.count+1);
    if(nativeResult!==true||nativeError||timingReadError||!appendedExactlyOne)
     throw Error(`Production frame callback did not append exactly one native timing-ring row: ${JSON.stringify({rateHz,nowMs:now,nativeResult,nativeError,timingReadError,before:cursorEvidence(beforeTiming),after:cursorEvidence(afterTiming)})}`);
    const checkpoint=c.observer.readPairedCheckpoint(),replaySequence=checkpoint.replay.trace.nextSequence;
    const receipt={source:'GameHost::frame',timestampMs:now,deltaMs:timing.deltaMs,success:true,replaySequence,
     timing:{timestampMs:timing.timestampMs,deltaMs:timing.deltaMs,simulationFrame:timing.simulationFrame,flags:{...timing.flags}},
     timingRing:{before:{base:beforeTiming.base,capacity:beforeTiming.capacity,count:beforeTiming.count,next:beforeTiming.next,stride:beforeTiming.stride},
      after:{base:afterTiming.base,capacity:afterTiming.capacity,count:afterTiming.count,next:afterTiming.next,stride:afterTiming.stride}}};
    callbacks.push(receipt);timestampMs+=interval;callbackCount++;timingCursor=afterTiming;
    if(replaySequence===sequenceBefore+1){
     const logicalTick={logicalTick:logicalTicks.length+1,sequenceBefore,sequenceAfter:replaySequence,callbacks};
     logicalTicks.push(logicalTick);return logicalTick;
    }
    if(replaySequence!==sequenceBefore)
     throw Error(`Production callback advanced ${replaySequence-sequenceBefore} Replay ticks at ${rateHz} Hz`);
   }
   throw Error(`No single Replay update completed within the bounded ${rateHz} Hz callback window`);
  },
  preview(alpha,options){return c.preview(alpha,options);},
  summary(requestedTicks){
   const callbacks=logicalTicks.flatMap(row=>row.callbacks),timingRows=callbacks.map(row=>row.timing);
   const presented=timingRows.filter(row=>row.flags.presented),high=presented.filter(row=>row.flags.highRefresh),between=high.filter(row=>!row.flags.tickDue);
   return {schema:'eagler-presentation/native-frame-callback-cadence/1',source:'GameHost::frame',mode:'normal-fixed-60hz',rateHz,
    requestedTicks,logicalTicks:logicalTicks.map(row=>({...row,callbacks:row.callbacks.map(item=>({...item}))})),
    summary:{callbackCount:callbacks.length,timingSampleCount:timingRows.length,
     tickDueCount:timingRows.filter(row=>row.flags.tickDue).length,presentedCount:presented.length,
     highRefreshPresented:high.length,betweenTickPresentations:between.length,
     firstTimestampMs:timingRows[0]?.timestampMs??null,lastTimestampMs:timingRows.at(-1)?.timestampMs??null},
    claims:{controlledNativeSchedulerCadence:true,physicalRefreshVerified:false,browserCompositorSubmissionMeasured:false}};
  },
 };
 return scheduler;
}
const status=text=>{$('status').textContent=text;};
function showBuild(build,prefix='基线'){$('build').textContent=`${prefix} ${String(build?.commit||'未知').slice(0,12)}\nWASM ${String(build?.wasm||'未知').slice(0,16)}…`;}
showBuild(identity);
for(let i=1;i<OWNERS.length;i++){const option=document.createElement('option');option.value=String(i);option.textContent=OWNERS[i];$('owner').append(option);}
status(identity.dataAvailable?'已找到测试 DATA；点击「启动实验」。':'请选择自己的 th08.dat，再启动实验。');
const controls=['play','inspect','step','step10','stage1','alpha','scan','export','frames'];
function enable(on){for(const id of controls)$(id).disabled=!on;}
function failure(error){console.error(error);status(String(error?.message||error));$('health').className='notice error';$('health').textContent=String(error?.message||error);}
async function action(fn){if(busy)return;busy=true;try{return await fn();}catch(e){failure(e);}finally{busy=false;}}
function installRuntimeHotkeys(target){
 if(runtimeHotkeyWindow&&runtimeHotkeyHandler)runtimeHotkeyWindow.removeEventListener('keydown',runtimeHotkeyHandler,true);
 runtimeHotkeyWindow=target;runtimeHotkeyHandler=event=>{if(event.code==='F8'&&!event.repeat){event.preventDefault();event.stopImmediatePropagation();void action(markIncident);}};
 target.addEventListener('keydown',runtimeHotkeyHandler,true);
}
window.__eaglerPrepareManagedRuntimeDataV1=async request=>{
 if(request.game!=='th08'||request.epoch!==epoch||!sessionDataBuffer)throw Error('Diagnostic DATA session mismatch');
 return {buffer:sessionDataBuffer.slice(0)};
};
async function boot({replayBytes=null,sfxEventProbe=false}={}){
 if(bootPending)throw Error('Runtime is already starting');
 if(!dataBuffer){const response=await fetch('/input/th08.dat');if(!response.ok)throw Error('请选择自己的 th08.dat');dataBuffer=await response.arrayBuffer();}
 if(dataBuffer.byteLength<16||dataBuffer.byteLength>128*1024*1024)throw Error('Invalid TH08 DATA');
 if(replayBytes!==null&&(!Array.isArray(replayBytes)||replayBytes.length>16*1024*1024))throw Error('Invalid Replay input');
 if(controller)await controller.close();
 invalidateReview();
 sessionDataBuffer=dataBuffer.slice(0);
 pendingSessionIdentity={...identity,inputs:{data:await contentIdentity(sessionDataBuffer),
   replay:replayBytes===null?null:await contentIdentity(replayBytes)},
   environment:{browser:navigator.userAgent,platform:navigator.platform}};
 pendingReplay=replayBytes;pendingSfxEventProbe=sfxEventProbe===true;controller=null;report=null;selected=null;readOnly=false;enable(false);$('boot').disabled=true;$('empty').hidden=true;
 $('runtime').hidden=false;$('reportFrame').hidden=true;$('overlay').hidden=true;$('rows').replaceChildren();$('window').textContent='未采样';
 showBuild(identity);
 for(const id of ['observed','suspects','unknown'])$(id).textContent='—';
 status('正在加载隔离 Runtime 和本地资源…');
 const ready=new Promise((resolve,reject)=>{
  const timer=setTimeout(()=>reject(Error('Runtime preparation timeout')),120000);
  bootPending={resolve:v=>{clearTimeout(timer);resolve(v);},reject:e=>{clearTimeout(timer);reject(e);}};
 });
 epoch++;$('runtime').src=`/runtime/th08.html?managedData=1&gameGeneration=presentation-lab&runtimeEpoch=${epoch}&manual=1`;
 try{await ready;return controller;}finally{$('boot').disabled=false;bootPending=null;}
}
window.addEventListener('message',async event=>{
 if(event.source!==$('runtime').contentWindow||event.origin!==location.origin||event.data?.epoch!==epoch)return;
 const m=event.data;
 if(m.event==='error'){bootPending?.reject(Error(m.message||m.error));failure(m.message||m.error);return;}
 if(m.event==='first-frame'){
  const r=$('runtime').contentWindow.__th08Runtime;
  let nativePresentCount=null;
  try{const pointer=r.core.sdl_stats();nativePresentCount=new Uint32Array(r.core.memory.buffer,pointer,6)[5];}catch{}
  firstFrameReceipts.set(m.epoch,{schema:'th08/first-frame-receipt/1',epoch:m.epoch,event:'first-frame',
    nativePresentCount,receivedAtPerformanceMs:performance.now(),origin:event.origin});
  return;
 }
 if(m.event!=='ready'||!bootPending)return;
 try{
  const w=$('runtime').contentWindow,r=w.__th08Runtime;
  installRuntimeHotkeys(w);
  w.__th08SfxLatencyProbeEnabled=pendingSfxEventProbe;
  w.Date.now=()=>100000; // Reproducible initialization seed, isolated iframe only.
  await r.command({command:'configure',music:'none',options:{touchEnabled:false,thpracEnabled:false},sharedResources:[{path:'/msgothic.ttc',url:'/msgothic.ttc'}]});
  // FileHost snapshots saves into BrowserRuntime when sdl_game_open runs.
  // Import through the real FS API before launch, not a post-launch memory poke.
  if(pendingReplay!==null){await r.command({command:'write',path:'replay/th8_01.rpy',bytes:pendingReplay});pendingReplay=null;}
  await r.launch();controller=new LabController(r,{...pendingSessionIdentity,runtimeEpoch:epoch});controller.step(100);enable(true);
  status('已启动。可正常游玩，也可逐逻辑帧检查。');updateScene();bootPending.resolve(controller);
 }catch(e){bootPending?.reject(e);failure(e);}
});
function updateScene(){if(!controller)return;const c=controller.core,r=controller.runtime;$('scene').textContent=`场景 ${c.status(r.app,0)} / 页面 ${c.status(r.app,8)} / 光标 ${c.status(r.app,9)}`;}
async function captureReplayRun({replayBytes,role,ticks=1200,drawAlphas=[0,0.5,1],fixture='named-replay',cadenceHz=null,expectedAuthorityGroups}={}){
 if(!Array.isArray(replayBytes)||!replayBytes.length||replayBytes.length>16*1024*1024)throw Error('A bounded Replay byte array is required');
 if(!Number.isSafeInteger(expectedAuthorityGroups)||expectedAuthorityGroups<1)throw Error('Profile-declared authority group count is required');
 const c=await boot({replayBytes,sfxEventProbe:true}),core=c.core,r=c.runtime,navigation=[];
 await new Promise(resolve=>setTimeout(resolve,0));
 for(let i=0;i<12&&core.status(r.app,9)!==4;i++)c.press('ArrowDown');
 if(core.status(r.app,9)!==4)throw Error('Replay menu not reached through native title input');
 if(core.sdl_replay_trace_control_v1(1,1)!==1)throw Error('Native Replay consumer trace could not be enabled');
 for(let i=0;i<5&&core.status(r.app,0)!==2;i++){
  c.press('KeyZ');navigation.push([core.status(r.app,0),core.status(r.app,8),core.status(r.app,9)]);
 }
 if(core.status(r.app,0)!==2||!(c.trace()[4]&8))throw Error('Real Replay playback did not start: '+JSON.stringify(navigation));
 c.clearKeys();c.step(120);
 if(core.status(r.app,12)!==1||!core.status(r.app,20))throw Error('Selected Replay is not consuming in an active game');
 if(core.sdl_replay_trace_control_v1(1,1)!==1)throw Error('Native Replay trace reset failed before paired sampling');
 const scheduled=cadenceHz===null?null:productionCadenceController(c,cadenceHz);
 const runController=scheduled??c;runController.freeze();
 const replayIdentity=await contentIdentity(replayBytes),firstFrame=firstFrameReceipts.get(c.identity.runtimeEpoch)||null;
 const build={game:c.identity.game,commit:identity.commit,commonCommit:identity.commonCommit,wasm:identity.wasm,loader:identity.loader,
  sourceDigest:identity.sourceDigest,toolDigest:identity.toolDigest,toolSources:identity.toolSources,nativeAbi:c.identity.nativeAbi,
  adapterVersion:c.identity.adapterVersion,profile:'presentation-lab',diagnostic:true,instrumented:identity.instrumented===true,
  productionBuild:identity.productionBuild||null};
 const captured=await captureReplayPairRun({controller:runController,role,ticks,scope:{game:'th08',variant:'normal'},identity:{build,data:c.identity.inputs.data},replayIdentity,
  epoch:c.identity.runtimeEpoch,readCheckpoint:()=>c.observer.readPairedCheckpoint(),
  readAudioRowsSince:sequence=>c.observer.readSfxRowsSince(sequence),drawAlphas,expectedAuthorityGroups,
  onBatch:async()=>new Promise(resolve=>setTimeout(resolve,0))});
 if(scheduled)captured.scheduler=scheduled.summary(ticks);
 const receipt=firstFrame&&firstFrame.event==='first-frame'&&firstFrame.epoch===c.identity.runtimeEpoch&&
  firstFrame.origin===location.origin&&Number.isSafeInteger(firstFrame.nativePresentCount)&&firstFrame.nativePresentCount>0
  ?{status:'passed',...firstFrame}:{status:'incomplete',epoch:c.identity.runtimeEpoch,firstFrame};
 return {...captured,fixture,navigation,firstFrame:receipt,
   display07:{status:receipt.status,scope:'Runtime first-frame event followed a native successful presentation counter in this Runtime epoch',
    physicalScanoutVerified:false}};
}
function renderReport(value){
 report=value;selected=selectedGroup=null;$('window').textContent=`逻辑帧 ${value.previousTick} → ${value.tick}`;
 displayBounds=new Map();
 const detail=$('detail');detail.replaceChildren();const heading=document.createElement('h3');heading.textContent='选择一个对象';const hint=document.createElement('p');hint.textContent='点击问题组或对象框，查看本次报告中的具体属性与采样证据。';detail.append(heading,hint);
 showBuild(value.build,readOnly?'报告基线':'基线');
 $('observed').textContent=value.totalObjects??value.objects.length;$('suspects').textContent=value.issueGroups?.length??value.objects.filter(o=>o.severity>=3).length;
 $('unknown').textContent=value.objects.filter(o=>['unobserved','unscoped','ambiguous','lifecycle','offscreen'].includes(o.status)).length;
 const purityFailed=value.purityStatus?value.purityStatus==='fail':value.purity===false;
 const messages=[];if(!value.valid)messages.push('样本窗口不完整，不能作为完整覆盖结论');
 if(value.interpolationReady===false||value.gate===false)messages.push('当前状态未满足插值条件');
 if(!value.displayTiming?.samples)messages.push('当前报告为固定 α 检查，尚无正常显示循环记录');
 else if(!value.displayTiming.betweenTickPresentations)messages.push('当前显示循环记录中没有逻辑帧之间的额外呈现');
 if(purityFailed)messages.push('所监测的权威字段发生变化：'+[...new Set(value.stateChanges.map(s=>s.group))].join('、')+'；自动扫描已停止');
 else if(value.purityStatus==='unknown')messages.push('重画未改变已覆盖字段，但状态指纹仍有明确缺口，纯度结论为 unknown');
 if(value.negativeControl)messages.push('负对照：额外绘制保持当前位置并触发音频处理');
 if(value.worldFrozen)messages.push('原作暂停：世界运动不作漏插值判定');
 $('health').className='notice '+(purityFailed?'error':messages.length?'warn':'');
 $('health').textContent=messages.join('。')||'重画未改变所监测的权威字段。点击对象查看属性证据；不是全游戏通过。';
 $('coverage').textContent=JSON.stringify(value.coverage,null,2);$('overlay').hidden=false;$('alpha').value='1';$('alphaValue').textContent='1.00';
 if(readOnly){
  $('runtime').hidden=true;
  $('scene').textContent=`报告场景 ${value.scene?.status?.[0]??'未知'}`;
  const pictures=savedPictures();$('reportFrame').hidden=!pictures.length;$('empty').hidden=!!pictures.length;
  if(!pictures.length)$('empty').textContent='这份报告不含画面；右侧保留字段证据。';
  $('alpha').disabled=!pictures.length;if(pictures.length)showSavedFrame(1);
 }
 renderRows();drawBoxes();if(!readOnly)updateScene();$('export').disabled=false;
}
function savedPictures(){return (Array.isArray(report?.images)?report.images:[]).slice(0,5).filter(p=>Number.isFinite(p.alpha)&&typeof p.png==='string'&&/^data:image\/png;base64,[A-Za-z0-9+/=]+$/.test(p.png));}
function showSavedFrame(alpha){
 const pictures=savedPictures();if(!pictures.length)return;
 const nearest=pictures.reduce((a,b)=>Math.abs(b.alpha-alpha)<Math.abs(a.alpha-alpha)?b:a);
 $('reportFrame').src=nearest.png;$('alphaValue').textContent=`${nearest.alpha.toFixed(2)} · 保存的采样帧`;
 displayBounds=new Map((Array.isArray(nearest.boxes)?nearest.boxes:[]).slice(0,8192).map(b=>[b.key,b.bounds]));drawBoxes();
}
function visibleRows(){if(!report)return [];const query=$('search').value.trim().toLowerCase();return report.objects.filter(o=>(!$('owner').value||o.ownerId===Number($('owner').value))&&($('level').value==='all'||o.severity>=3)&&(!query||`${o.key} ${o.script} ${o.owner}`.toLowerCase().includes(query)));}
function renderRows(){
 const body=$('rows');body.replaceChildren();const rows=visibleRows();
 if($('level').value==='issues'&&report?.issueGroups){
  const keys=new Set(rows.map(o=>o.key));
  for(const g of report.issueGroups.filter(g=>g.exampleKeys.some(k=>keys.has(k))).slice(0,250)){
   const tr=document.createElement('tr'),a=document.createElement('td'),b=document.createElement('td'),c=document.createElement('td');
   if(g.key===selectedGroup?.key)tr.classList.add('selected');
   a.textContent=`${g.owner} · ${g.instanceCount} 个实例`;const small=document.createElement('small');small.textContent=`ANM ${g.anm} / script ${g.script}`;a.append(small);
   b.textContent=STATUS[g.status]||g.status;b.className=`s${g.severity}`;c.textContent=g.label;
   tr.append(a,b,c);tr.onclick=()=>{const o=report.objects.find(o=>o.key===g.exampleKey)||rows.find(o=>g.exampleKeys.includes(o.key));if(o)select(o,g);};body.append(tr);
  }
  if(body.children.length)return;
 }
 if(!rows.length){const tr=document.createElement('tr'),td=document.createElement('td');td.colSpan=3;td.textContent='当前筛选无疑似项。不代表未访问场景已覆盖。';tr.append(td);body.append(tr);return;}
 for(const o of rows.slice(0,250)){
  const tr=document.createElement('tr');if(o.key===selected?.key)tr.classList.add('selected');
  const a=document.createElement('td'),b=document.createElement('td'),c=document.createElement('td');
  a.textContent=`${o.owner} · ${o.object}`;const small=document.createElement('small');small.textContent=`ANM ${o.anm} / script ${o.script} / part ${o.part}`;a.append(small);
  b.textContent=STATUS[o.status]||o.status;b.className=`s${o.severity}`;
  c.textContent=o.fields.filter(f=>['missing-interpolation','held-review','non-idempotent','endpoint-mismatch','downstream-held'].includes(f.status)).map(f=>f.label).join('、')||'—';
  tr.append(a,b,c);tr.onclick=()=>select(o);body.append(tr);
 }
}
const format=value=>value?.map(n=>Number(n).toFixed(Math.abs(n)<.01?5:3)).join(', ')||'—';
function select(o,group=null){selected=o;selectedGroup=group;renderRows();drawBoxes();const detail=$('detail');detail.replaceChildren();const h=document.createElement('h3');h.textContent=`${o.owner} · 对象 ${o.object}`;detail.append(h);const p=document.createElement('p');p.textContent=`${SOURCE_PATHS[o.ownerId]||'尚未标记模块'} | ANM ${o.anm} / script ${o.script} / sprite ${o.sprite} / part ${o.part}`;detail.append(p);
 if(group){const note=document.createElement('p');note.textContent=`同组 ${group.instanceCount} 个实例；画框突出该组。下方是变化幅度最大的实例，可直接点击其他框检查。`;detail.append(note);}
 for(const f of o.fields.filter(f=>f.status!=='static')){
  const div=document.createElement('div');div.className='property';const head=document.createElement('strong');head.textContent=`${f.label}：${STATUS[f.status]||f.status}`;div.append(head);
  const values=document.createElement('pre');values.textContent=`前一逻辑帧  ${format(f.previous)}\n当前逻辑帧  ${format(f.current)}${f.authoredContinuous?'\n连续性依据：原作 ANM 持续操作或明确的运动 owner':''}`;div.append(values);
  const samples=document.createElement('div');samples.className='samples';for(const s of (f.samples||[]).slice(0,5)){const tag=document.createElement('span');tag.textContent=`α${s.alpha}: ${format(s.value).slice(0,100)}`;samples.append(tag);}div.append(samples);detail.append(div);
 }
 if(!o.fields.length){const p=document.createElement('p');p.textContent=STATUS[o.status]||o.status;detail.append(p);}
}
function drawBoxes(){
 const svg=$('overlay');svg.replaceChildren();if(!$('boxes').checked||!report)return;
 const rows=selectedGroup?report.objects.filter(o=>o.ownerId===selectedGroup.ownerId&&o.anm===selectedGroup.anm&&o.script===selectedGroup.script&&o.fields.some(f=>f.id===selectedGroup.field&&f.status===selectedGroup.status)).slice(0,100):selected?[selected]:visibleRows().filter(o=>o.status!=='offscreen').slice(0,100);
 for(const o of rows){const [l,t,r,b]=displayBounds.get(o.key)||o.bounds;if(![l,t,r,b].every(Number.isFinite)||r-l<=0||b-t<=0)continue;const rect=document.createElementNS('http://www.w3.org/2000/svg','rect');for(const [name,v] of Object.entries({x:l,y:t,width:r-l,height:b-t}))rect.setAttribute(name,v);rect.setAttribute('stroke',o.severity>=4?'#ff8a96':o.severity>=3?'#ebbe77':'#94d9bd');rect.onclick=()=>select(o);svg.append(rect);if(selected){const text=document.createElementNS(svg.namespaceURI,'text');text.setAttribute('x',Math.max(2,l));text.setAttribute('y',Math.max(13,t-4));text.textContent=`${o.owner} / ${o.object} / script ${o.script}`;svg.append(text);}}
}
function download(value,name){const url=URL.createObjectURL(new Blob([JSON.stringify(value,null,2)],{type:'application/json'}));const a=document.createElement('a');a.href=url;a.download=name;a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);}
function checked(){if(!controller||readOnly)throw Error('请先启动隔离 Runtime');return controller;}
async function inspect(){const value=checked().sweep({negativeControl:$('negative').checked});renderReport(value);status(`已采样 ${value.objects.length} 个绘制对象。`);return value;}
async function markIncident(){const value=checked().mark();renderReport(value);const response=await fetch('/incident',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(compactReport(value))});if(!response.ok)throw Error('F8 报告保存失败：'+await response.text());const saved=await response.json();status(`F8 已锁定：保存了异常前约 ${(value.timing.durationMs/1000).toFixed(1)} 秒的节奏与相机记录。告诉 agent「抓到了」即可。`);window.presentationLab.lastIncident={report:value,saved:saved.saved};return value;}
$('boot').onclick=()=>void action(boot);
$('play').onclick=()=>{try{checked().play();$('overlay').hidden=true;$('runtime').contentWindow.document.querySelector('canvas').focus();status('正在正常游玩；冻结并加入多个场景窗口后，再记录一次整组结论。');}catch(e){failure(e);}};
$('inspect').onclick=()=>void action(inspect);
for(const [id,n] of [['step',1],['step10',10]])$(id).onclick=()=>void action(async()=>{checked().step(n);return inspect();});
$('stage1').onclick=()=>void action(async()=>{const c=checked();if(c.core.status(c.runtime.app,0)!==1)throw Error('请在初始标题菜单使用；可重新启动实验。');for(let i=0;i<5;i++){c.press('KeyZ');await new Promise(r=>setTimeout(r,0));}c.step(241);return inspect();});
$('alpha').oninput=()=>{if(busy)return;try{const a=Number($('alpha').value);if(readOnly){showSavedFrame(a);return;}if(!controller)return;$('alphaValue').textContent=a.toFixed(2);const sample=checked().preview(a,{negativeControl:$('negative').checked});displayBounds=new Map(sample.records.map(r=>[keyOf(r),r.values.slice(21,25)]));drawBoxes();}catch(e){failure(e);}};
$('scan').onclick=()=>void action(async()=>{scanAbort=new AbortController();$('cancel').disabled=false;status('正在扫描；诊断耗时不代表正常游戏性能。');try{const scan=await checked().scan({signal:scanAbort.signal,negativeControl:$('negative').checked,onWindow:renderReport});window.presentationLab.lastScan=scan;status(scan.stoppedForPurity?'检测到状态变化，扫描已停止。':`完成 ${scan.reports.length} 个采样窗口。`);return scan;}finally{$('cancel').disabled=true;scanAbort=null;}});
$('cancel').onclick=()=>scanAbort?.abort();
$('export').onclick=()=>download(readOnly?report:{...checked().export(),scan:window.presentationLab.lastScan},`th08-presentation-${report?.tick||0}.json`);
$('frames').onclick=()=>void action(async()=>{const sourceLabel=report?.label||'manual-mark',display05Boundary=report?.display05Boundary||null;const value=checked().sweep({negativeControl:$('negative').checked,images:true,label:`${sourceLabel}/visual-window`});renderReport(value);const exported=compactReport(value);if(display05Boundary)exported.display05Boundary=display05Boundary;const safeLabel=String(value.label).replace(/[^a-z0-9-]+/gi,'-').slice(0,48),filename=`th08-presentation-${safeLabel}-${value.tick}.json`;const reportText=JSON.stringify(exported,null,2);download(exported,filename);prepareReview(reportText,filename);});
$('reviewExport').onclick=()=>void action(async()=>{
 if(!reviewMaterials.length)throw Error('请先加入场景窗口素材');
 if(!$('reviewGroup').checked)throw Error('请确认本次审查的是显示场景集');
 const record=await createVisualReview({materials:reviewMaterials.map(item=>({...item})),
  reviewer:{name:$('reviewer').value,kind:'human'},verdict:$('reviewVerdict').value,
  notes:$('reviewNotes').value,selectedChecks:$('reviewGroup').checked?displayReviewChecks:[],
  environment:{displayModel:$('reviewDisplay').value,refreshHz:Number($('reviewHz').value),
   browser:{name:$('reviewBrowser').value,version:$('reviewBrowserVersion').value},
   os:{name:$('reviewOS').value,version:$('reviewOSVersion').value}}});
 const last=JSON.parse(reviewMaterials.at(-1).reportText);
 download(record,`th08-visual-review-${last.tick}.json`);
 $('reviewStatus').textContent=`已导出一次结论，绑定 ${record.materials.length} 个窗口素材；请与全部报告文件一起保存。`;
});
$('negative').addEventListener('change',invalidateReview);
$('reviewGroup').addEventListener('change',clearReviewVerdict);
$('reviewVerdict').addEventListener('change',updateReviewControl);
for(const id of ['reviewer','reviewDisplay','reviewHz','reviewBrowser','reviewBrowserVersion','reviewOS','reviewOSVersion'])
 $(id).addEventListener('input',clearReviewVerdict);
for(const id of ['owner','level','search'])$(id).addEventListener('input',()=>{selected=selectedGroup=null;renderRows();drawBoxes();});$('boxes').onchange=drawBoxes;
$('zoom').onchange=()=>{const zoom=Number($('zoom').value);$('stage').style.width=640*zoom+'px';$('stage').style.height=480*zoom+'px';};
async function toggleFullscreen(){
 const viewport=$('gameViewport'),current=document.fullscreenElement||document.webkitFullscreenElement;
 if(current){if(document.exitFullscreen)await document.exitFullscreen();else document.webkitExitFullscreen?.();}
 else if(viewport.requestFullscreen)await viewport.requestFullscreen({navigationUI:'hide'});else if(viewport.webkitRequestFullscreen)viewport.webkitRequestFullscreen();else throw Error('当前浏览器不支持网页全屏');
}
function updateFullscreen(){const active=(document.fullscreenElement||document.webkitFullscreenElement)===$('gameViewport');$('fullscreen').textContent=active?'×':'⛶';$('fullscreen').setAttribute('aria-label',active?'退出全屏':'进入全屏');$('fullscreen').title=active?'退出全屏（Alt+Enter）':'进入全屏（Alt+Enter）';}
$('fullscreen').onclick=()=>void action(toggleFullscreen);
document.addEventListener('fullscreenchange',updateFullscreen);document.addEventListener('webkitfullscreenchange',updateFullscreen);
document.addEventListener('keydown',event=>{if(event.altKey&&event.code==='Enter'&&!event.repeat){event.preventDefault();void action(toggleFullscreen);}else if(event.code==='F8'&&!event.repeat){event.preventDefault();void action(markIncident);}});
$('data').onchange=async()=>{const f=$('data').files?.[0];if(!f)return;if(f.size>128*1024*1024){failure('DATA 文件过大');return;}dataBuffer=await f.arrayBuffer();status(`已选择 ${f.name}；重新启动实验即可使用。`);};
$('replay').onchange=()=>void action(async()=>{const f=$('replay').files?.[0];if(!f)return;if(f.size>16*1024*1024)throw Error('Replay 文件过大');await boot({replayBytes:Array.from(new Uint8Array(await f.arrayBuffer()))});status('已重新启动测试会话，Replay 已导入第一个槽位；请从游戏 Replay 菜单进入。');});
$('import').onchange=async()=>{try{const f=$('import').files?.[0];if(!f)return;if(f.size>32*1024*1024)throw Error('报告过大');const value=JSON.parse(await f.text()),r=value.last||value;if(r.schema!=='th08/presentation-audit/1'||!Array.isArray(r.objects)||r.objects.length>8192)throw Error('不支持的报告格式');controller?.freeze();readOnly=true;enable(false);renderReport(r);status('离线报告模式；重新启动实验回到游戏。');}catch(e){failure(e);}};
window.presentationLab={boot,get controller(){return controller;},get report(){return report;},identity,inspect,lastScan:null,renderReport,
  firstFrameReceipt:runtimeEpoch=>firstFrameReceipts.get(runtimeEpoch??epoch)||null,
  captureReplayRun,compareReplayPairRuns,compareReplayCadenceRuns};
const stageNames=['1面','2面','3面','4A · 灵梦','4B · 魔理沙','5面','6A · 永琳','6B · 辉夜','Extra · 妹红'];
let campaignFixtures=[];
function campaignStages(){
 const fixture=campaignFixtures.find(f=>f.id===$('campaignFixture').value);$('campaignStage').replaceChildren();
 for(const stage of fixture?.metadata.stageInfo||[]){const option=document.createElement('option');option.value=String(stage.stageIndex);option.textContent=stageNames[stage.stageIndex];$('campaignStage').append(option);}
 $('campaignGo').disabled=!fixture;
}
async function loadCampaign({fixtureId,stage,seconds=0}){
 const fixture=campaignFixtures.find(f=>f.id===fixtureId);
 if(!fixture||!fixture.metadata.stageInfo.some(s=>s.stageIndex===stage)||!Number.isFinite(seconds)||seconds<0||seconds>900)throw Error('请选择有效的录像、关卡与时间');
 const response=await fetch('/campaign/'+fixture.id+'.rpy');if(!response.ok)throw Error('本地录像不可用');
 const bytes=new Uint8Array(await response.arrayBuffer());
 const digest=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),b=>b.toString(16).padStart(2,'0')).join('');
 if(digest!==fixture.sha256)throw Error('录像哈希不匹配');
 const c=await boot({replayBytes:Array.from(bytes)}),core=c.core,r=c.runtime;
 for(let i=0;i<12&&core.status(r.app,9)!==4;i++)c.press('ArrowDown');
 if(core.status(r.app,9)!==4)throw Error('未进入 Replay 菜单');
 c.press('KeyZ');c.press('KeyZ');
 for(let i=0;i<10&&core.status(r.app,9)!==stage;i++)c.press('ArrowDown');
 if(core.status(r.app,9)!==stage)throw Error('原录像不含这一个关卡');
 for(let i=0;i<4&&core.status(r.app,0)!==2;i++)c.press('KeyZ');
 if(core.status(r.app,0)!==2||!(c.trace()[4]&8))throw Error('普通 Replay 未开始');
 c.clearKeys();c.freeze();let advanced=0;
 const info=()=>Array.from(new Int32Array(core.memory.buffer,core.audit_scene(),16));
 let s=info();
 while(s[0]===2&&s[1]===stage&&(s[4]<seconds*60||s[11])){
  for(let i=0;i<120&&(s[4]<seconds*60||s[11]);i++){
   if(core.sdl_loop_tick(r.app,1/60,17))throw Error('定位过程中游戏结束');advanced++;s=info();
   if(s[0]!==2||s[1]!==stage)break;
  }
  if(advanced>100000)throw Error('定位超过保护上限');
  status(`${fixture.listedPlayer} / ${stageNames[stage]}：${(s[4]/60).toFixed(1)} / ${seconds} 秒`);
  await new Promise(resolve=>setTimeout(resolve,0));
 }
 if(s[0]!==2||s[1]!==stage)throw Error('指定时间超过这份录像的关卡长度');
 const report=c.sweep({label:`${fixture.id}/${stageNames[stage]}/${s[4]}`});renderReport(report);
 status(`已定位：${fixture.listedPlayer} / ${stageNames[stage]} / ${(s[4]/60).toFixed(1)} 秒。点「继续游玩」看高刷。`);
 return report;
}
$('campaignFixture').onchange=campaignStages;
$('campaignGo').onclick=()=>void action(()=>loadCampaign({fixtureId:$('campaignFixture').value,stage:Number($('campaignStage').value),seconds:Number($('campaignSeconds').value)}));
window.presentationLab.loadCampaign=loadCampaign;
fetch('/campaign/manifest.json').then(r=>r.ok?r.json():null).then(corpus=>{
 campaignFixtures=corpus?.fixtures||[];$('campaignFixture').replaceChildren();
 for(const f of campaignFixtures){const option=document.createElement('option');option.value=f.id;option.textContent=`${f.metadata.difficulty===4?'Extra':'Lunatic'} / ${f.listedPlayer} / ${f.listedTeam}`;$('campaignFixture').append(option);}
 if(!campaignFixtures.length){const option=document.createElement('option');option.textContent='本服务未安装完整录像；仍可手动导入';$('campaignFixture').append(option);}
 campaignStages();
}).catch(()=>{$('campaignFixture').replaceChildren(new Option('本地录像列表不可用',''));});
