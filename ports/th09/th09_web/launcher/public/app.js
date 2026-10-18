import "./assets/launcher/app.mjs";

const room=document.querySelector("#th09Network");room.hidden=true;setInterval(()=>{room.hidden=!document.querySelector("#gameFrame")?.contentWindow?.__th09Runtime;},500);room.onclick=()=>document.querySelector("#gameFrame")?.contentWindow?.postMessage({protocol:"eagler-touhou/1",game:"th09",command:"network-open"},location.origin);

const charge=document.querySelector('#th09Charge');
const sendCharge=down=>document.querySelector('#gameFrame')?.contentWindow?.postMessage({protocol:'eagler-touhou/1',game:'th09',command:'keyboard',code:'KeyZ',down},location.origin);
let charging=false;
charge.addEventListener('pointerdown',e=>{e.preventDefault();e.stopPropagation();charge.setPointerCapture(e.pointerId);charging=true;sendCharge(true);});
for(const event of ['pointerup','pointercancel','lostpointercapture'])charge.addEventListener(event,e=>{e.preventDefault();e.stopPropagation();if(charging){charging=false;sendCharge(false);}});
setInterval(()=>{charge.hidden=!document.querySelector('#player').classList.contains('touch-enabled')||!document.querySelector('#gameFrame')?.contentWindow?.__th09Runtime;},500);

window.addEventListener('message',e=>{const child=document.querySelector('#gameFrame')?.contentWindow,m=e.data;if(e.source===child&&e.origin===location.origin&&m?.game==='th09'&&m?.protocol==='eagler-touhou/1'&&m.event==='network-dialog')document.querySelector('#player').classList.toggle('th09-network-open',!!m.open);});
