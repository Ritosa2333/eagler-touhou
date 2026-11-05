import {discoverNodeTests} from '../../../tools/testing/discovery.mjs';
import {fileURLToPath} from 'node:url';

// The standalone check and consumers' capability gates share this inventory.
export const RELAY_NODE_TESTS = Object.freeze((await discoverNodeTests(fileURLToPath(new URL('../tests/', import.meta.url))))
  .map(name => 'tests/' + name));
if (!RELAY_NODE_TESTS.length) throw Error('No Relay tests found');
