export function mountPauseMenu(host,{resume,exit,discard,restart}){
  const panel=document.createElement('div');panel.className='pc98-pause-menu';panel.hidden=true;panel.setAttribute('role','dialog');panel.setAttribute('aria-label','暂停');
  panel.innerHTML='<div class="pc98-pause-items"><button>继续游戏</button><button>退出游戏</button><button>不保存录像退出</button><button>重新开始</button></div>';
  const style=document.createElement('style');style.textContent=`.pc98-pause-menu[hidden]{display:none!important}.pc98-pause-menu{position:absolute;left:5%;top:3.33%;width:60%;height:93.33%;background:rgba(0,0,0,.8);z-index:30000;container-type:size;pointer-events:auto}.pc98-pause-items{position:absolute;top:31.25%;left:31%;width:34%;display:flex;flex-direction:column;gap:1cqh}.pc98-pause-items button{box-sizing:border-box;width:100%;min-height:5.58cqh;padding:0;border:0;border-radius:0;color:white;background:#294a70;font:clamp(10px,3.6cqh,23px)/1.15 sans-serif;cursor:pointer}.pc98-pause-items button.selected,.pc98-pause-items button:hover{background:#477aac;outline:1px solid #b5d9ff}`;
  document.head.append(style);host.append(panel);const buttons=[...panel.querySelectorAll('button')],actions=[resume,exit,discard,restart];let selected=0;
  function choose(index){selected=(index+buttons.length)%buttons.length;buttons.forEach((button,i)=>button.classList.toggle('selected',i===selected));}
  for(const [i,button] of buttons.entries()){button.onmouseenter=()=>choose(i);button.onclick=()=>void actions[i]();}
  return {open(){choose(0);panel.hidden=false;},close(){panel.hidden=true;},get visible(){return !panel.hidden;},confirmResume(){void resume();},move(value){choose(selected+value);},confirm(){void actions[selected]();},dispose(){panel.remove();style.remove();}};
}
