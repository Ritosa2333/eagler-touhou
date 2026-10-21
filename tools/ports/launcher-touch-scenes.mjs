import assert from 'node:assert/strict';

/** Shared DOM -> production controller -> native consumer checks.
 * Native keyboard samples supply the speed reference independently of touch. */
export async function runLauncherTouchScenes(io,{timeoutTicks=8,positionEpsilon=.02,fireMode}={}){
  const evidence={schema:'eagler-touhou/launcher-touch-scenes/1',fireMode,samples:[],gestures:[],comparisons:[]};
  for(const name of ['pointer','key','gamepad','step','readActions','observe','cancel'])
    assert.equal(typeof io?.[name],'function',`Launcher touch scene needs ${name}`);
  assert.ok(Number.isInteger(timeoutTicks)&&timeoutTicks>0&&timeoutTicks<=32);
  assert.ok(['toggle','held-key'].includes(fireMode),'Declare the product touch fire ownership mode');
  const sample=async label=>{
    const action=await io.readActions(),runtime=await io.observe();
    assert.equal(action?.available,true,`${label}: native action observer is unavailable`);
    assert.equal(action.complete,true,`${label}: player consumer did not run`);
    assert.equal(action.phase,'consumer-run',`${label}: player consumer is paused`);
    assert.ok(Number.isFinite(runtime?.position?.x)&&Number.isFinite(runtime?.position?.y),`${label}: native position is absent`);
    const entry={label,action,position:{...runtime.position}};evidence.samples.push(entry);return entry;
  };
  let previous;
  const ticks=async(label,predicate=()=>true)=>{
    for(let n=0;n<timeoutTicks;n++){
      await io.step(1);const current=await sample(`${label}-${n+1}`);
      assert.equal(current.action.sequence,previous.action.sequence+1,`${label}: native consumer skipped a tick`);
      previous=current;if(predicate(current))return current;
    }
    assert.fail(`${label}: native consumer never reached the requested state`);
  };
  const gesture=async(control,type)=>{evidence.gestures.push({control,type});await io.pointer({control,type});};
  const speed=async(label)=>{
    // Drain the declared one-update input queue before measuring a full tick.
    await ticks(label+'-settle');const before=previous;
    const after=await ticks(label+'-measure');
    return Math.hypot(after.position.x-before.position.x,after.position.y-before.position.y);
  };
  let failure;
  try{
    await io.cancel();await io.step(2);previous=await sample('baseline');
    assert.equal(previous.action.focused,false);assert.equal(previous.action.consumed.fire,false);
    await io.key('ArrowRight',true);await ticks('physical-fast',s=>!s.action.focused);
    const fast=await speed('physical-fast');assert.ok(fast>positionEpsilon,'Physical reference did not move the actual player');
    await io.key('ShiftLeft',true);await ticks('physical-focus',s=>s.action.focused&&s.action.consumed.focus);
    const slow=await speed('physical-slow');assert.ok(slow>positionEpsilon&&slow<fast-positionEpsilon,'Physical Shift did not establish a distinct native slow speed');
    await io.key('ShiftLeft',false);await ticks('physical-focus-release',s=>!s.action.focused&&!s.action.consumed.focus);
    await gesture('focus','pointerdown');await gesture('focus','pointerup');
    await ticks('toggle-focus-on',s=>s.action.focused&&s.action.consumed.focus);
    const toggled=await speed('toggle-slow');
    assert.ok(Math.abs(toggled-slow)<=positionEpsilon,'Toggle focus speed differs from the native Shift reference');
    for(let n=0;n<3;n++){
      const held=await ticks('toggle-persists');assert.equal(held.action.focused,true,'Toggle focus was released with the pointer');
    }
    await gesture('focus','pointerdown');await gesture('focus','pointerup');
    await ticks('toggle-focus-off',s=>!s.action.focused&&!s.action.consumed.focus);
    const restored=await speed('toggle-fast');
    assert.ok(Math.abs(restored-fast)<=positionEpsilon,'Second focus activation did not restore the native fast speed');
    evidence.comparisons.push({kind:'toggle-focus-native-speed',fast,slow,toggled,restored,positionEpsilon});
    await io.key('ArrowRight',false);await ticks('direction-release');
    for(const [owner,hold] of [['physical',down=>io.key('KeyZ',down)],['gamepad',down=>io.gamepad(down)]]){
      await hold(true);await ticks(`${owner}-fire-on`,s=>s.action.consumed.fire);
      await gesture('fire','pointerdown');if(fireMode==='toggle')await gesture('fire','pointerup');
      await ticks(`${owner}-and-touch-fire`,s=>s.action.consumed.fire);
      await hold(false);await ticks(`${owner}-fire-release`);
      for(let n=0;n<timeoutTicks;n++)assert.equal((await ticks(`${owner}-release-touch-remains`)).action.consumed.fire,true,
        owner==='physical'?'Physical key release removed the independent touch fire owner':
          'Gamepad release removed the independent touch fire owner');
      await hold(true);await ticks(`${owner}-fire-again`,s=>s.action.consumed.fire);
      if(fireMode==='toggle')await gesture('fire','pointerdown');await gesture('fire','pointerup');
      for(let n=0;n<3;n++)assert.equal((await ticks(`${owner}-fire-remains`)).action.consumed.fire,true,
        `Touch release removed the independent ${owner} fire owner`);
      // Keep a transient touch owner active at cancellation. Toggle Fire is
      // explicitly off first, so it cannot mask loss of the other input owner.
      await gesture('focus','pointerdown');await gesture('focus','pointerup');
      await ticks(`${owner}-with-touch-focus`,s=>s.action.focused&&s.action.consumed.fire);
      await io.cancel();
      await ticks(`${owner}-cancel-focus-release`,s=>!s.action.focused&&!s.action.consumed.focus);
      for(let n=0;n<3;n++)assert.equal((await ticks(`cancel-retains-${owner}`)).action.consumed.fire,true,
        `Touch cancellation cleared ${owner} fire`);
      await hold(false);await ticks(`${owner}-all-fire-released`,s=>!s.action.consumed.fire);
    }
    if(fireMode==='held-key'){
      await gesture('fire','pointerdown');await ticks('held-touch-before-cancel',s=>s.action.consumed.fire);
      await io.cancel();await ticks('held-touch-cancel-released',s=>!s.action.consumed.fire);
      await gesture('fire','pointerdown');await ticks('held-touch-new-contact',s=>s.action.consumed.fire);
      await gesture('fire','pointerup');await ticks('held-touch-new-contact-released',s=>!s.action.consumed.fire);
    }
    return{...evidence,complete:false,scope:'Production touch controller DOM gestures and native focus/fire consumers | measured fixture geometry'};
  }catch(error){failure=error;error.evidence=evidence;throw error;}
  finally{
    const errors=[];
    for(const perform of [...['ArrowRight','ShiftLeft','KeyZ'].map(code=>()=>io.key(code,false)),
      ()=>io.gamepad(false),()=>io.cancel()]){
      try{await perform();}catch(error){errors.push(String(error));}
    }
    if(errors.length){
      if(failure)failure.cleanupErrors=errors;
      else throw Error(`Launcher input cleanup failed: ${errors.join(' | ')}`);
    }
  }
}

function sceneIdentity(contract){
  assert.equal(typeof contract?.game,'string','Launcher scene needs an explicit game');
  assert.ok(['normal','multiplayer'].includes(contract?.variant),'Launcher scene needs an explicit variant');
  return{game:contract.game,variant:contract.variant};
}

function requireConsumer(value,label,seat){
  assert.equal(value?.available,true,`${label}: native consumer observation is unavailable`);
  assert.equal(value.complete,true,`${label}: native consumer observation is incomplete`);
  assert.equal(value.phase,'consumer-run',`${label}: native consumer is not running`);
  assert.ok(Number.isSafeInteger(value.sequence),`${label}: native consumer sequence is absent`);
  if(value.localSeat!==undefined)assert.equal(value.localSeat,seat,`${label}: native consumer belongs to a different local seat`);
  return value;
}

/** Production Launcher mapper -> Runtime gamepad bridge -> native consumer.
 * IO methods must expose the production mapper's read-only sample journal and
 * pair it with exactly one manually advanced native consumer tick. */
export async function runLauncherGamepadMappingScene(io,{contract,timeoutTicks=8}={}){
  const identity=sceneIdentity(contract),seat=contract?.mappingSeat,nativeSeat=contract?.nativeSeat;
  assert.ok(Number.isInteger(seat)&&seat>=0&&seat<=1,'Gamepad mapping contract needs its mapping seat');
  assert.ok(Number.isInteger(nativeSeat)&&nativeSeat>=0&&nativeSeat<=1,'Gamepad mapping contract needs its native local seat');
  assert.ok(typeof contract?.nativeSeatSource==='string'&&contract.nativeSeatSource.length>0,
    'Gamepad mapping contract needs the registered local-seat source');
  assert.ok(contract?.actions?.shoot&&Number.isInteger(contract.actions.shoot.bits)&&contract.actions.shoot.bits>0,
    'Gamepad mapping contract needs the independently registered shoot action mask');
  assert.ok(contract?.actions?.right&&Number.isInteger(contract.actions.right.bits)&&contract.actions.right.bits>0,
    'Gamepad mapping contract needs the independently registered right action mask');
  for(const name of ['step','readActions','observe'])assert.equal(typeof io?.[name],'function',`Gamepad scene needs ${name}`);
  const map=io?.mapping;
  for(const name of ['refresh','snapshot','setEnabled','select','bind','setButton','setAxis','reorder','cursor','samplesSince','reload','neutralize'])
    assert.equal(typeof map?.[name],'function',`Gamepad scene needs mapping.${name}`);
  assert.ok(Number.isInteger(timeoutTicks)&&timeoutTicks>0&&timeoutTicks<=32);
  const evidence={schema:'eagler-touhou/launcher-gamepad-scenes/1',...identity,mappingSeat:seat,nativeSeat,
    identities:[],profileIsolation:null,operations:[],ticks:[],reloaded:false,complete:false};
  const sampleStep=async label=>{
    const before=requireConsumer(await io.readActions(),`${label} before`,nativeSeat),cursor=await map.cursor();
    await io.step(1);
    const after=requireConsumer(await io.readActions(),`${label} after`,nativeSeat);
    assert.equal(after.sequence,before.sequence+1,`${label}: native consumer skipped a tick`);
    const samples=(await map.samplesSince(cursor)).map(row=>({...row,nativeConsumerSequence:after.sequence}));
    assert.ok(Array.isArray(samples)&&samples.length>0,`${label}: production mapper sample journal did not cover the native tick`);
    const result={label,before,after,samples};evidence.ticks.push(result);return result;
  };
  const state=async(label)=>{
    const snapshot=await map.refresh();
    assert.ok(snapshot?.enabled===true,`${label}: Launcher gamepad mapping is disabled`);
    assert.ok(Array.isArray(snapshot.devices)&&snapshot.devices.length>=2,`${label}: two simulated connected devices are required`);
    const [first,second]=[...snapshot.devices].sort((a,b)=>a.index-b.index);
    assert.ok(first?.key&&second?.key&&first.key!==second.key,`${label}: devices need distinct persistent identities`);
    assert.ok(Number.isInteger(first.index)&&Number.isInteger(second.index)&&first.index!==second.index,
      `${label}: devices need distinct live Gamepad API indexes`);
    return{snapshot,first,second};
  };
  const isolateProfiles=async devices=>{
    const isolated=[];
    for(const device of devices){
      // Selecting materializes the actual standard/custom profile in the
      // production model snapshot. Preserve it before clearing any action so
      // the report shows which defaults could otherwise alias the test input.
      await map.select(seat,device.key);
      const before=await map.snapshot(),profile=before?.profiles?.[device.key];
      assert.ok(profile?.bindings&&typeof profile.bindings==='object',
        `Cannot inspect the production mapping profile for ${device.key}`);
      const originalBindings=structuredClone(profile.bindings),actions=Object.keys(originalBindings);
      assert.ok(actions.includes('shoot')&&actions.includes('right'),
        `The production profile for ${device.key} lacks the registered test actions`);
      for(const action of actions){
        assert.ok(Array.isArray(originalBindings[action]),`${device.key}: ${action} profile binding is malformed`);
        await map.bind(device.key,action,[]);
      }
      const after=await map.snapshot(),isolatedProfile=after?.profiles?.[device.key];
      assert.ok(isolatedProfile?.bindings&&typeof isolatedProfile.bindings==='object',
        `The isolated production profile for ${device.key} is unavailable`);
      const isolatedBindings=structuredClone(isolatedProfile.bindings);
      for(const action of actions)assert.deepEqual(isolatedBindings[action],[],
        `${device.key}: unrelated production ${action} binding remains active in the isolated scene`);
      isolated.push({key:device.key,index:device.index,standard:device.standard===true,
        originalBindings,clearedActions:actions,isolatedBindings});
    }
    return{source:'production GamepadMappingSnapshot after selecting each connected device',
      policy:'clear every declared action before binding only this scene\'s Shoot and Right controls',devices:isolated};
  };
  const rowFor=(tick,key,index,bits,label)=>{
    const rows=tick.samples.filter(row=>row?.seat===seat&&row?.selectedKey===key&&row?.selectedIndex===index&&row?.bits===bits);
    assert.ok(rows.length>0,`${label}: no production sample ties seat ${seat}, ${key}@${index}, and bits ${bits} to this native tick`);
    return rows.at(-1);
  };
  const assertControlSample=(row,key,control,active,label)=>{
    const source=row?.devices?.find(device=>device?.key===key);
    assert.ok(source,`${label}: same-tick gamepad sample has no raw source state for ${key}`);
    if(control.kind==='button'){
      const button=source.buttons?.find(value=>value?.index===control.index);
      assert.equal(!!button?.pressed,active,`${label}: button ${control.index} source state disagrees with its stimulus`);
      if(active)assert.ok(Number.isFinite(button?.value)&&button.value>=.5,`${label}: button ${control.index} has no active value`);
    }else{
      assert.ok(Array.isArray(source.axes)&&Number.isFinite(source.axes[control.index]),`${label}: axis ${control.index} source value is absent`);
      assert.equal(source.axes[control.index],active?control.value:control.rest,
        `${label}: axis ${control.index} source state disagrees with its stimulus`);
    }
    return source;
  };
  const nativeFire=(value,label)=>{
    assert.equal(typeof value?.consumed?.fire,'boolean',`${label}: native fire consumer state is absent`);
    return value.consumed.fire;
  };
  const applyControl=async(key,control,active,label)=>{
    const value=control.kind==='button'?!!active:active?control.value:control.rest;
    const receipt=control.kind==='button'?await map.setButton(key,control.index,value):await map.setAxis(key,control.index,value);
    const stimulus={label,key,control:{kind:control.kind,index:control.index},active,value,receipt:structuredClone(receipt??null)};
    (evidence.stimuli??=[]).push(stimulus);
    return receipt;
  };
  const associateStimulus=(label,tick)=>{
    let row;
    for(let i=(evidence.stimuli?.length??0)-1;i>=0;i--)if(evidence.stimuli[i].label===label){row=evidence.stimuli[i];break;}
    assert.ok(row,`${label}: production control stimulus receipt was not retained`);
    row.nativeConsumerSequence=tick.after.sequence;
    return row;
  };
  const neutralTick=async(label,key,index)=>{
    for(let n=0;n<timeoutTicks;n++){
      const tick=await sampleStep(`${label}-${n+1}`),row=tick.samples.find(item=>item?.seat===seat&&item?.selectedKey===key);
      if(row?.bits===0){rowFor(tick,key,index,0,label);return tick;}
    }
    assert.fail(`${label}: selected gamepad never produced a neutral sample`);
  };
  const waitFire=async(label,key,index,bits,expected)=>{
    for(let n=0;n<timeoutTicks;n++){
      const tick=await sampleStep(`${label}-${n+1}`),fire=nativeFire(tick.after,label);
      if(tick.samples.some(row=>row?.seat===seat&&row?.selectedKey===key&&row?.selectedIndex===index&&row?.bits===bits)&&fire===expected)
        return tick;
    }
    const recentTicks=evidence.ticks.slice(-timeoutTicks).map(tick=>{
      const rows=tick.samples.filter(row=>row?.seat===seat&&row?.selectedKey===key&&row?.selectedIndex===index);
      return{label:tick.label,sequence:tick.after.sequence,nativeFire:tick.after.consumed?.fire,
        mappedRows:rows.map(row=>({bits:row.bits,devices:row.devices??null}))};
    });
    const mappedSeen=recentTicks.some(tick=>tick.mappedRows.some(row=>row.bits===bits));
    const operationLabel=label.replace(/-native-(down|up)$/,'');
    const stimulusSuffix=expected?':selected-active':':selected-release';
    const activeStimulus=[...(evidence.stimuli??[])].reverse().find(item=>item.label===operationLabel+stimulusSuffix)??null;
    const diagnosis=mappedSeen&&!expected
      ?'The mapper was neutral but the native fire consumer remained active.'
      :mappedSeen
        ?'The selected mapper emitted the requested bits, but the native fire consumer stayed inactive.'
        :'The selected mapper did not emit the requested bits during the native tick window.';
    const error=new Error(`${label}: ${diagnosis}`);
    error.evidence={schema:'eagler-touhou/launcher-gamepad-failure/1',game:identity.game,variant:identity.variant,
      label,seat,key,index,expectedBits:bits,expectedNativeFire:expected,stimulus:activeStimulus,recentTicks};
    throw error;
  };
  const unselectedRight=async({label,selected,other,control})=>{
    const before=await io.observe();
    assert.ok(Number.isFinite(before?.position?.x)&&Number.isFinite(before?.position?.y),`${label}: native position is unavailable`);
    const stimulusLabel=`${label}:nonselected-active`;
    await applyControl(other.key,control,true,stimulusLabel);
    const tick=await sampleStep(label);
    associateStimulus(stimulusLabel,tick);
    assertControlSample(rowFor(tick,selected.key,selected.index,0,label),other.key,control,true,label);
    assert.equal(nativeFire(tick.after,label),false,`${label}: an unselected controller reached native fire`);
    const after=await io.observe();
    assert.deepEqual(after.position,before.position,`${label}: an unselected controller moved the native player`);
    evidence.operations.push({kind:'unselected-axis',label,selectedKey:selected.key,otherKey:other.key,
      before:before.position,after:after.position,sequence:tick.after.sequence,samples:tick.samples});
    await applyControl(other.key,control,false,`${label}:nonselected-release`);
    await neutralTick(`${label}-release`,selected.key,selected.index);
  };
  const oneAction=async({label,key,index,action,control,bits})=>{
    await map.neutralize();await map.select(seat,key);await neutralTick(`${label}-mapping-release`,key,index);
    await map.bind(key,action,[control]);
    // Mapping edits suspend a held state until the selected source is sampled neutral.
    await neutralTick(`${label}-binding-release`,key,index);
    if(action==='shoot'){
      const stimulusLabel=`${label}:selected-active`;
      await applyControl(key,control,true,stimulusLabel);
      const held=await waitFire(`${label}-native-down`,key,index,bits,true);
      associateStimulus(stimulusLabel,held);
      assertControlSample(rowFor(held,key,index,bits,label),key,control,true,label);
      await applyControl(key,control,false,`${label}:selected-release`);
      const released=await waitFire(`${label}-native-up`,key,index,0,false);
      assertControlSample(rowFor(released,key,index,0,label),key,control,false,label);
      evidence.operations.push({kind:'button',label,key,index,seat,control:{kind:control.kind,index:control.index},held:held.after.sequence,released:released.after.sequence,
        sample:rowFor(held,key,index,bits,label)});
      return{held,released};
    }
    const before=await io.observe();
    assert.ok(Number.isFinite(before?.position?.x),`${label}: native position is unavailable for the axis action`);
    const stimulusLabel=`${label}:selected-active`;
    await applyControl(key,control,true,stimulusLabel);
    for(let n=0;n<timeoutTicks;n++){
      const tick=await sampleStep(`${label}-native-axis-${n+1}`),after=await io.observe();
      if(tick.samples.some(row=>row?.seat===seat&&row?.selectedKey===key&&row?.selectedIndex===index&&row?.bits===bits)&&
          Number.isFinite(after?.position?.x)&&after.position.x>before.position.x){
        associateStimulus(stimulusLabel,tick);
        assertControlSample(rowFor(tick,key,index,bits,label),key,control,true,label);
        evidence.operations.push({kind:'axis',label,key,index,seat,control:{kind:control.kind,index:control.index},before:before.position,after:after.position,
          sequence:tick.after.sequence,sample:rowFor(tick,key,index,bits,label)});
        await applyControl(key,control,false,`${label}:selected-release`);
        for(let n=0;n<timeoutTicks;n++){
          const tick=await sampleStep(`${label}-axis-release-${n+1}`);
          if(tick.samples.some(row=>row?.seat===seat&&row?.selectedKey===key&&row?.selectedIndex===index&&row?.bits===0))break;
        }
        return{before,after,tick};
      }
    }
    assert.fail(`${label}: selected axis did not move the native player right`);
  };
  let failure;
  try{
    const initial=await map.refresh();assert.ok(initial&&typeof initial==='object','Gamepad model snapshot is unavailable');
    await map.setEnabled(true);
    const {first,second}=await state('initial discovery');
    evidence.identities=[{key:first.key,index:first.index,name:first.name},{key:second.key,index:second.index,name:second.name}];
    const shoot=contract.actions.shoot,right=contract.actions.right;
    for(const item of [shoot,right]){
      assert.ok(item.first?.control&&item.second?.control,`The ${item.action??'action'} binding needs separate first/second device controls`);
      assert.ok(['button','axis'].includes(item.first.control.kind)&&['button','axis'].includes(item.second.control.kind));
    }
    evidence.profileIsolation=await isolateProfiles([first,second]);
    // Distinct controls on isolated profiles make the selected identity
    // observable without retaining unrelated standard-layout actions.
    await map.bind(first.key,'shoot',[shoot.first.control]);await map.bind(first.key,'right',[right.first.control]);
    await map.bind(second.key,'shoot',[shoot.second.control]);await map.bind(second.key,'right',[right.second.control]);
    await map.select(seat,first.key);await neutralTick('first-selected-neutral',first.key,first.index);

    const initialAction=requireConsumer(await io.readActions(),'baseline',nativeSeat);
    assert.equal(nativeFire(initialAction,'baseline'),false,'Gamepad scene must start from neutral native fire');
    const excludedStimulus='nonselected-device-button:nonselected-active';
    await applyControl(second.key,shoot.second.control,true,excludedStimulus);
    const excluded=await sampleStep('nonselected-device-button');
    associateStimulus(excludedStimulus,excluded);
    assertControlSample(rowFor(excluded,first.key,first.index,0,'nonselected-device-button'),second.key,shoot.second.control,true,'nonselected-device-button');
    assert.equal(nativeFire(excluded.after,'nonselected-device-button'),false,'The unselected controller reached the native fire consumer');
    await applyControl(second.key,shoot.second.control,false,'nonselected-device-button:nonselected-release');await neutralTick('nonselected-device-release',first.key,first.index);
    await unselectedRight({label:'nonselected-device-axis',selected:first,other:second,control:right.second.control});

    await oneAction({label:'first-device-shoot',key:first.key,index:first.index,action:'shoot',control:shoot.first.control,bits:shoot.bits});
    await oneAction({label:'first-device-right-axis',key:first.key,index:first.index,action:'right',control:right.first.control,bits:right.bits});

    await map.neutralize();await map.select(seat,second.key);await neutralTick('second-selected-neutral',second.key,second.index);
    const secondAction=await oneAction({label:'second-device-shoot',key:second.key,index:second.index,action:'shoot',control:shoot.second.control,bits:shoot.bits});
    const reverseStimulus='first-device-unselected-after-switch:nonselected-active';
    await applyControl(first.key,shoot.first.control,true,reverseStimulus);
    const reverseExcluded=await sampleStep('first-device-unselected-after-switch');
    associateStimulus(reverseStimulus,reverseExcluded);
    assertControlSample(rowFor(reverseExcluded,second.key,second.index,0,'first-device-unselected-after-switch'),first.key,shoot.first.control,true,
      'first-device-unselected-after-switch');
    assert.equal(nativeFire(reverseExcluded.after,'first-device-unselected-after-switch'),false,'A formerly selected controller reached the native consumer');
    await applyControl(first.key,shoot.first.control,false,'first-device-unselected-after-switch:nonselected-release');await neutralTick('first-device-release-after-switch',second.key,second.index);
    await unselectedRight({label:'first-device-unselected-axis-after-switch',selected:second,other:first,control:right.first.control});
    await oneAction({label:'second-device-right-axis',key:second.key,index:second.index,action:'right',control:right.second.control,bits:right.bits});

    await map.neutralize();const previousSelection=(await map.snapshot()).seats[seat];
    assert.equal(previousSelection,second.key,'The selected device identity changed before index reorder');
    const reordered=await map.reorder([{key:first.key,index:second.index},{key:second.key,index:first.index}]);
    const newSecond=reordered.devices.find(item=>item.key===second.key),newFirst=reordered.devices.find(item=>item.key===first.key);
    assert.ok(newSecond&&newFirst,`Reordering changed a persistent controller identity`);
    assert.notEqual(newSecond.index,second.index,'The second controller index did not change');
    assert.equal(reordered.seats[seat],second.key,'A Gamepad API index reorder changed the selected controller identity');
    evidence.operations.push({kind:'index-reorder',seat,selectedKey:second.key,before:second.index,after:newSecond.index,
      otherKey:first.key,otherBefore:first.index,otherAfter:newFirst.index});
    await neutralTick('reordered-selected-neutral',second.key,newSecond.index);
    await oneAction({label:'reordered-device-shoot',key:second.key,index:newSecond.index,action:'shoot',control:shoot.second.control,bits:shoot.bits});

    await map.neutralize();const persisted=await map.snapshot();
    const reloaded=await map.reload();
    assert.equal(reloaded?.enabled,true,'The gamepad mapping enabled state did not survive model reload');
    assert.equal(reloaded?.seats?.[seat],second.key,'The selected controller identity did not survive model reload');
    const afterReload=reloaded.devices.find(item=>item.key===second.key);
    assert.equal(afterReload?.index,newSecond.index,'The reloaded selected controller was not discovered at its current index');
    assert.deepEqual(reloaded.profiles[second.key]?.bindings?.shoot, persisted.profiles[second.key]?.bindings?.shoot,
      'The selected controller button mapping did not survive model reload');
    assert.deepEqual(reloaded.profiles[second.key]?.bindings?.right, persisted.profiles[second.key]?.bindings?.right,
      'The selected controller axis mapping did not survive model reload');
    evidence.reloaded=true;evidence.persisted={selectedKey:reloaded.seats[seat],index:afterReload.index,
      shoot:reloaded.profiles[second.key].bindings.shoot,right:reloaded.profiles[second.key].bindings.right};
    await neutralTick('reloaded-selected-neutral',second.key,afterReload.index);
    await oneAction({label:'reloaded-device-shoot',key:second.key,index:afterReload.index,action:'shoot',control:shoot.second.control,bits:shoot.bits});
    evidence.complete=true;evidence.scope='Production Launcher gamepad mapping and Runtime input bridge | two controlled device identities | normal native local seat';
    return evidence;
  } catch(error) {
    failure=error;
    throw error;
  } finally {
    try{await map.neutralize();}
    catch(error){
      if(failure){failure.cleanupErrors=[...(failure.cleanupErrors??[]),String(error)];}
      else throw error;
    }
  }
}

/** Exercise the production layout model and the DOM layout applicator with
 * independent portrait and landscape profiles and a reload from saved storage. */
export async function runLauncherTouchLayoutScene(io,{contract}={}){
  const identity=sceneIdentity(contract),control=contract?.control;
  assert.equal(typeof control,'string','Touch layout scene needs a declared control');
  const placements=contract?.profiles;
  for(const orientation of ['landscape','portrait']){
    const item=placements?.[orientation]?.[control];
    assert.ok(item&&Number.isFinite(item.x)&&Number.isFinite(item.y)&&Number.isFinite(item.scale),`${orientation}: independent control placement is absent`);
    assert.ok(item.x>0&&item.x<1&&item.y>0&&item.y<1&&item.scale>0,'Touch layout test placements must be interior and positive');
  }
  const layout=io?.layout;
  for(const name of ['setViewport','begin','updateControl','save','reload','apply','storageSnapshot'])
    assert.equal(typeof layout?.[name],'function',`Touch layout scene needs layout.${name}`);
  assert.equal(typeof io.readActions,'function','Touch layout scene needs the native action observer');
  const evidence={schema:'eagler-touhou/launcher-touch-layout-scenes/1',...identity,control,orientations:[],complete:false};
  const before=await io.readActions();
  assert.equal(before?.available,true,'Touch layout scene has no native consumer snapshot');
  assert.equal(before.complete,true,'Touch layout scene has an incomplete native consumer snapshot');
  assert.equal(before.phase,'consumer-run','Touch layout scene requires a running native consumer');
  assert.ok(Number.isSafeInteger(before.sequence),'Touch layout scene has no native consumer sequence');
  assert.equal(typeof before?.consumed?.fire,'boolean','Touch layout scene has no native fire state');
  assert.equal(typeof before?.consumed?.focus,'boolean','Touch layout scene has no native focus state');
  const beforeKey={sequence:before.sequence,fire:before.consumed?.fire,focus:before.consumed?.focus};
  for(const orientation of ['landscape','portrait']){
    const requestedViewport=contract?.viewports?.[orientation];
    assert.ok(Number.isInteger(requestedViewport?.width)&&Number.isInteger(requestedViewport?.height)&&requestedViewport.width>0&&requestedViewport.height>0,
      `${orientation}: provide a real browser viewport size`);
    assert.equal(orientation==='landscape'?requestedViewport.width>requestedViewport.height:requestedViewport.height>requestedViewport.width,
      true,`${orientation}: viewport aspect does not match its label`);
    const viewport=await layout.setViewport(requestedViewport);
    assert.deepEqual(viewport,{width:requestedViewport.width,height:requestedViewport.height},`${orientation}: browser viewport did not reach the requested orientation`);
    const tree=await layout.begin(orientation);
    assert.equal(tree?.target,'player',`${orientation}: layout capture did not use the launched Player surface`);
    assert.ok(Array.isArray(tree?.controls)&&tree.controls.length>0,`${orientation}: layout capture did not expose the mounted control tree`);
    for(const controlNode of tree.controls){
      assert.ok(controlNode?.name&&controlNode?.id&&controlNode.mounted===true&&controlNode.owner==='player',
        `${orientation}: layout control ${controlNode?.name??'<unknown>'} is missing from the launched Player surface`);
    }
    const placement=placements[orientation][control];
    await layout.updateControl(control,placement);
    const saved=await layout.save();
    assert.equal(saved?.persisted,true,`${orientation}: production layout did not persist to storage`);
    assert.ok(saved.value?.profiles?.[orientation],`${orientation}: saved production profile is absent`);
    const stored=await layout.storageSnapshot();
    assert.equal(stored?.key,contract.storageKey,`${orientation}: layout was stored under a different owner key`);
    assert.deepEqual(stored?.value?.profiles?.[orientation]?.controls?.[control],placement,
      `${orientation}: persisted placement differs from the independent editor input`);
    for(const previous of evidence.orientations){
      const priorPlacement=placements[previous.orientation][control];
      assert.deepEqual(stored?.value?.profiles?.[previous.orientation]?.controls?.[control],priorPlacement,
        `${orientation}: saving this profile overwrote the previously saved ${previous.orientation} layout`);
    }
    evidence.orientations.push({orientation,tree,stored});
  }
  await layout.reload();
  for(const orientation of ['landscape','portrait']){
    const viewport=await layout.setViewport(contract.viewports[orientation]);
    assert.deepEqual(viewport,contract.viewports[orientation],`${orientation}: restored profile uses the wrong viewport orientation`);
    const applied=await layout.apply(orientation,control),expected=placements[orientation][control];
    assert.equal(applied?.available,true,`${orientation}: saved layout could not be applied to the actual control DOM`);
    assert.equal(applied.target,'player',`${orientation}: layout target is not the launched Player surface`);
    assert.equal(applied.orientation,orientation,`${orientation}: editor applied the other orientation profile`);
    assert.equal(applied.storageKey,contract.storageKey,`${orientation}: applied layout came from a different storage owner`);
    assert.deepEqual(applied.placement,expected,`${orientation}: restored profile does not match the saved editor data`);
    const safe=applied.safeRect,rect=applied.controlRect;
    assert.ok(safe?.width>0&&safe?.height>0&&rect?.width>0&&rect?.height>0,`${orientation}: actual layout geometry is unavailable`);
    const actualX=(rect.left+rect.width/2-safe.left)/safe.width,actualY=(rect.top+rect.height/2-safe.top)/safe.height;
    const oneCssPixel=1/Math.min(safe.width,safe.height);
    assert.ok(Math.abs(actualX-expected.x)<=oneCssPixel&&Math.abs(actualY-expected.y)<=oneCssPixel,
      `${orientation}: actual control rectangle does not match the saved normalized position`);
    assert.ok(Number.isFinite(applied.renderedScale)&&Math.abs(applied.renderedScale-expected.scale)<=1e-6,
      `${orientation}: actual control scaling differs from the saved layout`);
    evidence.orientations.find(row=>row.orientation===orientation).applied={...applied,actual:{x:actualX,y:actualY}};
  }
  const after=await io.readActions();
  assert.equal(after?.available,true,'Touch layout scene lost the native consumer observation');
  assert.equal(after?.consumed?.fire,beforeKey.fire,'Editing and applying layout changed native fire input');
  assert.equal(after?.consumed?.focus,beforeKey.focus,'Editing and applying layout changed native focus input');
  assert.equal(after.sequence,beforeKey.sequence,'Layout editing advanced or replaced the paused native consumer');
  evidence.complete=true;evidence.scope='Production touch layout storage/model and applied control geometry | orientation-specific profiles';
  return evidence;
}

/** Compare actual Runtime touch targets from the same direct-touch gesture
 * before and after the production game zoom transform. */
export async function runLauncherTouchMagnifierScene(io,{contract}={}){
  const identity=sceneIdentity(contract),m=io?.magnifier;
  for(const name of ['snapshot','frameRect','setAvailable','pinch','pointerCursor','pointerEventsSince','pointerAt','reset','cancel'])
    assert.equal(typeof m?.[name],'function',`Magnifier scene needs magnifier.${name}`);
  for(const name of ['step','readNativeTarget'])assert.equal(typeof io?.[name],'function',`Magnifier scene needs ${name}`);
  const start=contract?.gesture?.start,end=contract?.gesture?.end,pinch=contract?.pinch;
  for(const point of [start,end])assert.ok(point&&Number.isFinite(point.x)&&Number.isFinite(point.y)&&point.x>0&&point.x<1&&point.y>0&&point.y<1,
    'Magnifier scene needs interior normalized gesture points');
  assert.ok(Array.isArray(pinch?.pointerIds)&&pinch.pointerIds.length===2&&pinch.pointerIds.every(Number.isSafeInteger)&&
    new Set(pinch.pointerIds).size===2,'Magnifier scene needs two distinct browser touch identities');
  for(const side of ['start','end'])assert.ok(Array.isArray(pinch?.[side])&&pinch[side].length===2&&pinch[side].every(point=>
    point&&Number.isFinite(point.x)&&Number.isFinite(point.y)&&point.x>0&&point.x<1&&point.y>0&&point.y<1),
    `Magnifier pinch needs two interior ${side} points`);
  const evidence={schema:'eagler-touhou/launcher-touch-magnifier-scenes/1',...identity,gestures:[],complete:false};
  const readTarget=async(label)=>{
    const value=await io.readNativeTarget();
    assert.equal(value?.available,true,`${label}: native touch target trace is unavailable`);
    assert.equal(value.complete,true,`${label}: native touch target trace is incomplete`);
    assert.equal(value.source,contract.traceSource,`${label}: target did not come from the registered native touch trace`);
    assert.ok(Number.isSafeInteger(value.sequence),`${label}: native touch sequence is absent`);
    assert.ok(Number.isFinite(value.target?.x)&&Number.isFinite(value.target?.y),`${label}: native touch target is absent`);
    return value;
  };
  const cssPoint=(rect,point)=>({clientX:rect.left+rect.width*point.x,clientY:rect.top+rect.height*point.y});
  const gesture=async(label)=>{
    const rect=await m.frameRect();
    assert.ok(Number.isFinite(rect?.left)&&Number.isFinite(rect?.top)&&rect.width>0&&rect.height>0,
      `${label}: displayed Runtime frame geometry is unavailable`);
    const cursor=await m.pointerCursor(),down=cssPoint(rect,start),move=cssPoint(rect,end),before=await readTarget(`${label} before`);
    const requestedPointerId=Number.isSafeInteger(contract.pointerId)?contract.pointerId:19471;
    const downReceipt=await m.pointerAt({type:'pointerdown',pointerId:requestedPointerId,...down});
    const pointerId=Number.isSafeInteger(downReceipt?.pointerId)?downReceipt.pointerId:requestedPointerId;
    await io.step(1);
    const armed=await readTarget(`${label} armed`);
    assert.equal(armed.sequence,before.sequence+1,`${label}: native consumer did not advance once after Pointer down`);
    const moveReceipt=await m.pointerAt({type:'pointermove',pointerId:requestedPointerId,...move});
    assert.equal(moveReceipt?.pointerId??pointerId,pointerId,`${label}: browser changed the active touch Pointer identity`);
    await io.step(1);
    const moved=await readTarget(`${label} moved`);
    assert.equal(moved.sequence,armed.sequence+1,`${label}: native consumer did not advance once after Pointer move`);
    const upReceipt=await m.pointerAt({type:'pointerup',pointerId:requestedPointerId,...move});
    assert.equal(upReceipt?.pointerId??pointerId,pointerId,`${label}: browser ended a different touch Pointer identity`);
    await io.step(1);
    const pointerReceipt=await m.pointerEventsSince(cursor),events=Array.isArray(pointerReceipt)?pointerReceipt:pointerReceipt?.events;
    assert.ok(Array.isArray(events),`${label}: browser Pointer event journal is unavailable`);
    assert.equal(pointerReceipt?.drops??0,0,`${label}: browser Pointer event journal overflowed`);
    for(const type of ['pointerdown','pointermove','pointerup']){
      const event=events.find(row=>row?.type===type&&row.pointerId===pointerId);
      assert.ok(event,`${label}: browser did not deliver the ${type} Pointer`);
      assert.equal(event.isTrusted,true,`${label}: Pointer input must come from the browser input path`);
      assert.equal(event.pointerType,'touch',`${label}: Pointer input was not a touch contact`);
      assert.equal(event.surfaceOwner,'player',`${label}: trusted ${type} did not reach a touch surface inside the launched Player`);
      assert.equal(event.surfaceTarget,'touchDirectSurface',`${label}: trusted ${type} did not target the production Player direct-touch surface`);
    }
    const delta={x:moved.target.x-armed.target.x,y:moved.target.y-armed.target.y};
    assert.ok(Number.isFinite(delta.x)&&Number.isFinite(delta.y),`${label}: native target delta is invalid`);
    assert.ok(delta.x!==0||delta.y!==0,`${label}: trusted screen Pointer did not move the native Runtime touch target`);
    const moveEvent=events.find(row=>row?.type==='pointermove'&&row.pointerId===pointerId);
    assert.equal(moveEvent.clientX,move.clientX,`${label}: Runtime did not receive the requested absolute screen x coordinate`);
    assert.equal(moveEvent.clientY,move.clientY,`${label}: Runtime did not receive the requested absolute screen y coordinate`);
    evidence.gestures.push({label,rect,screen:{down,move},native:{before,armed,moved},delta,events});
    return evidence.gestures.at(-1);
  };
  let failure;
  try{
    await m.setAvailable(false);await m.reset();await m.cancel();
    const plain=await gesture('magnifier-off');
    await m.setAvailable(true);
    const beforePinchRect=await m.frameRect(),pinchCursor=await m.pointerCursor();
    const pinchPoint=(point)=>cssPoint(beforePinchRect,point);
    const pinchScreen={start:pinch.start.map(pinchPoint),end:pinch.end.map(pinchPoint),pointerIds:[...pinch.pointerIds]};
    await m.pinch(pinchScreen);
    const pinchReceipt=await m.pointerEventsSince(pinchCursor),pinchEvents=Array.isArray(pinchReceipt)?pinchReceipt:pinchReceipt?.events;
    assert.ok(Array.isArray(pinchEvents),`Magnifier pinch: browser Pointer journal is unavailable`);
    assert.equal(pinchReceipt?.drops??0,0,'Magnifier pinch: browser Pointer journal overflowed');
    for(const type of ['pointerdown','pointermove','pointerup']){
      const rows=pinchEvents.filter(row=>row?.type===type&&row.pointerType==='touch');
      assert.ok(new Set(rows.map(row=>row.pointerId)).size>=2,`Magnifier pinch: two touch ${type} events were not observed`);
      for(const row of rows){
        assert.equal(row.isTrusted,true,'Magnifier pinch input must come from the browser touch path');
        assert.equal(row.surfaceOwner,'player',`Magnifier pinch ${type} did not reach a touch surface inside the launched Player`);
        assert.equal(row.surfaceTarget,'touchDirectSurface',`Magnifier pinch ${type} did not target the production Player direct-touch surface`);
      }
    }
    const zoom=await m.snapshot(),zoomRect=await m.frameRect();
    assert.equal(zoom?.active,true,'Production magnifier availability did not activate');
    assert.equal(zoom?.target,'player','Magnifier transform is not attached to the launched Player surface');
    assert.ok(Number.isFinite(zoom?.scale)&&zoom.scale>1,'Two-finger browser touch did not activate production zoom');
    assert.ok(zoomRect.width!==plain.rect.width||zoomRect.height!==plain.rect.height||
      zoomRect.left!==plain.rect.left||zoomRect.top!==plain.rect.top,
      'The magnifier did not change the actual displayed Runtime frame geometry');
    await m.cancel();
    const magnified=await gesture('magnifier-on');
    assert.ok(plain.screen.move.clientX!==magnified.screen.move.clientX||plain.screen.move.clientY!==magnified.screen.move.clientY,
      'The same displayed game target did not produce different absolute screen coordinates after zoom');
    assert.equal(Math.fround(plain.delta.x),Math.fround(magnified.delta.x),
      'The same screen-relative Pointer gesture mapped to a different native x target after zoom');
    assert.equal(Math.fround(plain.delta.y),Math.fround(magnified.delta.y),
      'The same screen-relative Pointer gesture mapped to a different native y target after zoom');
    evidence.comparison={pinch:{screen:pinchScreen,events:pinchEvents,zoom},plainDelta:plain.delta,magnifiedDelta:magnified.delta,
      plainScreen:plain.screen,magnifiedScreen:magnified.screen,zoom};
    await m.cancel();await m.reset();
    assert.equal((await m.snapshot()).scale,1,'Magnifier reset did not restore the original Runtime scale');
    evidence.complete=true;evidence.scope='Trusted browser touch Pointers | production direct-touch controller and zoom transform | native Runtime touch-target trace';
    return evidence;
  } catch(error) {
    failure=error;
    throw error;
  } finally {
    try{await m.cancel();}
    catch(error){
      if(failure){failure.cleanupErrors=[...(failure.cleanupErrors??[]),String(error)];}
      else throw error;
    }
  }
}
