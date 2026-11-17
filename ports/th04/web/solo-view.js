// Remove the launch page from layout and keyboard navigation during play.
// This is presentation isolation, not a separate emulator thread.
export function mountSoloView(host) {
  const style=document.createElement('style');
  style.textContent=`
    .solo-view-hidden{display:none!important}
    html.solo-game-view body{background:#000!important;overflow:hidden}
    html.solo-game-view body::before,html.solo-game-view body::after{display:none!important}
  `;
  document.head.append(style);
  let active=false,previous=[];
  function update(){
    const next=host.classList.contains('immersive');
    if(next===active)return;
    active=next;document.documentElement.classList.toggle('solo-game-view',active);
    if(active){
      for(let branch=host;branch&&branch!==document.body;branch=branch.parentElement){
        for(const sibling of branch.parentElement.children){
          if(sibling===branch||['SCRIPT','STYLE','LINK'].includes(sibling.tagName))continue;
          previous.push([sibling,sibling.inert]);
          sibling.classList.add('solo-view-hidden');sibling.inert=true;
        }
      }
    }else{
      for(const [element,inert] of previous){element.classList.remove('solo-view-hidden');element.inert=inert;}
      previous=[];
    }
  }
  // Observe only the player class; no per-frame work or subtree observation.
  new MutationObserver(update).observe(host,{attributes:true,attributeFilter:['class']});
  update();
}
