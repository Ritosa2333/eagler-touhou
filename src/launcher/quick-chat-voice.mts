// Voice playback for quick-chat phrases.
//
// The clips are authored wav files shipped with the Launcher from
// assets/quick-chat/<id>.wav, one per phrase id in the quick-chat contract. The
// repository publishes only these reviewed clips; every other wav stays
// forbidden by .gitignore and scripts/audit-publication.mjs. Each clip is loaded
// once per session, and a missing clip stays silent rather than surfacing an
// error.
const clips = new Map<string, HTMLAudioElement>();

export function quickChatVoiceUrl(phraseId: string): string {
  return `assets/quick-chat/${encodeURIComponent(phraseId)}.wav`;
}

export function playQuickChatVoice(phraseId: string): void {
  let clip = clips.get(phraseId);
  if(!clip) {
    clip = new Audio(quickChatVoiceUrl(phraseId));
    clip.preload = "auto";
    clips.set(phraseId, clip);
  }
  try { clip.currentTime = 0; } catch {}
  void clip.play().catch(() => {});
}
