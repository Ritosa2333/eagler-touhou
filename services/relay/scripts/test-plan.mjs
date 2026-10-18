import {discoverNodeTests} from '../../../tools/testing/discovery.mjs';
import {fileURLToPath} from 'node:url';

const testsDirectory = fileURLToPath(new URL('../tests/', import.meta.url));

// npm test and the standalone capability gate use the same recursive test
// inventory; fixture programs are never treated as executable tests.
export const RELAY_NODE_TESTS = Object.freeze((await discoverNodeTests(testsDirectory)).map(file => 'tests/' + file));
if (!RELAY_NODE_TESTS.length) throw new Error('No Relay tests found');
