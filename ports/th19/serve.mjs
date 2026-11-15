import {fileURLToPath} from 'node:url';
import {startServer} from './scripts/static-server.mjs';
startServer({routes:[{prefix:'/',directory:fileURLToPath(new URL('.',import.meta.url))}]});
