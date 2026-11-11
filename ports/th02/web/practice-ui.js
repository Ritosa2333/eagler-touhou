import {PRACTICE_DEFAULTS,PRACTICE_SETUP,PRACTICE_VERSION,normalizePracticeSetup,phaseList,hpLimit} from './practice-config.js';
const labels={invincible:'F1 无敌',lockLives:'F2 锁残机',lockBombs:'F3 无限 Bomb',lockPower:'F4 锁当前火力',lockTime:'F5 锁阶段超时',enemyInvincible:'U 敌人无敌',lockRank:'锁 Rank',disableBomb:'Shift+D 禁用 X',disableFocus:'Shift+A 禁用低速',disableShot:'Shift+S 禁用射击',forceFocus:'强制低速',quickRetry:'启用 Shift+F 重试',preparedRetry:'预备快速重试（额外内存）',alwaysPoint:'常驻判定点',showKeys:'显示按键',showInfo:'显示游戏信息'};
const hotkeys={F1:'invincible',F2:'lockLives',F3:'lockBombs',F4:'lockPower',F5:'lockTime',KeyU:'enemyInvincible'};
const tools=Object.entries(labels);
export function mountPractice({host,onChange=()=>{},onStart=()=>{},onRetry=()=>{},onSave=()=>{},onPause=()=>{},onFocus=()=>{},isSynthetic=()=>false,onRelease=()=>{}}){
  const css=document.createElement('link');css.rel='stylesheet';css.href=new URL('practice-ui.css',import.meta.url).href;document.head.append(css);
  const screen=host.querySelector('.solo-screen');screen.classList.add('practice-screen');
  const panel=document.createElement('aside');panel.className='practice-panel';panel.id='practice-panel';panel.hidden=true;panel.setAttribute('aria-label','单人辅助与练习');
  panel.innerHTML=`<header><strong>单人辅助 · 退格</strong><button type="button" data-close-practice>收起</button></header><p data-practice-status role="status">完整流程 · 从原版菜单开始</p><p data-power-status>F4 锁火力：关</p><div class="practice-switches">${tools.slice(0,6).map(([k,n])=>`<label><input type="checkbox" data-practice="${k}"> ${n}</label>`).join('')}</div><details data-advanced><summary>特定练习与高级设置 · F12</summary><section class="practice-jump"><h3>跳转练习位置</h3><p>仅从这里开始特定练习。到达目标后使用原版暂停；初始 0 备用残机、0 Bomb。初始火力只在跳转时设置，80 为满火力。</p><div class="settings-grid"><label>角色<select data-jump="character">${['灵梦 A','灵梦 B','灵梦 C'].map((n,i)=>`<option value="${i}">${n}</option>`).join('')}</select></label><label>难度<select data-jump="difficulty">${['Easy','Normal','Hard','Lunatic','Extra'].map((n,i)=>`<option value="${i}">${n}</option>`).join('')}</select></label><label>关卡<select data-jump="stage">${['一面','二面','三面','四面','五面','Extra'].map((n,i)=>`<option value="${i}">${n}</option>`).join('')}</select></label><label>入口<select data-jump="entry"><option value="stage">关卡开头</option><option value="mid">道中 Boss</option><option value="boss">关底 Boss</option></select></label><label>阶段<select data-jump="phase"><option value="0">正常开始</option></select></label>${[['lives','备用残机（固定 0）',0,8],['bombs','Bomb（固定 0）',0,8],['power','初始火力（80 为满）',1,80],['rank','动态 Rank',-6,16],['hp','Boss 初始血量（0 为原值）',0,8500]].map(([key,name,min,max])=>`<label>${name}<input type="number" data-jump="${key}" min="${min}" max="${max}" value="${PRACTICE_SETUP[key]}" ${['lives','bombs'].includes(key)?'disabled':''}></label>`).join('')}</div><div class="row"><button type="button" data-practice-jump class="primary">从所选位置开始</button><button type="button" data-full-run>返回原版完整流程</button></div><p data-jump-error role="status"></p></section><label>游戏速度<select data-practice="speed">${[.25,.5,.75,1,1.5,2].map(n=>`<option value="${n}">${n*100}%</option>`).join('')}</select></label><p class="muted">F5 保留出弹与攻击帧，只限制阶段超时。</p><div class="practice-switches">${tools.slice(6).map(([k,n])=>`<label><input type="checkbox" data-practice="${k}"> ${n}</label>`).join('')}</div></details><div class="row"><button type="button" data-open-advanced>特定练习 · F12</button><button type="button" data-practice-retry>重试当前练习 · Shift+F</button></div><p>辅助快捷键仅在退格菜单展开时响应，收起后已开启的功能继续生效。Tab 显示游戏信息；F12 选择特定练习。</p>`;
  screen.append(panel);
  const jump=panel.querySelector('.practice-jump'),find=k=>jump.querySelector(`[data-jump="${k}"]`);
  find('hp').parentElement.firstChild.textContent='当前攻击血量（0 为原值；多体暂用原值）';
  let running=false,options={...PRACTICE_DEFAULTS},current=null,setup={...PRACTICE_SETUP},resultOpenedAt=0,resultShown=false;
  const result=document.createElement('dialog');result.className='practice-result';result.id='practice-result-dialog';result.setAttribute('aria-labelledby','practice-result-title');
  result.innerHTML='<h2 id="practice-result-title">练习成功</h2><p data-result-message></p><div class="row practice-result-choices"><button type="button" data-practice-again>重来</button><button type="button" data-practice-save disabled>保存本次 REP</button><button type="button" data-practice-other>选择其他练习</button></div><p class="practice-menu-help">↑ / ↓ 选择 · Z / Enter 确认</p>';
  host.append(result);result.addEventListener('cancel',event=>event.preventDefault());
  result.querySelector('[data-practice-again]').onclick=onRetry;result.querySelector('[data-practice-save]').onclick=onSave;
  result.querySelector('[data-practice-other]').onclick=()=>{resultShown=true;result.close();panel.hidden=true;show(true);};
  const button=document.createElement('button');button.type='button';button.textContent='辅助菜单 · 退格';button.dataset.practiceToolbar='';host.querySelector('.player-toolbar').append(button);button.onclick=()=>show();
  const preparing=document.createElement('div');preparing.className='practice-preparing';preparing.hidden=true;preparing.setAttribute('role','status');preparing.innerHTML='<div class="practice-notice">正在加载练习目标…</div>';screen.append(preparing);
  const overlay=document.createElement('div');overlay.className='practice-overlay';overlay.hidden=true;overlay.setAttribute('aria-live','off');screen.append(overlay);
  function phases(){
    const stage=Number(find('stage').value),entry=find('entry').value;
    const list=phaseList({stage,entry,character:Number(find('character').value)});
    const previous=Number(find('phase').value);find('phase').innerHTML='<option value="0">正常开始</option>'+list.map(p=>`<option value="${p}">阶段 ${p}</option>`).join('');if(list.includes(previous))find('phase').value=String(previous);find('phase').disabled=entry==='stage';
    const limit=hpLimit({stage,entry,phase:Number(find('phase').value)});find('hp').max=String(limit);find('hp').disabled=!limit;if(!limit)find('hp').value='0';
  }
  find('phase').onchange=()=>{const limit=hpLimit({stage:Number(find('stage').value),entry:find('entry').value,phase:Number(find('phase').value)});find('hp').max=String(limit);};
  for(const key of ['stage','entry','difficulty','character'])find(key).onchange=()=>{if(key==='difficulty'&&find(key).value==='4')find('stage').value='5';if(key==='stage'&&find(key).value==='5')find('difficulty').value='4';else if(key==='stage'&&find('difficulty').value==='4')find('difficulty').value='1';phases();};
  find('difficulty').value='1';phases();
  function syncJump(){const value=current?.mode==='target'?current.setup:setup;for(const [key,val]of Object.entries(value))if(key!=='phase')find(key).value=String(val);phases();find('phase').value=String(value.phase);}
  function configuration(){return {version:PRACTICE_VERSION,mode:'full',setup:{...setup},options:{...options}};}
  panel.querySelector('[data-practice-jump]').onclick=()=>{try{const value={};for(const input of jump.querySelectorAll('[data-jump]'))value[input.dataset.jump]=input.dataset.jump==='entry'?input.value:Number(input.value);setup=normalizePracticeSetup(value);jump.querySelector('[data-jump-error]').textContent='';panel.hidden=true;onStart({...configuration(),mode:'target',setup:{...setup}});}catch(error){jump.querySelector('[data-jump-error]').textContent=error.message;}};
  panel.querySelector('[data-full-run]').onclick=()=>{panel.hidden=true;result.close();onStart(configuration());};
  function render(){for(const input of panel.querySelectorAll('[data-practice]')){const k=input.dataset.practice;if(k==='speed')input.value=String(options[k]);else input.checked=options[k];}panel.querySelector('[data-power-status]').textContent=`火力 ${current?.power??'-'} / 80 · F4 锁火力：${options.lockPower?'开':'关'}`;}
  function change(key,value){options[key]=value;render();onChange({[key]:value});}
  for(const input of panel.querySelectorAll('[data-practice]'))input.onchange=()=>change(input.dataset.practice,input.dataset.practice==='speed'?Number(input.value):input.checked);
  function show(advanced=false){onRelease();const details=panel.querySelector('details');if(advanced){const close=!panel.hidden&&panel.classList.contains('practice-full');panel.hidden=close;panel.classList.toggle('practice-full',!close);details.open=!close;if(!close){syncJump();if(running)onPause();panel.scrollTop=0;panel.querySelector('[data-close-practice]').focus({preventScroll:true});}}else{const close=!panel.hidden&&!panel.classList.contains('practice-full');panel.classList.remove('practice-full');panel.hidden=close;details.open=false;}if(panel.hidden)onFocus();}
  panel.querySelector('[data-open-advanced]').onclick=()=>show(true);panel.querySelector('[data-advanced] summary').onclick=event=>{event.preventDefault();show(true);};
  panel.querySelector('[data-close-practice]').onclick=()=>{onRelease();panel.hidden=true;onFocus();};panel.querySelector('[data-practice-retry]').onclick=()=>{if(running&&current?.mode==='target')onRetry();};
  function onKey(event){
    if(isSynthetic(event))return;
    if(['F1','F5'].includes(event.code)&&!host.hidden&&!event.ctrlKey&&!event.metaKey&&!event.altKey)event.preventDefault();
    if(result.open&&['ArrowUp','ArrowDown','KeyZ','Enter','KeyX','Escape'].includes(event.code)){
      event.preventDefault();event.stopImmediatePropagation();if(event.repeat)return;const choices=[...result.querySelectorAll('.practice-result-choices button')].filter(b=>!b.disabled),selected=Math.max(0,choices.indexOf(document.activeElement));
      if(event.code==='ArrowUp'||event.code==='ArrowDown')selectResult(choices[(selected+(event.code==='ArrowUp'?-1:1)+choices.length)%choices.length]);else if(event.code==='KeyX'||event.code==='Escape')selectResult(result.querySelector('[data-practice-again]'));else if(performance.now()-resultOpenedAt>200)choices[selected]?.click();return;
    }
    if(!panel.hidden&&panel.classList.contains('practice-full')&&!event.ctrlKey&&!event.metaKey&&!event.altKey){
      const control=document.activeElement,editable=control?.matches('select,input[type="number"],textarea,[contenteditable="true"]');
      const controls=()=>[...panel.querySelectorAll('button:not(:disabled),input:not(:disabled),select:not(:disabled),textarea:not(:disabled)')].filter(n=>n.getClientRects().length);
      if(['F12','Escape','Tab'].includes(event.code)||!editable&&['ArrowUp','ArrowDown','Enter','Space','KeyZ','KeyX'].includes(event.code)){
        event.preventDefault();event.stopImmediatePropagation();if(event.repeat)return;
        if(['F12','Escape','KeyX'].includes(event.code)){onRelease();panel.hidden=true;onFocus();}
        else if(['Tab','ArrowUp','ArrowDown'].includes(event.code)){const list=controls(),at=list.indexOf(control),step=event.code==='ArrowUp'||event.code==='Tab'&&event.shiftKey?-1:1;const next=list[(at+step+list.length)%list.length];next?.focus({preventScroll:true});next?.scrollIntoView({block:'nearest'});}
        else if(panel.contains(control))control.click();return;
      }
      // Native select/number editing stays with the form;
      // the game's capture handler must never consume these navigation keys.
      if(editable){event.stopImmediatePropagation();return;}
    }
    if(event.ctrlKey||event.metaKey||event.altKey||event.target?.closest?.('input,select,textarea,[contenteditable="true"]'))return;
    if(!host.contains(document.activeElement)&&document.activeElement!==document.body)return;
    if(['Backspace','Tab','F12'].includes(event.code)){event.preventDefault();event.stopImmediatePropagation();if(event.repeat)return;if(event.code==='Tab')change('showInfo',!options.showInfo);else{if(event.code==='F12'&&result.open)result.close();show(event.code==='F12');}return;}
    let key=hotkeys[event.code];if(event.shiftKey&&['KeyD','KeyA','KeyS'].includes(event.code))key={KeyD:'disableBomb',KeyA:'disableFocus',KeyS:'disableShot'}[event.code];
    // Block browser F1/F5 actions too, but only the compact menu may toggle tools.
    if(key){event.preventDefault();event.stopImmediatePropagation();}
    if(panel.hidden||panel.classList.contains('practice-full')||!running)return;
    if(key){event.preventDefault();if(!event.repeat)change(key,!options[key]);return;}
    if(event.shiftKey&&event.code==='KeyF'){event.preventDefault();event.stopImmediatePropagation();if(!event.repeat&&current?.mode==='target'&&options.quickRetry)onRetry();}
  }
  window.addEventListener('keydown',onKey,true);
  function selectResult(button){for(const item of result.querySelectorAll('.practice-result-choices button'))item.classList.toggle('selected',item===button);button?.focus({preventScroll:true});}
  result.addEventListener('focusin',event=>{if(event.target.matches('.practice-result-choices button'))for(const item of result.querySelectorAll('.practice-result-choices button'))item.classList.toggle('selected',item===event.target);});
  function positionResult(){if(!result.open)return;const r=screen.getBoundingClientRect();result.style.left=`${r.left+r.width*.35}px`;result.style.top=`${r.top+r.height*.5}px`;result.style.width=`${Math.min(440,Math.max(0,r.width*.6-20))}px`;result.style.maxHeight=`${Math.max(0,r.height-16)}px`;}
  const observer=new ResizeObserver(positionResult);observer.observe(screen);window.addEventListener('resize',positionResult);window.addEventListener('scroll',positionResult,true);
  render();return {configuration,open:()=>show(true),get enabled(){return true;},get options(){return {...options};},complete({outcome='clear',recording=false,ready=false,failed=false}={}){
      result.querySelector('#practice-result-title').textContent=outcome==='clear'?'练习成功':'练习失败';result.querySelector('[data-practice-again]').textContent=outcome==='clear'?'重来':'继续练习';result.querySelector('[data-practice-save]').disabled=!ready;result.querySelector('[data-result-message]').textContent=(outcome==='clear'?'所选练习目标已完成。':'本次练习中弹失败。')+(ready?'本次录像已就绪，可保存或从同一起点重来。':failed?'录像处理失败，可从同一起点重来。':recording?'正在封存本次录像…':'本次未启用录像，可从同一起点重来。');
      if(!resultShown){panel.hidden=true;resultShown=true;result.showModal();resultOpenedAt=performance.now();selectResult(result.querySelector('[data-practice-again]'));}positionResult();
    },setRunning(value,config){running=value;resultShown=false;panel.hidden=true;preparing.hidden=!value||config?.mode!=='target';if(config){options={...config.options};render();}if(!value){result.close();overlay.hidden=true;current=null;}},
    update(state){current=state?.practice;if(!running&&current?.options)options={...current.options};const targeted=current?.mode==='target';preparing.hidden=!running||!targeted||current.ready;preparing.firstElementChild.textContent=current?.error||'正在加载练习目标…\n载入时静音，到达后进入原版暂停';panel.querySelector('[data-practice-retry]').disabled=!running||!targeted;
      panel.querySelector('[data-practice-status]').textContent=current?.error||(targeted?(current.outcome==='clear'?'练习成功':current.outcome==='failed'?'练习失败，等待继续':!current.ready?'正在准备特定练习入口…':current.waiting?'原版暂停中，按 Enter / Esc 继续':'特定练习进行中'):'完整流程 · 原版资源与关卡进程')+(state?.retryReady?' · 下次重试已就绪':'');render();
      const lines=[];if(current&&options.showInfo)lines.push(`Miss ${current.misses??0}  Bomb ${current.bombsUsed??0}`,`火力 ${current.power??'-'}/80 · F4 ${options.lockPower?'开':'关'}`,`动态 Rank ${current.rank??'-'}`,`${Math.floor((current.elapsed||0)/56.4)} 秒`);if(current&&options.showKeys)lines.push([[1,'↑'],[2,'↓'],[4,'←'],[8,'→'],[16,'Z'],[32,'X']].map(([bit,label])=>current.keys&bit?label:'·').join(' '));overlay.hidden=!lines.length;overlay.textContent=lines.join('\n');
    },dispose(){observer.disconnect();window.removeEventListener('keydown',onKey,true);window.removeEventListener('resize',positionResult);window.removeEventListener('scroll',positionResult,true);result.remove();panel.remove();overlay.remove();preparing.remove();button.remove();},
  };
}
