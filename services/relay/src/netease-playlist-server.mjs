import { createServer } from 'node:http';
import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { createNetEasePlaylistHandler } from './netease-playlist.mjs';

export function createNetEasePlaylistServer(read) {
  const handle = createNetEasePlaylistHandler(read);
  return createServer((request, response) => {
    if (request.url?.split('?', 1)[0] !== '/_eagler/netease-playlist') {
      response.writeHead(404).end();
      return;
    }
    void handle(request, response);
  });
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  const port = Number(process.env.EAGLER_NETEASE_PLAYLIST_PORT ?? 18465);
  if (!Number.isInteger(port) || port < 1 || port > 65535) {
    throw new Error('Invalid EAGLER_NETEASE_PLAYLIST_PORT');
  }
  createNetEasePlaylistServer().listen(port, '127.0.0.1');
}
