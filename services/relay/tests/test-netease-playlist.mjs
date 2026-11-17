import assert from 'node:assert/strict';
import {test} from 'node:test';
import {readNetEasePlaylist} from '../src/netease-playlist.mjs';

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
