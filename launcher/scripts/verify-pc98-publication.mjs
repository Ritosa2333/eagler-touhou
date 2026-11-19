import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {readFile} from 'node:fs/promises';
import {PRODUCT_CONTENT} from '../lib/content-definition.mjs';
import {validateNativeTH03Data} from '../lib/pc98-static-publication.mjs';
import {validatePackageDescriptor} from '../package/package-descriptor.mjs';
import {releaseCatalogEntryUrl, validateReleaseCatalog} from '../lib/contracts/release-catalog.mjs';
import {validateSiteManifest} from '../lib/contracts/site-manifest.mjs';
import {HOST_PROTOCOL} from '../lib/contracts/product-catalog.mjs';

const [site, expectedLobby] = process.argv.slice(2);
if (!site) throw Error('usage: node launcher/scripts/verify-pc98-publication.mjs <site-url> [built-lobby.html]');
const base = new URL(site.endsWith('/') ? site : site + '/');
let metadata;
const checked = await Promise.allSettled([verifyLobby(), ...['th01','th02','th03','th04'].map(verifyPackage)]);
const failures = checked.filter(result => result.status === 'rejected');
if (failures.length) throw new AggregateError(failures.map(result => result.reason), 'PC-98 publication verification failed');

async function get(url) {
  const result = await fetch(new URL(url, base), {cache:'no-store', signal:AbortSignal.timeout(60000)});
  if (!result.ok) throw Error(`${url}: HTTP ${result.status}`);
  return result;
}

function jsAssets(html) {
  return [...new Set([...html.matchAll(/\bhref="([^"]*\/assets\/[^"]+\.js)"/g)].map(match => match[1]))];
}
async function verifyLobby() {
  const html = await (await get('lobby.html')).text(), assets = jsAssets(html);
  assert(assets.length, 'lobby.html has no React module assets');
  if (expectedLobby) {
    const names = urls => urls.map(url => new URL(url, base).pathname.split('/').at(-1)).sort();
    assert.deepEqual(names(assets), names(jsAssets(await readFile(expectedLobby, 'utf8'))), 'the site is not serving the selected launcher build');
  }
  const code = (await Promise.all(assets.map(async url => (await get(url)).text()))).join('\n');
  const missing = ['mpTh01BallCount','mpPc98InitialSpares','mpPc98InitialBombs','mpTh03ShowHitboxes',
    'mpTh03DisableFocus','th03FocusToggle','pc98-room-touch-active'].filter(id => !code.includes(id));
  assert(!missing.length, 'lobby.html references a frontend without: ' + missing.join(', '));
  console.log('Lobby references the PC-98 settings and room touch surface');
}

async function verifyPackage(game) {
  metadata ??= Promise.all([
    get('site-manifest.json').then(async r => validateSiteManifest(await r.json())),
    get('release-catalog.json').then(async r => validateReleaseCatalog(await r.json())),
  ]);
  const [siteManifest, catalog] = await metadata;
  const hosted = siteManifest.games[game];
  assert(hosted?.runtime && hosted.multiplayerRuntime && catalog.games[game], `${game}: publication is incomplete`);
  assert.deepEqual(hosted.package, catalog.games[game], `${game}: package pointer mismatch`);
  const url = releaseCatalogEntryUrl(new URL('release-catalog.json', base), catalog, game);
  const descriptor = validatePackageDescriptor(await (await get(url)).json());
  assert.equal(descriptor.game, game);
  assert.equal(descriptor.revision, catalog.games[game].revision, `${game}: package revision mismatch`);
  const {dataFile, dataLayout, protocol, target} = descriptor.runtimeRequirement;
  assert.equal(protocol, HOST_PROTOCOL, `${game}: Package uses a different Runtime protocol`);
  assert.equal(target, game, `${game}: Package targets a different Runtime`);
  assert.equal(dataLayout, PRODUCT_CONTENT[game].dataLayout, `${game}: package layout differs from build`);
  assert.equal(dataLayout, hosted.gameData.layout, `${game}: site layout differs from Package`);
  const data = descriptor.files[dataFile];
  assert.equal(data.sha256, hosted.gameData.sha256, `${game}: site SHA-256 differs from Package`);
  assert.equal(data.bytes, hosted.gameData.bytes, `${game}: site size differs from Package`);
  if (game === 'th03') assert(!descriptor.components['pc98-manifest'], 'native TH03 cannot use a disk Package');
  const bytes = Buffer.from(await (await get(new URL(data.source, url))).arrayBuffer());
  assert.equal(bytes.length, data.bytes, `${game}: downloaded size mismatch`);
  assert.equal(createHash('sha256').update(bytes).digest('hex'), data.sha256, `${game}: downloaded SHA-256 mismatch`);
  if (game === 'th03') validateNativeTH03Data(bytes);
  else {
    const {ManagedContent} = await import(`../../ports/${game}/web/managed-data.mjs`);
    await ManagedContent.open(bytes, game);
  }
  console.log(`${game}: ${descriptor.revision} DATA verified`);
}
