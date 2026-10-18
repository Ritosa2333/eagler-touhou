import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {existsSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {TH08_PRESENTATION_LAB_EXPORTS} from './native-abi.mjs';

const root=resolve(import.meta.dirname,'../..');
const diagnosticBuildRoot=resolve(root,'th08_web/artifacts/presentation-lab');
const productionBuildRoot=resolve(root,'th08_web/artifacts/sdl3');
const diagnosticRuntime=resolve(root,'artifacts/presentation-lab/runtime');
assert.notEqual(diagnosticBuildRoot,productionBuildRoot);
assert.notEqual(diagnosticRuntime,resolve(root,'build-eagler'));

const sha256=bytes=>createHash('sha256').update(bytes).digest('hex');
const readBuild=directory=>JSON.parse(readFileSync(resolve(directory,'build.json'),'utf8'));
const wasmExports=path=>new Set(WebAssembly.Module.exports(new WebAssembly.Module(readFileSync(path))).map(entry=>entry.name));
const diagnostic=readBuild(diagnosticBuildRoot),production=readBuild(productionBuildRoot);
assert.equal(diagnostic.profile,'presentation-lab');
assert.equal(diagnostic.diagnostic,true);
assert.notEqual(production.profile,'presentation-lab');
assert.notEqual(production.diagnostic,true);

const diagnosticWasm=resolve(diagnosticBuildRoot,'th08-sdl.wasm');
const productionWasm=resolve(productionBuildRoot,'th08-sdl.wasm');
assert.equal(sha256(readFileSync(diagnosticWasm)),diagnostic.sha256);
assert.equal(sha256(readFileSync(productionWasm)),production.sha256);
const diagnosticNames=wasmExports(diagnosticWasm),productionNames=wasmExports(productionWasm);
for(const name of TH08_PRESENTATION_LAB_EXPORTS)assert(diagnosticNames.has(name),`diagnostic WASM is missing ${name}`);
for(const name of productionNames)assert(!name.startsWith('audit_')&&!name.startsWith('presentation_lab_'),`production WASM exposes ${name}`);

const manifest=JSON.parse(readFileSync(resolve(diagnosticRuntime,'manifest.json'),'utf8'));
assert.equal(manifest.profile,'presentation-lab');
assert.equal(manifest.execution.sha256,diagnostic.sha256);
assert.equal(sha256(readFileSync(resolve(diagnosticRuntime,'th08-sdl.wasm'))),diagnostic.sha256);
const inventory=JSON.parse(readFileSync(resolve(diagnosticRuntime,'runtime-files.json'),'utf8')).files;
for(const [name,entry] of Object.entries(inventory))assert.equal(sha256(readFileSync(resolve(diagnosticRuntime,name))),entry.sha256,`stale diagnostic runtime file ${name}`);

const browserSources=['adapter.mjs','app.mjs','run-campaign.py'].map(name=>readFileSync(resolve(import.meta.dirname,name),'utf8')).join('\n');
const called=new Set([...browserSources.matchAll(/(?:(?:this|controller)\.core|(?<![-\w])core)\.([A-Za-z_]\w*)/g)].map(match=>match[1]));
called.delete('memory');
for(const name of called)assert(TH08_PRESENTATION_LAB_EXPORTS.includes(name),`browser code uses undeclared native export ${name}`);

assert(existsSync(resolve(diagnosticRuntime,'runtime-files.json')));
console.log(`TH08 Presentation Lab release boundary PASS: ${diagnostic.sha256}`);
