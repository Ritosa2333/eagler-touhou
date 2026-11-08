export function withPlayerCueScript(html, source) {
  const marker = 'data-eagler-player-cue';
  if (html.includes(`<script ${marker}>`)) throw new Error('Multiplayer Runtime already contains the player cue');
  if (!/<head\b[^>]*>/i.test(html)) throw new Error('Multiplayer Runtime has no document head');
  if (typeof source !== 'string' || !source.trim() || /<\/script\b/i.test(source)) throw new Error('Invalid player cue script');
  return html.replace(/<head\b[^>]*>/i, head => `${head}<script ${marker}>\n${source}\n</script>`);
}
