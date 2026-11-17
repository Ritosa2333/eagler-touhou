// Per-seat preferences; the native game applies movement and draws markers.
export function mountFocusSettings(onChange){
  const controls=[1,2,3].map(id=>({enabled:document.getElementById('focus-enabled-'+id),
    point:document.getElementById('point-mode-'+id)}));
  const key='th02.local.focus-settings.v1';
  const targeting=document.getElementById('team-targeting');
  try{
    const saved=JSON.parse(localStorage.getItem(key));
    targeting.checked=saved?.targeting!==false;
    controls.forEach((c,i)=>{const s=(saved?.seats||saved)?.[i];if(!s)return;
      c.enabled.checked=s.enabled!==false;
      if(['off','focus','always'].includes(s.point))c.point.value=s.point;
    });
  }catch{}
  // Ignore any percentages saved by older pages; multiplayer focus is fixed.
  const values=()=>controls.map(c=>({enabled:c.enabled.checked,percent:50,point:c.point.value}));
  const change=()=>{try{localStorage.setItem(key,JSON.stringify({seats:values(),targeting:targeting.checked}));}catch{}onChange();};
  targeting.addEventListener('change',change);
  for(const c of controls){c.enabled.addEventListener('change',change);c.point.addEventListener('change',change);}
  return ()=>{const seats=values();return {seats,targeting:targeting.checked,enabledMask:seats.reduce((m,s,i)=>m|(s.enabled?1<<i:0),0),
    pointMask:seats.reduce((m,s,i)=>m|(s.point==='always'?8<<i:s.point==='focus'?1<<i:0),0)};};
}
