import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {resolveTitleFlow} from '../../tests/adaptation/adapters/title-flow.mjs';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');

export async function changeNativeConfiguration(contract,{before,observe,press,waitUntil}={}){
  const {menu,configuration:c}=contract;
  const matches=(state,expected)=>Object.entries(expected).every(([key,value])=>state[key]===value);
  let state=await waitUntil('native title before configuration',s=>matches(s,menu.main));
  for(let count=0;count<16&&state.cursor!==menu.optionsCursor;count++){await press('ArrowDown');state=await observe();}
  assert.equal(state.cursor,menu.optionsCursor,'Native Options entry was not selected');
  await press('KeyZ');
  state=await waitUntil('native Options input ready',s=>s.page===menu.optionsPage&&s.phase===menu.main.phase);
  for(let count=0;count<16&&state.cursor!==menu.soundCursor;count++){await press('ArrowDown');state=await observe();}
  assert.equal(state.cursor,menu.soundCursor,'Native sound-volume option was not selected');
  const value=before.files.find(file=>file.path===c.path)?.inspection.values[c.field];
  assert.ok(Number.isFinite(value)&&value>=c.minimum&&value<=c.maximum,'Configuration baseline is outside the declared menu probe range');
  const delta=value>=c.step?-c.step:c.step;
  await press(delta<0?'ArrowLeft':'ArrowRight');
  for(let count=0;count<3;count++){
    await press('Escape');if(matches(await observe(),menu.main))break;
  }
  await waitUntil('native title after configuration',s=>matches(s,menu.main));
  return{path:c.path,field:c.field,before:value,expected:value+delta,source:menu.source};
}

/** Read title-declared native menu fields without writing the Runtime. */
export function readNativeStorageMenu({runtime,menu}){
  const mounted=globalThis[runtime.global],core=mounted?.[runtime.core],app=mounted?.[runtime.app];
  const read=core?.[menu.readExport];
  if(typeof read!=='function'||!app)throw Error('Native storage menu observation is unavailable');
  return Object.fromEntries(Object.entries(menu.fields).map(([name,index])=>[name,read(app,index)]));
}

/** Copy the title's existing selection observation and flags from its active Runtime. */
export function readNativeStorageSelection({runtime,selection,menu}){
  const mounted=globalThis[runtime.global],core=mounted?.[runtime.core],module=mounted?.[runtime.module],app=mounted?.[runtime.app];
  const read=core?.[selection.readExport],status=core?.[menu.readExport];
  if(typeof read!=='function'||typeof status!=='function'||!app)throw Error('Native storage selection observation is unavailable');
  const pointer=read(app),buffer=core.memory?.buffer??module?.HEAPU8?.buffer;
  if(core.memory?.buffer&&module?.HEAPU8?.buffer&&core.memory.buffer!==module.HEAPU8.buffer)throw Error('Native storage selection memory owners differ');
  if(!buffer||!Number.isSafeInteger(pointer)||pointer<=0||pointer%4||!Number.isSafeInteger(selection.wordCount)||selection.wordCount<1||
    pointer+selection.wordCount*4>buffer.byteLength)throw Error('Native storage selection memory range is invalid');
  const rawWords=Array.from(new Int32Array(buffer,pointer,selection.wordCount));
  const values={};for(const [name,index] of Object.entries(selection.fields)){
    if(!Number.isSafeInteger(index)||index<0||index>=rawWords.length)throw Error('Native storage selection field is outside the observation');
    values[name]=rawWords[index];
  }
  return{...values,gameFlags:status(app,selection.gameFlagsField),rawWords};
}

export function assertNativeConfigurationAuthored(contract,change,snapshot){
  const file=snapshot.files.find(file=>file.path===change.path);
  assert.equal(file?.inspection.values[change.field],change.expected,'Native Options change was not saved');
  assert.notEqual(change.before,change.expected,'Native Options probe did not change its baseline');
  assert.equal(change.path,contract.configuration.path);
  return{status:'passed',path:change.path,field:change.field,before:change.before,after:change.expected};
}

export async function captureNativeUserData(contract,{read,saveBytes,stage,includeContents=false}={}){
  const files=[];
  for(const file of contract.files){
    const bytes=Buffer.from(await read(file.path)),inspection=file.inspect(bytes);
    const evidencePath=await saveBytes(file.path,bytes);
    files.push({path:file.path,kind:file.kind,bytes:bytes.length,sha256:hash(bytes),inspection,evidencePath,
      ...(includeContents?{contentBase64:bytes.toString('base64')}:{})});
  }
  return{stage,files};
}

export async function assertNativeUserDataRestored(contract,before,after,{exactBytes=true,readEvidence}={}){
  assert.equal(before.files.length,contract.files.length);assert.equal(after.files.length,contract.files.length);
  for(const file of contract.files){
    const expected=before.files.find(item=>item.path===file.path),actual=after.files.find(item=>item.path===file.path);
    assert.ok(expected&&actual,`Missing native ${file.kind} at ${file.path}`);
    const left=Buffer.from(await readEvidence(expected.evidencePath)),right=Buffer.from(await readEvidence(actual.evidencePath));
    assert.equal(left.length,expected.bytes);assert.equal(right.length,actual.bytes);
    assert.equal(hash(left),expected.sha256);assert.equal(hash(right),actual.sha256);
    const expectedValue=file.inspect(left),actualValue=file.inspect(right);
    assert.equal(actualValue.semanticSha256,expectedValue.semanticSha256,`${file.path}: native saved data changed`);
    if(exactBytes)assert.equal(actual.sha256,expected.sha256,`${file.path}: persisted bytes differ`);
  }
  return{status:'passed',exactBytes,paths:contract.files.map(file=>file.path)};
}

/** Recompute persisted-file evidence from retained bytes, not producer pass flags. */
export function assessNativeUserDataJourney(contract,journey,id){
  assert.equal(journey?.schema,'eagler-touhou/native-storage-journey/1');
  assert.equal(journey.source,contract.source);
  const inspect=snapshot=>{
    assert.equal(snapshot?.files?.length,contract.files.length,'Native file class is missing');
    const files=new Map();
    for(const declaration of contract.files){
      const matches=snapshot.files.filter(file=>file.path===declaration.path);assert.equal(matches.length,1);
      const file=matches[0];assert.equal(file.kind,declaration.kind);
      assert.ok(typeof file.contentBase64==='string'&&file.evidencePath,'Native file bytes or retained artifact are missing');
      const bytes=Buffer.from(file.contentBase64,'base64');assert.equal(bytes.toString('base64'),file.contentBase64);
      assert.equal(bytes.length,file.bytes);assert.equal(hash(bytes),file.sha256);
      const value=declaration.inspect(bytes);assert.deepEqual(value,file.inspection);files.set(file.path,{...file,inspection:value});
    }return files;
  };
  const before=inspect(journey.before),authored=inspect(journey.authored),reopened=inspect(journey.reopened);
  const life=journey.lifecycle;
  assert.equal(life?.exitAbsentBeforeStop,true,'The Runtime had already exited before the stop action');
  assert.ok(life.stopError===null||life.stopRetiredFrame===true&&/Frame was detached|Execution context was destroyed|Target page.*closed/i.test(life.stopError),'Production stop failed');
  assert.ok(Number.isSafeInteger(life?.beforeEpoch)&&life.beforeEpoch>0&&Number.isSafeInteger(life.afterEpoch)&&life.afterEpoch>life.beforeEpoch);
  assert.equal(life.exit?.event,'exit');assert.equal(life.exit.epoch,life.beforeEpoch);assert.equal(life.first?.event,'first-frame');assert.equal(life.first.epoch,life.afterEpoch);
  assert.equal(life.exit.code,0,'Production stop did not exit successfully');
  assert.equal(life.exit.status,'success','Production stop did not report success');
  const c=contract.configuration,change=journey.configuration;
  assert.equal(change.path,c.path);assert.equal(change.field,c.field);
  assert.equal(change.before,before.get(c.path).inspection.values[c.field]);
  assert.equal(change.after,reopened.get(c.path).inspection.values[c.field]);
  assert.ok(change.after>=c.minimum&&change.after<=c.maximum&&Math.abs(change.after-change.before)===c.step,'Native configuration change is absent');
  const score=contract.files.find(file=>file.kind==='score').path;
  assert.equal(authored.get(score).inspection.semanticSha256,reopened.get(score).inspection.semanticSha256,'Shutdown changed native score semantics');
  const left=before.get(score).inspection.values.statistics,right=reopened.get(score).inspection.values.statistics;
  assert.equal(right[6].total,Math.min(999999,left[6].total+1),'Native ordinary game count was not persisted');
  const changedRows=left.slice(0,5).map((row,index)=>right[index].total-row.total);
  assert.equal(changedRows.filter(delta=>delta===1).length,1,'Native difficulty game count was not persisted');
  assert.ok(changedRows.every(delta=>delta===0||delta===1));
  const selection=journey.selection;
  assert.ok(Number.isSafeInteger(selection?.difficulty)&&selection.difficulty>=0&&selection.difficulty<5&&
    Number.isSafeInteger(selection.character)&&selection.character>=0&&selection.character<12,'Actual native menu selection is missing');
  const menu=contract.menu,observations=journey.selectionObservations;
  assert.ok(observations?.difficulty&&observations.character,'Native menu selection observations are missing');
  for(const [name,page] of [['difficulty',menu.difficultyPage],['character',menu.characterPage]]){
    const observation=observations[name];
    assert.equal(observation.scene,menu.main.scene);assert.equal(observation.phase,menu.main.phase);
    assert.equal(observation.page,page,'Native selection was observed on another menu');
  }
  assert.equal(observations.difficulty.cursor,selection.difficulty);
  assert.equal(observations.character.cursor,menu.characterCursor);
  assert.equal(selection.character,menu.character);
  const entered=journey.entered,nativeSelection=journey.enteredSelection;
  assert.equal(entered?.schema,'eagler-touhou/runtime-observation/1','Native gameplay observation is missing');
  assert.equal(entered.variant,'normal');
  assert.equal(resolveTitleFlow({game:entered.game,variant:entered.variant}).isGameplayReady(entered),true,'Native gameplay is not ready');
  assert.equal(nativeSelection?.difficulty,selection.difficulty);assert.equal(nativeSelection.character,selection.character);
  assert.ok(Array.isArray(nativeSelection.rawWords)&&nativeSelection.rawWords.length===contract.selection.wordCount&&
    nativeSelection.rawWords.every(Number.isSafeInteger),'Native selection raw words are missing');
  for(const [name,index] of Object.entries(contract.selection.fields))assert.equal(nativeSelection[name],nativeSelection.rawWords[index]);
  for(const name of ['practice','replay','cheats','assisted'])assert.equal(nativeSelection[name],0,'Native game is not an ordinary unassisted run');
  const ordinaryFlags=contract.selection.ordinaryFlags;
  assert.ok(Number.isSafeInteger(ordinaryFlags?.mask)&&Number.isSafeInteger(ordinaryFlags.value)&&
    Number.isSafeInteger(nativeSelection.gameFlags)&&(nativeSelection.gameFlags&ordinaryFlags.mask)===ordinaryFlags.value,
  'Native game flags are not an active ordinary run');
  assert.equal(changedRows[selection.difficulty],1,'Score was saved under another difficulty');
  for(let difficulty=0;difficulty<7;difficulty++){
    const expected=structuredClone(left[difficulty]);
    if(difficulty===selection.difficulty||difficulty===6){expected.total=Math.min(999999,expected.total+1);
      expected.characters[selection.character]=Math.min(999999,expected.characters[selection.character]+1);}
    assert.deepEqual(right[difficulty],expected,'Native game statistics changed outside the selected difficulty and character');
  }
  const same=(base,next,exact=true)=>{for(const declaration of contract.files){const a=base.get(declaration.path),b=next.get(declaration.path);
    assert.equal(a.inspection.semanticSha256,b.inspection.semanticSha256);if(exact)assert.equal(a.sha256,b.sha256);}};
  const replay=journey.replay;
  assert.ok(replay?.sourceFixture?.sha256&&replay.bytes>0);assert.equal(replay.sha256,replay.sourceFixture.sha256);assert.equal(replay.bytes,replay.sourceFixture.bytes);
  const replayEqual=value=>{
    assert.equal(value?.bytes,replay.bytes);assert.equal(value?.sha256,replay.sha256);
    assert.equal(typeof value.contentBase64,'string','Retained Replay read bytes are missing');
    const bytes=Buffer.from(value.contentBase64,'base64');assert.equal(bytes.toString('base64'),value.contentBase64);
    assert.equal(bytes.length,value.bytes);assert.equal(hash(bytes),value.sha256);
  };
  replayEqual(replay);replayEqual(replay.sourceFixture);
  replayEqual(journey.replayRestored?.reopened);
  if(id==='SAVE-05'||id==='SAVE-06'){
    const offline=inspect(journey.offline),saved=inspect(journey.offlineSaved);
    assert.equal(journey.offline.pageOnline,false);same(reopened,offline);same(offline,saved,false);
    replayEqual(journey.replayRestored?.offline);replayEqual(journey.replayRestored?.offlineSaved);
  }
  if(id==='SAVE-06'){
    assert.ok(journey.generations?.before&&journey.generations?.after&&journey.generations.before!==journey.generations.after);
    same(reopened,inspect(journey.updated));replayEqual(journey.replayRestored?.updated);
  }
  return{status:'passed',paths:contract.files.map(file=>file.path)};
}
