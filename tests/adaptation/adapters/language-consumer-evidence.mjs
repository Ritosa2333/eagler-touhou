import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {normalizeLanguageConsumerState} from '../../../tools/ports/language-consumer-state.mjs';
import {assessLanguageScene,assessMessageTextRasters} from '../../../tools/ports/language-contract.mjs';
import {languageSourceFnv,resolveLanguageTextSource} from '../../../tools/ports/language-text-sources.mjs';
import {decodeLanguageArchive} from '../../../tools/ports/language-inputs.mjs';
import {dialogueTouchSceneRequest,normalizeDialogueTouchManifest,normalizeDialogueTouchObservation} from './dialogue.mjs';
import {assertControlledSceneArmed,assertControlledSceneWorld,assertControlledSceneStimulus} from './scene.mjs';
import {matchesObservationFlowEnd} from './flow-steps.mjs';
import {isDeepStrictEqual} from 'node:util';

const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
function sourceBytes(receipt){
  assert.ok(Number.isSafeInteger(receipt.bytes)&&receipt.bytes>0&&receipt.bytes<=16*1024*1024);
  const bytes=readFileSync(receipt.path);assert.equal(bytes.length,receipt.bytes);assert.equal(hash(bytes),receipt.sha256);
  assert.equal(languageSourceFnv(bytes),receipt.fnv64);return bytes;
}
function assertImage(receipt,{driver,scene,name,identity}){
  assert.equal(resolve(receipt.path),resolve(driver.outputDirectory,`${scene.id}-${name}.png`));
  const bytes=readFileSync(receipt.path);assert.equal(bytes.length,receipt.bytes);assert.equal(hash(bytes),receipt.sha256);
  assert.equal(bytes.subarray(0,8).toString('hex'),'89504e470d0a1a0a');assert.equal(bytes.subarray(12,16).toString(),'IHDR');
  assert.ok(bytes.readUInt32BE(16)>0&&bytes.readUInt32BE(20)>0);assert.deepEqual(receipt.identity,identity);
}

/** Reopen source bytes and recompute both text consumers, native worlds and visual material identity. */
export function assertLanguageConsumerEvidence({driver,scene,adapter,expected,original}){
  const e=scene.consumers,profile=adapter.language;assert.equal(e.schema,'eagler-touhou/language-consumers/1');assert.deepEqual(e.cleanupErrors,[]);
  assert.equal(driver.sourceStable,true);assert.deepEqual(driver.sourceSnapshotEnd,driver.sourceSnapshotStart);assert.deepEqual(driver.teardownErrors,[]);
  assert.equal(e.identity.url,scene.runtime.url);assert.ok(Number.isFinite(e.identity.timeOrigin)&&Number.isSafeInteger(e.identity.app));
  assert.equal(scene.consumerSettings.touchEnabled,true);assert.equal(e.clock.mode,'native-manual');
  assert.equal(e.clockEnd?.mode,'native-manual');assert.equal(e.clockEnd.resumed,true);
  assert.equal(e.clockEnd.ticks,e.controlled.receipts.length,'Every manual tick needs a controlled-world receipt');
  assert.ok(e.navigation.length>0&&e.navigation.length<=512);
  for(const sample of e.navigation)assert.deepEqual(sample.state,normalizeLanguageConsumerState(sample.raw,profile));
  assert.ok(e.keyInputs.length>0);
  for(const key of e.keyInputs){
    assert.equal(key.mask,profile.navigation.masks[key.code]);assert.equal(key.inputField,profile.navigation.inputField);
    assert.deepEqual(key.transitionFields,profile.navigation.transitionFields);assert.equal(key.released,true);assert.equal(key.acknowledged,true);
    assert.ok(e.navigation.some(sample=>isDeepStrictEqual(sample.state,key.before)));assert.equal(key.before[key.inputField]&key.mask,0);
    assert.ok(key.samples.length>0&&key.samples.length<=256);
    for(const state of key.samples)assert.ok(e.navigation.some(sample=>isDeepStrictEqual(sample.state,state)));
    const state=key.samples.at(-1);assert.ok((state[key.inputField]&key.mask)!==0||key.transitionFields.some(field=>state[field]!==key.before[field]));
  }
  assert.equal(matchesObservationFlowEnd(profile.consumers.statistics.enter,e.statistics.state),true,'Native statistics scene was not observed');
  assert.ok(e.navigation.some(sample=>isDeepStrictEqual(sample.state,e.statistics.state)));
  assert.ok(e.navigation.some(sample=>matchesObservationFlowEnd(profile.consumers.statistics.exit,sample.state)),'Native return to title was not observed');
  const statistics=profile.consumers.statistics,baseline=Object.fromEntries(statistics.samples.map(sample=>[sample.key,sample.originalHex]));
  assert.equal(assessLanguageScene({trace:e.statistics.trace,profile:{samples:statistics.samples},expected,baseline,original}).status,'observed');
  assertImage(e.statistics.image,{driver,scene,name:'statistics',identity:e.identity});
  const sources=driver.originalTextSources;assert.equal(sources.schema,'eagler-touhou/language-text-sources/1');
  assert.equal(sources.game,driver.scope.game);assert.equal(sources.variant,driver.scope.variant);
  for(const key of ['bytes','sha256'])assert.equal(sources.archive[key],driver.inputs.data[key]);
  const tool=readFileSync(sources.tool.path);assert.equal(tool.length,sources.tool.bytes);assert.equal(hash(tool),sources.tool.sha256);
  assertControlledSceneArmed(e.arm);let previous=assertControlledSceneWorld(e.controlled.initial);
  for(const receipt of e.controlled.receipts){assertControlledSceneStimulus(receipt,previous,{kind:'dialogue'});previous=receipt;}
  assert.equal(previous.writeReceipt.committedBytes,0);
  assert.ok(e.settle.length>0&&e.settle.length<=profile.consumers.dialogue.maxReadyTicks);
  const ready=e.settle.at(-1);assert.equal(ready.words[0],1);assert.equal(ready.words[1],255);assert.equal(ready.words[2],1);
  assert.equal(ready.words[19],adapter.deathbomb.lifeStates.alive);assert.equal(ready.words[6],0);
  const {contract}=dialogueTouchSceneRequest({game:sources.game,variant:sources.variant});
  const manifest=normalizeDialogueTouchManifest(e.dialogueManifestRaw,contract);assert.equal(manifest.available,true);
  assert.equal(manifest.runtime.pointer,e.identity.app,'Loaded dialogue belongs to another native application');
  assert.deepEqual(e.dialogue.evidence.manifest.source,manifest.source);assert.deepEqual(e.dialogue.evidence.manifest.candidates,manifest.candidates);
  assert.equal(e.dialogue.checkStatus,'passed');const capture=e.dialogueCapture,state=normalizeDialogueTouchObservation(capture.raw,contract);
  assert.deepEqual(state,capture.state);assert.deepEqual(capture.identity,e.identity);assert.deepEqual(state.runtime,manifest.runtime);
  assert.equal(state.dialogue.present,true);assert.equal(state.dialogue.opcode,contract.waitOpcode);
  assert.equal(state.dialogue.pausedFrames>=profile.consumers.dialogue.captureWaitTicks,true);
  assert.ok(e.navigation.some(sample=>isDeepStrictEqual(sample.state,capture.ui)));
  assert.ok(Math.max(...profile.consumers.dialogue.textAlphaFields.map(field=>capture.ui[field]))>=profile.consumers.dialogue.minimumTextAlpha);
  assert.equal(state.fixture.used,true);assert.equal(state.fixture.gateEnabled,true);assert.equal(state.fixture.writeLocked,true);assert.equal(state.fixture.committedBytes,0);
  const selected=manifest.candidates.find(candidate=>candidate.entry===state.dialogue.message);assert.ok(selected);
  assert.equal(state.dialogue.instructionOffset,selected.message.waitOffset);assert.equal(selected.entry,e.dialogue.evidence.candidate.entry);
  let pack;
  if(!original){const input=driver.inputs.languages.packs[scene.language],bytes=readFileSync(input.path);
    assert.equal(bytes.length,input.bytes);assert.equal(hash(bytes),input.sha256);pack=decodeLanguageArchive(bytes,{game:sources.game,language:scene.language});}
  const resolved={};
  for(const key of ['ecl','message']){const asset=manifest.source[key].asset,input=resolveLanguageTextSource(sources,{language:scene.language,original,asset}),bytes=sourceBytes(input);
    const member=pack?.files.find(file=>file.path===profile.consumers.dialogue.mountRoot+asset);
    if(member){assert.equal(input.kind,'language-pack');assert.equal(input.member,member.path);assert.deepEqual(bytes,Buffer.from(member.bytes));}
    else {assert.equal(input.kind,'original-data');assert.deepEqual(input,sources.files[asset]);}
    assert.equal(input.bytes,manifest.source[key].byteLength);assert.equal(input.fnv64,manifest.source[key].fnv64);resolved[key]={input,bytes};}
  assert.deepEqual(e.dialogueSource.input,resolved.message.input);
  const parsed=adapter.parseDialogueText(resolved.message.bytes,selected.entry);
  assert.deepEqual(parsed,e.dialogueSource.expected);assert.ok(Number.isSafeInteger(e.beforeMessageTraceOrdinal)&&e.beforeMessageTraceOrdinal>0);
  const proof=assessMessageTextRasters({trace:capture.trace,expected:parsed,scene:profile.consumers.dialogue.gameScene,after:e.beforeMessageTraceOrdinal});
  assert.equal(proof.status,'observed');
  if(original)assert.ok(proof.rows.some(row=>[...row.text].some(character=>character.codePointAt(0)>127)),'Original dialogue proof needs actual Japanese glyphs');
  assertImage(capture.image,{driver,scene,name:'dialogue',identity:e.identity});
  return{statistics:statistics.samples.map(sample=>sample.key),dialogue:proof.rows.map(row=>row.text)};
}
