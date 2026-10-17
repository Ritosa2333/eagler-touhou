import { parseMeasuredNetplayTiming } from '../contracts/netplay-timing.mjs';

let progress: Record<string, unknown> | null = null;
let resultUntil = 0;
export function resetCalibrationProgress() { progress=null;resultUntil=0; }
export function recordCalibrationProgress(value: unknown) {
  if(!value || typeof value!=='object')return;
  const v=value as Record<string,unknown>;
  if(v.phase==='closed'){resetCalibrationProgress();return;}
  if(v.phase==='ready') {
    if(!parseMeasuredNetplayTiming(v) || v.route==='spectator')return;
    progress=v;resultUntil=Date.now()+8000;return;
  }
  if(!['waiting','stabilizing','measuring','negotiating','retrying','suspended','unavailable'].includes(String(v.phase)))return;
  progress=v;resultUntil=0;
}
export function renderCalibrationConnection(windowElement: HTMLElement, onReturn?: () => void): boolean {
  windowElement.removeAttribute('data-calibration');
  windowElement.querySelector('#netplayCalibrationDismiss')?.remove();
  const note=windowElement.querySelector<HTMLElement>('#netplayConnectionNote');
  if(note)note.hidden=true;
  if(!progress || (progress.phase==='ready' && Date.now()>=resultUntil)){
    windowElement.querySelector('#netplayConnectionReturn')?.remove();return false;
  }
  const english=document.documentElement.dataset.uiLocale==='en';
  const ready=progress.phase==='ready';
  windowElement.hidden=false;
  windowElement.classList.remove('reconnecting');
  windowElement.dataset.calibration=String(progress.phase);
  const title=windowElement.querySelector<HTMLElement>('#netplayConnectionTitle')!;
  title.setAttribute('role','status');title.setAttribute('aria-live','polite');
  const nextTitle=ready ? (english?'Connection measured':'联机测量完成')
    : progress.phase==='unavailable' ? (english?'Unable to start multiplayer':'暂时无法开始联机')
    : progress.phase==='suspended' ? (english?'Waiting for the game page…':'等待返回游戏页面…')
    : progress.phase==='retrying' ? (english?'Retrying connection measurement…':'连接波动，正在重新测量…')
    : progress.phase==='waiting' ? (english?'Connecting to players…':'正在连接其他玩家…')
    : progress.phase==='stabilizing' ? (english?'Allowing connection to settle…':'正在稳定连接…')
    : progress.phase==='negotiating' ? (english?'Confirming measured delay…':'各方确认测量结果…')
    : (english?'Measuring connection latency…':'正在测量连接延迟…');
  if(title.textContent!==nextTitle)title.textContent=nextTitle;
  const summary=windowElement.querySelector<HTMLElement>('#netplayConnectionSummary')!;
  summary.hidden=false;
  summary.replaceChildren();
  if(ready){
    const delay=document.createElement('span');
    delay.textContent=english?`Input delay: ${progress.inputDelay} frame(s)`:`输入延迟：${progress.inputDelay} 帧`;
    const rollback=document.createElement('span');
    rollback.textContent=english?`Rollback: ${Number(progress.adonisMode)===2?'on':'off'}`:`回滚：${Number(progress.adonisMode)===2?'开启':'关闭'}`;
    summary.append(delay,rollback);
  }else summary.textContent=progress.phase==='unavailable'
    ? Number(progress.reason)===8 ? (english?'The measured delay is too high. Return to the room and choose a manual delay.':'测得的延迟过高，请返回房间调整输入延迟。')
      : (english?'Connection measurement could not finish. Return to the room to try again.':'暂时无法完成联机测量，请返回房间重新开始。')
    : progress.phase==='suspended' ? (english?'Keep this game page open. Measurement resumes automatically.':'请保持游戏页面在前台，恢复后会自动重新测量。')
    : progress.phase==='retrying' ? (english?`Preparing attempt ${progress.attempt??1}/${progress.maxAttempts??4}. Waiting for all players to reconnect.`:`准备第 ${progress.attempt??1}/${progress.maxAttempts??4} 次测量，正在等待所有玩家恢复。`)
    : progress.phase==='stabilizing'
    ? (english?'Measurement starts after one second.':'等待 1 秒后开始测量。')
    : progress.phase==='waiting' ? (english?'Waiting for all player input channels.':'等待所有玩家的输入通道就绪。')
    : progress.phase==='negotiating' ? (english?'Waiting for every player to confirm.':'等待所有玩家确认测量结果。')
    : (english?`Probes ${progress.probes??0}/129 · replies ${progress.replies??0}/120`:`探测 ${progress.probes??0}/129 · 有效应答 ${progress.replies??0}/120`);
  const rows=windowElement.querySelector<HTMLElement>('#netplayConnectionPeers')!;
  rows.replaceChildren();
  if(!ready&&progress.phase!=='unavailable'){
    const meter=document.createElement('progress');meter.id='netplayCalibrationProgress';
    meter.max=129;if(progress.phase==='measuring')meter.value=Number(progress.probes??0);
    meter.setAttribute('aria-label',title.textContent);rows.append(meter);
  }else if(ready){
    const table=document.createElement('table');table.className='netplay-calibration-table';
    const head=table.createTHead().insertRow();
    for(const text of english?['Player','Measured latency','Maximum latency']:['玩家','测定延迟','最大延迟']){
      const th=document.createElement('th');th.scope='col';th.textContent=text;head.append(th);
    }
    const body=table.createTBody();
    const calibration=progress.calibration as Record<string,unknown>|undefined;
    const players=calibration?.players as Array<Record<string,unknown>>|undefined;
    for(const p of players??[]){
      if(!Number(p.samples))continue;
      const row=body.insertRow();const label=document.createElement('th');label.scope='row';label.textContent=`P${Number(p.player)+1}`;row.append(label);
      // The same frozen statistic that determines D, never the mean.
      for(const key of ['p95Us','maxUs'])row.insertCell().textContent=`${(Number(p[key])/1000).toFixed(2)} ms`;
    }
    rows.append(table);
    if(players?.length===3&&note){note.hidden=false;note.textContent=english?'Each player’s slowest input link.':'各玩家最慢的输入链路。';}
    const dismiss=document.createElement('button');dismiss.id='netplayCalibrationDismiss';dismiss.type='button';
    dismiss.textContent=english?'Dismiss':'收起';
    dismiss.onclick=()=>{resultUntil=0;windowElement.hidden=true;};windowElement.append(dismiss);
  }
  if(!ready&&onReturn){
    let button=windowElement.querySelector<HTMLButtonElement>('#netplayConnectionReturn');
    if(!button){button=document.createElement('button');button.id='netplayConnectionReturn';button.type='button';windowElement.append(button);}
    button.textContent=english?'Return to room':'返回房间';button.onclick=onReturn;
  }else windowElement.querySelector('#netplayConnectionReturn')?.remove();
  return true;
}
