import {normalizePractice,validatePracticeOptions,phaseList,PRACTICE_RANK_LIMITS} from './practice-config.js';
// TH04 DS offsets from MAIN disassembly/ReC98; never use display/HUD estimates
// to decide attack completion. TH04 boss_hittest_shots owns phase_frame.
const limits=PRACTICE_RANK_LIMITS;
const fixed=[{3:1501,4:601},{3:2001,5:700},{},{},{4:500,7:500,10:500,13:288,16:288,17:1000},{16:2500},{4:192,6:4000}];
const loops=[{2:16},{2:11,4:11},{3:40},{2:52},{2:4,5:4,8:4,11:4,14:4},{2:10,4:10,8:10,12:10,14:18},{2:36}];
export function createPracticeEngine({mod,patch,config,clock,onComplete=()=>{}}){
  const initial=normalizePractice(config),setup=initial.setup,targeted=initial.mode==='target';let options=initial.options;
  let base=-1,d=-1,csBase=-1,resident=-1,initialized=false,ready=false,preparing=targeted,waiting=false,outcome=null,error=null;
  let frames=0,elapsed=0,misses=0,bombsUsed=0,lastNativeBombs=0,lockedPower=null,warped=false,prepareFrames=0,seenBoss=false,selectedPhase=0,pendingHitCounted=false,seekFinalHp=0,idleBoundaries=0,observedFrames=-1;
  let effectiveKeys=0,effectiveFocus=0;
  let entryCleanup=null,redrawFrames=0;
  const heap=()=>mod.HEAPU8,view=()=>new DataView(heap().buffer),u8=n=>heap()[d+n],u16=n=>view().getUint16(d+n,true),i16=n=>view().getInt16(d+n,true),w8=(n,x)=>{heap()[d+n]=x;},w16=(n,x)=>view().setUint16(d+n,x,true);
  const e=patch.practice.entries,flag=(name,value)=>{heap()[csBase+e[name]]=Number(!!value);};
  function locate(packed){
    const cs=packed&65535,ds=packed>>>16,h=heap(),v=view();
    if(base>=0){csBase=base+cs*16;d=base+ds*16;return;}
    const sig=new TextEncoder().encode(patch.signature);
    for(let at=h.indexOf(sig[0]);at>=0;at=h.indexOf(sig[0],at+1)){
      if(!sig.every((b,i)=>h[at+i]===b))continue;
      const b=at-patch.mailbox_cs_offset-cs*16,data=b+ds*16;
      if(b<0||b+0x100000>h.length||data<0||data+0xba8a>h.length)continue;
      const r=b+v.getUint16(data+0xba88,true)*16+v.getUint16(data+0xba86,true);
      if(new TextDecoder().decode(h.subarray(r,r+11))!=='HUMAConfig\0')continue;
      base=b;d=data;csBase=b+cs*16;resident=r;return;
    }
    throw Error('TH04 练习运行实例身份校验失败');
  }
  function stocks(){if(targeted){heap()[resident+11]=1;heap()[resident+13]=0;}}
  function refreshResources(bits){heap()[csBase+e.resourceDirty]|=bits;}
  function setPower(value){if(u8(0x4664)!==value){w8(0x4664,value);refreshResources(1);}}
  function resources(){
    stocks();setPower(setup.power);
    // TH04 Dream consists of a pickup count (0..7) and its score cache.
    // Preparation must not carry Dream pickups into the selected entry.
    if(u8(0x4676)||u16(0xbccc)){w8(0x4676,0);w16(0xbccc,0);refreshResources(2);}
    const [a,b]=limits[setup.difficulty];w8(0x5395,Math.max(a,Math.min(b,setup.rank)));
  }
  function active(){
    if(u16(0xbcd0)&&u16(0xbcd0)!==0x11c0&&u8(0x53d9)<254)return 'boss';
    if(u16(0x46b6)&&u16(0x46b6)!==0x11c0&&u8(0x53c5)<254)return 'mid';return null;
  }
  function clearEntry(){
    // Preparation protection is removed before entering native pause. Native
    // collisions on the first resumed update have no extra grace period.
    w8(0x4662,0);w8(0x4663,0);w8(0x4669,0);w8(0x466a,0);w8(0xbcca,0);w8(0xbcba,0);
    w16(0x4656,0);w16(0x4658,0);w16(0x3974,0);
  }
  function clearEntryProjectiles(){
    // Clear live instances only. Templates and native phase initialization
    // remain owned by the selected attack; delayed gather emitters must go too.
    entryCleanup={};
    const pools=[['bullets',0x5a22,26,440],['lasers',0x42f0,24,2],['gather',0x9292,42,16],['items',0xaf34,20,32],['circles',0x9594,10,16]];
    // Marisa's four bits are part of the Boss, not stray projectiles.
    // Yuuka's safety circle is created inside phase 4, not on entry; an old
    // circle must be cleared too. Gengetsu's column coordinates have no live
    // flag and remain intact; only his current pattern reads those positions.
    const start=setup.stage===3&&setup.entry==='boss'&&setup.character<2?4:0;
    pools.push(['custom',0xb204+start*26,26,32-start]);
    for(const [name,offset,stride,count]of pools){
      let cleared=0;for(let i=0;i<count;i++){const at=offset+i*stride;if(u8(at))cleared++;w8(at,0);}entryCleanup[name]=cleared;
    }
    w8(0xbcb9,0);w8(0xbcba,0);
    heap().fill(1,d+0x4700,d+0x4d40);redrawFrames=2;
  }
  function locks(){
    stocks();if(options.lockPower&&ready){lockedPower??=u8(0x4664);setPower(lockedPower);}
    if(options.lockRank){const [a,b]=limits[u8(0x4348)]||limits[1];w8(0x5395,Math.max(a,Math.min(b,setup.rank)));}
    flag('lockLives',ready&&options.lockLives);flag('unlimitedBombs',ready&&options.lockBombs);flag('enemyLock',ready&&options.enemyInvincible);
  }
  function inputs(){
    let keys=u16(0x3974);if(preparing||outcome)keys=0;if(options.disableBomb)keys&=~16;if(options.disableShot)keys&=~32;w16(0x3974,keys);effectiveKeys=keys;
    if(options.disableFocus)w8(0x3976,0);else if(options.forceFocus)w8(0x3976,1);effectiveFocus=u8(0x3976);
  }
  function timeLock(){
    const type=active(),stage=u8(0x5394);if(!type)return;
    if(type==='mid'){
      const counter={1:0x4274,2:0x425a,3:0x4287}[stage],limit={1:17,2:12,3:9}[stage];
      if(counter&&u8(counter)>=limit-2)w8(counter,0);
      // Midboss 1 retreat is commanded by map scroll, not an HP timer.
      if(stage===0&&u8(0x53c5)===3&&u8(0x4277)>2)w8(0x4277,2);return;
    }
    const phase=u8(0x53d9),reimu=stage===3&&u8(0x5398)===1,gengetsu=stage===6&&u16(0xbcd0)===0xc7da;
    let cap=reimu?({11:1000}[phase]):gengetsu?({7:1500,8:5000}[phase]):fixed[stage]?.[phase];
    let count=reimu?({2:9,4:17,6:11,8:10,9:12}[phase]):gengetsu?({2:22,3:22,4:22,5:22}[phase]):loops[stage]?.[phase];
    if(stage===2&&phase===3)count=Math.min(40,(u8(0x46e4)+1)*8);
    const counter=stage===0?0xbced:stage===6&&!gengetsu?0x46ae:0x53df;
    if(cap&&i16(0x53da)>=cap-2)w16(0x53da,0);
    if(count&&u8(counter)>=Math.max(1,count-2))w8(counter,0);
  }
  function arrive(){
    const type=active(),phase=u8(type==='mid'?0x53c5:0x53d9),gengetsu=setup.stage===6&&u16(0xbcd0)===0xc7da;
    const wantedGen=setup.stage===6&&setup.branch==='gengetsu';
    const minimum=type==='mid'?(setup.stage===0?3:1):setup.stage===2?3:2;
    if(!(setup.entry==='stage'||type===setup.entry&&(!wantedGen||gengetsu)&&(setup.phase?phase===setup.phase:phase>=minimum)))return false;
    // Preserve the real final-survival HP carried through the preceding kill.
    if(type==='boss'&&setup.phase&&i16(0x53d6)<=0&&seekFinalHp>0)w16(0x53d6,seekFinalHp);
    if(setup.entry!=='stage')clearEntryProjectiles();
    ready=true;preparing=false;waiting=true;selectedPhase=phase;resources();refreshResources(3);clearEntry();
    if(setup.hp&&type){const at=type==='mid'?0x53c2:0x53d6,max=i16(at),min=type==='mid'?0:i16(0x53e0);w16(at,Math.max(min+1,Math.min(max,setup.hp)));}
    flag('entryPause',1);elapsed=misses=bombsUsed=0;lastNativeBombs=heap()[resident+50];clock?.setSpeed(options.speed);return true;
  }
  function prepare(){
    if(++prepareFrames>40000){error='目标准备超时，请改选关卡开头';preparing=false;flag('hold',1);return;}
    if(!warped){
      warped=true;
      // Stage assets/STD are loaded; omit only the unrelated enemy VM. The
      // original dialog still allocates and loads all boss resources.
      if(setup.entry!=='stage'){
        view().setUint32(d+0x4646,view().getUint32(d+0xbcd0,true),true);
        if(setup.entry==='boss'){w8(0x4277,0);view().setUint32(d+0x46b6,view().getUint32(d+0xbcd0,true),true);w8(0x53c5,255);w16(0x53c2,0);}
        else w16(0x538a,u16(0x53c0));
      }
    }
    const type=active(),phase=u8(type==='mid'?0x53c5:0x53d9),gengetsu=setup.stage===6&&u16(0xbcd0)===0xc7da;
    const wantedGen=setup.stage===6&&setup.branch==='gengetsu';
    if(arrive())return;
    resources();w8(0x4662,2);w8(0x4669,0);w8(0x466a,0);
    if(type==='boss'){
      // Elly's prelude uses an absolute stage-frame deadline.
      if(setup.stage===2&&phase===1)w16(0x538a,9240);
      const attacks=phaseList({...setup,entry:'boss',branch:gengetsu?'gengetsu':'auto'});
      if(attacks.includes(phase)&&(wantedGen&&!gengetsu||setup.phase>phase)){
        if(i16(0x53e0)===0&&i16(0x53d6)>0)seekFinalHp=i16(0x53d6);
        w16(0x53d6,Math.max(0,i16(0x53e0)-1));
      }
    }
  }
  function finish(value){if(outcome)return;outcome=value;flag('hold',1);onComplete(status());}
  function complete(){
    if(u8(0x5394)!==setup.stage||u8(0x5392)===2)return true;
    const phase=u8(setup.entry==='mid'?0x53c5:0x53d9);
    if(setup.entry==='mid')return setup.phase?phase!==selectedPhase:phase>=254;
    const boss=active()==='boss';if(boss&&u8(0x53d9)>=2)seenBoss=true;
    // Extra whole stage/Boss includes both sisters; Mugetsu's first defeat
    // is a native handoff, not completion of the entire Extra boss range.
    if(setup.phase)return phase!==selectedPhase;
    if(setup.stage===6&&setup.branch!=='gengetsu')return seenBoss&&u16(0xbcd0)===0xc7da&&phase>=254;
    return seenBoss&&phase>=254;
  }
  function status(){return {mode:initial.mode,ready,preparing,waiting,outcome,error,frames,elapsed,misses,bombsUsed,options:{...options},setup:{...setup},...(d>=0?{stage:u8(0x5394),phase:u8(0x53d9),midPhase:u8(0x53c5),bossHp:i16(0x53d6),phaseEndHp:i16(0x53e0),midHp:i16(0x53c2),phaseFrame:u16(0x53da),bossUpdate:u16(0xbcd0),midUpdate:u16(0x46b6),power:u8(0x4664),shotLevel:u8(0x4665),shotFunction:u16(0x449a),dream:u16(0xbccc)*10,dreamItems:u8(0x4676),rank:u8(0x5395),keys:effectiveKeys,focus:effectiveFocus,lives:heap()[resident+11]-1,bombs:heap()[resident+13],invincibility:u8(0x4662),missTimer:u8(0x466a),playerHit:u8(0x4669),bombActive:!!u8(0x4368)}:{})};}
  return {
    observeBoundary(){
      if(!targeted||!initialized||!preparing||error)return;
      const program=mod._emGetRunningProgram&&mod.UTF8ToString?.(mod._emGetRunningProgram());
      if(program&&!/^MAIN(?:\.EXE)?$/i.test(program))error='目标准备中 MAIN 已退出，请按 F12 改选目标或重新开始';
      if(observedFrames!==frames){observedFrames=frames;idleBoundaries=0;}else if(++idleBoundaries>100000)error='目标准备未能继续，请按 F12 改选目标或重新开始';
      if(error)preparing=false;
    },
    configure(value){validatePracticeOptions(value,true);if('lockPower'in value&&value.lockPower!==options.lockPower)lockedPower=value.lockPower&&ready?u8(0x4664):null;options={...options,...value};clock?.setSpeed(preparing&&prepareFrames?8:options.speed);},
    signal(port,packed){
      locate(packed);if(error||heap()[resident+62])return;
      if(port===0x7fe0){
        if(targeted){heap()[resident+17]=setup.stage;heap()[resident+19]=48+setup.stage;w8(0x5394,setup.stage);}
        initialized=true;flag('targetPractice',targeted);return;
      }
      if(!initialized||u8(0x5392)>2)return;
      if(port===0x7fe1){
        frames++;if(!targeted&&!ready){ready=true;lastNativeBombs=heap()[resident+50];}
        // Native midboss activation runs immediately after this callback.
        // A long Boss-only attempt can otherwise cross its original stage
        // deadline and spawn the unrelated midboss on top of the Boss.
        if(targeted&&setup.entry==='boss')w16(0x53c0,(u16(0x538a)-1)&65535);
        if(preparing){if(!prepareFrames&&setup.entry!=='stage')clock?.setSpeed(8);prepare();}
        if(redrawFrames){heap().fill(1,d+0x4700,d+0x4d40);redrawFrames--;}
        if(ready&&!waiting&&!outcome)elapsed++;
        inputs();locks();
      }
      if(port===0x7fe2){
        if(waiting&&ready&&!outcome)elapsed++;
        waiting=false;
        // The native pause/dialog can resense held keys after the frame hook.
        // Apply restrictions at the actual player update as well.
        inputs();
        if(options.lockTime&&ready)timeLock();
        // Player updates precede world collisions in TH04. A pending hit was
        // produced by the preceding native collision (and its bullet removed).
        if(ready&&!outcome&&u8(0x4669)&&u8(0x4662)<=1&&!u8(0x466a)){
          if(!pendingHitCounted)misses++;if(options.invincible)w8(0x4669,0);
        }
        pendingHitCounted=false;
        locks();
      }
      if(port===0x7fe3){if(targeted&&ready&&!outcome&&u8(0x466a)&&!options.lockLives)finish('failed');locks();}
      if(port===0x7fe4){
        const bombs=heap()[resident+50];if(ready&&!outcome)bombsUsed+=(bombs-lastNativeBombs)&255;lastNativeBombs=bombs;locks();
        if(ready&&!outcome){
          // A hit and a phase clear in this same native update fails first.
          if(u8(0x4669)&&u8(0x4662)<=1&&!u8(0x466a)){
            misses++;pendingHitCounted=true;
            if(options.invincible){w8(0x4669,0);pendingHitCounted=false;}
            else if(targeted&&!options.lockLives)finish('failed');
          }
          if(targeted&&!outcome&&complete())finish('clear');
        }
      }
      // The state machine has changed phase, but the selected attack has not
      // updated yet. Retire old emitters before gather/items/render run.
      if(port===0x7fe5&&preparing&&warped&&setup.entry!=='stage')arrive();
    },
    get status(){return status();},get debug(){return {base,d,csBase,resident,prepareFrames,entryCleanup};}
  };
}
