// File hashes only; does not instantiate an emulator or run a browser.
const fs = require('node:fs'), path = require('node:path'), crypto = require('node:crypto');
const root = path.resolve(__dirname, '..'), web = path.join(root, 'web');
const sha = bytes => crypto.createHash('sha256').update(bytes).digest('hex');
function write() {
  const vendor = path.join(web, 'vendor/dosbox-x');
  const buildFile=path.join(vendor,'core-build.json');
  const build=fs.existsSync(buildFile)?JSON.parse(fs.readFileSync(buildFile)):null;
  const runtime = {
    schema:3, game:'TH03', package:'emulators', version:'8.5.2', machine:'pc98',
    local:{core:'simple', cycles:'max 100%', continuation:'JSPI', initialHeapMiB:64, maximumHeapMiB:256},
    network:{core:'simple', cycles:16000, continuation:'full Asyncify', binaryen:'132', initialHeapMiB:64, maximumHeapMiB:256,snapshotStride:2,writeJournal:{schema:1,pageBytes:4096,enabled:'rollback and multi-memory'}},
    presentation:{local:'direct', network:'completed batch', sha256:sha(fs.readFileSync(path.join(web, 'frame-presenter.js')))},
    sourceWasm:sha(fs.readFileSync(path.join(vendor, 'wdosbox-x-jspi.wasm'))),
    keyboardFix:{excludedKeys:[340,341,342,343,344,345,346,347]}, files:{},
  };
  if(build){
    for(const [name,expected]of Object.entries(build.files))
      if(sha(fs.readFileSync(path.join(vendor,name)))!==expected)throw Error('Promoted core identity mismatch: '+name);
    runtime.local.sourceWasm=runtime.sourceWasm;
    runtime.sourceWasm=build.rawWasmSha256;
    runtime.sourceRevisions=build.sourceRevisions;
    Object.assign(runtime.network,build.releaseProfile,{idleFusion:build.idleFusion,snapshotScan:build.snapshotScan,
      continuation:'full Asyncify, TH03 zero-time native scheduling',gameplayAccepted:build.gameplayAccepted});
    if(build.inputPolicy)runtime.network.inputPolicy=build.inputPolicy;
  }
  for (const name of fs.readdirSync(vendor).sort()) if (name !== 'runtime.json') runtime.files[name] = sha(fs.readFileSync(path.join(vendor, name)));
  fs.writeFileSync(path.join(vendor, 'runtime.json'), JSON.stringify(runtime, null, 2) + '\n');
  const paths = [
    'lan.html','lan.js','preload-cache.js','room-runtime.css',
    'replay/recorder.js','replay/tape.js','replay/profile.js','replay/sha256.js','replay/battle-proof.js',
    'app.js','site-adapter.css','fonts/th03-pause-pixel.woff2','fonts/OFL-Unifont.txt','network-performance.js','netplay.js','netplay/udp-config.js','netplay/input-policy.js','lockstep.js','frame-queue.js','controls.js',
    'netplay/input-transport.js','netplay/async-lockstep.js','netplay/frame-controls.js','netplay/simulation.js','netplay/simulation-worker.js','netplay/simulation-client.js',
    'native-pause.js','pause-search.js','native-patch.json','native/main.exe','native/start.com','native/music.com',
    'local-sfx.js','network-audio.js','network-audio.json','native/network-audio.com','sfx/manifest.json',
    ...fs.readdirSync(path.join(web,'sfx')).filter(name=>name.endsWith('.wav')).map(name=>'sfx/'+name),
    'native/game-jp.bat','native/game-cn.bat','native-music.js','local-bgm.js','bgm/manifest.json','bgm-version.js','bgm/akyu/manifest.json',
    'fire-control.js','player-ui.js','player-ui.css','touch-input.js','touch-layout.js','disk.js','scores.js','sha256.js',
    'netplay/native-hash.js','frame-limit.js','frame-presenter.js','startup-assets.js','dosbox-adapter.js','dosbox-disk.js',
    'dosbox-keys.js','dosbox-snapshots.js','dosbox-memory-diff.js','dosbox-audio-output.js','dosbox-boot.js',
    'local.html','local-worker-client.js','solo-worker-client.js','solo-worker.js','solo-worker-environment.js',
    'vendor/dosbox-x/runtime.json', ...Object.keys(runtime.files)
      .filter(name => !['wdosbox-x-fast.wasm','wdosbox-x-jspi.wasm','dosbox-jspi.mjs'].includes(name))
      .map(name => 'vendor/dosbox-x/' + name),
  ];
  const network = {
    protocol:'th03-rollback/7', tickHz:60, epoch:946684800000,
    adapter:'dosbox-x-full-asyncify-v1', dispatch:'dosbox-x-simple', cpu_cycles:16000,snapshotStride:2,
    native_sound:{bgm:0,se:0,schema:3,config_bgm_mode:1,mute:'network PCM output',se_part:15,
      effects:'confirmed-local-waveforms',bridge:'network-audio.json'}, files:{},
  };
  if(build)Object.assign(network,{adapter:'dosbox-x-th03-native-v1',snapshotScan:build.snapshotScan,
    nativeScheduling:build.releaseProfile.nativeScheduling,nativeCpu:build.releaseProfile.nativeCpu,
    nativePlanar:build.releaseProfile.nativePlanar,idleFusion:build.idleFusion});
  if(build?.inputPolicy)network.inputPolicy=build.inputPolicy;
  for (const name of paths.sort()) network.files[name] = sha(fs.readFileSync(path.join(web, name)));
  fs.writeFileSync(path.join(web, 'lockstep-runtime.json'), JSON.stringify(network, null, 2) + '\n');
  console.log(`Updated TH03 DOSBox-X manifests (${paths.length} network resources)`);
}
if (require.main === module) write();
module.exports = {write};
