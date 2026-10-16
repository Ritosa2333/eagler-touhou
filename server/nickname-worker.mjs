import {parentPort, workerData} from 'node:worker_threads';
import {createPublicNicknames} from './public-nicknames.mjs';
import {createDeepSeekNicknameReviewer} from './nickname-review.mjs';

const members = new Map();
const names = createPublicNicknames({...workerData,
  review: createDeepSeekNicknameReviewer({apiKey: workerData.apiKey, apiBase: workerData.apiBase, model: workerData.model}),
  onUpdate(key,profile) {const member=members.get(key);if(member)parentPort.postMessage({update:true,member,profile});}});
parentPort.on('message', async ({id, operation, member, address, name}) => {
  try {
    const key=names.memberKey(member);
    if(members.size>=10000&&!members.has(key))members.delete(members.keys().next().value);
    members.set(key,member);
    const value = operation === 'current' ? names.current(member) : await names.enqueue(member, address, name);
    parentPort.postMessage({id, value});
    if(operation!=='current')setImmediate(()=>void names.processQueue().catch(()=>{}));
  } catch { parentPort.postMessage({id, error: true}); }
});
setInterval(()=>void names.processQueue().catch(()=>{}),1000).unref();
parentPort.postMessage({ready: true});
