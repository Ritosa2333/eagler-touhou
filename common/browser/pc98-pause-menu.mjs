export function mountPauseMenu(host,{resume,exit,discard,restart,save,centered=false}){
  const panel=document.createElement('div');panel.className='pc98-pause-menu';panel.hidden=true;if(centered)panel.classList.add('pc98-pause-centered');panel.setAttribute('role','dialog');panel.setAttribute('aria-label','暂停');
  panel.innerHTML='<div class="pc98-pause-items"><button>继续游戏</button><button>退出游戏</button><button>不存 REP 退出游戏</button><button>重新开始</button></div>';
  const style=document.createElement('style');style.textContent=`.pc98-pause-menu[hidden]{display:none!important}.pc98-pause-menu{position:absolute;left:5%;top:3.33%;width:60%;height:93.33%;background:rgba(0,0,0,.8);z-index:30000;container-type:size;pointer-events:auto}.pc98-pause-menu.pc98-pause-centered{left:20%}.pc98-pause-items{position:absolute;top:31.25%;left:31%;width:34%;display:flex;flex-direction:column;gap:1cqh}.pc98-pause-items button{box-sizing:border-box;width:100%;min-height:5.58cqh;padding:0;border:0;border-radius:0;color:white;background:#294a70;font:clamp(10px,3.6cqh,23px)/1.15 sans-serif;cursor:pointer}.pc98-pause-items button.selected,.pc98-pause-items button:hover{background:#477aac;outline:1px solid #b5d9ff}`;
  document.head.append(style);host.append(panel);const buttons=[...panel.querySelectorAll('button')];let selected=0,selectingReplay=false;
  const mainLabels=['继续游戏','退出游戏','不存 REP 退出游戏','重新开始'],replayLabels=['保存本次 REP','不保存','返回'];
  function replayMenu(value){selectingReplay=value;const labels=value?replayLabels:mainLabels;buttons.forEach((button,i)=>{button.hidden=i>=labels.length;if(!button.hidden)button.textContent=labels[i];});choose(0);}
  const actions=[resume,()=>save?replayMenu(true):exit(),discard,restart],replayActions=[save,discard,()=>replayMenu(false)];
  function invoke(index){void (selectingReplay?replayActions:actions)[index]();}
  function choose(index){const count=selectingReplay?replayLabels.length:buttons.length;selected=(index+count)%count;buttons.forEach((button,i)=>button.classList.toggle('selected',i===selected));}
  for(const [i,button] of buttons.entries()){button.onmouseenter=()=>choose(i);button.onclick=()=>invoke(i);}
  return {open(){replayMenu(false);panel.hidden=false;},close(){panel.hidden=true;},get visible(){return !panel.hidden;},confirmResume(){if(selectingReplay)replayMenu(false);else void resume();},move(value){choose(selected+value);},confirm(){invoke(selected);},dispose(){panel.remove();style.remove();}};
}
