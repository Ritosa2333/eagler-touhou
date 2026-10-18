import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {inspectReplay} from './inspect-replay.mjs';
const repository=fileURLToPath(new URL('../../',import.meta.url));
const common=resolve(process.env.EAGLER_COMMON_ROOT||fileURLToPath(new URL('../../../eagler-common',import.meta.url)));
const {createFileAdapter}=await import(pathToFileURL(resolve(common,'testkit/replay-verifier/file-adapter.mjs')));
export default createFileAdapter({repository,game:'th15',inspectReplay,originalScript:'tools/replay-verifier/capture-original.mjs',candidateScript:'tools/replay-verifier/capture-candidate.mjs'});
