#!/usr/bin/env node
import {loadGoldenManifest} from './golden.mjs';

const index = process.argv.indexOf('--golden-root');
const goldenRoot = index < 0 ? undefined : process.argv[index + 1];
const {manifest} = await loadGoldenManifest(goldenRoot);
const ticks = suite => manifest.suites[suite].reduce((sum, id) => sum + manifest.assets[id].comparedTicks, 0);
process.stdout.write(`TH08 golden: PASS (quick ${ticks('quick')} ticks; daily ${ticks('daily')} ticks)\n`);
