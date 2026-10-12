const $=id=>document.getElementById(id),pending=new Map();let native,candidate,serial=0,imageFrame=-1;
function report(error){$('status').textContent=error.message??String(error);}
function request(command,fields={}){return new Promise((resolve,reject)=>{
  const id=String(++serial),timer=setTimeout(()=>{pending.delete(id);reject(Error('网页版尚未就绪或命令超时'));},120000);
  pending.set(id,{resolve,reject,timer});$('candidate').contentWindow.postMessage({protocol:'th12-frame-inspector/1',id,command,...fields},location.origin);
});}
window.addEventListener('message',event=>{
  if(event.origin!==location.origin||event.source!==$('candidate').contentWindow||event.data?.protocol!=='th12-frame-inspector/1')return;
  const task=pending.get(event.data.id);if(!task)return;clearTimeout(task.timer);pending.delete(event.data.id);
  if(event.data.error)task.reject(Error(event.data.error));else{candidate=event.data.state;render();task.resolve(candidate);}
});
async function command(kind,fields={}){const response=await fetch('/api/command',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({kind,...fields})});const body=await response.json();if(!response.ok)throw Error(body.error);}
const format=row=>JSON.stringify(row,null,2);
function render(){
  if(native){$('status').textContent=native.status==='error'?native.error:native.status==='idle'?'请先关闭已有原版游戏，再点击“启动检查副本”。':`原版：${native.status==='paused'?'已停帧':native.status==='closed'?'已关闭':'运行中'} · 第${native.stage??'—'}面 · 录像帧 ${native.replayCursor??'—'} · 画面帧 ${native.presentFrame??'—'}`;$('native-state').textContent=format(native);}
  if(candidate)$('candidate-state').textContent=format(candidate);
  if(!native?.replayActive||!candidate)return;
  if(native.stage!==candidate.stage||native.replayCursor!==candidate.replayCursor){$('comparison').textContent='帧号尚未对齐；点击“网页对齐原版当前帧”。';return;}
  const fields=['scoreStored','rng','piv','graze','power','playerFixed'],diff=fields.filter(key=>JSON.stringify(native[key])!==JSON.stringify(candidate[key]));
  $('comparison').textContent=diff.length?`同帧分歧：${diff.join('、')}。原版显示分数 ${(native.displayScoreStored??0)*10}，原始分数 ${native.scoreStored*10}。`:`此帧已采集的字段一致（分数、随机数、最大得点、擦弹、灵力、自机位置）。原版显示分数 ${(native.displayScoreStored??0)*10}；尚未覆盖全部内部对象。`;
}
async function poll(){try{native=await fetch('/api/state').then(r=>r.json());render();if(native.status==='paused'&&native.presentFrame!==imageFrame){imageFrame=native.presentFrame;$('original').src='/api/frame.png?frame='+imageFrame;} }catch(error){report(error);}setTimeout(poll,350);}
for(const button of document.querySelectorAll('[data-command]'))button.onclick=()=>command(button.dataset.command).catch(report);
$('load').onclick=()=>request('load',{replay:$('replay').value,stage:Number($('stage').value)}).catch(report);
$('seek').onclick=()=>request('seek',{frame:Number($('frame').value)}).catch(report);
$('until').onclick=()=>command('until',{stage:Number($('stage').value),frame:Number($('frame').value)}).catch(report);
$('align').onclick=()=>{if(native?.status!=='paused'||!native.replayActive)return report(Error('先在原版录像中停帧'));return request('seek',{frame:native.replayCursor}).catch(report);};
$('both-step').onclick=async()=>{try{if(native?.status!=='paused'||!candidate||native.stage!==candidate.stage||native.replayCursor!==candidate.replayCursor)throw Error('先停帧并对齐两边');const previous=native.presentFrame;await command('step');for(let i=0;i<100;i++){await new Promise(r=>setTimeout(r,50));native=await fetch('/api/state').then(r=>r.json());if(native.status==='paused'&&native.presentFrame>previous){await request('seek',{frame:native.replayCursor});return;}}throw Error('原版单步超时');}catch(error){report(error);}};
$('time').onchange=()=>{const match=$('time').value.match(/^(\d+):(\d{1,2})(?:\.(\d{1,2}))?$/);if(!match||Number(match[2])>=60)return report(Error('时间格式为 4:03.95'));$('frame').value=Math.round((Number(match[1])*60+Number(match[2])+Number((match[3]??'0').padEnd(2,'0'))/100)*60);};
poll();
