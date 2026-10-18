// Observe the shared source used by this build, without accepting one fixed
// upstream commit forever. Component trees remain stable across sibling edits.
import {execFileSync} from 'node:child_process';
import {resolve, relative} from 'node:path';
import {sourceIdentity} from '../../../launcher/lib/release-manifest.mjs';
export const COMMON_ROOT = resolve(process.env.EAGLER_COMMON_ROOT || resolve(import.meta.dirname, '../../../common'));
export const COMMON_SOURCE = await sourceIdentity(COMMON_ROOT);
const git = args => execFileSync('git', ['-C', COMMON_ROOT, ...args], {encoding: 'utf8', windowsHide: true}).trim();
const repository = git(['rev-parse', '--show-toplevel']);
export const COMMON_SOURCE_PATH = relative(repository, COMMON_ROOT).replaceAll('\\', '/') || '.';
export const COMMON_COMMIT = git(['rev-parse', 'HEAD']);
export const COMMON_REVISION = git(['rev-parse', COMMON_SOURCE_PATH === '.' ? 'HEAD^{tree}' : 'HEAD:' + COMMON_SOURCE_PATH]);
