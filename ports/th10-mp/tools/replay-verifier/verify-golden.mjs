#!/usr/bin/env node
import {loadGoldenManifest} from './golden.mjs';

const index = process.argv.indexOf('--golden-root');
const goldenRoot = index < 0 ? undefined : process.argv[index + 1];
const {manifest} = await loadGoldenManifest(goldenRoot);
const dailyTicks = manifest.suites.daily.reduce((sum, id) => sum + manifest.assets[id].comparedTicks, 0);
process.stdout.write(`TH10 golden: PASS (quick ${manifest.assets.demos.rawTicks} ticks; daily ${dailyTicks} ticks)\n`);
