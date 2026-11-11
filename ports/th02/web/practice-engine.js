import {normalizePractice,validatePracticeOptions} from './practice-config.js';
// Every mutation runs synchronously inside TH02's audited OUT boundary.
export function createPracticeEngine({mod,patch,config,clock}){
  const initial=normalizePractice(config),setup=initial.setup,targeted=initial.mode==='target';let options=initial.options;
  let base=-1,d=-1,csBase=-1,resident=-1,ready=false,preparing=targeted,waiting=false,outcome=null,error=null,warped=false;
  let frames=0,elapsed=0,misses=0,bombsUsed=0,lastBombs=0,lockedPower=null,prepareFrames=0,selectedPhase=0,seenBoss=false,lastPort=0,entryRedraw=0;
  const heap=()=>mod.HEAPU8,view=()=>new DataView(heap().buffer),u8=n=>heap()[d+n],u16=n=>view().getUint16(d+n,true),i16=n=>view().getInt16(d+n,true),u32=n=>view().getUint32(d+n,true),w8=(n,x)=>{heap()[d+n]=x;},w16=(n,x)=>view().setUint16(d+n,x,true),w32=(n,x)=>view().setUint32(d+n,x,true);
  const e=patch.practice.entries,flag=(name,value)=>{heap()[csBase+e[name]]=Number(!!value);};
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
  function stocks(){if(targeted){w8(0xb2c,0);w8(0xb2d,0);}}
  function power(value){if(u8(0xaa6)!==value){w8(0xaa6,value);flag('powerDirty',1);}}
  function resources(){stocks();power(setup.power);w16(0x2802,Math.min(setup.difficulty===0?4:16,setup.rank));}
  function setHP(){
    if(!setup.hp)return;const {stage:s,phase:p,hp}=setup;
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
      if(setup.entry!=='stage'){w32(0x91d0,nil);if(setup.entry==='boss'){w8(0x2baa,0);w8(0xa91,1);w8(0xa93,0);}else{w8(0x2baa,1);w16(0x28da,u16(0x2ba6));}}
    }
    const type=active(),p=phase(),s=setup.stage;
    let reached=setup.entry==='stage'||type===setup.entry;
    if(type==='mid')reached=reached&&(s===0?u16(0x2be0)>=18:s===1?u16(0x2be0)>=320:s===2?u16(0x2be0)>=128:s===3?u16(0x2be0)>=2:u16(0x2be0)>=2);
    if(type==='boss')reached=reached&&!u8(0x2bfa)&&(setup.phase?p===setup.phase:s>=4?p%2===1:true);
    if(reached){
      ready=true;preparing=false;waiting=true;selectedPhase=p;resources();setHP();clearEntry();flag('entryPause',1);elapsed=misses=bombsUsed=0;lastBombs=u8(0xbde);clock?.setSpeed(options.speed);return;
    }
    resources();w8(0x2b94,0);w8(0x2b95,2);
    if(type==='boss'&&setup.phase>p&&s===4&&p%2===0&&p<8)w16(0x2be0,100);
    if(type==='boss'&&s===5&&p===0)w16(0x2be0,50);
    // Reach later attacks via native transitions, without executing a prior
    // stage or inventing object pointers. Their initializer remains native.
    if(type==='boss'&&setup.phase>p){if(s===0)w16(0x2bea,setup.phase===2?700:1400);else if(s===1)w16(0x2bea,p===1?701:1501);else if(s>=4&&p%2)w16(0x2bea,s===4?1600:1900);}
  }
  function finish(value){if(outcome)return;outcome=value;flag('hold',1);}
  function completed(){
    if(u8(0x8f5)!==setup.stage||u8(0x19f6)===2)return true;
    if(setup.entry==='mid')return !u8(0x2baa);
    if(active()==='boss')seenBoss=true;
    if(setup.phase)return phase()!==selectedPhase;
    return setup.entry==='boss'&&seenBoss&&u8(0x2bfa)!==0;
  }
  function status(){return {mode:initial.mode,ready,preparing,waiting,outcome,error,frames,elapsed,misses,bombsUsed,options:{...options},setup:{...setup},...(d>=0?{stage:u8(0x8f5),phase:phase(),active:active(),phaseFrame:u16(0x2be0),damage:u16(0x2bea),power:u8(0xaa6),rank:i16(0x2802),keys:u16(0x2538),lives:u8(0xb2c),bombs:u8(0xb2d),invincibility:u8(0x2b95),missTimer:u8(0x2b99),playerHit:u8(0x2b94),bombActive:!!u8(0x3e44)}:{})};}
  return {configure(value){validatePracticeOptions(value,true);if('lockPower'in value&&value.lockPower!==options.lockPower)lockedPower=value.lockPower&&ready?u8(0xaa6):null;options={...options,...value};clock?.setSpeed(preparing&&prepareFrames?2:options.speed);},
    signal(port,packed){
      lastPort=port;
      locate(packed);if(heap()[resident+39])return;
      if(port===0x7fd0){flag('targetPractice',targeted);return;}
      if(port===0x7fd3&&preparing&&setup.phase>phase()&&phase()%2&&active()==='boss')w16(0x2be0,0);
      if(port===0x7fd1){
        frames++;if(!targeted&&!ready){ready=true;lastBombs=u8(0xbde);}
        if(waiting)waiting=false;
        if(preparing){if(!prepareFrames&&setup.entry!=='stage')clock?.setSpeed(2);prepare();}
        if(ready&&!waiting&&!outcome)elapsed++;
        let bits=u16(0x2538);if(preparing||outcome)bits=0;if(options.disableBomb)bits&=~32;if(options.disableShot)bits&=~16;w16(0x2538,bits);locks();
      }
      if(port===0x7fd2&&ready&&!outcome&&!u8(0x2b99)){
        misses++;if(options.invincible)w8(0x2b94,0);else if(targeted&&!options.lockLives)finish('failed');
      }
      if(port===0x7fd5){
        // After native Boss generation but before entity collision/rendering:
        // F_REMOVE suppresses preparation entities now; next loop's native
        // invalidators restore their tiles and release the slots before the
        // input boundary can enter the arrival pause.
        if(preparing){
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
        if(preparing)entryRedraw=2;
        if(entryRedraw){w8(0x52d8,1);entryRedraw--;}
        const bombs=u8(0xbde);if(ready&&!outcome)bombsUsed+=(bombs-lastBombs)&255;lastBombs=bombs;locks();
        if(ready&&!outcome&&options.invincible&&u8(0x2b94)&&!u8(0x2b95)&&!u8(0x2b96)&&!u8(0xaa7)){misses++;w8(0x2b94,0);}
        // Bullets update after player death processing in TH02. Prioritize a
        // new collision over a simultaneous Boss completion.
        if(targeted&&ready&&!outcome){if(u8(0x2b94)&&!u8(0x2b95)&&!u8(0x2b96)&&!u8(0xaa7)&&!options.invincible&&!options.lockLives){misses++;finish('failed');}else if(completed())finish('clear');}
      }
    },get status(){return {...status(),shotLevel:d>=0?u8(0x3e42):null};},get debug(){return {base,d,csBase,resident,prepareFrames,lastPort,hold:heap()[csBase+e.hold]};}};
}
