/** Vite publishes a fixed mode; source-only UI consumers are development previews. */
const setting = import.meta.env?.VITE_PREVIEW_FEATURES;
export const previewFeaturesEnabled = setting === 'true' || (setting !== 'false' && (import.meta.env?.DEV ?? true));
export const siteNoticeEnabled = import.meta.env?.VITE_SITE_NOTICE !== 'false';

export function isPreviewOnlyPage(pathname: string): boolean {
  return /\/(?:ui-review|login|profile)\.html\/?$/.test(pathname);
}
