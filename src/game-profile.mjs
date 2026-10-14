const CONFIG='/savesth12/th12.cfg',SCORES='/savesth12/scoreth12.dat';
const encoder=new TextEncoder(),decoder=new TextDecoder();
export const DEFAULT_SETTINGS=Object.freeze({bgmVolume:100,seVolume:80,
  keyMap:Object.freeze({up:'ArrowUp',down:'ArrowDown',left:'ArrowLeft',right:'ArrowRight',shoot:'KeyZ',bomb:'KeyX',focus:'ShiftLeft',pause:'Escape',skip:'ControlLeft'})});
export function normalizeSettings(value={}) {
  const keyMap={...DEFAULT_SETTINGS.keyMap};
  for(const key of Object.keys(keyMap))if(typeof value.keyMap?.[key]==='string'&&/^(?:Arrow(?:Up|Down|Left|Right)|Key[A-Z]|Digit[0-9]|Space|Enter|Escape|Shift(?:Left|Right)|Control(?:Left|Right))$/.test(value.keyMap[key]))keyMap[key]=value.keyMap[key];
  const volume=(value,fallback)=>Number.isFinite(value)?Math.max(0,Math.min(100,Math.round(value/5)*5)):fallback;
  return {bgmVolume:volume(value.bgmVolume,100),seVolume:volume(value.seVolume,80),keyMap};
}
const loadoutKey=(character,shot)=>character*2+shot;
const practiceKey=(character,shot,difficulty)=>`${loadoutKey(character,shot)}:${difficulty}`;
function freshData(){return {schema:'th12-web-score/1',scores:[],spellRecords:{},clears:{},practice:{},music:[],lastName:'PLAYER',playSeconds:0};}
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
  constructor(store){this.store=store;this.settings=normalizeSettings();this.data=freshData();this.pending=Promise.resolve();this.warnings=[];}
  async load(){
    for(const [path,schema] of [[CONFIG,'th12-web-config/1'],[SCORES,'th12-web-score/1']]){
      const bytes=await this.store.read(path);if(!bytes)continue;
      try{
        const value=JSON.parse(decoder.decode(bytes));
        if(value.schema!==schema)throw Error('unknown profile format');
        if(path===CONFIG)this.settings=normalizeSettings(value.settings);
        else{
          const base=freshData();for(const key of ['clears','practice','spellRecords'])if(value[key]&&typeof value[key]==='object'&&!Array.isArray(value[key]))base[key]=value[key];
          base.scores=Array.isArray(value.scores)?value.scores.filter(validRun).slice(0,420).map(normalizedRun):[];
          base.music=Array.isArray(value.music)?[...new Set(value.music.filter(id=>/^th12_\d\d$/.test(id)))]:[];
          if(typeof value.lastName==='string')base.lastName=value.lastName.slice(0,8);
          if(Array.isArray(value.nativeSpellHistory)&&value.nativeSpellHistory.length===1792&&value.nativeSpellHistory.every(n=>Number.isInteger(n)&&n>=0&&n<=99999))base.nativeSpellHistory=value.nativeSpellHistory;
          if(Number.isFinite(value.playSeconds))base.playSeconds=Math.max(0,value.playSeconds);
          this.data=base;
        }
      }catch{
        // Imported retail bytes are retained, rather than overwritten by a
        // browser JSON profile that does not pretend to be the retail codec.
        this.warnings.push(path);this[`${path===CONFIG?'config':'score'}ReadOnly`]=true;
      }
    }
    return this;
  }
  canExtra(character,shot){
    if(character===undefined)return Object.values(this.data.clears).some(Boolean);
    return [0,1,2,3].some(difficulty=>this.data.clears[practiceKey(character,shot,difficulty)]);
  }
  maxPracticeStage(character,shot,difficulty){return Math.max(1,Math.min(6,this.data.practice[practiceKey(character,shot,difficulty)]??1));}
  canPractice(character,shot,difficulty,stage){return stage>=1&&stage<=this.maxPracticeStage(character,shot,difficulty);}
  recordStageEntry(character,shot,difficulty,stage){
    if(stage<=6){const key=practiceKey(character,shot,difficulty);this.data.practice[key]=Math.max(this.data.practice[key]??1,stage);}
  }
  unlockMusic(id){if(/^th12_\d\d$/.test(id)&&!this.data.music.includes(id))this.data.music.push(id);}
  rankings(character,shot,difficulty,stage=0){
    return this.data.scores.filter(r=>matchesRun(r,{character,shot,difficulty})&&r.ranked!==false&&
      !nonnegative(r.continueCount??r.continues)&&r.mode!=='replay'&&r.mode!=='demo'&&
      (stage===0?!r.practice&&r.mode!=='practice':(r.practice||r.mode==='practice')&&r.stage===stage))
      .sort((a,b)=>b.score-a.score).slice(0,10).map(r=>({...r}));
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
    const runs=this.data.scores.filter(run=>matchesRun(run,filter)),cards=this.spellRecords(filter);
    const playSeconds=runs.reduce((total,r)=>total+nonnegative(r.playSeconds??r.durationSeconds),0);
    return {runs:runs.length,clears:runs.filter(r=>r.clear).length,
      noContinueClears:runs.filter(r=>r.clear&&!nonnegative(r.continueCount??r.continues)&&!r.practice&&r.mode!=='practice').length,
      practiceRuns:runs.filter(r=>r.practice||r.mode==='practice').length,
      continueRuns:runs.filter(r=>nonnegative(r.continueCount??r.continues)>0).length,
      bestScore:runs.reduce((score,run)=>Math.max(score,run.score),0),
      spellCardsSeen:cards.filter(c=>c.seen).length,spellCardsCaptured:cards.filter(c=>c.captures>0).length,
      spellAttempts:cards.reduce((total,r)=>total+r.attempts,0),spellCaptures:cards.reduce((total,r)=>total+r.captures,0),
      playSeconds:!['character','shot','difficulty','stage'].some(key=>filter[key]!==undefined&&!(key==='stage'&&filter[key]===0))?nonnegative(this.data.playSeconds):playSeconds,
      rankings:this.rankings(filter.character,filter.shot,filter.difficulty,filter.practiceStage??0)};
  }
  recordRun(result){
    if(!validRun(result))throw Error('游戏结算记录无效');
    const run=normalizedRun({...result,name:result.name??this.data.lastName,date:result.date??new Date().toISOString()});
    if(run.clear&&!run.practice&&run.stage===6&&!run.continueCount)this.data.clears[practiceKey(run.character,run.shot,run.difficulty)]=true;
    this.recordStageEntry(run.character,run.shot,run.difficulty,run.stage);
    this.data.scores.push(run);this.data.scores.sort((a,b)=>b.score-a.score);this.data.scores=this.data.scores.slice(0,420);this.data.lastName=run.name;
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
    const config=encoder.encode(JSON.stringify({schema:'th12-web-config/1',settings:this.settings})),scores=encoder.encode(JSON.stringify(this.data));
    this.pending=this.pending.catch(()=>{}).then(async()=>{
      if(!this.configReadOnly)await this.store.write(CONFIG,config);
      if(!this.scoreReadOnly)await this.store.write(SCORES,scores);
      await this.store.sync();
    });return this.pending;
  }
}
