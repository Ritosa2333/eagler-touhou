import {build} from 'esbuild';
import {mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
const project = fileURLToPath(new URL('../', import.meta.url));
/** Bundle all published entries. The nickname worker stays beside the relay. */
export async function buildNetplayRelay(output) {
  await mkdir(output, {recursive:true});
  await build({absWorkingDir:project, entryPoints:{'netplay-relay':'src/netplay-relay.mjs',
    'nickname-worker':'src/nickname-worker.mjs','room-probe-policy':'src/room-probe-policy.mjs',
    'spectator-frame':'src/spectator-frame.mjs'}, outdir:resolve(output), outExtension:{'.js':'.mjs'},
    bundle:true, platform:'node', format:'esm', target:'node22', external:['ws'],
    logLevel:'silent'});
}
if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  if (process.argv.length > 3) throw new Error('Usage: node scripts/build.mjs [OUTPUT_DIRECTORY]');
  await buildNetplayRelay(process.argv[2]||resolve(project,'artifacts/build/server'));
}
