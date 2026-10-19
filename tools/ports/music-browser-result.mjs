/** Preserve every error channel collected during the selected music journey. */
export function musicScenarioErrors(record,{fixtureFailures=[]}={}){
  const errors=[];
  const append=(channel,values,mode)=>{
    for(const value of values??[])errors.push({channel,...(mode?{mode}:{}),value});
  };
  append('page-or-scene',record.errors);append('console',record.consoleErrors);
  append('http-or-request',record.httpErrors);append('fixture-server',fixtureFailures);
  for(const mode of record.modes??[]){
    if(mode.responseAuditError)append('response-audit',[mode.responseAuditError],mode.mode);
    append('response-identity',mode.fixtureResponses?.errors,mode.mode);
    append('runtime-protocol',(mode.events??[]).filter(event=>event&&
      (event.event==='error'||event.event==='abort'||event.ok===false)),mode.mode);
  }
  return errors;
}

/** Report the executed title scene separately from AUDIO-01 full-scope coverage. */
export function musicTitleSceneOutcome({oggPcmObserved,expectedTrack=null,observedTrack=null,trackInput=null,errors=[]}={}) {
  const trackIdentityPassed=expectedTrack==null||
    (observedTrack===expectedTrack&&trackInput?.name===expectedTrack&&typeof trackInput.sha256==='string');
  const passed=oggPcmObserved===true&&trackIdentityPassed&&errors.length===0;
  return {
    scenario:{id:'title-ogg-output',status:passed?'passed':'failed',passed,
      scope:'Runtime title output after OGG configure and launch'},
    coverage:{id:'AUDIO-01',status:passed?'partial':'failed',complete:false,
      scope:'Full track switching, gameplay scene changes, looping and lifecycle coverage remain unrun'},
    trackIdentity:expectedTrack==null?'not-exposed-by-native-stats':trackIdentityPassed?'verified':'failed',
  };
}
