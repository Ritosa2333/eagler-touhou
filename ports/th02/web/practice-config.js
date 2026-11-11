// TH02 native ranges: 80 power, spare lives, signed play-performance Rank.
export const PRACTICE_VERSION='th02-browser-practice/1';
export const PRACTICE_DEFAULTS=Object.freeze({invincible:false,lockLives:false,lockBombs:false,lockPower:false,lockTime:false,enemyInvincible:false,lockRank:false,disableBomb:false,disableFocus:false,disableShot:false,forceFocus:false,quickRetry:true,preparedRetry:true,showKeys:false,showInfo:false,alwaysPoint:false,speed:1});
export const PRACTICE_SETUP=Object.freeze({stage:0,character:0,difficulty:1,entry:'stage',phase:0,lives:0,bombs:0,power:80,rank:0,hp:0});
export const PRACTICE_PHASES=Object.freeze([[1,2,3],[1,2,3],[],[],[1,3,5,7,9],[1,3,5,7,9]]);
export function phaseList(s){return s.entry==='boss'?PRACTICE_PHASES[s.stage]:[];}
export function hpLimit(s){
  if(s.entry==='stage'||s.stage===2||s.entry==='mid'&&s.stage===5)return 0;
  if(s.entry==='mid')return [301,381,0,400][s.stage]||0;
  if(s.stage===0)return s.phase?[700,700,861][s.phase-1]:2261;
  if(s.stage===1)return s.phase?[701,800,899][s.phase-1]:2400;
  if(s.stage===3)return 900;
  return s.stage===4?[600,700,800,1500,1100][Math.floor((s.phase||1)/2)]:[1800,1800,1800,1800,1300][Math.floor((s.phase||1)/2)];
}
export function validatePracticeOptions(value,partial=false){
  if(!value||typeof value!=='object'||Array.isArray(value))throw Error('练习选项无效');
  for(const [k,v]of Object.entries(value))if(!(k in PRACTICE_DEFAULTS)||(k==='speed'?![.25,.5,.75,1,1.5,2].includes(v):typeof v!=='boolean'))throw Error('练习选项无效：'+k);
  if(!partial&&Object.keys(PRACTICE_DEFAULTS).some(k=>!(k in value)))throw Error('练习选项不完整');return value;
}
export function normalizePracticeSetup(value={}){
  if(!value||typeof value!=='object'||Array.isArray(value))throw Error('练习起点无效');
  const ranges={stage:[0,5],character:[0,2],difficulty:[0,4],phase:[0,9],lives:[0,0],bombs:[0,0],power:[1,80],rank:[-6,16],hp:[0,8500]};
  for(const [k,v]of Object.entries(value)){
    if(!(k in PRACTICE_SETUP))throw Error('未知练习起点：'+k);
    if(k in ranges){const [a,b]=ranges[k];if(!Number.isInteger(v)||v<a||v>b)throw Error('练习参数超出范围：'+k);}
    else if(!['stage','mid','boss'].includes(v))throw Error('练习入口无效');
  }
  const s={...PRACTICE_SETUP,...value};if(s.stage===5||s.difficulty===4){s.stage=5;s.difficulty=4;}
  if(s.entry==='mid'&&s.stage===4)throw Error('五面没有道中 Boss');
  if(s.entry==='stage')s.phase=0;
  if(s.phase&&!phaseList(s).includes(s.phase))throw Error('该入口没有此独立攻击');
  if(s.hp>hpLimit(s))throw Error(hpLimit(s)?'该攻击的初始血量范围为 1–'+hpLimit(s):'该入口暂不支持自定义血量，请使用 0（原值）');return s;
}
export function normalizePractice(value){
  if(!value||value.version!==PRACTICE_VERSION||!['full','target'].includes(value.mode))throw Error('TH02 练习版本不支持');
  return {version:PRACTICE_VERSION,mode:value.mode,setup:normalizePracticeSetup(value.setup),options:{...PRACTICE_DEFAULTS,...validatePracticeOptions(value.options||{},true)}};
}

export function createPracticeClock(initial=1){
  let speed=initial,anchor=performance.now(),elapsed=anchor,paused=false;const epoch=Date.now()-anchor,RealDate=Date;
  const now=()=>elapsed+(paused?0:(performance.now()-anchor)*speed);
  function PracticeDate(...args){if(!new.target)return new RealDate(epoch+now()).toString();return Reflect.construct(RealDate,args.length?args:[epoch+now()],new.target);}
  Object.setPrototypeOf(PracticeDate,RealDate);PracticeDate.prototype=RealDate.prototype;PracticeDate.now=()=>Math.floor(epoch+now());
  const hostPerformance=new Proxy(performance,{get(t,k){if(k==='now')return now;const value=Reflect.get(t,k,t);return typeof value==='function'?value.bind(t):value;}});
  return {Date:PracticeDate,performance:hostPerformance,now,date:()=>Math.floor(epoch+now()),get speed(){return speed;},setPaused(value){elapsed=now();anchor=performance.now();paused=!!value;},setSpeed(value){if(![.25,.5,.75,1,1.5,2,8].includes(value))throw Error('无效变速');elapsed=now();anchor=performance.now();speed=value;}};
}
