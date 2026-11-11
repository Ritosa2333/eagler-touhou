// Three product entries; transport is an option within an online entry.
// Old bookmarks are normalized here, never advertised as separate modes.
export function readNetworkMode(search=location.search){
  const params=new URLSearchParams(search),legacy=params.get('network')||'lan';
  const network=legacy==='public'||legacy.startsWith('public-')||legacy==='direct-ws'?'public':'lan';
  const transport=params.get('transport')==='ws'||['public-ws','direct-ws'].includes(legacy)?'ws':'rtc';
  const rollback=['on','off'].includes(params.get('rollback'))?params.get('rollback'):null;
  return {network,transport,rollback};
}
export function networkUrl(mode){
  const params=new URLSearchParams({network:mode.network});
  if(mode.transport==='ws')params.set('transport','ws');
  if(mode.rollback)params.set('rollback',mode.rollback);
  return `lan.html?${params}`;
}
