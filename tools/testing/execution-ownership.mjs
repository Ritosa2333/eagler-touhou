import {readFile} from 'node:fs/promises';
import {isAbsolute,relative,resolve,sep} from 'node:path';
import {pathToFileURL} from 'node:url';
import {isNodeTest} from './discovery.mjs';
import {loadOracleSuite} from './oracle-suite.mjs';

const testScriptPattern=/^(?:test.*|.*\.test)\.(?:mjs|cjs|js|py|ps1)$/i;
const pathPattern=/\.(?:mjs|cjs|js|py|ps1)$/i;

function normalize(file){return file.replaceAll('\\','/');}
function isCandidateTestPath(file){
  const normalized=normalize(file),name=normalized.split('/').at(-1);
  if(normalized.split('/').includes('fixtures'))return false;
  return isNodeTest(normalized)||testScriptPattern.test(name);
}
function relativeInside(root,path){
  const absolute=resolve(path),rel=relative(resolve(root),absolute);
  return rel&&!rel.startsWith(`..${sep}`)&&rel!=='..'&&!isAbsolute(rel)?normalize(rel):undefined;
}
function suiteCategory(suite,task){
  if(task?.resources?.includes('browser'))return 'browser';
  if(task?.id==='runtime-protocol'||/source-contract/.test(task?.id??''))return 'source-protocol';
  if(/module-unit/.test(task?.id??''))return 'module-unit';
  if(['native-gamepad','touch-native-invariants'].includes(task?.id)||/cooperative-rules/.test(task?.id??''))return 'production-component';
  return suite?.coverageKind??'module-unit';
}

async function componentManifestEntries(root,suiteId,task){
  const entries=[],scriptArgs=task.args.filter(arg=>typeof arg==='string'&&/scripts[\\/]check\.mjs$/.test(arg));
  if(scriptArgs.length){
    const checkPath=resolve(task.cwd,scriptArgs[0]);
    if(checkPath.toLowerCase()===resolve(root,'launcher/scripts/check.mjs').toLowerCase()){
      const manifest=await import(pathToFileURL(resolve(root,'launcher/tests/test-plan.mjs')).href);
      const workspace=task.args.includes('--workspace')||task.args.includes('--lane=workspace');
      const nodeFiles=workspace?manifest.WORKSPACE_NODE_TESTS:manifest.REPOSITORY_NODE_TESTS;
      const pythonFiles=workspace?manifest.WORKSPACE_PYTHON_TESTS:manifest.REPOSITORY_PYTHON_TESTS;
      for(const file of [...nodeFiles,...pythonFiles])entries.push({file:`launcher/${file}`,declaredBy:`launcher/tests/test-plan.mjs#${workspace?'WORKSPACE':'REPOSITORY'}_*_TESTS`,category:workspace?'source-protocol':'production-component'});
    }else{
      for(const component of ['relay','turn']){
        const planPath=resolve(root,`services/${component}/scripts/test-plan.mjs`);
        if(checkPath.toLowerCase()!==resolve(root,`services/${component}/scripts/check.mjs`).toLowerCase())continue;
        const manifest=await import(pathToFileURL(planPath).href);
        const names=Object.keys(manifest).filter(name=>/TESTS$/.test(name));
        for(const name of names)for(const file of manifest[name]??[])entries.push({file:`services/${component}/${file}`,
          declaredBy:`services/${component}/scripts/test-plan.mjs#${name}`,category:'module-unit'});
      }
    }
  }

  const portCheck=task.args.find(arg=>typeof arg==='string'&&/tools[\\/]ports[\\/]check\.mjs$/.test(arg));
  if(portCheck){
    const manifest=JSON.parse(await readFile(resolve(root,'tools/ports/shared-checks.json'),'utf8'));
    for(const gate of manifest.gates)for(const file of gate.files)entries.push({file,declaredBy:`tools/ports/shared-checks.json#${gate.id}`,
      category:gate.level==='source-contract'?'source-protocol':'module-unit',repeatKind:'per-title-shared-gate'});
  }
  return entries;
}

async function filesFromTask(root,suiteId,suite,task){
  const entries=await componentManifestEntries(root,suiteId,task),seen=new Set(entries.map(entry=>normalize(entry.file)));
  for(const arg of task.args){
    if(typeof arg!=='string'||arg.startsWith('-')||!pathPattern.test(arg))continue;
    const file=relativeInside(root,resolve(task.cwd,arg));
    if(!file||!isCandidateTestPath(file)||seen.has(file))continue;
    entries.push({file,declaredBy:`tests/suites.mjs#${suiteId}.tasks/${task.id}`,category:suiteCategory(suite,task)});
    seen.add(file);
  }
  return entries.map(entry=>({...entry,file:normalize(entry.file)}));
}

function duplicateDisposition(file,owners){
  const shared=owners.filter(owner=>owner.repeatKind==='per-title-shared-gate');
  if(shared.length===1&&owners.length===2&&file.startsWith('launcher/tests/')&&
    owners.some(owner=>owner.category==='production-component'))
    return 'Launcher component test is also rerun as a per-title shared gate with the selected game set.';
  const negative=owners.filter(owner=>owner.executionMode==='negative-control');
  const ordinary=owners.filter(owner=>owner.executionMode!=='negative-control');
  if(owners.length===2&&negative.length===1&&ordinary.length===1&&owners[0].suite===owners[1].suite)
    return 'The same browser fixture runs once normally and once with its explicit negative-control mutation.';
  return undefined;
}

function duplicatesForProfile(files){
  return Object.entries(files).flatMap(([file,owners])=>{
    if(owners.length<2)return [];
    const reason=duplicateDisposition(file,owners);
    return [{file,owners,...(reason?{reason}:{})}];
  });
}

export function assertNoSameProfileTestDuplicates(report){
  const duplicates=Object.entries(report.profiles).flatMap(([profile,result])=>
    result.duplicates.filter(duplicate=>!duplicate.reason).map(duplicate=>({profile,...duplicate})));
  if(duplicates.length){
    const details=duplicates.map(({profile,file,owners})=>`${profile}: ${file} (${owners.map(owner=>`${owner.suite}/${owner.task}`).join(', ')})`);
    throw new Error(`Test files have multiple owners in one profile:\n${details.join('\n')}`);
  }
}

async function packageScriptOwners(root,allFiles){
  const owners=new Map(),packageFiles=allFiles.filter(file=>file.startsWith('ports/')&&file.endsWith('/package.json'));
  for(const manifestFile of packageFiles){
    let manifest;try{manifest=JSON.parse(await readFile(resolve(root,manifestFile),'utf8'));}catch{continue;}
    const packageRoot=manifestFile.slice(0,-'/package.json'.length);
    for(const [script,command] of Object.entries(manifest.scripts??{})){
      if(!/^test(?::|-|$)/i.test(script))continue;
      const references=[...String(command).matchAll(/(?:^|\s)([\w./*\\-]+\.(?:mjs|cjs|js|py|ps1))(?=\s|$)/gi)].map(match=>match[1]);
      for(const reference of references){
        const rel=normalize(reference).replace(/^\.\//,'');
        const resolved=relativeInside(root,resolve(root,packageRoot,rel));
        if(!resolved)throw new Error(`Package test script path escapes repository: ${manifestFile}#scripts.${script} -> ${reference}`);
        const glob=resolved.includes('*')?new RegExp(`^${resolved.split('*').map(part=>part.replace(/[.*+?^${}()|[\]\\]/g,'\\$&')).join('[^/]*')}$`):undefined;
        const matched=glob?allFiles.filter(file=>glob.test(normalize(file))):[resolved];
        for(const file of matched){
          if(!allFiles.includes(file)||!isCandidateTestPath(file))continue;
          const current=owners.get(file)??[];current.push(`${manifestFile}#scripts.${script}`);owners.set(file,current);
        }
      }
    }
  }
  return owners;
}

async function oracleManifestInventory(root,allFiles,profileSelections,catalog){
  const manifestFiles=allFiles.filter(file=>/^ports\/[^/]+\/web\/tests\/cpp\/oracle-suite\.json$/.test(normalize(file)));
  const ownersByFile=new Map(),manifests=[];
  const suiteProfiles=Object.entries(profileSelections)
    .filter(([profile,suiteIds])=>profile!=='selection'&&suiteIds.includes('ports-oracle'))
    .map(([profile])=>profile).sort();
  const currentSelection=Array.isArray(profileSelections.selection)?[...profileSelections.selection]:null;
  const targets=catalog['ports-oracle']?.targets??[];
  for(const manifestFile of manifestFiles){
    const file=normalize(manifestFile),match=file.match(/^ports\/([^/]+)\/web\/tests\/cpp\/oracle-suite\.json$/);
    const owner=match[1],titleRoot=resolve(root,'ports',owner,'web');
    // This is the same identity, directory-discovery, and exact-classification gate used by the oracle runner.
    // Let its failures abort ownership reporting instead of silently dropping malformed local registrations.
    const suite=await loadOracleSuite(titleRoot);
    const target=targets.find(item=>item&&typeof item.root==='string'&&resolve(item.root)===titleRoot);
    const declared=suite.tests.map(item=>{
      const testFile=normalize(`ports/${owner}/web/tests/cpp/${item.file}`);
      const declaration={owner,manifest:file,role:item.role,
        ...(item.role==='candidate'?{group:item.group}:{maintenanceTask:item.task})};
      const entries=ownersByFile.get(testFile)??[];entries.push(declaration);ownersByFile.set(testFile,entries);
      return {file:testFile,...declaration};
    });
    manifests.push({owner,manifest:file,discoveredFiles:suite.discovered.length,
      candidateFiles:suite.tests.filter(item=>item.role==='candidate').length,
      maintenanceFiles:suite.tests.filter(item=>item.role==='maintenance').length,
      declarations:declared,
      unifiedSuite:{id:'ports-oracle',targetDeclared:Boolean(target),targetRoot:target?normalize(relative(root,titleRoot)):null,
        registeredProfiles:target?suiteProfiles:[],
        currentRunSelection:currentSelection===null?null:currentSelection.includes('ports-oracle')&&Boolean(target)},
      ciScheduling:'not-assessed-by-local-ownership'});
  }
  return {manifests,ownersByFile,suiteProfiles,currentSelection};
}

/** Resolve test-file ownership from the actual selected suite commands and their existing component manifests. */
export async function buildExecutionOwnership({root,catalog,profileSelections,files}){
  const allFiles=files??[];
  const profiles={};
  for(const [profile,suiteIds] of Object.entries(profileSelections)){
    const byFile={};
    for(const suiteId of [...new Set(suiteIds)]){
      const suite=catalog[suiteId];
      if(!suite)throw new Error(`${profile}: ownership references missing suite ${suiteId}`);
      // Runtime and oracle suites are explicit scenario owners, not Node test-file inventories.
      if(suite.tier==='acceptance'&&suiteId!=='launcher-workspace')continue;
      for(const task of suite.tasks())for(const entry of await filesFromTask(root,suiteId,suite,task)){
        const owners=byFile[entry.file]??[];
        owners.push({suite:suiteId,task:task.id,category:entry.category,declaredBy:entry.declaredBy,
          ...(entry.repeatKind?{repeatKind:entry.repeatKind}:{}),
          executionMode:task.args.includes('--negative-control')?'negative-control':'normal'});
        byFile[entry.file]=owners;
      }
    }
    profiles[profile]={suiteIds:[...new Set(suiteIds)],files:byFile,duplicates:duplicatesForProfile(byFile)};
  }

  const testOwners={};
  for(const [profile,result] of Object.entries(profiles))for(const [file,owners] of Object.entries(result.files)){
    const target=testOwners[file]??{profiles:[],owners:[]};
    if(!target.profiles.includes(profile))target.profiles.push(profile);
    target.owners.push(...owners.map(owner=>({profile,...owner})));
    testOwners[file]=target;
  }
  const profileReuse=Object.entries(testOwners).filter(([,owner])=>owner.profiles.length>1)
    .map(([file,owner])=>{
      const [left,right]=owner.profiles.map(name=>profiles[name]);
      const smaller=left&&right?(left.suiteIds.length<=right.suiteIds.length?left:right):undefined;
      const larger=smaller===left?right:left;
      const inherited=smaller&&larger&&smaller.suiteIds.every(id=>larger.suiteIds.includes(id))?smaller:undefined;
      let reason;
      if(inherited)reason=`${inherited.suiteIds.length} suite IDs from the smaller profile are included in the larger profile.`;
      else {
        const hasWorkspace=owner.owners.some(item=>item.suite==='launcher-workspace'&&item.declaredBy.includes('WORKSPACE_'));
        const hasIntegrationLane=owner.owners.some(item=>item.profile==='integration'&&
          (item.suite==='ports-source'||item.suite==='browser-shell'));
        if(hasWorkspace&&hasIntegrationLane)reason='The Launcher workspace manifest and integration shared-gate or browser-shell lanes both execute this declared path.';
      }
      return {file,profiles:owner.profiles,...(reason?{reason}:{})};
    });

  const hardDomains=[
    {directory:'tools',owner:'tooling'},
    {directory:'tests/adaptation',owner:'tooling'},
    {directory:'services/relay/tests',owner:'relay'},
    {directory:'services/turn/tests',owner:'turn'},
    {directory:'services/telemetry/tests',owner:'telemetry'},
  ];
  const managedOrphans=[];
  for(const {directory,owner} of hardDomains){
    const scheduled=Object.values(profiles).some(profile=>profile.suiteIds.includes(owner));
    if(!scheduled)continue;
    const candidates=allFiles.filter(file=>file.startsWith(`${directory}/`)&&isNodeTest(file));
    const owners=new Set(Object.values(profiles).flatMap(profile=>Object.entries(profile.files)
      .filter(([,entries])=>entries.some(entry=>entry.suite===owner)).map(([file])=>file)));
    for(const file of candidates)if(!owners.has(file))managedOrphans.push({file,expectedSuite:owner});
  }
  const launcherOwnershipPath='launcher/tests/test-test-ownership.mjs';
  const launcherOwnershipScheduled=Object.values(profiles).some(profile=>profile.suiteIds.includes('launcher-core'));
  if(launcherOwnershipScheduled&&allFiles.includes(launcherOwnershipPath)&&!testOwners[launcherOwnershipPath])
    managedOrphans.push({file:launcherOwnershipPath,expectedSuite:'launcher-core'});

  const packageOwners=await packageScriptOwners(root,allFiles);
  const oracle=await oracleManifestInventory(root,allFiles,profileSelections,catalog);
  const suiteOwned=new Set(Object.keys(testOwners));
  const portCandidates=allFiles.filter(file=>file.startsWith('ports/')&&isCandidateTestPath(file));
  const unownedPortCandidates=portCandidates.filter(file=>!suiteOwned.has(file)&&!packageOwners.has(file)&&!oracle.ownersByFile.has(normalize(file)));
  const manuallyDeclaredPortCandidates=portCandidates.filter(file=>!suiteOwned.has(file)&&packageOwners.has(file));
  const manifestOwnedPortCandidates=portCandidates.filter(file=>!suiteOwned.has(file)&&oracle.ownersByFile.has(normalize(file)));
  const byTopDirectory={};
  for(const file of unownedPortCandidates){const area=file.split('/').slice(0,2).join('/');byTopDirectory[area]=(byTopDirectory[area]??0)+1;}

  const suiteKinds={};
  for(const [profile,suiteIds] of Object.entries(profileSelections))for(const suiteId of suiteIds){
    const suite=catalog[suiteId];if(!suite)continue;
    const category=suite.coverageKind??'module-unit';
    const key=`${profile}/${suiteId}`;suiteKinds[key]={category,scope:suite.scope};
  }

  const intentionalProfileRepeats=Object.entries(profiles).flatMap(([profile,result])=>result.duplicates
    .filter(duplicate=>duplicate.reason).map(duplicate=>({profile,...duplicate})));
  return {profiles,testOwners,profileReuse,intentionalProfileRepeats,managedOrphans,suiteKinds,
    outsideSuiteInventory:{portsCandidateNames:portCandidates.length,packageScriptOwned:manuallyDeclaredPortCandidates.length,
      oracleManifestOwned:manifestOwnedPortCandidates.length,
      oracleManifestCandidateFiles:oracle.manifests.reduce((sum,item)=>sum+item.candidateFiles,0),
      oracleManifestMaintenanceFiles:oracle.manifests.reduce((sum,item)=>sum+item.maintenanceFiles,0),
      oracleManifests:oracle.manifests.map(({owner,manifest,discoveredFiles,candidateFiles,maintenanceFiles,unifiedSuite,ciScheduling})=>
        ({owner,manifest,discoveredFiles,candidateFiles,maintenanceFiles,localOwner:'oracle-suite.json',unifiedSuite,ciScheduling})),
      unownedCandidateNames:unownedPortCandidates.length,unownedByTopDirectory:byTopDirectory,
      unownedExamples:unownedPortCandidates.slice(0,8),
      note:'Local oracle manifest ownership, selected unified-suite scheduling, and CI scheduling are separate. Manifest registration does not establish CI execution; filename candidates outside these owners remain review-only, not pass evidence.'}};
}

export function summarizeExecutionOwnership(report){
  const profiles=Object.fromEntries(Object.entries(report.profiles).map(([name,profile])=>{
    const categories={};
    for(const owners of Object.values(profile.files))for(const owner of owners)categories[owner.category]=(categories[owner.category]??0)+1;
    return [name,{suites:profile.suiteIds.length,testPaths:Object.keys(profile.files).length,
      ownerEntries:Object.values(profile.files).reduce((sum,owners)=>sum+owners.length,0),categories,
      intentionalRepeats:profile.duplicates.filter(item=>item.reason).length,
      blockedDuplicates:profile.duplicates.filter(item=>!item.reason).length}];
  }));
  const repeatReasons={};
  for(const item of report.intentionalProfileRepeats)repeatReasons[item.reason]=(repeatReasons[item.reason]??0)+1;
  const examplesByReason=[];
  for(const item of report.intentionalProfileRepeats)if(!examplesByReason.some(example=>example.reason===item.reason))
    examplesByReason.push({profile:item.profile,file:item.file,reason:item.reason});
  const pairs={};
  for(const item of report.profileReuse){
    const key=item.profiles.join(' + '),pair=pairs[key]??{count:0,reason:item.reason??'Repeated path is listed in both profile manifests.'};
    pair.count++;if(pair.examples?.length<3)pair.examples.push(item.file);else if(!pair.examples)pair.examples=[item.file];pairs[key]=pair;
  }
  const suiteCategories={};
  for(const [key,entry] of Object.entries(report.suiteKinds)){
    const profile=key.split('/')[0],categories=suiteCategories[profile]??{};
    categories[entry.category]=(categories[entry.category]??0)+1;suiteCategories[profile]=categories;
  }
  return {profiles,suiteCategories,profileReusePaths:report.profileReuse.length,profileReusePairs:pairs,
    intentionalProfileRepeats:{count:report.intentionalProfileRepeats.length,byReason:repeatReasons,
      examples:examplesByReason},
    managedOrphans:report.managedOrphans,outsideSuiteInventory:report.outsideSuiteInventory};
}
