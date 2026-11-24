let filterItems, safeAssetUrl, words, decodeReplay;

async function loadingDeadline(operation, resource, cancel) {
  let timer;
  try {
    return await Promise.race([operation, new Promise((resolve, reject) => {
      timer = setTimeout(() => { reject(new Error(`本地资源读取超时：${resource}`)); cancel?.(); }, 15000);
    })]);
  } finally { clearTimeout(timer); }
}

const find = selector => document.querySelector(selector);
const library = find('#library');
const detail = find('#detail');
const archiveSelect = find('#archive');
const state = { view: 'textures', variant: null, manifest: null, base: '', catalog: null, cache: new Map(),
  archive: 'title.anm', query: '', ticket: 0, variantTicket: 0, detailTicket: 0, audio: null };
const names = { textures: '纹理图集', sprites: '精灵切片', anm: '动画指令', ecl: '关卡脚本',
  music: '原版音乐', sounds: '音效', replays: '原版回放数据', status: '还原状态' };

function node(tag, text, className) {
  const result = document.createElement(tag);
  if (text !== undefined) result.textContent = text;
  if (className) result.className = className;
  return result;
}

function fail(error) {
  const box = find('#error');
  box.hidden = false;
  box.textContent = `读取失败：${error.message}\n请确认资源准备已完成，且通过本地服务器访问，而不是直接打开 HTML。`;
  if (!state.manifest) {
    const selector = find('#variant');
    selector.replaceChildren(node('option', '加载失败，请刷新重试'));
    selector.disabled = true;
    detail.replaceChildren(node('div', '资源加载失败，请查看上方诊断后刷新重试。', 'empty'));
  }
}

async function json(url) {
  const controller = new AbortController();
  return loadingDeadline((async () => {
    const response = await fetch(url, {signal: controller.signal});
    if (!response.ok) throw new Error(`${response.status} ${url}`);
    return await response.json();
  })(), url, () => controller.abort());
}

function asset(relative) {
  return safeAssetUrl(state.base, relative);
}

async function resource(relative) {
  const url = asset(relative);
  if (!state.cache.has(url)) state.cache.set(url, json(url));
  try {
    return await state.cache.get(url);
  } catch (error) {
    state.cache.delete(url);
    throw error;
  }
}

function stopAudio() {
  if (!state.audio) return;
  state.audio.pause();
  state.audio.removeAttribute('src');
  state.audio.load();
  state.audio = null;
}

function heading(title, subtitle) {
  detail.replaceChildren(node('h3', title), node('div', subtitle, 'detail-subtitle'));
}

function link(label, url) {
  const anchor = node('a', label);
  anchor.href = url;
  anchor.target = '_blank';
  anchor.rel = 'noopener';
  return anchor;
}

function choose(button, callback) {
  for (const selected of library.querySelectorAll('.selected')) selected.classList.remove('selected');
  button.classList.add('selected');
  const ticket = state.ticket;
  const current = () => ticket === state.ticket && button.isConnected && button.classList.contains('selected');
  Promise.resolve().then(() => { if (current()) return callback(); }).catch(error => { if (current()) fail(error); });
}

function row(primary, secondary, callback, options = {}) {
  const button = node('button', undefined, 'asset');
  if (options.image) {
    const image = node('img');
    image.src = options.image;
    image.alt = '';
    image.loading = 'lazy';
    button.append(image);
  } else {
    button.append(node('span', options.number ?? '◇', 'number'));
  }
  const label = node('span', undefined, 'asset-text');
  label.append(node('b', primary), node('small', secondary));
  button.append(label);
  button.addEventListener('click', () => choose(button, callback));
  library.append(button);
  return button;
}

async function imageAt(url) {
  const image = new Image();
  image.src = url;
  await loadingDeadline(image.decode(), url, () => image.removeAttribute('src'));
  return image;
}

async function showTexture(archive, entry, sprite) {
  const ticket = ++state.detailTicket;
  heading(sprite ? `精灵 #${sprite.index} · 原始 ID ${sprite.sourceId}` : entry.name,
    `Entry ${entry.index} · ANM v7 · 声明 ${entry.width}×${entry.height} · 偏移 ${entry.xOffset}, ${entry.yOffset}`);
  if (!entry.texture) {
    detail.append(node('div', '此条目没有嵌入纹理。它可能是动态绘制目标；不会伪造图片。', 'empty'));
    return;
  }
  const texture = entry.texture;
  const url = asset(texture.file);
  const image = await imageAt(url);
  if (ticket !== state.detailTicket) return;
  const canvas = node('canvas');
  canvas.setAttribute('aria-label', sprite ? '原版纹理精灵切片' : '原版纹理预览');
  const width = sprite ? Math.ceil(sprite.width) : texture.width;
  const height = sprite ? Math.ceil(sprite.height) : texture.height;
  if (width <= 0 || height <= 0 || width > 8192 || height > 8192) throw new Error('不能预览此精灵的几何尺寸');
  canvas.width = width;
  canvas.height = height;
  const context = canvas.getContext('2d');
  context.imageSmoothingEnabled = false;
  if (sprite) context.drawImage(image, sprite.x, sprite.y, sprite.width, sprite.height, 0, 0, width, height);
  else context.drawImage(image, 0, 0);
  const checker = node('div', undefined, 'checker');
  checker.append(canvas);
  detail.append(checker);
  const caption = node('div', undefined, 'caption');
  caption.append(node('span', `THTX ${texture.width}×${texture.height} · 格式 ${texture.format} · 真实贴图，未执行动画`),
    link('查看 PNG ↗', url));
  detail.append(caption);
  const source = node('div', `SHA-256 ${texture.sha256}`, 'detail-subtitle');
  source.style.marginTop = '14px';
  detail.append(source);
  if (sprite) detail.append(node('div', `源矩形 x=${sprite.x}, y=${sprite.y}, w=${sprite.width}, h=${sprite.height}`, 'code'));
}

function argumentsText(raw) {
  try {
    return words(raw).map(value => `${value.integer} / ${value.float === null ? 'NaN/Inf' : Number(value.float.toPrecision(6))}`).join(' ; ');
  } catch {
    return `原始数据：${raw}`;
  }
}

function showCommands(title, commands, subtitle, sourceLink) {
  ++state.detailTicket;
  heading(title, `${subtitle} · ${commands.length} 条指令 · 仅索引，不执行`);
  detail.append(node('p', '参数同时显示 int32 / float32 的位解释，未赋予操作码游戏语义；mask、分支及回调尚待逆向核对。', 'detail-subtitle'));
  const table = node('table', undefined, 'command-table');
  const header = node('tr');
  for (const title of ['偏移', '时刻', 'Opcode', 'Mask', '参数 int / float']) header.append(node('th', title));
  const tableHead = node('thead');
  tableHead.append(header);
  table.append(tableHead);
  const body = node('tbody');
  for (const command of commands.slice(0, 500)) {
    const tableRow = node('tr');
    for (const value of [`0x${command.offset.toString(16)}`, command.time, command.opcode,
      `0x${command.mask.toString(16)}`, argumentsText(command.raw)]) tableRow.append(node('td', value));
    body.append(tableRow);
  }
  table.append(body);
  const scroll = node('div', undefined, 'scroll-table');
  scroll.append(table);
  detail.append(scroll);
  const caption = node('div', undefined, 'caption');
  caption.append(node('span', commands.length > 500 ? '表格展示前 500 条；完整数据在 JSON 中。' : '指令顺序和原始参数均保留。'));
  if (sourceLink) caption.append(link('查看 thtk 反编译脚本 ↗', sourceLink));
  detail.append(caption);
}

function showAudio(item, music) {
  ++state.detailTicket;
  stopAudio();
  heading(item.name, music ? `PCM ${item.sampleRate} Hz · ${item.channels} 声道 · 从自有 thbgm.dat 提取`
    : `${item.sampleRate} Hz · ${item.channels} 声道 · ${item.duration.toFixed(2)} 秒`);
  const panel = node('div', undefined, 'audio-panel');
  panel.append(node('div', '♫', 'audio-note'));
  const audio = node('audio');
  audio.controls = true;
  audio.preload = 'metadata';
  audio.src = asset(item.file);
  audio.addEventListener('error', () => {
    if (state.audio === audio) fail(new Error(`音频加载/解码失败：${item.name}`));
  });
  panel.append(audio);
  state.audio = audio;
  if (music) {
    const label = node('label', undefined, 'audio-options');
    const checkbox = node('input');
    checkbox.type = 'checkbox';
    label.append(checkbox, node('span', '按原版描述表试听循环（非正式游戏音频调度）'));
    panel.append(label);
    audio.addEventListener('timeupdate', () => {
      if (checkbox.checked && audio.currentTime >= item.loopEnd - 0.06) audio.currentTime = item.loopStart;
    });
    audio.addEventListener('ended', () => {
      if (checkbox.checked) {
        audio.currentTime = item.loopStart;
        audio.play().catch(fail);
      }
    });
  }
  detail.append(panel);
  if (music) {
    detail.append(node('div', `loopStart ${item.loopStart.toFixed(6)} s\nloopEnd ${item.loopEnd.toFixed(6)} s\n原始字节偏移 0x${item.offset.toString(16)}\nPCM ${item.pcmBytes.toLocaleString()} bytes\nSHA-256 ${item.sha256}`, 'code'));
  }
}

function updateRuntimeOverview(progress, build) {
  const synchronized = progress.generation === build.generation;
  const version = `公版 ${progress.publicVersion}`;
  find('#status-version').textContent = synchronized ? `v${progress.publicVersion}` : '待同步';
  find('#runtime-phase').textContent = synchronized ? `${version} · 菜单还原中` : '运行核心版本待同步';
  find('#metric-runtime').textContent = synchronized ? version : '验收版本待同步';
  find('#metric-native').textContent = `${progress.nativeSources} / ${progress.productionTranslationUnits}`;
  find('#metric-apis').textContent = progress.productionAPIs;
  find('#metric-device').textContent = progress.browserDeviceAcceptance ? '已有设备验收' : '设备尚未验收';
  find('#runtime-change').textContent = `${version}：${progress.latestChange}`;
  find('#runtime-boundary').textContent = synchronized ? progress.evidenceScope
    : '验收快照与当前运行核心不一致，不能把快照当作当前核心已通过。';
  return synchronized;
}

async function synchronizeRuntimeOverview() {
  const [progress, build] = await Promise.all([json('/runtime/status.json'), json('/runtime/build.json')]);
  if (progress.schema !== 'th128-public-progress/1' || !Number.isSafeInteger(progress.publicVersion))
    throw new Error('当前运行状态快照格式不受支持');
  updateRuntimeOverview(progress, build);
}

async function showStatus(ticket) {
  const [progress, build] = await Promise.all([json('/runtime/status.json'), json('/runtime/build.json')]);
  if (ticket !== state.ticket) return;
  if (progress.schema !== 'th128-public-progress/1' || !Number.isSafeInteger(progress.publicVersion)
      || !Array.isArray(progress.milestones) || !Array.isArray(progress.verified)
      || !Array.isArray(progress.remaining) || !Array.isArray(progress.boundaries)) {
    throw new Error('当前还原状态快照格式不受支持');
  }
  const synchronized = updateRuntimeOverview(progress, build);
  const version = `公版 ${progress.publicVersion}`;
  heading('还原进度与证据边界', `${version} · 已公开运行核心的验收范围；此页只展示状态，不运行游戏。`);
  if (!synchronized) {
    detail.append(node('div', '验收快照与当前运行核心不一致：以下只展示快照对应的历史范围，不能当作当前核心已验收。', 'error'));
  }
  detail.append(node('div', `最近公开修复：${progress.latestChange}\n${progress.evidenceScope}\n状态同步：${new Date(progress.updatedAt).toLocaleString('zh-CN', {hour12: false})}`, 'code'));
  const list = node('ul', undefined, 'status-list');
  const markers = {complete: '✓', partial: '◐', pending: '○'};
  for (const milestone of progress.milestones) {
    if (!Object.hasOwn(markers, milestone.status)) throw new Error('未知还原里程碑状态');
    list.append(node('li', `${markers[milestone.status]}　${milestone.id}　${milestone.title}：${milestone.detail}`,
      milestone.status === 'complete' ? 'done' : 'pending'));
  }
  detail.append(list);
  detail.append(node('h3', '已验证的具体范围'));
  const verified = node('ul', undefined, 'status-list');
  for (const label of progress.verified) verified.append(node('li', label, 'done'));
  detail.append(verified);
  detail.append(node('h3', '尚未完成'));
  const remaining = node('ul', undefined, 'status-list');
  for (const label of progress.remaining) remaining.append(node('li', label, 'pending'));
  detail.append(remaining);
  detail.append(node('div', progress.boundaries.join('\n'), 'code'));
  const manifest = state.manifest;
  const source = Object.entries(manifest.sourceFiles).map(([name, value]) => `${name}\n${value.sha256}`).join('\n\n');
  detail.append(node('div', `运行核心\nGeneration ${build.generation}\n${progress.nativeSources}源文件 / ${progress.productionTranslationUnits}生产编译单元 / ${progress.productionAPIs} API\n\n原始输入身份\n\n${source}\n\n资源包原始分类记录（不代表运行时功能缺失判定）：${JSON.stringify(manifest.uninterpretedResourceTypes)}`, 'code'));
  library.append(node('div', `${version}\n正常资源与 ESC / GameOver 菜单还原中。\n整菜单与整产品尚未完成。`, 'empty'));
}

async function showReplay(item) {
  const ticket = ++state.detailTicket;
  heading(item.name, '从原版 DAT 内置 demo 提取；只解码输入与状态记录，不播放关卡。');
  const response = await fetch(asset(`runtime/${item.name}`));
  if (!response.ok) throw new Error(`回放读取失败：${item.name}`);
  const replay = decodeReplay(new Uint8Array(await response.arrayBuffer()));
  if (ticket !== state.detailTicket) return;
  const metadata = replay.metadata;
  detail.append(node('div', `名字 ${metadata.name ?? '(无法解码)'}\n路线 ${metadata.routeName} · 难度 ${metadata.rankName}\n关卡数 ${replay.stages.length} · 原始分数 ${metadata.score.toLocaleString()}\n原版 USER/未知数据均保留；原版播放兼容与游戏执行尚未验收。`, 'code'));
  const stageSelector = node('select', undefined, 'sub-select');
  stageSelector.setAttribute('aria-label', '回放关卡');
  for (const [index, stage] of replay.stages.entries()) {
    const option = node('option', `${stage.stageName} · ${stage.numFrames.toLocaleString()} 帧 · seed ${stage.seed}`);
    option.value = index;
    stageSelector.append(option);
  }
  const frameSlider = node('input');
  frameSlider.type = 'range';
  frameSlider.min = '0';
  frameSlider.value = '0';
  frameSlider.className = 'replay-slider';
  frameSlider.setAttribute('aria-label', '回放输入帧');
  const output = node('div', undefined, 'code');
  const explain = bits => {
    const keys = [[1,'射击/冰冻'],[2,'Perfect Freeze'],[8,'低速'],[16,'上'],[32,'下'],[64,'左'],[128,'右'],[512,'自动射击/对话快进']];
    return keys.filter(([mask]) => bits & mask).map(([, label]) => label).join(' + ') || '无';
  };
  const update = () => {
    const stage = replay.stages[Number(stageSelector.value)];
    frameSlider.max = Math.max(0, stage.numFrames - 1);
    const frame = Math.min(Number(frameSlider.value), stage.numFrames - 1);
    if (frame < 0) { output.textContent = '此关无输入帧。'; return; }
    const [holding, pressed, released] = stage.inputs.at(frame);
    output.textContent = `帧 ${frame.toLocaleString()} / ${(stage.numFrames - 1).toLocaleString()}\n按住 0x${holding.toString(16).padStart(4,'0')}  ${explain(holding)}\n按下 0x${pressed.toString(16).padStart(4,'0')}  ${explain(pressed)}\n释放 0x${released.toString(16).padStart(4,'0')}  ${explain(released)}\n\n起点 ${stage.position.join(', ')} · 干劲 ${stage.motivation}%\nPerfect Freeze ${stage.perfectFreeze}% · 冻结累计面积 ${stage.freezeArea}`;
  };
  stageSelector.addEventListener('change', () => { frameSlider.value = '0'; update(); });
  frameSlider.addEventListener('input', update);
  detail.append(stageSelector, frameSlider, output);
  update();
}

async function render() {
  if (!state.manifest) return;
  const ticket = ++state.ticket;
  try {
    await renderCurrent(ticket);
  } catch (error) {
    if (ticket === state.ticket) throw error;
  }
}

async function renderCurrent(ticket) {
  ++state.detailTicket;
  stopAudio();
  find('#error').hidden = true;
  find('#section-title').textContent = names[state.view];
  find('#current-view').textContent = names[state.view];
  for (const button of document.querySelectorAll('nav button')) button.classList.toggle('active', button.dataset.view === state.view);
  library.replaceChildren();
  detail.replaceChildren(node('div', '正在读取…', 'empty'));
  const archiveViews = ['textures', 'sprites', 'anm'];
  const files = state.view === 'ecl' ? state.manifest.ecl : state.manifest.animations;
  archiveSelect.hidden = !(archiveViews.includes(state.view) || state.view === 'ecl');
  if (!archiveSelect.hidden) {
    if (!files.some(file => file.name === state.archive)) state.archive = state.view === 'ecl' ? 'st_a1_1.ecl' : 'title.anm';
    if (!files.some(file => file.name === state.archive)) state.archive = files[0]?.name;
    archiveSelect.replaceChildren(...files.map(file => {
      const option = node('option', file.name);
      option.value = file.name;
      return option;
    }));
    archiveSelect.value = state.archive;
  }
  if (state.view === 'status') {
    await showStatus(ticket);
    return;
  }
  const query = state.query;
  if (state.view === 'replays') {
    const items = filterItems(state.manifest.files.filter(file => file.name.endsWith('.rpy')), query);
    for (const [index, item] of items.entries()) {
      const button = row(item.name, `${item.bytes.toLocaleString()} bytes · 原版内置 demo`, () => showReplay(item), {number: 'RPY'});
      if (index === 0) choose(button, () => showReplay(item));
    }
  } else if (state.view === 'music' || state.view === 'sounds') {
    const music = state.view === 'music';
    const items = filterItems(music ? state.manifest.music : state.manifest.sounds, query);
    for (const [index, item] of items.entries()) {
      const button = row(item.name, music ? `${item.loopEnd.toFixed(1)} s · 原版循环表` : `${item.duration.toFixed(2)} s · PCM`,
        () => showAudio(item, music), { number: String(index + 1).padStart(2, '0') });
      if (index === 0) choose(button, () => showAudio(item, music));
    }
  } else {
    const file = files.find(file => file.name === state.archive);
    if (!file) throw new Error('资源索引为空');
    const data = await resource(file.file);
    if (ticket !== state.ticket) return;
    if (state.view === 'textures') {
      const items = filterItems(data.entries, query);
      for (const entry of items) {
        const button = row(entry.name, `#${entry.index} · ${entry.width}×${entry.height}${entry.texture ? '' : ' · 无纹理'}`,
          () => showTexture(data, entry), { image: entry.texture ? asset(entry.texture.file) : null, number: String(entry.index) });
        if (entry === items.find(item => item.index === 1) || (items.length === 1)) choose(button, () => showTexture(data, entry));
      }
    } else if (state.view === 'sprites') {
      const items = filterItems(data.sprites.map(sprite => ({ ...sprite, name: `#${sprite.index} ID ${sprite.sourceId}` })), query);
      for (const [index, sprite] of items.entries()) {
        const button = row(sprite.name, `Entry ${sprite.entry} · ${sprite.width}×${sprite.height}`,
          () => showTexture(data, data.entries[sprite.entry], sprite), { number: String(sprite.index) });
        if (index === 0) choose(button, () => showTexture(data, data.entries[sprite.entry], sprite));
      }
    } else if (state.view === 'anm') {
      const items = filterItems(data.scripts.map(script => ({ ...script, name: `#${script.index} ID ${script.sourceId}` })), query);
      for (const [index, script] of items.entries()) {
        const button = row(script.name, `Entry ${script.entry} · ${script.commands.length} 条`,
          () => showCommands(script.name, script.commands, `ANM v7 · Entry ${script.entry} · 文件偏移 0x${script.offset.toString(16)}`),
          { number: String(script.index) });
        if (index === 0) choose(button, () => showCommands(script.name, script.commands, `ANM v7 · Entry ${script.entry}`));
      }
    } else if (state.view === 'ecl') {
      const items = filterItems(data.subroutines, query);
      for (const [index, subroutine] of items.entries()) {
        const button = row(subroutine.name, `${subroutine.commands.length} 条 · 0x${subroutine.offset.toString(16)}`,
          () => showCommands(subroutine.name, subroutine.commands, `${file.name} · SCPT/1`, asset(file.text)), { number: 'sub' });
        if (index === 0) choose(button, () => showCommands(subroutine.name, subroutine.commands, `${file.name} · SCPT/1`, asset(file.text)));
      }
    }
  }
  if (!library.children.length) {
    library.append(node('div', '没有匹配的资源', 'empty'));
    detail.replaceChildren(node('div', '请更换筛选条件。', 'empty'));
  } else if (!library.querySelector('.selected')) library.querySelector('button')?.click();
}

async function loadVariant(variant) {
  const ticket = ++state.variantTicket;
  ++state.ticket;
  ++state.detailTicket;
  stopAudio();
  const reference = state.catalog.variants[variant];
  let manifest, manifestUrl;
  try {
    manifestUrl = safeAssetUrl(new URL('/assets/', location.href).href, reference.manifest);
    manifest = await json(manifestUrl);
  } catch (error) {
    if (ticket !== state.variantTicket) return;
    if (state.variant) find('#variant').value = state.variant;
    throw error;
  }
  if (ticket !== state.variantTicket) return;
  if (manifest.game !== 'th128' || manifest.playable !== false || manifest.schema !== 'th128-prepared-assets/1') {
    throw new Error('不是受支持的 TH128 M0 资源包');
  }
  state.variant = variant;
  state.base = new URL('./', manifestUrl).href;
  state.manifest = manifest;
  state.cache.clear();
  const totals = manifest.totals;
  find('#metric-files').textContent = totals.files;
  find('#metric-images').textContent = `${totals.textures} / ${totals.sprites.toLocaleString()}`;
  find('#metric-instructions').textContent = totals.eclInstructions.toLocaleString();
  find('#metric-routes').textContent = `${totals.routeScripts} / ${totals.musicTracks}`;
  find('#replay-count').textContent = manifest.files.filter(file => file.name.endsWith('.rpy')).length;
  for (const count of document.querySelectorAll('[data-count]')) count.textContent = totals[count.dataset.count].toLocaleString();
  await render();
}

for (const button of document.querySelectorAll('nav button')) {
  button.addEventListener('click', () => {
    state.view = button.dataset.view;
    state.query = '';
    find('#search').value = '';
    render().catch(fail);
  });
}
archiveSelect.addEventListener('change', () => {
  state.archive = archiveSelect.value;
  render().catch(fail);
});
find('#search').addEventListener('input', event => {
  state.query = event.target.value;
  render().catch(fail);
});
find('#variant').addEventListener('change', event => loadVariant(event.target.value).catch(fail));
window.addEventListener('pagehide', stopAudio);

try {
  const [inspector, replay] = await loadingDeadline(Promise.all([import('./inspector.mjs'), import('./runtime/replay.mjs')]), '工作台页面依赖');
  ({filterItems, safeAssetUrl, words} = inspector);
  ({decodeReplay} = replay);
  if ([filterItems, safeAssetUrl, words, decodeReplay].some(value => typeof value !== 'function')) {
    throw new Error('工作台页面依赖接口不完整，请重启本地预览服务后刷新。');
  }
  state.catalog = await json('/assets/catalog.json');
  if (state.catalog.schema !== 'th128-asset-catalog/1') throw new Error('未知资源目录版本');
  const selector = find('#variant');
  selector.replaceChildren(...Object.entries(state.catalog.variants).map(([variant, record]) => {
    const option = node('option', record.title);
    option.value = variant;
    return option;
  }));
  selector.disabled = false;
  selector.value = state.catalog.variants.zh ? 'zh' : Object.keys(state.catalog.variants)[0];
  await loadVariant(selector.value);
  await synchronizeRuntimeOverview();
} catch (error) {
  fail(error);
}
