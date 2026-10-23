import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,readFile,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createStaticThcrapPack} from '../../launcher/server/thcrap-static-pack.mjs';
import {inspectLanguageInputs,languageInputReceipt,selectLanguageInputs,LANGUAGE_INPUT_SCHEMA} from './language-inputs.mjs';

async function fixture(operation){
  const root=await mkdtemp(join(tmpdir(),'eagler-language-inputs-'));
  try{
    const built=createStaticThcrapPack({pack:{game:'th08',language:'lang_en',assets:[]},resources:[
      {targetPath:'/thcrap/th08/localization/themes.etl',bytes:Buffer.from('ETL1\0\0\0\0')},
    ]});
    const pack=join(root,'lang_en.zip'),catalog=join(root,'catalog.json');
    await writeFile(pack,built.archive);
    await writeFile(catalog,JSON.stringify({schema:'eagler-touhou/thcrap-static-catalog/1',game:'th08',languages:[built.catalog]}));
    const input={schema:LANGUAGE_INPUT_SCHEMA,products:{th08:{normal:{catalog,packs:{lang_en:pack}}}}};
    await operation({root,built,pack,catalog,input});
  }finally{await rm(root,{recursive:true,force:true});}
}

test('real static producer archives pass the production consumer with catalog-bound identities',async()=>fixture(async({input,built})=>{
  const selected=selectLanguageInputs(input,{game:'th08',variant:'normal'});
  const inspected=await inspectLanguageInputs(selected),receipt=languageInputReceipt(inspected);
  assert.equal(inspected.packs.lang_en.files[0].path,'/thcrap/th08/localization/themes.etl');
  assert.equal(receipt.packs.lang_en.sha256,built.sha256);
  assert.equal(receipt.packs.lang_en.members['/thcrap/th08/localization/themes.etl'].bytes,8);
  assert.equal(Object.hasOwn(receipt.packs.lang_en,'files'),false,'report receipts do not embed private resource bytes');
  assert.throws(()=>selectLanguageInputs(input,{game:'th08',variant:'multiplayer'}),/th08\/multiplayer/);
  assert.throws(()=>selectLanguageInputs(input,{game:'th07',variant:'normal'}),/th07\/normal/);
}));

test('changed archive bytes cannot borrow a valid catalog declaration',async()=>fixture(async({input,pack})=>{
  const original=await readFile(pack),altered=Buffer.from(original);altered[altered.length-1]^=1;
  await writeFile(pack,altered);
  await assert.rejects(inspectLanguageInputs(selectLanguageInputs(input,{game:'th08',variant:'normal'})),/SHA-256 differs/);
}));

test('catalog selection never falls back to another product or an unpublished language',async()=>fixture(async({input,catalog,built})=>{
  await writeFile(catalog,JSON.stringify({schema:'eagler-touhou/thcrap-static-catalog/1',game:'th07',languages:[built.catalog]}));
  const selected=selectLanguageInputs(input,{game:'th08',variant:'normal'});
  await assert.rejects(inspectLanguageInputs(selected),/another game/);
  await writeFile(catalog,JSON.stringify({schema:'eagler-touhou/site-manifest/1',games:{th08:{languages:[]}}}));
  await assert.rejects(inspectLanguageInputs(selected),/not published/);
  await writeFile(catalog,JSON.stringify({schema:'eagler-touhou/site-manifest/1',games:{th08:{languages:[built.catalog,built.catalog]}}}));
  await assert.rejects(inspectLanguageInputs(selected),/repeats/);
}));

test('a correctly hashed archive for a different native game fails the real member validator',async()=>fixture(async({input,catalog,pack})=>{
  const wrong=createStaticThcrapPack({pack:{game:'th07',language:'lang_en',assets:[]},resources:[
    {targetPath:'/thcrap/th07/localization/themes.etl',bytes:Buffer.from('ETL1\0\0\0\0')},
  ]});
  await writeFile(pack,wrong.archive);
  await writeFile(catalog,JSON.stringify({schema:'eagler-touhou/thcrap-static-catalog/1',game:'th08',languages:[wrong.catalog]}));
  await assert.rejects(inspectLanguageInputs(selectLanguageInputs(input,{game:'th08',variant:'normal'})),/语言包清单不兼容/);
}));
