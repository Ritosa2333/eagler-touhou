import {isReservedPublicNickname, PUBLIC_NICKNAME_REVIEW_PROMPT} from './nickname-policy.mjs';

// Bounded transport; callers supply the policy and the content for that policy.
export function createCompletionReviewer({apiKey, apiBase = 'https://api.deepseek.com', model = 'deepseek-flash', fetch: request = globalThis.fetch, timeoutMs = 8000}) {
  if (!apiKey || /[\r\n]/.test(apiKey)) throw new Error('Nickname moderation requires a server API key');
  let endpoint;
  try {
    endpoint = new URL(apiBase);
    if (endpoint.protocol !== 'https:' || endpoint.username || endpoint.password || endpoint.search || endpoint.hash) throw new Error();
    endpoint.pathname = endpoint.pathname.replace(/\/+$/, '') + '/chat/completions';
  } catch { throw new Error('Nickname moderation requires a plain HTTPS API base'); }
  return async (prompt, content) => {
    const response = await request(endpoint.href, {
      method: 'POST', redirect: 'error', signal: AbortSignal.timeout(timeoutMs),
      headers: {'Authorization': `Bearer ${apiKey}`, 'Content-Type': 'application/json'},
      body: JSON.stringify({model, thinking: {type: 'disabled'}, max_tokens: 2, temperature: 0,
        messages: [
          {role: 'system', content: prompt},
          {role: 'user', content},
        ]}),
    });
    if (!response.ok || Number(response.headers.get('content-length')) > 16384) {
      await response.body?.cancel().catch(() => {});
      throw new Error('Nickname review unavailable');
    }
    if (!response.body) throw new Error('Invalid nickname review response');
    const reader = response.body.getReader(), chunks = [];
    let size = 0;
    try {
      for (;;) {
        const {done, value} = await reader.read();
        if (done) break;
        size += value.byteLength;
        if (size > 16384) throw new Error('Invalid nickname review response');
        chunks.push(value);
      }
    } finally {
      await reader.cancel().catch(() => {});
      reader.releaseLock();
    }
    const body = Buffer.concat(chunks, size).toString('utf8');
    const result = JSON.parse(body), choice = result.choices?.[0];
    const verdict = choice?.message?.content?.trim();
    if (choice?.finish_reason !== 'stop' || !['0', '1'].includes(verdict)) throw new Error('Invalid nickname review verdict');
    return verdict === '1';
  };
}

export function createDeepSeekNicknameReviewer(options) {
  const review = createCompletionReviewer(options);
  return name => isReservedPublicNickname(name) ? Promise.resolve(false) : review(PUBLIC_NICKNAME_REVIEW_PROMPT, JSON.stringify(name));
}
