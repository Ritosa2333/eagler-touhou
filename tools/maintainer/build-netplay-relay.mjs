import {build} from 'esbuild';
import {mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {authoredSourcesPlugin} from '../../lib/authored-source-resolution.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
/** Keep both entry points together: worker URLs are resolved beside the relay. */
export async function buildNetplayRelay(output) {
  await mkdir(output, {recursive:true});
  await build({absWorkingDir:project, entryPoints:{'netplay-relay':'server/netplay-relay.mjs',
    'nickname-worker':'server/nickname-worker.mjs'}, outdir:resolve(output), outExtension:{'.js':'.mjs'},
    bundle:true, platform:'node', format:'esm', target:'node22', external:['ws'],
    plugins:[authoredSourcesPlugin(project)], logLevel:'silent'});
}
if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  if (process.argv.length !== 3) throw new Error('Usage: node tools/maintainer/build-netplay-relay.mjs OUTPUT_DIRECTORY');
  await buildNetplayRelay(process.argv[2]);
}
