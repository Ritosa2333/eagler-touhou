// Shared browser menus and playback controls, also available in fullscreen.
export function mountSoloReplayToolbar(host,{isPlayback=()=>false,isSynthetic=()=>false,onHold=()=>{},onRelease=()=>{},onAgain=()=>{},onStop=()=>{},onFast=()=>{}}={}){
  const $=id=>document.getElementById(id);
  const style=document.createElement('style');
  style.textContent='.solo-replay-dialog{z-index:1000;max-width:min(460px,88vw);padding:24px;border:1px solid #8fa68e;border-radius:12px;background:#16202d;color:#fff;font:16px system-ui}.solo-replay-dialog::backdrop{background:#0009}.solo-replay-dialog button{margin:8px;padding:10px 20px}.solo-replay-dialog button[data-selected=true]{outline:3px solid #d9ef9a}.solo-replay-dialog p{white-space:pre-line;line-height:1.6}';
  host.append(style);
  const pressed=new Set();let blocked=new Set(),selection=0,fast=false;
  function menu(id,title,text,buttons){
    const dialog=document.createElement('dialog');dialog.id=id;dialog.className='solo-replay-dialog';
    const heading=document.createElement('h2');heading.textContent=title;dialog.append(heading);
    const summary=document.createElement('p');summary.textContent=text;dialog.append(summary);
    for(const [id,label,action] of buttons){const b=document.createElement('button');b.id=id;b.textContent=label;b.onclick=action;dialog.append(b);}
    const help=document.createElement('p');help.textContent='← → 选择 · Z / Enter 确认';dialog.append(help);host.append(dialog);
    return {dialog,heading,summary};
  }
  const save=menu('save-dialog','保存本局 Replay？','正在生成录像…',[
    ['save-replay','保存',()=>$('record-save').click()],['discard-replay','丢弃',()=>$('record-discard').click()],
  ]);
  const end=menu('end-dialog','Replay 播放完成','',[
    ['replay-again','再次播放',()=>{end.dialog.close();onAgain();}],['end-replay','结束回放',()=>{end.dialog.close();onStop();}],
  ]);
  function current(){return save.dialog.open?save.dialog:end.dialog.open?end.dialog:null;}
  function select(dialog,index){const buttons=[...dialog.querySelectorAll('button')];selection=(index+buttons.length)%buttons.length;buttons.forEach((button,i)=>button.dataset.selected=String(i===selection));}
  function show(dialog){blocked=new Set(pressed);select(dialog,0);if(!dialog.open)dialog.showModal();}
  function clearFast(){if(!fast)return;fast=false;onFast(false);const b=$('replay-fast');if(b){b.dataset.fast='false';b.textContent='快进（Ctrl）';}}
  save.dialog.addEventListener('cancel',event=>event.preventDefault());
  end.dialog.addEventListener('cancel',event=>{event.preventDefault();end.dialog.close();onStop();});
  // Install before each game's capture handlers; synthetic DOS key delivery
  // must pass through, while a physical menu confirmation never reaches DOS.
  for(const type of ['keydown','keyup'])window.addEventListener(type,event=>{
    if(isSynthetic(event))return;
    if(type==='keydown')pressed.add(event.code);else{pressed.delete(event.code);blocked.delete(event.code);}
    const dialog=current();
    if(dialog){
      if(event.code==='Tab')return;
      event.preventDefault();event.stopImmediatePropagation();
      if(type!=='keydown'||event.repeat||blocked.size)return;
      if(['ArrowLeft','ArrowUp'].includes(event.code))select(dialog,selection-1);
      else if(['ArrowRight','ArrowDown'].includes(event.code))select(dialog,selection+1);
      else if(['Escape','KeyX'].includes(event.code)){select(dialog,1);if(dialog===end.dialog)$('end-replay').click();}
      else if(['KeyZ','Enter','Space'].includes(event.code))dialog.querySelectorAll('button')[selection].click();
      return;
    }
    if(!isPlayback()||event.target?.closest?.('input,select,textarea,[contenteditable=true]'))return;
    if(['Escape','Space'].includes(event.code)){
      event.preventDefault();event.stopImmediatePropagation();
      if(type==='keydown'&&!event.repeat)$('replay-pause').click();
    }else if(['ControlLeft','ControlRight'].includes(event.code)){
      event.preventDefault();event.stopImmediatePropagation();
      if(type==='keyup')clearFast();else if(!event.repeat){fast=true;onFast(true);}
    }
  },true);
  window.addEventListener('blur',()=>{pressed.clear();blocked.clear();clearFast();});
  const toolbar=document.createElement('details');toolbar.className='solo-replay-toolbar';
  toolbar.style.cssText='position:absolute;z-index:40;left:8px;top:8px;max-width:min(430px,85%);padding:6px;background:#16202de8;color:white;border-radius:7px;font:13px system-ui';
  const title=document.createElement('summary');title.textContent='Replay';title.style.cursor='pointer';toolbar.append(title);host.append(toolbar);
  const fastButton=document.createElement('button');fastButton.id='replay-fast';fastButton.textContent='快进（Ctrl）';fastButton.disabled=true;
  fastButton.onclick=()=>{fast=!fast;fastButton.dataset.fast=String(fast);fastButton.textContent=fast?'恢复速度':'快进（Ctrl）';onFast(fast);host.querySelector('canvas')?.focus();};
  $('replay-pause').after(fastButton);
  for(const id of ['record-finish','record-save','record-discard','replay-pause','replay-fast','replay-stop']){
    const source=$(id),button=document.createElement('button');button.type='button';button.dataset.replayAction=id;
    const update=()=>{button.textContent=source.textContent;button.disabled=source.disabled;button.hidden=source.disabled;};
    button.onclick=()=>source.click();update();toolbar.append(button);
    new MutationObserver(update).observe(source,{attributes:true,childList:true,subtree:true});
  }
  const status=document.createElement('p');status.setAttribute('role','status');toolbar.append(status);
  const update=()=>{status.textContent=$('replay-status').textContent+' '+$('status').textContent;};update();
  for(const node of [$('replay-status'),$('status')])new MutationObserver(update).observe(node,{childList:true,subtree:true});
  return {
    prepareSave(){onHold();$('save-replay').disabled=true;save.summary.textContent='正在生成本局录像…';show(save.dialog);},
    offerSave(blob,result){$('save-replay').disabled=false;save.summary.textContent=`${result.timeline?.reason==='clear'?'本局已通关。':'本局录制已结束。'}\n仅保留首次续关之前的操作。\n文件 ${(blob.size/1024).toFixed(1)} KiB`;show(save.dialog);},
    closeSave(){if(save.dialog.open){save.dialog.close();onRelease();}},
    showEnd(matches){clearFast();$('replay-fast').disabled=true;end.heading.textContent=matches?'Replay 播放完成':'Replay 校验不一致';end.summary.textContent=matches?'本局录像已播完，终态内存校验一致。':'播放已停止，终态内存与录像不一致。';show(end.dialog);},
    reset(){clearFast();if(end.dialog.open)end.dialog.close();},
    get open(){return !!current();},
  };
}
