import {acceptMultiplayerRules,multiplayerRulesFingerprint,multiplayerRulesWire} from '../../th128/site/runtime/multiplayer-rules.mjs';

const schema='th128-mp-ready/1';
const fields=['schema','kind','matchId','epoch','fingerprint','allowedMask','player'];
const nativeMask=763;

// Call after the local engine has loaded and verified the assets in rules.
// The caller must serialize receives in transport order and bind sourceSeat
// from the transport, independently of the value in the message.
export async function createMultiplayerStartGate({rules,localPlayer,allowedMask}){
  if(localPlayer!==0&&localPlayer!==1)throw new TypeError('Invalid local seat');
  if(!Number.isSafeInteger(allowedMask)||allowedMask<1||allowedMask>nativeMask||(allowedMask&~nativeMask))
    throw new RangeError('Invalid native capability mask');
  const fingerprint=await multiplayerRulesFingerprint(rules);
  const {matchId,epoch}=rules;
  const identity={schema,matchId,epoch,fingerprint,allowedMask,player:localPlayer};
  const ready=Object.freeze({...identity,kind:'ready',rules:multiplayerRulesWire(rules)});
  const ack=Object.freeze({...identity,kind:'ack'});
  let remoteReady=false,localAcknowledged=false;
  function status(){return Object.freeze({remoteReady,localAcknowledged,canStart:remoteReady&&localAcknowledged});}
  async function receive(message,sourceSeat){
    if(sourceSeat!==1-localPlayer)throw new Error('Startup must come from the bound remote seat');
    if(!message||![Object.prototype,null].includes(Object.getPrototypeOf(message)))throw new TypeError('Invalid startup data');
    const descriptors=Object.getOwnPropertyDescriptors(message),keys=Reflect.ownKeys(descriptors);
    const kind=descriptors.kind?.value;
    const expected=kind==='ready'?[...fields,'rules']:fields;
    if((kind!=='ready'&&kind!=='ack')||keys.length!==expected.length||expected.some(key=>!descriptors[key]||!Object.hasOwn(descriptors[key],'value')))
      throw new Error('Unexpected startup fields');
    const m=Object.fromEntries(expected.map(key=>[key,descriptors[key].value]));
    if(m.player!==sourceSeat||m.schema!==schema||m.matchId!==matchId||m.epoch!==epoch||m.fingerprint!==fingerprint)
      throw new Error('Startup identity mismatch');
    if(m.allowedMask!==allowedMask)throw new Error('Startup native capability mismatch');
    if(kind==='ready'){
      await acceptMultiplayerRules(rules,m.rules,m.fingerprint);
      remoteReady=true;return ack;
    }
    if(!remoteReady)throw new Error('Startup acknowledgement before remote ready');
    localAcknowledged=true;return null;
  }
  return Object.freeze({offer:()=>ready,status,receive});
}
