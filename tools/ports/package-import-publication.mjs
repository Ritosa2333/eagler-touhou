import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile,writeFile,readdir} from 'node:fs/promises';
import {resolve,relative} from 'node:path';
import {buildAppShell} from '../../launcher/lib/app-shell-build.mjs';
import {runtimeAppShellPaths} from '../../launcher/lib/app-shell-validation.mjs';
import {validateSiteManifest} from '../../launcher/lib/contracts/site-manifest.mjs';
import {resolveRuntimeGenerationWorkerSource} from '../../launcher/lib/contracts-build.mjs';
import {writeLegacySiteManifestAlias} from '../../launcher/compat/site-manifest.mjs';
import {verifyReleaseManifest,writeReleaseManifest} from '../../launcher/lib/release-manifest.mjs';
import {createPublishedPackageFixture} from './offline-installation-authority.mjs';

/** Publish real ZIP bytes under the Hosted contract, retaining the prepared frontend and Runtime. */
export async function publishPackageImportSite({siteDirectory,game,archivePath,expectedRevision,publishCatalog=true}){
  const root=resolve(siteDirectory),original=await verifyReleaseManifest(root);
  const fixture=await createPublishedPackageFixture({siteDirectory:root,game,archivePath,expectedRevision,publishCatalog});
  const readJson=async name=>JSON.parse(await readFile(resolve(root,name),'utf8'));
  const manifest=await readJson('site-manifest.json'),deployment=await readJson('deployment.json');
  manifest.shared.resourceMode='hosted';
  for(const [name,path]of [['vanillaFont','shared/msgothic.ttc'],['unicodeFont','shared/unifont.otf']]){
    if(fixture.files.some(file=>file.source===path)){
      const bytes=await readFile(resolve(root,path));manifest.shared[name]=path+'?v='+createHash('sha256').update(bytes).digest('hex');
    }else delete manifest.shared[name];
  }
  validateSiteManifest(manifest);await writeFile(resolve(root,'site-manifest.json'),JSON.stringify(manifest,null,2)+'\n');
  await writeLegacySiteManifestAlias(root,manifest);
  const shellFiles=deployment.frontend?.validationMetadata?.graph?.shellFiles;assert.ok(Array.isArray(shellFiles),'Prepared frontend must retain its sealed App Shell graph');
  const shell=await buildAppShell({quiet:true,globDirectory:root,swDest:resolve(root,'app-shell-sw.js'),shellFiles,
    deferredPaths:runtimeAppShellPaths(manifest),deferredPathPrefixes:['runtime/'],
    workerContractSource:await readFile(await resolveRuntimeGenerationWorkerSource(),'utf8')});
  const inventory=[];
  const visit=async directory=>{for(const entry of await readdir(directory,{withFileTypes:true})){
    const path=resolve(directory,entry.name);if(entry.isDirectory()){await visit(path);continue;}
    const name=relative(root,path).replaceAll('\\','/');if(['deployment.json','release-manifest.json','checksums.txt'].includes(name))continue;
    assert.equal(entry.isFile(),true);const bytes=await readFile(path);inventory.push({path:name,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')});
  }};await visit(root);inventory.sort((a,b)=>a.path.localeCompare(b.path,'en'));
  Object.assign(deployment,{resourceMode:'hosted',appShell:shell.contract,files:inventory});
  await writeFile(resolve(root,'deployment.json'),JSON.stringify(deployment,null,2)+'\n');
  await writeReleaseManifest(root,{profile:original.profile,sources:original.sources,parameters:{...original.parameters,
    acceptancePublication:{game,archive:fixture.sourceArchive,revision:fixture.revision,preparedReleaseId:original.releaseId}}});
  const release=await verifyReleaseManifest(root);return{fixture,deployment,releaseId:release.releaseId};
}
