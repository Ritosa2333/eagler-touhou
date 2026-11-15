import createModule from './archive-decoder.mjs';
import {createArchiveDecodeEndpoint} from './Th14ArchiveDecodeEndpoint.mjs';

const accept=createArchiveDecodeEndpoint(createModule,(message,transfer=[])=>self.postMessage(message,transfer));
self.addEventListener('message',event=>accept(event.data));
