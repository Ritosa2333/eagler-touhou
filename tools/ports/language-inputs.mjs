import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {createRequire} from 'node:module';
import {isAbsolute,resolve} from 'node:path';
import {fileIdentity} from '../testing/dependency-snapshot.mjs';
import {validateStaticLanguagePackEntries} from '../../launcher/src/launcher/language-pack-validation.mts';

const {unzipSync}=createRequire(new URL('../../launcher/package.json',import.meta.url))('fflate');
export const LANGUAGE_INPUT_SCHEMA='eagler-touhou/language-inputs/1';
const languageId=/^lang_[a-z0-9]+(?:-[a-z0-9]+)*$/;
const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const object=(value,label)=>assert.ok(value&&typeof value==='object'&&!Array.isArray(value),`${label} must be an object`);
function localFile(value,label){
  assert.ok(typeof value==='string'&&isAbsolute(value),`${label} must be an absolute local file`);
  return resolve(value);
}

/** Only explicitly selected game/mode inputs are read. A normal pack never fills an MP input implicitly. */
export function selectLanguageInputs(value,{game,variant}){
  object(value,'Language inputs');assert.equal(value.schema,LANGUAGE_INPUT_SCHEMA,'Unsupported language inputs');
  assert.ok(typeof game==='string'&&game.trim(),'Select a language input game');
  assert.ok(['normal','multiplayer'].includes(variant),'Select a language input variant');
  object(value.products,'Language input products');
  const product=value.products[game]?.[variant];object(product,`Language input ${game}/${variant}`);
  assert.ok(Object.keys(product).every(key=>['catalog','packs','assets'].includes(key)),'Unknown language input field');
  const catalog=localFile(product.catalog,'Language catalog');object(product.packs,'Selected language packs');
  const packs=Object.fromEntries(Object.entries(product.packs).map(([id,path])=>{
    assert.match(id,languageId,'Invalid selected language');return[id,localFile(path,`Language pack ${id}`)];
  }));
  assert.ok(Object.keys(packs).length>0,'Select at least one translated language pack');
  return{game,variant,catalog,packs,...(product.assets?{assets:localFile(product.assets,'Prepared game assets')}:{} )};
}

function catalogEntries(value,game){
  if(value.schema==='eagler-touhou/site-manifest/1'){
    const product=value.games?.[game];object(product,`Published language product ${game}`);
    return product.languages;
  }
  assert.equal(value.schema,'eagler-touhou/thcrap-static-catalog/1','Unsupported language catalog');
  assert.equal(value.game,game,'Language catalog belongs to another game');return value.languages;
}

/** The current production archive validator is used after catalog byte/hash verification. */
export async function inspectLanguageInputs(product){
  const catalogIdentity=await fileIdentity(product.catalog),catalogBytes=await readFile(product.catalog);
  assert.equal(sha256(catalogBytes),catalogIdentity.sha256,'Language catalog changed before parsing');
  const catalog=JSON.parse(catalogBytes.toString('utf8'));
  const entries=catalogEntries(catalog,product.game);
  assert.ok(Array.isArray(entries),'Language catalog must declare language entries');
  const indexed=new Map();
  for(const entry of entries){
    assert.ok(entry&&languageId.test(entry.id),'Language catalog contains an invalid language ID');
    assert.ok(!indexed.has(entry.id),'Language catalog repeats a language ID');indexed.set(entry.id,entry);
  }
  const packs={};
  for(const [language,path] of Object.entries(product.packs)){
    const declaration=indexed.get(language)?.pack;
    assert.ok(declaration,`Language ${language} is not published in the selected catalog`);
    assert.ok(Number.isSafeInteger(declaration.bytes)&&declaration.bytes>0&&declaration.bytes<=64*1024*1024,
      `Language ${language} has an invalid archive size`);
    assert.match(declaration.sha256,/^[a-f0-9]{64}$/i,`Language ${language} needs its published SHA-256`);
    const identity=await fileIdentity(path);
    assert.equal(identity.bytes,declaration.bytes,`Language ${language} archive size differs from its catalog`);
    assert.equal(identity.sha256,declaration.sha256.toLowerCase(),`Language ${language} archive SHA-256 differs from its catalog`);
    const archive=await readFile(path);
    assert.equal(sha256(archive),identity.sha256,`Language ${language} archive changed before parsing`);
    let memberCount=0,unpackedBytes=0;
    const unzipped=unzipSync(archive,{filter:file=>{
      assert.ok(++memberCount<=257,'Language archive exceeds its member limit');
      assert.ok(file.originalSize<=64*1024*1024,'Language archive member is too large');
      unpackedBytes+=file.originalSize;
      assert.ok(unpackedBytes<=256*1024*1024,'Language archive exceeds its unpacked byte limit');
      return true;
    }});
    const validated=validateStaticLanguagePackEntries(unzipped,{game:product.game,language});
    const members={};
    for(const file of validated.files){
      members[file.path]={bytes:file.bytes.length,sha256:sha256(file.bytes)};
    }
    packs[language]={path,identity,declaration:structuredClone(declaration),manifest:validated.manifest,
      files:validated.files,members};
    assert.deepEqual(await fileIdentity(path),identity,`Language ${language} archive changed during preflight`);
  }
  assert.deepEqual(await fileIdentity(product.catalog),catalogIdentity,'Language catalog changed during preflight');
  return{game:product.game,variant:product.variant,catalog:{path:product.catalog,...catalogIdentity},packs,
    ...(product.assets?{assets:product.assets}:{})};
}

/** A report records identities; the original archive and its private bytes stay local. */
export function languageInputReceipt(inspected){
  return{game:inspected.game,variant:inspected.variant,catalog:inspected.catalog,
    packs:Object.fromEntries(Object.entries(inspected.packs).map(([id,pack])=>[id,{path:pack.path,...pack.identity,
      runtimeVersion:pack.manifest.runtimeVersion,members:pack.members}]))};
}
