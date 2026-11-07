import {createServer} from 'node:http';
import {isIP} from 'node:net';
import {createNicknameService} from './nickname-service.mjs';

const token = process.env.NICKNAME_BRIDGE_SECRET;
if (!token || token.length < 32) throw Error('Set NICKNAME_BRIDGE_SECRET');

const service = await createNicknameService({...process.env,EAGLER_NETPLAY_PUBLIC_NICKNAMES:'1'});
const server = createServer(async (request,response) => {
  function reply(status,value) {
    response.writeHead(status, {'content-type':'application/json','cache-control':'no-store'});
    response.end(JSON.stringify(value));
  }
  if (request.headers.authorization !== 'Bearer ' + token) return reply(403,null);
  if (request.method !== 'POST' || !['/current','/submit'].includes(request.url)) return reply(404,null);
  try {
    const chunks = [];
    let size = 0;
    for await (const chunk of request) {
      size += chunk.length;
      if (size > 2048) return reply(413,null);
      chunks.push(chunk);
    }
    const {member,address,name} = JSON.parse(Buffer.concat(chunks).toString('utf8'));
    if (typeof member !== 'string' || !/^[A-Za-z0-9_-]{8,64}$/.test(member)) return reply(400,null);
    if (request.url === '/current') return reply(200,await service.current(member));
    if (!isIP(address) || typeof name !== 'string' || name.length > 256) return reply(400,null);
    return reply(200,await service.submit(member,address,name));
  } catch {return reply(503,null);}
});
server.requestTimeout = 15000;
server.listen(Number(process.env.NICKNAME_BRIDGE_PORT || 18411),'127.0.0.1');
for (const signal of ['SIGTERM','SIGINT']) process.once(signal, () => {
  server.close(); service.close().finally(() => process.exit());
});
