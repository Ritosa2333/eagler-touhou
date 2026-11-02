// TH12 1.00b: curve dispatcher 0x464f80 / mode table 0x46523c.
// Velocity and Hermite belong to the interpolator, not this curve.
export function easing(t,mode){
  t=Math.max(0,Math.min(1,t));
  if(mode>=1&&mode<=3)return t**(mode+1);
  if(mode>=4&&mode<=6)return 1-(1-t)**(mode-2);
  if(mode>=9&&mode<=11){const p=mode-7;return t<.5?(2*t)**p/2:1-(2-2*t)**p/2;}
  if(mode>=12&&mode<=14){const p=mode-10;return t<.5?(1-(1-2*t)**p)/2:((2*t-1)**p+1)/2;}
  return mode===15?0:mode===16?1:t;
}
