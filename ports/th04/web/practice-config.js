export const PRACTICE_VERSION='th04-browser-practice/1';
export const PRACTICE_DEFAULTS=Object.freeze({invincible:false,lockLives:false,lockBombs:false,lockPower:false,lockTime:false,enemyInvincible:false,lockRank:false,disableBomb:false,disableFocus:false,disableShot:false,forceFocus:false,quickRetry:true,preparedRetry:true,showKeys:false,showInfo:false,alwaysPoint:false,speed:1});
export const PRACTICE_SETUP=Object.freeze({stage:0,character:0,difficulty:1,entry:'stage',phase:0,branch:'auto',lives:0,bombs:0,power:128,rank:16,hp:0});
export const PRACTICE_RANK_LIMITS=Object.freeze([[4,16],[11,24],[20,32],[22,34],[16,20]].map(Object.freeze));
// Only actual attack states; entrance/fill/transform states are not targets.
export const PRACTICE_PHASES=Object.freeze([[2,3,4],[2,3,4,5],[3],[2],[2,4,5,7,8,10,11,13,14,16,17],[2,4,8,12,14,16],[2,4,6]]);
export function phaseList(s){return s.entry==='stage'?[]:s.entry==='mid'?(s.stage===0?[3]:[1]):s.stage===3&&s.character>=2?[2,4,6,8,9,11]:s.stage===6&&s.branch==='gengetsu'?[2,3,4,5,7,8]:PRACTICE_PHASES[s.stage];}
export function validatePracticeOptions(value,partial=false){
  if(!value||typeof value!=='object'||Array.isArray(value))throw Error('练习选项无效');
  for(const [k,v]of Object.entries(value))if(!(k in PRACTICE_DEFAULTS)||(k==='speed'?![.25,.5,.75,1,1.5,2].includes(v):typeof v!=='boolean'))throw Error('练习选项无效：'+k);
  if(!partial&&Object.keys(PRACTICE_DEFAULTS).some(k=>!(k in value)))throw Error('练习选项不完整');return value;
}
export function normalizePracticeSetup(value={}){
  if(!value||typeof value!=='object'||Array.isArray(value))throw Error('练习起点无效');
  const ranges={stage:[0,6],character:[0,3],difficulty:[0,4],phase:[0,17],lives:[0,0],bombs:[0,0],power:[1,128],rank:[4,34],hp:[0,30000]};
  for(const [k,v]of Object.entries(value)){
    if(!(k in PRACTICE_SETUP))throw Error('未知练习起点：'+k);
    if(k in ranges){const [a,b]=ranges[k];if(!Number.isInteger(v)||v<a||v>b)throw Error('练习参数超出范围：'+k);}
    else if(!(k==='entry'?['stage','mid','boss']:['auto','gengetsu']).includes(v))throw Error('练习入口无效');
  }
  const s={...PRACTICE_SETUP,...value};if(s.stage===6||s.difficulty===4){s.stage=6;s.difficulty=4;}
  const [low,high]=PRACTICE_RANK_LIMITS[s.difficulty];s.rank=Math.max(low,Math.min(high,s.rank));if(s.stage!==6)s.branch='auto';
  if(s.entry==='mid'&&s.stage>3)throw Error('本版道中 Boss 定点仅开放一至四面；Extra 脚本敌人不作 HP Boss');
  if(s.entry==='stage')s.phase=0;
  if(s.phase&&!phaseList(s).includes(s.phase))throw Error('该 Boss 没有此攻击入口');return s;
}
export function normalizePractice(value){
  if(!value||value.version!==PRACTICE_VERSION||!['full','target'].includes(value.mode))throw Error('TH04 练习版本不支持');
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
