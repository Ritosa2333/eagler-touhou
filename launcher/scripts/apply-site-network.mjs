// Apply operator routing to an inactive, verified publication candidate.
import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {fileURLToPath} from 'node:url';
import {buildAppShell} from '../lib/app-shell-build.mjs';
import {verifyReleaseManifest, writeReleaseManifest} from '../lib/release-manifest.mjs';

const args = Object.fromEntries(process.argv.slice(2).map(arg => {
  const match = /^--(site|config|worker-contract)=(.+)$/.exec(arg);
  if (!match) throw new Error('usage: apply-site-network.mjs --site=INACTIVE_CANDIDATE --config=FILE --worker-contract=FILE');
  return [match[1],match[2]];
}));
if (!args.site || !args.config || !args['worker-contract']) throw new Error('Missing site network publication argument');
const site = resolve(args.site);
const previous = await verifyReleaseManifest(site);
const deployment = JSON.parse(await readFile(resolve(site,'deployment.json')));
if (!deployment.appShell) throw new Error('Site network recovery requires an enabled App Shell');
const config = await readFile(resolve(args.config));
const workerContractSource = await readFile(resolve(args['worker-contract']),'utf8');
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const sourceFiles = ['scripts/apply-site-network.mjs','lib/app-shell-build.mjs','lib/app-shell-contract.mjs',
  'src/site-mirror-sw.js','src/app-shell-sw.js','src/runtime-cache-sw.js','compat/runtime-generation-reader.js'];
const project = fileURLToPath(new URL('../', import.meta.url));
const files = await Promise.all(sourceFiles.map(async path => ({path,sha256:sha(await readFile(resolve(project,path)))})));
const tooling = sha(JSON.stringify({files,contract:sha(workerContractSource)}));
await writeFile(resolve(site,'site-network.json'),config);
const built = await buildAppShell({globDirectory:site,swDest:resolve(site,'app-shell-sw.js'),
  shellFiles:deployment.appShell.entries.filter(path => path !== './'),workerContractSource,quiet:true});
deployment.appShell = built.contract;
deployment.generatedAt = new Date().toISOString();
for (const path of ['site-network.json','app-shell-sw.js']) {
  const bytes = await readFile(resolve(site,path));
  const identity = {path,bytes:bytes.length,sha256:sha(bytes)};
  const entry = deployment.files.find(file => file.path === path);
  if (entry) Object.assign(entry,identity); else deployment.files.push(identity);
}
deployment.files.sort((a,b) => a.path.localeCompare(b.path,'en'));
await writeFile(resolve(site,'deployment.json'),JSON.stringify(deployment,null,2)+'\n');
const release = await writeReleaseManifest(site,{profile:previous.profile,
  sources:{...previous.sources,siteNetworkTooling:{files,sha256:tooling}},
  parameters:{...previous.parameters,siteNetwork:{tooling,configuration:sha(config),sourceRelease:previous.releaseId}}});
await verifyReleaseManifest(site);
console.log(JSON.stringify({releaseId:release.releaseId,buildId:built.buildId,files:release.files.length,tooling}));
