const ERROR_CHANNELS=Object.freeze(['pageErrors','consoleErrors','httpErrors','protocolErrors','responseErrors','teardownErrors']);

/**
 * Audit the final browser evidence without applying scenario-specific
 * allowlists. Each channel is retained exactly as supplied in the report.
 */
export function auditBrowserFinalErrors(channels){
  if(!channels||typeof channels!=='object'||Array.isArray(channels))throw new TypeError('Final browser error channels must be an object');
  const rawChannels={};
  for(const name of ERROR_CHANNELS){
    if(!Array.isArray(channels[name]))throw new TypeError(`Final browser error channel ${name} must be an array`);
    rawChannels[name]=channels[name];
  }
  const failedChannels=ERROR_CHANNELS.filter(name=>rawChannels[name].length>0);
  return{status:failedChannels.length?'failed':'passed',channels:rawChannels,failedChannels};
}

/** Apply the final audit to a Replay record, including a late failure gate. */
export function gateReplayRecordOnFinalErrors(record,channels){
  const audit=auditBrowserFinalErrors(channels);
  record.finalErrorAudit=audit;
  if(record.playback?.scenarioEvidence){
    record.playback.scenarioEvidence.finalErrorAudit=audit.status;
    record.playback.scenarioEvidence.browserAndRuntimeErrors=audit.status==='passed'?'none':'present';
  }
  if(audit.status==='failed'){
    record.status='failed';
    if(record.playback){
      if(record.playback.scenarioStatus!==undefined)
        record.playback.preTeardownScenarioStatus??=record.playback.scenarioStatus;
      record.playback.scenarioStatus='failed';
    }
    record.error??=`Final browser error audit failed: ${audit.failedChannels.join(', ')}`;
  }
  return audit;
}
