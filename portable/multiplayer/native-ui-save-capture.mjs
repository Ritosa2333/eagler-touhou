import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';

// The wrapper links the actual Application menu/storage owners. The explicit
// score and terminal seams make these UI/I/O artifacts diagnostic, not a
// network run, full clear, or an independently playable input recording.
export async function captureNativeSaveMenus({page,shot,out,evidence}){
  const report=evidence.nativeSave={diagnostic:true,transport:false,fullPlaythrough:false,
    scope:'Native GameSession menu inputs and Application MP Replay scan/metadata/save owners, with controlled score/StageExit seams',
    diagnosticBuild:[0x55494658,0x54483131,2,1],timestamp:1791504000,cases:[]};
  const write=()=>writeFileSync(resolve(out,'native-save-evidence.json'),JSON.stringify(report,null,2)+'\n');
  const state=()=>page.evaluate(()=>window.state());
  await page.evaluate(()=>{
    window.nativeTap=(held=0,pause=0)=>{step(1);step(1,held,0,0,pause);step(1);};
    window.nativeFinishName=()=>{nativeTap(1);nativeTap(16);nativeTap(64);nativeTap(1);};
    window.nativeSaved=slot=>JSON.parse(str(core._mp_fixture_ui_saved(slot)));
  });
  const files=()=>page.evaluate(async()=>{
    const root='/save/replay';if(!core.FS.analyzePath(root).exists)return [];
    return Promise.all(core.FS.readdir(root).filter(name=>/^th11_.+\.rpy$/.test(name)).sort().map(async name=>{
      const bytes=core.FS.readFile(root+'/'+name),hash=await crypto.subtle.digest('SHA-256',bytes);
      return {name,bytes:bytes.length,sha256:Array.from(new Uint8Array(hash),n=>n.toString(16).padStart(2,'0')).join('')};
    }));
  });
  const saved=slot=>page.evaluate(slot=>nativeSaved(slot),slot);
  const snapshot=async(name,entry)=>shot(page,name,entry);
  const finishReturn=async(kind)=>{
    if(kind===2){
      await page.evaluate(()=>nativeTap(2));
    }else{
      await page.evaluate(()=>{step(12);nativeTap(2);nativeTap(0,1);nativeTap(1);});
    }
    for(let n=0;n<40&&(await state()).phase!=='finished';++n)await page.evaluate(()=>step(1));
    assert.equal((await state()).phase,'finished','native Return/Back must retire the session');
  };
  const begin=async({kind,scoreUnits,readOnly,backup=false})=>{
    await page.evaluate(({kind,scoreUnits,readOnly,backup})=>{
      if(!core._mp_fixture_ui_begin(kind===1?7:1,0))throw Error(err());
      // Record from native frame zero before the 125 neutral birth frames.
      if(!core._mp_fixture_ui_record_io(readOnly?1:0))throw Error('record_io');
      step(125);if(!core._mp_fixture_ui_result_score(scoreUnits))throw Error('score seam');
      if(backup&&!core._mp_fixture_ui_flush())throw Error(err());
      if(!core._mp_fixture_ui_terminal(kind))throw Error('terminal seam');
      if(kind===2)step(1);else step(30,1);
    },{kind,scoreUnits,readOnly,backup});
    if(kind===2){
      let ticks=0;
      while((await state()).phase==='ending'&&ticks<4000){
        ticks+=await page.evaluate(()=>{let n=0;for(;n<60&&state().phase==='ending';++n)step(1,512|(n&1?1:0));return n;});
      }
      assert.equal((await state()).phase,'game_over','original Ending/Staff must finish');
      await page.evaluate(()=>step(30));
    }
    const s=await state();assert.equal(s.recordIo,true);assert.equal(s.readOnly,readOnly);
    assert.equal(s.sharedScoreUnits,scoreUnits);
    assert.equal(s.displayedScoreUnits,scoreUnits,'prepared diagnostic score must be fully displayed before terminal capture');
    assert.equal(kind===2?s.titleSubstate:s.pauseState,kind===2?2:kind===1?25:18);
    assert.equal(kind===2?s.titleNameLength:s.pauseNameLength,0,'held entry Shot must not edit the native result name');
  };
  const enterSave=async({kind,slot,name,capture})=>{
    // First registration is shared score name A. This is the original native
    // character grid, driven only through confirmed P1 directional/Shot input.
    await page.evaluate(()=>nativeFinishName());
    if(kind===2){
      await page.evaluate(()=>step(18));
      const s=await state();assert.equal(s.titleScreen,15);assert.equal(s.titleSubstate,2);
    }else{
      await page.evaluate(()=>step(20)); // Let the original end-choice ANM finish its entrance.
      const s=await state();assert.equal(s.pauseState,kind===1?22:14);assert.equal(s.pauseCursor,kind===1?0:1);
      if(capture)await snapshot(name+'-actions','Original native Return/Replay Save after one shared score name; Continue/Retry removed');
      await page.evaluate(()=>{nativeTap(32);nativeTap(1);step(12);});
      assert.equal((await state()).pauseState,kind===1?23:16);
    }
    await page.evaluate(slot=>{for(let n=1;n<slot;++n)nativeTap(32);},slot);
    const list=await state();assert.equal(kind===2?list.titleCursor:list.pauseCursor,slot-1);
    if(capture)await snapshot(name+'-replay-slots','Original native Replay Save list, real MP files scanned; selected No.'+String(slot).padStart(2,'0'));
    await page.evaluate(()=>{nativeTap(1);step(12);});
    const naming=await state();assert.equal(kind===2?naming.titleSubstate:naming.pauseState,kind===2?3:kind===1?24:17);
    assert.equal(kind===2?naming.titleNameCursor:naming.pauseNameCursor,90,'native Replay name keeps the shared registered name A');
    if(capture)await snapshot(name+'-replay-name','Original native MP Replay name entry with real recording metadata');
    const before=await state();await page.evaluate(()=>nativeTap(1));
    const after=await state();assert.equal(kind===2?after.titleSubstate:after.pauseState,kind===2?2:kind===1?23:16);
    // nativeTap = neutral, confirming Shot, neutral. Save is after Append on
    // the confirming frame, one frame before the final release in this tap.
    return {confirmFrame:before.archiveFrames+2,after};
  };
  const validate=(header,{slot,scoreUnits,kind,confirmFrame})=>{
    assert.equal(header.valid,true,'saved bytes must pass the actual native ReplayArchive decoder');
    assert.equal(header.slot,slot);assert.equal(header.name.trimEnd(),'A');assert.equal(header.scoreUnits,scoreUnits);
    assert.equal(header.playerCount,3);assert.equal(header.recordedPlayer,0);assert.equal(header.completed,kind!==0);
    assert.equal(header.frames,confirmFrame,'save includes its confirmed native menu input');
    assert.deepEqual(header.diagnosticBuild,report.diagnosticBuild);
  };
  assert.deepEqual(await files(),[],'diagnostic save namespace starts empty');
  for(const entry of [
    {kind:0,name:'native-game-over',scoreUnits:1234567,slot:1,backup:true},
    {kind:1,name:'native-extra',scoreUnits:2345678,slot:3},
    {kind:2,name:'native-ending',scoreUnits:3456789,slot:4}
  ]){
    const before=await files();await begin({...entry,readOnly:false});
    if(entry.backup){const auto=await saved(1);assert.equal(auto.valid,true);assert.equal(auto.name,'PLAYER');assert.equal(auto.scoreUnits,entry.scoreUnits);assert.equal(auto.completed,false);}
    await snapshot(entry.name+'-ranking','Original one shared score '+(entry.kind===2?'Name Regist after real Ending/Staff':'Score Ranking')+'; diagnostic score='+entry.scoreUnits+' stored units');
    const result=await enterSave({...entry,capture:true}),header=await saved(entry.slot);validate(header,{...entry,confirmFrame:result.confirmFrame});
    const manual=await files();
    if(entry.backup){
      assert.equal(result.after.autoPath,'','manual naming replaces automatic ownership of the same slot');
      const named=manual.find(file=>file.name==='th11_01.rpy');
      await page.evaluate(()=>{step(3);if(!core._mp_fixture_ui_flush())throw Error(err());});
      const next=await files();assert.deepEqual(next.find(file=>file.name===named.name),named,'later automatic backup must preserve the named file byte-for-byte');
      const auto=await saved(2);assert.equal(auto.valid,true);assert.equal(auto.name,'PLAYER');assert.equal(auto.scoreUnits,entry.scoreUnits);
      assert.equal((await state()).autoPath,'/save/replay/th11_02.rpy');
      report.automaticNaming={sameSlotManualPreserved:true,namedFile:named,nextAutomaticFile:next.find(file=>file.name==='th11_02.rpy'),nextAutomaticHeader:auto};
    }else{
      for(const prior of before)assert.deepEqual(manual.find(file=>file.name===prior.name),prior,'unselected existing Replay must stay unchanged');
    }
    await finishReturn(entry.kind);
    report.cases.push({mode:'live',...entry,header,confirmFrame:result.confirmFrame,files:await files(),nativeReturn:true});write();
    const beforeReadOnly=await files();await begin({...entry,backup:false,readOnly:true});
    await enterSave({...entry,capture:false});
    assert.deepEqual(await files(),beforeReadOnly,'read-only native Replay Save must not write or overwrite any file');
    await page.evaluate(()=>{if(!core._mp_fixture_ui_flush())throw Error(err());});
    assert.deepEqual(await files(),beforeReadOnly,'read-only automatic backup must not write files');
    await finishReturn(entry.kind);
    report.cases.push({mode:'readOnly',kind:entry.kind,name:entry.name,scoreUnits:entry.scoreUnits,slot:entry.slot,filesUnchanged:true,files:beforeReadOnly,nativeReturn:true});write();
  }
  assert.equal(await page.evaluate(()=>core.FS.analyzePath('/save/scoreth11.dat').exists),false,'score/name registration remains memory-only');
  report.scoreFileAbsent=true;report.finalFiles=await files();
  const replayDir=resolve(out,'diagnostic-replays');mkdirSync(replayDir,{recursive:true});
  for(const file of report.finalFiles){
    const bytes=Uint8Array.from(await page.evaluate(name=>Array.from(core.FS.readFile('/save/replay/'+name)),file.name));
    assert.equal(createHash('sha256').update(bytes).digest('hex'),file.sha256);writeFileSync(resolve(replayDir,file.name),bytes);
  }
  report.passed=true;write();
}
