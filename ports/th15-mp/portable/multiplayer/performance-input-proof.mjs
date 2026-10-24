// Post-window evidence rules. Production input, capture and Replay ownership
// stay with the native runtime; this module only checks completed observations.
function require(value,message){if(!value)throw Error(message);}
function uint(value){return Number.isInteger(value)&&value>=0&&value<0xffffffff;}
export function proofState(snapshot){
  const net=snapshot?.net,rollback=snapshot?.rollback;
  require(net?.length>=24&&rollback?.length>=20,'Missing native input-proof snapshot');
  require(snapshot.nativePhase===1&&!snapshot.error&&net[1]&&net[2]&&!net[13]&&!net[23],'Input proof left the active gameplay session');
  const state={players:net[5],seat:net[6],delay:net[7],generation:net[12],capture:rollback[3],committed:rollback[4]};
  require([2,3].includes(state.players)&&state.seat<state.players&&uint(state.capture)&&uint(state.delay)&&uint(state.generation),'Invalid native input-proof frontier');
  return state;
}
export function assertProofState(snapshot,reference){
  const state=proofState(snapshot);
  for(const field of ['players','seat','delay','generation'])require(state[field]===reference[field],'Input proof changed '+field);
  require(state.capture>=reference.capture,'Input proof capture frontier regressed');return state;
}
export function neutralPlan(snapshots){
  const references=snapshots.map(proofState),players=references.length;
  require([2,3].includes(players)&&references.every((state,seat)=>state.players===players&&state.seat===seat),'Input proof seat ownership mismatch');
  const first=Math.max(...references.map(state=>state.capture+state.delay));
  return {players,references,first,end:first+2};
}
export function committedHorizon(snapshots,plan,end){
  require(uint(end)&&end>0&&snapshots.length===plan.players,'Invalid required input-proof horizon');
  return snapshots.every((snapshot,seat)=>{const state=assertProofState(snapshot,plan.references[seat]);return uint(state.committed)&&state.committed>=end-1;});
}
export function assertBeforeDeadline(now,deadline,phase){
  require(Number.isFinite(now)&&Number.isFinite(deadline)&&now<deadline,phase+' deadline expired');
}
export async function awaitBeforeDeadline(promise,deadline,phase){
  assertBeforeDeadline(performance.now(),deadline,phase);let timer;
  try{const result=await Promise.race([promise,new Promise((resolve,reject)=>{
    timer=setTimeout(()=>reject(Error(phase+' deadline expired')),Math.max(0,deadline-performance.now()));
  })]);assertBeforeDeadline(performance.now(),deadline,phase);return result;}finally{clearTimeout(timer);}
}
function replay(bytes){
  require(bytes instanceof Uint8Array&&bytes.length>=40,'Missing native-confirmed Replay bytes');
  const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),word=at=>view.getUint32(at,true);
  require(new TextDecoder().decode(bytes.subarray(0,8))==='EAGLRPY1'&&word(8)===1&&word(12)===15,'Unexpected input-proof Replay format');
  const players=bytes[20],frames=word(32),prefix=40+word(24)+8*word(28);
  require([2,3].includes(players)&&prefix+frames*players*12===bytes.length,'Invalid input-proof Replay extent');
  return {bytes,view,players,frames,prefix,offset:(frame,seat)=>prefix+(frame*players+seat)*12};
}
function covered(buffers,players,end){
  require(buffers.length===players,'Missing input-proof peer Replay');
  const parsed=buffers.map(replay);
  for(const value of parsed)require(value.players===players&&value.frames>=end,'Confirmed Replay does not cover the required proof interval');
  const reference=parsed[0],length=end*players*12;
  for(const value of parsed.slice(1))for(let at=0;at<length;++at)
    require(value.bytes[value.prefix+at]===reference.bytes[reference.prefix+at],'Input-proof all-seat confirmed prefixes disagree');
  return parsed;
}
export function proveNeutral(buffers,plan){
  const parsed=covered(buffers,plan.players,plan.end),value=parsed[0];
  require(plan.end===plan.first+2,'Neutral proof must contain two consecutive frames');
  for(let frame=plan.first;frame<plan.end;++frame)for(let seat=0;seat<plan.players;++seat){
    const at=value.offset(frame,seat);
    for(let byte=0;byte<12;++byte)require(value.bytes[at+byte]===0,'Native-confirmed neutral baseline contains input');
  }
  return {first:plan.first,end:plan.end,players:plan.players,framesPerSeat:2,allSeatsNeutral:true,
    confirmedFrames:parsed.map(value=>value.frames)};
}
export function fireHorizon(hosts,plan){
  require(hosts.length===plan.players,'Missing concurrent Host evidence');
  return Math.max(...hosts.map((host,seat)=>{
    assertProofState(host.before,plan.references[seat]);
    const after=assertProofState(host.afterInput,plan.references[seat]);return after.capture+after.delay;
  }))+2;
}
export function proveFire(buffers,plan,hosts,end){
  // Recheck the same immutable neutral prefix inside the later saved Replay.
  proveNeutral(buffers,plan);const parsed=covered(buffers,plan.players,end),value=parsed[0];
  require(hosts.length===plan.players&&end>=fireHorizon(hosts,plan),'Missing post-ACK capture coverage');
  const seats=hosts.map((host,seat)=>{
    require(!host.errors?.length&&host.inputAckBeforeSync===true,'Input ACK did not precede the concurrent sync ACK');
    const input=host.events?.find(event=>event.kind==='input-ack'),sync=host.events?.find(event=>event.kind==='sync-ack');
    require(input&&sync&&Number.isFinite(input.at)&&input.at<sync.at,'Concurrent Host ACK timestamps do not demonstrate live input');
    const before=assertProofState(host.before,plan.references[seat]);
    const first=before.capture+before.delay;
    require(first>=plan.end&&first+1<end,'Fire proof overlaps neutral baseline or has no new frame interval');
    const buttons=frame=>value.view.getUint16(value.offset(frame,seat),true);
    let transition=null,fire=0,right=0;
    for(let frame=first;frame<end;++frame){const held=buttons(frame);fire+=!!(held&1);right+=!!(held&128);
      if(transition===null&&frame+1<end&&!(buttons(frame-1)&1)&&(held&1)&&(buttons(frame+1)&1))transition=frame;
    }
    require(transition!==null,'No new native-confirmed neutral-to-fire transition for seat '+seat);
    return {seat,first,end,transitionFrame:transition,previousButtons:buttons(transition-1),
      transitionButtons:buttons(transition),followingButtons:buttons(transition+1),fireFrames:fire,rightFramesObserved:right,
      directionMeaning:'Direction is observed; movement readiness can suppress joystick directions.'};
  });
  return {passed:true,neutral:{first:plan.first,end:plan.end},end,seats,confirmedFrames:parsed.map(value=>value.frames)};
}
