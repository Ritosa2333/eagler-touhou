import {resolve} from 'node:path';
import {runWasiCpp} from '../../../../tools/testing/wasi-cpp.mjs';

await runWasiCpp({source:resolve(import.meta.dirname,'test-visual.cpp')});
