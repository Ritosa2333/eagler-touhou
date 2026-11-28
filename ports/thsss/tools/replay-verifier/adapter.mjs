import {dirname, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {describeReplay} from '../../THSSS.Browser/wwwroot/host/replay/describe.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const repo = resolve(here, '../..');
const common = resolve(process.env.EAGLER_COMMON_ROOT || resolve(repo, '../../common'));
const {createFileAdapter} = await import(pathToFileURL(resolve(common, 'testkit/replay-verifier/file-adapter.mjs')));

export default createFileAdapter({
  repository: repo,
  game: 'thsss',
  inspectReplay: bytes => describeReplay(bytes),
  originalScript: 'tools/replay-verifier/capture-original.mjs',
  candidateScript: 'tools/replay-verifier/capture-candidate.mjs',
});
