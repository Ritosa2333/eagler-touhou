const equal=(a,b)=>a.length===b.length&&a.every((value,index)=>value===b[index]);
const angleDifference=(current,previous)=>Math.atan2(Math.sin(current-previous),Math.cos(current-previous));

// 只比较相邻绘图快照；离散变化使用当前状态。
export function commandMotion(current,previous){
  const a=current.transform,b=previous?.transform;
  if(!previous||previous.texture.texture!==current.texture.texture||previous.mode!==current.mode||previous.opaque!==current.opaque||previous.topology!==current.topology||
      !equal(previous.area,current.area)||(!(a&&b)&&!equal(previous.uv,current.uv))||!equal(previous.depth,current.depth))return null;
  if(a&&b){
    if(a.left!==b.left||a.right!==b.right||a.top!==b.top||a.bottom!==b.bottom||a.scale*b.scale<=0)return null;
    const x=a.x-b.x,y=a.y-b.y,scale=(a.scale-b.scale)/a.scale,angle=angleDifference(a.angle,b.angle);
    if(![x,y,scale,angle].every(Number.isFinite)||Math.abs(x)>=64||Math.abs(y)>=64||Math.abs(scale)>.5||Math.abs(angle)>=Math.PI/2)return null;
    return {x,y,scale,angle,originX:a.x,originY:a.y};
  }
  if(a||b)return null;
  const points=current.points.map((value,index)=>value-previous.points[index]);
  return points.every(value=>Number.isFinite(value)&&Math.abs(value)<64)?{points}:null;
}
