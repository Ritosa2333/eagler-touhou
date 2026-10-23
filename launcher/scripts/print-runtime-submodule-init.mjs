import {loadRuntimeBuildConfig} from '../lib/runtime-build-profiles.mjs';
import {runtimeSubmoduleInitCommands} from '../lib/runtime-build-dependencies.mjs';
import {launcherRoot,workspaceRoot} from '../lib/workspace-layout.mjs';
import {loadSourceLayout} from '../lib/source-layout.mjs';

const options={};
for(const arg of process.argv.slice(2)) {
  const match=arg.match(/^--(game|variant)=(.+)$/);
  if(!match||Object.hasOwn(options,match[1]))throw new Error(`invalid or duplicate argument: ${arg}`);
  options[match[1]]=match[2];
}
if(!options.game)throw new Error('usage: node launcher/scripts/print-runtime-submodule-init.mjs --game=thXX [--variant=normal|multiplayer]');
options.variant??='normal';
const config=await loadRuntimeBuildConfig(),entry=config.games[options.game],definition=entry?.variants[options.variant];
if(!entry||!definition)throw new Error(`unknown Runtime build: ${options.game}/${options.variant}`);
const recipe={...entry.sourceBuild,...definition.sourceBuild},declarations=recipe.requiredSubmodules??[];
if(!declarations.length)throw new Error(`No required submodules are declared for ${options.game}/${options.variant}`);
const layout=loadSourceLayout(launcherRoot(),{environment:{...process.env,EAGLER_WORKSPACE_ROOT:workspaceRoot()}});
const owner=definition.workspaceRepository??entry.workspaceRepository,source=layout.path(owner);
const commands=runtimeSubmoduleInitCommands({source,declarations});
const quote=process.platform==='win32'
  ? value=>`'${value.replaceAll("'","''")}'`
  : value=>`'${value.replaceAll("'",`'\\''`)}'`;
for(const {cwd,args} of commands)
  process.stdout.write(`git -C ${quote(cwd)} ${args.map(quote).join(' ')}\n`);
