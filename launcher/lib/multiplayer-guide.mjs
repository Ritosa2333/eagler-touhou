export const MULTIPLAYER_GUIDE_PATH = '/docs/multiplayer';
export const COOP_GUIDE_GAMES = Object.freeze(/** @type {const} */ (['th06', 'th07', 'th08', 'th10', 'th11', 'th15']));
export const VERSUS_GUIDE_GAMES = Object.freeze(/** @type {const} */ (['th09', 'th19']));
export const MULTIPLAYER_GUIDE_TOPICS = Object.freeze(/** @type {const} */ ([
  'coop', ...COOP_GUIDE_GAMES, ...VERSUS_GUIDE_GAMES,
]));

/** @param {string | null | undefined} [game] */
export function multiplayerGuidePath(game) {
  const topic = MULTIPLAYER_GUIDE_TOPICS.find(value => value === game);
  return topic ? `${MULTIPLAYER_GUIDE_PATH}/${topic}` : MULTIPLAYER_GUIDE_PATH;
}

/** @param {string} pathname */
export function multiplayerGuideTopic(pathname) {
  const path = pathname.replace(/\/$/, '');
  if (path.endsWith(MULTIPLAYER_GUIDE_PATH) || path.endsWith('/multiplayer.html')) return 'start';
  return MULTIPLAYER_GUIDE_TOPICS.find(topic => path.endsWith(multiplayerGuidePath(topic)));
}
