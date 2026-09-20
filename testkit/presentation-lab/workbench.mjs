const $=id=>document.getElementById(id);
const config=(await import('/lab-config.mjs')).default;
const identity=await fetch('/build.json').then(response=>{
  if(!response.ok)throw Error('Presentation Lab build identity unavailable');
  return response.json();
});
let controller=null,dataBuffer=null,busy=false,epoch=0,bootPending=null,report=null;

document.title=`${config.game.toUpperCase()} · Presentation Lab`;
$('game').textContent=config.game.toUpperCase();
$('heading').textContent=config.title||'帧间显微镜';
$('dataLabel').firstChild.textContent=config.dataLabel||`选择自己的 ${config.game}.dat`;
$('build').textContent=`WASM ${String(identity.wasm||'unknown').slice(0,16)}…`;
const status=text=>{$('status').textContent=text;};
const fail=error=>{console.error(error);status(String(error?.message||error));$('health').className='error';$('health').textContent=String(error?.message||error);};
async function action(work){if(busy)return;busy=true;try{return await work();}catch(error){fail(error);}finally{busy=false;}}
function setEnabled(value){for(const id of ['play','inspect','step','step10','scan','frames'])$(id).disabled=!value;}
function download(value,name){const url=URL.createObjectURL(new Blob([JSON.stringify(value,null,2)],{type:'application/json'})),link=document.createElement('a');link.href=url;link.download=name;link.click();setTimeout(()=>URL.revokeObjectURL(url),1000);}
function checked(){if(!controller)throw Error('请先启动隔离 Runtime');return controller;}

window.__eaglerPrepareManagedRuntimeDataV1=async request=>{
  if(request.game!==config.game||request.epoch!==epoch||!dataBuffer)throw Error('Diagnostic DATA session mismatch');
  return {buffer:dataBuffer.slice(0)};
};

async function loadDefaultData(){
  if(dataBuffer)return;
  const response=await fetch(config.dataUrl||`/input/${config.game}.dat`);
  if(!response.ok)throw Error(`请选择自己的 ${config.game}.dat`);
  dataBuffer=await response.arrayBuffer();
}

async function boot({replayBytes=null}={}){
  if(bootPending)throw Error('Runtime is already starting');
  await loadDefaultData();
  if(dataBuffer.byteLength<16||dataBuffer.byteLength>128*1024*1024)throw Error('Invalid game DATA');
  if(controller)await controller.close();
  controller=null;report=null;setEnabled(false);$('boot').disabled=true;status('正在加载隔离 Runtime…');
  const ready=new Promise((resolve,reject)=>{const timer=setTimeout(()=>reject(Error('Runtime preparation timeout')),120000);bootPending={resolve:value=>{clearTimeout(timer);resolve(value);},reject:error=>{clearTimeout(timer);reject(error);},replayBytes};});
  epoch++;$('runtime').src=config.runtimeUrl(epoch);
  try{return await ready;}finally{$('boot').disabled=false;bootPending=null;}
}

window.addEventListener('message',async event=>{
  if(event.source!==$('runtime').contentWindow||event.origin!==location.origin||event.data?.epoch!==epoch)return;
  const message=event.data;if(message.event==='error'){bootPending?.reject(Error(message.message||message.error));return;}
  if(message.event!=='ready'||!bootPending)return;
  try{
    const target=$('runtime').contentWindow,runtime=config.runtime(target);
    target.addEventListener('keydown',event=>{if(event.code==='F8'&&!event.repeat){event.preventDefault();event.stopImmediatePropagation();void action(markIncident);}},{capture:true});
    await config.configure(runtime);
    if(bootPending.replayBytes)await config.installReplay(runtime,bootPending.replayBytes);
    await runtime.launch();controller=config.createController(runtime,{...identity,runtimeEpoch:epoch});
    setEnabled(true);status('已启动。可继续游玩，或冻结检查当前帧间窗口。');bootPending.resolve(controller);
  }catch(error){bootPending.reject(error);}
});

function valueText(value){const values=Array.isArray(value)?value:[value];return values.every(Number.isFinite)?values.map(number=>number.toFixed(Math.abs(number)<.01?5:3)).join(', '):'—';}
function severity(object){return Number(object.severity)||0;}
function render(value){
  report=value;$('window').textContent=`逻辑帧 ${value.previousTick??'?'} → ${value.tick??'?'}`;
  $('observed').textContent=value.totalObjects??value.objects?.length??0;
  $('suspects').textContent=value.issueGroups?.length??value.objects?.filter(object=>severity(object)>=3).length??0;
  const purity=value.purityStatus??(value.purity===false?'fail':value.purity===true?'pass':'unknown');
  $('health').className=purity==='fail'?'error':purity==='unknown'?'warn':'';
  $('health').textContent=purity==='fail'?'重画改变了已覆盖状态；本窗口不可作为纯 presentation 证据。':purity==='unknown'?'状态覆盖不完整；对象插值结果可查看，但纯度保持未知。':'已覆盖状态在重复重画前后保持一致。';
  const body=$('rows');body.replaceChildren();
  for(const object of (value.objects||[]).slice(0,250)){
    const row=document.createElement('tr'),name=document.createElement('td'),state=document.createElement('td'),fields=document.createElement('td');
    name.textContent=`${object.owner||object.ownerId} · ${object.object??object.key}`;state.textContent=object.status||'static';
    fields.textContent=(object.fields||[]).filter(field=>field.status!=='static'&&field.status!=='interpolated').map(field=>`${field.label||field.id}: ${field.status}`).join('、')||'—';
    row.append(name,state,fields);row.onclick=()=>{$('detail').textContent=(object.fields||[]).map(field=>`${field.label||field.id} / ${field.status}\n前: ${valueText(field.previous)}\n后: ${valueText(field.current)}`).join('\n\n')||JSON.stringify(object.lifecycle||{},null,2);};body.append(row);
  }
  $('coverage').textContent=JSON.stringify(value.coverage||{},null,2);$('alpha').disabled=false;$('export').disabled=false;
}

async function inspect(){const value=checked().sweep();render(value);status(`已采样 ${value.objects?.length||0} 个绘制对象。`);return value;}
async function markIncident(){const value=checked().mark();render(value);const response=await fetch('/incident',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(checked().observer.compact(value))});if(!response.ok)throw Error('F8 报告保存失败：'+await response.text());status('F8 已锁定并保存当前事件证据。');return value;}

$('boot').onclick=()=>void action(boot);
$('play').onclick=()=>{try{checked().play();$('runtime').contentWindow.document.querySelector('canvas')?.focus();status('正在正常游玩。');}catch(error){fail(error);}};
$('inspect').onclick=()=>void action(inspect);
for(const [id,count] of [['step',1],['step10',10]])$(id).onclick=()=>void action(async()=>{checked().step(count);return inspect();});
$('alpha').oninput=()=>{try{const alpha=Number($('alpha').value);$('alphaValue').textContent=alpha.toFixed(2);checked().preview(alpha);}catch(error){fail(error);}};
$('scan').onclick=()=>void action(async()=>{status('正在进行有界扫描…');const scan=await checked().scan({onWindow:render});window.presentationLab.lastScan=scan;status(`完成 ${scan.windows} 个采样窗口。`);return scan;});
$('frames').onclick=()=>void action(async()=>{const value=checked().sweep({images:true,label:'manual-mark'});render(value);download(checked().observer.compact(value),`${config.game}-presentation-${value.tick}.json`);});
$('export').onclick=()=>download(checked().export(),`${config.game}-presentation-session.json`);
$('data').onchange=async()=>{const file=$('data').files?.[0];if(!file)return;if(file.size>128*1024*1024)throw Error('DATA 文件过大');dataBuffer=await file.arrayBuffer();status(`已选择 ${file.name}。`);};
$('replay').onchange=()=>void action(async()=>{const file=$('replay').files?.[0];if(!file)return;if(!config.installReplay)throw Error('该作品尚未接入 Replay 导入');if(file.size>16*1024*1024)throw Error('Replay 文件过大');return boot({replayBytes:Array.from(new Uint8Array(await file.arrayBuffer()))});});
$('fullscreen').onclick=()=>void action(async()=>{if(document.fullscreenElement)await document.exitFullscreen();else await $('viewport').requestFullscreen({navigationUI:'hide'});});
document.addEventListener('keydown',event=>{if(event.altKey&&event.code==='Enter'&&!event.repeat){event.preventDefault();$('fullscreen').click();}else if(event.code==='F8'&&!event.repeat){event.preventDefault();void action(markIncident);}});
window.presentationLab={boot,get controller(){return controller;},get report(){return report;},identity};
status(identity.dataAvailable?'已找到测试 DATA；点击「启动实验」。':`请选择自己的 ${config.game}.dat。`);
