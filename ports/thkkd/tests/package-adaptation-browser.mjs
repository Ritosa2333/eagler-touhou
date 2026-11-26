import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,runtimeCommand} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';
import {authoredSourcesPlugin} from '../../../launcher/lib/authored-source-resolution.mjs';

const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const {build}=require('esbuild');
const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const output=resolve(args.output||'ports/thkkd/build/adaptation-20261127/package');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/package-adaptation-browser/1',checks:[]};
const bundle=await build({stdin:{contents:"export * from './package/package-store.mjs'; export {installPackageFromAcquisition} from './package/package-installer.mjs';",resolveDir:resolve('launcher')},bundle:true,format:'esm',platform:'browser',write:false,logLevel:'silent',plugins:[authoredSourcesPlugin(resolve('launcher'))]});
const moduleURL='data:text/javascript;base64,'+Buffer.from(bundle.outputFiles[0].text).toString('base64');
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:19027/',profile:args.profile,report});const {browser,page}=driver;
const expectedBuildId=JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId;
try{
  await page.setBypassServiceWorker(true);await page.setCacheEnabled(false);await page.setBypassCSP(true);await driver.library();await driver.start({expectedBuildId});
  const config=[...new TextEncoder().encode('01100\n')],replay=[...replayFixture({count:420})];
  await runtimeCommand(page,'write',{path:'config.txt',bytes:config});await runtimeCommand(page,'write',{path:'replay/package-check.rpy',bytes:replay});await runtimeCommand(page,'sync');
  const saved=await runtimeCommand(page,'read',{path:'th_kaikeidou01.dat'});await driver.library();
  report.install=await page.evaluate(async moduleURL=>{
    const api=await import(moduleURL),before=await api.readCurrentPackageGeneration('thkkd');
    if(!before.generation)throw Error('No real imported thkkd Package');
    const files=Object.keys(before.generation.files),objects=new Map();
    for(const id of files)objects.set(id,await api.readPackageObject(before.generation.files[id].objectId));
    const descriptor=structuredClone(before.generation.descriptor);descriptor.revision='adaptation-'+crypto.randomUUID();
    for(const id of files)descriptor.files[id].revision='adaptation-file-'+crypto.randomUUID();
    const controller=new AbortController();let aborted=false;
    try{await api.installPackageFromAcquisition({descriptor,desiredFileIds:files,source:'local',reuseCurrent:false,expectedGenerationId:before.generation.id,signal:controller.signal,onProgress:()=>controller.abort(),acquire:async id=>{controller.abort();return objects.get(id).blob;}});}catch(error){aborted=controller.signal.aborted;}
    const interrupted=await api.readCurrentPackageGeneration('thkkd');
    if(!aborted||interrupted.generation.id!==before.generation.id)throw Error('Interrupted import changed current generation');
    await api.installPackageFromAcquisition({descriptor,desiredFileIds:files,source:'local',reuseCurrent:false,expectedGenerationId:before.generation.id,acquire:async id=>objects.get(id).blob});
    const after=await api.readCurrentPackageGeneration('thkkd');
    if(after.generation.id===before.generation.id||after.generation.descriptor.revision!==descriptor.revision)throw Error('Complete import did not activate new generation');
    for(const id of files){const object=await api.readPackageObject(after.generation.files[id].objectId);if(object.bytes!==objects.get(id).bytes)throw Error('Resource object size changed');}
    return {interruptedRetained:before.generation.id===interrupted.generation.id,switchedAfterComplete:before.generation.id!==after.generation.id,files:files.length,resourceBytes:[...objects.values()].reduce((sum,object)=>sum+object.bytes,0)};
  },moduleURL);
  report.checks.push('PACKAGE-03-interrupted-real-resource-install-keeps-current-generation','PACKAGE-03-resume-activates-complete-resource-generation');
  await driver.library();await driver.start({expectedBuildId});
  assert.deepEqual((await runtimeCommand(page,'read',{path:'th_kaikeidou01.dat'})).bytes,saved.bytes);
  assert.deepEqual((await runtimeCommand(page,'read',{path:'config.txt'})).bytes,config);
  assert.deepEqual((await runtimeCommand(page,'read',{path:'replay/package-check.rpy'})).bytes,replay);
  report.checks.push('SAVE-06-resource-generation-replacement-preserves-native-save-config-and-replay','PACKAGE-03-reload-and-launch-after-replacement');
  await driver.collect();assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks,install:report.install}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
