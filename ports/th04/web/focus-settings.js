// Persistent browser preference. Online display stays outside the input timeline.
export function mountFocusSettings(container,{local=false,alwaysPointControl=null,profile=null}={}){
  const boxes=[];
  for(let slot=0;slot<(local?2:1);slot++){
    const key=`th04.focus-point.${local?'local'+slot:(profile||'online')}`;
    const label=document.createElement('label'),box=document.createElement('input');
    box.type='checkbox';box.checked=true;
    try{box.checked=localStorage.getItem(key)!=='off';}catch{}
    box.addEventListener('change',()=>{try{localStorage.setItem(key,box.checked?'on':'off');}catch{}});
    label.append(box,document.createTextNode(`${local?'P'+(slot+1)+' ':''}低速显示判定点`));
    container.append(label);boxes.push(box);
  }
  // The touch marker belongs beside the low-speed marker preferences. Move
  // the original bound control so storage and the sampled input stay in sync.
  if(alwaysPointControl){
    alwaysPointControl.classList.remove('touch-option');
    container.append(alwaysPointControl);
  }
  const note=document.createElement('small');
  note.textContent='判定点设置只影响当前浏览器的画面，不影响其他玩家。';
  container.append(note);
  return ()=>boxes.reduce((mask,box,slot)=>mask|(box.checked?1<<slot:0),0);
}
