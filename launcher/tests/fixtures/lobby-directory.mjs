import {PRODUCT_GAMES} from '../../../common/contracts/product-catalog.mjs';

/** Public visual data only. The manifest has no launchable Runtime or DATA. */
export function createLobbyDirectoryFixture(now = Date.now()) {
  const digest = 'a'.repeat(64);
  const manifest = {
    schema: 'eagler-touhou/site-manifest/1', protocol: 'eagler-touhou/1', profile: 'visual-directory',
    shared: {resourceMode: 'hosted', testBuild: true, vanillaFont: 'font.ttf', unicodeFont: 'font.otf', netplayRelay: 'wss://directory.visual.invalid/netplay'},
    games: Object.fromEntries(Object.entries(PRODUCT_GAMES).map(([id, product]) => [id, {
      runtime: `visual-fixture-runtime/${id}.html`,
      ...('multiplayerRuntime' in product ? {multiplayerRuntime: `visual-fixture-runtime/${id}-mp.html`} : {}),
      gameData: {path: product.package.dataTarget.slice(1), bytes: 1, sha256: digest, version: `sha256-${digest}`, layout: `sha256-${digest}`},
      music: {midi: {files: []}},
    }])),
  };
  const player = (name, patch = {}) => ({initial: name[0], name, online: true, ready: false, controlMode: 'normal', ...patch});
  const room = (product, code, patch = {}) => ({
    product, code, capacity: 3, players: 2, ready: 1, difficulty: 1, spectators: 0, phase: 'lobby', joinable: true,
    publicNames: true, seats: [player('演示玩家甲', {ready: true}), player('演示玩家乙', {controlMode: 'touch'}), null],
    disableCheatMovement: false, challengeMode: false, ...patch,
  });
  const rooms = [
    room('th06mp', '4106', {spectators: 2, disableCheatMovement: true, challengeMode: true}),
    room('th09mp', '4109', {
      capacity: 2, players: 2, ready: 2, phase: 'playing', joinable: false, spectatable: true, spectators: 8,
      seats: [player('完整公开昵称甲乙丙丁戊己', {ready: true, stats: {wins: 3, losses: 2, minutes: 8}}), player('P2', {ready: true, controlMode: 'touch', stats: {wins: 5, losses: 1, minutes: 8}})],
    }),
    room('th10mp', '4110', {
      players: 3, ready: 3, phase: 'playing', joinable: false, spectatable: true, spectators: 12, startedAt: now - 185000,
      seats: [player('演示玩家甲', {ready: true}), player('演示玩家乙', {ready: true, controlMode: 'touch'}), player('演示玩家丙', {ready: true, controlMode: 'cheat'})],
    }),
    room('th12mp', '4112', {players: 3, ready: 3, spectators: 3, seats: ['一号玩家', '二号玩家', '三号玩家'].map(name => player(name, {ready: true}))}),
    room('th15mp', '4115', {difficulty: 3, seats: [player('等待同伴重连'), player('已断开玩家', {online: false, controlMode: 'touch'}), null]}),
  ];
  return {manifest, rooms, total: 45};
}
