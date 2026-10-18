// Browser acceptance follows the canonical Launcher, including its managed DATA.
// EAGLER_SDL_FIXTURE and TH_PLAYWRIGHT point to local/private validation inputs.
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
if(!process.env.EAGLER_LAUNCHER_ROOT)throw Error('Set EAGLER_LAUNCHER_ROOT to the canonical Launcher adapter checkout');
process.env.TH_GAME??='th10';
await import(pathToFileURL(resolve(process.env.EAGLER_LAUNCHER_ROOT,'tests/browser/test-sdl-adapters.mjs')).href);
