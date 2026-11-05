// A room number resolves only a read-only archive; it never joins a player seat.
const input=document.getElementById('room')||document.getElementById('code');
const button=document.getElementById('watch-room');
if(button&&input)button.onclick=async()=>{
  const output=document.getElementById('watch-status');
  button.disabled=true;
  try{
    const room=input.value.trim();if(!/^\d{4}$/.test(room))throw Error('请输入四位房间号');
    output.textContent='正在连接观战…';
    const r=await fetch('/api/replay-room?'+new URLSearchParams({room}),{cache:'no-store',signal:AbortSignal.timeout(5000)});
    const value=await r.json();if(!r.ok)throw Error(value.error||'观战连接失败');
    if(!/^[\w-]{32,80}$/.test(value.watch))throw Error('观战链接无效');
    const url=new URL('../replay.html',import.meta.url);url.searchParams.set('remote',value.watch);location.href=url.href;
  }catch(e){output.textContent=e.message||String(e);}finally{button.disabled=false;}
};
