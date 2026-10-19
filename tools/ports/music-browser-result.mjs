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
