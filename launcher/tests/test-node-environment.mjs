import {test} from 'vitest';
import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { mkdtemp, mkdir, rm, writeFile, readFile } from "node:fs/promises";
import {execFileSync} from 'node:child_process';
import { tmpdir } from "node:os";
import { resolve } from "node:path";
import { assertSupportedNode, isSupportedNodeVersion, requiredNodeDependencies, installedEnvironmentMatches, ensureNodeDependencies } from "../host/lib/node-environment.mjs";

test("test-node-environment", async () => {
assert.equal(isSupportedNodeVersion("v22.0.0"),true);
assert.equal(isSupportedNodeVersion("21.9.0"),false);
assert.throws(()=>assertSupportedNode("v21.9.0"),/Node\.js >= 22/);
assert.doesNotThrow(()=>assertSupportedNode("v22.0.0"));
const root=await mkdtemp(resolve(tmpdir(),"eagler-node-env-"));
await mkdir(resolve(root,"src"),{recursive:true});
await writeFile(resolve(root,"src","app.mts"),"export {};\n");
assert.ok(requiredNodeDependencies(root).includes("typescript"),"source checkout must require the TS compiler");
const pre=await mkdtemp(resolve(tmpdir(),"eagler-node-env-pre-"));
assert.ok(!requiredNodeDependencies(pre).includes("typescript"),"prebuilt bundle must not require source compiler");
const required=requiredNodeDependencies(root);
const packages={"":{}};
for(const name of required){packages[`node_modules/${name}`]={version:"1.0.0"};await mkdir(resolve(root,"node_modules",name),{recursive:true});await writeFile(resolve(root,"node_modules",name,"package.json"),JSON.stringify({name,version:"1.0.0"}));}
const lock=JSON.stringify({lockfileVersion:3,packages});
await writeFile(resolve(root,"package-lock.json"),lock);
const identity=createHash("sha256").update(lock).digest("hex");
await writeFile(resolve(root,"node_modules",".eagler-host-lock.json"),JSON.stringify({schema:"eagler-touhou/node-environment/1",lockSha256:identity,nodeMajor:Number.parseInt(process.versions.node,10),required}));
assert.equal(await installedEnvironmentMatches(root,required,identity),true);
await rm(resolve(root,"node_modules","typescript"),{recursive:true,force:true});
assert.equal(await installedEnvironmentMatches(root,required,identity),false,"source checkout without TypeScript must be refreshed");
await mkdir(resolve(root,"node_modules","typescript"),{recursive:true});
await writeFile(resolve(root,"node_modules","typescript","package.json"),JSON.stringify({name:"typescript",version:"1.0.0"}));
assert.equal(await installedEnvironmentMatches(root,required,identity),true);
packages[`node_modules/${required[0]}`].version="2.0.0";
await writeFile(resolve(root,"package-lock.json"),JSON.stringify({lockfileVersion:3,packages}));
assert.equal(await installedEnvironmentMatches(root,required,identity),false,"lock/install drift must invalidate environment");
console.log("Node environment contract: PASS");

// A real first install, without an existing stamp or registry/network inputs.
// Local package fixtures exercise the actual platform npm startup and lockfile.
const fresh=await mkdtemp(resolve(tmpdir(),'eagler-host-first install-'));
try {
  const names=requiredNodeDependencies(fresh), dependencies={};
  for(const name of names){
    const folder=resolve(fresh,'vendor',name);await mkdir(folder,{recursive:true});
    await writeFile(resolve(folder,'package.json'),JSON.stringify({name,version:'1.0.0'}));
    dependencies[name]=`file:vendor/${name}`;
  }
  await writeFile(resolve(fresh,'package.json'),JSON.stringify({name:'host-bootstrap-fixture',version:'1.0.0',private:true,dependencies}));
  await writeFile(resolve(fresh,'.npmrc'),'install-links=true\n');
  const lockArgs=['install','--package-lock-only','--ignore-scripts','--offline','--no-audit','--no-fund'];
  if(process.platform==='win32')execFileSync(process.env.ComSpec||'cmd.exe',['/d','/s','/c','npm.cmd '+lockArgs.join(' ')],{cwd:fresh,windowsHide:true,stdio:'pipe'});
  else execFileSync('npm',lockArgs,{cwd:fresh,stdio:'pipe'});
  await ensureNodeDependencies(fresh);
  const stamp=JSON.parse(await readFile(resolve(fresh,'node_modules','.eagler-host-lock.json'),'utf8'));
  assert.equal(await installedEnvironmentMatches(fresh,names,stamp.lockSha256),true);
  await ensureNodeDependencies(fresh);
  console.log('Actual first Host dependency install, spaced cwd, locked local packages and warm stamp: PASS');
} finally {
  assert.equal(resolve(fresh,'..'),resolve(tmpdir()));
  await rm(fresh,{recursive:true,force:true});
}
});
