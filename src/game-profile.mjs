import {NATIVE_PROFILE_PATHS,createNativeConfig,createNativeScore,decodeNativeConfig,decodeNativeScore,encodeNativeConfig,encodeNativeScore} from './native-profile.mjs';
const {config:CONFIG,score:SCORES,web:WEB_PROFILE}=NATIVE_PROFILE_PATHS;
const encoder=new TextEncoder(),decoder=new TextDecoder();
export const DEFAULT_SETTINGS=Object.freeze({bgmVolume:100,seVolume:80,
  joyButtons:Object.freeze({shoot:0,bomb:1,focus:2,pause:4,skip:3}),
  keyMap:Object.freeze({up:'ArrowUp',down:'ArrowDown',left:'ArrowLeft',right:'ArrowRight',shoot:'KeyZ',bomb:'KeyX',focus:'ShiftLeft',pause:'Escape',skip:'ControlLeft'})});
export function normalizeSettings(value={}) {
  const keyMap={...DEFAULT_SETTINGS.keyMap};
  const joyButtons={...DEFAULT_SETTINGS.joyButtons};for(const key of Object.keys(joyButtons))if(Number.isInteger(value.joyButtons?.[key])&&value.joyButtons[key]>=-1&&value.joyButtons[key]<32)joyButtons[key]=value.joyButtons[key];
  for(const key of Object.keys(keyMap))if(typeof value.keyMap?.[key]==='string'&&/^(?:Arrow(?:Up|Down|Left|Right)|Key[A-Z]|Digit[0-9]|Space|Enter|Escape|Shift(?:Left|Right)|Control(?:Left|Right))$/.test(value.keyMap[key]))keyMap[key]=value.keyMap[key];
  const volume=(value,fallback)=>Number.isFinite(value)?Math.max(0,Math.min(100,Math.round(value/5)*5)):fallback;
  return {bgmVolume:volume(value.bgmVolume,100),seVolume:volume(value.seVolume,80),keyMap,joyButtons};
}
const loadoutKey=(character,shot)=>character*2+shot;
const practiceKey=(character,shot,difficulty)=>`${loadoutKey(character,shot)}:${difficulty}`;
function freshData(){return {schema:'th12-web-score/1',scores:[],spellRecords:{},clears:{},practice:{},music:[],endingsSeen:[],lastName:'PLAYER',playSeconds:0,nativeTotals:{}};}
function validRun(r){return r&&[r.character,r.shot,r.difficulty,r.stage,r.score].every(Number.isInteger)&&r.character>=0&&r.character<3&&r.shot>=0&&r.shot<2&&r.difficulty>=0&&r.difficulty<5&&r.stage>=1&&r.stage<=7&&r.score>=0;}
const nonnegative=(value,fallback=0)=>Number.isFinite(value)?Math.max(0,value):fallback;
const count=value=>Math.min(99999,Math.trunc(nonnegative(value)));
const validPlayer=(character,shot)=>Number.isInteger(character)&&character>=0&&character<3&&Number.isInteger(shot)&&shot>=0&&shot<2;
const matchesRun=(run,{character,shot,difficulty,stage}={})=>
  (character===undefined||run.character===character)&&(shot===undefined||run.shot===shot)&&
  (difficulty===undefined||run.difficulty===difficulty)&&(stage===undefined||stage===0||run.stage===stage);
const normalizedRun=run=>({...run,practice:run.practice===true||run.mode==='practice',
  continueCount:Math.trunc(nonnegative(run.continueCount??run.continues)),
  name:String(run.name??'PLAYER').slice(0,8)});

function normalizedData(value={}){
  // Retain extension fields and the complete old browser history in the
  // sidecar. Only fields used by the game are normalized here.
  const base={...value,...freshData()};
  for(const key of ['clears','practice','spellRecords','nativeTotals','nativePractice'])if(value[key]&&typeof value[key]==='object'&&!Array.isArray(value[key]))base[key]=value[key];
  base.scores=Array.isArray(value.scores)?value.scores.filter(validRun).map(normalizedRun):[];
  base.music=Array.isArray(value.music)?[...new Set(value.music.filter(id=>/^th(?:12|10)_\d\d$/.test(id)))]:[];
  base.endingsSeen=Array.isArray(value.endingsSeen)?[...new Set(value.endingsSeen.filter(id=>Number.isInteger(id)&&id>=0&&id<12))]:[];
  if(typeof value.lastName==='string')base.lastName=value.lastName.slice(0,8);
  if(Number.isFinite(value.playSeconds))base.playSeconds=Math.max(0,value.playSeconds);
  if(Array.isArray(value.nativeMachineTotals)&&value.nativeMachineTotals.length===6)base.nativeMachineTotals=value.nativeMachineTotals;
  if(Array.isArray(value.nativeEndingFlags)&&value.nativeEndingFlags.length===12)base.nativeEndingFlags=value.nativeEndingFlags.map(flag=>Math.max(0,Math.min(255,Math.trunc(flag)||0)));
  if(Array.isArray(value.nativeSpellHistory)&&value.nativeSpellHistory.length===1792&&value.nativeSpellHistory.every(n=>Number.isInteger(n)&&n>=0))base.nativeSpellHistory=value.nativeSpellHistory;
  else{
    base.nativeSpellHistory=Array(1792).fill(0);
    for(let id=0;id<128;id++)for(let player=0;player<6;player++){
      const record=base.spellRecords[`${id}:${player}`],at=id*14+player*2;if(!record)continue;
      base.nativeSpellHistory[at]=count(record.attempts);base.nativeSpellHistory[at+1]=count(record.captures);
      base.nativeSpellHistory[id*14+12]=count(base.nativeSpellHistory[id*14+12]+count(record.attempts));
      base.nativeSpellHistory[id*14+13]=count(base.nativeSpellHistory[id*14+13]+count(record.captures));
    }
  }
  return base;
}
function sameNativeRun(a,b){
  return a.character===b.character&&a.shot===b.shot&&a.difficulty===b.difficulty&&!!a.practice===!!b.practice&&
    (!a.practice||a.stage===b.stage)&&Math.trunc((a.score??0)/10)===b.scoreStored&&
    Math.trunc(nonnegative(a.continueCount??a.continues))===b.continueCount&&
    (a.practice||String(a.name??'').trim()===String(b.name??'').trim())&&
    (!a.timestamp||!b.timestamp||Math.trunc(a.timestamp)===b.timestamp);
}
function mergeNativeData(native,supplement){
  const cached=normalizedData(supplement),base={...cached,...native},used=new Set();base.scores=native.scores.map(run=>({...run}));
  for(const saved of cached.scores.filter(run=>!run.native||run.webHistory)){
    const index=base.scores.findIndex((run,index)=>!used.has(index)&&sameNativeRun(saved,run));
    if(index>=0){used.add(index);base.scores[index]={...saved,...base.scores[index],webHistory:true};}
    else base.scores.push({...saved,native:false,nativeId:undefined,nativeEligible:false});
  }
  base.spellRecords={...cached.spellRecords};for(const [key,record] of Object.entries(native.spellRecords))base.spellRecords[key]={...base.spellRecords[key],...record};
  // The executable has113 records; browser-only extended slots stay available.
  base.nativeSpellHistory=[...native.nativeSpellHistory];for(let at=113*14;at<1792;at++)base.nativeSpellHistory[at]=cached.nativeSpellHistory[at];
  base.playSeconds=Math.max(native.playSeconds,nonnegative(cached.playSeconds));
  return base;
}

// Retail ECL422/437/438/439 allocate a separate record number per difficulty.
// Stage1 hard-only midboss and stage4/6 normal-only cards offset the ranges.
// Titles stay in the supplied resources or encountered-card records.
export function spellMetadata(id){
  if(!Number.isInteger(id)||id<0||id>112)return null;
  const stage=id<10?1:id<26?2:id<42?3:id<57?4:id<77?5:id<100?6:7;
  const difficulty=id>=100?4:id<2?id+2:id>=54&&id<=56?id-53:id>=93&&id<=95?id-92:
    id<54?(id-2)%4:id<77?(id-57)%4:id<93?(id-77)%4:(id-96)%4;
  return {id,stage,difficulty};
}

export class GameProfile {
  constructor(store){this.store=store;this.nativeConfig=createNativeConfig();this.nativeScore=createNativeScore();this.settings=normalizeSettings();this.data={...freshData(),...this.nativeScore.data};this.pending=Promise.resolve();this.warnings=[];this.legacyProfiles={};}
  async load(){
    let supplement={},webSettings={},scoreValue=null,scoreIsNative=false;
    const webBytes=await this.store.read(WEB_PROFILE);
    if(webBytes)try{const web=JSON.parse(decoder.decode(webBytes));if(web.schema==='th12-web-profile/2'){supplement=web.data??{};webSettings=web.settings??{};this.legacyProfiles=web.legacyProfiles??{};}}catch{this.warnings.push(WEB_PROFILE);}
    for(const [path,schema] of [[CONFIG,'th12-web-config/1'],[SCORES,'th12-web-score/1']]){
      const bytes=await this.store.read(path);if(!bytes)continue;
      try{
        if(decoder.decode(bytes.subarray(0,64)).trimStart().startsWith('{')){
          const value=JSON.parse(decoder.decode(bytes));if(value.schema!==schema)throw Error('unknown profile format');
          this.legacyProfiles[path===CONFIG?'config':'score']=JSON.parse(JSON.stringify(value));
          if(path===CONFIG)webSettings=value.settings??{};else scoreValue=normalizedData(value);
        }else if(path===CONFIG){this.nativeConfig=decodeNativeConfig(bytes);this.configIsNative=true;}
        else{this.nativeScore=decodeNativeScore(bytes);scoreValue=this.nativeScore.data;scoreIsNative=true;}
      }catch{
        // An invalid file remains intact until the user replaces it.
        this.warnings.push(path);this[`${path===CONFIG?'config':'score'}ReadOnly`]=true;
      }
    }
    this.settings=normalizeSettings(webSettings);
    if(this.configIsNative)this.settings={...this.settings,...this.nativeConfig.settings,keyMap:this.settings.keyMap};
    this.data=scoreIsNative?mergeNativeData(scoreValue,supplement):scoreValue??(Object.keys(supplement).length?normalizedData(supplement):{...freshData(),...this.nativeScore.data});
    return this;
  }
  canExtra(character,shot){
    const characters=character===undefined?[0,1,2]:[character],shots=shot===undefined?[0,1]:[shot];
    return characters.some(c=>shots.some(s=>this.data.nativeEndingFlags?!!(this.data.nativeEndingFlags[6+loadoutKey(c,s)]&0x10):[1,2,3].some(d=>this.data.clears[practiceKey(c,s,d)])));
  }
  maxPracticeStage(character,shot,difficulty){return Math.max(0,Math.min(6,this.data.practice[practiceKey(character,shot,difficulty)]??1));}
  canPractice(character,shot,difficulty,stage){const entries=this.data.nativePractice?.[practiceKey(character,shot,difficulty)];return stage>=1&&stage<=6&&(entries?!!entries[stage-1]?.unlocked:stage<=this.maxPracticeStage(character,shot,difficulty));}
  recordStageEntry(character,shot,difficulty,stage){
    // Retail420a64 unlocks a stage after clearing it, rather than entering it.
  }
  recordStageClear(character,shot,difficulty,stage,score=0){
    if(!validPlayer(character,shot)||!Number.isInteger(difficulty)||difficulty<0||difficulty>3||!Number.isInteger(stage)||stage<1||stage>6)return;
    const key=practiceKey(character,shot,difficulty);this.data.nativePractice??={};
    const entries=this.data.nativePractice[key]??=Array.from({length:6},(_,index)=>({scoreStored:0,completed:false,unlocked:index<(this.data.practice[key]??0)}));
    const entry=entries[stage-1];entry.completed=entry.unlocked=true;entry.scoreStored=Math.max(entry.scoreStored??0,Math.trunc(nonnegative(score)/10));
    this.data.practice[key]=Math.max(this.data.practice[key]??0,stage);
  }
  unlockMusic(id){if(/^th(?:12|10)_\d\d$/.test(id)&&!this.data.music.includes(id))this.data.music.push(id);}
  recordEndingSeen(id){if(Number.isInteger(id)&&id>=0&&id<12){if(!this.data.endingsSeen.includes(id))this.data.endingsSeen.push(id);if(this.data.nativeEndingFlags)this.data.nativeEndingFlags[id]|=1;}}
  machineTotals(character,shot){
    const player=loadoutKey(character,shot);this.data.nativeMachineTotals??=Array(6).fill(null);
    if(!this.data.nativeMachineTotals[player]){
      const totals=Array.from({length:5},(_,difficulty)=>this.data.nativeTotals[`${player}:${difficulty}`]);
      const runs=this.data.scores.filter(run=>!run.native&&run.mode!=='replay'&&run.mode!=='demo'&&run.character===character&&run.shot===shot);
      this.data.nativeMachineTotals[player]={playCount:totals.some(Boolean)?totals.reduce((sum,t)=>sum+count(t?.playCount),0):runs.length,
        playFrames:totals.some(Boolean)?totals.reduce((sum,t)=>sum+nonnegative(t?.playFrames),0):Math.round(runs.reduce((sum,r)=>sum+nonnegative(r.playSeconds??r.durationSeconds),0)*60),
        clearCounts:totals.map((total,difficulty)=>count(total?.clearCount??runs.filter(run=>run.difficulty===difficulty&&run.clear&&!run.practice).length))};
    }
    return this.data.nativeMachineTotals[player];
  }
  nativeTotals(character,shot,difficulty){
    const key=practiceKey(character,shot,difficulty),machine=this.machineTotals(character,shot);
    return this.data.nativeTotals[key]={playCount:machine.playCount,playFrames:machine.playFrames,clearCount:machine.clearCounts[difficulty]??0};
  }
  recordPlayStart(character,shot,difficulty){const t=this.machineTotals(character,shot);t.playCount=Math.min(99999,(t.playCount||0)+1);this.nativeTotals(character,shot,difficulty);}
  recordPlayFrame(character,shot,difficulty){const t=this.machineTotals(character,shot);t.playFrames=Math.min(0xcdfe5ff,(t.playFrames||0)+1);}
  recordClear(character,shot,difficulty,{continues,stage}={}){
    const t=this.machineTotals(character,shot);t.clearCounts[difficulty]=Math.min(99999,(t.clearCounts[difficulty]||0)+1);this.nativeTotals(character,shot,difficulty);
    if(stage===6&&Number.isInteger(continues)){
      const ending=loadoutKey(character,shot)+(continues?0:6);this.data.nativeEndingFlags??=Array(12).fill(0);this.data.nativeEndingFlags[ending]|=1|(difficulty>=1?0x10:0);this.recordEndingSeen(ending);
      if(!continues)this.data.clears[practiceKey(character,shot,difficulty)]=true;
    }
  }
  highScore(character,shot,difficulty,stage=0){
    const ranked=this.rankings(character,shot,difficulty,stage).reduce((best,run)=>Math.max(best,run.score??0),0);
    const practice=stage?this.data.nativePractice?.[practiceKey(character,shot,difficulty)]?.[stage-1]?.scoreStored??0:0;
    return Math.max(ranked,practice*10);
  }
  rankings(character,shot,difficulty,stage=0){
    return this.data.scores.filter(r=>matchesRun(r,{character,shot,difficulty})&&r.nativeEligible!==false&&r.ranked!==false&&
      (r.native||!nonnegative(r.continueCount??r.continues))&&r.mode!=='replay'&&r.mode!=='demo'&&
      (stage===0?!r.practice&&r.mode!=='practice':(r.practice||r.mode==='practice')&&r.stage===stage))
      .sort((a,b)=>(b.scoreStored??Math.trunc(b.score/10))-(a.scoreStored??Math.trunc(a.score/10))).slice(0,10).map(r=>({...r}));
  }
  /** Encountered cards, aggregated across the selected machine(s). Counts from
   * native history are authoritative when loaded; cached title/best values are
   * retained separately. Querying records never changes counters or RNG. */
  spellRecords({character,shot,difficulty,stage,includeUnseen=false}={}){
    const players=Array.from({length:6},(_,i)=>i).filter(player=>
      (character===undefined||Math.floor(player/2)===character)&&(shot===undefined||player%2===shot));
    const words=this.data.nativeSpellHistory,result=[];
    for(let id=0;id<128;++id){
      const metadata=spellMetadata(id);
      if(difficulty!==undefined&&metadata?.difficulty!==difficulty||stage!==undefined&&stage!==0&&metadata?.stage!==stage)continue;
      let attempts=0,captures=0,bestTime=0,bestBonus=0,title='';
      for(const player of players){
        const saved=this.data.spellRecords[`${id}:${player}`]||{};
        const native=Array.isArray(words)&&words.length===1792;
        const tried=count(native?words[id*14+player*2]:saved.attempts),captured=Math.min(tried,count(native?words[id*14+player*2+1]:saved.captures));
        attempts+=tried;captures+=captured;
        if(typeof saved.title==='string'&&saved.title)title=saved.title;
        const time=nonnegative(saved.bestTime);
        if(time>0)bestTime=bestTime>0?Math.min(bestTime,time):time;
        bestBonus=Math.max(bestBonus,nonnegative(saved.bestBonus));
      }
      if(Array.isArray(words)&&words.length===1792&&players.length===6){
        attempts=count(words[id*14+12]);captures=Math.min(attempts,count(words[id*14+13]));
      }
      if(!attempts&&!includeUnseen)continue;
      if(!metadata&&!attempts)continue;
      result.push({id,stage:metadata?.stage??null,difficulty:metadata?.difficulty??null,title,
        attempts,captures,captureRate:attempts?captures/attempts:0,bestTime,bestBonus,seen:attempts>0});
    }
    return result;
  }
  dataSummary(filter={}){
    const runs=this.data.scores.filter(run=>!run.nativePlaceholder&&!run.nativePractice&&matchesRun(run,filter)),cards=this.spellRecords(filter);
    const playSeconds=runs.reduce((total,r)=>total+nonnegative(r.playSeconds??r.durationSeconds),0);
    const machines=(this.data.nativeMachineTotals??[]).filter((machine,player)=>machine&&(filter.character===undefined||Math.floor(player/2)===filter.character)&&(filter.shot===undefined||player%2===filter.shot));
    const nativeStatistics={playCount:machines.reduce((sum,machine)=>sum+nonnegative(machine.playCount),0),playFrames:machines.reduce((sum,machine)=>sum+nonnegative(machine.playFrames),0),
      clearCount:machines.reduce((sum,machine)=>sum+(filter.difficulty===undefined?machine.clearCounts.reduce((n,value)=>n+nonnegative(value),0):nonnegative(machine.clearCounts[filter.difficulty])),0)};
    return {runs:runs.length,clears:runs.filter(r=>r.clear).length,
      noContinueClears:runs.filter(r=>r.clear&&!nonnegative(r.continueCount??r.continues)&&!r.practice&&r.mode!=='practice').length,
      practiceRuns:runs.filter(r=>r.practice||r.mode==='practice').length,
      continueRuns:runs.filter(r=>nonnegative(r.continueCount??r.continues)>0).length,
      bestScore:runs.reduce((score,run)=>Math.max(score,run.score),0),
      spellCardsSeen:cards.filter(c=>c.seen).length,spellCardsCaptured:cards.filter(c=>c.captures>0).length,
      spellAttempts:cards.reduce((total,r)=>total+r.attempts,0),spellCaptures:cards.reduce((total,r)=>total+r.captures,0),
      playSeconds:!['character','shot','difficulty','stage'].some(key=>filter[key]!==undefined&&!(key==='stage'&&filter[key]===0))?nonnegative(this.data.playSeconds):playSeconds,
      nativeStatistics,rankings:this.rankings(filter.character,filter.shot,filter.difficulty,filter.practiceStage??0)};
  }
  recordRun(result){
    if(!validRun(result))throw Error('游戏结算记录无效');
    const run=normalizedRun({...result,name:result.name??this.data.lastName,date:result.date??new Date().toISOString()});
    if(run.clear&&!run.practice&&run.stage===6){
      if(!run.continueCount)this.data.clears[practiceKey(run.character,run.shot,run.difficulty)]=true;
      const ending=loadoutKey(run.character,run.shot)+(run.continueCount?0:6);this.recordEndingSeen(ending);
      this.data.nativeEndingFlags??=Array(12).fill(0);this.data.nativeEndingFlags[ending]|=1|(run.difficulty>=1?0x10:0);
    }
    if(run.clear)this.recordStageClear(run.character,run.shot,run.difficulty,run.stage,run.practice?run.score:0);
    this.data.scores.push(run);this.data.scores.sort((a,b)=>b.score-a.score);this.data.lastName=run.name;
    return run;
  }
  recordSpell(id,character,shot,{capture=false,time=0,bonus=0,title=''}){
    if(!Number.isInteger(id)||id<0||id>127||!validPlayer(character,shot))return;
    const player=loadoutKey(character,shot),key=`${id}:${player}`,r=this.data.spellRecords[key]??{attempts:0,captures:0,bestTime:0,bestBonus:0,title};
    if(this.data.nativeSpellHistory){r.attempts=count(this.data.nativeSpellHistory[id*14+player*2]);r.captures=count(this.data.nativeSpellHistory[id*14+player*2+1]);}
    r.attempts=Math.min(99999,r.attempts+1);if(capture){r.captures=Math.min(99999,r.captures+1);if(time>0)r.bestTime=r.bestTime>0?Math.min(r.bestTime,time):time;r.bestBonus=Math.max(r.bestBonus,bonus);}
    if(title)r.title=title;this.data.spellRecords[key]=r;
    if(this.data.nativeSpellHistory){
      const words=this.data.nativeSpellHistory;words[id*14+player*2]=r.attempts;words[id*14+player*2+1]=r.captures;
      words[id*14+12]=Math.min(99999,count(words[id*14+12])+1);
      if(capture)words[id*14+13]=Math.min(99999,count(words[id*14+13])+1);
    }
  }
  spellHistoryWords(){return this.data.nativeSpellHistory?.slice()??Array(1792).fill(0);}
  updateSpellHistory(records){
    const words=records.flat(2);if(words.length!==1792||!words.every(n=>Number.isInteger(n)&&n>=0&&n<=99999))throw Error('符卡记录无效');
    this.data.nativeSpellHistory=words;
    records.forEach((row,id)=>row.slice(0,6).forEach(([attempts,captures],player)=>{const key=`${id}:${player}`;if(attempts||this.data.spellRecords[key])this.data.spellRecords[key]={...this.data.spellRecords[key],attempts,captures};}));
  }
  saveSettings(settings){this.settings=normalizeSettings(settings);return this.save();}
  save(){
    for(let character=0;character<3;character++)for(let shot=0;shot<2;shot++)for(let difficulty=0;difficulty<5;difficulty++)this.nativeTotals(character,shot,difficulty);
    // Finish browser-JSON migration before replacing its legacy filenames.
    // The sidecar also carries fields which have no retail representation.
    const web=encoder.encode(JSON.stringify({schema:'th12-web-profile/2',settings:this.settings,data:this.data,legacyProfiles:this.legacyProfiles}));
    const config=this.configReadOnly?null:encodeNativeConfig(this.settings,this.nativeConfig),scores=this.scoreReadOnly?null:encodeNativeScore(this.data,this.nativeScore);
    this.pending=this.pending.catch(()=>{}).then(async()=>{
      await this.store.write(WEB_PROFILE,web);
      if(config)await this.store.write(CONFIG,config);
      if(scores)await this.store.write(SCORES,scores);
      await this.store.sync();
    });return this.pending;
  }
}
