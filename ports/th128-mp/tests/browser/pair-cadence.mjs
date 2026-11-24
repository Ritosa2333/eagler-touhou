const status=document.querySelector('#display-clock'),intervals=[];
let previous=null,frame=0;
function inspect(now){
  if(previous!==null)intervals.push(Math.round((now-previous)*100)/100);
  previous=now;frame++;
  if(intervals.length>16)intervals.shift();
  status.textContent=JSON.stringify({frame,visibility:document.visibilityState,gaps:intervals});
  if(frame<90)requestAnimationFrame(inspect);
}
requestAnimationFrame(inspect);
