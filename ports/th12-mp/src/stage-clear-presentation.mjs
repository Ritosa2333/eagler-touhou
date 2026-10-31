export function stageClearAward(h,practice=false){
  const stage=h[39]|0,difficulty=h[19]|0,power=h[4]|0,lives=h[5]|0,bombs=h[6]|0;
  let points=Math.trunc(h[20]/100);points-=points%10;
  const base=stage*1000000;
  if(practice)return base;
  if(stage===6){
    if(difficulty===4)return base+(power+lives*100)*400000+points*100;
    return base+(power+(bombs+lives*2)*[25,50,100,150][difficulty])*10000+points*100;
  }
  return stage===7?base+(points+(power+(bombs+lives*2)*150)*100)*100:base;
}
export function stageClearText(h,poses,practice=false){
  //421350 binds front85;41f040 borrows its alpha for the reward digits.
  const clear=poses.find(p=>p.a?.name==='front'&&p.a.sprite===70&&p.a.visible);
  if(!clear)return null;
  const alpha=Math.max(0,Math.min(255,Math.round(clear.a.alpha*255)));
  return {text:stageClearAward(h,practice).toLocaleString('en-US'),font:3,x:224,y:200,z:0,scaleX:1,scaleY:1,alignX:0,alignY:0,colorARGB:((alpha<<24)|0xffffff)>>>0};
}
export function stageTransitionPose(transition){
  if(!transition)return null;
  //4057e0 ->452a60(2,30,...,11),452470: black field rectangle at draw11.
  //451ed0/4523e0 use byte alpha with a linear30-tick timer.
  const age=Math.max(0,Math.min(30,transition.age)),alpha=transition.out?Math.trunc(255*age/30):Math.trunc(255-255*age/30);
  return {rectangle:{bounds:[32,16,416,464],argb:(alpha<<24)>>>0},drawPriority:11,clip:1};
}
