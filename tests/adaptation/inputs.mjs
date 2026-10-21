import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {dirname,resolve} from 'node:path';

export const INPUT_ENVIRONMENT=Object.freeze({
  runtimeManifest:'EAGLER_TEST_RUNTIME_MANIFEST',packages:'EAGLER_TEST_PACKAGES',
  updatePackages:'EAGLER_TEST_UPDATE_PACKAGES',musicManifest:'EAGLER_TEST_MUSIC_MANIFEST',
  artwork:'EAGLER_TEST_ARTWORK',launcherUrl:'EAGLER_TEST_LAUNCHER_URL',
  diagnosticManifest:'EAGLER_TEST_DIAGNOSTIC_MANIFEST',
  languageManifest:'EAGLER_TEST_LANGUAGE_MANIFEST',
  sourceReview:'EAGLER_TEST_SOURCE_REVIEW',
  minimalRuntime:'EAGLER_TEST_MINIMAL_RUNTIME',
});

/** Local paths live in one user-owned configuration, outside the test catalog. */
export async function adaptationEnvironment(path,environment=process.env){
  const result={...environment};
  if(!path)return result;
  const absolute=resolve(path),config=JSON.parse(await readFile(absolute,'utf8'));
  assert.equal(config.schema,'eagler-touhou/adaptation-inputs/1','Unsupported adaptation inputs schema');
  for(const [name,value] of Object.entries(config)){
    if(name==='schema')continue;
    assert.ok(Object.hasOwn(INPUT_ENVIRONMENT,name),`Unknown adaptation input: ${name}`);
    assert.ok(typeof value==='string'&&value.trim(),`Adaptation input ${name} must be a nonempty string`);
    if(name==='launcherUrl'){
      const url=new URL(value);assert.ok(['http:','https:'].includes(url.protocol),'Launcher URL must use HTTP or HTTPS');
      result[INPUT_ENVIRONMENT[name]]=url.href;
    }else result[INPUT_ENVIRONMENT[name]]=resolve(dirname(absolute),value);
  }
  return result;
}
