import {test} from 'vitest';
import {assertServerHttpContract} from './support/server-http-contract.mjs';

test('server', assertServerHttpContract);
