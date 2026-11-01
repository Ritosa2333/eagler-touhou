// Shared main policy only; each presentation keeps its own lifecycle owner.
export {
  parseSiteNoticeText,
  SITE_NOTICE_DISMISSED_KEY,
  SITE_NOTICE_DURATION_MS,
  SITE_NOTICE_STORAGE_KEY,
  type SiteNoticeLine,
  type SiteNoticeSegment,
  siteNoticeBrandAsset,
} from '../../../src/launcher/site-notice-content.mts';
