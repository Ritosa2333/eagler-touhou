import {normalizePractice,validatePracticeOptions} from './practice-config.js';
// Every mutation runs synchronously inside TH02's audited OUT boundary.
export function createPracticeEngine({mod,patch,config,clock,restarting=false}){
  const initial=normalizePractice(config),setup=initial.setup;let targeted=initial.mode==='target',options=initial.options;
  let base=-1,d=-1,csBase=-1,resident=-1,ready=false,preparing=targeted,waiting=false,outcome=null,error=null,warped=false;
  let frames=0,elapsed=0,misses=0,bombsUsed=0,lastBombs=0,lockedPower=null,prepareFrames=0,selectedPhase=0,seenBoss=false,lastPort=0,entryRedraw=0,fatal=false;
  let restartMusicWait=null;
  let loadCount=0;
  const heap=()=>mod.HEAPU8,view=()=>new DataView(heap().buffer),u8=n=>heap()[d+n],u16=n=>view().getUint16(d+n,true),i16=n=>view().getInt16(d+n,true),u32=n=>view().getUint32(d+n,true),w8=(n,x)=>{heap()[d+n]=x;},w16=(n,x)=>view().setUint16(d+n,x,true),w32=(n,x)=>view().setUint32(d+n,x,true);
  const e=patch.practice.entries,flag=(name,value)=>{heap()[csBase+e[name]]=Number(!!value);};
  const assistKeys=['invincible','lockLives','lockBombs','lockPower','lockTime','enemyInvincible'];
  function writeGuestAssists(){if(csBase>=0&&Number.isInteger(e.assistFlags))heap()[csBase+e.assistFlags]=assistKeys.reduce((bits,key,i)=>bits|(options[key]?1<<i:0),0);}
  function readGuestAssists(){if(csBase<0||!Number.isInteger(e.assistFlags))return;const bits=heap()[csBase+e.assistFlags];for(let i=0;i<assistKeys.length;i++){const key=assistKeys[i],next=!!(bits&(1<<i));if(key==='lockPower'&&next!==options[key])lockedPower=next&&ready?u8(0xaa6):null;options[key]=next;}}
  function locate(packed){
    const cs=packed&65535,ds=packed>>>16,h=heap(),v=view(),sig=new TextEncoder().encode(patch.signature);
    if(base>=0&&sig.every((b,i)=>h[base+cs*16+patch.entries.mailbox+i]===b)){
      const data=base+ds*16,r=base+v.getUint16(data+0x27fe,true)*16+v.getUint16(data+0x27fc,true);
      if(new TextDecoder().decode(h.subarray(r,r+11))==='MIKOConfig\0'){d=data;csBase=base+cs*16;resident=r;return;}
    }
    // Also revalidate program identity on reload; never reuse an old DS.
    for(let at=h.indexOf(sig[0]);at>=0;at=h.indexOf(sig[0],at+1)){
      if(!sig.every((b,i)=>h[at+i]===b))continue;
      const b=at-patch.entries.mailbox-cs*16,data=b+ds*16;
      if(b<0||b+0x100000>h.length||data<0||data+0x9400>h.length)continue;
      const r=b+v.getUint16(data+0x27fe,true)*16+v.getUint16(data+0x27fc,true);
      if(new TextDecoder().decode(h.subarray(r,r+11))!=='MIKOConfig\0')continue;
      base=b;d=data;csBase=b+cs*16;resident=r;return;
    }
    throw Error('TH02 练习运行实例身份校验失败');
  }
  function active(){return u8(0x19f6)===1&&u32(0x1a0c)===u32(0x1a10)?'boss':u8(0x2baa)?'mid':null;}
  function phase(){const s=u8(0x8f5);if(s===0)return u16(0x2bea)<700?1:u16(0x2bea)<1400?2:3;if(s===1)return u8(0x7886)+1;if(s===4)return u16(0x91f8);if(s===5)return u8(0x7b42);return 0;}
  function stocks(){if(targeted&&preparing&&!Number.isInteger(e.initialPower)){w8(0xb2c,setup.lives);w8(0xb2d,setup.bombs);}}
  function power(value){if(u8(0xaa6)!==value){w8(0xaa6,value);flag('powerDirty',1);}}
  function resources(){if(Number.isInteger(e.initialPower))return;stocks();power(setup.power);w16(0x2802,Math.min(setup.difficulty===0?4:16,setup.rank));}
  function setHP(){
    if(!setup.hp||Number.isInteger(e.initialHp))return;const {stage:s,phase:p,hp}=setup;
    const end=setup.entry==='mid'?[301,381,0,400][s]:s===0?(p?[700,1400,2261][p-1]:2261):s===1?(p?[701,1501,2400][p-1]:2400):s===3?900:s===4?[600,700,800,1500,1100][Math.floor((p||1)/2)]:[1800,1800,1800,1800,1300][Math.floor((p||1)/2)];
    w16(0x2bea,end-hp);
  }
  function locks(){
    stocks();if(options.lockPower&&ready){lockedPower??=u8(0xaa6);power(lockedPower);}
    if(options.lockRank)w16(0x2802,Math.min(u8(0x8f4)===0?4:16,setup.rank));
    flag('lockLives',ready&&options.lockLives);flag('unlimitedBombs',ready&&options.lockBombs);flag('enemyLock',ready&&options.enemyInvincible);
    flag('lockTime',ready&&options.lockTime);
    // Extra's mid encounter is a 420-frame script with no HP. Repeat its
    // complete pattern, allowing native frame-one initialization each cycle.
    if(ready&&options.lockTime&&active()==='mid'&&u8(0x8f5)===5&&u16(0x2be0)>=420)w16(0x2be0,0);
  }
  function clearEntry(){for(const n of [0x2b94,0x2b95,0x2b96,0x2b99,0xaa7,0x2b97])w8(n,0);w16(0x2538,0);w8(0x109d,255);w8(0x109e,255);}
  function prepare(){
    if(++prepareFrames>30000){error='TH02 练习目标准备超时，请改选关卡开头';preparing=false;flag('hold',1);return;}
    if(prepareFrames===1&&setup.entry!=='stage'){
      // Let the next native loop erase both title rows before the Boss
      // initializer can reset the stage clock. Keep this frame on the stage.
      resources();w8(0x2b94,0);w8(0x2b95,2);w32(0x2ba2,159);return;
    }
    if(!warped){
      warped=true;const nil=u32(0x1a0c);
      // Disable the unrelated enemy spawner, then ask the native scroll/Boss
      // transition or midboss update to initialize its actual resources.
      if(!Number.isInteger(e.initialEntry)&&setup.entry!=='stage'){w32(0x91d0,nil);if(setup.entry==='boss'){w8(0x2baa,0);w8(0xa91,1);w8(0xa93,0);}else{w8(0x2baa,1);w16(0x28da,u16(0x2ba6));}}
    }
    const type=active(),p=phase(),s=setup.stage;
    let reached=setup.entry==='stage'||type===setup.entry;
    if(type==='mid')reached=reached&&(s===0?u16(0x2be0)>=18:s===1?u16(0x2be0)>=320:s===2?u16(0x2be0)>=128:s===3?u16(0x2be0)>=2:u16(0x2be0)>=2);
    if(type==='boss')reached=reached&&!u8(0x2bfa)&&(setup.phase?p===setup.phase:s>=4?p%2===1:true);
    if(Number.isInteger(e.targetReached))reached=!!heap()[csBase+e.targetReached];
    if(reached){
      resources();if(Number.isInteger(e.arrivalPending))flag('arrivalPending',1);else{w32(0xb28,0);w32(0x3e3c,0);w16(0x3e40,0);w16(0xb46,0);clearEntry();}ready=true;preparing=false;if(Number.isInteger(e.targetPreparing))flag('targetPreparing',0);waiting=true;selectedPhase=p;setHP();flag('entryPause',1);elapsed=misses=bombsUsed=0;lastBombs=u8(0xbde);clock?.setSpeed(options.speed);return;
    }
    resources();w8(0x2b94,0);w8(0x2b95,2);
    if(!Number.isInteger(e.initialPhase)&&type==='boss'&&setup.phase>p&&s===4&&p%2===0&&p<8)w16(0x2be0,100);
    if(!Number.isInteger(e.initialPhase)&&type==='boss'&&s===5&&p===0)w16(0x2be0,50);
    // Reach later attacks via native transitions, without executing a prior
    // stage or inventing object pointers. Their initializer remains native.
    if(!Number.isInteger(e.initialPhase)&&type==='boss'&&setup.phase>p){if(s===0)w16(0x2bea,setup.phase===2?700:1400);else if(s===1)w16(0x2bea,p===1?701:1501);else if(s>=4&&p%2)w16(0x2bea,s===4?1600:1900);}
  }
  function finish(value){if(outcome)return;outcome=value;flag('hold',1);}
  function completed(){
    if(u8(0x8f5)!==setup.stage||u8(0x19f6)===2)return true;
    if(setup.entry==='mid')return !u8(0x2baa);
    if(active()==='boss')seenBoss=true;
    if(setup.phase)return phase()!==selectedPhase;
    return seenBoss&&u8(0x2bfa)!==0;
  }
  function liveGuest(){const h=heap(),at=csBase+patch.entries.mailbox;return csBase>=0&&resident>=0&&at>=0&&at+36<=h.length&&resident+80<=h.length&&'TH02SOLOINPUTv1!'.split('').every((c,i)=>h[at+i]===c.charCodeAt(0))&&[1,2].includes(h[at+22])&&'MIKOConfig'.split('').every((c,i)=>h[resident+i]===c.charCodeAt(0))&&h[resident+10]===0&&h[resident+11]<=5&&h[resident+39]===0;}
  function status(){const live=liveGuest();return {nativeInfo:Number.isInteger(e.infoEnabled),mode:targeted?'target':'full',ready,preparing,waiting,outcome,error,frames,elapsed,misses,bombsUsed,options:{...options},setup:{...setup},...(csBase>=0&&Number.isInteger(e.resultRequest)?{nativeResult:{visible:live&&!!heap()[csBase+e.resultVisible],request:live?heap()[csBase+e.resultRequest]:0}}:{}),...(resident>=0?{restartSetup:{stage:heap()[resident+22]===4?5:0,difficulty:heap()[resident+22],character:heap()[resident+38],lives:heap()[resident+26],bombs:heap()[resident+25]}}:{}),...(csBase>=0&&Number.isInteger(e.formOpen)?{nativeForm:{visible:live&&heap()[csBase+e.formOpen]===2,request:live?heap()[csBase+e.formRequest]:0,values:live?Array.from(heap().subarray(csBase+e.formValues,csBase+e.formValues+10)):[],hp:live?view().getUint16(csBase+e.formHp,true):0,settings:live?Array.from(heap().subarray(csBase+e.formOptions,csBase+e.formOptions+11)):[]}}:{}),...(csBase>=0&&Number.isInteger(e.pauseOpen)?{nativePause:{visible:live&&!!heap()[csBase+e.pauseOpen],choice:live?heap()[csBase+e.pauseOpen+1]:0,request:live?heap()[csBase+e.pauseRequest]:0}}:{}),...(d>=0?{stage:u8(0x8f5),phase:phase(),active:active(),phaseFrame:u16(0x2be0),damage:u16(0x2bea),power:u8(0xaa6),rank:i16(0x2802),keys:u16(0x2538),lives:u8(0xb2c),bombs:u8(0xb2d),invincibility:u8(0x2b95),missTimer:u8(0x2b99),playerHit:u8(0x2b94),bombActive:!!u8(0x3e44)}:{})};}
  return {exitPractice(){targeted=false;preparing=false;waiting=false;outcome=null;flag('targetPractice',0);if(Number.isInteger(e.targetPreparing))flag('targetPreparing',0);flag('entryPause',0);flag('hold',0);clock?.setSpeed(options.speed);},gameOver(){if(targeted&&ready&&!outcome)finish('failed');},clear(){if(targeted&&ready&&!outcome)finish('clear');},configure(value){validatePracticeOptions(value,true);if('lockPower'in value&&value.lockPower!==options.lockPower)lockedPower=value.lockPower&&ready?u8(0xaa6):null;options={...options,...value};writeGuestAssists();clock?.setSpeed(preparing&&prepareFrames?8:options.speed);},
    signal(port,packed){
      lastPort=port;
      locate(packed);if(heap()[resident+39])return;
      // A reloaded MAIN can inherit a stopped music driver whose completion
      // status never reaches 0xff. Skip this wait once, then restore its code.
      if(port===0x7fd0&&restarting){const at=csBase+0x73d;if(heap()[at]!==0x9a)throw Error('TH02 restart music wait mismatch');restartMusicWait={at,bytes:heap().slice(at,at+5)};heap().fill(0x90,at,at+5);restarting=false;}
      if(port===0x7fd1&&restartMusicWait){heap().set(restartMusicWait.bytes,restartMusicWait.at);restartMusicWait=null;}
      if(port===0x7fd0){loadCount++;writeGuestAssists();flag('targetPractice',targeted);if(Number.isInteger(e.targetPreparing))flag('targetPreparing',preparing);if(Number.isInteger(e.initialPower)){heap()[csBase+e.initialLives]=setup.lives;heap()[csBase+e.initialBombs]=setup.bombs;heap()[csBase+e.initialPower]=setup.power;view().setInt16(csBase+e.initialRank,Math.min(setup.difficulty===0?4:16,setup.rank),true);}if(Number.isInteger(e.initialPhase))heap()[csBase+e.initialPhase]=setup.phase;if(Number.isInteger(e.initialEntry))heap()[csBase+e.initialEntry]=({stage:0,mid:1,boss:2})[setup.entry];if(Number.isInteger(e.initialHp))view().setUint16(csBase+e.initialHp,setup.hp,true);if(Number.isInteger(e.initialStage))heap()[csBase+e.initialStage]=setup.stage;stocks();return;}
      readGuestAssists();
      if(port===0x7fd3&&!Number.isInteger(e.initialPhase)&&preparing&&setup.phase>phase()&&phase()%2&&active()==='boss')w16(0x2be0,0);
      if(port===0x7fd1){
        if(Number.isInteger(e.infoEnabled)&&Number.isInteger(e.infoValues)){
          heap()[csBase+e.infoEnabled]=(options.showInfo||options.showKeys)?1:0;
          const info=[misses,bombsUsed,u8(0xaa6),i16(0x2802),u16(0x2538)],v=view(),p=csBase+e.infoValues;
          for(const [i,n] of [u8(0xb2c),u8(0xb2d),info[0],info[1],info[2],Math.max(0,info[3]),Math.floor(elapsed/56.4),options.showKeys?info[4]:0].entries())v.setUint16(p+i*2,Math.max(0,Math.min(65535,n)),true);
        }
        frames++;if(!targeted&&!ready){ready=true;lastBombs=u8(0xbde);}
        if(waiting)waiting=false;
        if(preparing){if(!prepareFrames&&setup.entry!=='stage')clock?.setSpeed(8);prepare();}
        if(ready&&!waiting&&!outcome)elapsed++;
        let bits=u16(0x2538);if(preparing||outcome)bits=0;if(options.disableBomb)bits&=~32;if(options.disableShot)bits&=~16;w16(0x2538,bits);locks();
      }
      if(port===0x7fd2&&ready&&!outcome&&!u8(0x2b99)){
        misses++;if(options.invincible)w8(0x2b94,0);
      }
      if(port===0x7fd5){
        // After native Boss generation but before entity collision/rendering:
        // F_REMOVE suppresses preparation entities now; next loop's native
        // invalidators restore their tiles and release the slots before the
        // input boundary can enter the arrival pause.
        if(preparing&&!Number.isInteger(e.targetPreparing)){
          for(let n=0x2c18;n<0x2c18+150*20;n+=20)if(u8(n)===1)w8(n,2);
          for(let n=0x3ce6;n<0x3ce6+20*16;n+=16)if(u8(n)===1)w8(n,2);
          // TH02's delayed/firing lasers are a separate 12 x 12-byte pool.
          // Their native invalidator also consumes F_REMOVE (2); clearing
          // only the ordinary bullet pool leaves prior attacks armed.
          for(let n=0x5f6c;n<0x5f6c+12*12;n+=12)if(u8(n)===1)w8(n,2);
        }
      }
      if(port===0x7fd4){
        // Native Bomb cleanup uses this full-tile redraw flag on consecutive
        // pages (E3E5). TM_NONE Bosses repaint their own background. The
        // redraw after arrival never retires target shots or advances attacks.
        if(!Number.isInteger(e.targetPreparing)){if(preparing)entryRedraw=2;if(entryRedraw){w8(0x52d8,1);entryRedraw--;}}
        const bombs=u8(0xbde);if(ready&&!outcome)bombsUsed+=(bombs-lastBombs)&255;lastBombs=bombs;locks();
        if(ready&&!outcome&&options.invincible&&u8(0x2b94)&&!u8(0x2b95)&&!u8(0x2b96)&&!u8(0xaa7)){misses++;w8(0x2b94,0);}
        // Bullets update after player death processing in TH02. Prioritize a
        // new collision over a simultaneous Boss completion.
        if(targeted&&ready&&!outcome){if(Number.isInteger(e.targetOutcome)){const result=heap()[csBase+e.targetOutcome];if(result)finish(result===1?'clear':'failed');}else{const miss=u8(0xaa7),frame=u8(0x2b99),lives=u8(0xb2c);if(!miss||lives>0)fatal=false;if(miss&&lives===0&&frame>0&&frame<24)fatal=true;if(fatal&&miss&&lives===0&&frame===0)finish('failed');else if(!miss&&!u8(0x2b94)&&completed())finish('clear');}}
      }
    },get loadCount(){return loadCount;},get status(){return {...status(),shotLevel:d>=0?u8(0x3e42):null};},get debug(){return {base,d,csBase,resident,prepareFrames,lastPort,hold:heap()[csBase+e.hold]};}};
}
