import {discoverNodeTests} from '../../../tools/testing/discovery.mjs';
import {fileURLToPath} from 'node:url';

const testsDirectory = fileURLToPath(new URL('../tests/', import.meta.url));

export const TURN_NODE_TESTS = Object.freeze((await discoverNodeTests(testsDirectory)).map(file => 'tests/' + file));
if (!TURN_NODE_TESTS.length) throw new Error('No TURN tests found');
