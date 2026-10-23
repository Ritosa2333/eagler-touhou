import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createReadStream} from 'node:fs';
import {readFile,stat} from 'node:fs/promises';
import {isAbsolute,join,resolve} from 'node:path';
import {PAIRED_CADENCE_DIFFERENCE_GROUPS} from '../../tests/adaptation/adapters/diagnostics.mjs';

export const DIAGNOSTIC_INPUT_SCHEMA='eagler-touhou/diagnostic-inputs/1';
const productKey=/^[a-z][a-z0-9-]{1,19}\/(?:normal|multiplayer)$/;

function object(value,label){
  assert.ok(value&&typeof value==='object'&&!Array.isArray(value),`${label} must be an object`);
}

/** Build a clean, explicit Lab environment so inherited snapshot selection cannot override this package. */
export function buildDiagnosticServerEnvironment(base,profile,product,port){
  object(base,'Base server environment');object(profile?.envNames,'Diagnostic profile environment names');
  object(product,'Selected diagnostic product');
  assert.ok(Number.isInteger(port)&&port>0&&port<=65535,'Lab server port must be a valid TCP port');
  const environment={...base,PORT:String(port)};
  for(const key of ['font','data','inputRoot']){
    const name=profile.envNames[key];
    assert.ok(typeof name==='string'&&name.length,`Diagnostic profile is missing the ${key} environment name`);
    assert.ok(typeof product[key]==='string'&&product[key].length,`Selected product is missing ${key}`);
    environment[name]=product[key];
  }
  const snapshotName=profile.envNames.snapshot;
  assert.ok(typeof snapshotName==='string'&&snapshotName.length,
    'Diagnostic profile must identify the inherited snapshot selector');
  for(const name of Object.keys(environment))if(name.toUpperCase()===snapshotName.toUpperCase())delete environment[name];
  return{environment,clearedSnapshotEnvironment:snapshotName};
}

function absolute(value,label){
  assert.ok(typeof value==='string'&&value.trim(),`${label} must be a non-empty absolute path`);
  assert.ok(isAbsolute(value),`${label} must be an absolute path`);
  return resolve(value);
}

function diagnosticProduct(value,key){
  object(value,`products[${key}]`);
  const allowed=['data','font','inputRoot','replayFiles'],required=['data','font','inputRoot'];
  for(const name of required)assert.ok(Object.hasOwn(value,name),`products[${key}] must declare ${name}`);
  assert.ok(Object.keys(value).every(name=>allowed.includes(name)),
    `products[${key}] may declare only font, data, inputRoot and replayFiles`);
  const replayFiles=value.replayFiles??{};
  object(replayFiles,`products[${key}].replayFiles`);
  for(const name of Object.keys(replayFiles))assert.match(name,/^[A-Za-z][A-Za-z0-9_-]*$/,
    `products[${key}].replayFiles has an invalid fixture key`);
  return Object.freeze({
    font:absolute(value.font,`products[${key}].font`),
    data:absolute(value.data,`products[${key}].data`),
    inputRoot:absolute(value.inputRoot,`products[${key}].inputRoot`),
    replayFiles:Object.freeze(Object.fromEntries(Object.entries(replayFiles).map(([name,path])=>
      [name,absolute(path,`products[${key}].replayFiles.${name}`)]))),
  });
}

/** Validate the private path manifest without reading game bytes or selecting another product implicitly. */
export function validateDiagnosticInputs(value,{game,variant}={}){
  object(value,'Diagnostic input manifest');
  assert.equal(value.schema,DIAGNOSTIC_INPUT_SCHEMA,'Unsupported diagnostic input schema');
  object(value.products,'Diagnostic products');
  const products={};
  for(const [gameId,modes] of Object.entries(value.products)){
    object(modes,`products.${gameId}`);
    for(const [mode,product] of Object.entries(modes)){
      assert.match(`${gameId}/${mode}`,productKey,'Diagnostic product key must be thNN/normal or thNN/multiplayer');
      products[`${gameId}/${mode}`]=diagnosticProduct(product,`${gameId}/${mode}`);
    }
  }
  assert.ok(typeof game==='string'&&game.trim(),'Select a game explicitly');
  assert.ok(['normal','multiplayer'].includes(variant),'Select normal or multiplayer explicitly');
  const selected=products[`${game}/${variant}`];
  assert.ok(selected,`Diagnostic input manifest is missing ${game}/${variant}`);
  return Object.freeze({schema:DIAGNOSTIC_INPUT_SCHEMA,products:Object.freeze(products),game,variant,product:selected});
}

export async function loadDiagnosticInputs(path,selection){
  const absoluteManifest=absolute(path,'--diagnostics');
  let value;
  try{value=JSON.parse(await readFile(absoluteManifest,'utf8'));}
  catch(error){throw new Error(`Could not read diagnostic input manifest ${absoluteManifest}: ${error.message}`);}
  return{path:absoluteManifest,...validateDiagnosticInputs(value,selection)};
}

async function fileIdentity(path,label){
  const before=await stat(path);
  assert.ok(before.isFile()&&before.size>0,`${label} must be a non-empty regular file: ${path}`);
  const hash=createHash('sha256');
  for await(const chunk of createReadStream(path))hash.update(chunk);
  const after=await stat(path);
  assert.equal(after.size,before.size,`${label} changed size while being read: ${path}`);
  assert.equal(after.mtimeMs,before.mtimeMs,`${label} changed while being read: ${path}`);
  return{path,bytes:after.size,sha256:hash.digest('hex')};
}

/** Bind only the local files consumed by the requested lanes and title profile. */
export async function inspectDiagnosticProduct(product,{quickInputs=[],dailyFixtures=[],lanes,pairedReplay=null}={}){
  object(product,'Selected diagnostic product');
  assert.ok(Array.isArray(lanes)&&lanes.length>0,'Select diagnostic lanes before inspecting local inputs');
  assert.ok(lanes.every(lane=>['presentation','quick','daily'].includes(lane)),'Unknown diagnostic lane');
  const rootInfo=await stat(product.inputRoot);
  assert.ok(rootInfo.isDirectory(),`inputRoot must be a directory: ${product.inputRoot}`);
  const files={
    font:await fileIdentity(product.font,'Font'),
    data:await fileIdentity(product.data,'DATA'),
  };
  if(pairedReplay){
    assert.ok(lanes.includes('presentation'),'Paired Replay requires the presentation lane');
    assert.equal(pairedReplay.schema,'eagler-presentation/replay-pair-capability/3','Unsupported paired Replay capability');
    assert.ok(typeof pairedReplay.fixture==='string'&&pairedReplay.fixture.trim(),'Paired Replay fixture identity is absent');
    assert.ok(typeof pairedReplay.inputFile==='string'&&/^[A-Za-z0-9._-]+\.rpy$/i.test(pairedReplay.inputFile),
      'Paired Replay input must be a plain Replay filename');
    assert.ok(Number.isSafeInteger(pairedReplay.ticks)&&pairedReplay.ticks>0&&pairedReplay.ticks<=3600,
      'Paired Replay requires a bounded logical window');
    assert.ok(Number.isSafeInteger(pairedReplay.expectedAuthorityGroups)&&pairedReplay.expectedAuthorityGroups>0,
      'Paired Replay requires the profile-declared authority-group count');
    assert.deepEqual(pairedReplay.cadencesHz,[60,120,144],
      'Paired Replay must declare the 60, 120 and 144 Hz callback rates');
    assert.ok(Array.isArray(pairedReplay.cadenceDifferenceGroups)&&
      pairedReplay.cadenceDifferenceGroups.length===PAIRED_CADENCE_DIFFERENCE_GROUPS.length&&
      PAIRED_CADENCE_DIFFERENCE_GROUPS.every(id=>pairedReplay.cadenceDifferenceGroups.includes(id))&&
      new Set(pairedReplay.cadenceDifferenceGroups).size===pairedReplay.cadenceDifferenceGroups.length,
      'Paired Replay must declare only the profile-supported cadence difference groups');
    files.pairedReplay={...pairedReplay,...await fileIdentity(join(product.inputRoot,pairedReplay.inputFile),'Paired Replay input')};
  }
  if(lanes.includes('quick')){
    assert.ok(Array.isArray(quickInputs)&&quickInputs.length>0,'Profile has no quick input fixtures');
    files.quickInputs=[];
    const names=new Set();
    for(const input of quickInputs){
      const name=typeof input==='string'?input:input?.file;
      assert.ok(typeof name==='string'&&name.length>0&&!name.includes('/')&&!name.includes('\\')&&name!=='.'&&name!=='..',
        'Quick input names must be plain filenames');
      assert.ok(!names.has(name),`Duplicate quick input filename ${name}`);names.add(name);
      assert.ok(typeof input==='string'||(typeof input.id==='string'&&Number.isSafeInteger(input.index)&&input.index>=0),
        'Quick input records require an id and zero-based index');
      files.quickInputs.push({...(typeof input==='string'?{}:{id:input.id,index:input.index}),
        ...await fileIdentity(join(product.inputRoot,name),'Quick Replay input')});
    }
  }
  if(lanes.includes('daily')){
    assert.ok(Array.isArray(dailyFixtures)&&dailyFixtures.length>0,'Profile has no daily Replay fixtures');
    files.dailyReplays={};
    for(const fixture of dailyFixtures){
      assert.ok(typeof fixture.replayKey==='string'&&fixture.replayKey,'Daily Replay fixture must identify its input key');
      const path=product.replayFiles?.[fixture.replayKey];
      assert.ok(path,`Diagnostic inputs are missing daily Replay ${fixture.replayKey}`);
      files.dailyReplays[fixture.replayKey]=await fileIdentity(path,`Daily Replay ${fixture.replayKey}`);
    }
  }
  return files;
}
