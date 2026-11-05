// Run only as the owned loopback test relay process; the source relay is unmodified.
import {createHash} from 'node:crypto';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {installRelayTransportProbe} from './relay-transport-probe.mjs';
import {relayRtcTimeoutConfiguration} from './relay-startup-probe.mjs';

if(process.env.EAGLER_NETPLAY_RELAY_HOST!=='127.0.0.1'||!process.env.TH09_RELAY_SOURCE||!process.send)throw Error('Relay diagnostics require an explicitly owned loopback child with IPC');
const source=resolve(process.env.TH09_RELAY_SOURCE),config=JSON.parse(process.env.TH09_RELAY_TRACE_CONFIG||'{}');
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const sourceIdentity={relaySource:source,relaySha256:sha(source),probeSha256:sha(fileURLToPath(new URL('./relay-transport-probe.mjs',import.meta.url))),fixtureSha256:sha(fileURLToPath(import.meta.url))};
sourceIdentity.startupProbeSha256=sha(fileURLToPath(new URL('./relay-startup-probe.mjs',import.meta.url)));
if(config.startupTrace){sourceIdentity.rtcTimeout=relayRtcTimeoutConfiguration(process.env);config.rtcTimeoutMs=sourceIdentity.rtcTimeout.rtcTimeoutMs;}
// All injected waiting belongs to the diagnostic wrapper. Do not stack a hidden
// service delay/drop with it. Compatibility aliases must not contradict this.
for(const [key,value] of Object.entries({EAGLER_NETPLAY_RELAY_DELAY_MS:'0',EAGLER_NETPLAY_RELAY_JITTER_MS:'0',EAGLER_NETPLAY_RELAY_DROP_EVERY:'0',
 EAGLER_NETPLAY_RELAY_DROP_FIRST_INPUT_PER_EDGE:'0',EAGLER_NETPLAY_RELAY_DROP_INPUT_LATEST_FROM:'-1',EAGLER_NETPLAY_RELAY_DROP_INPUT_LATEST_TO:'-1',EAGLER_NETPLAY_TEST_ROUTE_SKEW_MS:'0'}))process.env[key]=value;
const require=createRequire(source),WebSocket=require('ws');
sourceIdentity.wsModule=require.resolve('ws');sourceIdentity.wsSha256=sha(sourceIdentity.wsModule);
const probe=installRelayTransportProbe(WebSocket,config);
let requests=0;
process.on('message',message=>{
 if(message?.type!=='th09-relay-trace'||!Number.isInteger(message.id)||!['snapshot','stop'].includes(message.operation))return;
 if(++requests>16)return;
 const trace=probe.snapshot({stop:message.operation==='stop'});
 process.send?.({type:'th09-relay-trace',id:message.id,trace:{...trace,sourceIdentity}},()=>{});
});
await import(pathToFileURL(source).href);

