import assert from 'node:assert/strict';
import {assertPackageImportParity} from '../../../tools/ports/package-import-parity.mjs';
import {classifyStaticDiscoveryEvents} from '../../../tools/ports/offline-runtime-https.mjs';

export function assessPackageImportEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/offline-runtime-browser/1'||entry?.id!=='PACKAGE-04'||!driver.importParity)return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'package-import-source-parity',checks:['PACKAGE-04']};
  try{
    assert.equal(product.variant,'normal');assert.equal(driver.scope.game,product.game);assert.equal(driver.scope.variant,product.variant);
    assert.equal(driver.checks.filter(row=>row.id===entry.id).length,1);assert.ok(driver.checks.includes(entry));assert.equal(entry.status,'passed');assert.equal(entry.complete,true);
    assert.equal(driver.finalErrorAudit.status,'passed');assert.deepEqual(driver.teardownErrors,[]);
    assert.deepEqual(driver.harnessSourcesPostflight,driver.harnessSources);assert.ok(Object.keys(driver.harnessSources).length>10);
    for(const [key,value]of Object.entries(driver.runtimePackages.normal))assert.deepEqual(driver.postflight.normalRuntimePackage[key],value);
    assert.deepEqual(driver.postflight.privateData,driver.runtimePackages.data);
    const expected=driver.importParityInputs,envelope=driver.importParity;
    assert.equal(expected.game,product.game);assert.deepEqual(expected.expectedData,driver.runtimePackages.data);assert.equal(expected.expectedLayout,driver.runtimePackages.dataLayout);
    assert.deepEqual(expected.archiveIdentity,driver.postflight.archive);
    for(const key of ['bytes','sha256'])assert.equal(expected.archiveIdentity[key],driver.importPublication.fixture.sourceArchive[key]);
    assert.equal(expected.publishedRevision,driver.importPublication.fixture.revision);
    assert.equal(expected.runtimeGeneration,driver.packagedSite.normalRuntime.generation);
    assert.equal(envelope.game,product.game);assert.equal(envelope.variant,product.variant);assert.deepEqual(envelope.adaptationIds,['PACKAGE-04']);
    assert.deepEqual(driver.importPublication.postflight,driver.importPublication.staticSite);
    assert.deepEqual(driver.staticSitePostflight,driver.server.staticSite);
    const events=classifyStaticDiscoveryEvents(driver.browserEvents,driver.browserSiteResponses.flatMap(group=>group.responses));
    assert.deepEqual(events.blocking,[]);assert.deepEqual(events.expected,driver.expectedImportDiscovery);
    for(const row of envelope.observation.routes){
      const origin=new URL(row.appUrl).origin,site=row.source==='online'?driver.importPublication:row.source==='original-file'?driver.rawImportPublication:driver.server;
      assert.equal(origin,new URL(site.baseUrl).origin);assert.equal(new URL(row.runtime.frameUrl).origin,origin);assert.equal(new URL(row.reopened.runtime.frameUrl).origin,origin);
      if(row.source==='original-file'){
        assert.deepEqual(site.postflight,site.staticSite);assert.equal(site.deployment.resourceMode,'hosted');assert.equal(site.fixture.catalog,null);
      }
    }
    assertPackageImportParity(envelope.observation,expected);result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
