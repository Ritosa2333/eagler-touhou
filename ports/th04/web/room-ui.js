// Shared room chrome; controllers keep their original nodes and handlers.
import {mountTitlePreview} from './room-title-preview.js';
import {mountNetworkStatus} from './network-status.js';
const games = {
  '01': '东方灵异传', '02': '东方封魔录', '03': '东方梦时空',
  '04': '东方幻想乡', '05': '东方怪绮谈',
};
const game = document.body.dataset.roomGame;
if (games[game]) mountRoom(game);

function mountRoom(game) {
  const $ = id => document.getElementById(id);
  const lobby = $('lobby'), entry = $('entry');
  if (!lobby || !entry || $('room-workspace-header')) return;
  const make = (tag, className, text) => {
    const node = document.createElement(tag);
    if (className) node.className = className;
    if (text) node.textContent = text;
    return node;
  };
  const icon = name => {
    const img = make('img', 'room-ui-icon');
    img.src = new URL(`room-ui-assets/${name}.svg`, import.meta.url).href;
    img.alt = ''; return img;
  };
  const root = lobby.parentElement, original = [...lobby.children];
  const source = $(['01', '05'].includes(game) ? 'room-title' : 'room-info');
  const codeSource = game === '01' ? $('room-code') : source;
  const status = $(game === '03' ? 'lan-status' : 'status');
  const statusPlace = document.createComment('room status'); status?.before(statusPlace);
  const entryPlace = document.createComment('room entry'); entry.before(entryPlace);
  const start = $('room-start') || $('start'), ready = $('ready'), leave = $('leave');
  const actionRows = original.filter(child => child.contains(ready) || child.contains(start));
  const roster = game === '01' ? $('members') : lobby.querySelector('.seat-row, .seats');
  const seatsSource = $('seats');
  const stageContent = game === '01' ? $('player') : game === '05' ? document.querySelector('.game-column') : $('game');
  const localMusic = document.querySelector('.local-music-settings');
  const connections = [...root.querySelectorAll(':scope > details')];
  const personal = [$('control-settings'), $('configure-controls')?.closest('p'), $('network-runtime')?.closest('p')];
  const help = document.querySelector('.controls-help'), resourceStatus = $('resource-status');
  const startup = $('startup-progress'), history = $('history-enabled')?.closest('section');
  const header = make('header', 'room-ui-header'); header.id = 'room-workspace-header';
  const heading = make('div', 'room-ui-heading');
  heading.append(make('h1', '', games[game]), make('span', 'room-ui-mode'), make('span', 'room-ui-difficulty'));
  const headerActions = make('div', 'room-ui-header-actions');
  const copy = make('button', 'room-ui-copy');
  copy.type = 'button'; copy.title = '复制房间号'; copy.setAttribute('aria-label', '复制房间号');
  const code = make('strong'); copy.append(make('span', '', '房间号'), code, icon('room-copy'));
  const settingsToggle = make('button', 'room-ui-settings-toggle');
  settingsToggle.type = 'button'; settingsToggle.title = '本局设置';
  settingsToggle.setAttribute('aria-label', '打开本局设置'); settingsToggle.append(icon('room-gear-six'));
  headerActions.append(copy, settingsToggle);
  const headerReturn = make('button', 'room-ui-return-header', '返回房间');
  headerReturn.type='button';headerReturn.id='return-room-header';headerReturn.hidden=true;
  headerReturn.onclick=()=>$('return-room')?.click();headerActions.prepend(headerReturn);
  if (leave) {
    leave.classList.add('room-ui-back'); leave.setAttribute('aria-label', '离开房间'); leave.title = '离开房间';
    leave.prepend(icon('room-caret-left')); header.append(leave);
  }
  header.append(heading, headerActions);
  mountNetworkStatus(header);
  const stage = make('section', 'room-ui-stage'); stage.setAttribute('aria-label', `${games[game]}游戏画面`);
  const waiting = make('div', 'room-ui-waiting'), cover = make('img', 'room-ui-cover');
  cover.src = new URL('room-ui-assets/cover.jpg', import.meta.url).href; cover.alt = `${games[game]}封面`;
  const caption = make('div', 'room-ui-waiting-caption');
  caption.append(make('span', 'room-ui-eyebrow', `TOUHOU / ${game}`), make('h2', '', games[game]), make('p', '', '等待全员准备，由房主开始游戏'));
  waiting.append(cover, caption); stage.append(waiting);
  if (stageContent) { stageContent.classList.add('room-ui-native'); stage.append(stageContent); }
  const rosterHost = make('section', 'room-ui-roster'); rosterHost.setAttribute('aria-label', '玩家席位');
  if (roster) rosterHost.append(roster);
  const seatCards = [];
  if (seatsSource && roster) {
    for (const button of [...roster.querySelectorAll('button')]) {
      const card = make('article', 'room-ui-seat'), state = make('p', 'room-ui-seat-state');
      button.before(card); card.append(button, state); seatCards.push({button, card, state});
    }
    seatsSource.classList.add('room-ui-source-info');
  }
  const aside = make('aside', 'room-ui-sidebar'), tabs = make('div', 'room-ui-tabs'), panels = make('div', 'room-ui-panels');
  tabs.setAttribute('role', 'tablist'); tabs.setAttribute('aria-label', '房间选项');
  const panelMap = new Map(), tabMap = new Map();
  const labels = {game: '本局设置', watch: '观战', personal: '本机', network: '连接'};
  const selectTab = (name, focus = false) => {
    for (const [key, tab] of tabMap) {
      tab.setAttribute('aria-selected', String(key === name)); tab.tabIndex = key === name ? 0 : -1;
      panelMap.get(key).hidden = key !== name;
    }
    if (focus) tabMap.get(name).focus();
    settingsToggle.setAttribute('aria-expanded', String(name === 'game'));
  };
  Object.entries(labels).forEach(([name, label], index, values) => {
    const tab = make('button', '', label), panel = make('section', 'room-ui-tab-panel');
    tab.type = 'button'; tab.id = `room-ui-tab-${name}`; panel.id = `room-ui-panel-${name}`;
    tab.setAttribute('role', 'tab'); tab.setAttribute('aria-controls', panel.id);
    panel.setAttribute('role', 'tabpanel'); panel.setAttribute('aria-labelledby', tab.id);
    tab.onclick = () => selectTab(name);
    tab.onkeydown = event => {
      let next;
      if (event.key === 'ArrowRight') next = (index + 1) % values.length;
      if (event.key === 'ArrowLeft') next = (index + values.length - 1) % values.length;
      if (event.key === 'Home') next = 0;
      if (event.key === 'End') next = values.length - 1;
      if (next !== undefined) { event.preventDefault(); selectTab(values[next][0], true); }
    };
    tabMap.set(name, tab); panelMap.set(name, panel); tabs.append(tab); panels.append(panel);
  });
  settingsToggle.setAttribute('aria-controls', 'room-ui-panel-game');
  settingsToggle.onclick = () => { selectTab('game', true); aside.scrollIntoView({block: 'nearest'}); };
  aside.append(tabs, panels);
  const footer = make('footer', 'room-ui-actions');
  if (ready) { ready.classList.add('room-ui-ready'); footer.append(ready); }
  if (start) { start.classList.add('room-ui-start'); footer.append(start); }
  const gamePanel = panelMap.get('game'), watchPanel = panelMap.get('watch'), personalPanel = panelMap.get('personal'), networkPanel = panelMap.get('network');
  const relocations = [];
  const relocateInRoom = (node, destination) => {
    if (!node) return;
    const anchor = document.createComment('room option'); node.before(anchor);
    relocations.push({node, destination, anchor});
  };
  gamePanel.append(make('h2', '', '本局设置'));
  for (const child of original) {
    if (child === roster) continue;
    if (actionRows.includes(child) || child.classList.contains('room-heading')) { child.hidden = true; continue; }
    gamePanel.append(child);
  }
  if (game === '01') {
    const tools = make('div', 'room-ui-room-tools');
    for (const id of ['swap', 'settings']) if ($(id)) tools.append($(id));
    gamePanel.prepend(tools); $('settings')?.addEventListener('click', () => selectTab('game'));
  }
  if (source) { source.classList.add('room-ui-source-info'); gamePanel.prepend(source); }
  if (resourceStatus) { resourceStatus.classList.add('room-ui-resource'); gamePanel.append(resourceStatus); }
  if (startup) networkPanel.append(startup);
  mountNetworkStatus(networkPanel,{details:true});
  for (const node of connections) relocateInRoom(node, networkPanel);
  personalPanel.append(make('h2', '', '本机设置'));
  relocateInRoom(localMusic, personalPanel);
  for (const node of personal) relocateInRoom(node, personalPanel);
  relocateInRoom(help, personalPanel);
  const watchLinks = make('div', 'room-ui-watch-links');
  const watchEmpty = make('p', 'room-ui-muted', '游戏开始后，可在这里打开或分享观战链接。');
  watchPanel.append(make('h2', '', '观战'), watchEmpty, watchLinks);
  if (history) relocateInRoom(history, watchPanel);
  else watchPanel.append(make('p', 'room-ui-muted', '同浏览器链接用于本机观看；跨设备观战链接由房主分享。'));
  const statusHost = make('div', 'room-ui-notice'); statusHost.setAttribute('aria-label', '房间状态');
  lobby.append(header, stage, rosterHost, aside, footer, statusHost);
  lobby.classList.add('room-ui-workspace'); document.body.classList.add('unified-room');
  for (const child of [...root.children]) {
    if (child !== lobby && (child !== entry || game !== '01') && !child.matches('script, link, dialog')) child.classList.add('room-ui-outside');
  }
  if (root !== document.body) for (const child of document.body.children) {
    if (child !== root && !child.matches('script, link, dialog')) child.classList.add('room-ui-outside');
  }
  selectTab('game');
  let codeValue = '', copyTimer, nativeOpened = false;
  const playerNode = () => {
    const selector = '.th02-player, .th03-player, .th04-player';
    return stageContent?.matches(selector) ? stageContent : stageContent?.querySelector(selector);
  };
  const copyFeedback = make('span', 'room-ui-copy-feedback'); copyFeedback.setAttribute('role', 'status'); headerActions.append(copyFeedback);
  copy.onclick = async () => {
    clearTimeout(copyTimer);
    try {
      if (!codeValue) return;
      if (navigator.clipboard?.writeText) await navigator.clipboard.writeText(codeValue);
      else {
        const input = make('textarea', 'room-ui-clipboard'); input.value = codeValue;
        document.body.append(input); input.select(); const ok = document.execCommand('copy'); input.remove();
        if (!ok) throw Error('copy');
      }
      copyFeedback.textContent = '房间号已复制';
    } catch { copyFeedback.textContent = `房间号：${codeValue}，请手动复制`; }
    copyTimer = setTimeout(() => { copyFeedback.textContent = ''; }, 4000);
  };
  const update = () => {
    const returnButton = $('return-room');
    if (returnButton && ['01', '03'].includes(game) && returnButton.parentNode !== headerActions) headerActions.prepend(returnButton);
    const active = !lobby.hidden; document.body.classList.toggle('room-ui-in-room', active);
    for (const {node, destination, anchor} of relocations) {
      if (active && node.parentNode !== destination) destination.append(node);
      if (!active && node.parentNode === destination) anchor.after(node);
      node.classList.toggle('room-ui-outside', !active);
    }
    if (status) {
      const destination = active ? statusHost : statusPlace.parentNode;
      if (status.parentNode !== destination) { if (active) statusHost.append(status); else statusPlace.after(status); }
      status.classList.toggle('room-ui-outside', !active);
    }
    if (game === '01') {
      if (active && entry.parentElement !== gamePanel) gamePanel.append(entry);
      if (!active && entry.parentElement === gamePanel) entryPlace.after(entry);
    }
    const text = `${codeSource?.textContent || ''} ${$('room-settings')?.textContent || ''}`;
    codeValue = game === '01' ? codeSource.textContent.trim() : text.match(/房间\s*(\d{4})/)?.[1] || '';
    if (code.textContent !== codeValue) code.textContent = codeValue; copy.disabled = !codeValue;
    const players = text.match(/([23])\s*人/)?.[1] || $('players')?.value || '2';
    const mode = game === '03' ? '双人对战' : `${players} 人合作`;
    const difficultySelect = gamePanel.querySelector('#rank, #difficulty, #room-difficulty');
    const difficulty = text.match(/\b(Easy|Normal|Hard|Lunatic|Extra)\b/)?.[1] || difficultySelect?.selectedOptions[0]?.textContent || '';
    const modeLabel = heading.querySelector('.room-ui-mode'), difficultyLabel = heading.querySelector('.room-ui-difficulty');
    if (modeLabel.textContent !== mode) modeLabel.textContent = mode;
    if (difficultyLabel.textContent !== difficulty) difficultyLabel.textContent = difficulty;
    ready?.setAttribute('aria-pressed', String(ready.textContent.includes('取消')));
    const nativePlayer = playerNode();
    const started = game === '01' ? $('cover')?.hidden === true : game === '03' ? !stageContent.hidden : game === '05' ? !$('pause').disabled : nativePlayer?.classList.contains('immersive');
    // Returning from fullscreen must keep the actual game visible in its room.
    nativeOpened = active && lobby.dataset.roomPhase !== 'lobby' && (nativeOpened || !!started);
    headerReturn.hidden=!active||!returnButton||returnButton.hidden||['01','03'].includes(game)||nativeOpened;
    headerReturn.disabled=!!returnButton?.disabled;
    if(leave){leave.setAttribute('aria-label',leave.textContent.trim());leave.title=leave.textContent.trim();}
    stage.classList.toggle('room-ui-started', nativeOpened);
    if (seatsSource) {
      const values = seatsSource.textContent.split(' / ');
      for (const [index, {button, card, state}] of seatCards.entries()) {
        card.hidden = button.hidden;
        if (state.textContent !== (values[index] || '')) state.textContent = values[index] || '';
        card.dataset.local = button.getAttribute('aria-pressed') || 'false';
        card.dataset.ready = String((values[index] || '').includes('已准备'));
      }
    }
    if (game === '01') for (const seat of roster.children) {
      seat.dataset.local = String(seat.textContent.includes('你')); seat.dataset.ready = String(seat.textContent.includes('已准备'));
    }
  };
  const observer = new MutationObserver(update);
  observer.observe(lobby, {attributes: true, attributeFilter: ['hidden']});
  for (const node of [source, game === '01' ? $('room-settings') : null, codeSource === source ? null : codeSource, seatsSource, game === '01' ? roster : null]) {
    if (node) observer.observe(node, {childList: true, subtree: true, characterData: true});
  }
  for (const node of [ready, $('cover'), $('pause'), stageContent]) {
    if (node) observer.observe(node, {attributes: true, attributeFilter: ['hidden', 'disabled', 'class'], childList: node === ready});
  }
  const findHistory = () => {
    for (const a of [...stage.querySelectorAll('a')].filter(a => ['同浏览器观战', '跨设备观战链接'].includes(a.textContent.trim()))) {
      const info = a.nextElementSibling; watchLinks.append(a);
      if (info?.tagName === 'SPAN') watchLinks.append(info);
    }
    watchEmpty.hidden = !!watchLinks.querySelector('a:not([hidden])') || !!history;
  };
  const historyObserver = new MutationObserver(records => {
    if (records.some(r => [...r.addedNodes].some(n => n.nodeType === 1 && (n.matches('a, .toolbar, .player-toolbar') || n.querySelector('a'))))) findHistory();
  });
  historyObserver.observe(stage, {childList: true, subtree: true});
  new MutationObserver(() => { watchEmpty.hidden = !!watchLinks.querySelector('a:not([hidden])') || !!history; }).observe(watchLinks, {attributes: true, subtree: true, attributeFilter: ['hidden']});
  const launchObserver = new MutationObserver(() => {
    const player = playerNode();
    if (player) { observer.observe(player, {attributes: true, attributeFilter: ['class']}); launchObserver.disconnect(); update(); }
  });
  if (stageContent) launchObserver.observe(stageContent, {childList: true, subtree: true});
  const existingPlayer = playerNode();
  if (existingPlayer) { observer.observe(existingPlayer, {attributes: true, attributeFilter: ['class']}); launchObserver.disconnect(); }
  findHistory(); update(); mountTitlePreview({game, lobby, waiting});
}
