import {execFileSync} from 'node:child_process';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runtimeSubmoduleInitCommands} from '../lib/runtime-build-dependencies.mjs';
import {loadRuntimeBuildConfig} from '../lib/runtime-build-profiles.mjs';
import {launcherRoot,workspaceRoot} from '../lib/workspace-layout.mjs';
import {loadSourceLayout} from '../lib/source-layout.mjs';

export function parseRuntimeDependencyArgs(args) {
  const options={};
  for (const arg of args) {
    const match=arg.match(/^--(game|variant)=(.+)$/);
    if (!match || Object.hasOwn(options,match[1]))
      throw new Error(`invalid or duplicate argument: ${arg}`);
    options[match[1]]=match[2];
  }
  if (!options.game) throw new Error('usage: node launcher/scripts/prepare-runtime-dependencies.mjs --game=thXX[,thYY] [--variant=normal|multiplayer]');
  const games=options.game.split(',').map(game=>game.trim());
  if (games.some(game=>!game)) throw new Error('game list contains an empty entry');
  options.games=[...new Set(games)];
  options.variant??='normal';
  return options;
}

function pathKey(path) {
  const value=resolve(path);
  return process.platform==='win32'?value.toLowerCase():value;
}

export function planRuntimeDependencyPreparation({games,variant='normal',config,layout}) {
  if (!Array.isArray(games) || !games.length) throw new Error('at least one Runtime game is required');
  if (!config?.games || !layout?.path) throw new Error('Runtime build configuration and source layout are required');
  const selectedGames=[],sources=[],commands=[],seenGames=new Set(),seenSources=new Set(),seenCommands=new Set();
  for (const game of games) {
    if (seenGames.has(game)) continue;
    seenGames.add(game);
    const entry=config.games[game],definition=entry?.variants?.[variant];
    if (!entry || !definition) throw new Error(`unknown Runtime build: ${game}/${variant}`);
    const owner=definition.workspaceRepository??entry.workspaceRepository;
    if (typeof owner!=='string'||!owner) throw new Error(`Runtime build has no workspace repository: ${game}/${variant}`);
    const source=resolve(layout.path(owner)),sourceId=pathKey(source);
    selectedGames.push(game);
    if (!seenSources.has(sourceId)) {
      seenSources.add(sourceId);
      sources.push(source);
    }
    const recipe={...entry.sourceBuild,...definition.sourceBuild};
    const declarations=recipe.requiredSubmodules??[];
    if (!Array.isArray(declarations)) throw new Error(`requiredSubmodules must be an array: ${game}/${variant}`);
    for (const command of runtimeSubmoduleInitCommands({source,declarations})) {
      const key=`${pathKey(command.cwd)}\0${JSON.stringify(command.args)}`;
      if (seenCommands.has(key)) continue;
      seenCommands.add(key);
      commands.push(command);
    }
  }
  return {games:selectedGames,sources,commands};
}

export function executeRuntimeDependencyCommands(commands,run=command=>
  execFileSync('git',command.args,{cwd:command.cwd,stdio:'inherit',windowsHide:true})) {
  if (!Array.isArray(commands)) throw new Error('Runtime dependency command list must be an array');
  for (const command of commands) run(command);
}

export async function main(args=process.argv.slice(2)) {
  const options=parseRuntimeDependencyArgs(args);
  const config=await loadRuntimeBuildConfig();
  const layout=loadSourceLayout(launcherRoot(),{
    environment:{...process.env,EAGLER_WORKSPACE_ROOT:workspaceRoot()},
  });
  const plan=planRuntimeDependencyPreparation({...options,config,layout});
  if (!plan.commands.length) {
    process.stdout.write(`No required Runtime submodules for ${plan.games.join(', ')} / ${options.variant}\n`);
    return;
  }
  for (const command of plan.commands)
    process.stdout.write(`Preparing ${command.cwd}: git submodule update --init -- ${command.args.slice(4).join(' ')}\n`);
  executeRuntimeDependencyCommands(plan.commands);
}

if (process.argv[1] && resolve(process.argv[1])===fileURLToPath(import.meta.url)) {
  try { await main(); }
  catch (error) {
    process.stderr.write(`${error.message}\n`);
    process.exitCode=1;
  }
}
