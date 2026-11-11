// Load separately so import/initialization errors remain visible on the page.
const start=document.getElementById('start');
const status=document.getElementById('status');
try{
  await import('./solo.js');
  status.textContent='等待启动';
  start.disabled=false;
}catch(error){
  console.error(error);
  const message=`单机页面初始化失败：${error.message||error}`;
  status.textContent=message;status.dataset.error='true';
  const cover=document.getElementById('cover');
  if(cover)cover.textContent=message;
  start.textContent='刷新重试';start.disabled=false;
  start.onclick=()=>location.reload();
}
