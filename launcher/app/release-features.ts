import {uiReviewEnabled as resolveUiReviewEnabled} from '../lib/frontend-static-manifest.mjs';

/** Resolve build-time feature gates with production-safe defaults. */
export type ReleaseFeatureEnvironment = {
  DEV?: boolean;
  VITE_PREVIEW_FEATURES?: string;
  VITE_UI_REVIEW_ENABLED?: string;
  VITE_ACCOUNTS_ENABLED?: string;
  VITE_LISTEN_TOGETHER_ENABLED?: string;
};

export function resolveReleaseFeatures(environment: ReleaseFeatureEnvironment) {
  const previewSetting = environment.VITE_PREVIEW_FEATURES;
  const previewFeaturesEnabled = previewSetting === 'true' || (previewSetting !== 'false' && (environment.DEV ?? true));
  const uiReviewEnabled = resolveUiReviewEnabled({
    VITE_UI_REVIEW_ENABLED: environment.VITE_UI_REVIEW_ENABLED,
    VITE_PREVIEW_FEATURES: previewSetting,
    NODE_ENV: (environment.DEV ?? true) ? 'development' : 'production',
  });
  const accountsEnabled = environment.VITE_ACCOUNTS_ENABLED === 'true';
  return {
    previewFeaturesEnabled,
    uiReviewEnabled,
    accountsEnabled,
    listenTogetherEnabled: environment.VITE_LISTEN_TOGETHER_ENABLED !== 'false',
    isPreviewOnlyPage(pathname: string) {
      const previewPage = /\/ui-review\.html\/?$/.test(pathname);
      const accountPage = /^(?:\/(?:login|profile)\.html|\/u\/[^/]+)\/?$/.test(pathname);
      return (previewPage && !uiReviewEnabled) || (accountPage && !accountsEnabled);
    },
  };
}

const releaseFeatures = resolveReleaseFeatures(import.meta.env ?? {});
export const previewFeaturesEnabled = releaseFeatures.previewFeaturesEnabled;
export const uiReviewEnabled = releaseFeatures.uiReviewEnabled;
export const accountsEnabled = releaseFeatures.accountsEnabled;
export const listenTogetherEnabled = releaseFeatures.listenTogetherEnabled;
export const isPreviewOnlyPage = releaseFeatures.isPreviewOnlyPage;
