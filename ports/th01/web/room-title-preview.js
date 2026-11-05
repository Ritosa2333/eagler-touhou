// Room decoration owns its own worker; launching a match destroys it first.
export function mountTitlePreview({game, lobby, waiting}) {
  if (typeof Worker !== 'function' ||
      !HTMLCanvasElement.prototype.transferControlToOffscreen) return;
  let worker, canvas, generation = 0, key = '', timeout, failed = '';
  const stop = () => {
    generation++; clearTimeout(timeout); worker?.terminate(); worker = null;
    canvas?.remove(); canvas = null; waiting.classList.remove('room-ui-title-ready');
    waiting.dataset.preview = 'idle'; key = '';
    delete waiting.dataset.previewFrame;delete waiting.dataset.previewInfo;delete waiting.dataset.previewError;delete waiting.dataset.previewLast;
  };
  const update = () => {
    const language = lobby.dataset.roomLanguage || 'cn';
    const next = language;
    const allowed = !lobby.hidden && lobby.dataset.roomPhase === 'lobby';
    if (!allowed) { if (key) stop(); failed=''; return; }
    if (key === next) return;
    if (key) stop();
    if (failed === next) return;
    failed = '';
    if (!document.getElementById('resource-status')?.textContent.includes('已校验并预载')) return;
    stop(); key = next; const epoch = generation;
    waiting.dataset.preview = 'loading';
    (async () => {
      const {assetBytes, freshJSON} = await import('./preload-cache.js');
      let disk, wasmBinary, module;
      if (game === '01') {
        const version = globalThis.th01RoomPreload?.version || await freshJSON('netplay/runtime.json');
        module = globalThis.th01RoomPreload?.module || await WebAssembly.compile(await assetBytes('game.wasm', {hash:version.files['game.wasm']}));
      } else {
        const suffix = language === 'jp' ? '-jp' : '';
        const manifestName = game === '02' ? 'solo-patch.json' : game === '03' ? 'disks/manifest.json' : game === '04' ? `solo-patch${suffix}.json` : 'solo-disks.json';
        const vendor = game === '05' ? 'vendor/dosbox-net/' : 'vendor/dosbox-x/';
        const [manifest, runtime] = await Promise.all([freshJSON(manifestName), freshJSON(game==='02'?'netplay/runtime.json':vendor+'runtime.json')]);
        const spec = game === '02' ? manifest.disks[language] : game === '03' ? manifest.games[`3-${language}`] : game === '04' ? manifest.disk : manifest.languages[language];
        const url = spec.url || spec.file || `th04-solo${suffix}.hdi.gz`;
        const hash = game==='02'?runtime.cores.plain.sha256:runtime.files['wdosbox-x-rollback.wasm'];
        if (!spec.sha256 || !hash) throw Error('标题资源缺少校验信息');
        [disk, wasmBinary] = await Promise.all([
          assetBytes(url, {hash:spec.sha256, size:spec.size, gzip:true}),
          assetBytes(vendor+'wdosbox-x-rollback.wasm', {hash})
        ]);
      }
      if (epoch !== generation) return;
      canvas = document.createElement('canvas'); canvas.width = 640; canvas.height = 400;
      canvas.className = 'room-ui-title-canvas'; canvas.setAttribute('aria-label', '原生游戏标题预览');
      canvas.setAttribute('role', 'img'); canvas.inert = true; waiting.prepend(canvas);
      worker = new Worker(new URL('./room-title-worker.js', import.meta.url), {type: 'module', name: `TH${game} room title`});
      const fail = message => { if (epoch !== generation) return; failed = next; stop(); waiting.dataset.preview = 'unavailable'; waiting.dataset.previewError=String(message); };
      worker.onerror = event => fail(event.message);
      worker.onmessage = ({data}) => {
        if (epoch !== generation) return;
        if (data.type === 'error') { console.warn('Room title preview:', data.message); fail(data.message); }
        if (data.type === 'ready') { clearTimeout(timeout); waiting.classList.add('room-ui-title-ready'); waiting.dataset.preview = 'ready'; waiting.dataset.previewInfo = JSON.stringify(data); }
        if (data.type === 'frame') { waiting.dataset.previewFrame=String(data.frames);waiting.dataset.previewLast=JSON.stringify(data); }
      };
      const offscreen = canvas.transferControlToOffscreen();
      worker.postMessage({type: 'boot', game, canvas: offscreen, disk, wasmBinary, module}, [offscreen, ...(disk?[disk.buffer,wasmBinary.buffer]:[])]);
      worker.postMessage({type:'pause',value:document.hidden});
      timeout = setTimeout(()=>fail('标题加载超时'), 90000);
    })().catch(error => { if (epoch === generation) { console.warn('Room title preview:', error); failed = next; stop(); waiting.dataset.preview = 'unavailable'; waiting.dataset.previewError=error.message; } });
  };
  const observer = new MutationObserver(update);
  observer.observe(lobby, {attributes: true, attributeFilter: ['hidden', 'data-room-phase', 'data-room-language']});
  const resources = document.getElementById('resource-status');
  if (resources) observer.observe(resources, {childList: true, characterData: true, subtree: true});
  document.addEventListener('visibilitychange', () => worker?.postMessage({type: 'pause', value: document.hidden}));
  window.addEventListener('pagehide', stop);
  lobby.addEventListener('room-preview-stop', stop);
  update();
}
