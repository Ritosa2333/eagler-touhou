// Visible finite practice UI. Existing page consumers own all game/native state.
export const PRACTICE_CHOICES=Object.freeze([
 ['A1-3','A1 第三面 · Normal',3,1],['A2-3','A2 第三面 · Normal',5,1],
 ['B1-3','B1 第三面 · Normal',8,1],['B2-3','B2 第三面 · Normal',10,1],
 ['C1-3','C1 第三面 · Normal',13,1],['C2-3','C2 第三面 · Normal',15,1],
 ['EX','EX 起点 · Extra',16,4]
].map(row=>Object.freeze(row)));
const choices=new Map(PRACTICE_CHOICES.map(row=>[row[0],row]));
export function mountPracticeMenu({root,opener,getAvailability,onOpen=()=>{},onFocusGame=()=>{},launch}){
 if(!root||!opener||typeof getAvailability!=='function'||typeof launch!=='function')throw new Error('Practice UI consumers unavailable');
 const find=name=>{const e=root.querySelector('[data-practice="'+name+'"]');if(!e)throw new Error('Practice control missing: '+name);return e;};
 const select=find('select'),start=find('start'),restart=find('restart'),cancel=find('cancel'),status=find('status'),replace=find('replace'),replaceRow=find('replaceRow'),listeners=[];
 let disposed=false,opening=false,ticket=0,pending=null;
 const listen=(target,type,fn)=>{target.addEventListener(type,fn);listeners.push(()=>target.removeEventListener(type,fn));};
 select.replaceChildren(...PRACTICE_CHOICES.map(([value,label])=>{const o=root.ownerDocument.createElement('option');o.value=value;o.textContent=label;return o;}));
 root.hidden=true;opener.setAttribute('aria-expanded','false');status.textContent='选择练习关卡；练习局不计正常通关与解锁。';
 function refresh(){
  if(disposed)return;
  const a=getAvailability(),blocked=!a.ready||a.busy||opening||!!pending;
  opener.disabled=blocked;select.disabled=!!pending||opening;start.disabled=blocked;
  restart.disabled=blocked||!choices.has(a.practiceRoute);replaceRow.hidden=!a.inRun;
 }
 function close({focus=true}={}){
  ++ticket;pending?.controller.abort();root.hidden=true;opener.setAttribute('aria-expanded','false');replace.checked=false;
  if(pending)status.textContent='已取消练习启动。';
  if(!disposed)refresh();if(focus)onFocusGame();
 }
 async function open(){
  if(disposed||opening||pending)return;const a=getAvailability();if(!a.ready||a.busy){refresh();return;}
  opening=true;root.hidden=false;opener.setAttribute('aria-expanded','true');replace.checked=false;refresh();const mine=ticket;
  try{await onOpen();if(!disposed&&mine===ticket&&!root.hidden)select.focus();}
  catch(error){if(!disposed&&mine===ticket)status.textContent='暂时无法打开练习：'+error.message;}
  finally{opening=false;if(!disposed)refresh();}
 }
 async function begin(route){
  if(disposed||root.hidden||pending||opening)return;
  const a=getAvailability(),choice=choices.get(route);
  if(!a.ready||a.busy||!choice){status.textContent='练习暂未就绪，请稍后重试。';refresh();return;}
  if(a.inRun&&!replace.checked){status.textContent='请先确认替换当前局；尚未保存的进度不会保留。';return;}
  const controller=new AbortController(),mine=++ticket,owner={controller};
  const current=()=>!disposed&&ticket===mine&&!controller.signal.aborted;
  pending=owner;status.textContent='正在准备 '+choice[1]+'…';refresh();
  try{
   const receipt=await launch(route,{signal:controller.signal,isCurrent:current});if(!current())return;
   if(receipt?.route!==route||receipt.stageId!==choice[2]||receipt.difficulty!==choice[3]
    ||receipt.nativeTainted!==true||receipt.normalModeClear!==false)throw new Error('练习身份或非正常成绩标记未确认');
   status.textContent=choice[1]+' 练习已开始，不计正常通关与解锁。';root.hidden=true;opener.setAttribute('aria-expanded','false');replace.checked=false;onFocusGame();
  }catch(error){if(current())status.textContent='练习启动失败：'+error.message;}
  finally{if(pending===owner)pending=null;if(!disposed)refresh();}
 }
 listen(opener,'click',()=>{void open();});listen(start,'click',()=>{void begin(select.value);});
 listen(restart,'click',()=>{const route=getAvailability().practiceRoute;if(choices.has(route))void begin(route);});
 listen(cancel,'click',()=>close());listen(select,'change',()=>{replace.checked=false;});
 listen(root,'keydown',event=>{event.stopPropagation();if(event.code==='Escape'){event.preventDefault();close();}});
 listen(root,'keyup',event=>event.stopPropagation());
 refresh();
 return Object.freeze({get isOpen(){return !disposed&&!root.hidden;},refresh,close,
  dispose(){if(disposed)return;disposed=true;close({focus:false});for(const remove of listeners)remove();opener.disabled=true;}
 });
}
