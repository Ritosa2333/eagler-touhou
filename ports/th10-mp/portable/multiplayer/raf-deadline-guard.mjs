// Test-only startup control. Native OFF retains the original Emscripten loop.
// A Module locks its selected mode on the first loop start, even after stop.
// Both native schemas have the same 32 fields. v1 permits one timeout per RAF;
// v2 rearms after full completion. compensationUsed remains a boolean meaning
// at least one consumed timer attempt for that RAF, not a v2 service limit.
export const rafDeadlineGuardStatusSchema=[
 'schema','enabled','locked','running','loopEpoch','requestSerial','pendingRaf',
 'timerArmed','compensationUsed','callbackActive','lastSource','lastDecision',
 'rafRequests','rafArrivals','rafServices','timerArrivals','timerServices',
 'staleArrivals','nearSkips','timestampSkips','eligibilitySkips',
 'lastRafTimestampMs','lastAcceptedTimestampMs','lastCallbackCompletedMs',
 'lastRequestAtMs','lastDeadlineMs','lastArrivalAtMs','lastTimerLatenessMs',
 'maxTimerLatenessMs','lastRealRafDeltaMs','callbackSequence','skippedRafSamples'];

export function readRafDeadlineGuardStatus(core){
 if(!core.sdl_raf_guard_status)return null;
 const pointer=core.sdl_raf_guard_status();
 const memory=core.memory.buffer;
 if(!Number.isInteger(pointer)||pointer<=0||pointer%8||pointer+256>memory.byteLength)
  throw Error('Invalid RAF deadline guard status pointer');
 const value=Array.from(new Float64Array(memory,pointer,32));
 if(![1,2].includes(value[0])||value.some(v=>!Number.isFinite(v))||
    [1,2,3,6,7,8,9].some(i=>value[i]!==0&&value[i]!==1)||
    [4,5,10,11,12,13,14,15,16,17,18,19,20,30,31].some(i=>!Number.isSafeInteger(value[i])||value[i]<0)||
    value[10]>2||value[11]>10||value.slice(21,27).some(v=>v< -1)||value[28]<0||value[29]<0)
  throw Error('Invalid RAF deadline guard status');
 return value;
}

export function configureRafDeadlineGuard(core,enabled){
 if(enabled===null||enabled===undefined)return readRafDeadlineGuardStatus(core);
 if(typeof enabled!=='boolean')throw Error('RAF deadline guard option must be boolean');
 if(!core.sdl_raf_guard_set_enabled||!core.sdl_raf_guard_status)
  throw Error('RAF deadline guard controls required by explicit option');
 const before=readRafDeadlineGuardStatus(core);
 if(before[2]!==0||before[3]!==0)
  throw Error('RAF deadline guard must be chosen before the first loop start');
 if(core.sdl_raf_guard_set_enabled(Number(enabled))!==Number(enabled))
  throw Error('RAF deadline guard option rejected');
 const after=readRafDeadlineGuardStatus(core);
 if(after[1]!==Number(enabled)||after[2]!==0||after[3]!==0)
  throw Error('RAF deadline guard setup did not take effect');
 return after;
}
