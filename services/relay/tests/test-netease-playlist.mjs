import assert from 'node:assert/strict';
import {test} from 'node:test';
import {readNetEasePlaylist} from '../src/netease-playlist.mjs';
import {createNetEasePlaylistServer} from '../src/netease-playlist-server.mjs';
import {once} from 'node:events';

test('imports the complete list of track IDs in playlist order', async () => {
  const ids = Array.from({length: 57}, (_, index) => index + 100);
  const tracks = await readNetEasePlaylist('1234', async url => {
    if (url.pathname === '/api/v6/playlist/detail') {
      return Response.json({code: 200, playlist: {
        tracks: ids.slice(0, 20).map(id => ({id})),
        trackIds: ids.map(id => ({id})),
      }});
    }
    assert.equal(url.pathname, '/api/v3/song/detail');
    assert.deepEqual(JSON.parse(url.searchParams.get('c')).map(({id}) => id), ids);
    return Response.json({code: 200, songs: [...ids].reverse().map(id => ({id, name: 'Song ' + id}))});
  });
  assert.deepEqual(tracks.map(track => track.id), ids);
});

test('the standalone playlist route serves more than the 20-track preview', async t => {
  const tracks = Array.from({length: 57}, (_, index) => ({id: index + 1, source: 'netease'}));
  const server = createNetEasePlaylistServer(async id => {
    assert.equal(id, '1234');
    return tracks;
  }).listen(0, '127.0.0.1');
  t.after(() => server.close());
  await once(server, 'listening');
  const origin = 'http://127.0.0.1:' + server.address().port;
  const response = await fetch(origin + '/_eagler/netease-playlist?id=1234');
  assert.equal(response.status, 200);
  assert.equal(response.headers.get('access-control-allow-origin'), '*');
  assert.deepEqual(await response.json(), tracks);
  assert.equal((await fetch(origin + '/_eagler/netease-playlist')).status, 400);
  assert.equal((await fetch(origin + '/')).status, 404);
});
