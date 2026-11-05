// Test control only. The native mode is chosen after configure and before the
// first real input, then remains locked for this session and its next runs.
// Deep information pacing and the experimental extra Draw are independent.
export const networkGapTailStatusSchema=['schema','enabled','locked','gapScale','phaseScale','effectiveScale'];

export function readNetworkGapTailStatus(core,app){
 if(!core.multiplayer_network_gap_tail_status)return null;
 const pointer=core.multiplayer_network_gap_tail_status(app);
 if(!Number.isInteger(pointer)||pointer<=0||pointer%8||pointer+48>core.memory.buffer.byteLength)
  throw Error('Invalid network gap tail status pointer');
 const value=Array.from(new Float64Array(core.memory.buffer,pointer,6));
 if(value[0]!==1||![0,1].includes(value[1])||![0,1].includes(value[2])||
    value.some(v=>!Number.isFinite(v))||value.slice(3).some(v=>v<=0))
  throw Error('Invalid network gap tail status');
 return value;
}

export function configureNetworkGapTail(core,app,enabled){
 if(enabled===null||enabled===undefined)return readNetworkGapTailStatus(core,app);
 if(typeof enabled!=='boolean')throw Error('Network gap tail option must be boolean');
 if(!core.multiplayer_network_gap_tail_set_enabled||!core.multiplayer_network_gap_tail_status)
  throw Error('Network gap tail controls required by explicit option');
 const before=readNetworkGapTailStatus(core,app);
 if(before[2]!==0)throw Error('Network gap tail must be chosen before the first real input');
 if(core.multiplayer_network_gap_tail_set_enabled(app,Number(enabled))!==1)
  throw Error('Network gap tail option rejected');
 const after=readNetworkGapTailStatus(core,app);
 if(after[1]!==Number(enabled)||after[2]!==0)throw Error('Network gap tail setup did not take effect');
 return after;
}
export const networkTailPresentationStatusSchema=['schema','enabled','locked'];

export function readNetworkTailPresentationStatus(core,app){
 if(!core.multiplayer_network_tail_presentation_status)return null;
 const pointer=core.multiplayer_network_tail_presentation_status(app);
 if(!Number.isInteger(pointer)||pointer<=0||pointer%8||pointer+24>core.memory.buffer.byteLength)
  throw Error('Invalid network tail presentation status pointer');
 const value=Array.from(new Float64Array(core.memory.buffer,pointer,3));
 if(value[0]!==1||![0,1].includes(value[1])||![0,1].includes(value[2]))
  throw Error('Invalid network tail presentation status');
 return value;
}

export function configureNetworkTailPresentation(core,app,enabled){
 if(enabled===null||enabled===undefined)return readNetworkTailPresentationStatus(core,app);
 if(typeof enabled!=='boolean')throw Error('Network tail presentation option must be boolean');
 if(!core.multiplayer_network_tail_presentation_set_enabled||!core.multiplayer_network_tail_presentation_status)
  throw Error('Independent network tail presentation controls required by explicit option');
 const before=readNetworkTailPresentationStatus(core,app);
 if(before[2]!==0)throw Error('Network tail presentation must be chosen before the first real input');
 if(core.multiplayer_network_tail_presentation_set_enabled(app,Number(enabled))!==1)
  throw Error('Network tail presentation option rejected');
 const after=readNetworkTailPresentationStatus(core,app);
 if(after[1]!==Number(enabled)||after[2]!==0)throw Error('Network tail presentation setup did not take effect');
 return after;
}
