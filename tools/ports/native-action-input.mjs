import assert from 'node:assert/strict';

/** Shared held actions and edge pulses; transports own the actual input device. */
export function createNativeActionInput(io,{pulseTicks,source,describe=()=>({})}={}){
  assert.equal(typeof io.set,'function');assert.equal(typeof io.step,'function');
  assert.ok(Number.isSafeInteger(pulseTicks)&&pulseTicks>0&&pulseTicks<=60);assert.ok(typeof source==='string'&&source);
  const held=new Set(),pending=new Map(),serials={bombSerial:0,escapeSerial:0};
  const evidence={source,pulseTicks,ticks:0,events:[],disposed:false};
  const emit=async(action,down,cause)=>{
    if(held.has(action)===down)return null;
    // A dispatched down remains owned after failed acknowledgement, for cleanup.
    if(down)held.add(action);
    let receipt;
    try{receipt=await io.set(action,down);assert.equal(receipt?.acknowledged,true,`${source} ${action} was not acknowledged`);}
    finally{if(!down)held.delete(action);}
    const event={action,...describe(action),down,cause,afterTick:evidence.ticks,receipt};evidence.events.push(event);return event;
  };
  return{
    evidence,
    controls:async fields=>{
      assert.equal(evidence.disposed,false);assert.equal(fields.joystickX,0);assert.equal(fields.joystickY,0);
      const events=[];
      for(const [field,action] of [['focusEnabled','focus'],['fireEnabled','fire']]){
        assert.equal(typeof fields[field],'boolean');const event=await emit(action,fields[field],'held-control');if(event)events.push(event);
      }
      for(const [field,action] of [['bombSerial','bomb'],['escapeSerial','escape']]){
        assert.ok(Number.isSafeInteger(fields[field])&&fields[field]>=0&&fields[field]<=0xffffffff);
        if(fields[field]===serials[field])continue;
        assert.equal(fields[field],(serials[field]+1)>>>0,'Native action serials must advance by one');
        assert.equal(held.has(action),false,'A native action pulse is already held');
        events.push(await emit(action,true,'action-pulse'));serials[field]=fields[field];pending.set(action,pulseTicks);
      }
      return{acknowledged:true,source,events};
    },
    step:async count=>{
      assert.equal(evidence.disposed,false);assert.ok(Number.isSafeInteger(count)&&count>0&&count<=120);
      for(let index=0;index<count;index++){
        await io.step(1);evidence.ticks++;
        for(const [action,remaining] of [...pending]){
          if(remaining>1)pending.set(action,remaining-1);
          else{await emit(action,false,'pulse-expired');pending.delete(action);}
        }
      }
    },
    dispose:async()=>{
      const errors=[];for(const action of [...held])try{await emit(action,false,'dispose');}catch(error){errors.push(error);}
      pending.clear();evidence.disposed=true;
      if(errors.length)throw new AggregateError(errors,'Native action inputs did not all release');
    },
  };
}
