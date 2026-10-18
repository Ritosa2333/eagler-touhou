import {readdirSync} from 'node:fs';

// The standalone check and consumers' capability gates share this inventory.
export const RELAY_NODE_TESTS = Object.freeze(
  readdirSync(new URL('../tests/', import.meta.url))
    .filter(name => name.endsWith('.mjs')).sort().map(name => 'tests/' + name),
);
