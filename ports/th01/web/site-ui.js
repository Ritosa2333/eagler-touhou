// Shared navigation for the native edition, matching the multiorb launcher.
const query=new URLSearchParams(location.search);
const page=document.body.dataset.page==='room'?(query.get('network')==='public'?'public':'lan'):document.body.dataset.page;
const header=document.getElementById('site-header');
if(header){
  header.innerHTML='<header class="masthead"><a class="brand" href="index.html">TOUHOU / 01</a><small>東方靈異伝 · HIGHLY RESPONSIVE TO PRAYERS</small></header><nav class="mode-nav" aria-label="游戏模式"><a data-mode="solo" href="solo.html">单人游戏</a><a data-mode="local" href="local.html">本地合作</a><a data-mode="lan" href="lan.html?network=lan">局域网联机</a><a data-mode="public" href="lan.html?network=public">公网联机</a></nav>';
  header.querySelector(`[data-mode="${page}"]`)?.setAttribute('aria-current','page');
}
// Local static hosting and room hosting have separate launchers and ports.
// A room-capable origin (including a public HTTPS tunnel) stays on that origin.
fetch('/api/network',{cache:'no-store'}).then(response=>{
  if(response.ok||location.protocol!=='http:')return;
  for(const link of document.querySelectorAll('a[href^="lan.html"]')){
    const url=new URL(link.getAttribute('href'),location.href);url.port='9874';link.href=url.href;
    link.title='请先运行 start-lan.bat';
  }
}).catch(()=>{});
