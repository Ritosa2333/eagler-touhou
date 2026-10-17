// Explicitly preloaded by the isolated integration test only. Never deployed.
import {isMainThread} from 'node:worker_threads';
import assert from 'node:assert/strict';
import {existsSync} from 'node:fs';
if (!isMainThread) globalThis.fetch = async (url, request) => {
  assert.equal(url, (process.env.EAGLER_NETPLAY_NICKNAME_API_BASE || 'https://api.deepseek.com').replace(/\/+$/,'') + '/chat/completions');
  const body = JSON.parse(request.body), name = JSON.parse(body.messages[1].content);
  assert.equal(body.max_tokens, 2); assert.equal(body.thinking.type, 'disabled');
  await new Promise(resolve => setTimeout(resolve, 80));
  if (name === '雨夜灵梦' && process.env.EAGLER_TEST_NICKNAME_GATE && !existsSync(process.env.EAGLER_TEST_NICKNAME_GATE)) {
    await new Promise(resolve=>setTimeout(resolve,1500));return new Response('',{status:503});
  }
  if (name === '故障昵称' && process.env.EAGLER_TEST_NICKNAME_RECOVER !== '1') return new Response('', {status: 503});
  return new Response(JSON.stringify({choices: [{finish_reason:'stop',message:{content:name === '拒绝昵称' ? '0' : '1'}}]}));
};
