import {readFileSync, writeFileSync} from 'node:fs';
const module = new URL('../browser/adaptive-peer-route.mjs', import.meta.url);
const target = new URL('../src/netplay/BrowserPeerTransport.cpp', import.meta.url);
const start = '        // BEGIN generated adaptive-peer-route\n';
const end = '        // END generated adaptive-peer-route\n';
const input = readFileSync(target, 'utf8').replaceAll('\r\n', '\n');
const begin = input.indexOf(start), finish = input.indexOf(end, begin);
if (begin < 0 || finish < 0) throw new Error('Adaptive route embedding markers are missing');
const body = readFileSync(module, 'utf8').replaceAll('\r\n', '\n').replace('export function', 'function').trimEnd();
const output = input.slice(0, begin) + start + body.split('\n').map(line => line ? '        ' + line : '').join('\n') + '\n' + input.slice(finish);
if (process.argv.includes('--check')) {
    if (output !== input) throw new Error('Run node common/tools/sync-adaptive-peer-route.mjs');
} else writeFileSync(target, output);
