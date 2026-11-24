// Stage4 protocol rules only; no native World or transport is started here.
const fields=['schema','matchId','epoch','mode','route','seed','coreSHA256','resourceSHA256','inputDelay','rollbackWindow'];
const routes=new Map([['A1-3',[3,1]],['A2-3',[5,1]],['B1-3',[8,1]],['B2-3',[10,1]],['C1-3',[13,1]],['C2-3',[15,1]],['EX',[16,4]]]);
const prepared=new WeakSet(),hashPattern=/^[a-f0-9]{64}$/;
function integer(value,min,max){return Number.isSafeInteger(value)&&value>=min&&value<=max;}
export function createMultiplayerRules(input){
 if(!input||Array.isArray(input)||![Object.prototype,null].includes(Object.getPrototypeOf(input)))throw Error('Rules require a plain data object');
 const descriptors=Object.getOwnPropertyDescriptors(input),keys=Reflect.ownKeys(descriptors);
 if(keys.length!==fields.length||keys.some(key=>typeof key!=='string'||!fields.includes(key)))throw Error('Unknown or missing rule fields');
 const wire={};
 for(const key of fields){const row=descriptors[key];if(!row||!Object.hasOwn(row,'value'))throw Error('Rules require data property fields');wire[key]=row.value;}
 if(wire.schema!=='th128-multiplayer-rules/1'||wire.mode!=='parallel-stage')throw Error('Unsupported rules schema or mode');
 if(typeof wire.matchId!=='string'||!/^[a-zA-Z0-9_-]{1,64}$/.test(wire.matchId)||!integer(wire.epoch,1,2147483647))throw Error('Invalid match identity');
 if(typeof wire.route!=='string'||!routes.has(wire.route)||!integer(wire.seed,0,65535))throw Error('Invalid route or seed');
 for(const key of ['coreSHA256','resourceSHA256'])if(typeof wire[key]!=='string'||!hashPattern.test(wire[key]))throw Error('Invalid full SHA256 rule field');
 if(!integer(wire.inputDelay,0,8)||!integer(wire.rollbackWindow,0,120))throw Error('Invalid input scheduling limits');
 const[stageId,difficulty]=routes.get(wire.route),rules=Object.freeze({...wire,stageId,difficulty,tickRate:60,players:2,progress:0,normalModeClear:false,carryVerified:false});
 prepared.add(rules);return rules;
}
function canonical(rules){
 if(!prepared.has(rules))throw Error('Canonical rules must come from createMultiplayerRules');
 return JSON.stringify(rules);
}
export async function multiplayerRulesFingerprint(rules){
 const bytes=new TextEncoder().encode(canonical(rules)),digest=await globalThis.crypto.subtle.digest('SHA-256',bytes);
 return Array.from(new Uint8Array(digest),byte=>byte.toString(16).padStart(2,'0')).join('');
}
export async function acceptMultiplayerRules(local,remoteWire,claimedFingerprint){
 const expected=canonical(local),remote=createMultiplayerRules(remoteWire);
 if(canonical(remote)!==expected)throw Error('Multiplayer rules mismatch');
 if(typeof claimedFingerprint!=='string'||!hashPattern.test(claimedFingerprint))throw Error('Invalid rule fingerprint');
 const fingerprint=await multiplayerRulesFingerprint(remote);if(fingerprint!==claimedFingerprint)throw Error('Rule fingerprint mismatch');
 return Object.freeze({accepted:true,fingerprint,rules:remote});
}
