#!/usr/bin/env node
import {existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {resolve, relative} from 'node:path';
import {execFileSync} from 'node:child_process';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
import {targets, root, inspectInputs, sourceDependencies, sourceIdentity, slash} from './inputs.mjs';
import {checkRuntimeSource, runtimeSourceFiles} from './runtime-contracts.mjs';
import {STATUS_SCHEMA, checkStates, retainedVerification, validateStatusReport, readStatusReport} from './status.mjs';
import {adaptationMarkdown, preserveManualEvidence, portDefinitions, notApplicable, readVerificationSnapshot} from '../docs/new-adaptation.mjs';
import {runTasks} from '../testing/runner.mjs';
import {parseWindowsCompilerHelpers} from '../testing/compiler-helpers.mjs';

const spec = JSON.parse(readFileSync(resolve(root, 'docs/adaptation/checklist.json'), 'utf8'));
const sharedPlan = JSON.parse(readFileSync(resolve(root, 'tools/ports/shared-checks.json'), 'utf8'));
const ownSources = ['tools/ports/check.mjs','tools/ports/inputs.mjs','tools/ports/runtime-contracts.mjs',
  'tools/ports/status.mjs','tools/ports/shared-checks.json','tools/build/runtime-inputs.mjs',
  'docs/adaptation/checklist.json','tools/docs/new-adaptation.mjs',
  'launcher/config/runtime-builds.json','launcher/config/workspace.json','launcher/src/contracts/runtime-protocol.mts',
  'common/contracts/product-catalog.mjs'].map(name => resolve(root, name));

export function sharedGateTasks(gate,games,{repositoryRoot=root,compilerHelpers=[]}={}) {
  return gate.files.map((file,index)=>({id:`${gate.id}-${index+1}`,command:process.execPath,
    args:[resolve(repositoryRoot,file)],cwd:resolve(repositoryRoot,'launcher'),timeoutMs:180000,
    environment:{EAGLER_PORT_CHECK_GAMES:JSON.stringify(games)},
    ...(compilerHelpers.length ? {cleanupImages:compilerHelpers} : {})}));
}

async function commandCheck(id, files, checks, level, output, games,
  {repositoryRoot=root, compilerHelpers=[]}={}) {
  const taskOutput=resolve(output,`${id}-tasks`);
  const tasks=sharedGateTasks({id,files},games,{repositoryRoot,compilerHelpers});
  let report, runnerError;
  try {
    report=await runTasks(tasks,{output:taskOutput,concurrency:1,failFast:false,
      metadata:{gateId:id,gateLevel:level,checks}});
  } catch (error) {
    runnerError=error.message;
  }
  const results=new Map((report?.results ?? []).map(result=>[result.id,result]));
  const logs=tasks.map((task,index)=>{
    const file=files[index],result=results.get(task.id);
    let detail='';
    if(result?.log) {
      try { detail=readFileSync(result.log,'utf8'); }
      catch (error) { detail=`Unable to read task log ${result.log}: ${error.message}\n`; }
    }
    if(result?.error)detail+=`${result.error}\n`;
    return `===== ${file} [${result?.status ?? 'not-run'}] =====\n${detail}`;
  });
  if(runnerError)logs.push(`===== task runner failed =====\n${runnerError}\n`);
  writeFileSync(resolve(output,`${id}.log`),logs.join('\n'),'utf8');
  const failed=!report||report.status!=='passed';
  return {id,checks,level,files,complete:true,status:failed?'failed':'passed',
    ...(failed ? {error:`Executable checks failed; see ${id}.log`} : {})};
}

/** Run every selected shared gate, preserving each gate's domain and continuing after failures. */
export async function runSharedGates(gates,{output,games,repositoryRoot=root,compilerHelpers=[]}={}) {
  mkdirSync(output,{recursive:true});
  const results=[];
  for(const gate of gates) {
    try {
      results.push({...gate,...await commandCheck(gate.id,gate.files,gate.checks,gate.level,output,games,
        {repositoryRoot,compilerHelpers})});
    } catch(error) {
      writeFileSync(resolve(output,`${gate.id}.log`),`===== task runner failed =====\n${error.message}\n`,'utf8');
      results.push({...gate,complete:true,status:'failed',error:`Executable checks failed; see ${gate.id}.log`});
    }
  }
  return results;
}

function evidenceSources(target) {
  const names = execFileSync('git', ['ls-files','--cached','--others','--exclude-standard','-z','--',
    target.directory,'launcher/app','launcher/src','launcher/lib','launcher/tests','launcher/package',
    'launcher/package.json','launcher/package-lock.json',
    'launcher/config','common','tools/ports','tools/build','tools/browser','tools/testing','tests'],
    {cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:16*1024*1024}).split('\0').filter(Boolean);
  return [...ownSources, ...names.filter(name=>/\.(?:mjs|mts|ts|tsx|jsx|cpp|c|h|hpp|inc|html|json|py|sh|ps1|cmake)$/.test(name)||name.endsWith('/CMakeLists.txt'))
    .map(name=>resolve(root,name)).filter(existsSync)];
}

export function validatePortLayout(directory, inputFiles) {
  const issues = [];
  const owner = resolve(root, directory);
  const compiler = new Set(inputFiles.map(path => resolve(path)));
  const names = execFileSync('git',['ls-files','--cached','--others','--exclude-standard','-z','--',directory],
    {cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:8*1024*1024}).split('\0').filter(Boolean);
  for (const name of names) {
    const child = resolve(root,name);
    if (!existsSync(child)) continue;
    const local = slash(relative(owner,child)), top = local.split('/')[0];
    if (/(?:_web$|^(?:analysis|reports|incremental|source_reconstruction|native_recovered|handoff|deploy|\.github)$)/.test(top))
      issues.push(`${directory}/${top}: directory is outside the current port layout`);
    if (/\.(?:asm|obj|pdb|dmp)$/i.test(local) && !compiler.has(child))
      issues.push(`${name}: not a declared compiler input`);
  }
  return [...new Set(issues)];
}

function parseArgs(args) {
  const options = {variant:'both', output:resolve(root,'artifacts/validation/ports')};
  for (const arg of args) {
    if (arg === '--all') continue;
    if (arg === '--write-status') {options.writeStatus = true; continue;}
    if (arg === '--check-status') {options.checkStatus = true; continue;}
    const match = arg.match(/^--(game|variant|output)=(.+)$/);
    if (!match) throw new Error(`Unknown argument: ${arg}`);
    options[match[1]] = match[1] === 'output' ? resolve(match[2]) : match[2];
  }
  return options;
}

function compactRecord(record) {
  // The complete compiler inventory is a local/CI artifact; the tracked status
  // keeps its identity, scope, and the executable evidence used by the table.
  const {inputFiles, ...compact} = record;
  return compact;
}

async function main(args) {
  if (args.includes('--help')) {
    console.log('node tools/ports/check.mjs [--game=th08] [--variant=normal|multiplayer|both] [--write-status|--check-status] [--output=DIR]');
    return;
  }
  const options = parseArgs(args);
  const selected = targets(options);
  mkdirSync(options.output, {recursive:true});
  const compilerHelpers=parseWindowsCompilerHelpers(process.env,process.platform);
  const preparation=await runTasks([{id:'launcher-build',command:process.execPath,
    args:[resolve(root,'launcher/scripts/build-launcher.mjs'),'--force'],cwd:resolve(root,'launcher'),timeoutMs:180000,
    ...(compilerHelpers.length ? {cleanupImages:compilerHelpers} : {})}],
  {output:resolve(options.output,'launcher-build-task'),concurrency:1});
  const preparationResult=preparation.results[0];
  writeFileSync(resolve(options.output,'launcher-build.log'),readFileSync(preparationResult.log,'utf8'));
  if (preparationResult.status!=='passed') throw new Error('Launcher module build failed; see launcher-build.log');
  const previous = readStatusReport(root);
  const games = [...new Set(selected.map(target=>target.game))];
  const shared = await runSharedGates(sharedPlan.gates.filter(gate=>selected.some(target=>
    (!gate.games || gate.games.includes(target.game)) && (!gate.variants || gate.variants.includes(target.variant)))),
  {output:options.output,games,repositoryRoot:root,compilerHelpers});
  const records = [];
  for (const target of selected) {
    const label = `${target.game}-${target.variant}`;
    try {
      const input = await inspectInputs(target);
      const {dependencies, inputFiles: dependencyFiles} = sourceDependencies(target);
      writeFileSync(resolve(options.output, `${label}-inputs.json`), JSON.stringify(input,null,2)+'\n');
      const issues = validatePortLayout(target.directory, input.inputFiles);
      const owner = resolve(root,target.directory);
      const nativeFiles = input.translationUnits.map(name => resolve(owner,name));
      const files = [...input.inputFiles, ...dependencyFiles, ...runtimeSourceFiles(target.game, PRODUCT_GAMES[target.game]).map(name=>resolve(owner,name)),
        resolve(owner,target.recipe.kind === 'cmake' ? 'CMakeLists.txt' : target.recipe.build[0]), ...evidenceSources(target)];
      const gates = [{id:'source-inputs', status:issues.length ? 'failed':'passed', level:'source-inputs', complete:true,
        checks:['BUILD-01'], files:[target.directory], ...(issues.length ? {error:issues.join('\n')}: {})},
        ...checkRuntimeSource({...target, product:PRODUCT_GAMES[target.game], nativeFiles, root}),
        ...shared.filter(gate => (!gate.games || gate.games.includes(target.game)) &&
          (!gate.variants || gate.variants.includes(target.variant)))];
      const identity = sourceIdentity(files,{game:target.game,variant:target.variant,
        features:target.definition.features,selection:input.selection,dependencies,
        compilerFlags:input.compilerFlags.map(flag=>flag.replaceAll('\\','/').replaceAll(slash(root),'<repo>'))});
      gates.push(...retainedVerification(previous?.targets?.find(record=>record.game===target.game && record.variant===target.variant),identity));
      records.push({game:target.game, variant:target.variant, directory:target.directory,
        sourceDigest:identity, sourceSelection:input.selection, dependencies, translationUnits:input.translationUnits.length,
        features:target.definition.features, command:input.command, gates});
      const failed = gates.filter(gate=>gate.status==='failed');
      console.log(`${target.game}/${target.variant}: ${failed.length ? 'FAIL':'PASS'} (${gates.length-failed.length}/${gates.length} checks)`);
      for (const gate of failed) console.log(`  ${gate.id}: ${gate.error.split('\n')[0]}`);
    } catch (error) {
      records.push({game:target.game,variant:target.variant,directory:target.directory,sourceDigest:null,
        gates:[{id:'source-inputs',status:'failed',level:'source-inputs',complete:true,checks:['BUILD-01'],error:error.message}]});
      console.log(`${target.game}/${target.variant}: FAIL (${error.message.split('\n')[0]})`);
    }
  }
  const selectedKeys = new Set(records.map(record=>`${record.game}/${record.variant}`));
  const combined = [...(previous?.targets ?? []).filter(record=>!selectedKeys.has(`${record.game}/${record.variant}`)), ...records]
    .sort((a,b)=>`${a.directory}/${a.variant}`.localeCompare(`${b.directory}/${b.variant}`,'en'));
  const report = {schema:STATUS_SCHEMA, identityAlgorithm:'sha256/canonical-source-text',
    targets:combined.map(compactRecord)};
  const validation = validateStatusReport(report,{expectedTargets:options.game ? selected:targets(), allowedTargets:targets(),
    checkIds:new Set(spec.checks.map(check=>check.id))});
  writeFileSync(resolve(options.output,'report.json'),JSON.stringify(report,null,2)+'\n');
  if (options.writeStatus) {
    writeFileSync(resolve(root,'docs/adaptation/status.json'),JSON.stringify(report,null,2)+'\n');
    for (const port of portDefinitions()) {
      if (options.game && port.game !== options.game) continue;
      const evidence = Object.fromEntries(port.variants.map(variant=>{
        const result = combined.find(record=>record.directory===port.directory&&record.variant===variant);
        return [variant, result ? checkStates({port,variant,checks:spec.checks,gates:result.gates,notApplicableFor:notApplicable}) : {}];
      }));
      const path=resolve(root,port.directory,'ADAPTATION.md');
      const verification=readVerificationSnapshot(port);
      writeFileSync(path,preserveManualEvidence(adaptationMarkdown(port,{evidence,verification}),existsSync(path)?readFileSync(path,'utf8'):''));
    }
  }
  if (options.checkStatus) {
    if (!previous) validation.push('Missing docs/adaptation/status.json; run --write-status');
    else for (const record of records) {
      const old = previous.targets.find(item=>item.game===record.game&&item.variant===record.variant);
      if (old?.sourceDigest !== record.sourceDigest) validation.push(`${record.game}/${record.variant}: stale status source identity`);
    }
  }
  for (const issue of validation) console.error(issue);
  if (validation.length || records.some(record=>record.gates.some(gate=>gate.status==='failed'))) process.exitCode=1;
}

if (resolve(process.argv[1] ?? '') === resolve(import.meta.filename)) {
  main(process.argv.slice(2)).catch(error=>{console.error(error);process.exitCode=1;});
}
