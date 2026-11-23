import {cirnoPaletteSet} from './coop-cirno-palettes.mjs';

// Old parallel-stage sessions/recordings cannot be opened as co-op.
const fields=['schema','mode','matchId','epoch','players','difficulty','route','challengeMode','seed','inputDelay','rollbackWindow','coreSHA256','resourceSHA256'];
const entries=new Map([['A',[1,'st_a1_1.ecl']],['B',[6,'st_b1_1.ecl']],['C',[11,'st_c1_1.ecl']],['EX',[16,'st_ex.ecl']]]);
const optionalFields=['randomCirnoPalette','cirnoPaletteSet','freezeMotivationMultiplier'];
export const validFreezeMotivationMultiplier=value=>[2.5,2,1,0.75,0.5].includes(value);
const wireOf=new WeakMap(),integer=(n,min,max)=>Number.isSafeInteger(n)&&n>=min&&n<=max;
export function makeCoopRules(input){
 if(!input||![Object.prototype,null].includes(Object.getPrototypeOf(input)))throw Error('Expected plain cooperative room settings');
 const descriptors=Object.getOwnPropertyDescriptors(input);
 const palette=descriptors.randomCirnoPalette,set=descriptors.cirnoPaletteSet,motivation=descriptors.freezeMotivationMultiplier;
 if(motivation&&!validFreezeMotivationMultiplier(motivation.value))throw Error('Invalid freeze motivation multiplier rule');
 if((palette&&typeof palette.value!=='boolean')||(set&&(palette?.value!==true||set.value!==cirnoPaletteSet)))throw Error('Invalid Cirno palette rule');
 if(Reflect.ownKeys(descriptors).length!==fields.length+optionalFields.filter(key=>Object.hasOwn(descriptors,key)).length||fields.some(k=>!Object.hasOwn(descriptors[k]??{},'value')))throw Error('Unknown or missing cooperative rule');
 const w=Object.freeze({...Object.fromEntries(fields.map(k=>[k,descriptors[k].value])),
   ...(palette?.value===true?{randomCirnoPalette:true}:{}),...(set?{cirnoPaletteSet}:{}),
   ...(motivation&&motivation.value!==1?{freezeMotivationMultiplier:motivation.value}:{})});
 if(w.schema!=='th128-coop/1'||w.mode!=='same-stage-coop'||typeof w.matchId!=='string'||!/^[a-zA-Z0-9_-]{1,64}$/.test(w.matchId)||!integer(w.epoch,1,0x7fffffff))throw Error('Invalid co-op match identity');
 if(!integer(w.players,2,3)||!integer(w.difficulty,0,4)||!entries.has(w.route)||(w.route==='EX')!==(w.difficulty===4)||typeof w.challengeMode!=='boolean')throw Error('Invalid co-op seats, route, difficulty or challenge');
 if(!integer(w.seed,0,65535)||!integer(w.inputDelay,0,8)||!integer(w.rollbackWindow,0,2))throw Error('Invalid cooperative timing or seed');
 if([w.coreSHA256,w.resourceSHA256].some(h=>typeof h!=='string'||!/^[a-f0-9]{64}$/.test(h)))throw Error('Missing co-op core/package identity');
 const [stageId,ecl]=entries.get(w.route),rules=Object.freeze({...w,randomCirnoPalette:w.randomCirnoPalette===true,freezeMotivationMultiplier:w.freezeMotivationMultiplier??1,stageId,ecl,tickRate:60});wireOf.set(rules,w);return rules;
}
export function coopWire(rules){const wire=wireOf.get(rules);if(!wire)throw Error('Not a validated co-op rule');return wire;}
export async function coopFingerprint(rules){const bytes=new TextEncoder().encode(JSON.stringify(coopWire(rules)));return Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',bytes)),n=>n.toString(16).padStart(2,'0')).join('');}
export async function checkCoopAgreement(local,wire,claim){const remote=makeCoopRules(wire);if(JSON.stringify(coopWire(local))!==JSON.stringify(coopWire(remote))||claim!==await coopFingerprint(remote))throw Error('Co-op room settings or build differ');return remote;}