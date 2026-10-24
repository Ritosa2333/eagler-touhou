import assert from 'node:assert/strict';
import {readFile,writeFile,readdir} from 'node:fs/promises';
import {resolve,basename,isAbsolute} from 'node:path';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {PRODUCT_CONTENT} from '../../launcher/lib/content-definition.mjs';
import {inspectRoomResources} from './room-resources.mjs';
import {inspectOfflineArtwork} from './offline-runtime-artwork.mjs';
import {inspectOfflineUpdateDescriptor} from './offline-runtime-update.mjs';
import {fileIdentity} from '../testing/dependency-snapshot.mjs';
import {preloadLayout,createSiteSeed,featureConfig,runLogged} from './launcher-fixture-preparation.mjs';

import {fileSetIdentity,verifyReleaseManifest} from '../../launcher/lib/release-manifest.mjs';

/** Verify the produced artifact's monorepo provenance, including the shared Runtime owner. */
export async function verifyLauncherFixtureSources({site,root,game}){
  const workspace=JSON.parse(await readFile(resolve(root,'launcher/config/workspace.json'),'utf8'));
  assert.ok(Object.hasOwn(PRODUCT_GAMES,game),`Unknown fixture product ${game}`);
  const manifest=await verifyReleaseManifest(site),sources={};
  for(const owner of ['launcher','common',game]){
    const sourcePath=workspace.repositories?.[owner];assert.ok(sourcePath,`${owner}: source owner is not configured`);
    const source=manifest.sources?.[sourcePath];assert.ok(source,`${owner}: packaged source identity is missing`);
    assert.equal(source.sourcePath,sourcePath,`${owner}: packaged sourcePath differs from the monorepo owner`);
    assert.match(source.revision??'',/^[a-f0-9]{40}$/i,`${owner}: source revision is missing`);
    assert.match(source.sha256??'',/^[a-f0-9]{64}$/i,`${owner}: source identity is missing`);
    sources[owner]={repository:sourcePath,...source};
  }
  return{releaseId:manifest.releaseId,sources};
}

/** Bind optional OGG declarations to real source bytes and the imported package component. */
export async function verifyLauncherFixtureMusic({game,input,descriptor}){
  const contract=PRODUCT_GAMES[game].package,source=resolve(input.sourceDirectory);
  assert.ok(isAbsolute(input.sourceDirectory),'OGG source directory must be absolute');
  assert.equal(input.mount,contract.musicMounts.ogg,'OGG mount differs from the product contract');
  const suffix=['games','web-content',game,...contract.musicSourceDirectories.ogg.split('/')].join('/');
  assert.ok(source.replaceAll('\\','/').toLowerCase().endsWith('/'+suffix.toLowerCase()),'OGG source differs from the product directory');
  assert.ok(Array.isArray(input.files)&&input.files.length>0,'OGG input inventory is empty');
  const names=input.files.map(file=>file.name);
  assert.equal(new Set(names).size,names.length,'OGG inventory contains duplicates');
  assert.deepEqual(names,[...names].sort((a,b)=>a.localeCompare(b)),'OGG inventory must be sorted');
  const actual=(await readdir(source,{withFileTypes:true})).filter(entry=>entry.isFile()&&/\.ogg$/i.test(entry.name)).map(entry=>entry.name).sort((a,b)=>a.localeCompare(b));
  assert.deepEqual(names,actual,'OGG source inventory changed');
  const componentIds=Object.values(descriptor.components??{}).filter(component=>component.type==='ogg').flatMap(component=>component.files);
  assert.deepEqual([...componentIds].sort(),names.map(name=>'ogg:'+name).sort(),'Imported ZIP must declare the complete OGG component');
  for(const file of input.files){
    assert.match(file.name,/^[A-Za-z0-9_.-]+\.ogg$/i,'OGG filename must be a basename');
    assert.ok(isAbsolute(file.path),'OGG file path must be absolute');
    assert.equal(resolve(file.path),resolve(source,file.name),'OGG file escapes its source directory');
    const identity=await fileIdentity(file.path);
    assert.deepEqual(identity,{bytes:file.bytes,sha256:file.sha256},`${file.name}: OGG source identity changed`);
    const declared=descriptor.files['ogg:'+file.name];
    assert.ok(declared,`${file.name}: ZIP OGG declaration is missing`);
    assert.equal(declared.target,input.mount+'/'+file.name,`${file.name}: ZIP mount differs`);
    assert.equal(declared.bytes,file.bytes,`${file.name}: ZIP OGG byte count differs`);
    assert.equal(declared.sha256,file.sha256,`${file.name}: ZIP OGG SHA differs`);
    assert.equal(basename(declared.source),file.name,`${file.name}: ZIP OGG source differs`);
  }
  return{sourceDirectory:source,mount:input.mount,files:input.files.map(file=>({...file}))};
}

/** Prepare the real import-mode Launcher; browser/server ownership stays with its caller. */
export async function prepareLauncherRuntimeFixture({root,output,site=resolve(output,'site'),target,identity,dataIdentity,shared={},artwork,packageMap,
  resourceMode='import',profile='web-validation-offline-desktop',musicInputs=null,practiceFeatures={}}){
  const game=target.game,launcher=resolve(root,'launcher');
  const requestedFeatures=featureConfig({resourceMode,practiceFeatures});
  for(const requestedGame of Object.keys(practiceFeatures))assert.equal(requestedGame,game,'Practice feature request must belong to the selected fixture product');
  const practiceRequest={...practiceFeatures};
  assert.equal(target.variant,'normal','Launcher fixture requires the selected normal Runtime');
  assert.equal(resourceMode,'import','This fixture prepares the production local-package import route');
  assert.equal(typeof packageMap?.[game],'string',`${game}: offline ZIP input is missing`);
  const artworkPreflight=await inspectOfflineArtwork(artwork);
  assert.equal(artworkPreflight.available,true,`Offline Launcher artwork input is incomplete: ${artworkPreflight.missing.map(file=>file.name).join(', ')}`);
  if(target.dataSha256)assert.equal(dataIdentity.sha256,target.dataSha256,`${target.id}: DATA differs from the acceptance manifest`);
  const packagePreflight=await inspectRoomResources({baseUrl:'http://127.0.0.1/',targets:[target],packageMap:{[game]:packageMap[game]}});
  assert.equal(packagePreflight.passed,true,`${game}: offline ZIP preflight failed: ${JSON.stringify(packagePreflight.games)}`);
  const resource=packagePreflight.games[0];
  assert.equal(resource.expectedData.sha256,dataIdentity.sha256,`${game}: ZIP preflight DATA differs from actual Runtime input`);
  const dataLayout=await preloadLayout(target,identity);
  assert.equal(dataLayout,PRODUCT_CONTENT[game]?.dataLayout??dataLayout,`${game}: Runtime DATA layout disagrees with product contract`);
  const sharedIdentity={};for(const [name,path] of Object.entries(shared))sharedIdentity[name]={path,...await fileIdentity(path)};
  const seed=createSiteSeed(game,dataIdentity,dataLayout,profile);
  let music=null;
  if(musicInputs){
    const archive=await inspectOfflineUpdateDescriptor(packageMap[game]);
    assert.equal(archive.archiveSha256,resource.package.archiveSha256,'ZIP changed after fixture preflight');
    music=await verifyLauncherFixtureMusic({game,input:musicInputs,descriptor:archive.descriptor});
    seed.games[game].music.ogg={...fileSetIdentity(music.files.map(file=>file.name),music.files),mount:music.mount,
      files:music.files.map(file=>file.name)};
  }
  const id=basename(site).replace(/^site-/,''),seedPath=resolve(output,`site-seed-${id}.json`),featuresPath=resolve(output,`features-${id}.json`);
  await writeFile(seedPath,JSON.stringify(seed,null,2)+'\n');await writeFile(featuresPath,JSON.stringify(requestedFeatures,null,2)+'\n');
  const args=['scripts/package-server.mjs',`--output=${site}`,`--site-manifest=${seedPath}`,`--feature-config=${featuresPath}`,
    `--profile=${profile}`,'--runtime-variants=normal',`--games=${game}`,'--music=midi',`--artwork-dir=${artwork}`,`--${game}-build=${target.runtime}`];
  const packaging=await runLogged(process.execPath,args,{cwd:launcher,stdoutPath:resolve(output,'package-server.stdout.log'),stderrPath:resolve(output,'package-server.stderr.log'),label:'Launcher package-server'});
  await runLogged(process.execPath,['scripts/verify-server-build.mjs',site],{cwd:launcher,stdoutPath:resolve(output,'verify-server.stdout.log'),stderrPath:resolve(output,'verify-server.stderr.log'),label:'verify-server-build'});
  const sourceProvenance=await verifyLauncherFixtureSources({site,root,game});
  const [siteManifest,deployment,runtimeManifest,releaseCatalog]=await Promise.all(['site-manifest.json','deployment.json','runtime-manifest.json','release-catalog.json'].map(async name=>JSON.parse(await readFile(resolve(site,name),'utf8'))));
  assert.equal(siteManifest.shared.resourceMode,resourceMode);assert.deepEqual(deployment.games,[game]);
  const expectedPractice=requestedFeatures.games[game]?.thprac??false;
  assert.equal(siteManifest.games[game].features?.thprac,expectedPractice,`${game}: published thprac feature differs from the explicit request`);
  const features={request:practiceRequest,configPath:featuresPath,config:JSON.parse(await readFile(featuresPath,'utf8')),
    seedPath,published:{game,thprac:siteManifest.games[game].features.thprac}};
  assert.deepEqual(features.config,requestedFeatures,'Launcher feature input changed during packaging');
  assert.equal(deployment.appShell?.schema,'eagler-touhou/app-shell/2',`${game}: packaged Launcher does not declare its App Shell`);
  assert.deepEqual(releaseCatalog.games,{},'Import-mode site must not silently supply a second online package');
  if(music)assert.deepEqual(siteManifest.games[game].music.ogg,seed.games[game].music.ogg,'Published OGG declaration differs from the verified imported component');
  const runtimeGroup=runtimeManifest.groups.find(group=>group.root===`runtime/${game}/`);
  assert.ok(runtimeGroup?.current?.generation,`${game}: packaged normal Runtime generation is missing`);
  return{site,artworkPreflight,resource,dataLayout,packaging,siteManifest,deployment,runtimeManifest,releaseCatalog,runtimeGroup,sharedIdentity,music,features,sourceProvenance};
}
