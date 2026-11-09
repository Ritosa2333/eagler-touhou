/** Resolve build-time feature gates with production-safe defaults. */
export type ReleaseFeatureEnvironment = {
  DEV?: boolean;
  VITE_PREVIEW_FEATURES?: string;
  VITE_ACCOUNTS_ENABLED?: string;
  VITE_LISTEN_TOGETHER_ENABLED?: string;
};

export function resolveReleaseFeatures(environment: ReleaseFeatureEnvironment) {
  const previewSetting = environment.VITE_PREVIEW_FEATURES;
  const previewFeaturesEnabled = previewSetting === 'true' || (previewSetting !== 'false' && (environment.DEV ?? true));
  const accountsEnabled = environment.VITE_ACCOUNTS_ENABLED === 'true';
  return {
    previewFeaturesEnabled,
    accountsEnabled,
    listenTogetherEnabled: environment.VITE_LISTEN_TOGETHER_ENABLED === 'true',
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
export const listenTogetherEnabled = releaseFeatures.listenTogetherEnabled;
export const isPreviewOnlyPage = releaseFeatures.isPreviewOnlyPage;
