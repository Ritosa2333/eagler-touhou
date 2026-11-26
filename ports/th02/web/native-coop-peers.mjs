// WebRTC and relay channels carry the same seat rows; the fast WebRTC lane
// can deliver rows out of order.
// No local input is sampled twice while a frame is waiting for a peer.
import {MAX_INPUT,packTouch,unpackTouch} from './touch-input.js';

export const TH02_NATIVE_COOP_PROTOCOL = 'th02-native-coop/4';
export const TH02_COOP_INPUT_MASK = 0x04ff;
export const validCoopInput = value => Number.isSafeInteger(value)&&value>=0&&value<=MAX_INPUT&&
    !(value%4096&~TH02_COOP_INPUT_MASK)&&value===packTouch(value%4096,unpackTouch(value));
const roles = ['host','guest','guest2'];
const integer = (value,min,max) => Number.isSafeInteger(value) && value>=min && value<=max;
const maxAhead = 64;
const hashInterval = 120;
const captureReserve = 4;

export function createConfirmedPeerRun({count,seat,delay,identity,transport,world,sample,fail,checkpoint=()=>{},confirmed=()=>{}}){
    if (![2,3].includes(count) || !integer(seat,0,count-1) || !integer(delay,0,9)) throw Error('Invalid TH02 peer run');
    const runIdentity = JSON.stringify([TH02_NATIVE_COOP_PROTOCOL,count,delay,identity]);
    const rows = Array.from({length:count},()=>new Map(Array.from({length:delay},(_,i)=>[i,0])));
    const peers = [];
    const hashes = new Map();
    let frame=0, nextCapture=delay, closed=false, finished=false;
    const ready = () => !closed && peers.every(peer=>peer.hello);
    const broadcast = message => {
        const text = JSON.stringify(message);
        for(const {channel} of peers)channel.send(text);
    };
    function close(){
        if(closed)return;
        closed=true;
        transport.close();
    }
    const guard = action => (...args) => {
        if(closed)return;
        try {return action(...args);}
        catch(error) {close();fail(error);}
    };
    function insert(owner,at,mask){
        if(!integer(at,Math.max(frame,delay),frame+maxAhead) || !validCoopInput(mask))
            throw Error('TH02 input stream is out of sequence');
        const old=rows[owner].get(at);
        if(old!==undefined&&old!==mask)throw Error('Conflicting TH02 seat input');
        rows[owner].set(at,mask);
    }
    function check(owner,at,hash){
        if(!integer(at,Math.max(hashInterval,frame-maxAhead),frame+maxAhead) || at%hashInterval || !integer(hash,0,0xffffffff))
            throw Error('Invalid TH02 state check');
        const record=hashes.get(at)??{hash,seats:new Set()};
        if(record.seats.has(owner)||record.hash!==hash)throw Error(`TH02 co-op state diverged at frame ${at}`);
        record.seats.add(owner);
        if(record.seats.size===count){hashes.delete(at);checkpoint(at,hash);}
        else hashes.set(at,record);
    }
    for(let remote=0;remote<count;++remote)if(remote!==seat){
        const channel=transport.channelFor(roles[remote]),peer={channel,hello:false};
        peers.push(peer);
        channel.onopen=guard(()=>channel.send(JSON.stringify({type:'hello',identity:runIdentity})));
        channel.onmessage=guard(event=>{
            const message=JSON.parse(event.data);
            if(message.type==='hello'){
                if(peer.hello||message.identity!==runIdentity)throw Error('TH02 room rules or build do not match');
                peer.hello=true;return;
            }
            // The relay can deliver an input ahead of its hello. Nothing in
            // rows is applied until every peer has confirmed the same run.
            if(message.type==='input')insert(remote,message.frame,message.mask);
            else if(message.type==='check')check(remote,message.frame,message.hash);
            else throw Error('Invalid TH02 peer message');
        });
    }
    return {
        start:guard(()=>transport.connect()),
        capture:guard(()=>{
            if(!ready()||finished)return false;
            if(nextCapture>frame+delay+captureReserve)return false;
            const future=nextCapture++,mask=sample();
            insert(seat,future,mask);
            broadcast({type:'input',frame:future,mask});
            return true;
        }),
        advance:guard((keepBufferedFrame=false)=>{
            if(!ready()||finished)return false;
            if(rows.some(row=>!row.has(frame)||(keepBufferedFrame&&!row.has(frame+1))))return false;
            const masks=rows.map(row=>row.get(frame));while(masks.length<3)masks.push(0);
            finished=world.step(frame,masks);
            confirmed(frame,masks);
            for(const row of rows)row.delete(frame);
            ++frame;
            if(frame%hashInterval===0){
                const hash=world.hash();
                check(seat,frame,hash);
                broadcast({type:'check',frame,hash});
            }
            return true;
        }),
        close,
        get ready(){return ready();},
        get frame(){return frame;},
        get captured(){return nextCapture-delay;},
        get finished(){return finished;},
    };
}

// Capture and simulation have independent fixed-retrace budgets. A late
// input does not stop local sampling; a recovered row does not cost another
// physical sample. Limit both the queued lead and catch-up work per paint.
export class ConfirmedRoomClock {
    constructor(period){
        this.period=period;
        this.last=undefined;
        this.captureDue=0;
        this.stepDue=0;
        this.waits=0;
        this.catchups=0;
    }
    frame(now,peer){
        const period=this.period;
        const elapsed=this.last===undefined?period:now-this.last;
        this.last=now;
        const delta=elapsed>=4*period?period:elapsed;
        this.captureDue=Math.min(this.captureDue+delta,2*period);
        this.stepDue=Math.min(this.stepDue+delta,2*period);
        while(this.captureDue>=period){
            this.captureDue-=period;
            if(!peer.capture()){
                this.captureDue=0;
                break;
            }
        }
        let stepped=0;
        while(this.stepDue>=period&&peer.frame<peer.captured&&!peer.finished){
            if(!peer.advance()){
                ++this.waits;
                break;
            }
            this.stepDue-=period;
            ++stepped;
        }
        // Retire the capture backlog after the link recovers. Keep one real
        // frame in reserve so catching up does not reopen the same wait.
        if(stepped===1&&peer.frame<peer.captured&&peer.advance(true))++stepped;
        if(stepped>1)++this.catchups;
    }
}
