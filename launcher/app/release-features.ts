/** Resolve build-time feature gates with production-safe defaults. */
export type ReleaseFeatureEnvironment = {
  DEV?: boolean;
  VITE_PREVIEW_FEATURES?: string;
  VITE_ACCOUNTS_ENABLED?: string;
  VITE_SITE_NOTICE?: string;
};

export function resolveReleaseFeatures(environment: ReleaseFeatureEnvironment) {
  const previewSetting = environment.VITE_PREVIEW_FEATURES;
  const previewFeaturesEnabled = previewSetting === 'true' || (previewSetting !== 'false' && (environment.DEV ?? true));
  const accountsEnabled = environment.VITE_ACCOUNTS_ENABLED === 'true';
  const siteNoticeEnabled = environment.VITE_SITE_NOTICE !== 'false';
  return {
    previewFeaturesEnabled,
    accountsEnabled,
    siteNoticeEnabled,
    isPreviewOnlyPage(pathname: string) {
      const previewPage = /\/ui-review\.html\/?$/.test(pathname);
      const accountPage = /^(?:\/(?:login|profile)\.html|\/u\/[^/]+)\/?$/.test(pathname);
      return (previewPage && !previewFeaturesEnabled) || (accountPage && !accountsEnabled);
    },
  };
}

const releaseFeatures = resolveReleaseFeatures(import.meta.env ?? {});
export const previewFeaturesEnabled = releaseFeatures.previewFeaturesEnabled;
export const accountsEnabled = releaseFeatures.accountsEnabled;
export const siteNoticeEnabled = releaseFeatures.siteNoticeEnabled;
export const isPreviewOnlyPage = releaseFeatures.isPreviewOnlyPage;
