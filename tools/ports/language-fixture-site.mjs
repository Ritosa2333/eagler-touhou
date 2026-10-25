import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {cp,mkdir,readFile,writeFile} from 'node:fs/promises';
import {dirname,resolve} from 'node:path';
import {PRODUCT_CONTENT} from '../../launcher/lib/content-definition.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {createStaticThcrapPack,staticLanguagePackPath} from '../../launcher/server/thcrap-static-pack.mjs';
import {runLogged,preloadLayout,createSiteSeed,featureConfig} from './launcher-fixture-preparation.mjs';
import {omitLanguageTableKeys,decodeLanguageTable} from './language-tables.mjs';

/** One assembled production Launcher serves all cold contexts in the selected language scene group. */
export async function prepareLanguageFixture({root,output,target,identity,dataIdentity,shared,artwork,inspected,profile}){
  const game=target.game,product=PRODUCT_GAMES[game],launcher=resolve(root,'launcher');
  assert.equal(target.variant,'normal','The current language UI journey selects normal explicitly');
  const packsRoot=resolve(output,'prepared-language-packs'),site=resolve(output,'site');
  await mkdir(resolve(packsRoot,'language'),{recursive:true});
  const catalog=[],expected={},mounts={};
  for(const [id,pack] of Object.entries(inspected.packs)){
    const path=staticLanguagePackPath(id);await cp(pack.path,resolve(packsRoot,path),{errorOnExist:true,force:false});
    catalog.push({id,title:pack.manifest.title??id,pack:{...pack.declaration,url:path}});
    const table=pack.files.find(file=>file.path===profile.table);
    assert.ok(table,`Language ${id} does not contain the declared table ${profile.table}`);
    expected[id]=decodeLanguageTable(table.bytes);
    mounts[id]=pack.members;
    for(const sample of profile.samples)assert.ok(expected[id].entries.some(entry=>entry.key===sample.key&&entry.hasTranslation),
      `Language ${id} does not translate declared sample ${sample.key}`);
  }
  const baseLanguage=Object.keys(inspected.packs)[0],base=inspected.packs[baseLanguage],partialLanguage='lang_acceptance-partial';
  assert.ok(!inspected.packs[partialLanguage],'Partial language control ID is reserved by the scene');
  const partialTable=omitLanguageTableKeys(base.files.find(file=>file.path===profile.table).bytes,[profile.missingKey]);
  const partial=createStaticThcrapPack({pack:{game,language:partialLanguage,title:'Partial translation control',assets:[]},
    resources:base.files.map(file=>({targetPath:file.path,bytes:file.path===profile.table?
      partialTable:file.bytes}))});
  await writeFile(resolve(packsRoot,partial.catalog.pack.url),partial.archive);catalog.push(partial.catalog);
  expected[partialLanguage]=decodeLanguageTable(partialTable);
  mounts[partialLanguage]={...base.members,[profile.table]:{bytes:partialTable.length,sha256:createHash('sha256').update(partialTable).digest('hex')}};
  await writeFile(resolve(packsRoot,'catalog.json'),JSON.stringify({schema:'eagler-touhou/thcrap-static-catalog/1',
    game,runtimeVersion:'independent',languages:catalog},null,2)+'\n');
  const dataLayout=await preloadLayout(target,identity),seed=createSiteSeed(game,dataIdentity,dataLayout,'web-validation-language');
  const features=featureConfig({resourceMode:'hosted',languages:{[game]:['ja',...catalog.map(entry=>entry.id)]}});
  const seedPath=resolve(output,'site-seed.json'),featuresPath=resolve(output,'features.json');
  await writeFile(seedPath,JSON.stringify(seed,null,2)+'\n');await writeFile(featuresPath,JSON.stringify(features,null,2)+'\n');
  let assets=inspected.assets;
  if(product.dataProvider==='retail-memory'){
    assets=resolve(output,'prepared-retail-data');await mkdir(assets,{recursive:true});
    const declaration=PRODUCT_CONTENT[game].hostPreparation?.dataAssets;
    const relative=declaration?.kind==='original-file'?declaration.source:declaration?.kind==='prepared-content'?`${game}.data`:null;
    assert.ok(relative,`${game} must declare its hosted retail DATA preparation`);
    await mkdir(dirname(resolve(assets,relative)),{recursive:true});await cp(target.data,resolve(assets,relative),{errorOnExist:true,force:false});
  }
  assert.ok(assets,`${game}: language input must name its prepared assets directory`);
  assert.ok(shared['msgothic.ttc']&&shared['unifont.otf'],'Language journey requires explicitly identified MS Gothic and Unifont inputs');
  const args=['scripts/package-server.mjs',`--output=${site}`,`--site-manifest=${seedPath}`,`--feature-config=${featuresPath}`,
    '--profile=web-validation-language','--runtime-variants=normal',`--games=${game}`,'--music=midi',`--artwork-dir=${artwork}`,
    `--${game}-build=${target.runtime}`,`--${game}-assets=${assets}`,`--${game}-data-assets=${assets}`,
    `--${game}-language-packs=${packsRoot}`,`--vanilla-font=${shared['msgothic.ttc']}`,`--font=${shared['unifont.otf']}`];
  const packaging=await runLogged(process.execPath,args,{cwd:launcher,label:'Language Launcher packaging',
    stdoutPath:resolve(output,'package-server.stdout.log'),stderrPath:resolve(output,'package-server.stderr.log')});
  await runLogged(process.execPath,['scripts/verify-server-build.mjs',site],{cwd:launcher,label:'Language Launcher verification',
    stdoutPath:resolve(output,'verify-server.stdout.log'),stderrPath:resolve(output,'verify-server.stderr.log')});
  const manifest=JSON.parse(await readFile(resolve(site,'site-manifest.json'),'utf8'));
  assert.equal(manifest.games[game].gameData.sha256,dataIdentity.sha256,'Hosted DATA differs from the acceptance input');
  assert.equal(manifest.games[game].gameData.bytes,dataIdentity.bytes,'Hosted DATA size differs from the acceptance input');
  const partialIdentity={bytes:partial.archive.length,sha256:partial.sha256};
  return{site,manifest,packaging,expected,mounts,baseLanguage,partialLanguage,partialIdentity,
    partialInput:{path:resolve(packsRoot,partial.catalog.pack.url),language:partialLanguage,...partialIdentity}};
}
