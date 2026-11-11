// Native positions identify the upper-left of the old 4x4 marker. Keep its
// center on the actual player position; the artwork is presentation only.
export function drawHitPoint(context,x,y){
 if(!Number.isFinite(x)||!Number.isFinite(y)||x<0||y<0)return;
 const cx=Math.round(x)+2,cy=Math.round(y)+2;
 context.save();
 context.fillStyle='#161021';context.beginPath();context.arc(cx,cy,5.5,0,Math.PI*2);context.fill();
 context.fillStyle='#fff';context.beginPath();context.arc(cx,cy,4.5,0,Math.PI*2);context.fill();
 context.fillStyle='#f04d85';context.beginPath();context.arc(cx,cy,3.1,0,Math.PI*2);context.fill();
 context.fillStyle='#fff';context.beginPath();context.arc(cx,cy,1.35,0,Math.PI*2);context.fill();
 context.restore();
}
export function showHitPoint({playing,focus,focusEnabled=true,focusPoint=true,always=false}){
 return !!playing&&(!!always||focusEnabled!==false&&focusPoint!==false&&!!focus);
}
